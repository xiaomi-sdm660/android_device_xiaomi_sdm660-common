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
IPACM_Wlan.cpp

@brief
This file implements the WLAN iface functionality.

@Author
Skylar Chang

*/

#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <IPACM_Wlan.h>
#include <IPACM_Netlink.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <IPACM_Wan.h>
#include <IPACM_Lan.h>
#include <IPACM_IfaceManager.h>

/* static member to store the number of total wifi clients within all APs*/
int IPACM_Wlan::total_num_wifi_clients = 0;

uint32_t* IPACM_Wlan::dummy_flt_rule_hdl_v4 = NULL;
uint32_t* IPACM_Wlan::dummy_flt_rule_hdl_v6 = NULL;
int IPACM_Wlan::num_wlan_ap_iface = 0;

IPACM_Wlan::IPACM_Wlan(int iface_index) : IPACM_Lan(iface_index)
{
#define WLAN_AMPDU_DEFAULT_FILTER_RULES 3

	wlan_ap_index = IPACM_Wlan::num_wlan_ap_iface;
	if(wlan_ap_index < 0 || wlan_ap_index > 1)
	{
		IPACMERR("Wlan_ap_index is not correct: %d, not creating instance.\n", wlan_ap_index);
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
		delete this;
		return;
	}

	num_wifi_client = 0;
	header_name_count = 0;

	if(iface_query != NULL)
	{
		wlan_client_len = (sizeof(ipa_wlan_client)) + (iface_query->num_tx_props * sizeof(wlan_client_rt_hdl));
		wlan_client = (ipa_wlan_client *)calloc(IPA_MAX_NUM_WIFI_CLIENTS, wlan_client_len);
		if (wlan_client == NULL)
		{
			IPACMERR("unable to allocate memory\n");
			return;
		}
		IPACMDBG("index:%d constructor: Tx properties:%d\n", iface_index, iface_query->num_tx_props);
	}
	Nat_App = NatApp::GetInstance();
	if (Nat_App == NULL)
	{
		IPACMERR("unable to get Nat App instance \n");
		return;
	}


	IPACM_Wlan::num_wlan_ap_iface++;
	IPACMDBG("Now the number of wlan AP iface is %d\n", IPACM_Wlan::num_wlan_ap_iface);
	add_dummy_flt_rule();

	return;
}


IPACM_Wlan::~IPACM_Wlan()
{
	IPACM_EvtDispatcher::deregistr(this);
	IPACM_IfaceManager::deregistr(this);
	return;
}

void IPACM_Wlan::event_callback(ipa_cm_event_id event, void *param)
{
	if(is_active == false && event != IPA_LAN_DELETE_SELF)
	{
		IPACMDBG("The interface is no longer active, return.\n");
		return;
	}

	int ipa_interface_index;
	int wlan_index;
	ipacm_ext_prop* ext_prop;
	ipacm_event_iface_up* data_wan;

	switch (event)
	{

	case IPA_WLAN_LINK_DOWN_EVENT:
		{
			ipacm_event_data_fid *data = (ipacm_event_data_fid *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_LINK_DOWN_EVENT\n");
				handle_down_evt();
				/* reset the AP-iface category to unknown */
				IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat=UNKNOWN_IF;
				IPACM_Iface::ipacmcfg->DelNatIfaces(dev_name); // delete NAT-iface
				IPACM_Wlan::total_num_wifi_clients = (IPACM_Wlan::total_num_wifi_clients) - \
                                                                     (num_wifi_client);
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
			IPACM_Wlan::num_wlan_ap_iface--;
			IPACMDBG("Now the number of wlan AP iface is %d\n", IPACM_Wlan::num_wlan_ap_iface);
			del_dummy_flt_rule();

			IPACMDBG("Received IPA_LAN_DELETE_SELF event.\n");
			IPACMDBG("ipa_WLAN (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
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
				/* check v4 not setup before, v6 can have 2 iface ip */
				if( ((data->iptype != ip_type) && (ip_type != IPA_IP_MAX))
				    || ((data->iptype==IPA_IP_v6) && (num_dft_rt_v6!=MAX_DEFAULT_v6_ROUTE_RULES)))
				{
				  IPACMDBG("Got IPA_ADDR_ADD_EVENT ip-family:%d, v6 num %d: \n",data->iptype,num_dft_rt_v6);
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

						evt_data.event = IPA_HANDLE_WLAN_UP;
						evt_data.evt_data = (void *)info;

						/* Insert IPA_HANDLE_WLAN_UP to command queue */
						IPACMDBG("posting IPA_HANDLE_WLAN_UP for IPv4 with below information\n");
						IPACMDBG("IPv4 address:0x%x, IPv4 address mask:0x%x\n",
										 info->ipv4_addr, info->addr_mask);
						IPACM_EvtDispatcher::PostEvt(&evt_data);
					}

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
								IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v4);
							}
						else
						{
							IPACM_Lan::handle_wan_up(IPA_IP_v4);
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
								IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v6);
							}
						else
						{
							IPACM_Lan::handle_wan_up(IPA_IP_v6);
						}
					}
					}

					IPACMDBG("posting IPA_HANDLE_WLAN_UP:Finished checking wan_up\n");
				}
			}
		}
		break;

	case IPA_HANDLE_WAN_UP:
	{
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
				IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v4);
			}
		else
		{
			IPACM_Lan::handle_wan_up(IPA_IP_v4);
		}
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
				IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v6);
			}
		else
		{
			IPACM_Lan::handle_wan_up(IPA_IP_v6);
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
		if(data_wan->is_sta == false && wlan_ap_index > 0)
		{
			IPACMDBG("This is not the first AP instance and not STA mode, ignore WAN_DOWN event.\n");
			return;
		}
		if (rx_prop != NULL)
		{
			handle_wan_down(data_wan->is_sta);
		}
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
		if(data_wan->is_sta == false && wlan_ap_index > 0)
		{
			IPACMDBG("This is not the first AP instance and not STA mode, ignore WAN_DOWN event.\n");
			return;
		}
		if (rx_prop != NULL)
		{
			handle_wan_down_v6(data_wan->is_sta);
		}
		break;

	case IPA_WLAN_CLIENT_ADD_EVENT_EX:
		{
			ipacm_event_data_wlan_ex *data = (ipacm_event_data_wlan_ex *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_CLIENT_ADD_EVENT\n");
				handle_wlan_client_init_ex(data);
			}
		}
		break;

	case IPA_WLAN_CLIENT_DEL_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_CLIENT_DEL_EVENT\n");
				/* support lan2lan ipa-HW feature*/
				handle_lan2lan_msg_post(data->mac_addr, IPA_LAN_CLIENT_DISCONNECT);
				handle_wlan_client_down_evt(data->mac_addr);
			}
		}
		break;

	case IPA_WLAN_CLIENT_POWER_SAVE_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_CLIENT_POWER_SAVE_EVENT\n");
				/* support lan2lan ipa-HW feature*/
				handle_lan2lan_msg_post(data->mac_addr, IPA_LAN_CLIENT_POWER_SAVE);
				handle_wlan_client_pwrsave(data->mac_addr);
			}
		}
		break;

	case IPA_WLAN_CLIENT_RECOVER_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_CLIENT_RECOVER_EVENT\n");
				/* support lan2lan ipa-HW feature*/
				handle_lan2lan_msg_post(data->mac_addr, IPA_LAN_CLIENT_POWER_RECOVER);

				wlan_index = get_wlan_client_index(data->mac_addr);
				if ((wlan_index != IPACM_INVALID_INDEX) &&
						(get_client_memptr(wlan_client, wlan_index)->power_save_set == true))
				{

					IPACMDBG("change wlan client out of  power safe mode \n");
					get_client_memptr(wlan_client, wlan_index)->power_save_set = false;

					/* First add route rules and then nat rules */
                    if(get_client_memptr(wlan_client, wlan_index)->ipv4_set == true) /* for ipv4 */
				    {
						     IPACMDBG("recover client index(%d):ipv4 address: 0x%x\n",
										 wlan_index,
										 get_client_memptr(wlan_client, wlan_index)->v4_addr);

						IPACMDBG("Adding Route Rules\n");
					    handle_wlan_client_route_rule(data->mac_addr, IPA_IP_v4);

						IPACMDBG("Adding Nat Rules\n");
						Nat_App->ResetPwrSaveIf(get_client_memptr(wlan_client, wlan_index)->v4_addr);
				    }

				    if(get_client_memptr(wlan_client, wlan_index)->ipv6_set != 0) /* for ipv6 */
				    {
					    handle_wlan_client_route_rule(data->mac_addr, IPA_IP_v6);
				    }
			    }
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
				if (handle_wlan_client_ipaddr(data) == IPACM_FAILURE)
				{
					return;
				}
				/* support lan2lan ipa-hw feature */
				handle_lan2lan_client_active(data, IPA_LAN_CLIENT_ACTIVE);

				handle_wlan_client_route_rule(data->mac_addr, data->iptype);
				if (data->iptype == IPA_IP_v4)
				{
					Nat_App->ResetPwrSaveIf(data->ipv4_addr);
				}
			}
		}
		break;

		/* handle software routing enable event, iface will update softwarerouting_act to true*/
	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG("Received IPA_SW_ROUTING_ENABLE\n");
		IPACM_Iface::handle_software_routing_enable();
		break;

		/* handle software routing disable event, iface will update softwarerouting_act to false*/
	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG("Received IPA_SW_ROUTING_DISABLE\n");
		IPACM_Iface::handle_software_routing_disable();
		break;

	default:
		break;
	}
	return;
}

