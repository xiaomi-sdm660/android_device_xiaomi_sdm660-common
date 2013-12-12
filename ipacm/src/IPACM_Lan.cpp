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
#include <fcntl.h>
#include <sys/ioctl.h>
#include <IPACM_Netlink.h>
#include <IPACM_Lan.h>
#include <IPACM_Wan.h>
#include <IPACM_IfaceManager.h>
#include "linux/rmnet_ipa_fd_ioctl.h"
#include "linux/ipa_qmi_service_v01.h"
#include "linux/msm_ipa.h"

IPACM_Lan::IPACM_Lan(int iface_index) : IPACM_Iface(iface_index)
{

	num_eth_client = 0;
	header_name_count = 0;

	Nat_App = NatApp::GetInstance();
	if (Nat_App == NULL)
	{
		IPACMERR("unable to get Nat App instance \n");
		return;
	}
	
	/* support eth multiple clients */
	eth_client_len = (sizeof(ipa_eth_client)) + (iface_query->num_tx_props * sizeof(eth_client_rt_hdl));
	eth_client = (ipa_eth_client *)calloc(IPA_MAX_NUM_ETH_CLIENTS, eth_client_len);
	if (eth_client == NULL)
	{
		IPACMERR("unable to allocate memory\n");
		return;
	}

	IPACMDBG(" IPACM->IPACM_Lan(%d) constructor: Tx:%d Rx:%d\n", ipa_if_num,
					 iface_query->num_tx_props, iface_query->num_rx_props);

	num_wan_ul_fl_rule_v4 = 0;
	num_wan_ul_fl_rule_v6 = 0;

	memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
	memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
	
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
	ipacm_ext_prop* ext_prop;

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

					if (IPACM_Wan::isWanUP())
					{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
							if(data->iptype == IPA_IP_v4 || data->iptype == IPA_IP_MAX)
							{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
								handle_wan_up_ex(ext_prop, IPA_IP_v4);
							}			
						}
						else
						{
							handle_wan_up(IPA_IP_v4);
						}
					}

					if(IPACM_Wan::isWanUP_V6())
					{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
							if(data->iptype == IPA_IP_v6 || data->iptype == IPA_IP_MAX)
							{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
								handle_wan_up_ex(ext_prop, IPA_IP_v6);
							}
						}
						else
						{
							handle_wan_up(IPA_IP_v6);
						}
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
	
		if(IPACM_Wan::backhaul_is_sta_mode == false)
		{
			if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
				handle_wan_up_ex(ext_prop, IPA_IP_v4);
			}
		}
		else
		{
			handle_wan_up(IPA_IP_v4);
		}
		break;

	case IPA_HANDLE_WAN_UP_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_UP_V6 event\n");
		
		if(IPACM_Wan::backhaul_is_sta_mode == false)
		{
			if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
				handle_wan_up_ex(ext_prop, IPA_IP_v6);
			}
		}
		else
		{
			handle_wan_up(IPA_IP_v6);
		}
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		handle_wan_down(IPACM_Wan::backhaul_is_sta_mode);
		break;

	case IPA_HANDLE_WAN_DOWN_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		handle_wan_down_v6(IPACM_Wan::backhaul_is_sta_mode);
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			IPACMDBG("check iface %s category: %d\n",IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("ETH iface got client \n");
				/* first construc ETH full header */
					handle_eth_hdr_init(data->mac_addr);
					IPACMDBG("construct ETH header and route rules \n");
				/* Associate with IP and construct RT-rule */
				if (handle_eth_client_ipaddr(data) == IPACM_FAILURE)
				{
					return;
				}
				handle_eth_client_route_rule(data->mac_addr, data->iptype);
				return;
			}
		}
		break;

	case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		{
			ipacm_event_data_all *data = (ipacm_event_data_all *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			
			IPACMDBG("check iface %s category: %d\n",IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("ETH iface delete client \n");
				handle_eth_client_down_evt(data->mac_addr);
				return;
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

/* delete filter rule for wan_down event for IPv4*/
int IPACM_Lan::handle_wan_down(bool is_sta_mode)
{
	ipa_fltr_installed_notif_req_msg_v01 flt_index;
	int fd;

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	flt_rule_count_v4 = IPV4_DEFAULT_FILTERTING_RULES + IPACM_Iface::ipacmcfg->ipa_num_private_subnet;
	
	if(is_sta_mode == false)
	{
		if (m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v4, 
			IPA_IP_v4, num_wan_ul_fl_rule_v4) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(&flt_index, 0, sizeof(flt_index));
		flt_index.source_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[0].src_pipe);
		flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
		flt_index.filter_index_list_len = 0;
		flt_index.embedded_pipe_index_valid = 1;
		flt_index.embedded_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index.retain_header_valid = 1;
		flt_index.retain_header = 0;
		flt_index.embedded_call_mux_id_valid = 1;
		flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();

		if(false == m_filtering.SendFilteringRuleIndex(&flt_index))
		{
			IPACMERR("Error sending filtering rule index, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		num_wan_ul_fl_rule_v4 = 0;
	}
	else
	{
		if (m_filtering.DeleteFilteringHdls(&lan_wan_fl_rule_hdl[0], IPA_IP_v4, 1) == false)
		{
			IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}		
	}
	
	close(fd);
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
	rt_rule_entry->rule.dst = iface_query->excp_pipe;  //go to A5
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
	    rt_rule_entry->rule.dst = iface_query->excp_pipe;  //go to A5
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
			flt_rule_entry.rule.retain_hdr = 1;
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

		flt_rule_count_v4 += IPACM_Iface::ipacmcfg->ipa_num_private_subnet;
		
		/* copy filter rule hdls */
		for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{
			private_fl_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
		}
		free(m_pFilteringTable);
	}
	return IPACM_SUCCESS;
}


/* for STA mode wan up:  configure filter rule for wan_up event*/
int IPACM_Lan::handle_wan_up(ipa_ip_type ip_type)
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

	if(ip_type == IPA_IP_v4)
	{
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
			free(m_pFilteringTable);
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
	}
	else if(ip_type == IPA_IP_v6)
	{
		/* add default v6 filter rule */
		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_flt_rule) +
					1 * sizeof(struct ipa_flt_rule_add));

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
			return IPACM_FAILURE;
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
			return IPACM_FAILURE;
		}
		else
		{
			IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
		}

		/* copy filter hdls */
		dft_v6fl_rule_hdl[IPV6_DEFAULT_FILTERTING_RULES] = m_pFilteringTable->rules[0].flt_rule_hdl;
		free(m_pFilteringTable);	
	}

	return IPACM_SUCCESS;
}

