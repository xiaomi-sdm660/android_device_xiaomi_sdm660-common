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
#include "IPACM_Netlink.h"
#include "IPACM_Lan.h"
#include "IPACM_Wan.h"
#include "IPACM_IfaceManager.h"
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
	if(iface_query != NULL)
	{

	eth_client_len = (sizeof(ipa_eth_client)) + (iface_query->num_tx_props * sizeof(eth_client_rt_hdl));
	eth_client = (ipa_eth_client *)calloc(IPA_MAX_NUM_ETH_CLIENTS, eth_client_len);
	if (eth_client == NULL)
	{
		IPACMERR("unable to allocate memory\n");
		return;
	}

	IPACMDBG(" IPACM->IPACM_Lan(%d) constructor: Tx:%d Rx:%d\n", ipa_if_num,
					 iface_query->num_tx_props, iface_query->num_rx_props);
	}

	num_wan_ul_fl_rule_v4 = 0;
	num_wan_ul_fl_rule_v6 = 0;

	memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
	memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));

	memset(lan2lan_flt_rule_hdl_v4, 0, MAX_OFFLOAD_PAIR * sizeof(lan2lan_flt_rule_hdl));
	num_lan2lan_flt_rule_v4 = 0;

	memset(lan2lan_flt_rule_hdl_v6, 0, MAX_OFFLOAD_PAIR * sizeof(lan2lan_flt_rule_hdl));
	num_lan2lan_flt_rule_v6 = 0;

	memset(lan2lan_hdr_hdl_v4, 0, MAX_OFFLOAD_PAIR*sizeof(lan2lan_hdr_hdl));
	memset(lan2lan_hdr_hdl_v6, 0, MAX_OFFLOAD_PAIR*sizeof(lan2lan_hdr_hdl));

	is_active = true;
	memset(tcp_ctl_flt_rule_hdl_v4, 0, NUM_TCP_CTL_FLT_RULE*sizeof(uint32_t));
	memset(tcp_ctl_flt_rule_hdl_v6, 0, NUM_TCP_CTL_FLT_RULE*sizeof(uint32_t));
	is_mode_switch = false;
	if_ipv4_subnet =0;
	memset(private_fl_rule_hdl, 0, IPA_MAX_PRIVATE_SUBNET_ENTRIES * sizeof(uint32_t));
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
	if(is_active == false && event != IPA_LAN_DELETE_SELF)
	{
		IPACMDBG("The interface is no longer active, return.\n");
		return;
	}

	int ipa_interface_index;
	ipacm_ext_prop* ext_prop;
	ipacm_event_iface_up* data_wan;

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
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				return;
			}
		}
		break;

	case IPA_CFG_CHANGE_EVENT:
		{
			if ( IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat != ipa_if_cate)
			{
				IPACMDBG("Received IPA_CFG_CHANGE_EVENT and category changed\n");
				/* delete previous instance */
				handle_down_evt();
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				is_mode_switch = true; // need post internal usb-link up event
				return;
			}
		}
		break;

	case IPA_PRIVATE_SUBNET_CHANGE_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			/* internel event: data->if_index is ipa_if_index */
			if (data->if_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_PRIVATE_SUBNET_CHANGE_EVENT from itself posting, ignore\n");
				return;
			}
			else
			{
				IPACMDBG("Received IPA_PRIVATE_SUBNET_CHANGE_EVENT from other LAN iface \n");
#ifdef FEATURE_IPA_ANDROID
				handle_private_subnet_android(IPA_IP_v4);
#endif
				IPACMDBG(" delete old private subnet rules, use new sets \n");
				return;
			}
		}
		break;

	case IPA_LAN_DELETE_SELF:
	{
		ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
		if(data->if_index == ipa_if_num)
		{
			IPACMDBG("Received IPA_LAN_DELETE_SELF event.\n");
			IPACMDBG("ipa_LAN (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
			/* posting link-up event for cradle use-case */
			if(is_mode_switch)
			{
				IPACMDBG("Posting IPA_USB_LINK_UP_EVENT event for (%s)\n", dev_name);
				ipacm_cmd_q_data evt_data;
				memset(&evt_data, 0, sizeof(evt_data));

				ipacm_event_data_fid *data_fid = NULL;
				data_fid = (ipacm_event_data_fid *)malloc(sizeof(ipacm_event_data_fid));
				if(data_fid == NULL)
				{
					IPACMERR("unable to allocate memory for IPA_USB_LINK_UP_EVENT data_fid\n");
					return;
				}
				if(IPACM_Iface::ipa_get_if_index(dev_name, &(data_fid->if_index)))
				{
					IPACMERR("Error while getting interface index for %s device", dev_name);
				}
				evt_data.event = IPA_USB_LINK_UP_EVENT;
				evt_data.evt_data = data_fid;
				//IPACMDBG("Posting event:%d\n", evt_data.event);
				IPACM_EvtDispatcher::PostEvt(&evt_data);
			}
			delete this;
		}
		break;
	}

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
					if(handle_addr_evt(data) == IPACM_FAILURE)
					{
						return;
					}
#ifdef FEATURE_IPA_ANDROID
					add_dummy_private_subnet_flt_rule(data->iptype);
					handle_private_subnet_android(data->iptype);
#else
					handle_private_subnet(data->iptype);
#endif

					if (IPACM_Wan::isWanUP())
					{
						if(data->iptype == IPA_IP_v4 || data->iptype == IPA_IP_MAX)
						{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
								handle_wan_up_ex(ext_prop, IPA_IP_v4);
							}
						else
						{
							handle_wan_up(IPA_IP_v4);
						}
					}
					}

					if(IPACM_Wan::isWanUP_V6())
					{
						if((data->iptype == IPA_IP_v6 || data->iptype == IPA_IP_MAX) && num_dft_rt_v6 == 1)
						{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
								handle_wan_up_ex(ext_prop, IPA_IP_v6);
							}
						else
						{
							handle_wan_up(IPA_IP_v6);
						}
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
					IPACMDBG("Finish handling IPA_ADDR_ADD_EVENT for ip-family(%d)\n", data->iptype);
				}
			}
		}
		break;

	case IPA_HANDLE_WAN_UP:
		IPACMDBG("Received IPA_HANDLE_WAN_UP event\n");

		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG("Backhaul is sta mode?%d\n", data_wan->is_sta);
		if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
		{
		if(data_wan->is_sta == false)
		{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
				handle_wan_up_ex(ext_prop, IPA_IP_v4);
			}
		else
		{
			handle_wan_up(IPA_IP_v4);
		}
		}
		break;

	case IPA_HANDLE_WAN_UP_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_UP_V6 event\n");

		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG("Backhaul is sta mode?%d\n", data_wan->is_sta);
		if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
		{
		if(data_wan->is_sta == false)
		{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
				handle_wan_up_ex(ext_prop, IPA_IP_v6);
			}
		else
		{
			handle_wan_up(IPA_IP_v6);
		}
		}
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG("Backhaul is sta mode?%d\n", data_wan->is_sta);
		handle_wan_down(data_wan->is_sta);
		break;

	case IPA_HANDLE_WAN_DOWN_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN_V6 event\n");
		data_wan = (ipacm_event_iface_up*)param;
		if(data_wan == NULL)
		{
			IPACMERR("No event data is found.\n");
			return;
		}
		IPACMDBG("Backhaul is sta mode?%d\n", data_wan->is_sta);
		handle_wan_down_v6(data_wan->is_sta);
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
				handle_lan2lan_client_active(data, IPA_LAN_CLIENT_ACTIVE);
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
				handle_lan2lan_client_active(data, IPA_LAN_CLIENT_INACTIVE);
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