/*Configure the initial filter rules */
int IPACM_Wlan::init_fl_rule(ipa_ip_type iptype)
{
	int res = IPACM_SUCCESS, len, offset;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	/* update the iface ip-type to be IPA_IP_v4, IPA_IP_v6 or both*/
	if (iptype == IPA_IP_v4)
	{
		if ((ip_type == IPA_IP_v4) || (ip_type == IPA_IP_MAX))
		{
			IPACMDBG("Interface(%s:%d) already in ip-type %d\n", dev_name, ipa_if_num, ip_type);
			return res;
		}

		if (ip_type == IPA_IP_v6)
		{
			ip_type = IPA_IP_MAX;
		}
		else
		{
			ip_type = IPA_IP_v4;
		}
		IPACMDBG("Interface(%s:%d) now ip-type is %d\n", dev_name, ipa_if_num, ip_type);
	}
	else
	{
		if ((ip_type == IPA_IP_v6) || (ip_type == IPA_IP_MAX))
		{
			IPACMDBG("Interface(%s:%d) already in ip-type %d\n", dev_name, ipa_if_num, ip_type);
			return res;
		}

		if (ip_type == IPA_IP_v4)
		{
			ip_type = IPA_IP_MAX;
		}
		else
		{
			ip_type = IPA_IP_v6;
		}

		IPACMDBG("Interface(%s:%d) now ip-type is %d\n", dev_name, ipa_if_num, ip_type);
	}

    /* ADD corresponding ipa_rm_resource_name of RX-endpoint before adding all IPV4V6 FT-rules */
	if(rx_prop != NULL)
	{
	    IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe],false);
		IPACMDBG("Add producer dependency from %s with registered rx-prop\n", dev_name);
	}
	else
	{
		/* Adding the check if no Rx property registered, no filter rules will be added */
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	/* construct ipa_ioc_add_flt_rule with default filter rules */
	if (iptype == IPA_IP_v4)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Dummy ipv4 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}
#ifndef CT_OPT
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
								+ MAX_OFFLOAD_PAIR;
#else
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
								+ NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR;
#endif

#ifdef FEATURE_IPA_ANDROID
		offset = offset + wlan_ap_index * (IPA_MAX_PRIVATE_SUBNET_ENTRIES - IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
		len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (IPV4_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_mdfy));
		pFilteringTable = (struct ipa_ioc_mdfy_flt_rule *)calloc(1, len);
		if (!pFilteringTable)
		{
			IPACMERR("Error Locate ipa_ioc_mdfy_flt_rule memory...\n");
			return IPACM_FAILURE;
		}
		memset(pFilteringTable, 0, len);

		pFilteringTable->commit = 1;
		pFilteringTable->ip = iptype;
		pFilteringTable->num_rules = (uint8_t)IPV4_DEFAULT_FILTERTING_RULES;

		memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));

		flt_rule.status = -1;

		flt_rule.rule.retain_hdr = 1;
		flt_rule.rule.to_uc = 0;
		flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
		flt_rule.rule.eq_attrib_type = 0;

		/* Configuring Fragment Filtering Rule */
		IPACMDBG("rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);
		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));

		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset];
		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));

		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule.rule.attrib.u.v4.dst_addr = 0xE0000000;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+1];
		memcpy(&(pFilteringTable->rules[1]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		/* Configuring Broadcast Filtering Rule */
		flt_rule.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+2];
		memcpy(&(pFilteringTable->rules[2]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Failed to modify default ipv4 filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			/* copy filter hdls */
			for (int i = 0; i < IPV4_DEFAULT_FILTERTING_RULES; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					dft_v4fl_rule_hdl[i] = pFilteringTable->rules[i].rule_hdl;
					IPACMDBG("Default v4 filter Rule %d HDL:0x%x\n", i, dft_v4fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failed adding default v4 Filtering rule %d\n", i);
				}
			}
		}
	}
	else
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v6 == NULL)
		{
			IPACMERR("Dummy ipv6 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}
#ifndef CT_OPT
		offset = wlan_ap_index * (IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR)
								+ MAX_OFFLOAD_PAIR;
#else
		offset = wlan_ap_index * (IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR)
								+ NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR;
#endif
		len = sizeof(struct ipa_ioc_mdfy_flt_rule) + (IPV6_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_mdfy));
		pFilteringTable = (struct ipa_ioc_mdfy_flt_rule *)calloc(1, len);
		if (!pFilteringTable)
		{
			IPACMERR("Error Locate ipa_ioc_mdfy_flt_rule memory...\n");
			return IPACM_FAILURE;
		}
		memset(pFilteringTable, 0, len);

		pFilteringTable->commit = 1;
		pFilteringTable->ip = iptype;
		pFilteringTable->num_rules = (uint8_t)IPV6_DEFAULT_FILTERTING_RULES;

		memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));

		flt_rule.status = -1;

		flt_rule.rule.retain_hdr = 1;
		flt_rule.rule.to_uc = 0;
		flt_rule.rule.action = IPA_PASS_TO_EXCEPTION;
		flt_rule.rule.eq_attrib_type = 0;

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule.rule.attrib, &rx_prop->rx[0].attrib, sizeof(flt_rule.rule.attrib));
		flt_rule.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = 0XFF000000;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = 0X00000000;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset];
		memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		/* Configuring fe80::/10 Link-Scoped Unicast Filtering Rule */
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = 0XFFC00000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = 0xFE800000;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = 0X00000000;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset+1];
		memcpy(&(pFilteringTable->rules[1]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		/* Configuring fec0::/10 Reserved by IETF Filtering Rule */
		flt_rule.rule.attrib.u.v6.dst_addr_mask[0] = 0XFFC00000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[0] = 0xFEC00000;
		flt_rule.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule.rule.attrib.u.v6.dst_addr[3] = 0X00000000;
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset+2];
		memcpy(&(pFilteringTable->rules[2]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

		if (m_filtering.ModifyFilteringRule(pFilteringTable) == false)
		{
			IPACMERR("Failed to modify default ipv6 filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			for (int i = 0; i < IPV6_DEFAULT_FILTERTING_RULES; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					dft_v6fl_rule_hdl[i] = pFilteringTable->rules[i].rule_hdl;
					IPACMDBG("Default v6 Filter Rule %d HDL:0x%x\n", i, dft_v6fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failing adding v6 default IPV6 rule %d\n", i);
				}
			}
		}
	}

fail:
	free(pFilteringTable);
	return res;
}

int IPACM_Wlan::add_dummy_lan2lan_flt_rule(ipa_ip_type iptype)
{
	if(rx_prop == NULL)
	{
		IPACMDBG("There is no rx_prop for iface %s, not able to add dummy lan2lan filtering rule.\n", dev_name);
		return IPACM_FAILURE;
	}

	int offset;
	if(iptype == IPA_IP_v4)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Dummy ipv4 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}

#ifndef CT_OPT
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#else
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
						+ NUM_TCP_CTL_FLT_RULE;
#endif

#ifdef FEATURE_IPA_ANDROID
		offset = offset + wlan_ap_index * (IPA_MAX_PRIVATE_SUBNET_ENTRIES - IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
		for (int i = 0; i < MAX_OFFLOAD_PAIR; i++)
		{
			lan2lan_flt_rule_hdl_v4[i].rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+i];
			lan2lan_flt_rule_hdl_v4[i].valid = false;
			IPACMDBG("Lan2lan v4 flt rule %d hdl:0x%x\n", i, lan2lan_flt_rule_hdl_v4[i].rule_hdl);
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v6 == NULL)
		{
			IPACMERR("Dummy ipv6 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}

#ifndef CT_OPT
		offset = wlan_ap_index * (IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR);
#else
		offset = wlan_ap_index * (IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR)
						+ NUM_TCP_CTL_FLT_RULE;
#endif

		for (int i = 0; i < MAX_OFFLOAD_PAIR; i++)
		{
			lan2lan_flt_rule_hdl_v6[i].rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset+i];
			lan2lan_flt_rule_hdl_v6[i].valid = false;
			IPACMDBG("Lan2lan v6 flt rule %d hdl:0x%x\n", i, lan2lan_flt_rule_hdl_v6[i].rule_hdl);
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/* configure private subnet filter rules*/
int IPACM_Wlan::handle_private_subnet(ipa_ip_type iptype)
{
	int i, len, res = IPACM_SUCCESS, offset;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	if (iptype == IPA_IP_v4)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Dummy ipv4 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}

#ifndef CT_OPT
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
						+ IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR;
#else
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
						+ IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR;
#endif

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
			flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+i];
			flt_rule.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
			flt_rule.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));
		}

		if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
		{
			IPACMERR("Failed to modify private subnet filtering rules.\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		/* copy filter rule hdls */
		for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{
			private_fl_rule_hdl[i] = pFilteringTable->rules[i].rule_hdl;
		}
	}
	else
	{
		return IPACM_SUCCESS;
	}
fail:
	free(pFilteringTable);
	return res;
}