int IPACM_Lan::handle_wan_up_ex(ipacm_ext_prop* ext_prop, ipa_ip_type iptype)
{
	int fd, ret = IPACM_SUCCESS, cnt;
	IPACM_Config* ipacm_config = IPACM_Iface::ipacmcfg;	
	struct ipa_ioc_write_qmapid mux;						

	if(rx_prop != NULL)
	{
		/* give mud ID to IPA-driver for WLAN/LAN pkts */
		fd = open(IPA_DEVICE_NAME, O_RDWR);	
		if (0 == fd) 
		{		
			IPACMDBG("Failed opening %s.\n", IPA_DEVICE_NAME);		
			return IPACM_FAILURE;						
		}

		mux.qmap_id = ipacm_config->GetQmapId();	
		for(cnt=0; cnt<rx_prop->num_rx_props; cnt++)
		{
			mux.client = rx_prop->rx[cnt].src_pipe;	
			ret = ioctl(fd, IPA_IOC_WRITE_QMAPID, &mux);	
			if (ret) 
			{	
				IPACMERR("Failed to write mux id %d\n", mux.qmap_id);		
				close(fd);
				return IPACM_FAILURE;
			}
		}
		close(fd);
	}

	ret = handle_uplink_filter_rule(ext_prop, iptype);
	return ret;
}

