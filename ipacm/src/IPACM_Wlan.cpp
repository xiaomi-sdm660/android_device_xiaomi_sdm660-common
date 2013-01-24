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

IPACM_Wlan::IPACM_Wlan(int iface_index) : IPACM_Lan(iface_index)
{
#define WLAN_AMPDU_DEFAULT_FILTER_RULES 3

	num_wifi_client = 0;
	header_name_count = 0;

	/* one for WAN and two for Soft-routing */
	wlan_ampdu_flt_rule.num_rules = WLAN_AMPDU_DEFAULT_FILTER_RULES;

	wlan_ampdu_flt_rule.hdl[0] = 0;            /* for WAN */
	wlan_ampdu_flt_rule.ip[0] = IPA_IP_v4;     /* for WAN */

	wlan_ampdu_flt_rule.hdl[1] = 0;           /* for Soft-routing */
	wlan_ampdu_flt_rule.ip[1] = IPA_IP_v4;    /* for Soft-routing */

	wlan_ampdu_flt_rule.hdl[2] = 0;            /* for Soft-routing */
	wlan_ampdu_flt_rule.ip[2] = IPA_IP_v6;     /* for Soft-routing */

	wlan_client_len = (sizeof(ipa_wlan_client)) + (iface_query->num_tx_props * sizeof(wlan_client_rt_hdl));
	wlan_client = (ipa_wlan_client *)calloc(IPA_MAX_NUM_WIFI_CLIENTS, wlan_client_len);
	if (wlan_client == NULL)
	{
		IPACMERR("unable to allocate memory\n");
		return;
	}

	IPACMDBG("index:%d constructor: Tx properties:%d\n", iface_index, iface_query->num_tx_props);
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
				IPACMDBG("ipa_WLAN (%s):ipa_index (%d) instance close \n",
								 IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
				delete this;
				return;
			}
		}
		break;

	case IPA_ADDR_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);

			if (ipa_interface_index == ipa_if_num)
			{
				if ((data->iptype != ip_type) && (ip_type != IPA_IP_MAX)) // check not setup before
				{
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

					IPACM_Lan::handle_addr_evt(data);
					handle_addr_evt(data);

					IPACM_Lan::handle_private_subnet(data->iptype);
					handle_private_subnet(data->iptype);

					if (IPACM_Wan::isWanUP() && (data->iptype == IPA_IP_v4))
					{
						IPACM_Lan::handle_wan_up();
						handle_wan_up();
					}

					IPACMDBG("posting IPA_HANDLE_WLAN_UP:Finished checking wan_up\n");
				}
			}
		}
		break;

	case IPA_HANDLE_WAN_UP:
		IPACMDBG("Received IPA_HANDLE_WAN_UP event\n");
		IPACM_Lan::handle_wan_up();
		handle_wan_up();
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		IPACM_Lan::handle_wan_down();
		handle_wan_down();
		break;

	case IPA_WLAN_CLIENT_ADD_EVENT:
		{
			ipacm_event_data_mac *data = (ipacm_event_data_mac *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_WLAN_CLIENT_ADD_EVENT\n");
				handle_wlan_client_init(data->mac_addr);
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
				if (ip_type != IPA_IP_v6) /* for ipv4 */
				{
					handle_wlan_client_route_rule(data->mac_addr, IPA_IP_v4);
				}

				if (ip_type != IPA_IP_v4) /* for ipv6 */
				{
					handle_wlan_client_route_rule(data->mac_addr, IPA_IP_v6);
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
				handle_wlan_client_route_rule(data->mac_addr, data->iptype);
			}
		}
		break;

		/* handle software routing enable event*/
	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG("Received IPA_SW_ROUTING_ENABLE\n");
		IPACM_Iface::handle_software_routing_enable();
		handle_software_routing_enable();
		break;

		/* handle software routing disable event*/
	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG("Received IPA_SW_ROUTING_DISABLE\n");
		IPACM_Iface::handle_software_routing_disable();
		handle_software_routing_disable();
		break;

	default:
		break;
	}
	return;
}