/* install UL filter rule from Q6 */
int IPACM_Wlan::handle_uplink_filter_rule(ipacm_ext_prop* prop, ipa_ip_type iptype)
{
	ipa_flt_rule_add flt_rule_entry;
	int len = 0, cnt, ret = IPACM_SUCCESS, offset;
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

	if(wlan_ap_index > 0)
	{
		IPACMDBG("This is not the first WLAN AP, do not install modem UL rules.\n");
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

	if(iptype == IPA_IP_v4)
	{
#ifndef CT_OPT
		offset = 2*(IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#else
		offset = 2*(IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif

#ifdef FEATURE_IPA_ANDROID
		offset = offset + 2 * (IPA_MAX_PRIVATE_SUBNET_ENTRIES - IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
	}
	else
	{
#ifndef CT_OPT
		offset = 2*(IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR);
#else
		offset = 2*(IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR);
#endif
	}

	for(cnt=0; cnt<prop->num_ext_props; cnt++)
	{
		memcpy(&flt_rule_entry.rule.eq_attrib,
					 &prop->prop[cnt].eq_attrib,
					 sizeof(prop->prop[cnt].eq_attrib));
		flt_rule_entry.rule.rt_tbl_idx = prop->prop[cnt].rt_tbl_idx;
		memcpy(&pFilteringTable->rules[cnt], &flt_rule_entry, sizeof(flt_rule_entry));

		flt_index.filter_index_list[cnt].filter_index = offset+cnt;
		IPACMDBG("Modem UL filtering rule %d has index %d\n", cnt, offset+cnt);

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

/* handle wifi client initial,copy all partial headers (tx property) */
int IPACM_Wlan::handle_wlan_client_init_ex(ipacm_event_data_wlan_ex *data)
{

#define WLAN_IFACE_INDEX_LEN 2

	int res = IPACM_SUCCESS, len = 0, i, evt_size;
	char index[WLAN_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
        uint32_t cnt;

	/* start of adding header */
	IPACMDBG("Wifi client number for this iface: %d & total number of wlan clients: %d\n",
                 num_wifi_client,IPACM_Wlan::total_num_wifi_clients);

	if ((num_wifi_client >= IPA_MAX_NUM_WIFI_CLIENTS) ||
			(IPACM_Wlan::total_num_wifi_clients >= IPA_MAX_NUM_WIFI_CLIENTS))
	{
		IPACMERR("Reached maximum number of wlan clients\n");
		return IPACM_FAILURE;
	}

	IPACMDBG("Wifi client number: %d\n", num_wifi_client);

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

		evt_size = sizeof(ipacm_event_data_wlan_ex) + data->num_of_attribs * sizeof(struct ipa_wlan_hdr_attrib_val);
		get_client_memptr(wlan_client, num_wifi_client)->p_hdr_info = (ipacm_event_data_wlan_ex*)malloc(evt_size);
		memcpy(get_client_memptr(wlan_client, num_wifi_client)->p_hdr_info, data, evt_size);

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

				for(i = 0; i < data->num_of_attribs; i++)
				{
					if(data->attribs[i].attrib_type == WLAN_HDR_ATTRIB_MAC_ADDR)
					{
						memcpy(get_client_memptr(wlan_client, num_wifi_client)->mac,
								data->attribs[i].u.mac_addr,
								sizeof(get_client_memptr(wlan_client, num_wifi_client)->mac));

						/* copy client mac_addr to partial header */
						memcpy(&pHeaderDescriptor->hdr[0].hdr[data->attribs[i].offset],
									 get_client_memptr(wlan_client, num_wifi_client)->mac,
									 IPA_MAC_ADDR_SIZE);
					}
					else if(data->attribs[i].attrib_type == WLAN_HDR_ATTRIB_STA_ID)
					{
						/* copy client id to header */
						memcpy(&pHeaderDescriptor->hdr[0].hdr[data->attribs[i].offset],
									&data->attribs[i].u.sta_id, sizeof(data->attribs[i].u.sta_id));
					}
					else
					{
						IPACMDBG("The attribute type is not expected!\n");
					}
				}

				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
							 sizeof(pHeaderDescriptor->hdr[0].name));

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));

				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_WLAN_PARTIAL_HDR_NAME_v4, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
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

				get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG("client(%d) v4 full header name:%s header handle:(0x%x)\n",
								 num_wifi_client,
								 pHeaderDescriptor->hdr[0].name,
								 get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v4);
				get_client_memptr(wlan_client, num_wifi_client)->ipv4_header_set=true;
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

				for(i = 0; i < data->num_of_attribs; i++)
				{
					if(data->attribs[i].attrib_type == WLAN_HDR_ATTRIB_MAC_ADDR)
					{
						memcpy(get_client_memptr(wlan_client, num_wifi_client)->mac,
								data->attribs[i].u.mac_addr,
								sizeof(get_client_memptr(wlan_client, num_wifi_client)->mac));

						/* copy client mac_addr to partial header */
						memcpy(&pHeaderDescriptor->hdr[0].hdr[data->attribs[i].offset],
								get_client_memptr(wlan_client, num_wifi_client)->mac,
								IPA_MAC_ADDR_SIZE);
					}
					else if (data->attribs[i].attrib_type == WLAN_HDR_ATTRIB_STA_ID)
					{
						/* copy client id to header */
						memcpy(&pHeaderDescriptor->hdr[0].hdr[data->attribs[i].offset],
								&data->attribs[i].u.sta_id, sizeof(data->attribs[i].u.sta_id));
					}
					else
					{
						IPACMDBG("The attribute type is not expected!\n");
					}
				}

				pHeaderDescriptor->commit = true;
				pHeaderDescriptor->num_hdrs = 1;

				memset(pHeaderDescriptor->hdr[0].name, 0,
							 sizeof(pHeaderDescriptor->hdr[0].name));

				snprintf(index,sizeof(index), "%d", ipa_if_num);
				strlcpy(pHeaderDescriptor->hdr[0].name, index, sizeof(pHeaderDescriptor->hdr[0].name));
				if (strlcat(pHeaderDescriptor->hdr[0].name, IPA_WLAN_PARTIAL_HDR_NAME_v6, sizeof(pHeaderDescriptor->hdr[0].name)) > IPA_RESOURCE_NAME_MAX)
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

				get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG("client(%d) v6 full header name:%s header handle:(0x%x)\n",
								 num_wifi_client,
								 pHeaderDescriptor->hdr[0].name,
											 get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v6);

				get_client_memptr(wlan_client, num_wifi_client)->ipv6_header_set=true;
				break;
			}
		}

		/* initialize wifi client*/
		get_client_memptr(wlan_client, num_wifi_client)->route_rule_set_v4 = false;
		get_client_memptr(wlan_client, num_wifi_client)->route_rule_set_v6 = 0;
		get_client_memptr(wlan_client, num_wifi_client)->ipv4_set = false;
		get_client_memptr(wlan_client, num_wifi_client)->ipv6_set = 0;
		get_client_memptr(wlan_client, num_wifi_client)->power_save_set=false;
		num_wifi_client++;
		header_name_count++; //keep increasing header_name_count
		IPACM_Wlan::total_num_wifi_clients++;
		res = IPACM_SUCCESS;
		IPACMDBG("Wifi client number: %d\n", num_wifi_client);
	}
	else
	{
		return res;
	}

fail:
	free(pHeaderDescriptor);
	return res;
}

/*handle wifi client */
int IPACM_Wlan::handle_wlan_client_ipaddr(ipacm_event_data_all *data)
{
	int clnt_indx;
	int v6_num;

	IPACMDBG("number of wifi clients: %d\n", num_wifi_client);
	IPACMDBG(" event MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 data->mac_addr[0],
					 data->mac_addr[1],
					 data->mac_addr[2],
					 data->mac_addr[3],
					 data->mac_addr[4],
					 data->mac_addr[5]);

	clnt_indx = get_wlan_client_index(data->mac_addr);

		if (clnt_indx == IPACM_INVALID_INDEX)
		{
			IPACMERR("wlan client not found/attached \n");
			return IPACM_FAILURE;
		}

	IPACMDBG("Ip-type received %d\n", data->iptype);
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG("ipv4 address: 0x%x\n", data->ipv4_addr);
		if (data->ipv4_addr != 0) /* not 0.0.0.0 */
		{
			if (get_client_memptr(wlan_client, clnt_indx)->ipv4_set == false)
			{
				get_client_memptr(wlan_client, clnt_indx)->v4_addr = data->ipv4_addr;
				get_client_memptr(wlan_client, clnt_indx)->ipv4_set = true;
			}
			else
			{
			   /* check if client got new IPv4 address*/
			   if(data->ipv4_addr == get_client_memptr(wlan_client, clnt_indx)->v4_addr)
			   {
			     IPACMDBG("Already setup ipv4 addr for client:%d, ipv4 address didn't change\n", clnt_indx);
				 return IPACM_FAILURE;
			   }
			   else
			   {
			     IPACMDBG("ipv4 addr for client:%d is changed \n", clnt_indx);
			     delete_default_qos_rtrules(clnt_indx,IPA_IP_v4);
		         get_client_memptr(wlan_client, clnt_indx)->route_rule_set_v4 = false;
			     get_client_memptr(wlan_client, clnt_indx)->v4_addr = data->ipv4_addr;
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
                   if(get_client_memptr(wlan_client, clnt_indx)->ipv6_set < IPV6_NUM_ADDR)
		   {

		       for(v6_num=0;v6_num < get_client_memptr(wlan_client, clnt_indx)->ipv6_set;v6_num++)
	               {
			      if( data->ipv6_addr[0] == get_client_memptr(wlan_client, clnt_indx)->v6_addr[v6_num][0] &&
			           data->ipv6_addr[1] == get_client_memptr(wlan_client, clnt_indx)->v6_addr[v6_num][1] &&
			  	        data->ipv6_addr[2]== get_client_memptr(wlan_client, clnt_indx)->v6_addr[v6_num][2] &&
			  	         data->ipv6_addr[3] == get_client_memptr(wlan_client, clnt_indx)->v6_addr[v6_num][3])
			      {
			  	    IPACMDBG("Already see this ipv6 addr for client:%d\n", clnt_indx);
			  	    return IPACM_FAILURE; /* not setup the RT rules*/
			  		break;
			      }
		       }

		       /* not see this ipv6 before for wifi client*/
			   get_client_memptr(wlan_client, clnt_indx)->v6_addr[get_client_memptr(wlan_client, clnt_indx)->ipv6_set][0] = data->ipv6_addr[0];
			   get_client_memptr(wlan_client, clnt_indx)->v6_addr[get_client_memptr(wlan_client, clnt_indx)->ipv6_set][1] = data->ipv6_addr[1];
			   get_client_memptr(wlan_client, clnt_indx)->v6_addr[get_client_memptr(wlan_client, clnt_indx)->ipv6_set][2] = data->ipv6_addr[2];
			   get_client_memptr(wlan_client, clnt_indx)->v6_addr[get_client_memptr(wlan_client, clnt_indx)->ipv6_set][3] = data->ipv6_addr[3];
			   get_client_memptr(wlan_client, clnt_indx)->ipv6_set++;
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

/*handle wifi client routing rule*/
int IPACM_Wlan::handle_wlan_client_route_rule(uint8_t *mac_addr, ipa_ip_type iptype)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	uint32_t tx_index;
	int wlan_index,v6_num;
	const int NUM = 1;

	if(tx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	IPACMDBG("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	wlan_index = get_wlan_client_index(mac_addr);
	if (wlan_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not found/attached \n");
		return IPACM_SUCCESS;
	}

	/* during power_save mode, even receive IP_ADDR_ADD, not setting RT rules*/
	if (get_client_memptr(wlan_client, wlan_index)->power_save_set == true)
	{
		IPACMDBG("wlan client is in power safe mode \n");
		return IPACM_SUCCESS;
	}

        if (iptype==IPA_IP_v4)
	{
	IPACMDBG("wlan client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", wlan_index, iptype,
					 get_client_memptr(wlan_client, wlan_index)->ipv4_set,
					 get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4);
	}
        else
	{
	  IPACMDBG("wlan client index: %d, ip-type: %d, ipv6_set:%d, ipv6_rule_num:%d \n", wlan_index, iptype,
					 get_client_memptr(wlan_client, wlan_index)->ipv6_set,
					 get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6);
	}


	/* Add default  Qos routing rules if not set yet */
	if ((iptype == IPA_IP_v4
			 && get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4 == false
			 && get_client_memptr(wlan_client, wlan_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
		            && get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6 < get_client_memptr(wlan_client, wlan_index)->ipv6_set
					))
	{
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
		                IPACMDBG("client index(%d):ipv4 address: 0x%x\n", wlan_index,
		  				        get_client_memptr(wlan_client, wlan_index)->v4_addr);

                                IPACMDBG("client(%d): v4 header handle:(0x%x)\n",
		  				 wlan_index,
		  				 get_client_memptr(wlan_client, wlan_index)->hdr_hdl_v4);
				strncpy(rt_rule->rt_tbl_name,
								IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
								sizeof(rt_rule->rt_tbl_name));


			        rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			        memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.hdr_hdl = get_client_memptr(wlan_client, wlan_index)->hdr_hdl_v4;
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(wlan_client, wlan_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;

			        if (false == m_routing.AddRoutingRule(rt_rule))
  	                        {
  	          	            IPACMERR("Routing rule addition failed!\n");
  	          	            free(rt_rule);
  	          	            return IPACM_FAILURE;
			        }

			        /* copy ipv4 RT hdl */
		                get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4 =
  	   	                rt_rule->rules[0].rt_rule_hdl;
		                IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
		      	        get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4, iptype);
  	   	        }
  	   	        else
  	   	        {
		            for(v6_num = get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6;v6_num < get_client_memptr(wlan_client, wlan_index)->ipv6_set;v6_num++)
			    {
                                IPACMDBG("client(%d): v6 header handle:(0x%x)\n",
		  	    			 wlan_index,
		  	    			 get_client_memptr(wlan_client, wlan_index)->hdr_hdl_v6);

		                /* v6 LAN_RT_TBL */
			    	strncpy(rt_rule->rt_tbl_name,
			    					IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
			    					sizeof(rt_rule->rt_tbl_name));

		                /* Support QCMAP LAN traffic feature, send to A5 */
				rt_rule_entry->rule.dst = iface_query->excp_pipe;
			        memset(&rt_rule_entry->rule.attrib, 0, sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = 0;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][3];
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

		                get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6[v6_num] = rt_rule->rules[0].rt_rule_hdl;
		                IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
		            				 get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6[v6_num], iptype);

			        /*Copy same rule to v6 WAN RT TBL*/
  	                        strncpy(rt_rule->rt_tbl_name,
  	                 					IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name,
  	                 					sizeof(rt_rule->rt_tbl_name));

                                /* Downlink traffic from Wan iface, directly through IPA */
				rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			        memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
		   	        rt_rule_entry->rule.hdr_hdl = get_client_memptr(wlan_client, wlan_index)->hdr_hdl_v6;
			        rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][0];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][1];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][2];
		   	        rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wlan_client, wlan_index)->v6_addr[v6_num][3];
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

		                get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6_wan[v6_num] = rt_rule->rules[0].rt_rule_hdl;

				IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index,
		            				 get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6_wan[v6_num], iptype);
			    }
			}

  	        } /* end of for loop */

		free(rt_rule);

		if (iptype == IPA_IP_v4)
		{
			get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4 = true;
		}
		else
		{
			get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6 = get_client_memptr(wlan_client, wlan_index)->ipv6_set;
		}
	}

	return IPACM_SUCCESS;
}