/* handle ETH client initial, construct full headers (tx property) */
int IPACM_Lan::handle_eth_hdr_init(uint8_t *mac_addr)
{

#define ETH_IFACE_INDEX_LEN 2

	int res = IPACM_SUCCESS, len = 0;
	char index[ETH_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
    uint32_t cnt;
	int clnt_indx;
	
	clnt_indx = get_eth_client_index(mac_addr);
	
	if (clnt_indx != IPACM_INVALID_INDEX)
	{
		IPACMERR("eth client is found/attached already with index %d \n", clnt_indx);
		return IPACM_FAILURE;
	}	

	/* add header to IPA */
	if (num_eth_client >= IPA_MAX_NUM_ETH_CLIENTS)
	{
		IPACMERR("Reached maximum number(%d) of eth clients\n", IPA_MAX_NUM_ETH_CLIENTS);
		return IPACM_FAILURE;
	}

	IPACMDBG("ETH client number: %d\n", num_eth_client);	
	
	memcpy(get_client_memptr(eth_client, num_eth_client)->mac,
				 mac_addr,
				 sizeof(get_client_memptr(eth_client, num_eth_client)->mac));
	
	
	IPACMDBG("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	IPACMDBG("stored MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(eth_client, num_eth_client)->mac[0],
					 get_client_memptr(eth_client, num_eth_client)->mac[1],
					 get_client_memptr(eth_client, num_eth_client)->mac[2],
					 get_client_memptr(eth_client, num_eth_client)->mac[3],
					 get_client_memptr(eth_client, num_eth_client)->mac[4],
					 get_client_memptr(eth_client, num_eth_client)->mac[5]);

	/* add header to IPA */ 
	if(tx_prop != NULL)
	{
		len = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
		pHeaderDescriptor = (struct ipa_ioc_add_hdr *)calloc(1, len);
		if (pHeaderDescriptor == NULL)
		{
			IPACMERR("calloc failed to allocate pHeaderDescriptor\n");
			return IPACM_FAILURE;
		}

		/* copy partial header for v4*/
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
				 if(tx_prop->tx[cnt].ip==IPA_IP_v4)
				 {
								IPACMDBG("Got partial v4-header name from %d tx props\n", cnt);
								memset(&sCopyHeader, 0, sizeof(sCopyHeader));
								memcpy(sCopyHeader.name,
											 tx_prop->tx[cnt].hdr_name,
											 sizeof(sCopyHeader.name));

								IPACMDBG("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
								if (m_header.CopyHeader(&sCopyHeader) == false)
								{
									PERROR("ioctl copy header failed");
									res = IPACM_FAILURE;
									goto fail;
								}

								IPACMDBG("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
								IPACMDBG("header eth2_ofst_valid: %d, eth2_ofst: %d\n", sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
								if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
								{
									IPACMERR("header oversize\n");
									res = IPACM_FAILURE;
									goto fail;
								}
								else
								{
									memcpy(pHeaderDescriptor->hdr[0].hdr,
												 sCopyHeader.hdr, 
												 sCopyHeader.hdr_len);
								}

								/* copy client mac_addr to partial header */
								if (sCopyHeader.is_eth2_ofst_valid)
								{
									memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
											 mac_addr,
											 IPA_MAC_ADDR_SIZE);								
								}

								pHeaderDescriptor->commit = true;
								pHeaderDescriptor->num_hdrs = 1;

								memset(pHeaderDescriptor->hdr[0].name, 0,
											 sizeof(pHeaderDescriptor->hdr[0].name));

								sprintf(index, "%d", ipa_if_num);
								strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));
								
								strncat(pHeaderDescriptor->hdr[0].name,
												IPA_ETH_HDR_NAME_v4,
												sizeof(IPA_ETH_HDR_NAME_v4));

								sprintf(index, "%d", header_name_count);
								strncat(pHeaderDescriptor->hdr[0].name, index, sizeof(index));

								pHeaderDescriptor->hdr[0].hdr_len = sCopyHeader.hdr_len;
								pHeaderDescriptor->hdr[0].hdr_hdl = -1;
								pHeaderDescriptor->hdr[0].is_partial = 0;
								pHeaderDescriptor->hdr[0].status = -1;

					 if (m_header.AddHeader(pHeaderDescriptor) == false ||
							pHeaderDescriptor->hdr[0].status != 0)
					 {
						IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", pHeaderDescriptor->hdr[0].status);
						res = IPACM_FAILURE;
						goto fail;
					 }

					get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
					IPACMDBG("eth-client(%d) v4 full header name:%s header handle:(0x%x)\n",
												 num_eth_client,
												 pHeaderDescriptor->hdr[0].name,
												 get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v4);
									get_client_memptr(eth_client, num_eth_client)->ipv4_header_set=true;	

					break;  
				 }
		}


		/* copy partial header for v6*/
		for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
		{
			if(tx_prop->tx[cnt].ip==IPA_IP_v6)
			{

				IPACMDBG("Got partial v6-header name from %d tx props\n", cnt);
				memset(&sCopyHeader, 0, sizeof(sCopyHeader));
				memcpy(sCopyHeader.name,
						tx_prop->tx[cnt].hdr_name,
							sizeof(sCopyHeader.name));

				IPACMDBG("header name: %s in tx:%d\n", sCopyHeader.name,cnt);
				if (m_header.CopyHeader(&sCopyHeader) == false)
				{
					PERROR("ioctl copy header failed");
					res = IPACM_FAILURE;
					goto fail;
				}

				IPACMDBG("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
				IPACMDBG("header eth2_ofst_valid: %d, eth2_ofst: %d\n", sCopyHeader.is_eth2_ofst_valid, sCopyHeader.eth2_ofst);
				if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
				{
					IPACMERR("header oversize\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					memcpy(pHeaderDescriptor->hdr[0].hdr,
							sCopyHeader.hdr,
								sCopyHeader.hdr_len);
				}

				/* copy client mac_addr to partial header */
				if (sCopyHeader.is_eth2_ofst_valid)
				{				
					memcpy(&pHeaderDescriptor->hdr[0].hdr[sCopyHeader.eth2_ofst],
						mac_addr,
						IPA_MAC_ADDR_SIZE);
				}
				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
					 sizeof(pHeaderDescriptor->hdr[0].name));

				sprintf(index, "%d", ipa_if_num);
				strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));

				strncat(pHeaderDescriptor->hdr[0].name,
						IPA_ETH_HDR_NAME_v6,
						sizeof(IPA_ETH_HDR_NAME_v6));

				sprintf(index, "%d", header_name_count);
				strncat(pHeaderDescriptor->hdr[0].name, index, sizeof(index));
						
				pHeaderDescriptor->hdr[0].hdr_len = sCopyHeader.hdr_len;
				pHeaderDescriptor->hdr[0].hdr_hdl = -1;
				pHeaderDescriptor->hdr[0].is_partial = 0;
				pHeaderDescriptor->hdr[0].status = -1;

				if (m_header.AddHeader(pHeaderDescriptor) == false ||
						pHeaderDescriptor->hdr[0].status != 0)
				{
					IPACMERR("ioctl IPA_IOC_ADD_HDR failed: %d\n", pHeaderDescriptor->hdr[0].status);
					res = IPACM_FAILURE;
					goto fail;
				}

				get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG("eth-client(%d) v6 full header name:%s header handle:(0x%x)\n",
						 num_eth_client,
						 pHeaderDescriptor->hdr[0].name,
									 get_client_memptr(eth_client, num_eth_client)->hdr_hdl_v6);
						 
									get_client_memptr(eth_client, num_eth_client)->ipv6_header_set=true;			   

				break;  

			}
		}
	}
	/* initialize wifi client*/
	get_client_memptr(eth_client, num_eth_client)->route_rule_set_v4 = false;
    get_client_memptr(eth_client, num_eth_client)->route_rule_set_v6 = 0;
	get_client_memptr(eth_client, num_eth_client)->ipv4_set = false;
    get_client_memptr(eth_client, num_eth_client)->ipv6_set = 0;
	num_eth_client++;
	header_name_count++; //keep increasing header_name_count
	res = IPACM_SUCCESS;
	IPACMDBG("eth client number: %d\n", num_eth_client);