/* handle wifi client initial,copy all partial headers (tx property) */
int IPACM_Wlan::handle_wlan_client_init(uint8_t *mac_addr)
{

#define WLAN_IFACE_INDEX_LEN 2

	int res = IPACM_SUCCESS, len = 0;
	char index[WLAN_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;

	/* start of adding header */
	if ((num_wifi_client >= IPA_MAX_NUM_WIFI_CLIENTS) ||
			(IPACM_Wlan::total_num_wifi_clients >= IPA_MAX_NUM_WIFI_CLIENTS))
	{
		IPACMERR("Reached maximum number of wlan clients\n");
		return IPACM_FAILURE;
	}

	IPACMDBG("Wifi client number: %d\n", num_wifi_client);

	IPACMDBG("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	memcpy(get_client_memptr(wlan_client, num_wifi_client)->mac,
				 mac_addr,
				 sizeof(get_client_memptr(wlan_client, num_wifi_client)->mac));

	IPACMDBG("stored MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 get_client_memptr(wlan_client, num_wifi_client)->mac[0],
					 get_client_memptr(wlan_client, num_wifi_client)->mac[1],
					 get_client_memptr(wlan_client, num_wifi_client)->mac[2],
					 get_client_memptr(wlan_client, num_wifi_client)->mac[3],
					 get_client_memptr(wlan_client, num_wifi_client)->mac[4],
					 get_client_memptr(wlan_client, num_wifi_client)->mac[5]);

	/* add header to IPA */
	len = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)calloc(1, len);
	if (pHeaderDescriptor == NULL)
	{
		IPACMERR("calloc failed to allocate pHeaderDescriptor\n");
		return IPACM_FAILURE;
	}

	/* copy partial header */
	memset(&sCopyHeader, 0, sizeof(sCopyHeader));
	memcpy(sCopyHeader.name,
				 tx_prop->tx[0].hdr_name,
				 sizeof(sCopyHeader.name));

	IPACMDBG("header name: %s\n", sCopyHeader.name);
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

	/* copy client mac_addr to partial header */
	memcpy(&pHeaderDescriptor->hdr[0].hdr[IPA_WLAN_PARTIAL_HDR_OFFSET],
				 get_client_memptr(wlan_client, num_wifi_client)->mac,
				 IPA_MAC_ADDR_SIZE);

	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memset(pHeaderDescriptor->hdr[0].name, 0,
				 sizeof(pHeaderDescriptor->hdr[0].name));

	sprintf(index, "%d", ipa_if_num);
	strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));

	strncat(pHeaderDescriptor->hdr[0].name,
					IPA_WLAN_PARTIAL_HDR_NAME,
					sizeof(IPA_WLAN_PARTIAL_HDR_NAME));

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

	get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl = pHeaderDescriptor->hdr[0].hdr_hdl;
	IPACMDBG("client(%d) full header name:%s header handle:(0x%x)\n",
					 num_wifi_client,
					 pHeaderDescriptor->hdr[0].name,
					 get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl);
	/* initialize wifi client*/
	get_client_memptr(wlan_client, num_wifi_client)->route_rule_set_v4 = false;
	get_client_memptr(wlan_client, num_wifi_client)->route_rule_set_v6 = false;
	get_client_memptr(wlan_client, num_wifi_client)->ipv4_set = false;
	get_client_memptr(wlan_client, num_wifi_client)->ipv6_set = false;

	num_wifi_client++;
	header_name_count++; //keep increasing header_name_count
	IPACM_Wlan::total_num_wifi_clients++;
	res = IPACM_SUCCESS;

	IPACMDBG("Wifi client number: %d\n", num_wifi_client);

fail:
	free(pHeaderDescriptor);

	return res;
}