/*handle wifi client power-save mode*/
int IPACM_Wlan::handle_wlan_client_pwrsave(uint8_t *mac_addr)
{
	int clt_indx;
	IPACMDBG("wlan->handle_wlan_client_pwrsave();\n");

	clt_indx = get_wlan_client_index(mac_addr);
	if (clt_indx == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not attached\n");
		return IPACM_SUCCESS;
	}

        if (get_client_memptr(wlan_client, clt_indx)->power_save_set == false)
	{
		/* First reset nat rules and then route rules */
	    if(get_client_memptr(wlan_client, clt_indx)->ipv4_set == true)
	    {
			IPACMDBG("Deleting Nat Rules\n");
			Nat_App->UpdatePwrSaveIf(get_client_memptr(wlan_client, clt_indx)->v4_addr);
 	     }

		IPACMDBG("Deleting default qos Route Rules\n");
		delete_default_qos_rtrules(clt_indx, IPA_IP_v4);
		delete_default_qos_rtrules(clt_indx, IPA_IP_v6);
                get_client_memptr(wlan_client, clt_indx)->power_save_set = true;
	}
	else
	{
		IPACMDBG("wlan client already in power-save mode\n");
	}
    return IPACM_SUCCESS;
}

/*handle wifi client del mode*/
int IPACM_Wlan::handle_wlan_client_down_evt(uint8_t *mac_addr)
{
	int clt_indx;
	uint32_t tx_index;
	int num_wifi_client_tmp = num_wifi_client;
	int num_v6;

	IPACMDBG("total client: %d\n", num_wifi_client_tmp);

	clt_indx = get_wlan_client_index(mac_addr);
	if (clt_indx == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not attached\n");
		return IPACM_SUCCESS;
	}

	/* First reset nat rules and then route rules */
	if(get_client_memptr(wlan_client, clt_indx)->ipv4_set == true)
	{
	        IPACMDBG("Deleting Nat Rules\n");
	        Nat_App->UpdatePwrSaveIf(get_client_memptr(wlan_client, clt_indx)->v4_addr);
 	}

	if (delete_default_qos_rtrules(clt_indx, IPA_IP_v4))
	{
		IPACMERR("unbale to delete v4 default qos route rules\n");
		return IPACM_FAILURE;
	}

	if (delete_default_qos_rtrules(clt_indx, IPA_IP_v6))
	{
		IPACMERR("unbale to delete v6 default qos route rules\n");
		return IPACM_FAILURE;
	}

	/* Delete wlan client header */
	if(get_client_memptr(wlan_client, clt_indx)->ipv4_header_set == true)
	{
	if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, clt_indx)->hdr_hdl_v4)
			== false)
	{
		return IPACM_FAILURE;
	}
		get_client_memptr(wlan_client, clt_indx)->ipv4_header_set = false;
	}

	if(get_client_memptr(wlan_client, clt_indx)->ipv6_header_set == true)
	{
	if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, clt_indx)->hdr_hdl_v6)
			== false)
	{
		return IPACM_FAILURE;
	}
		get_client_memptr(wlan_client, clt_indx)->ipv6_header_set = false;
	}

	/* Reset ip_set to 0*/
	get_client_memptr(wlan_client, clt_indx)->ipv4_set = false;
	get_client_memptr(wlan_client, clt_indx)->ipv6_set = 0;
	get_client_memptr(wlan_client, clt_indx)->ipv4_header_set = false;
	get_client_memptr(wlan_client, clt_indx)->ipv6_header_set = false;
	get_client_memptr(wlan_client, clt_indx)->route_rule_set_v4 = false;
	get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6 = 0;
	free(get_client_memptr(wlan_client, clt_indx)->p_hdr_info);

	for (; clt_indx < num_wifi_client_tmp - 1; clt_indx++)
	{
		get_client_memptr(wlan_client, clt_indx)->p_hdr_info = get_client_memptr(wlan_client, (clt_indx + 1))->p_hdr_info;

		memcpy(get_client_memptr(wlan_client, clt_indx)->mac,
					 get_client_memptr(wlan_client, (clt_indx + 1))->mac,
					 sizeof(get_client_memptr(wlan_client, clt_indx)->mac));

		get_client_memptr(wlan_client, clt_indx)->hdr_hdl_v4 = get_client_memptr(wlan_client, (clt_indx + 1))->hdr_hdl_v4;
		get_client_memptr(wlan_client, clt_indx)->hdr_hdl_v6 = get_client_memptr(wlan_client, (clt_indx + 1))->hdr_hdl_v6;
		get_client_memptr(wlan_client, clt_indx)->v4_addr = get_client_memptr(wlan_client, (clt_indx + 1))->v4_addr;

		get_client_memptr(wlan_client, clt_indx)->ipv4_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv4_set;
		get_client_memptr(wlan_client, clt_indx)->ipv6_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv6_set;
		get_client_memptr(wlan_client, clt_indx)->ipv4_header_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv4_header_set;
		get_client_memptr(wlan_client, clt_indx)->ipv6_header_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv6_header_set;

		get_client_memptr(wlan_client, clt_indx)->route_rule_set_v4 = get_client_memptr(wlan_client, (clt_indx + 1))->route_rule_set_v4;
		get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6 = get_client_memptr(wlan_client, (clt_indx + 1))->route_rule_set_v6;

                for(num_v6=0;num_v6< get_client_memptr(wlan_client, clt_indx)->ipv6_set;num_v6++)
	        {
		    get_client_memptr(wlan_client, clt_indx)->v6_addr[num_v6][0] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[num_v6][0];
		    get_client_memptr(wlan_client, clt_indx)->v6_addr[num_v6][1] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[num_v6][1];
		    get_client_memptr(wlan_client, clt_indx)->v6_addr[num_v6][2] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[num_v6][2];
		    get_client_memptr(wlan_client, clt_indx)->v6_addr[num_v6][3] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[num_v6][3];
                }

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			get_client_memptr(wlan_client, clt_indx)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4 =
				 get_client_memptr(wlan_client, (clt_indx + 1))->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4;

			for(num_v6=0;num_v6< get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6;num_v6++)
			{
			  get_client_memptr(wlan_client, clt_indx)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6[num_v6] =
			   	 get_client_memptr(wlan_client, (clt_indx + 1))->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6[num_v6];
			  get_client_memptr(wlan_client, clt_indx)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6_wan[num_v6] =
			   	 get_client_memptr(wlan_client, (clt_indx + 1))->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6_wan[num_v6];
		    }
		}
	}

	IPACMDBG(" %d wifi client deleted successfully \n", num_wifi_client);
	num_wifi_client = num_wifi_client - 1;
	IPACM_Wlan::total_num_wifi_clients = IPACM_Wlan::total_num_wifi_clients - 1;
	IPACMDBG(" Number of wifi client: %d\n", num_wifi_client);

	return IPACM_SUCCESS;
}

