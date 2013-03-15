/* 
Copyright (c) 2013, The Linux Foundation. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:
* Redistributions of source code must retain the above copyright
notice, this list of conditions and the following disclaimer. 
 
* Redistributions in binary form must reproduce the above
copyright notice, this list of conditions and the following
disclaimer in the documentation and/or other materials provided
with the distribution. 
 
* Neither the name of The Linux Foundation nor the names of its
contributors may be used to endorse or promote products derived
from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*!
	@file
	IPACM_Lan.cpp

	@brief
	This file implements the LAN iface functionality.

	@Author
	Skylar Chang

*/
#include <string.h>
#include <sys/ioctl.h>
#include <IPACM_Netlink.h>
#include <IPACM_Lan.h>
#include <IPACM_Wan.h>
#include <IPACM_IfaceManager.h>

IPACM_Lan::IPACM_Lan(int iface_index) : IPACM_Iface(iface_index)
{
	num_uni_rt = 0;
	num_dft_rt = 0;

	rt_rule_len = sizeof(struct ipa_lan_rt_rule) + (iface_query->num_tx_props * sizeof(uint32_t));
	route_rule = (struct ipa_lan_rt_rule *)calloc(IPA_MAX_NUM_UNICAST_ROUTE_RULES, rt_rule_len);
	if (route_rule == NULL)
	{
		IPACMERR("unable to allocate memory\n");
		return;
	}

	IPACMDBG(" IPACM->IPACM_Lan(%d) constructor: Tx:%d Rx:%d\n", ipa_if_num,
					 iface_query->num_tx_props, iface_query->num_rx_props);

	return;
}

IPACM_Lan::~IPACM_Lan()
{
	IPACM_EvtDispatcher::deregistr(this);
	IPACM_IfaceManager::deregistr(this);
	return;
}