/*handle wifi client */
int IPACM_Wlan::handle_wlan_client_ipaddr(ipacm_event_data_all *data)
{
	int clnt_indx;

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
		if (clnt_indx == IPACM_INVALID_INDEX)
		{
			IPACMDBG("wlan client not found/attached \n");
			return IPACM_FAILURE;
		}
	}

	IPACMDBG("Ip type received %d\n", data->iptype);
	if (data->iptype == IPA_IP_v4)
	{
		IPACMDBG("Ipv4 with ipv4 address: 0x%x\n", data->ipv4_addr);
		if (data->ipv4_addr != 0) /* not 0.0.0.0 */
		{
			if (get_client_memptr(wlan_client, clnt_indx)->ipv4_set == false)
			{
				get_client_memptr(wlan_client, clnt_indx)->v4_addr = data->ipv4_addr;
				get_client_memptr(wlan_client, clnt_indx)->ipv4_set = true;
			}
			else
			{
				IPACMDBG("Already setup ipv4 addr for client:%d\n", clnt_indx);
			}
		}
	}
	else
	{
		if ((data->ipv6_addr[0] != 0) || (data->ipv6_addr[1] != 0) ||
				(data->ipv6_addr[2] != 0) || (data->ipv6_addr[3] || 0)) /* check if all 0 not valid ipv6 address */
		{
			if (get_client_memptr(wlan_client, clnt_indx)->ipv6_set == false)
			{
				get_client_memptr(wlan_client, clnt_indx)->v6_addr[0] = data->ipv6_addr[0];
				get_client_memptr(wlan_client, clnt_indx)->v6_addr[1] = data->ipv6_addr[1];
				get_client_memptr(wlan_client, clnt_indx)->v6_addr[2] = data->ipv6_addr[2];
				get_client_memptr(wlan_client, clnt_indx)->v6_addr[3] = data->ipv6_addr[3];
				get_client_memptr(wlan_client, clnt_indx)->ipv6_set = true;
			}
			else
			{
				IPACMDBG("Already setup ipv6 addr for client:%d\n", clnt_indx);
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
	int wlan_index;

	IPACMDBG("Received mac_addr MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

	wlan_index = get_wlan_client_index(mac_addr);
	if (wlan_index == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not found/attached \n");
		return IPACM_SUCCESS;
	}

	IPACMDBG("wlan client index: %d, ip-type: %d, ipv4_set:%d, ipv4_rule_set:%d \n", wlan_index, iptype,
					 get_client_memptr(wlan_client, wlan_index)->ipv4_set,
					 get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4);
	/* Add default 4 Qos routing rules if not set yet */
	if ((iptype == IPA_IP_v4
			 && get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4 == false
			 && get_client_memptr(wlan_client, wlan_index)->ipv4_set == true)
			|| (iptype == IPA_IP_v6
					&& get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6 == false
					&& get_client_memptr(wlan_client, wlan_index)->ipv6_set == true))
	{
		IPACMDBG("client index(%d):ipv4 address: 0x%x\n", wlan_index, 
						        get_client_memptr(wlan_client, wlan_index)->v4_addr);

		IPACMDBG("client(%d): header handle:(0x%x), num of rules: %d\n",
						 wlan_index,
						 get_client_memptr(wlan_client, wlan_index)->hdr_hdl,
						 iface_query->num_tx_props);


		rt_rule = (struct ipa_ioc_add_rt_rule *)
			 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
							(iface_query->num_tx_props * sizeof(struct ipa_rt_rule_add)));

		if (rt_rule == NULL)
		{
			PERROR("Error Locate ipa_ioc_add_rt_rule memory...\n");
			return IPACM_FAILURE;
		}

		rt_rule->commit = 1;
		rt_rule->num_rules = iface_query->num_tx_props;
		rt_rule->ip = iptype;

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if (iptype == IPA_IP_v4)
			{
				strncpy(rt_rule->rt_tbl_name,
								IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name,
								sizeof(rt_rule->rt_tbl_name));
			}
			else
			{
				strncpy(rt_rule->rt_tbl_name,
								IPACM_Iface::ipacmcfg->rt_tbl_v6.name,
								sizeof(rt_rule->rt_tbl_name));
			}

			rt_rule_entry = &rt_rule->rules[tx_index];
			rt_rule_entry->at_rear = 0;
			rt_rule_entry->rule.dst = tx_prop->tx[tx_index].dst_pipe;
			rt_rule_entry->rule.hdr_hdl = get_client_memptr(wlan_client, wlan_index)->hdr_hdl;

			memcpy(&rt_rule_entry->rule.attrib,
						 &tx_prop->tx[tx_index].attrib,
						 sizeof(rt_rule_entry->rule.attrib));
			rt_rule_entry->rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;

			if (iptype == IPA_IP_v4)
			{
				rt_rule_entry->rule.attrib.u.v4.dst_addr = get_client_memptr(wlan_client, wlan_index)->v4_addr;
				rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
			}
			else if (iptype == IPA_IP_v6)
			{
				rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = get_client_memptr(wlan_client, wlan_index)->v6_addr[0];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = get_client_memptr(wlan_client, wlan_index)->v6_addr[1];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = get_client_memptr(wlan_client, wlan_index)->v6_addr[2];
				rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = get_client_memptr(wlan_client, wlan_index)->v6_addr[3];
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0xFFFFFFFF;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0xFFFFFFFF;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0xFFFFFFFF;
				rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0xFFFFFFFF;
			}
		} /* end of for loop */

		if (false == m_routing.AddRoutingRule(rt_rule))
		{
			IPACMERR("Routing rule addition failed!\n");
			free(rt_rule);
			return IPACM_FAILURE;
		}

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if (iptype == IPA_IP_v4)
			{
				get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4 =
					 rt_rule->rules[tx_index].rt_rule_hdl;
				IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index, 
								 get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4, iptype);
			}
			else if (iptype == IPA_IP_v6)
			{
				get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6 =
					 rt_rule->rules[tx_index].rt_rule_hdl;
				IPACMDBG("tx:%d, rt rule hdl=%x ip-type: %d\n", tx_index, 
								 get_client_memptr(wlan_client, wlan_index)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6, iptype);
			}
		}

		free(rt_rule);

		if (iptype == IPA_IP_v4)
		{
			get_client_memptr(wlan_client, wlan_index)->route_rule_set_v4 = true;
		}
		else
		{
			get_client_memptr(wlan_client, wlan_index)->route_rule_set_v6 = true;
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

	/*check if got duplicate power-save mode*/
	if ((ip_type != IPA_IP_v6 && get_client_memptr(wlan_client, clt_indx)->route_rule_set_v4 == true)
			|| (ip_type != IPA_IP_v4 && get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6 == true))
	{
		return delete_default_qos_rtrules(clt_indx);
	}
	else
	{
		IPACMDBG("wlan client already in power-save mode\n");
		return IPACM_SUCCESS;
	}
}

/*handle wifi client del mode*/
int IPACM_Wlan::handle_wlan_client_down_evt(uint8_t *mac_addr)
{
	int clt_indx;
	uint32_t tx_index;
	int num_wifi_client_tmp = num_wifi_client;

	IPACMDBG("total client: %d\n", num_wifi_client_tmp);

	clt_indx = get_wlan_client_index(mac_addr);
	if (clt_indx == IPACM_INVALID_INDEX)
	{
		IPACMDBG("wlan client not attached\n");
		return IPACM_SUCCESS;
	}

	if (delete_default_qos_rtrules(clt_indx))
	{
		IPACMDBG("unbale to delete default qos route rules\n");
		return IPACM_FAILURE;
	}

	/* Delete wlan client header */
	if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, clt_indx)->hdr_hdl)
			== false)
	{
		return IPACM_FAILURE;
	}

	/* Reset ip_set to 0*/
	get_client_memptr(wlan_client, clt_indx)->ipv4_set = false;
	get_client_memptr(wlan_client, clt_indx)->ipv6_set = false;
	get_client_memptr(wlan_client, clt_indx)->route_rule_set_v4 = false;
	get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6 = false;

	for (; clt_indx < num_wifi_client_tmp - 1; clt_indx++)
	{
		memcpy(get_client_memptr(wlan_client, clt_indx)->mac,
					 get_client_memptr(wlan_client, (clt_indx + 1))->mac,
					 sizeof(get_client_memptr(wlan_client, clt_indx)->mac));

		get_client_memptr(wlan_client, clt_indx)->hdr_hdl = get_client_memptr(wlan_client, (clt_indx + 1))->hdr_hdl;
		get_client_memptr(wlan_client, clt_indx)->v4_addr = get_client_memptr(wlan_client, (clt_indx + 1))->v4_addr;

		get_client_memptr(wlan_client, clt_indx)->ipv4_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv4_set;
		get_client_memptr(wlan_client, clt_indx)->ipv6_set = get_client_memptr(wlan_client, (clt_indx + 1))->ipv6_set;
		get_client_memptr(wlan_client, clt_indx)->route_rule_set_v4 = get_client_memptr(wlan_client, (clt_indx + 1))->route_rule_set_v4;
		get_client_memptr(wlan_client, clt_indx)->route_rule_set_v6 = get_client_memptr(wlan_client, (clt_indx + 1))->route_rule_set_v6;

		get_client_memptr(wlan_client, clt_indx)->v6_addr[0] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[0];
		get_client_memptr(wlan_client, clt_indx)->v6_addr[1] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[1];
		get_client_memptr(wlan_client, clt_indx)->v6_addr[2] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[2];
		get_client_memptr(wlan_client, clt_indx)->v6_addr[3] = get_client_memptr(wlan_client, (clt_indx + 1))->v6_addr[3];

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			get_client_memptr(wlan_client, clt_indx)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4 =
				 get_client_memptr(wlan_client, (clt_indx + 1))->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4;

			get_client_memptr(wlan_client, clt_indx)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6 =
				 get_client_memptr(wlan_client, (clt_indx + 1))->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6;
		}
	}

	IPACMDBG(" %d wifi client deleted successfully \n", num_wifi_client);
	num_wifi_client = num_wifi_client - 1;
	IPACM_Wlan::total_num_wifi_clients = IPACM_Wlan::total_num_wifi_clients - 1;
	IPACMDBG(" Number of wifi client: %d\n", num_wifi_client);

	return IPACM_SUCCESS;
}