/*handle wlan iface down event*/
int IPACM_Wlan::handle_down_evt()
{
	int res = IPACM_SUCCESS, i;

	IPACMDBG("WLAN ip-type: %d \n", ip_type);
	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		IPACMERR("Invalid iptype: 0x%x\n", ip_type);
		goto fail;
	}

	/* Delete v4 filtering rules */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
	{
		IPACMDBG("Delete default v4 filter rules\n");
		/* delete default filter rules */
		for(i=0; i<IPV4_DEFAULT_FILTERTING_RULES; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v4, dft_v4fl_rule_hdl[i]) == IPACM_FAILURE)
			{
				IPACMERR("Error deleting dft IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#ifdef CT_OPT
		IPACMDBG("Delete tcp control flt rules.\n");
		/* Delete tcp control flt rules */
		for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v4, tcp_ctl_flt_rule_hdl_v4[i]) == IPACM_FAILURE)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#endif
		IPACMDBG("Delete lan2lan v4 flt rules.\n");
		/* delete lan2lan ipv4 flt rules */
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v4, lan2lan_flt_rule_hdl_v4[i].rule_hdl) == IPACM_FAILURE)
			{
				IPACMERR("Error deleting lan2lan IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}

		IPACMDBG("Delete private v4 filter rules\n");
		/* delete private-ipv4 filter rules */
#ifdef FEATURE_IPA_ANDROID
		for(i=0; i<IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v4, private_fl_rule_hdl[i]) == IPACM_FAILURE)
			{
				IPACMERR("Error deleting private subnet IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#else
		for(i=0; i<IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v4, private_fl_rule_hdl[i]) == IPACM_FAILURE)
			{
				IPACMERR("Error deleting private subnet IPv4 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#endif
	}

	/* Delete v6 filtering rules */
	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
	{
		IPACMDBG("Delete default %d v6 filter rules\n", IPV6_DEFAULT_FILTERTING_RULES);
		/* delete default filter rules */
		for(i=0; i<IPV6_DEFAULT_FILTERTING_RULES; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v6, dft_v6fl_rule_hdl[i]) == IPACM_FAILURE)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#ifdef CT_OPT
		IPACMDBG("Delete tcp control flt rules.\n");
		/* Delete tcp control flt rules */
		for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v6, tcp_ctl_flt_rule_hdl_v6[i]) == IPACM_FAILURE)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}