/* LAN-iface's callback function */
void IPACM_Lan::event_callback(ipa_cm_event_id event, void *param)
{
	int ipa_interface_index;

	switch (event)
	{
	case IPA_LINK_DOWN_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_LINK_DOWN_EVENT\n");
				handle_down_evt();
				IPACMDBG("ipa_LAN (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
				delete this;
				return;
			}
		}
		break;

	case IPA_ADDR_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			if ( (data->iptype == IPA_IP_v4 && data->ipv4_addr == 0) ||
					 (data->iptype == IPA_IP_v6 && 
						data->ipv6_addr[0] == 0 && data->ipv6_addr[1] == 0 && 
					  data->ipv6_addr[2] == 0 && data->ipv6_addr[3] == 0) )
			{
				IPACMDBG("Invalid address, ignore IPA_ADDR_ADD_EVENT event\n");
				return;
			}


			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_ADDR_ADD_EVENT\n");

				/* check v4 not setup before, v6 can have 2 iface ip */
				if( ((data->iptype != ip_type) && (ip_type != IPA_IP_MAX)) 
				    || ((data->iptype==IPA_IP_v6) && (num_dft_rt!=MAX_DEFAULT_v6_ROUTE_RULES))) 
				{
				  IPACMDBG("Got IPA_ADDR_ADD_EVENT ip-family:%d, v6 num %d: \n",data->iptype,num_dft_rt);
					handle_addr_evt(data);
					handle_private_subnet(data->iptype);
				  
					if (IPACM_Wan::isWanUP() && (data->iptype == IPA_IP_v4))
					{
						handle_wan_up();
					}

					/* Post event to NAT */
					if (data->iptype == IPA_IP_v4)
					{
						ipacm_cmd_q_data evt_data;
						ipacm_event_iface_up *info;

						info = (ipacm_event_iface_up *)
							 malloc(sizeof(ipacm_event_iface_up));
						if (info == NULL)
						{
							IPACMERR("Unable to allocate memory\n");
							return;
						}

						memcpy(info->ifname, dev_name, IF_NAME_LEN);
						info->ipv4_addr = data->ipv4_addr;
						info->addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[0].subnet_mask;

						evt_data.event = IPA_HANDLE_LAN_UP;
						evt_data.evt_data = (void *)info;

						/* Insert IPA_HANDLE_LAN_UP to command queue */
						IPACMDBG("posting IPA_HANDLE_LAN_UP for IPv4 with below information\n");
						IPACMDBG("IPv4 address:0x%x, IPv4 address mask:0x%x\n",
										 info->ipv4_addr, info->addr_mask);
						IPACM_EvtDispatcher::PostEvt(&evt_data);
					}
				}
			}
		}
		break;

	case IPA_HANDLE_WAN_UP:
		IPACMDBG("Received IPA_HANDLE_WAN_UP event\n");
		handle_wan_up();
		break;

	case IPA_ROUTE_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			/* unicast routing rule add */
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_ROUTE_ADD_EVENT\n");
				handle_route_add_evt(data);
			}
		}
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		handle_wan_down();
		break;

	case IPA_ROUTE_DEL_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			/* unicast routing rule del */
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_ROUTE_DEL_EVENT\n");
				handle_route_del_evt(data);
			}
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT\n");
				ipacm_event_data_addr *data_addr;
				data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
				if (data_addr == NULL)
				{
					IPACMERR("unable to allocate memory for event data_addr\n");
					return;
				}

				data_addr->if_index = data->if_index;
				data_addr->iptype = data->iptype;
				if (data->iptype == IPA_IP_v4)
				{
					data_addr->ipv4_addr = data->ipv4_addr;
					data_addr->ipv4_addr_mask = 0xFFFFFFFF;
				}
				else
				{
					memcpy(data_addr->ipv6_addr,
								 data->ipv6_addr,
								 sizeof(data_addr->ipv6_addr));
					data_addr->ipv6_addr_mask[0] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[1] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[2] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[3] = 0xFFFFFFFF;
				}
				handle_route_add_evt(data_addr);
			}
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT\n");
				ipacm_event_data_addr *data_addr;
				data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
				if (data_addr == NULL)
				{
					IPACMERR("unable to allocate memory for event data_addr\n");
					return;
				}

				data_addr->if_index = data->if_index;
				data_addr->iptype = data->iptype;
				if (data->iptype == IPA_IP_v4)
				{
					data_addr->ipv4_addr = data->ipv4_addr;
					data_addr->ipv4_addr_mask = 0xFFFFFFFF;
				}
				else
				{
					memcpy(data_addr->ipv6_addr,
								 data->ipv6_addr,
								 sizeof(data_addr->ipv6_addr));
					data_addr->ipv6_addr_mask[0] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[1] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[2] = 0xFFFFFFFF;
					data_addr->ipv6_addr_mask[3] = 0xFFFFFFFF;
				}
				handle_route_del_evt(data_addr);
			}
		}
		break;

	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG("Received IPA_SW_ROUTING_ENABLE\n");
		/* handle software routing enable event*/
		handle_software_routing_enable();
		break;

	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG("Received IPA_SW_ROUTING_DISABLE\n");
		/* handle software routing disable event*/
		handle_software_routing_disable();
		break;

	default:
		break;
	}

	return;
}

