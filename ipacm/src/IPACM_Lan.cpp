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
        ipv6_set = 0;
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
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
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
				    || ((data->iptype==IPA_IP_v6) && (num_dft_rt_v6!=MAX_DEFAULT_v6_ROUTE_RULES))) 
				{
				  IPACMDBG("Got IPA_ADDR_ADD_EVENT ip-family:%d, v6 num %d: \n",data->iptype,num_dft_rt_v6);
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
			if ((ipa_interface_index == ipa_if_num) && (data->iptype == IPA_IP_v6))
			{
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT for ipv6\n");
				handle_route_add_evt_v6(data);
			}
			/* support ipv4 unicast route add coming from bridge0 via new_neighbor event */
                        if ((ipa_interface_index == ipa_if_num) && (data->iptype == IPA_IP_v4))
	                {
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT for ipv4 \n");
                                ipacm_event_data_addr *data2;				
				data2 = (ipacm_event_data_addr *)
							 malloc(sizeof(ipacm_event_data_addr));				
				if (data2 == NULL)
				{
							IPACMERR("Unable to allocate memory\n");
							return;
				}
				memset(data2, 0, sizeof(data2));
				data2->iptype = IPA_IP_v4;
                                data2->ipv4_addr = data->ipv4_addr;
                                data2->ipv4_addr_mask = 0xFFFFFFFF;
				IPACMDBG("IPv4 address:0x%x, mask:0x%x\n",
										 data2->iptype, data2->ipv4_addr_mask);
                                handle_route_add_evt(data2);
				free(data2);
			}	
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if ((ipa_interface_index == ipa_if_num) && (data->iptype == IPA_IP_v6) )
				{
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT for ipv6\n");
				handle_route_del_evt_v6(data);
			}
			/* support ipv4 unicast route delete coming from bridge0 via new_neighbor event */
                        if ((ipa_interface_index == ipa_if_num) && (data->iptype == IPA_IP_v4))
		        {
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT for ipv4 \n");
                                ipacm_event_data_addr *data2;				
				data2 = (ipacm_event_data_addr *)
							 malloc(sizeof(ipacm_event_data_addr));				
				if (data2 == NULL)
				{
							IPACMERR("Unable to allocate memory\n");
							return;
				}
				memset(data2, 0, sizeof(data2));
				data2->iptype = IPA_IP_v4;
                                data2->ipv4_addr = data->ipv4_addr;
                                data2->ipv4_addr_mask = 0xFFFFFFFF;
				IPACMDBG("IPv4 address:0x%x, mask:0x%x\n",
										 data2->iptype, data2->ipv4_addr_mask);
                                handle_route_add_evt(data2);
				free(data2);
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


/*handle USB client IPv6*/
int IPACM_Lan::handle_route_add_evt_v6(ipacm_event_data_all *data)
{
	/* add unicate route for LAN */
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr sRetHeader;
	uint32_t tx_index;
	int v6_num;

	IPACMDBG(" usb MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0],
					 data->mac_addr[1],
					 data->mac_addr[2],
					 data->mac_addr[3],
					 data->mac_addr[4],
					 data->mac_addr[5]);

	if (tx_prop == NULL)
	{
		IPACMDBG("No Tx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}					 
					 
	IPACMDBG("Ip-type received %d, ipv6_set: %d\n", data->iptype,ipv6_set);
	if (data->iptype == IPA_IP_v6)
	{
	    /* check if all 0 not valid ipv6 address */
		if ((data->ipv6_addr[0]!= 0) || (data->ipv6_addr[1]!= 0) ||
				(data->ipv6_addr[2]!= 0) || (data->ipv6_addr[3] || 0))
		{
		   IPACMDBG("ipv6 address: 0x%x:%x:%x:%x\n", data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
           if(ipv6_set<IPV6_NUM_ADDR)		
		   {
		       /* check if see that before or not*/
		       for(v6_num = 0;v6_num < ipv6_set;v6_num++)
	           {
			      if( data->ipv6_addr[0] == v6_addr[v6_num][0] && 
			           data->ipv6_addr[1] == v6_addr[v6_num][1] &&
			  	        data->ipv6_addr[2]== v6_addr[v6_num][2] && 
			  	         data->ipv6_addr[3] == v6_addr[v6_num][3])
			      {
			  	    IPACMDBG("Already see this ipv6 addr for LAN iface \n");
			  	    return IPACM_SUCCESS; /* not setup the RT rules*/
			  		break;
			  	  }  
			   }	   
		   
	               /* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
	               IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
		   
		       /* not see this ipv6 before for LAN client*/
			   v6_addr[ipv6_set][0] = data->ipv6_addr[0];
			   v6_addr[ipv6_set][1] = data->ipv6_addr[1];
			   v6_addr[ipv6_set][2] = data->ipv6_addr[2];
			   v6_addr[ipv6_set][3] = data->ipv6_addr[3];

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
		       rt_rule_entry = &rt_rule->rules[0];
		       rt_rule_entry->at_rear = false;


		       for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		       {
		       
		           if(data->iptype != tx_prop->tx[tx_index].ip)
		           {
		           	IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule added\n", 
		           					    tx_index, tx_prop->tx[tx_index].ip,data->iptype);		
		           	continue;
		           }		

		           strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name);
				   
		           /* Support QCMAP LAN traffic feature, send to A5 */
                           rt_rule_entry->rule.dst = IPA_CLIENT_A5_LAN_WAN_CONS;
				   memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
				   rt_rule_entry->rule.hdr_hdl = 0;
		       	   rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
		       	   
		       	   if (false == m_routing.AddRoutingRule(rt_rule))
		       	   {
		       	   	IPACMERR("Routing rule addition failed!\n");
		       	   	free(rt_rule);
		       	   	return IPACM_FAILURE;
		       	   }				   
		       	   get_rt_ruleptr(route_rule, num_uni_rt)->rt_rule_hdl[tx_index]
		       		  = rt_rule_entry->rt_rule_hdl;
		       	   IPACMDBG("ipv6 rt rule hdl1 for LAN-table=0x%x, entry:0x%x\n", rt_rule_entry->rt_rule_hdl,get_rt_ruleptr(route_rule, num_uni_rt)->rt_rule_hdl[tx_index]);

			       /* Construct same v6 rule for rt_tbl_wan_v6              */
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

		           /* Downlink traffic from Wan iface, directly through IPA */
		       	   rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
		       	   memcpy(&rt_rule_entry->rule.attrib,
		       	   			 &tx_prop->tx[tx_index].attrib,
		       	   			 sizeof(rt_rule_entry->rule.attrib));
		       	   rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		       	   rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
		       	   
		           strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name);
		       	   if (false == m_routing.AddRoutingRule(rt_rule))
		       	   {
		       	   	IPACMERR("Routing rule addition failed!\n");
		       	   	free(rt_rule);
		       	   	return IPACM_FAILURE;
		       	   }				   
		       	   get_rt_ruleptr(route_rule, (num_uni_rt+1))->rt_rule_hdl[tx_index]
		       		  = rt_rule_entry->rt_rule_hdl;
		       	   IPACMDBG("ipv6 rt rule hdl1 for WAN-table=0x%x, entry:0x%x\n", rt_rule_entry->rt_rule_hdl,get_rt_ruleptr(route_rule, (num_uni_rt+1))->rt_rule_hdl[tx_index]);


			   }

		           get_rt_ruleptr(route_rule, num_uni_rt)->ip = IPA_IP_v6;
		           get_rt_ruleptr(route_rule, num_uni_rt)->v6_addr[0] = data->ipv6_addr[0];
		           get_rt_ruleptr(route_rule, num_uni_rt)->v6_addr[1] = data->ipv6_addr[1];
		           get_rt_ruleptr(route_rule, num_uni_rt)->v6_addr[2] = data->ipv6_addr[2];
		           get_rt_ruleptr(route_rule, num_uni_rt)->v6_addr[3] = data->ipv6_addr[3];

		           get_rt_ruleptr(route_rule, (num_uni_rt+1))->ip = IPA_IP_v6;
		           get_rt_ruleptr(route_rule, (num_uni_rt+1))->v6_addr[0] = data->ipv6_addr[0];
		           get_rt_ruleptr(route_rule, (num_uni_rt+1))->v6_addr[1] = data->ipv6_addr[1];
		           get_rt_ruleptr(route_rule, (num_uni_rt+1))->v6_addr[2] = data->ipv6_addr[2];
		           get_rt_ruleptr(route_rule, (num_uni_rt+1))->v6_addr[3] = data->ipv6_addr[3];

		           num_uni_rt=num_uni_rt+2;
		       free(rt_rule);
			   ipv6_set++;
			   IPACMDBG("Total unicast Rt rules: %d\n", num_uni_rt);
           }
		   else
		   {
		     IPACMDBG("Already got 3 ipv6 addr for LAN usb-client:\n");
			 return IPACM_SUCCESS; /* not setup the RT rules*/
		   } 		   
		}
	}

	return IPACM_SUCCESS;
}


/* handle unicast routing rule add event */
int IPACM_Lan::handle_route_add_evt(ipacm_event_data_addr *data)
{
	/* add unicate route for LAN */
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr sRetHeader;
	uint32_t tx_index;
	int i;

	IPACMDBG("LAN callback: unicast IPA_ROUTE_ADD_EVENT\n");

	if (tx_prop == NULL)
	{
		IPACMDBG("No Tx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (data->iptype == IPA_IP_v6)
	{
		IPACMDBG("Not setup v6 unicast RT-rule with new_route event for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}


	if (num_uni_rt < IPA_MAX_NUM_UNICAST_ROUTE_RULES)
	{
        /* check the unicast RT rule is set or not*/
	    for (i = 0; i < num_uni_rt; i++)
	    {

		   if ( data->iptype == IPA_IP_v4 && 
		         (data->ipv4_addr == get_rt_ruleptr(route_rule, i)->v4_addr) &&
					(data->ipv4_addr_mask == get_rt_ruleptr(route_rule, i)->v4_addr_mask) )
		   {
 		       IPACMDBG("find previous-added ipv4 unicast RT entry as index: %d, ignore adding\n",i);		       
		       return IPACM_SUCCESS;
			   break;
                    }
	    }

                /* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV4 RT-rule set */
	    IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);

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

		rt_rule_entry = &rt_rule->rules[0];
		rt_rule_entry->at_rear = false;

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

		get_rt_ruleptr(route_rule, num_uni_rt)->ip = IPA_IP_v4;
		get_rt_ruleptr(route_rule, num_uni_rt)->v4_addr = data->ipv4_addr;
		get_rt_ruleptr(route_rule, num_uni_rt)->v4_addr_mask = data->ipv4_addr_mask;
                IPACMDBG("index: %d, ip-family: %d, IPv4 address:0x%x, IPv4 address mask:0x%x   \n", 
	    	            num_uni_rt, get_rt_ruleptr(route_rule, num_uni_rt)->ip,
				  get_rt_ruleptr(route_rule, num_uni_rt)->v4_addr,
			          get_rt_ruleptr(route_rule, num_uni_rt)->v4_addr_mask);				
			

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

	if (tx_prop == NULL)
	{
		IPACMDBG("No Tx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (data->iptype == IPA_IP_v6)
	{
		IPACMDBG("Not setup v6 unicast RT-rule with new_route event for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}
	
	/* delete 1 unicast RT rule */
	for (i = 0; i < num_uni_rt; i++)
	{

		if (data->iptype == IPA_IP_v4)
		{
			if ((data->ipv4_addr == get_rt_ruleptr(route_rule, i)->v4_addr) &&
					(data->ipv4_addr_mask == get_rt_ruleptr(route_rule, i)->v4_addr_mask))
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
				for (; i < num_uni_rt; i++)
				{
					get_rt_ruleptr(route_rule, i)->v4_addr = get_rt_ruleptr(route_rule, (i + 1))->v4_addr;
					get_rt_ruleptr(route_rule, i)->v4_addr_mask = get_rt_ruleptr(route_rule, (i + 1))->v4_addr_mask;
					get_rt_ruleptr(route_rule, i)->v6_addr[0] = get_rt_ruleptr(route_rule, (i + 1))->v6_addr[0];
					get_rt_ruleptr(route_rule, i)->v6_addr[1] = get_rt_ruleptr(route_rule, (i + 1))->v6_addr[1];
					get_rt_ruleptr(route_rule, i)->v6_addr[2] = get_rt_ruleptr(route_rule, (i + 1))->v6_addr[2];
					get_rt_ruleptr(route_rule, i)->v6_addr[3] = get_rt_ruleptr(route_rule, (i + 1))->v6_addr[3];
					get_rt_ruleptr(route_rule, i)->ip = get_rt_ruleptr(route_rule, (i + 1))->ip;

					for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
					{
						get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index] = get_rt_ruleptr(route_rule, (i + 1))->rt_rule_hdl[tx_index];
					}
				}

				num_uni_rt -= 1;
				
		        /* Del v4 RM dependency */
	            if(num_uni_rt == 0)
				{
				   /* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule*/ 
				   IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				}
				
				return IPACM_SUCCESS;
			}
		}
	
	}

	return IPACM_FAILURE;
}

/* handle unicast routing rule del event */
int IPACM_Lan::handle_route_del_evt_v6(ipacm_event_data_all *data)
{
	int i,v6_num;
	uint32_t tx_index;

	if (tx_prop == NULL)
	{
		IPACMDBG("No Tx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	/* delete 1 unicast RT rule with 2 entries */
	for (i = 0; i < num_uni_rt; i++)
	{
	    if (data->iptype == IPA_IP_v6)
		{
			if ((data->ipv6_addr[0] == get_rt_ruleptr(route_rule, i)->v6_addr[0]) &&
					(data->ipv6_addr[1] == get_rt_ruleptr(route_rule, i)->v6_addr[1]) &&
					(data->ipv6_addr[2] == get_rt_ruleptr(route_rule, i)->v6_addr[2]) &&
					(data->ipv6_addr[3] == get_rt_ruleptr(route_rule, i)->v6_addr[3]))
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
					
					/* also delete the v6 rules for rt_tbl_wan_v6*/
					if (m_routing.DeleteRoutingHdl(get_rt_ruleptr(route_rule, i+1)->rt_rule_hdl[tx_index],
																				 IPA_IP_v6) == false)
					{
						IPACMERR("Routing rule deletion failed!\n");
						return IPACM_FAILURE;
					}
					
				}

				/* remove that delted route rule entry*/
				for (; i < num_uni_rt; i++)
				{
					for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
					{
						get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index] = get_rt_ruleptr(route_rule, (i + 2))->rt_rule_hdl[tx_index];
					}
					get_rt_ruleptr(route_rule, i)->v4_addr = get_rt_ruleptr(route_rule, (i + 2))->v4_addr;
					get_rt_ruleptr(route_rule, i)->v4_addr_mask = get_rt_ruleptr(route_rule, (i + 2))->v4_addr_mask;
					get_rt_ruleptr(route_rule, i)->v6_addr[0] = get_rt_ruleptr(route_rule, (i + 2))->v6_addr[0];
					get_rt_ruleptr(route_rule, i)->v6_addr[1] = get_rt_ruleptr(route_rule, (i + 2))->v6_addr[1];
					get_rt_ruleptr(route_rule, i)->v6_addr[2] = get_rt_ruleptr(route_rule, (i + 2))->v6_addr[2];
					get_rt_ruleptr(route_rule, i)->v6_addr[3] = get_rt_ruleptr(route_rule, (i + 2))->v6_addr[3];
					get_rt_ruleptr(route_rule, i)->ip = get_rt_ruleptr(route_rule, (i + 2))->ip;

				}

				num_uni_rt -= 2;
				
				/* remove this ipv6-address from LAN client*/
				for(v6_num = 0;v6_num < ipv6_set;v6_num++)
	            {
			        if( data->ipv6_addr[0] == v6_addr[v6_num][0] && 
			             data->ipv6_addr[1] == v6_addr[v6_num][1] &&
			  	          data->ipv6_addr[2]== v6_addr[v6_num][2] && 
			  	           data->ipv6_addr[3] == v6_addr[v6_num][3])
			        {
			  	        IPACMDBG("Delete this ipv6 addr for LAN iface \n");						
						/* remove that delted route rule entry*/
				        for (; v6_num <= ipv6_set; v6_num++)
				        {
					       v6_addr[v6_num][0] = v6_addr[v6_num+1][0];
					       v6_addr[v6_num][1] = v6_addr[v6_num+1][1];
					       v6_addr[v6_num][2] = v6_addr[v6_num+1][2];
					       v6_addr[v6_num][3] = v6_addr[v6_num+1][3];				        	
				        }						
						ipv6_set-=1;						
			  	        IPACMDBG("left %d ipv6-addr for LAN iface \n",ipv6_set);						
			  		    break;
			  	    }  
			    }	

		            /* Del v6 RM dependency */
	                    if(num_uni_rt == 0)
				{
				   /* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule */ 
				   IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
				}				
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

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

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
	int num_ipv6_addr;
	int res = IPACM_SUCCESS;

	/* construct ipa_ioc_add_flt_rule with v6 rules */
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("set route/filter rule ip-type: %d \n", data->iptype);

	if (data->iptype == IPA_IP_v4)
	{
	rt_rule = (struct ipa_ioc_add_rt_rule *)
		 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
						NUM_RULES * sizeof(struct ipa_rt_rule_add));

		if (!rt_rule)
	{
		IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
		return IPACM_FAILURE;
	}

	rt_rule->commit = 1;
	rt_rule->num_rules = NUM_RULES;
	rt_rule->ip = data->iptype;
	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = false;
	rt_rule_entry->rule.dst = IPA_CLIENT_A5_LAN_WAN_CONS;  //go to A5
	rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;
   		strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name);
		rt_rule_entry->rule.attrib.u.v4.dst_addr      = data->ipv4_addr;
		rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
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
		dft_rt_rule_hdl[0] = rt_rule_entry->rt_rule_hdl;
        IPACMDBG("ipv4 iface rt-rule hdl1=0x%x\n", dft_rt_rule_hdl[0]);		
		/* initial multicast/broadcast/fragment filter rule */
		IPACM_Iface::init_fl_rule(data->iptype);
	}
	else
	{
	    /* check if see that v6-addr already or not*/
	    for(num_ipv6_addr=0;num_ipv6_addr<num_dft_rt_v6;num_ipv6_addr++)
	    {
                if((ipv6_addr[num_ipv6_addr][0] == data->ipv6_addr[0]) &&	
	           (ipv6_addr[num_ipv6_addr][1] == data->ipv6_addr[1]) &&	
	            (ipv6_addr[num_ipv6_addr][2] == data->ipv6_addr[2]) &&	
	                (ipv6_addr[num_ipv6_addr][3] == data->ipv6_addr[3]))
                {
	           return IPACM_SUCCESS;
	           break; 
	        }
	    }

		rt_rule = (struct ipa_ioc_add_rt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
							NUM_RULES * sizeof(struct ipa_rt_rule_add));

		if (!rt_rule)
		{
		IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = NUM_RULES;
		rt_rule->ip = data->iptype;
		strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name);

	    rt_rule_entry = &rt_rule->rules[0];
	    rt_rule_entry->at_rear = false;
	    rt_rule_entry->rule.dst = IPA_CLIENT_A5_LAN_WAN_CONS;  //go to A5
	    rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;	
		rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = data->ipv6_addr[0];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = data->ipv6_addr[1];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = data->ipv6_addr[2];
		rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = data->ipv6_addr[3];
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
		ipv6_addr[num_dft_rt_v6][0] = data->ipv6_addr[0];
		ipv6_addr[num_dft_rt_v6][1] = data->ipv6_addr[1];
		ipv6_addr[num_dft_rt_v6][2] = data->ipv6_addr[2];
		ipv6_addr[num_dft_rt_v6][3] = data->ipv6_addr[3];
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
		dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6] = rt_rule_entry->rt_rule_hdl;

        /* setup same rule for v6_wan table*/		
		strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name);
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
		dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6+1] = rt_rule_entry->rt_rule_hdl;
		IPACMDBG("ipv6 wan iface rt-rule hdl=0x%x hdl=0x%x, num_dft_rt_v6: %d \n", 
		          dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6],
		          dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES + 2*num_dft_rt_v6+1],num_dft_rt_v6);

		if (num_dft_rt_v6 == 0)
		{
			/* initial multicast/broadcast/fragment filter rule */
			IPACM_Iface::init_fl_rule(data->iptype);

			if (rx_prop != NULL)
			{
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
			    dft_v6fl_rule_hdl[IPV6_DEFAULT_FILTERTING_RULES] = m_pFilteringTable->rules[0].flt_rule_hdl;
				free(m_pFilteringTable);
			}
		}
		num_dft_rt_v6++;
	}

	IPACMDBG("number of default route rules %d\n", num_dft_rt_v6);

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

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

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

		/* Make LAN-traffic always go A5, use default IPA-RT table */
		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4))
		{
			IPACMERR("LAN m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_default_v4);
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}
		
		for (i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;

                        /* Support priave subnet feature including guest-AP can't talk to primary AP etc */
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_default_v4.hdl;
			IPACMDBG(" private filter rule use table: %s\n",IPACM_Iface::ipacmcfg->rt_tbl_default_v4.name);

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

        IPACMDBG("Finished delete default iface ipv4 rules \n ");	
	
	/* delete default v6 routing rule */
	if (ip_type != IPA_IP_v4)
	{
		/* may have multiple ipv6 iface-RT rules*/
		for (i = 0; i < 2*num_dft_rt_v6; i++)
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

        IPACMDBG("Finished delete default iface ipv6 rules \n ");	

	/* free unicast routing rule	*/
	if (tx_prop != NULL)
	{
		for (i = 0; i < num_uni_rt; i++)
		{
			for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
			{
							if(get_rt_ruleptr(route_rule, i)->ip != tx_prop->tx[tx_index].ip)
							{
	    	            	                            IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no unicast LAN RT-rule deleted, index: %d\n", 
	    	            					    tx_index, tx_prop->tx[tx_index].ip,get_rt_ruleptr(route_rule, i)->ip,i);		
										continue;
							}		

				IPACMDBG("Tx:%d, ip-type: %d - ip-type: %d Delete unicast LAN RT-rule deleted index: %d\n", 
         					    tx_index, tx_prop->tx[tx_index].ip,get_rt_ruleptr(route_rule, i)->ip,i);		

				if (m_routing.DeleteRoutingHdl(get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index],
																			 get_rt_ruleptr(route_rule, i)->ip) == false)
				{
					IPACMERR("Routing rule 0x%x deletion failed!\n",get_rt_ruleptr(route_rule, i)->rt_rule_hdl[tx_index]);
					res = IPACM_FAILURE;
					goto fail;
				}
			}

		}
	}

        /* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule */ 
        IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);				

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true && rx_prop != NULL)
	{
		handle_software_routing_disable();
	}


	/* delete default filter rules */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
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

        IPACMDBG("Finished delete default iface ipv4 filtering rules \n ");

	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
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

        IPACMDBG("Finished delete default iface ipv6 filtering rules \n ");
	
	/* delete wan filter rule */
	if (IPACM_Wan::isWanUP() && rx_prop != NULL)
	{
		handle_wan_down();
	}

fail:
	/* Delete corresponding ipa_rm_resource_name of RX-endpoint after delete all IPV4V6 FT-rule */ 
	IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);	

	if (tx_prop != NULL)
	{
		free(tx_prop); 
	}
	if (rx_prop != NULL)
	{
		free(rx_prop);
	}
	if (iface_query != NULL)
	{
		free(iface_query);
	}
	
	return res;
}