#endif
		IPACMDBG("Delete lan2lan v6 filter rules\n");
		/* delete lan2lan ipv6 filter rules */
		for(i=0; i<MAX_OFFLOAD_PAIR; i++)
		{
			if(reset_to_dummy_flt_rule(IPA_IP_v6, lan2lan_flt_rule_hdl_v6[i].rule_hdl) == IPACM_FAILURE)
			{
				IPACMERR("Error deleting lan2lan IPv6 flt rules.\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}
	IPACMDBG("finished delete default filtering rules\n ");

	/* Delete default v4 RT rule */
	if (ip_type != IPA_IP_v6)
	{
		IPACMDBG("Delete default v4 routing rules\n");
		if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[0], IPA_IP_v4)
				== false)
		{
			IPACMERR("Routing rule deletion failed!\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}

	/* Delete default v6 RT rule */
	if (ip_type != IPA_IP_v4)
	{
		IPACMDBG("Delete default v6 routing rules\n");
		/* May have multiple ipv6 iface-RT rules */
		for (i = 0; i < 2*num_dft_rt_v6; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[MAX_DEFAULT_v4_ROUTE_RULES+i], IPA_IP_v6)
					== false)
			{
				IPACMERR("Routing rule deletion failed!\n");
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	}
	IPACMDBG("finished deleting default RT rules\n ");


	/* delete wan filter rule */
	if (IPACM_Wan::isWanUP() && rx_prop != NULL)
	{
		IPACMDBG("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
		IPACM_Lan::handle_wan_down(IPACM_Wan::backhaul_is_sta_mode);
	}

	if (IPACM_Wan::isWanUP_V6() && rx_prop != NULL)
	{
		IPACMDBG("LAN IF goes down, backhaul type %d\n", IPACM_Wan::backhaul_is_sta_mode);
		IPACM_Lan::handle_wan_down_v6(IPACM_Wan::backhaul_is_sta_mode);
	}

	IPACMDBG("finished deleting wan filtering rules\n ");


	/* check software routing fl rule hdl */
	if (softwarerouting_act == true && rx_prop != NULL )
	{
		IPACMDBG("Delete sw routing filtering rules\n");
		IPACM_Iface::handle_software_routing_disable();
	}
	IPACMDBG("finished delete software-routing filtering rules\n ");


	/* clean wifi-client header, routing rules */
	/* clean wifi client rule*/
	IPACMDBG("left %d wifi clients need to be deleted \n ", num_wifi_client);

	for (i = 0; i < num_wifi_client; i++)
	{
		delete_default_qos_rtrules(i, IPA_IP_v4);
		delete_default_qos_rtrules(i, IPA_IP_v6);

		IPACMDBG("Delete %d client header\n", num_wifi_client);

		handle_lan2lan_msg_post(get_client_memptr(wlan_client, i)->mac, IPA_LAN_CLIENT_DISCONNECT);

        if(get_client_memptr(wlan_client, i)->ipv4_header_set == true)
        {
			if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, i)->hdr_hdl_v4)
				== false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}

        if(get_client_memptr(wlan_client, i)->ipv6_header_set == true)
        {
			if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, i)->hdr_hdl_v6)
					== false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}
		}
	} /* end of for loop */

	/* free the wlan clients cache */
	IPACMDBG("Free wlan clients cache\n");

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
	IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);
		free(rx_prop);
	}

	for (i = 0; i < num_wifi_client; i++)
	{
		free(get_client_memptr(wlan_client, i)->p_hdr_info);
	}
	free(wlan_client);
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