/* handle unicast routing rule add event */
int IPACM_Lan::handle_route_add_evt(ipacm_event_data_addr *data)
{
	/* add unicate route for LAN */
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr sRetHeader;
	uint32_t tx_index;

	IPACMDBG("LAN callback: unicast IPA_ROUTE_ADD_EVENT\n");

	if (num_uni_rt < IPA_MAX_NUM_UNICAST_ROUTE_RULES)
	{
		/* unicast RT rule add start */
		rt_rule = (struct ipa_ioc_add_rt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
							1 * sizeof(struct ipa_rt_rule_add));
		if (!rt_rule)
		{
			IPACMERR("fail\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = (uint8_t)1;
		rt_rule->ip = data->iptype;

		if (data->iptype == IPA_IP_v4) 
		{  
		   strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name);
		}
		else 
		{
		   strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name);
     }

		rt_rule_entry = &rt_rule->rules[0];
		rt_rule_entry->at_rear = 1;

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
		
		    if(data->iptype != tx_prop->tx[tx_index].ip)
		    {
		    	IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule added\n", 
		    					    tx_index, tx_prop->tx[tx_index].ip,data->iptype);		
		    	continue;
		    }		
		
			if (tx_prop->tx[tx_index].hdr_name)
			{
				memset(&sRetHeader, 0, sizeof(sRetHeader));
				strncpy(sRetHeader.name,
								tx_prop->tx[tx_index].hdr_name,
								sizeof(tx_prop->tx[tx_index].hdr_name));

				if (false == m_header.GetHeaderHandle(&sRetHeader))
				{
					IPACMERR(" ioctl failed\n");
				        free(rt_rule);
				        return IPACM_FAILURE;
				}

				rt_rule_entry->rule.hdr_hdl = sRetHeader.hdl;
			}
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			if (data->iptype == IPA_IP_v4)
			{
				rt_rule_entry->rule.attrib.u.v4.dst_addr      = data->ipv4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = data->ipv4_addr_mask;
			}
			else
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = data->ipv6_addr_mask[0];
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = data->ipv6_addr_mask[1];
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = data->ipv6_addr_mask[2];
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = data->ipv6_addr_mask[3];
			}

			if (false == m_routing.AddRoutingRule(rt_rule))
			{
				IPACMERR("Routing rule addition failed!\n");
				free(rt_rule);
				return IPACM_FAILURE;
			}

			IPACMDBG("rt rule hdl1=0x%x\n", rt_rule_entry->rt_rule_hdl);
			get_rt_ruleptr(route_rule, num_uni_rt)->rt_rule_hdl[tx_index]
				 = rt_rule_entry->rt_rule_hdl;
		}
		memcpy(&get_rt_ruleptr(route_rule, num_uni_rt)->rule,
					 &rt_rule_entry->rule.attrib,
					 sizeof(get_rt_ruleptr(route_rule, num_uni_rt)->rule));

		get_rt_ruleptr(route_rule, num_uni_rt)->ip = data->iptype;
		free(rt_rule);
		num_uni_rt++;
	}
	else
	{
		IPACMDBG("unicast rule oversize\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/* handle unicast routing rule del event */
int IPACM_Lan::handle_route_del_evt(ipacm_event_data_addr *data)
{
	int i;
	uint32_t tx_index;

	/* delete 1 unicast RT rule */
	for (i = 0; i <= num_uni_rt; i++)
	{

		if (data->iptype == IPA_IP_v4)
		{
			if ((data->ipv4_addr == get_rt_ruleptr(route_rule, i)->rule.u.v4.dst_addr) &&
					(data->ipv4_addr_mask == get_rt_ruleptr(route_rule, i)->rule.u.v4.dst_addr_mask))
			{
				for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
				{
		            if(data->iptype != tx_prop->tx[tx_index].ip)
		            {
		            	IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule added\n", 
		            					    tx_index, tx_prop->tx[tx_index].ip,data->iptype);		
		            	continue;
		            }		

					if (m_routing.DeleteRoutingHdl(get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index],
																				 IPA_IP_v4) == false)
					{
						IPACMERR("Routing rule deletion failed!\n");
						return IPACM_FAILURE;
					}
				}

				/* remove that delted route rule entry*/
				for (; i <= num_uni_rt; i++)
				{
					get_rt_ruleptr(route_rule, i)->rule = get_rt_ruleptr(route_rule, (i + 1))->rule;
					get_rt_ruleptr(route_rule, i)->ip = get_rt_ruleptr(route_rule, (i + 1))->ip;

					for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
					{
						get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index] = get_rt_ruleptr(route_rule, (i + 1))->rt_rule_hdl[tx_index];
					}
				}

				num_uni_rt -= 1;
				return IPACM_SUCCESS;
			}
		}
		else
		{
			if ((data->ipv6_addr[0] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr[0]) &&
					(data->ipv6_addr[1] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr[1]) &&
					(data->ipv6_addr[2] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr[2]) &&
					(data->ipv6_addr[3] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr[3]) &&
					(data->ipv6_addr_mask[0] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr_mask[0]) &&
					(data->ipv6_addr_mask[1] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr_mask[1]) &&
					(data->ipv6_addr_mask[2] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr_mask[2]) &&
					(data->ipv6_addr_mask[3] == get_rt_ruleptr(route_rule, i)->rule.u.v6.dst_addr_mask[3]))
			{

				for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
				{
		                        if(data->iptype != tx_prop->tx[tx_index].ip)
		                        {
		            	               IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule added\n", 
		            					    tx_index, tx_prop->tx[tx_index].ip,data->iptype);		
		            	               continue;
		                        }		

					if (m_routing.DeleteRoutingHdl(get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index],
																				 IPA_IP_v6) == false)
					{
						IPACMERR("Routing rule deletion failed!\n");
						return IPACM_FAILURE;
					}
				}

				/* remove that delted route rule entry*/
				for (; i <= num_uni_rt; i++)
				{
					for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
					{
						get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index] = get_rt_ruleptr(route_rule, (i + 1))->rt_rule_hdl[tx_index];
					}
					get_rt_ruleptr(route_rule, i)->rule = get_rt_ruleptr(route_rule, (i + 1))->rule;
					get_rt_ruleptr(route_rule, i)->ip = get_rt_ruleptr(route_rule, (i + 1))->ip;
				}

				num_uni_rt -= 1;
				return IPACM_SUCCESS;
			}
		}
	}

	return IPACM_FAILURE;
}