/* duplicate ampdu filter rules for wan_up event */
int IPACM_Wlan::handle_wan_up(void)
{
	struct ipa_flt_rule_add flt_rule_entry;
	IPACMDBG("Wlan->handle_wan_up(); \n");

	ipa_ioc_add_flt_rule *m_pFilteringTable;
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
	m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
	m_pFilteringTable->global = false;
	m_pFilteringTable->ip = IPA_IP_v4;
	m_pFilteringTable->num_rules = (uint8_t)1;

	if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4))
	{
		IPACMDBG("m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_wan_v4);
		return IPACM_FAILURE;
	}

	memset(&flt_rule_entry, 0, sizeof(ipa_flt_rule_add));
	flt_rule_entry.at_rear = true;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_SRC_NAT; //IPA_PASS_TO_ROUTING
	flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.hdl;
	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(flt_rule_entry.rule.attrib));

	/* Match-all rule */
	flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
	flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
	flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

	memcpy(&m_pFilteringTable->rules[0], &flt_rule_entry, sizeof(flt_rule_entry));
	if (m_filtering.AddFilteringRule(m_pFilteringTable) == false)
	{
		IPACMDBG("Error Adding Filtering Rule, aborting...\n");
		perror("Wlan: Unable to add filtering table");
		free(m_pFilteringTable);
		return IPACM_FAILURE;
	}
	else
	{
		IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
	}

	/* copy filter hdls  */
	wlan_ampdu_flt_rule.hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
	free(m_pFilteringTable);

	return IPACM_SUCCESS;
}