/*handle lan2lan internal mesg posting*/
int IPACM_Wlan::handle_lan2lan_msg_post(uint8_t *mac_addr, ipa_cm_event_id event)
{
	int client_index;
	client_index = get_wlan_client_index(mac_addr);
	if (client_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not attached\n");
		return IPACM_SUCCESS;
	}

	ipacm_event_lan_client* lan_client;
	ipacm_cmd_q_data evt_data;
	if(get_client_memptr(wlan_client, client_index)->ipv4_set == true) /* handle ipv4 case*/
	{
		if(ip_type != IPA_IP_v4 && ip_type != IPA_IP_MAX)
		{
			IPACMERR("Client has IPv4 addr but iface does not have IPv4 up.\n");
			return IPACM_FAILURE;
		}

		lan_client = (ipacm_event_lan_client*)malloc(sizeof(ipacm_event_lan_client));
		if(lan_client == NULL)
		{
			IPACMERR("Unable to allocate memory.\n");
			return IPACM_FAILURE;
		}
		memset(lan_client, 0, sizeof(ipacm_event_lan_client));

		lan_client->iptype = IPA_IP_v4;
		lan_client->ipv4_addr = get_client_memptr(wlan_client, client_index)->v4_addr;
		lan_client->p_iface = this;

		memset(&evt_data, 0, sizeof(evt_data));
		evt_data.event = event;
		evt_data.evt_data = (void*)lan_client;

		IPACMDBG("Posting event: %d\n",event);
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}

	if(get_client_memptr(wlan_client, client_index)->ipv6_set > 0) /* handle v6 case: may be multiple v6 addr */
	{
		if(ip_type != IPA_IP_v6 && ip_type != IPA_IP_MAX)
		{
			IPACMERR("Client has IPv6 addr but iface does not have IPv6 up.\n");
			return IPACM_FAILURE;
		}
		int i;

		for(i=0; i<get_client_memptr(wlan_client, client_index)->ipv6_set; i++)
		{
			lan_client = (ipacm_event_lan_client*)malloc(sizeof(ipacm_event_lan_client));
			if(lan_client == NULL)
			{
				IPACMERR("Unable to allocate memory.\n");
				return IPACM_FAILURE;
			}
			memset(lan_client, 0, sizeof(ipacm_event_lan_client));

			lan_client->iptype = IPA_IP_v6;
			memcpy(lan_client->ipv6_addr, get_client_memptr(wlan_client, client_index)->v6_addr[i], 4*sizeof(uint32_t));
			lan_client->p_iface = this;

			memset(&evt_data, 0, sizeof(evt_data));
			evt_data.event = event;
			evt_data.evt_data = (void*)lan_client;

			IPACMDBG("Posting event: %d\n",event);
			IPACM_EvtDispatcher::PostEvt(&evt_data);
		}
	}
	return IPACM_SUCCESS;
}

int IPACM_Wlan::add_lan2lan_hdr(ipa_ip_type iptype, uint8_t* src_mac, uint8_t* dst_mac, uint32_t* hdr_hdl)
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

	int i, j, k, len;
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

				for(j=0; j<num_wifi_client; j++)	//Add src/dst mac to the header
				{
					if(memcmp(dst_mac, get_client_memptr(wlan_client, j)->mac, IPA_MAC_ADDR_SIZE) == 0)
					{
						break;
					}
				}
				if(j == num_wifi_client)
				{
					IPACMERR("Not able to find the wifi client from mac addr.\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACMDBG("Find wifi client at position %d\n", j);
					for(k = 0; k < get_client_memptr(wlan_client, j)->p_hdr_info->num_of_attribs; k++)
					{
						if(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].attrib_type == WLAN_HDR_ATTRIB_MAC_ADDR)
						{
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset],
									dst_mac, IPA_MAC_ADDR_SIZE);
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset + IPA_MAC_ADDR_SIZE],
									src_mac, IPA_MAC_ADDR_SIZE);
						}
						else if(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].attrib_type == WLAN_HDR_ATTRIB_STA_ID)
						{
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset],
									&get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].u.sta_id,
									sizeof(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].u.sta_id));
						}
						else
						{
							IPACMDBG("The attribute type is not expected!\n");
						}
					}
				}

				pHeader->commit = true;
				pHeader->num_hdrs = 1;

				memset(pHeader->hdr[0].name, 0, sizeof(pHeader->hdr[0].name));
				strlcpy(pHeader->hdr[0].name, IPA_LAN_TO_LAN_WLAN_HDR_NAME_V4, sizeof(pHeader->hdr[0].name));

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

				for(j=0; j<num_wifi_client; j++)	//Add src/dst mac to the header
				{
					if(memcmp(dst_mac, get_client_memptr(wlan_client, j)->mac, IPA_MAC_ADDR_SIZE) == 0)
					{
						break;
					}
				}
				if(j == num_wifi_client)
				{
					IPACMERR("Not able to find the wifi client from mac addr.\n");
					res = IPACM_FAILURE;
					goto fail;
				}
				else
				{
					IPACMDBG("Find wifi client at position %d\n", j);
					for(k = 0; k < get_client_memptr(wlan_client, j)->p_hdr_info->num_of_attribs; k++)
					{
						if(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].attrib_type == WLAN_HDR_ATTRIB_MAC_ADDR)
						{
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset],
									dst_mac, IPA_MAC_ADDR_SIZE);
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset + IPA_MAC_ADDR_SIZE],
									src_mac, IPA_MAC_ADDR_SIZE);
						}
						else if(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].attrib_type == WLAN_HDR_ATTRIB_STA_ID)
						{
							memcpy(&pHeader->hdr[0].hdr[get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].offset],
									&get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].u.sta_id,
									sizeof(get_client_memptr(wlan_client, j)->p_hdr_info->attribs[k].u.sta_id));
						}
						else
						{
							IPACMDBG("The attribute type is not expected!\n");
						}
					}
				}

				pHeader->commit = true;
				pHeader->num_hdrs = 1;

				memset(pHeader->hdr[0].name, 0, sizeof(pHeader->hdr[0].name));
				strlcpy(pHeader->hdr[0].name, IPA_LAN_TO_LAN_WLAN_HDR_NAME_V6, sizeof(pHeader->hdr[0].name));


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

/* add dummy filtering rules for WLAN AP-AP mode support */
void IPACM_Wlan::add_dummy_flt_rule()
{
	int num_v4_dummy_rule, num_v6_dummy_rule;

	if(IPACM_Wlan::num_wlan_ap_iface == 1)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 != NULL || IPACM_Wlan::dummy_flt_rule_hdl_v6 != NULL)
		{
			IPACMERR("Either v4 or v6 dummy filtering rule handle is not empty.\n");
			return;
		}
#ifndef CT_OPT
		num_v4_dummy_rule = 2*(IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		num_v6_dummy_rule = 2*(IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR);
#else
		num_v4_dummy_rule = 2*(IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		num_v6_dummy_rule = 2*(IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR);
#endif

#ifdef FEATURE_IPA_ANDROID
		num_v4_dummy_rule = num_v4_dummy_rule - 2* IPACM_Iface::ipacmcfg->ipa_num_private_subnet + 2 * IPA_MAX_PRIVATE_SUBNET_ENTRIES;
#endif

		IPACM_Wlan::dummy_flt_rule_hdl_v4 = (uint32_t*)malloc(num_v4_dummy_rule * sizeof(uint32_t));
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Failed to allocate memory.\n");
			return;
		}
		IPACM_Wlan::dummy_flt_rule_hdl_v6 = (uint32_t*)malloc(num_v6_dummy_rule * sizeof(uint32_t));
		if(IPACM_Wlan::dummy_flt_rule_hdl_v6 == NULL)
		{
			IPACMERR("Failed to allocate memory.\n");
			free(IPACM_Wlan::dummy_flt_rule_hdl_v4);
			IPACM_Wlan::dummy_flt_rule_hdl_v4 = NULL;
			return;
		}
		memset(IPACM_Wlan::dummy_flt_rule_hdl_v4, 0, num_v4_dummy_rule * sizeof(uint32_t));
		memset(IPACM_Wlan::dummy_flt_rule_hdl_v6, 0, num_v6_dummy_rule * sizeof(uint32_t));

		install_dummy_flt_rule(IPA_IP_v4, num_v4_dummy_rule);
		install_dummy_flt_rule(IPA_IP_v6, num_v6_dummy_rule);
	}
	return;
}