/* configure filter rule for wan_up event*/
int IPACM_Lan::handle_wan_up(void)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int len = 0;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("set WAN interface as default filter rule\n");

	len = sizeof(struct ipa_ioc_add_flt_rule) + (1 * sizeof(struct ipa_flt_rule_add));
	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
	if (m_pFilteringTable == NULL)
	{
		PERROR("Error Locate ipa_flt_rule_add memory...\n");
		return IPACM_FAILURE;
	}

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v4;
	m_pFilteringTable->num_rules = (uint8_t)1;

	IPACMDBG("Retrieving routing hanle for table: %s\n",
					 IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name);
	if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4))
	{
		IPACMERR("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4=0x%p) Failed.\n",
						 &IPACM_Iface::ipacmcfg->rt_tbl_wan_v4);
		return IPACM_FAILURE;
	}
	IPACMDBG("Routing hanle for table: %d\n", IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl);


	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add)); // Zero All Fields
	flt_rule_entry.at_rear = true;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT; //IPA_PASS_TO_ROUTING
	flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl;

	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(flt_rule_entry.rule.attrib));

	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x0;
	flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x0;

	memcpy(&m_pFilteringTable->rules[0], &flt_rule_entry, sizeof(flt_rule_entry));
	if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	else
	{
		IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n",
						 m_pFilteringTable->rules[0].flt_rule_hdl,
						 m_pFilteringTable->rules[0].status);
	}


	/* copy filter hdls  */
	lan_wan_fl_rule_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
	free(m_pFilteringTable);

	return IPACM_SUCCESS;
}


/* delete filter rule for wan_down event*/
int IPACM_Lan::handle_wan_down(void)
{

	if (m_filtering.DeleteFilteringHdls(&lan_wan_fl_rule_hdl[0],
																			IPA_IP_v4, 1) == false)
	{
		IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/* handle new_address event*/
int IPACM_Lan::handle_addr_evt(ipacm_event_data_addr *data)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_flt_rule_add flt_rule_entry;
	const int NUM_RULES = 1;
	int res = IPACM_SUCCESS;

	/* construct ipa_ioc_add_flt_rule with v6 rules */
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("set route/filter rule ip-type: %d \n", data->iptype);

	rt_rule = (struct ipa_ioc_add_rt_rule *)
		 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
						NUM_RULES * sizeof(struct ipa_rt_rule_add));

	if (rt_rule == NULL)
	{
		IPACMERR("fail to allocate ipa_ioc_add_rt_rule \n");
		return IPACM_FAILURE;
	}

	rt_rule->commit = 1;
	rt_rule->num_rules = NUM_RULES;
	rt_rule->ip = data->iptype;

	if (data->iptype == IPA_IP_v4) strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name);
	else strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name);

	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = 1;
	rt_rule_entry->rule.dst = IPA_CLIENT_A5_LAN_WAN_CONS;  //go to A5
	rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;

	if (data->iptype == IPA_IP_v4)
	{
		rt_rule_entry->rule.attrib.u.v4.dst_addr      = data->ipv4_addr;
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
	}
	else
	{
		rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
	}

	if (false == m_routing.AddRoutingRule(rt_rule))
	{
		IPACMERR("Routing rule addition failed!\n");
		res = IPACM_FAILURE;
		goto fail;
	}
	else if (rt_rule_entry->status)
	{
		IPACMERR("rt rule adding failed. Result=%d\n", rt_rule_entry->status);
		res = rt_rule_entry->status;
		goto fail;
	}
	IPACMDBG("rt rule hdl=%x with ip-type: %d\n", rt_rule_entry->rt_rule_hdl, data->iptype);

	if (data->iptype == IPA_IP_v4)
	{
		dft_rt_rule_hdl[0] = rt_rule_entry->rt_rule_hdl;

		/* initial multicast/broadcast/fragment filter rule */
		IPACM_Iface::init_fl_rule(data->iptype);
	}
	else
	{
		if (num_dft_rt == 0)
		{
			/* initial multicast/broadcast/fragment filter rule */
			IPACM_Iface::init_fl_rule(data->iptype);

			/* add default v6 filter rule */
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								1 * sizeof(struct ipa_flt_rule_add)
								);

			if (!m_pFilteringTable)
			{
				PERROR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)1;


			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6))
			{
				IPACMERR("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_v6);
				free(m_pFilteringTable);
				res = IPACM_FAILURE;
				goto fail;
			}


			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_v6.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(flt_rule_entry.rule.attrib));

			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0X00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
			flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding Filtering rule, aborting...\n");
				free(m_pFilteringTable);
				res = IPACM_FAILURE;
				goto fail;
			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_v6fl_rule_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
			free(m_pFilteringTable);
		}

		dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + num_dft_rt] = rt_rule_entry->rt_rule_hdl;
		num_dft_rt++;
	}

	IPACMDBG("number of default route rules %d\n", num_dft_rt);