/*delete ampdu filter rules for wan_down event*/
int IPACM_Wlan::handle_wan_down(void)
{
	IPACMDBG("\n");

	if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[0],
																			IPA_IP_v4, 1) == false)
	{
		return IPACM_FAILURE;
	}
	wlan_ampdu_flt_rule.hdl[0] = 0;

	return IPACM_SUCCESS;
}

/*duplicate ampdu filter rules for software_routing event*/
int IPACM_Wlan::handle_software_routing_enable(void)
{

	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;
	int res = IPACM_SUCCESS;

	IPACMDBG("\n");

	m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
		 calloc(1,
						sizeof(struct ipa_ioc_add_flt_rule) +
						1 * sizeof(struct ipa_flt_rule_add)
						);

	if (!m_pFilteringTable)
	{
		PERROR("Error Locate ipa_ioc_add_flt_rule memory...\n");
		return IPACM_FAILURE;
	}

	m_pFilteringTable->commit = 1;
	m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
	m_pFilteringTable->global = false;
	m_pFilteringTable->num_rules = (uint8_t)1;


	/* Configuring Software_routing Filtering Rule */
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule_entry.at_rear = false;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(flt_rule_entry.rule.attrib));

	memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));


/* check iface is v4 or v6 or both*/
	if (ip_type == IPA_IP_MAX)
	{
		/* handle v4 */
		m_pFilteringTable->ip = IPA_IP_v4;
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMDBG("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		wlan_ampdu_flt_rule.hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;



		/* handle v6*/
		m_pFilteringTable->ip = IPA_IP_v6;
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMDBG("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		wlan_ampdu_flt_rule.hdl[2] = m_pFilteringTable->rules[0].flt_rule_hdl;

	}
	else
	{
		if (ip_type == IPA_IP_v4) m_pFilteringTable->ip = IPA_IP_v4;
		else m_pFilteringTable->ip = IPA_IP_v6;

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMDBG("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		if (ip_type == IPA_IP_v4) wlan_ampdu_flt_rule.hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
		else wlan_ampdu_flt_rule.hdl[2] = m_pFilteringTable->rules[0].flt_rule_hdl;
	}

fail:
	free(m_pFilteringTable);

	return res;

}

/*delete ampdu filter rules for disabling software_routing event*/
int IPACM_Wlan::handle_software_routing_disable(void)
{

	if (ip_type == IPA_IP_MAX)
	{
		/* ipv4 case */
		if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[1],
																				IPA_IP_v4, 1) == false)
		{
			return IPACM_FAILURE;
		}

		/* ipv6 case */
		if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[2],
																				IPA_IP_v6, 1) == false)
		{
			return IPACM_FAILURE;
		}

	}
	else
	{
		if (ip_type == IPA_IP_v4)
		{
			if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[1],
																					IPA_IP_v4, 1) == false)
			{
				return IPACM_FAILURE;
			}
		}
		else if (ip_type == IPA_IP_v6)
		{
			if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[2],
																					IPA_IP_v6, 1) == false)
			{
				return IPACM_FAILURE;
			}
		}
	}

	return IPACM_SUCCESS;
}