/* install dummy filtering rules for WLAN AP-AP mode support */
int IPACM_Wlan::install_dummy_flt_rule(ipa_ip_type iptype, int num_rule)
{
	if(rx_prop == NULL)
	{
		IPACMDBG("There is no rx_prop for iface %s, not able to add dummy filtering rule.\n", dev_name);
		return IPACM_FAILURE;
	}

	int i, len, res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule;
	ipa_ioc_add_flt_rule* pFilteringTable;

	len = sizeof(struct ipa_ioc_add_flt_rule) + num_rule * sizeof(struct ipa_flt_rule_add);

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
	pFilteringTable->num_rules = num_rule;

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

		for(i=0; i<num_rule; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy ipv4 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			/* copy filter rule hdls */
			for (int i = 0; i < num_rule; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					IPACM_Wlan::dummy_flt_rule_hdl_v4[i] = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Dummy v4 flt rule %d hdl:0x%x\n", i, IPACM_Wlan::dummy_flt_rule_hdl_v4[i]);
				}
				else
				{
					IPACMERR("Failed adding dummy v4 flt rule %d\n", i);
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

		for(i=0; i<num_rule; i++)
		{
			memcpy(&(pFilteringTable->rules[i]), &flt_rule, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(pFilteringTable))
		{
			IPACMERR("Error adding dummy ipv6 flt rule\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			/* copy filter rule hdls */
			for (int i = 0; i < num_rule; i++)
			{
				if (pFilteringTable->rules[i].status == 0)
				{
					IPACM_Wlan::dummy_flt_rule_hdl_v6[i] = pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Lan2lan v6 flt rule %d hdl:0x%x\n", i, IPACM_Wlan::dummy_flt_rule_hdl_v6[i]);
				}
				else
				{
					IPACMERR("Failed adding v6 flt rule %d\n", i);
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

/* delete dummy flt rule for WLAN AP-AP mode support*/
void IPACM_Wlan::del_dummy_flt_rule()
{
	int num_v4_dummy_rule, num_v6_dummy_rule;

	if(IPACM_Wlan::num_wlan_ap_iface == 0)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL || IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Either v4 or v6 dummy flt rule is empty.\n");
			return;
		}
#ifndef CT_OPT
		num_v4_dummy_rule = 2*(IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		num_v6_dummy_rule = 2*(IPV6_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR);
#else
		num_v4_dummy_rule = 2*(IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
		num_v6_dummy_rule = 2*(IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR);
#endif

#ifdef FEATURE_IPA_ANDROID
		num_v4_dummy_rule = num_v4_dummy_rule - 2* IPACM_Iface::ipacmcfg->ipa_num_private_subnet + 2 * IPA_MAX_PRIVATE_SUBNET_ENTRIES;
#endif

		if(m_filtering.DeleteFilteringHdls(IPACM_Wlan::dummy_flt_rule_hdl_v4, IPA_IP_v4, num_v4_dummy_rule) == false)
		{
			IPACMERR("Failed to delete ipv4 dummy flt rules.\n");
			return;
		}
		if(m_filtering.DeleteFilteringHdls(IPACM_Wlan::dummy_flt_rule_hdl_v6, IPA_IP_v6, num_v6_dummy_rule) == false)
		{
			IPACMERR("Failed to delete ipv6 dummy flt rules.\n");
			return;
		}

		free(IPACM_Wlan::dummy_flt_rule_hdl_v4);
		IPACM_Wlan::dummy_flt_rule_hdl_v4 = NULL;
		free(IPACM_Wlan::dummy_flt_rule_hdl_v6);
		IPACM_Wlan::dummy_flt_rule_hdl_v6 = NULL;
	}
	return;
}

void IPACM_Wlan::install_tcp_ctl_flt_rule(ipa_ip_type iptype)
{
	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return;
	}

	int i, len, res = IPACM_SUCCESS, offset;
	struct ipa_flt_rule_mdfy flt_rule;
	struct ipa_ioc_mdfy_flt_rule* pFilteringTable;

	if (iptype == IPA_IP_v4)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Dummy ipv4 flt rule has not been installed.\n");
			return;
		}
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#ifdef FEATURE_IPA_ANDROID
		offset = offset + wlan_ap_index * (IPA_MAX_PRIVATE_SUBNET_ENTRIES - IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
	}
	else
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v6 == NULL)
		{
			IPACMERR("Dummy ipv6 flt rule has not been installed.\n");
			return;
		}
		offset = wlan_ap_index * (IPV6_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR);
	}

	len = sizeof(struct ipa_ioc_mdfy_flt_rule) + NUM_TCP_CTL_FLT_RULE * sizeof(struct ipa_flt_rule_mdfy);
	pFilteringTable = (struct ipa_ioc_mdfy_flt_rule*)malloc(len);
	if (!pFilteringTable)
	{
		IPACMERR("Failed to allocate ipa_ioc_mdfy_flt_rule memory...\n");
		return;
	}
	memset(pFilteringTable, 0, len);

	pFilteringTable->commit = 1;
	pFilteringTable->ip = iptype;
	pFilteringTable->num_rules = NUM_TCP_CTL_FLT_RULE;

	memset(&flt_rule, 0, sizeof(struct ipa_flt_rule_mdfy));
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

	/* add TCP FIN rule*/
	flt_rule.rule.eq_attrib.rule_eq_bitmap |= (1<<8);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].offset = 12;
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_FIN_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_FIN_SHIFT);
	flt_rule.rule.eq_attrib.num_ihl_offset_meq_32 = 1;
	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset];
	}
	else
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset];
	}
	memcpy(&(pFilteringTable->rules[0]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

	/* add TCP SYN rule*/
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_SYN_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_SYN_SHIFT);
	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+1];
	}
	else
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset+1];
	}
	memcpy(&(pFilteringTable->rules[1]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

	/* add TCP RST rule*/
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].value = (((uint32_t)1)<<TCP_RST_SHIFT);
	flt_rule.rule.eq_attrib.ihl_offset_meq_32[0].mask = (((uint32_t)1)<<TCP_RST_SHIFT);
	if(iptype == IPA_IP_v4)
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+2];
	}
	else
	{
		flt_rule.rule_hdl = IPACM_Wlan::dummy_flt_rule_hdl_v6[offset+2];
	}
	memcpy(&(pFilteringTable->rules[2]), &flt_rule, sizeof(struct ipa_flt_rule_mdfy));

	if (false == m_filtering.ModifyFilteringRule(pFilteringTable))
	{
		IPACMERR("Failed to modify tcp control filtering rules.\n");
		goto fail;
	}
	else
	{
		if(iptype == IPA_IP_v4)
		{
			for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
			{
				tcp_ctl_flt_rule_hdl_v4[i] = pFilteringTable->rules[i].rule_hdl;
			}
		}
		else
		{
			for(i=0; i<NUM_TCP_CTL_FLT_RULE; i++)
			{
				tcp_ctl_flt_rule_hdl_v6[i] = pFilteringTable->rules[i].rule_hdl;
			}
		}
	}

fail:
	free(pFilteringTable);
	return;
}

int IPACM_Wlan::add_dummy_private_subnet_flt_rule(ipa_ip_type iptype)
{
	if(rx_prop == NULL)
	{
		IPACMDBG("There is no rx_prop for iface %s, not able to add dummy lan2lan filtering rule.\n", dev_name);
		return IPACM_FAILURE;
	}

	int offset;
	if(iptype == IPA_IP_v4)
	{
		if(IPACM_Wlan::dummy_flt_rule_hdl_v4 == NULL)
		{
			IPACMERR("Dummy ipv4 flt rule has not been installed.\n");
			return IPACM_FAILURE;
		}

#ifndef CT_OPT
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
						+ IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR;
#else
		offset = wlan_ap_index * (IPV4_DEFAULT_FILTERTING_RULES + NUM_TCP_CTL_FLT_RULE + MAX_OFFLOAD_PAIR + IPACM_Iface::ipacmcfg->ipa_num_private_subnet)
						+ IPV4_DEFAULT_FILTERTING_RULES + MAX_OFFLOAD_PAIR + NUM_TCP_CTL_FLT_RULE;
#endif

#ifdef FEATURE_IPA_ANDROID
		offset = offset + wlan_ap_index * (IPA_MAX_PRIVATE_SUBNET_ENTRIES - IPACM_Iface::ipacmcfg->ipa_num_private_subnet);
#endif
		for (int i = 0; i < IPA_MAX_PRIVATE_SUBNET_ENTRIES; i++)
		{
			private_fl_rule_hdl[i] = IPACM_Wlan::dummy_flt_rule_hdl_v4[offset+i];
			IPACMDBG("Private subnet v4 flt rule %d hdl:0x%x\n", i, private_fl_rule_hdl[i]);
		}
	}
	return IPACM_SUCCESS;
}