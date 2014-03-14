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

	wlan_client_len = (sizeof(ipa_wlan_client)) + (iface_query->num_tx_props * sizeof(wlan_client_rt_hdl));
	wlan_client = (ipa_wlan_client *)calloc(IPA_MAX_NUM_WIFI_CLIENTS, wlan_client_len);
	if (wlan_client == NULL)
	{
		IPACMERR("unable to allocate memory\n");
		return;
	}

	Nat_App = NatApp::GetInstance();
	if (Nat_App == NULL)
	{
		IPACMERR("unable to get Nat App instance \n");
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
	int wlan_index;
	ipacm_ext_prop* ext_prop;
	ipacm_event_iface_up* data;

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
				/* reset the AP-iface category to unknown */				 
				IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat=UNKNOWN_IF;
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

					IPACM_Lan::handle_addr_evt(data);
					IPACM_Lan::handle_private_subnet(data->iptype);

					if (IPACM_Wan::isWanUP())
					{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
							if(data->iptype == IPA_IP_v4 || data->iptype == IPA_IP_MAX)
							{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
								IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v4);
							}
						}
						else
						{
							IPACM_Lan::handle_wan_up(IPA_IP_v4);
						}
					}

					if(IPACM_Wan::isWanUP_V6())
					{
						if(IPACM_Wan::backhaul_is_sta_mode == false)
						{
							if(data->iptype == IPA_IP_v6 || data->iptype == IPA_IP_MAX)
							{
								ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
								IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v6);
							}
						}
						else
						{
							IPACM_Lan::handle_wan_up(IPA_IP_v6);
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
		
		if(IPACM_Wan::backhaul_is_sta_mode == false)
		{
			if(ip_type == IPA_IP_v4 || ip_type == IPA_IP_MAX)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v4);
				IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v4);
			}
		}
		else
		{
			IPACM_Lan::handle_wan_up(IPA_IP_v4);
		}
	}
		break;

	case IPA_HANDLE_WAN_UP_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_UP_V6 event\n");
		
		if(IPACM_Wan::backhaul_is_sta_mode == false)
		{
			if(ip_type == IPA_IP_v6 || ip_type == IPA_IP_MAX)
			{
				ext_prop = IPACM_Iface::ipacmcfg->GetExtProp(IPA_IP_v6);
				IPACM_Lan::handle_wan_up_ex(ext_prop, IPA_IP_v6);
			}
		}
		else
		{
			IPACM_Lan::handle_wan_up(IPA_IP_v6);
		}
		break;

	case IPA_HANDLE_WAN_DOWN:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
		if (rx_prop != NULL)
		{
			IPACM_Lan::handle_wan_down(IPACM_Wan::backhaul_is_sta_mode);
		}
		
		break;

	case IPA_HANDLE_WAN_DOWN_V6:
		IPACMDBG("Received IPA_HANDLE_WAN_DOWN_V6 event\n");
		handle_wan_down_v6(IPACM_Wan::backhaul_is_sta_mode);
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

/* handle wifi client initial,copy all partial headers (tx property) */
int IPACM_Wlan::handle_wlan_client_init_ex(ipacm_event_data_wlan_ex *data)
{

#define WLAN_IFACE_INDEX_LEN 2

	int res = IPACM_SUCCESS, len = 0, i;
	char index[WLAN_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;
        uint32_t cnt;

	/* start of adding header */
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
												 
								sprintf(index, "%d", ipa_if_num);
								strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));
												 
								strncat(pHeaderDescriptor->hdr[0].name,
												IPA_WLAN_PARTIAL_HDR_NAME_v4,
												sizeof(IPA_WLAN_PARTIAL_HDR_NAME_v4));
												 
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

				sprintf(index, "%d", ipa_if_num);
				strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));

				strncat(pHeaderDescriptor->hdr[0].name,
								IPA_WLAN_PARTIAL_HDR_NAME_v6,
								sizeof(IPA_WLAN_PARTIAL_HDR_NAME_v6));

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

				get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
				IPACMDBG("client(%d) v6 full header name:%s header handle:(0x%x)\n",
								 num_wifi_client,
								 pHeaderDescriptor->hdr[0].name,
											 get_client_memptr(wlan_client, num_wifi_client)->hdr_hdl_v6);
						 
				get_client_memptr(wlan_client, num_wifi_client)->ipv6_header_set=true;			   
				break;  	
			}
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

	for (; clt_indx < num_wifi_client_tmp - 1; clt_indx++)
	{
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

	/* Delete v6 filtering rules */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
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
	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
	{
		IPACMDBG("Delete default %d v6 filter rules\n", IPV6_DEFAULT_FILTERTING_RULES);
		/* delete default filter rules */
		if (m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl,
																				IPA_IP_v6,
																				(IPV6_DEFAULT_FILTERTING_RULES + IPV6_DEFAULT_LAN_FILTERTING_RULES)) == false)
																				
		{
			res = IPACM_FAILURE;
			goto fail;
		}
	}
	IPACMDBG("finished delte default filtering rules\n ");

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

fail:
	/* Delete corresponding ipa_rm_resource_name of RX-endpoint after delete all IPV4V6 FT-rule */ 
	IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[rx_prop->rx[0].src_pipe]);

	free(wlan_client);
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