/*duplicate ampdu filter rules for initial iface configuration*/
int IPACM_Wlan::init_fl_rule(ipa_ip_type iptype)
{
	int res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("ip-type: %d\n", iptype);

	if (iptype == IPA_IP_v4)
	{
		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1,
							sizeof(struct ipa_ioc_add_flt_rule) +
							IPV4_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_add)
							);

		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}


		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = iptype;
		m_pFilteringTable->num_rules = (uint8_t)IPV4_DEFAULT_FILTERTING_RULES;

		/* Configuring Fragment Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xE0000000; /* DST_IP == 224.0.0.0 */
		memcpy(&(m_pFilteringTable->rules[1]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* Configuring Broadcast Filtering Rule */
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF; /* Filter DST_IP == 127.0.0.1 */
		memcpy(&(m_pFilteringTable->rules[2]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding RuleTable(2) to Filtering, aborting...\n");
			res = IPACM_FAILURE;
		}
		else
		{
			/* copy filter hdls */
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules] = m_pFilteringTable->rules[0].flt_rule_hdl;
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules + 1] = m_pFilteringTable->rules[1].flt_rule_hdl;
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules + 2] = m_pFilteringTable->rules[2].flt_rule_hdl;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules] = IPA_IP_v4;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules + 1] = IPA_IP_v4;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules + 2] = IPA_IP_v4;
			wlan_ampdu_flt_rule.num_rules += IPV4_DEFAULT_FILTERTING_RULES;
		}
	}
	else
	{
		/* IPv6 filter rule configuration */
		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1,
							sizeof(struct ipa_ioc_add_flt_rule) +
							IPV6_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_add)
							);

		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}
		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = iptype;
		m_pFilteringTable->num_rules = (uint8_t)IPV6_DEFAULT_FILTERTING_RULES;

		/* Configuring Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0XFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding Filtering Rule, aborting...\n");
			res = IPACM_FAILURE;
		}
		else
		{
			/* copy filter hdls */
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules] = m_pFilteringTable->rules[0].flt_rule_hdl;
			//wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules + 1] = m_pFilteringTable->rules[1].flt_rule_hdl;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules] = IPA_IP_v6;
			//wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules + 1] = IPA_IP_v6;
			wlan_ampdu_flt_rule.num_rules += IPV6_DEFAULT_FILTERTING_RULES;
		}
	}

	free(m_pFilteringTable);
	return res;
}