fail:
	free(pHeaderDescriptor);

	return res;
}

/*handle eth client */
int IPACM_Lan::handle_eth_client_ipaddr(ipacm_event_data_all *data)
{
	int clnt_indx;
	int v6_num;

	IPACMDBG("number of eth clients: %d\n", num_eth_client);
	IPACMDBG(" event MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0],
					 data->mac_addr[1],
					 data->mac_addr[2],
					 data->mac_addr[3],
					 data->mac_addr[4],
					 data->mac_addr[5]);

	clnt_indx = get_eth_client_index(data->mac_addr);

		if (clnt_indx == IPACM_INVALID_INDEX)
		{
			IPACMERR("eth client not found/attached \n");
			return IPACM_FAILURE;
		}

	IPACMDBG("Ip-type received %d\n", data->iptype);
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG("ipv4 address: 0x%x\n", data->ipv4_addr);
		if (data->ipv4_addr != 0) /* not 0.0.0.0 */
		{
			if (get_client_memptr(eth_client, clnt_indx)->ipv4_set == false)
			{
				get_client_memptr(eth_client, clnt_indx)->v4_addr = data->ipv4_addr;
				get_client_memptr(eth_client, clnt_indx)->ipv4_set = true;
			}
			else
			{
			   /* check if client got new IPv4 address*/
			   if(data->ipv4_addr == get_client_memptr(eth_client, clnt_indx)->v4_addr)
			   {
			     IPACMDBG("Already setup ipv4 addr for client:%d, ipv4 address didn't change\n", clnt_indx);
				 return IPACM_FAILURE;
			   }
			   else
			   {
			     IPACMDBG("ipv4 addr for client:%d is changed \n", clnt_indx);
			     delete_eth_rtrules(clnt_indx,IPA_IP_v4);
		         get_client_memptr(eth_client, clnt_indx)->route_rule_set_v4 = false;
			     get_client_memptr(eth_client, clnt_indx)->v4_addr = data->ipv4_addr;
			}
		}
	}
	else
	{
		    IPACMDBG("Invalid client IPv4 address \n");
		    return IPACM_FAILURE;
		}
	}
	else
	{
		if ((data->ipv6_addr[0] != 0) || (data->ipv6_addr[1] != 0) ||
				(data->ipv6_addr[2] != 0) || (data->ipv6_addr[3] || 0)) /* check if all 0 not valid ipv6 address */
		{
		   IPACMDBG("ipv6 address: 0x%x:%x:%x:%x\n", data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
                   if(get_client_memptr(eth_client, clnt_indx)->ipv6_set < IPV6_NUM_ADDR)		
		   {
		   
		       for(v6_num=0;v6_num < get_client_memptr(eth_client, clnt_indx)->ipv6_set;v6_num++)
	               {
			      if( data->ipv6_addr[0] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][0] && 
			           data->ipv6_addr[1] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][1] &&
			  	        data->ipv6_addr[2]== get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][2] && 
			  	         data->ipv6_addr[3] == get_client_memptr(eth_client, clnt_indx)->v6_addr[v6_num][3])
			      {
			  	    IPACMDBG("Already see this ipv6 addr for client:%d\n", clnt_indx);
			  	    return IPACM_FAILURE; /* not setup the RT rules*/
			  		break;
			      }  
		       }	   
		   
		       /* not see this ipv6 before for wifi client*/
			   get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][0] = data->ipv6_addr[0];
			   get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][1] = data->ipv6_addr[1];
			   get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][2] = data->ipv6_addr[2];
			   get_client_memptr(eth_client, clnt_indx)->v6_addr[get_client_memptr(eth_client, clnt_indx)->ipv6_set][3] = data->ipv6_addr[3];
			   get_client_memptr(eth_client, clnt_indx)->ipv6_set++;
		    }
		    else
		    {
		         IPACMDBG("Already got 3 ipv6 addr for client:%d\n", clnt_indx);
			 return IPACM_FAILURE; /* not setup the RT rules*/
		    }
		}
	}

	return IPACM_SUCCESS;
}