fail:
	free(rt_rule);

	return res;
}

/* configure private subnet filter rules*/
int IPACM_Lan::handle_private_subnet(ipa_ip_type iptype)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int i;

	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("lan->handle_private_subnet(); set route/filter rule \n");

	if (iptype == IPA_IP_v4)
	{

		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1,
							sizeof(struct ipa_ioc_add_flt_rule) +
							(IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_add)
							);
		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}
		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v4;
		m_pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
		{
			IPACMERR("LAN m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_lan_v4);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
#if 1
		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4))
		{
			IPACMERR("LAN m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_default_v4);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
#endif
		for (i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
#if 1
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_default_v4.hdl;
			IPACMDBG(" private filter rule use table: %s\n",IPACM_Iface::ipacmcfg->rt_tbl_default_v4.name);
#endif			
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(flt_rule_entry.rule.attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
			memcpy(&(m_pFilteringTable->rules[i]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
			IPACMDBG("Loop %d  5\n", i);
		}

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}

		/* copy filter rule hdls */
		for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{

			private_fl_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
		}
		free(m_pFilteringTable);
	}
	return IPACM_SUCCESS;
}



/*handle wlan iface down event*/
int IPACM_Lan::handle_down_evt()
{
	int i;
	uint32_t tx_index;
	int res = IPACM_SUCCESS;

	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		goto fail;
	}

	IPACMDBG("lan handle_down_evt\n ");

	if (ip_type != IPA_IP_v6)
	{
		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4)
				== false)
		{
			IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}

	/* delete default v6 routing rule */
	if (ip_type != IPA_IP_v4)
	{
		/* may have multiple ipv6 iface-RT rules*/
		for (i = 0; i < num_dft_rt; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + i], IPA_IP_v6)
					== false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}


	/* free unicast routing rule	*/
	for (i = 0; i < num_uni_rt; i++)
	{
		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{

		        if(get_rt_ruleptr(route_rule, i)->ip != tx_prop->tx[tx_index].ip)
 		        {
		    	        IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule added\n", 
		    					    tx_index, tx_prop->tx[tx_index].ip,get_rt_ruleptr(route_rule, i)->ip);		
		    	        continue;
		        }		

			if (m_routing.DeleteRoutingHdl(get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index],
																		 get_rt_ruleptr(route_rule, i)->ip) == false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}

	}

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true)
	{
		handle_software_routing_disable();
	}


	/* delete default filter rules */
	if (ip_type != IPA_IP_v6)
	{
		if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl,
																				IPA_IP_v4,
																				IPV4_DEFAULT_FILTERTING_RULES) == false)
		{
			IPACMERR("Error Adding Filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		/* free private-subnet ipv4 filter rules */
		if (m_filtering.DeleteFilteringHdls(
					private_fl_rule_hdl,
					IPA_IP_v4,
					IPACM_Iface::ipacmcfg->ipa_num_private_subnet) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}


	if (ip_type != IPA_IP_v4)
	{
		if (m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl,
																				IPA_IP_v6,
																				(IPV6_DEFAULT_FILTERTING_RULES + IPV6_DEFAULT_LAN_FILTERTING_RULES)) == false)
		{
			IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}

	/* delete wan filter rule */
	if (IPACM_Wan::isWanUP())
	{
		handle_wan_down();
	}

fail:
	free(tx_prop);
	free(rx_prop);
	free(iface_query);

	return res;
}