/*duplicate ampdu filter rules for new_address event*/
int IPACM_Wlan::handle_addr_evt(ipacm_event_data_addr *data)
{
	ipa_ioc_add_flt_rule *m_pFilteringTable;
	struct ipa_flt_rule_add flt_rule_entry;
	int NUM_RULES = 1;

	IPACMDBG(" set route/filter rule ip-type: %d \n", data->iptype);
	if (data->iptype == IPA_IP_v6)
	{
		if (num_dft_rt == 1) /*LAN handle_addr_evt will update this to 1*/
		{
			/* configure ampdu multicast/broadcast/fragment filter rule */
			init_fl_rule(data->iptype);

			/* add default v6 filter rule */
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								NUM_RULES * sizeof(struct ipa_flt_rule_add)
								);

			if (!m_pFilteringTable)
			{
				PERROR("Error Locate ipa_ioc_add_flt_rule memory...\n");
				return IPACM_FAILURE;
			}

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)NUM_RULES;

			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_v6))
			{
				PERROR("m_routing.GetRoutingTable() Failed.\n");
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
				PERROR("Error Adding RuleTable(0) to Filtering, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;

			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules] = m_pFilteringTable->rules[0].flt_rule_hdl;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules] = IPA_IP_v6;
			wlan_ampdu_flt_rule.num_rules++;
			free(m_pFilteringTable);
		}
	}
	else
	{
		init_fl_rule(data->iptype);
	}
	return IPACM_SUCCESS;
}