/*handle eth client routing rule*/
int IPACM_Lan::handle_eth_client_route_rule(uint8_t *mac_addr, ipa_ip_type iptype)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	int eth_index,v6_num;
	const int NUM = 1;

	if(tx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	eth_index = get_eth_client_index(mac_addr);
	if (eth_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG("eth client not found/attached \n");
		return IPACM_SUCCESS;
	}

	if (iptype==IPA_IP_v4) {
		IPACMDBG("eth client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv4_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v4);
	} else {
		IPACMDBG("eth client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", eth_index, iptype,
					 get_client_memptr(eth_client, eth_index)->ipv6_set,
					 get_client_memptr(eth_client, eth_index)->route_rule_set_v6);
	}
	
	/* Add default routing rules if not set yet */
	if ((iptype == IPA_IP_v4
			 && get_client_memptr(eth_client, eth_index)->route_rule_set_v4 == false
			 && get_client_memptr(eth_client, eth_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
		            && get_client_memptr(eth_client, eth_index)->route_rule_set_v6 < get_client_memptr(eth_client, eth_index)->ipv6_set
					))
	{
	
        /* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
        IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);	
	
		rt_rule = (struct ipa_ioc_add_rt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
						NUM * sizeof(struct ipa_rt_rule_add));

		if (rt_rule == NULL)
		{
			PERROR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = (uint8_t)NUM;
		rt_rule->ip = iptype;

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if(iptype != tx_prop->tx[tx_index].ip)
		    {
				IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no RT-rule added\n", 
						tx_index, tx_prop->tx[tx_index].ip,iptype);		
		   	        continue;
		    }	   
			
  	   	    rt_rule_entry = &rt_rule->rules[0];
			rt_rule_entry->at_rear = 0;

			if (iptype == IPA_IP_v4)
			{
		        IPACMDBG("client index(%d):ipv4 address: 0x%x\n", eth_index, 
		  		        get_client_memptr(eth_client, eth_index)->v4_addr);
          
                IPACMDBG("client(%d): v4 header handle:(0x%x)\n",
		  				 eth_index,
		  				 get_client_memptr(eth_client, eth_index)->hdr_hdl_v4);
				strncpy(rt_rule->rt_tbl_name,
								IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
								sizeof(rt_rule->rt_tbl_name));
	   

			    rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			    memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
			    rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	    rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(eth_client, eth_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

			    if (false == m_routing.AddRoutingRule(rt_rule))
  	            {
  	          	            IPACMERR("Routing rule addition failed!\n");
  	          	            free(rt_rule);
  	          	            return IPACM_FAILURE;
			    }
			  
			    /* copy ipv4 RT hdl */
		        get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4 =
  	   	        rt_rule->rules[0].rt_rule_hdl;
		        IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index, 
		      	get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4, iptype);
		  
  	   	    } else {
		   
		        for(v6_num = get_client_memptr(eth_client, eth_index)->route_rule_set_v6;v6_num < get_client_memptr(eth_client, eth_index)->ipv6_set;v6_num++)
			    {
                    IPACMDBG("client(%d): v6 header handle:(0x%x)\n",
		  	    			 eth_index,
		  	    			 get_client_memptr(eth_client, eth_index)->hdr_hdl_v6);
	            
		            /* v6 LAN_RT_TBL */
			    	strncpy(rt_rule->rt_tbl_name,
			    					IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
			    					sizeof(rt_rule->rt_tbl_name));
					
		            /* Support QCMAP LAN traffic feature, send to A5 */
					rt_rule_entry->rule.dst = iface_query->excp_pipe;
			        memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = 0;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
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

		            get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[v6_num] = rt_rule->rules[0].rt_rule_hdl;
		            IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index, 
		            				 get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[v6_num], iptype);
			        
			        /*Copy same rule to v6 WAN RT TBL*/
  	                strncpy(rt_rule->rt_tbl_name,
  	                 					IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name,
  	                 					sizeof(rt_rule->rt_tbl_name));
   	                
                    /* Downlink traffic from Wan iface, directly through IPA */
					rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			        memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = get_client_memptr(eth_client, eth_index)->hdr_hdl_v6;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(eth_client, eth_index)->v6_addr[v6_num][3];
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

		            get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[v6_num] = rt_rule->rules[0].rt_rule_hdl;
					IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index, 
		            				 get_client_memptr(eth_client, eth_index)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[v6_num], iptype);			  
			    }
			}
		   
  	    } /* end of for loop */

		free(rt_rule);

		if (iptype == IPA_IP_v4)
		{
			get_client_memptr(eth_client, eth_index)->route_rule_set_v4 = true;
		}
		else
		{
			get_client_memptr(eth_client, eth_index)->route_rule_set_v6 = get_client_memptr(eth_client, eth_index)->ipv6_set;
		}
	}

	return IPACM_SUCCESS;
}