#ifdef CT_OPT
	flt_rule_count_v4 = IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR
						+ NUM_TCP_CTL_FLT_RULE + IPACM_Iface::ipacmcfg->ipa_num_private_subnet;
#else
	flt_rule_count_v4 = IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR
						+ IPACM_Iface::ipacmcfg->ipa_num_private_subnet;
#endif

#ifdef FEATURE_IPA_ANDROID
	flt_rule_count_v4 = flt_rule_count_v4 - IPACM_Iface::ipacmcfg->ipa_num_private_subnet + IPA_MAX_PRIVATE_SUBNET_ENTRIES;
#endif

	if(is_sta_mode == false)
	{
		if (num_wan_ul_fl_rule_v4 > MAX_WAN_UL_FILTER_RULES)
		{
			IPACMERR("number of wan_ul_fl_rule_v4 > MAX_WAN_UL_FILTER_RULES, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}
		if (m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v4,
			IPA_IP_v4, num_wan_ul_fl_rule_v4) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(wan_ul_fl_rule_hdl_v4, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		num_wan_ul_fl_rule_v4 = 0;

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
	const int NUM_RULES = 1;
	int num_ipv6_addr;
	int res = IPACM_SUCCESS;

	IPACMDBG("set route/filter rule ip-type: %d \n", data->iptype);

/* Add private subnet*/
#ifdef FEATURE_IPA_ANDROID
if (data->iptype == IPA_IP_v4)
{
//	IPACMDBG("Origin IPACM private subnet_addr as: 0x%x \n", data->ipv4_addr);
	IPACMDBG("current IPACM private subnet_addr number(%d)\n", IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
	if_ipv4_subnet = (data->ipv4_addr >> 8) << 8;
	IPACMDBG(" Add IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
	if(IPACM_Iface::ipacmcfg->AddPrivateSubnet(if_ipv4_subnet, ipa_if_num) == false)
	{
		IPACMERR(" can't Add IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
	}
}
#endif /* defined(FEATURE_IPA_ANDROID)*/

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
#ifdef CT_OPT
		install_tcp_ctl_flt_rule(IPA_IP_v4);
#endif
		add_dummy_lan2lan_flt_rule(data->iptype);
		init_fl_rule(data->iptype);
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
				return IPACM_FAILURE;
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
#ifdef CT_OPT
			install_tcp_ctl_flt_rule(IPA_IP_v6);
#endif
			add_dummy_lan2lan_flt_rule(data->iptype);
			/* initial multicast/broadcast/fragment filter rule */
			init_fl_rule(data->iptype);
		}
		num_dft_rt_v6++;
		IPACMDBG("number of default route rules %d\n", num_dft_rt_v6);
	}

	IPACMDBG("finish route/filter rule ip-type: %d, res(%d)\n", data->iptype, res);

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
	else
	{
		IPACMDBG("No private subnet rules for ipv6 iface %s\n", dev_name);
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

	/* check only add static UL filter rule once */
	if ((num_dft_rt_v6 ==1 && iptype ==IPA_IP_v6) ||
			(iptype ==IPA_IP_v4))
	{
		IPACMDBG("num_dft_rt_v6 %d iptype %d\n", num_dft_rt_v6, iptype);
		ret = handle_uplink_filter_rule(ext_prop, iptype);
	}
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

								snprintf(index,sizeof(index), "%d", ipa_if_num);
								strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));

								if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_ETH_HDR_NAME_v4, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
								{
									IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
									res = IPACM_FAILURE;
									goto fail;
								}

								snprintf(index,sizeof(index), "%d", header_name_count);
								if (strlcat(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
								{
									IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
									res = IPACM_FAILURE;
									goto fail;
								}

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

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));

				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_ETH_HDR_NAME_v6, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}
				snprintf(index,sizeof(index), "%d", header_name_count);
				if (strlcat(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeaderDescriptor->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}

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
		/* initialize wifi client*/
		get_client_memptr(eth_client, num_eth_client)->route_rule_set_v4 = false;
		get_client_memptr(eth_client, num_eth_client)->route_rule_set_v6 = 0;
		get_client_memptr(eth_client, num_eth_client)->ipv4_set = false;
		get_client_memptr(eth_client, num_eth_client)->ipv6_set = 0;
		num_eth_client++;
		header_name_count++; //keep increasing header_name_count
		res = IPACM_SUCCESS;
		IPACMDBG("eth client number: %d\n", num_eth_client);
	}
	else
	{
		return res;
	}
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
		if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl, IPA_IP_v4, IPV4_DEFAULT_FILTERTING_RULES) == false)
		{
			IPACMERR("Error Deleting Filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
#ifdef CT_OPT
		if (m_filtering.DeleteFilteringHdls(tcp_ctl_flt_rule_hdl_v4, IPA_IP_v4, NUM_TCP_CTL_FLT_RULE) == false)
		{
			IPACMERR("Error deleting default filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
#endif
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(m_filtering.DeleteFilteringHdls(&(lan2lan_flt_rule_hdl_v4[i].rule_hdl), IPA_IP_v4, 1) == false)
			{
				IPACMERR("Error deleting lan2lan IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
		IPACMDBG("Deleted lan2lan IPv4 flt rules.\n");

		/* free private-subnet ipv4 filter rules */
		if (IPACM_Iface::ipacmcfg->ipa_num_private_subnet > IPA_PRIV_SUBNET_FILTER_RULE_HANDLES)
		{
			IPACMERR(" the number of rules are bigger than array, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

#ifdef FEATURE_IPA_ANDROID
		if(m_filtering.DeleteFilteringHdls(private_fl_rule_hdl, IPA_IP_v4, IPA_MAX_PRIVATE_SUBNET_ENTRIES) == false)
		{
			IPACMERR("Error deleting private subnet IPv4 flt rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
#else
		if (m_filtering.DeleteFilteringHdls(private_fl_rule_hdl, IPA_IP_v4, IPACM_Iface::ipacmcfg->ipa_num_private_subnet) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
#endif
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
#ifdef CT_OPT
		if (m_filtering.DeleteFilteringHdls(tcp_ctl_flt_rule_hdl_v6, IPA_IP_v6, NUM_TCP_CTL_FLT_RULE) == false)
		{
			IPACMERR("Error deleting default filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
#endif
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(m_filtering.DeleteFilteringHdls(&(lan2lan_flt_rule_hdl_v6[i].rule_hdl), IPA_IP_v6, 1) == false)
			{
				IPACMERR("Error deleting lan2lan IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
		IPACMDBG("Deleted lan2lan IPv6 flt rules.\n");
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

	/* posting ip to lan2lan module to delete RT/FILTER rules*/
	post_lan2lan_client_disconnect_msg();

/* Delete private subnet*/
#ifdef FEATURE_IPA_ANDROID
if (ip_type != IPA_IP_v6)
{
	IPACMDBG("current IPACM private subnet_addr number(%d)\n", IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
	IPACMDBG(" Delete IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
	if(IPACM_Iface::ipacmcfg->DelPrivateSubnet(if_ipv4_subnet, ipa_if_num) == false)
	{
		IPACMERR(" can't Delete IPACM private subnet_addr as: 0x%x \n", if_ipv4_subnet);
	}
}
#endif /* defined(FEATURE_IPA_ANDROID)*/
fail:
	/* Delete corresponding ipa_rm_resource_name of RX-endpoint after delete all IPV4V6 FT-rule */
	if (rx_prop != NULL)
	{
		free(rx_prop);
	IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);
	IPACMDBG("Finished delete dependency \n ");
	}

	if (eth_client != NULL)
	{
		free(eth_client);
	}

	if (tx_prop != NULL)
	{
		free(tx_prop);
	}
	if (iface_query != NULL)
	{
		free(iface_query);
	}

	is_active = false;
	post_del_self_evt();

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

#ifdef CT_OPT
	flt_rule_count_v6 = IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR;
#else
	flt_rule_count_v6 = IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR;
#endif

	if(is_sta_mode == false)
	{
		if (num_wan_ul_fl_rule_v6 > MAX_WAN_UL_FILTER_RULES)
		{
			IPACMERR(" the number of rules are bigger than array, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		if (m_filtering.DeleteFilteringHdls(wan_ul_fl_rule_hdl_v6,
			IPA_IP_v6, num_wan_ul_fl_rule_v6) == false)
		{
			IPACMERR("Error Deleting RuleTable(1) to Filtering, aborting...\n");
			close(fd);
			return IPACM_FAILURE;
		}

		memset(wan_ul_fl_rule_hdl_v6, 0, MAX_WAN_UL_FILTER_RULES * sizeof(uint32_t));
		num_wan_ul_fl_rule_v6 = 0;

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


/*handle lan2lan client active*/
int IPACM_Lan::handle_lan2lan_client_active(ipacm_event_data_all *data, ipa_cm_event_id event)
{
	if(data == NULL)
	{
		IPACMERR("Event data is empty.\n");
		return IPACM_FAILURE;
	}

	if(data->iptype == IPA_IP_v4 && ip_type != IPA_IP_v4 && ip_type != IPA_IP_MAX)
	{
		IPACMERR("Client has IPv4 addr but iface does not have IPv4 up.\n");
		return IPACM_FAILURE;
	}
	if(data->iptype == IPA_IP_v6 && ip_type != IPA_IP_v6 && ip_type != IPA_IP_MAX)
	{
		IPACMERR("Client has IPv6 addr but iface does not have IPv6 up.\n");
		return IPACM_FAILURE;
	}

	ipacm_cmd_q_data evt_data;
	memset(&evt_data, 0, sizeof(evt_data));

	ipacm_event_lan_client* lan_client;
	lan_client = (ipacm_event_lan_client*)malloc(sizeof(ipacm_event_lan_client));
	if(lan_client == NULL)
	{
		IPACMERR("Unable to allocate memory.\n");
		return IPACM_FAILURE;
	}
	memset(lan_client, 0, sizeof(ipacm_event_lan_client));
	lan_client->iptype = data->iptype;
	lan_client->ipv4_addr = data->ipv4_addr;
	memcpy(lan_client->ipv6_addr, data->ipv6_addr, 4 * sizeof(uint32_t));
	memcpy(lan_client->mac_addr, data->mac_addr, 6 * sizeof(uint8_t));
	lan_client->p_iface = this;

	evt_data.event = event;
	evt_data.evt_data = (void*)lan_client;

	IPACMDBG("Posting event: %d\n", event);
	IPACM_EvtDispatcher::PostEvt(&evt_data);
	return IPACM_SUCCESS;
}

int IPACM_Lan::add_lan2lan_flt_rule(ipa_ip_type iptype, uint32_t src_v4_addr, uint32_t dst_v4_addr, uint32_t* src_v6_addr, uint32_t* dst_v6_addr, uint32_t* rule_hdl)
{
	if(rx_prop == NULL)
	{
		IPACMERR("There is no rx_prop for iface %s, not able to add lan2lan filtering rule.\n", dev_name);
		return IPACM_FAILURE;
	}
	if(rule_hdl == NULL)
	{
		IPACMERR("Filteing rule handle is empty.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG("Got a new lan2lan flt rule with IP type: %d\n", iptype);

	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + sizeof(struct ipa_flt_rule_mdfy);

	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);

	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate lan2lan flt rule memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));

	if(iptype == IPA_IP_v4)
	{
		IPACMDBG("src_v4_addr: %d dst_v4_addr: %d\n", src_v4_addr, dst_v4_addr);

		if(num_lan2lan_flt_rule_v4 >= MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Lan2lan flt rule table is full, not able to add.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		if(false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4))
		{
			IPACMERR("Failed to get routing table %s handle.\n", IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4.name);
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG("Routing handle for table %s: %d\n", IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4.name,
				IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4.hdl);

		flt_rule.status = -1;
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_flt_rule_hdl_v4[i].valid == false)
			{
				flt_rule.rule_hdl = lan2lan_flt_rule_hdl_v4[i].rule_hdl;
				break;
			}
		}
		if(i == MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Failed to find a filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		flt_rule.rule.retain_hdr = 0;
		flt_rule.rule.to_uc = 0;
		flt_rule.rule.eq_attrib_type = 0;
		flt_rule.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4.hdl;

		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
					 sizeof(flt_rule.rule.attrib));
		IPACMDBG("Rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);

		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
		flt_rule.rule.attrib.u.v4.src_addr = src_v4_addr;
		flt_rule.rule.attrib.u.v4.src_addr_mask = 0xFFFFFFFF;

		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.dst_addr = dst_v4_addr;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			lan2lan_flt_rule_hdl_v4[i].valid = true;
			*rule_hdl = lan2lan_flt_rule_hdl_v4[i].rule_hdl;
			num_lan2lan_flt_rule_v4++;
			IPACMDBG("Flt rule modified, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		if(num_lan2lan_flt_rule_v6 >= MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Lan2lan flt rule table is full, not able to add.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		if(src_v6_addr == NULL || dst_v6_addr == NULL)
		{
			IPACMERR("Got IPv6 flt rule but without IPv6 src/dst addr.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG("src_v6_addr: 0x%08x%08x%08x%08x, dst_v6_addr: 0x%08x%08x%08x%08x\n", src_v6_addr[0], src_v6_addr[1],
				src_v6_addr[2], src_v6_addr[3], dst_v6_addr[0], dst_v6_addr[1], dst_v6_addr[2], dst_v6_addr[3]);

		if(false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6))
		{
			IPACMERR("Failed to get routing table %s handle.\n", IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6.name);
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG("Routing handle for table %s: %d\n", IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6.name,
				IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6.hdl);

		flt_rule.status = -1;
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_flt_rule_hdl_v6[i].valid == false)
			{
				flt_rule.rule_hdl = lan2lan_flt_rule_hdl_v6[i].rule_hdl;
				break;
			}
		}
		if(i == MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Failed to find a filtering rule handle.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		flt_rule.rule.retain_hdr = 0;
		flt_rule.rule.to_uc = 0;
		flt_rule.rule.eq_attrib_type = 0;
		flt_rule.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6.hdl;

		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
					 sizeof(flt_rule.rule.attrib));
		IPACMDBG("Rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);

		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
		flt_rule.rule.attrib.u.v6.src_addr[0] = src_v6_addr[0];
		flt_rule.rule.attrib.u.v6.src_addr[1] = src_v6_addr[1];
		flt_rule.rule.attrib.u.v6.src_addr[2] = src_v6_addr[2];
		flt_rule.rule.attrib.u.v6.src_addr[3] = src_v6_addr[3];
		flt_rule.rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.src_addr_mask[2] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.src_addr_mask[3] = 0xFFFFFFFF;


		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = dst_v6_addr[0];
		flt_rule.rule.attrib.u.v6.dst_addr[1] = dst_v6_addr[1];
		flt_rule.rule.attrib.u.v6.dst_addr[2] = dst_v6_addr[2];
		flt_rule.rule.attrib.u.v6.dst_addr[3] = dst_v6_addr[3];
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;

		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			lan2lan_flt_rule_hdl_v6[i].valid = true;
			*rule_hdl = lan2lan_flt_rule_hdl_v6[i].rule_hdl;
			num_lan2lan_flt_rule_v6++;
			IPACMDBG("Flt rule modified, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::add_dummy_lan2lan_flt_rule(ipa_ip_type iptype)
{
	if(rx_prop == NULL)
	{
		IPACMDBG("There is no rx_prop for iface %s, not able to add dummy lan2lan filtering rule.\n", dev_name);
		return 0;
	}

	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_add_flt_rule) +	MAX_OFFLOAD_PAIR * sizeof(struct ipa_flt_rule_add);

	pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt table memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = MAX_OFFLOAD_PAIR;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule.rule.retain_hdr = 0;
	flt_rule.at_rear = true;
	flt_rule.flt_rule_hdl = -1;
	flt_rule.status = -1;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;

	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
			sizeof(flt_rule.rule.attrib));

	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.src_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.src_addr = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr = ~0;

		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy lan2lan v4 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			flt_rule_count_v4 += MAX_OFFLOAD_PAIR;
			/* copy filter rule hdls */
			for (int i = 0; i < MAX_OFFLOAD_PAIR; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					lan2lan_flt_rule_hdl_v4[i].rule_hdl = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Lan2lan v4 flt rule %d hdl:0x%x\n", i, lan2lan_flt_rule_hdl_v4[i].rule_hdl);
				}
				else
				{
					IPACMERR("Failed adding lan2lan v4 flt rule %d\n", i);
					res = IPACM_FAILURE;
					goto fail;
				}
			}
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v6.src_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[3] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = ~0;

		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy lan2lan v6 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			flt_rule_count_v6 += MAX_OFFLOAD_PAIR;
			/* copy filter rule hdls */
			for (int i = 0; i < MAX_OFFLOAD_PAIR; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					lan2lan_flt_rule_hdl_v6[i].rule_hdl = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Lan2lan v6 flt rule %d hdl:0x%x\n", i, lan2lan_flt_rule_hdl_v6[i].rule_hdl);
				}
				else
				{
					IPACMERR("Failed adding lan2lan v6 flt rule %d\n", i);
					res = IPACM_FAILURE;
					goto fail;
				}
			}
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		goto fail;
	}

fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::del_lan2lan_flt_rule(ipa_ip_type iptype, uint32_t rule_hdl)
{
	int i;

	IPACMDBG("Del lan2lan flt rule with IP type: %d hdl: %d\n", iptype, rule_hdl);
	if(iptype == IPA_IP_v4)
	{
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_flt_rule_hdl_v4[i].rule_hdl == rule_hdl)
			{
				if(reset_to_dummy_flt_rule(IPA_IP_v4, rule_hdl) == IPACM_FAILURE)
				{
					IPACMERR("Failed to delete lan2lan v4 flt rule %d\n", rule_hdl);
					return IPACM_FAILURE;
				}
				IPACMDBG("Deleted lan2lan v4 flt rule %d\n", rule_hdl);
				lan2lan_flt_rule_hdl_v4[i].valid = false;
				num_lan2lan_flt_rule_v4--;
				break;
			}
		}

		if(i == MAX_OFFLOAD_PAIR) //not finding the rule
		{
			IPACMERR("The rule is not found.\n");
			return IPACM_FAILURE;
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_flt_rule_hdl_v6[i].rule_hdl == rule_hdl)
			{
				if(reset_to_dummy_flt_rule(IPA_IP_v6, rule_hdl) == IPACM_FAILURE)
				{
					IPACMERR("Failed to delete lan2lan v6 flt rule %d\n", rule_hdl);
					return IPACM_FAILURE;
				}
				IPACMDBG("Deleted lan2lan v6 flt rule %d\n", rule_hdl);
				lan2lan_flt_rule_hdl_v6[i].valid = false;
				num_lan2lan_flt_rule_v6--;
				break;
			}
		}

		if(i == MAX_OFFLOAD_PAIR) //not finding the rule
		{
			IPACMERR("The rule is not found.\n");
			return IPACM_FAILURE;
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

int IPACM_Lan::reset_to_dummy_flt_rule(ipa_ip_type iptype, uint32_t rule_hdl)
{
	int len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	IPACMDBG("Reset flt rule to dummy, IP type: %d, hdl: %d\n", iptype, rule_hdl);
	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + sizeof(struct ipa_flt_rule_mdfy);
	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);

	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt rule memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = 1;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
	flt_rule.status = -1;
	flt_rule.rule_hdl = rule_hdl;

	flt_rule.rule.retain_hdr = 0;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;

	if(iptype == IPA_IP_v4)
	{
		IPACMDBG("Reset IPv4 flt rule to dummy\n");

		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.dst_addr = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.src_addr = ~0;
		flt_rule.rule.attrib.u.v4.src_addr_mask = ~0;

		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACMDBG("Flt rule reset to dummy, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		IPACMDBG("Reset IPv6 flt rule to dummy\n");

		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v6.src_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr[3] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.src_addr_mask[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = ~0;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = ~0;


		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Error modifying filtering rule.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			IPACMDBG("Flt rule reset to dummy, hdl: 0x%x, status: %d\n", pFilteringTable->rules[0].rule_hdl,
						pFilteringTable->rules[0].status);
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		res = IPACM_FAILURE;
		goto fail;
	}

fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::add_lan2lan_hdr(ipa_ip_type iptype, uint8_t* src_mac, uint8_t* dst_mac, uint32_t* hdr_hdl)
{
	if(tx_prop == NULL)
	{
		IPACMERR("There is no tx_prop, cannot add header.\n");
		return IPACM_FAILURE;
	}
	if(src_mac == NULL || dst_mac == NULL)
	{
		IPACMERR("Either src_mac or dst_mac is null, cannot add header.\n");
		return IPACM_FAILURE;
	}
	if(hdr_hdl == NULL)
	{
		IPACMERR("Header handle is empty.\n");
		return IPACM_FAILURE;
	}

	int i, j, len;
	int res = IPACM_SUCCESS;
	char index[4];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeader;

	IPACMDBG("Get lan2lan header request, src_mac: 0x%02x%02x%02x%02x%02x%02x dst_mac: 0x%02x%02x%02x%02x%02x%02x\n",
			src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5], dst_mac[0], dst_mac[1],
			dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);

	len = sizeof(struct ipa_ioc_add_hdr) + sizeof(struct ipa_hdr_add);
	pHeader = (struct ipa_ioc_add_hdr *)malloc(len);
	if (pHeader == NULL)
	{
		IPACMERR("Failed to allocate header\n");
		return IPACM_FAILURE;
	}
	memset(pHeader, 0, len);

	if(iptype == IPA_IP_v4)
	{		/* copy partial header for v4*/
		for(i=0; i<tx_prop->num_tx_props; i++)
		{
			if(tx_prop->tx[i].ip == IPA_IP_v4)
			{
				IPACMDBG("Got v4-header name from %d tx props\n", i);
				memset(&sCopyHeader, 0, sizeof(sCopyHeader));
				memcpy(sCopyHeader.name, tx_prop->tx[i].hdr_name, sizeof(sCopyHeader.name));

				IPACMDBG("Header name: %s\n", sCopyHeader.name);
				if(m_header.CopyHeader(&sCopyHeader) == false)
				{
					IPACMERR("Copy header failed\n");
					res = IPACM_FAILURE;
					goto fail;
				}

				IPACMDBG("Header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
				if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
				{
					IPACMERR("Header oversize\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					memcpy(pHeader->hdr[0].hdr, sCopyHeader.hdr, sCopyHeader.hdr_len);
				}

				if(sCopyHeader.is_eth2_ofst_valid)
				{
					memcpy(&pHeader->hdr[0].hdr[sCopyHeader.eth2_ofst], dst_mac, IPA_MAC_ADDR_SIZE);
					memcpy(&pHeader->hdr[0].hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE], src_mac, IPA_MAC_ADDR_SIZE);
				}
				else
				{
					IPACMERR("Ethernet 2 header offset is invalid.\n");
				}

				pHeader->commit = true;
				pHeader->num_hdrs = 1;

				memset(pHeader->hdr[0].name, 0, sizeof(pHeader->hdr[0].name));
				strlcpy(pHeader->hdr[0].name, IPA_LAN_TO_LAN_USB_HDR_NAME_V4, sizeof(pHeader->hdr[0].name));

				for(j=0; j<MAX_OFFLOAD_PAIR; j++)
				{
					if( lan2lan_hdr_hdl_v4[j].valid == false)
					{
						IPACMDBG("Construct lan2lan hdr with index %d.\n", j);
						break;
					}
				}
				if(j == MAX_OFFLOAD_PAIR)
				{
					IPACMERR("Failed to find an available hdr index.\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				lan2lan_hdr_hdl_v4[j].valid = true;
				snprintf(index,sizeof(index), "%d", j);
				if (strlcat(pHeader->hdr[0].name, index, sizeof(pHeader->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeader->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}

				pHeader->hdr[0].hdr_len = sCopyHeader.hdr_len;
				pHeader->hdr[0].is_partial = 0;
				pHeader->hdr[0].hdr_hdl = -1;
				pHeader->hdr[0].status = -1;

				if (m_header.AddHeader(pHeader) == false || pHeader->hdr[0].status != 0)
				{
					IPACMERR("Ioctl IPA_IOC_ADD_HDR failed with status: %d\n", pHeader->hdr[0].status);
					res = IPACM_FAILURE;
					goto fail;
				}
				IPACMDBG("Installed v4 full header %s header handle 0x%08x\n", pHeader->hdr[0].name,
							pHeader->hdr[0].hdr_hdl);
				*hdr_hdl = pHeader->hdr[0].hdr_hdl;
				lan2lan_hdr_hdl_v4[j].hdr_hdl = pHeader->hdr[0].hdr_hdl;
				break;
			}
		}
	}
	else if(iptype == IPA_IP_v6)
	{		/* copy partial header for v6*/
		for(i=0; i<tx_prop->num_tx_props; i++)
		{
			if(tx_prop->tx[i].ip == IPA_IP_v6)
			{
				IPACMDBG("Got v6-header name from %d tx props\n", i);
				memset(&sCopyHeader, 0, sizeof(sCopyHeader));
				memcpy(sCopyHeader.name, tx_prop->tx[i].hdr_name, sizeof(sCopyHeader.name));

				IPACMDBG("Header name: %s\n", sCopyHeader.name);
				if(m_header.CopyHeader(&sCopyHeader) == false)
				{
					IPACMERR("Copy header failed\n");
					res = IPACM_FAILURE;
					goto fail;
				}

				IPACMDBG("Header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
				if (sCopyHeader.hdr_len > IPA_HDR_MAX_SIZE)
				{
					IPACMERR("Header oversize\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					memcpy(pHeader->hdr[0].hdr, sCopyHeader.hdr, sCopyHeader.hdr_len);
				}
				if(sCopyHeader.is_eth2_ofst_valid)
				{
					memcpy(&pHeader->hdr[0].hdr[sCopyHeader.eth2_ofst], dst_mac, IPA_MAC_ADDR_SIZE);
					memcpy(&pHeader->hdr[0].hdr[sCopyHeader.eth2_ofst+IPA_MAC_ADDR_SIZE], src_mac, IPA_MAC_ADDR_SIZE);
				}
				else
				{
					IPACMERR("Ethernet 2 header offset is invalid.\n");
				}

				pHeader->commit = true;
				pHeader->num_hdrs = 1;

				memset(pHeader->hdr[0].name, 0, sizeof(pHeader->hdr[0].name));
				strlcpy(pHeader->hdr[0].name, IPA_LAN_TO_LAN_USB_HDR_NAME_V6, sizeof(pHeader->hdr[0].name));

				for(j=0; j<MAX_OFFLOAD_PAIR; j++)
				{
					if( lan2lan_hdr_hdl_v6[j].valid == false)
					{
						IPACMDBG("Construct lan2lan hdr with index %d.\n", j);
						break;
					}
				}
				if(j == MAX_OFFLOAD_PAIR)
				{
					IPACMERR("Failed to find an available hdr index.\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				lan2lan_hdr_hdl_v6[j].valid = true;
				snprintf(index,sizeof(index), "%d", j);
				if (strlcat(pHeader->hdr[0].name, index, sizeof(pHeader->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
				{
					IPACMERR(" header name construction failed exceed length (%d)\n", strlen(pHeader->hdr[0].name));
					res = IPACM_FAILURE;
					goto fail;
				}

				pHeader->hdr[0].hdr_len = sCopyHeader.hdr_len;
				pHeader->hdr[0].is_partial = 0;
				pHeader->hdr[0].hdr_hdl = -1;
				pHeader->hdr[0].status = -1;

				if (m_header.AddHeader(pHeader) == false || pHeader->hdr[0].status != 0)
				{
					IPACMERR("Ioctl IPA_IOC_ADD_HDR failed with status: %d\n", pHeader->hdr[0].status);
					res = IPACM_FAILURE;
					goto fail;
				}
				IPACMDBG("Installed v6 full header %s header handle 0x%08x\n", pHeader->hdr[0].name,
							pHeader->hdr[0].hdr_hdl);
				*hdr_hdl = pHeader->hdr[0].hdr_hdl;
				lan2lan_hdr_hdl_v6[j].hdr_hdl = pHeader->hdr[0].hdr_hdl;
				break;
			}
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
	}

fail:
	free(pHeader);
	return res;
}

int IPACM_Lan::del_lan2lan_hdr(ipa_ip_type iptype, uint32_t hdr_hdl)
{
	int i;
	if (m_header.DeleteHeaderHdl(hdr_hdl) == false)
	{
		IPACMERR("Failed to delete header %d\n", hdr_hdl);
		return IPACM_FAILURE;
	}
	IPACMDBG("Deleted header %d\n", hdr_hdl);

	if(iptype == IPA_IP_v4)
	{
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_hdr_hdl_v4[i].hdr_hdl == hdr_hdl)
			{
				lan2lan_hdr_hdl_v4[i].valid = false;
				break;
			}
		}
		if(i == MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Failed to find corresponding hdr hdl.\n");
			return IPACM_FAILURE;
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(lan2lan_hdr_hdl_v6[i].hdr_hdl == hdr_hdl)
			{
				lan2lan_hdr_hdl_v6[i].valid = false;
				break;
			}
		}
		if(i == MAX_OFFLOAD_PAIR)
		{
			IPACMERR("Failed to find corresponding hdr hdl.\n");
			return IPACM_FAILURE;
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}
	return IPACM_SUCCESS;
}

int IPACM_Lan::add_lan2lan_rt_rule(ipa_ip_type iptype, uint32_t src_v4_addr, uint32_t dst_v4_addr,
			uint32_t* src_v6_addr, uint32_t* dst_v6_addr, uint32_t hdr_hdl, lan_to_lan_rt_rule_hdl* rule_hdl)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	int len;
	int res = IPACM_SUCCESS;

	IPACMDBG("Got a new lan2lan rt rule with IP type: %d\n", iptype);

	if(rule_hdl == NULL)
	{
		IPACMERR("Rule hdl is empty.\n");
		return IPACM_FAILURE;
	}
	memset(rule_hdl, 0, sizeof(lan_to_lan_rt_rule_hdl));

	if(tx_prop == NULL)
	{
		IPACMDBG("There is no tx_prop for iface %s, not able to add lan2lan routing rule.\n", dev_name);
		return IPACM_FAILURE;
	}

	len = sizeof(struct ipa_ioc_add_rt_rule) + sizeof(struct ipa_rt_rule_add);
	rt_rule = (struct ipa_ioc_add_rt_rule *)malloc(len);
	if (!rt_rule)
	{
		IPACMERR("Failed to allocate memory for rt rule\n");
		return IPACM_FAILURE;
	}
	memset(rt_rule, 0, len);

	rt_rule->commit = 1;
	rt_rule->num_rules = 1;
	rt_rule->ip = iptype;

	if(iptype == IPA_IP_v4)
	{
		IPACMDBG("src_v4_addr: 0x%08x dst_v4_addr: 0x%08x\n", src_v4_addr, dst_v4_addr);

		strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v4.name);
		rt_rule_entry = &rt_rule->rules[0];
		rt_rule_entry->at_rear = false;
		rt_rule_entry->rt_rule_hdl = 0;
		rt_rule_entry->status = -1;

		for (tx_index = 0; tx_index<iface_query->num_tx_props; tx_index++)
		{
		    if(tx_prop->tx[tx_index].ip != IPA_IP_v4)
		    {
		    	IPACMDBG("Tx:%d, iptype: %d conflict ip-type: %d bypass\n",
		    				tx_index, tx_prop->tx[tx_index].ip, IPA_IP_v4);
		    	continue;
		    }

			rt_rule_entry->rule.hdr_hdl = hdr_hdl;
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			memcpy(&rt_rule_entry->rule.attrib, &tx_prop->tx[tx_index].attrib,
					sizeof(rt_rule_entry->rule.attrib));

			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
			rt_rule_entry->rule.attrib.u.v4.src_addr      = src_v4_addr;
			rt_rule_entry->rule.attrib.u.v4.src_addr_mask = 0xFFFFFFFF;

			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			rt_rule_entry->rule.attrib.u.v4.dst_addr      = dst_v4_addr;
			rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

			if(m_routing.AddRoutingRule(rt_rule) == false)
			{
				IPACMERR("Routing rule addition failed\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACMDBG("Added rt rule hdl: 0x%08x\n", rt_rule_entry->rt_rule_hdl);
			rule_hdl->rule_hdl[rule_hdl->num_rule] = rt_rule_entry->rt_rule_hdl;
			rule_hdl->num_rule++;
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		if(src_v6_addr == NULL || dst_v6_addr == NULL)
		{
			IPACMERR("Got IPv6 rt rule but without IPv6 src/dst addr.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		IPACMDBG("src_v6_addr: 0x%08x%08x%08x%08x, dst_v6_addr: 0x%08x%08x%08x%08x\n", src_v6_addr[0], src_v6_addr[1],
				src_v6_addr[2], src_v6_addr[3], dst_v6_addr[0], dst_v6_addr[1], dst_v6_addr[2], dst_v6_addr[3]);

		strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_lan2lan_v6.name);
		rt_rule_entry = &rt_rule->rules[0];
		rt_rule_entry->at_rear = false;
		rt_rule_entry->rt_rule_hdl = 0;
		rt_rule_entry->status = -1;

		for (tx_index = 0; tx_index<iface_query->num_tx_props; tx_index++)
		{
		    if(tx_prop->tx[tx_index].ip != IPA_IP_v6)
		    {
		    	IPACMDBG("Tx:%d, iptype: %d conflict ip-type: %d bypass\n",
		    				tx_index, tx_prop->tx[tx_index].ip, IPA_IP_v6);
		    	continue;
		    }

			rt_rule_entry->rule.hdr_hdl = hdr_hdl;
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			memcpy(&rt_rule_entry->rule.attrib, &tx_prop->tx[tx_index].attrib,
					sizeof(rt_rule_entry->rule.attrib));

			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_SRC_ADDR;
			rt_rule_entry->rule.attrib.u.v6.src_addr[0] = src_v6_addr[0];
			rt_rule_entry->rule.attrib.u.v6.src_addr[1] = src_v6_addr[1];
			rt_rule_entry->rule.attrib.u.v6.src_addr[2] = src_v6_addr[2];
			rt_rule_entry->rule.attrib.u.v6.src_addr[3] = src_v6_addr[3];
			rt_rule_entry->rule.attrib.u.v6.src_addr_mask[0] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.src_addr_mask[1] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.src_addr_mask[2] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.src_addr_mask[3] = 0xFFFFFFFF;

			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = dst_v6_addr[0];
			rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = dst_v6_addr[1];
			rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = dst_v6_addr[2];
			rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = dst_v6_addr[3];
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;

			if(m_routing.AddRoutingRule(rt_rule) == false)
			{
				IPACMERR("Routing rule addition failed\n");
				res = IPACM_FAILURE;
				goto fail;
			}
			IPACMDBG("Added rt rule hdl: 0x%08x\n", rt_rule_entry->rt_rule_hdl);
			rule_hdl->rule_hdl[rule_hdl->num_rule] = rt_rule_entry->rt_rule_hdl;
			rule_hdl->num_rule++;
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
	}

fail:
	free(rt_rule);
	return res;
}

int IPACM_Lan::del_lan2lan_rt_rule(ipa_ip_type iptype, lan_to_lan_rt_rule_hdl rule_hdl)
{
	if(rule_hdl.num_rule <= 0 || rule_hdl.num_rule > MAX_NUM_PROP)
	{
		IPACMERR("The number of rule handles are not correct.\n");
		return IPACM_FAILURE;
	}

	int i, res = IPACM_SUCCESS;

	IPACMDBG("Get %d rule handles with IP type %d\n", rule_hdl.num_rule, iptype);
	for(i=0; i<rule_hdl.num_rule; i++)
	{
		if(m_routing.DeleteRoutingHdl(rule_hdl.rule_hdl[i], iptype) == false)
		{
			IPACMERR("Failed to delete routing rule hdl %d.\n", rule_hdl.rule_hdl[i]);
			res = IPACM_FAILURE;
		}
		IPACMDBG("Deleted routing rule handle %d\n",rule_hdl.rule_hdl[i]);
	}
	return res;
}

void IPACM_Lan::post_del_self_evt()
{
	ipacm_cmd_q_data evt;
	ipacm_event_data_fid* fid;
	fid = (ipacm_event_data_fid*)malloc(sizeof(ipacm_event_data_fid));
	if(fid == NULL)
	{
		IPACMERR("Failed to allocate fid memory.\n");
		return;
	}
	memset(fid, 0, sizeof(ipacm_event_data_fid));
	memset(&evt, 0, sizeof(ipacm_cmd_q_data));

	fid->if_index = ipa_if_num;

	evt.evt_data = (void*)fid;
	evt.event = IPA_LAN_DELETE_SELF;

	IPACMDBG("Posting event IPA_LAN_DELETE_SELF\n");
	IPACM_EvtDispatcher::PostEvt(&evt);
}

void IPACM_Lan::post_lan2lan_client_disconnect_msg()
{
	int i, j;
	ipacm_cmd_q_data evt_data;
	ipacm_event_lan_client* lan_client;

	for (i = 0; i < num_eth_client; i++)
	{
			if(get_client_memptr(eth_client, i)->ipv4_set == true)
			{
				lan_client = (ipacm_event_lan_client*)malloc(sizeof(ipacm_event_lan_client));
				if(lan_client == NULL)
				{
					IPACMERR("Failed to allocate memory.\n");
					return;
				}
				memset(lan_client, 0, sizeof(ipacm_event_lan_client));
				lan_client->iptype = IPA_IP_v4;
				lan_client->ipv4_addr = get_client_memptr(eth_client, i)->v4_addr;
				lan_client->p_iface = this;

				memset(&evt_data, 0, sizeof(ipacm_cmd_q_data));
				evt_data.evt_data = (void*)lan_client;
				evt_data.event = IPA_LAN_CLIENT_DISCONNECT;

				IPACMDBG("Posting event IPA_LAN_CLIENT_DISCONNECT\n");
				IPACM_EvtDispatcher::PostEvt(&evt_data);
			}

			for (j = 0; j < get_client_memptr(eth_client, i)->ipv6_set; j++)
			{
				lan_client = (ipacm_event_lan_client*)malloc(sizeof(ipacm_event_lan_client));
				if(lan_client == NULL)
				{
					IPACMERR("Failed to allocate memory.\n");
					return;
				}
				memset(lan_client, 0, sizeof(ipacm_event_lan_client));
				lan_client->iptype = IPA_IP_v6;
				lan_client->ipv6_addr[0] = get_client_memptr(eth_client, i)->v6_addr[j][0];
				lan_client->ipv6_addr[0] = get_client_memptr(eth_client, i)->v6_addr[j][0];
				lan_client->ipv6_addr[0] = get_client_memptr(eth_client, i)->v6_addr[j][0];
				lan_client->ipv6_addr[0] = get_client_memptr(eth_client, i)->v6_addr[j][0];
				lan_client->p_iface = this;

				memset(&evt_data, 0, sizeof(ipacm_cmd_q_data));
				evt_data.evt_data = (void*)lan_client;
				evt_data.event = IPA_LAN_CLIENT_DISCONNECT;

				IPACMDBG("Posting event IPA_LAN_CLIENT_DISCONNECT\n");
				IPACM_EvtDispatcher::PostEvt(&evt_data);
			}
	} /* end of for loop */
	return;
}

void IPACM_Lan::install_tcp_ctl_flt_rule(ipa_ip_type iptype)
{
	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return;
	}

	int len, i;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_add_flt_rule) +	NUM_TCP_CTL_FLT_RULE * sizeof(struct ipa_flt_rule_add);

	pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt table memory...\n");
		return;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->num_rules = NUM_TCP_CTL_FLT_RULE;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule.at_rear = true;
	flt_rule.flt_rule_hdl = -1;
	flt_rule.status = -1;

	flt_rule.rule.retain_hdr = 1;
	flt_rule.rule.to_uc = 0;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
	flt_rule.rule.eq_attrib_type = 1;

	flt_rule.rule.eq_attrib.rule_eq_bitmap = 0;

	flt_rule.rule.eq_attrib.rule_eq_bitmap |= (1<<14);
	flt_rule.rule.eq_attrib.metadata_meq32_present = 1;
	flt_rule.rule.eq_attrib.metadata_meq32.offset = 0;
	flt_rule.rule.eq_attrib.metadata_meq32.value = rx_prop->rx[0].attrib.meta_data;
	flt_rule.rule.eq_attrib.metadata_meq32.mask = rx_prop->rx[0].attrib.meta_data_mask;

	flt_rule.rule.eq_attrib.rule_eq_bitmap |= (1<<1);
	flt_rule.rule.eq_attrib.protocol_eq_present = 1;
	flt_rule.rule.eq_attrib.protocol_eq = IPACM_FIREWALL_IPPROTO_TCP;

	flt_rule.rule.eq_attrib.rule_eq_bitmap |= (1<<8);
	flt_rule.rule.eq_attrib.num_ihl_offset_meq_32 = 1;
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].offset = 12;

	/* add TCP FIN rule*/
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_FIN_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_FIN_SHIFT);
	memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_add));

	/* add TCP SYN rule*/
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_SYN_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_SYN_SHIFT);
	memcpy(&(pFilteringTable->rules[1]), &flt_rule, sizeof(struct ipa_flt_rule_add));

	/* add TCP RST rule*/
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_RST_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_RST_SHIFT);
	memcpy(&(pFilteringTable->rules[2]), &flt_rule, sizeof(struct ipa_flt_rule_add));

	if (false == m_filtering.AddFilteringRule(pFilteringTable))
	{
		IPACMERR("Error adding tcp control flt rule\n");
		goto fail;
	}
	else
	{
		if(iptype == IPA_IP_v4)
		{
			for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
			{
				flt_rule_count_v4++;
				tcp_ctl_flt_rule_hdl_v4[i] = pFilteringTable->rules[i].flt_rule_hdl;
			}
		}
		else
		{
			for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
			{
				flt_rule_count_v6++;
				tcp_ctl_flt_rule_hdl_v6[i] = pFilteringTable->rules[i].flt_rule_hdl;
			}
		}
	}

fail:
	free(pFilteringTable);
	return;
}

int IPACM_Lan::add_dummy_private_subnet_flt_rule(ipa_ip_type iptype)
{
	if(rx_prop == NULL)
	{
		IPACMDBG("There is no rx_prop for iface %s, not able to add dummy private subnet filtering rule.\n", dev_name);
		return 0;
	}

	if(iptype == IPA_IP_v6)
	{
		IPACMDBG("There is no ipv6 dummy filter rules needed for iface %s\n", dev_name);
		return 0;
	}
	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_add_flt_rule) +	IPA_MAX_PRIVATE_SUBNET_ENTRIES * sizeof(struct ipa_flt_rule_add);

	pFilteringTable = (struct ipa_ioc_add_flt_rule *)malloc(len);
	if (pFilteringTable == NULL)
	{
		IPACMERR("Error allocate flt table memory...\n");
		return IPACM_FAILURE;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ep = rx_prop->rx[0].src_pipe;
	pFilteringTable->global = false;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = IPA_MAX_PRIVATE_SUBNET_ENTRIES;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule.rule.retain_hdr = 0;
	flt_rule.at_rear = true;
	flt_rule.flt_rule_hdl = -1;
	flt_rule.status = -1;
	flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;

	memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib,
			sizeof(flt_rule.rule.attrib));

	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule.attrib.attrib_mask = IPA_FLT_SRC_ADDR | IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.src_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.src_addr = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = ~0;
		flt_rule.rule.attrib.u.v4.dst_addr = ~0;

		for(i=0; i<IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy private subnet v4 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			flt_rule_count_v4 += IPA_MAX_PRIVATE_SUBNET_ENTRIES;
			/* copy filter rule hdls */
			for (int i = 0; i < IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					private_fl_rule_hdl[i] = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Private subnet v4 flt rule %d hdl:0x%x\n", i, private_fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failed adding lan2lan v4 flt rule %d\n", i);
					res = IPACM_FAILURE;
					goto fail;
				}
			}
		}
	}
fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Lan::handle_private_subnet_android(ipa_ip_type iptype)
{
	int i, len, res = IPACM_SUCCESS, offset;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if(iptype == IPA_IP_v6)
	{
		IPACMDBG("There is no ipv6 dummy filter rules needed for iface %s\n", dev_name);
		return 0;
	}
	else
	{
		for(i=0; i<IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
		{
			reset_to_dummy_flt_rule(IPA_IP_v4, private_fl_rule_hdl[i]);
		}

		len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_mdfy);
		pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);
		if (!pFilteringTable)
		{
			IPACMERR("Failed to allocate ipa_ioc_mdfy_flt_rule memory...\n");
			return IPACM_FAILURE;
		}
		memset(pFilteringTable, 0, len);

		pFilteringTable->commit = 1;
		pFilteringTable->ip = iptype;
		pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

		/* Make LAN-traffic always go A5, use default IPA-RT table */
		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_default_v4))
		{
			IPACMERR("Failed to get routing table handle.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
		flt_rule.status = -1;

		flt_rule.rule.retain_hdr = 1;
		flt_rule.rule.to_uc = 0;
		flt_rule.rule.action = IPA_PASS_TO_ROUTING;
		flt_rule.rule.eq_attrib_type = 0;
		flt_rule.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_default_v4.hdl;
		IPACMDBG("Private filter rule use table: %s\n",IPACM_Iface::ipacmcfg->rt_tbl_default_v4.name);

		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));
		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

		for (i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
		{
			flt_rule.rule_hdl = private_fl_rule_hdl[i];
			flt_rule.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
			flt_rule.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
			IPACMDBG(" IPACM private subnet_addr as: 0x%x entry(%d)\n", flt_rule.rule.attrib.u.v4.dst_addr, i);
		}

		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Failed to modify private subnet filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}
fail:
	if(pFilteringTable != NULL)
	{
		free(pFilteringTable);
	}
	return res;
}