/*duplicate ampdu filter rules for private subnet configuration*/
int IPACM_Wlan::handle_private_subnet(ipa_ip_type iptype)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int i;

	IPACMDBG("wlan->handle_private_subnet(); set route/filter rule \n");

	if (iptype == IPA_IP_v4)
	{
		/* construct ipa_ioc_add_flt_rule with 1 rules */
		ipa_ioc_add_flt_rule *m_pFilteringTable;

		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
			 calloc(1,
							sizeof(struct ipa_ioc_add_flt_rule) +
							(IPACM_Iface::ipacmcfg->ipa_num_private_subnet) * sizeof(struct ipa_flt_rule_add)
							);

		if (!m_pFilteringTable)
		{
			PERROR("Error Locate ipa_ioc_add_flt_rule memory...\n");
			return IPACM_FAILURE;
		}

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = IPA_CLIENT_A5_WLAN_AMPDU_PROD;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = IPA_IP_v4;
		m_pFilteringTable->num_rules = (uint8_t)IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

		if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
		{
			PERROR("WLAN m_routing.GetRoutingTable(IPACM_Iface::ipacmcfg->rt_tbl_lan_v4) Failed.\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		}

		for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_private_subnet; i++)
		{
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			flt_rule_entry.at_rear = true;
			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(flt_rule_entry.rule.attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_mask;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = IPACM_Iface::ipacmcfg->private_subnet_table[i].subnet_addr;
			memcpy(&(m_pFilteringTable->rules[i]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
		}

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			PERROR("Error Adding RuleTable(0) to Filtering, aborting...\n");
			free(m_pFilteringTable);
			return IPACM_FAILURE;
		};


		/* copy filter hdls */
		for (i = 0; i < (IPACM_Iface::ipacmcfg->ipa_num_private_subnet); i++)
		{
			wlan_ampdu_flt_rule.hdl[wlan_ampdu_flt_rule.num_rules + i] = m_pFilteringTable->rules[i].flt_rule_hdl;
			wlan_ampdu_flt_rule.ip[wlan_ampdu_flt_rule.num_rules + i] = IPA_IP_v4;
		}
		wlan_ampdu_flt_rule.num_rules += IPACM_Iface::ipacmcfg->ipa_num_private_subnet;

		IPACMDBG("wlan handle private ok~\n");
		free(m_pFilteringTable);
	}
	return IPACM_SUCCESS;
}


/*handle wlan iface down event*/
int IPACM_Wlan::handle_down_evt()
{
	IPACMDBG("ip-type: %d \n", ip_type);
	uint32_t tx_index, rt_hdl;
	ipa_ip_type ip;
	int res = IPACM_SUCCESS, i;

	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		IPACMERR("Invalid iptype: 0x%x\n", ip_type);
		return IPACM_FAILURE;
	}

	/* Delete v6 filtering rules */
	if (ip_type != IPA_IP_v6)
	{
		IPACMDBG("Delete default v4 filter rules\n");
		/* delete default filter rules */
		if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl,
																				IPA_IP_v4,
																				IPV4_DEFAULT_FILTERTING_RULES) == false)
		{
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("Delete private v4 filter rules\n");
		/* free private-ipv4 filter rules */
		if (m_filtering.DeleteFilteringHdls(
					private_fl_rule_hdl,
					IPA_IP_v4,
					IPACM_Iface::ipacmcfg->ipa_num_private_subnet) == false)
		{
			res = IPACM_FAILURE;
			goto fail;
		}
	}

	/* Delete v4 filtering rules */
	if (ip_type != IPA_IP_v4)
	{
		IPACMDBG("Delete default %d v6 filter rules\n", IPV6_DEFAULT_FILTERTING_RULES);
		/* delete default filter rules */
		if (m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl,
																				IPA_IP_v6,
																				IPV6_DEFAULT_FILTERTING_RULES) == false)
		{
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	IPACMDBG("finished delte default filtering rules\n ");

	/* delete WLAN IPA_CLIENT_A5_WLAN_AMPDU_PROD filter rules*/
	for (i = 3; i < wlan_ampdu_flt_rule.num_rules; i++)
	{
		IPACMDBG("Delete WLAN IPA_CLIENT_A5_WLAN_AMPDU_PROD filter rules\n");
		if (m_filtering.DeleteFilteringHdls(&wlan_ampdu_flt_rule.hdl[i],
																				wlan_ampdu_flt_rule.ip[i], 1) == false)
		{
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	IPACMDBG("finished delte AMPDU filtering rules\n ");


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
		for (i = 0; i < num_dft_rt; i++)
		{
			if (m_routing.DeleteRoutingHdl(dft_rt_rule_hdl[i + 1], IPA_IP_v6)
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
	if (IPACM_Wan::isWanUP())
	{
		IPACMDBG("Delete wan filtering rules\n");

		IPACM_Lan::handle_wan_down();
		handle_wan_down();
	}
	IPACMDBG("finished deleting wan filtering rules\n ");


	/* check software routing fl rule hdl */
	if (softwarerouting_act == true)
	{
		IPACMDBG("Delete sw routing filtering rules\n");
		IPACM_Iface::handle_software_routing_disable();
		handle_software_routing_disable();
	}
	IPACMDBG("finished delete software-routing filtering rules\n ");


	/* clean wifi-client header, routing rules */
	/* clean wifi client rule*/
	IPACMDBG("left %d wifi clients need to be deleted \n ", num_wifi_client);
	for (i = 0; i < num_wifi_client; i++)
	{

		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
			if (ip_type != IPA_IP_v6) /* for ipv4 */
			{
				rt_hdl = get_client_memptr(wlan_client, i)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v4;
				ip = IPA_IP_v4;
			}
			else if (ip_type != IPA_IP_v4) /* for ipv6 */
			{
				rt_hdl = get_client_memptr(wlan_client, i)->wifi_rt_hdl[tx_index].wifi_rt_rule_hdl_v6;
				ip = IPA_IP_v6;
			}

			IPACMDBG("Delete %d client route rule %d\n", num_wifi_client, tx_index);
			if (m_routing.DeleteRoutingHdl(rt_hdl, ip) == false)
			{
				res = IPACM_FAILURE;
				goto fail;
			}

		} /* end of tx-index for loop*/

		IPACMDBG("Delete %d client header\n", num_wifi_client);
		if (m_header.DeleteHeaderHdl(get_client_memptr(wlan_client, i)->hdr_hdl)
				== false)
		{
			res = IPACM_FAILURE;
			goto fail;
		}

	} /* end of for loop */

	/* free the wlan clients cache */
	IPACMDBG("Free wlan clients cache\n");

fail:
	free(wlan_client);
	free(tx_prop);
	free(rx_prop);
	free(iface_query);

	return res;
}