/*handle eth client del mode*/
int IPACM_Lan::handle_eth_client_down_evt(uint8_t *mac_addr)
{
	int clt_indx;
	uint32_t tx_index;
	int num_eth_client_tmp = num_eth_client;
	int num_v6;

	IPACMDBG("total client: %d\n", num_eth_client_tmp);

	clt_indx = get_eth_client_index(mac_addr);
	if (clt_indx == IPACM_INVALID_INDEX)
	{
		IPACMDBG("eth client not attached\n");
		return IPACM_SUCCESS;
	}

	/* First reset nat rules and then route rules */
	if(get_client_memptr(eth_client, clt_indx)->ipv4_set == true)
	{
	        IPACMDBG("Deleting Nat Rules\n");
	        Nat_App->UpdatePwrSaveIf(get_client_memptr(eth_client, clt_indx)->v4_addr);
 	}	
	
	if (delete_eth_rtrules(clt_indx, IPA_IP_v4))
	{
		IPACMERR("unbale to delete ecm-client v4 route rules\n");
		return IPACM_FAILURE;
	}

	if (delete_eth_rtrules(clt_indx, IPA_IP_v6))
	{
		IPACMERR("unbale to delete ecm-client v6 route rules\n");
		return IPACM_FAILURE;
	}

	/* Delete eth client header */
	if(get_client_memptr(eth_client, clt_indx)->ipv4_header_set == true)
	{
		if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, clt_indx)->hdr_hdl_v4)
				== false)
		{
			return IPACM_FAILURE;
		}
		get_client_memptr(eth_client, clt_indx)->ipv4_header_set = false;
	}

	if(get_client_memptr(eth_client, clt_indx)->ipv6_header_set == true)
	{
		if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, clt_indx)->hdr_hdl_v6)
				== false)
		{
			return IPACM_FAILURE;
		}
		get_client_memptr(eth_client, clt_indx)->ipv6_header_set = false;
	}

	/* Reset ip_set to 0*/
	get_client_memptr(eth_client, clt_indx)->ipv4_set = false;
	get_client_memptr(eth_client, clt_indx)->ipv6_set = 0;
	get_client_memptr(eth_client, clt_indx)->ipv4_header_set = false;
	get_client_memptr(eth_client, clt_indx)->ipv6_header_set = false;
	get_client_memptr(eth_client, clt_indx)->route_rule_set_v4 = false;
	get_client_memptr(eth_client, clt_indx)->route_rule_set_v6 = 0;

	for (; clt_indx < num_eth_client_tmp - 1; clt_indx++)
	{
		memcpy(get_client_memptr(eth_client, clt_indx)->mac,
					 get_client_memptr(eth_client, (clt_indx + 1))->mac,
					 sizeof(get_client_memptr(eth_client, clt_indx)->mac));

		get_client_memptr(eth_client, clt_indx)->hdr_hdl_v4 = get_client_memptr(eth_client, (clt_indx + 1))->hdr_hdl_v4;
		get_client_memptr(eth_client, clt_indx)->hdr_hdl_v6 = get_client_memptr(eth_client, (clt_indx + 1))->hdr_hdl_v6;
		get_client_memptr(eth_client, clt_indx)->v4_addr = get_client_memptr(eth_client, (clt_indx + 1))->v4_addr;

		get_client_memptr(eth_client, clt_indx)->ipv4_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv4_set;
		get_client_memptr(eth_client, clt_indx)->ipv6_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv6_set;
		get_client_memptr(eth_client, clt_indx)->ipv4_header_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv4_header_set;
		get_client_memptr(eth_client, clt_indx)->ipv6_header_set = get_client_memptr(eth_client, (clt_indx + 1))->ipv6_header_set;

		get_client_memptr(eth_client, clt_indx)->route_rule_set_v4 = get_client_memptr(eth_client, (clt_indx + 1))->route_rule_set_v4;
		get_client_memptr(eth_client, clt_indx)->route_rule_set_v6 = get_client_memptr(eth_client, (clt_indx + 1))->route_rule_set_v6;

        for (num_v6=0;num_v6< get_client_memptr(eth_client, clt_indx)->ipv6_set;num_v6++)	 
	    {					
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][0] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][0];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][1] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][1];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][2] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][2];
		    get_client_memptr(eth_client, clt_indx)->v6_addr[num_v6][3] = get_client_memptr(eth_client, (clt_indx + 1))->v6_addr[num_v6][3]; 
        }

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4 =
				 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v4;

			for(num_v6=0;num_v6< get_client_memptr(eth_client, clt_indx)->route_rule_set_v6;num_v6++)	 
			{
			  get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6] =
			   	 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6[num_v6];
			  get_client_memptr(eth_client, clt_indx)->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6] =
			   	 get_client_memptr(eth_client, (clt_indx + 1))->eth_rt_hdl[tx_index].eth_rt_rule_hdl_v6_wan[num_v6];
		    }
		}
	}

	IPACMDBG(" %d eth client deleted successfully \n", num_eth_client);
	num_eth_client = num_eth_client - 1;
	IPACMDBG(" Number of eth client: %d\n", num_eth_client);

	/* Del RM dependency */
	if(num_eth_client == 0)
	{
	   /* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete all IPV4V6 RT-rule*/ 
	   IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);
	}
				
	return IPACM_SUCCESS;
}

/*handle LAN iface down event*/
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
	/* clean eth-client header, routing rules */
	IPACMDBG("left %d eth clients need to be deleted \n ", num_eth_client);
	for (i = 0; i < num_eth_client; i++)
	{
			delete_eth_rtrules(i, IPA_IP_v4);
			delete_eth_rtrules(i, IPA_IP_v6);
	
			IPACMDBG("Delete %d client header\n", num_eth_client);
	
	
			if(get_client_memptr(eth_client, i)->ipv4_header_set == true)
			{
			if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->hdr_hdl_v4)
					== false)
				{
					res = IPACM_FAILURE;
					goto fail;
				}
			}
	
			if(get_client_memptr(eth_client, i)->ipv6_header_set == true)
			{
			if (m_header.DeleteHeaderHdl(get_client_memptr(eth_client, i)->hdr_hdl_v6)
					== false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
			}		
	} /* end of for loop */
	
	/* free the edm clients cache */
	IPACMDBG("Free ecm clients cache\n");

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
		IPACMDBG("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
		handle_wan_down(IPACM_Wan::backhaul_is_sta_mode);
	}

	if (IPACM_Wan::isWanUP_V6() && rx_prop != NULL)
	{
		IPACMDBG("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
		handle_wan_down_v6(IPACM_Wan::backhaul_is_sta_mode);
	}
	
fail:
	/* Delete corresponding ipa_rm_resource_name of RX-endpoint after delete all IPV4V6 FT-rule */ 
	IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);	
	IPACMDBG("Finished delete dependency \n ");

	free(eth_client);

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

/* install UL filter rule from Q6 */
int IPACM_Lan::handle_uplink_filter_rule(ipacm_ext_prop* prop, ipa_ip_type iptype)
{
	ipa_flt_rule_add flt_rule_entry;
	int len = 0, cnt, ret = IPACM_SUCCESS;
	ipa_ioc_add_flt_rule *pFilteringTable;
	ipa_fltr_installed_notif_req_msg_v01 flt_index;
	int fd;
	int i;

	IPACMDBG("Set extended property rules in LAN\n");

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(prop == NULL || prop->num_ext_props <= 0)
	{
		IPACMDBG("No extended property.\n");
		return IPACM_SUCCESS;
	}

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
	}

	memset(&flt_index, 0, sizeof(flt_index));
	flt_index.source_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[0].src_pipe);
	flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
	flt_index.filter_index_list_len = prop->num_ext_props;
	flt_index.embedded_pipe_index_valid = 1;
	flt_index.embedded_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, IPA_CLIENT_APPS_LAN_WAN_PROD);
	flt_index.retain_header_valid = 1;
	flt_index.retain_header = 0;
	flt_index.embedded_call_mux_id_valid = 1;
	flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();

	IPACMDBG("flt_index: src pipe: %d, num of rules: %d, ebd pipe: %d, mux id: %d\n", flt_index.source_pipe_index,
				flt_index.filter_index_list_len, flt_index.embedded_pipe_index, flt_index.embedded_call_mux_id);
	
	len = sizeof(struct ipa_ioc_add_flt_rule) + prop->num_ext_props * sizeof(struct ipa_flt_rule_add);
	pFilteringTable = (struct ipa_ioc_add_flt_rule*)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = prop->num_ext_props;

	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add)); // Zero All Fields
	flt_rule_entry.at_rear = 1;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;

	flt_rule_entry.rule.retain_hdr = 0;
	flt_rule_entry.rule.to_uc = 0;
	flt_rule_entry.rule.eq_attrib_type = 1;
	if(iptype == IPA_IP_v4)
		flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT;
	else if(iptype == IPA_IP_v6)
		flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
	else
	{
		IPACMERR("IP type is not expected.\n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	for(cnt=0; cnt<prop->num_ext_props; cnt++)
	{
		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &prop->prop[cnt].eq_attrib,
					 sizeof(prop->prop[cnt].eq_attrib));
		flt_rule_entry.rule.rt_tbl_idx = prop->prop[cnt].rt_tbl_idx;
		memcpy(&pFilteringTable->rules[cnt], &flt_rule_entry, sizeof(flt_rule_entry));

		if(iptype == IPA_IP_v4)
		{
			IPACMDBG("Filtering rule %d has index %d\n", cnt, flt_rule_count_v4);
			flt_index.filter_index_list[cnt].filter_index = flt_rule_count_v4;
			flt_rule_count_v4++;
		}
		if(iptype == IPA_IP_v6)
		{
			IPACMDBG("Filtering rule %d has index %d\n", cnt, flt_rule_count_v6);
			flt_index.filter_index_list[cnt].filter_index = flt_rule_count_v6;
			flt_rule_count_v6++;
		}
		flt_index.filter_index_list[cnt].filter_handle = prop->prop[cnt].filter_hdl;
	}

	if(false == m_filtering.SendFilteringRuleIndex(&flt_index))
	{
		IPACMERR("Error sending filtering rule index, aborting...\n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	if(false == m_filtering.AddFilteringRule(pFilteringTable))
	{
		IPACMERR("Error Adding RuleTable to Filtering, aborting...\n");
		ret = IPACM_FAILURE;
		goto fail;
	}
	else
	{
		if(iptype == IPA_IP_v4)
		{
			for(i=0; i<pFilteringTable->num_rules; i++)
			{
				wan_ul_fl_rule_hdl_v4[num_wan_ul_fl_rule_v4] = pFilteringTable->rules[i].flt_rule_hdl;
				num_wan_ul_fl_rule_v4++;
			}
		}
		else if(iptype == IPA_IP_v6)
		{
			for(i=0; i<pFilteringTable->num_rules; i++)
			{
				wan_ul_fl_rule_hdl_v6[num_wan_ul_fl_rule_v6] = pFilteringTable->rules[i].flt_rule_hdl;
				num_wan_ul_fl_rule_v6++;
			}
		}
		else
		{
			IPACMERR("IP type is not expected.\n");
			goto fail;
		}
	}

fail:
	free(pFilteringTable);
	close(fd);
	return ret;
}

int IPACM_Lan::handle_wan_down_v6(bool is_sta_mode)
{
	ipa_fltr_installed_notif_req_msg_v01 flt_index;
	int fd;

	fd = open(IPA_DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", IPA_DEVICE_NAME);
		return IPACM_FAILURE;
	}

	flt_rule_count_v6 = IPV6_DEFAULT_FILTERTING_RULES;
	
	if(is_sta_mode == false)
	{
		if (m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v6, 
			IPA_IP_v6, num_wan_ul_fl_rule_v6) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(&flt_index, 0, sizeof(flt_index));
		flt_index.source_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, rx_prop->rx[0].src_pipe);
		flt_index.install_status = IPA_QMI_RESULT_SUCCESS_V01;
		flt_index.filter_index_list_len = 0;
		flt_index.embedded_pipe_index_valid = 1;
		flt_index.embedded_pipe_index = ioctl(fd, IPA_IOC_QUERY_EP_MAPPING, IPA_CLIENT_APPS_LAN_WAN_PROD);
		flt_index.retain_header_valid = 1;
		flt_index.retain_header = 0;
		flt_index.embedded_call_mux_id_valid = 1;
		flt_index.embedded_call_mux_id = IPACM_Iface::ipacmcfg->GetQmapId();

		if(false == m_filtering.SendFilteringRuleIndex(&flt_index))
		{
			IPACMERR("Error sending filtering rule index, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		num_wan_ul_fl_rule_v6 = 0;
	}
	else
	{
		if (m_filtering.DeleteFilteringHdls(&dft_v6fl_rule_hdl[IPV6_DEFAULT_FILTERTING_RULES],
																				IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error Adding RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}
	}

	close(fd);
	return IPACM_SUCCESS;
}
