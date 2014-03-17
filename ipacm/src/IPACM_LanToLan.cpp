/*
Copyright (c) 2014, The Linux Foundation. All rights reserved.

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
	IPACM_LanToLan.cpp

	@brief
	This file implements the functionality of offloading LAN to LAN traffic.

	@Author
	Shihuan Liu

*/

#include <stdlib.h>
#include <assert.h>
#include "IPACM_LanToLan.h"
#include "IPACM_Wlan.h"

IPACM_LanToLan::IPACM_LanToLan()
{
	num_offload_pair_v4_ = 0;
	num_offload_pair_v6_ = 0;
	client_info_v4_.reserve(IPA_LAN_TO_LAN_MAX_WLAN_CLIENT + IPA_LAN_TO_LAN_MAX_USB_CLIENT);
	client_info_v6_.reserve(3*(IPA_LAN_TO_LAN_MAX_WLAN_CLIENT + IPA_LAN_TO_LAN_MAX_USB_CLIENT));

	IPACM_EvtDispatcher::registr(IPA_LAN_CLIENT_ACTIVE, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_CLIENT_INACTIVE, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_CLIENT_DISCONNECT, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_CLIENT_POWER_SAVE, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_CLIENT_POWER_RECOVER, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_TO_LAN_NEW_CONNECTION, this);
	IPACM_EvtDispatcher::registr(IPA_LAN_TO_LAN_DEL_CONNECTION, this);
	return;
}

IPACM_LanToLan::~IPACM_LanToLan()
{
	client_table_v4::iterator it_v4;
	for(it_v4 = client_info_v4_.begin(); it_v4 != client_info_v4_.end(); it_v4++)
	{
		turnoff_offload_links(IPA_IP_v4, &(it_v4->second));
		clear_peer_list(&(it_v4->second));
	}
	client_info_v4_.clear();
	IPACMDBG("Clear IPv4 hash table in Lan2Lan distructor.\n");

	client_table_v6::iterator it_v6;
	for(it_v6 = client_info_v6_.begin(); it_v6 != client_info_v6_.end(); it_v6++)
	{
		turnoff_offload_links(IPA_IP_v6, &(it_v6->second));
		clear_peer_list(&(it_v6->second));
	}
	client_info_v6_.clear();
	IPACMDBG("Clear IPv4 hash table in Lan2Lan distructor.\n");

	return;
}

void IPACM_LanToLan::event_callback(ipa_cm_event_id event, void* param)
{
	switch(event)
	{
		case IPA_LAN_CLIENT_ACTIVE:
		{
			IPACMDBG("Get IPA_LAN_CLIENT_ACTIVE event.\n");
			ipacm_event_lan_client* data = (ipacm_event_lan_client*)param;
			handle_client_active(data);
			break;
		}

		case IPA_LAN_CLIENT_INACTIVE:
		{
			IPACMDBG("Get IPA_LAN_CLIENT_INACTIVE event.\n");
			ipacm_event_lan_client* data = (ipacm_event_lan_client*)param;
			handle_client_inactive(data);
			break;
		}

		case IPA_LAN_CLIENT_DISCONNECT:
		{
			IPACMDBG("Get IPA_LAN_CLIENT_DISCONNECT event.\n");
			ipacm_event_lan_client* data = (ipacm_event_lan_client*)param;
			handle_client_disconnect(data);
			break;
		}

		case IPA_LAN_TO_LAN_NEW_CONNECTION:
		{
			IPACMDBG("Get IPA_LAN_TO_LAN_NEW_CONNECTION event.\n");
			ipacm_event_connection* data = (ipacm_event_connection*)param;
			handle_new_connection(data);
			break;
		}

		case IPA_LAN_TO_LAN_DEL_CONNECTION:
		{
			IPACMDBG("Get IPA_LAN_TO_LAN_DEL_CONNECTION event.\n");
			ipacm_event_connection* data = (ipacm_event_connection*)param;
			handle_del_connection(data);
			break;
		}
		case IPA_LAN_CLIENT_POWER_SAVE:
		{
			IPACMDBG("Get IPA_LAN_CLIENT_POWER_SAVE event.\n");
			ipacm_event_lan_client* data = (ipacm_event_lan_client*)param;
			handle_client_power_save(data);
			break;
		}
		case IPA_LAN_CLIENT_POWER_RECOVER:
		{
			IPACMDBG("Get IPA_LAN_CLIENT_POWER_RECOVER event.\n");
			ipacm_event_lan_client* data = (ipacm_event_lan_client*)param;
			handle_client_power_recover(data);
			break;
		}
		default:
			break;
	}
	return;
}

void IPACM_LanToLan::handle_client_active(ipacm_event_lan_client* data)
{
	if(data == NULL)
	{
		IPACMERR("No client info is found.\n");
		return;
	}
	if(data->mac_addr == NULL || data->ipv6_addr == NULL || data->p_iface == NULL)
	{
		IPACMERR("Event data is not populated properly.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected.\n");
		return;
	}

	IPACMDBG("New client info: iface %s, iptype: %d, mac: 0x%02x%02x%02x%02x%02x%02x, v4_addr: 0x%08x, v6_addr: 0x%08x%08x%08x%08x \n",
		data->p_iface->dev_name, data->iptype,
		data->mac_addr[0], data->mac_addr[1], data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5],
		data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	bool client_not_found;
	client_info* client_ptr;

	if(data->iptype == IPA_IP_v4)
	{
		client_not_found = (client_info_v4_.count(data->ipv4_addr) == 0);
		IPACMDBG("Is the client not found? %d\n", client_not_found);
		client_info& client = client_info_v4_[data->ipv4_addr];
		client_ptr = &client;
	}
	else
	{
		uint64_t v6_addr;
		memcpy(&v6_addr, &(data->ipv6_addr[2]), sizeof(uint64_t));

		client_not_found = (client_info_v6_.count(v6_addr) == 0);
		IPACMDBG("Is the client not found? %d\n", client_not_found);
		client_info& client = client_info_v6_[v6_addr];
		client_ptr = &client;
	}

	if(client_not_found == true)
	{
		if(data->iptype == IPA_IP_v4)
		{
			client_ptr->ip.ipv4_addr = data->ipv4_addr;
		}
		else
		{
			memcpy(client_ptr->ip.ipv6_addr, data->ipv6_addr, sizeof(client_ptr->ip.ipv6_addr));
		}
		memcpy(client_ptr->mac_addr, data->mac_addr, sizeof(client_ptr->mac_addr));
		client_ptr->is_active =  true;
		client_ptr->is_powersave = false;
		client_ptr->p_iface = data->p_iface;

		generate_new_connection(data->iptype, client_ptr); //code review: remember of remove this once listening to real connection evt
	}
	else	//the client is found
	{
		if(client_ptr->is_active == true)	//the client is active
		{
			IPACMDBG("The client is active.\n");
			if(memcmp(client_ptr->mac_addr, data->mac_addr, sizeof(client_ptr->mac_addr)) == 0)
			{
				IPACMDBG("Mac addr is the same, do nothing.\n");
			}
			else
			{
				IPACMERR("The new client has same IP but differenc mac.\n");
				turnoff_offload_links(data->iptype, client_ptr);
				clear_peer_list(client_ptr);

				if(data->iptype == IPA_IP_v4)
				{
					client_ptr->ip.ipv4_addr = data->ipv4_addr;
				}
				else
				{
					memcpy(client_ptr->ip.ipv6_addr, data->ipv6_addr, sizeof(client_ptr->ip.ipv6_addr));
				}
				memcpy(client_ptr->mac_addr, data->mac_addr, sizeof(client_ptr->mac_addr));
				client_ptr->is_active =  true;
				client_ptr->is_powersave = false;
				client_ptr->p_iface = data->p_iface;

				generate_new_connection(data->iptype, client_ptr);
			}
		}
		else 	//the client is inactive
		{
			IPACMDBG("The client is inactive.\n");
			if(data->iptype == IPA_IP_v4)
			{
				client_ptr->ip.ipv4_addr = data->ipv4_addr;
			}
			else
			{
				memcpy(client_ptr->ip.ipv6_addr, data->ipv6_addr, sizeof(client_ptr->ip.ipv6_addr));
			}
			memcpy(client_ptr->mac_addr, data->mac_addr, sizeof(client_ptr->mac_addr));
			client_ptr->is_active =  true;
			client_ptr->is_powersave = false;
			client_ptr->p_iface = data->p_iface;

			check_potential_link(data->iptype, client_ptr);
			generate_new_connection(data->iptype, client_ptr);
		}
	}
	IPACMDBG("There are %d clients in v4 table and %d clients in v6 table.\n", client_info_v4_.size(), client_info_v6_.size());
	return;
}

void IPACM_LanToLan::check_potential_link(ipa_ip_type iptype, client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("Client is NULL.\n");
		return;
	}

	IPACMDBG("Check client's peer list.\n");
	IPACMDBG("Client: IP type: %d, IPv4 addr: 0x%08x, IPv6 addr: 0x%08x%08x%08x%08x\n", iptype, client->ip.ipv4_addr,
				client->ip.ipv6_addr[0], client->ip.ipv6_addr[1], client->ip.ipv6_addr[2], client->ip.ipv6_addr[3]);

	peer_info_list::iterator peer_it;
	int res, num = 0;

	for(peer_it = client->peer.begin(); peer_it != client->peer.end(); peer_it++)
	{
		if(peer_it->peer_pointer->is_active == true && peer_it->num_connection > 0)
		{
			res = IPACM_SUCCESS;
			res = add_offload_link(iptype, client, peer_it->peer_pointer);
			res = add_offload_link(iptype, peer_it->peer_pointer, client);
			if(res == IPACM_SUCCESS)
			{
				if(iptype == IPA_IP_v4)
				{
					num_offload_pair_v4_ ++;
					IPACMDBG("Now the number of v4 offload links is %d.\n", num_offload_pair_v4_);
				}
				else
				{
					num_offload_pair_v6_ ++;
					IPACMDBG("Now the number of v6 offload links is %d.\n", num_offload_pair_v6_);
				}
				num++;
			}
		}
	}
	IPACMDBG("Added %d offload links in total.\n", num);
	return;
}

int IPACM_LanToLan::add_offload_link(ipa_ip_type iptype, client_info* client, client_info* peer)
{
	if( (iptype == IPA_IP_v4 && num_offload_pair_v4_ >= MAX_OFFLOAD_PAIR)
		|| (iptype == IPA_IP_v6 && num_offload_pair_v6_ >= MAX_OFFLOAD_PAIR) )
	{
		IPACMDBG("The number of offload pairs already reaches maximum.\n");
		return IPACM_FAILURE;
	}
	if(client == NULL || peer == NULL)
	{
		IPACMERR("Either client or peer is NULL.\n");
		return IPACM_FAILURE;
	}

	uint32_t hdr_hdl, flt_hdl;
	lan_to_lan_rt_rule_hdl rt_rule_hdl;

	offload_link_info link_info;


	if(iptype == IPA_IP_v4)
	{
		IPACMDBG("Add offload link for IPv4, client IP: 0x%08x, peer IP: 0x%08x \n", client->ip.ipv4_addr, peer->ip.ipv4_addr);
	}
	else if(iptype == IPA_IP_v6)
	{
		IPACMDBG("Add offload link for IPv6, client IP: 0x%08x%08x%08x%08x, peer IP: 0x%08x%08x%08x%08x \n",
				client->ip.ipv6_addr[0], client->ip.ipv6_addr[1], client->ip.ipv6_addr[2], client->ip.ipv6_addr[3],
				peer->ip.ipv6_addr[0], peer->ip.ipv6_addr[1], peer->ip.ipv6_addr[2], peer->ip.ipv6_addr[3]);
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
		return IPACM_FAILURE;
	}

	//add lan2lan header
	if(peer->p_iface->add_lan2lan_hdr(iptype, client->mac_addr, peer->mac_addr, &hdr_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to create lan2lan header.\n");
		return IPACM_FAILURE;
	}
	IPACMDBG("Created lan2lan hdr with hdl %d.\n", hdr_hdl);

	//add lan2lan routing/filtering rules
	if(peer->p_iface->add_lan2lan_rt_rule(iptype, client->ip.ipv4_addr, peer->ip.ipv4_addr,
					client->ip.ipv6_addr, peer->ip.ipv6_addr, hdr_hdl, &rt_rule_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to create lan2lan rt rule.\n");
		goto rt_fail;
	}
	IPACMDBG("Created %d lan2lan rt rules.\n", rt_rule_hdl.num_rule);
	IPACMDBG("Created lan2lan rt rules with hdl: %d.\n", rt_rule_hdl.rule_hdl[0]);

	if(client->p_iface->add_lan2lan_flt_rule(iptype, client->ip.ipv4_addr, peer->ip.ipv4_addr,
					client->ip.ipv6_addr, peer->ip.ipv6_addr, &flt_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to create lan2lan flt rule.\n");
		goto flt_fail;
	}
	IPACMDBG("Created lan2lan flt rule with hdl %d.\n", flt_hdl);

	link_info.peer_pointer = peer;
	link_info.flt_rule_hdl = flt_hdl;
	link_info.hdr_hdl = hdr_hdl;
	memcpy(&link_info.rt_rule_hdl, &rt_rule_hdl, sizeof(lan_to_lan_rt_rule_hdl));

	client->link.push_back(link_info);

	return IPACM_SUCCESS;

flt_fail:
	peer->p_iface->del_lan2lan_rt_rule(iptype, rt_rule_hdl);

rt_fail:
	peer->p_iface->del_lan2lan_hdr(iptype, hdr_hdl);

	return IPACM_FAILURE;
}

void IPACM_LanToLan::handle_client_inactive(ipacm_event_lan_client* data)
{
	if(data == NULL)
	{
		IPACMERR("No client info is found.\n");
		return;
	}
	if(data->mac_addr == NULL || data->ipv6_addr == NULL || data->p_iface == NULL)
	{
		IPACMERR("Event data is not populated properly.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d.\n", data->iptype);
		return;
	}

	IPACMDBG("Del client info: iface %s, iptype: %d, mac: 0x%02x%02x%02x%02x%02x%02x, v4_addr: 0x%08x, v6_addr: 0x%08x%08x%08x%08x \n",
		data->p_iface->dev_name, data->iptype,
		data->mac_addr[0], data->mac_addr[1], data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5],
		data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	client_info* client_ptr;
	uint64_t v6_addr;

	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->ipv4_addr) == 0)//if not found the client, return
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v4_[data->ipv4_addr];
		client_ptr = &client;
	}
	else
	{
		memcpy(&v6_addr, &(data->ipv6_addr[2]), sizeof(uint64_t));
		if(client_info_v6_.count(v6_addr) == 0)	//if not found the client, insert it in table
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v6_[v6_addr];
		client_ptr = &client;
	}

	turnoff_offload_links(data->iptype, client_ptr);
	client_ptr->is_active = false;
	if(client_ptr->peer.size() == 0)
	{
		IPACMDBG("Peer list is empty, remove client entry.\n");
		if(data->iptype == IPA_IP_v4)
		{
			client_info_v4_.erase(data->ipv4_addr);
		}
		else
		{
			client_info_v6_.erase(v6_addr);
		}
	}
	return;
}

int IPACM_LanToLan::turnoff_offload_links(ipa_ip_type iptype, client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("Client is NULL.\n");
		return IPACM_FAILURE;
	}

	bool err_flag;
	offload_link_info_list::iterator client_it;
	offload_link_info_list::iterator peer_it;
	client_info* peer;

	for(client_it = client->link.begin(); client_it != client->link.end(); client_it++)
	{
		peer = client_it->peer_pointer;
		if(del_offload_link(iptype, client->p_iface, peer->p_iface, &(*client_it)) == IPACM_FAILURE)
		{
			IPACMERR("Failed to delete client's offload link.\n");
			return IPACM_FAILURE;
		}

		err_flag = true;
		for(peer_it = peer->link.begin(); peer_it != peer->link.end(); peer_it++)
		{
			if(peer_it->peer_pointer == client)
			{
				if(del_offload_link(iptype, peer->p_iface, client->p_iface, &(*peer_it)) == IPACM_FAILURE)
				{
					IPACMERR("Failed to delete peer's offload link.\n");
					return IPACM_FAILURE;
				}
				peer->link.erase(peer_it);
				err_flag = false;
				break;
			}
		}
		if(err_flag)
		{
			IPACMERR("Unable to find corresponding offload link in peer's entry.\n");
			return IPACM_FAILURE;
		}
		if(iptype == IPA_IP_v4)
		{
			num_offload_pair_v4_ --;
			IPACMDBG("Now the number of v4 offload pair is %d\n", num_offload_pair_v4_);
		}
		else
		{
			num_offload_pair_v6_ --;
			IPACMDBG("Now the number of v6 offload pair is %d\n", num_offload_pair_v6_);
		}
	}

	client->link.clear();
	return IPACM_SUCCESS;
}

int IPACM_LanToLan::del_offload_link(ipa_ip_type iptype, IPACM_Lan* client, IPACM_Lan* peer, offload_link_info* link)
{
	if(client == NULL || peer == NULL || link == NULL)
	{
		IPACMERR("Either iface or link is NULL.\n");
		return IPACM_FAILURE;
	}

	IPACMDBG("Delete an offload link for IP type: %d\n", iptype);

	int res = IPACM_SUCCESS;

	if(client->del_lan2lan_flt_rule(iptype, link->flt_rule_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to delete flt rule.\n");
		res = IPACM_FAILURE;
	}

	if(peer->del_lan2lan_rt_rule(iptype, link->rt_rule_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to delete rt rules.\n");
		res = IPACM_FAILURE;
	}

	if(peer->del_lan2lan_hdr(iptype, link->hdr_hdl) == IPACM_FAILURE)
	{
		IPACMERR("Failed to delete header.\n");
		res = IPACM_FAILURE;
	}

	return res;
}

void IPACM_LanToLan::handle_client_disconnect(ipacm_event_lan_client* data)
{
	if(data == NULL)
	{
		IPACMERR("No client info is found.\n");
		return;
	}
	if(data->mac_addr == NULL || data->ipv6_addr == NULL || data->p_iface == NULL)
	{
		IPACMERR("Event data is not populated properly.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d.\n", data->iptype);
		return;
	}

	IPACMDBG("Del client info: iface %s, iptype: %d, mac: 0x%02x%02x%02x%02x%02x%02x, v4_addr: 0x%08x, v6_addr: 0x%08x%08x%08x%08x \n",
		data->p_iface->dev_name, data->iptype,
		data->mac_addr[0], data->mac_addr[1], data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5],
		data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	client_info* client_ptr;
	uint64_t v6_addr;
	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->ipv4_addr) == 0)	//if not found the client, return
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v4_[data->ipv4_addr];
		client_ptr = &client;
	}
	else
	{
		memcpy(&v6_addr, &(data->ipv6_addr[2]), sizeof(uint64_t));
		if(client_info_v6_.count(v6_addr) == 0)	//if not found the client, insert it in table
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v6_[v6_addr];
		client_ptr = &client;
	}

	turnoff_offload_links(data->iptype, client_ptr);
	clear_peer_list(client_ptr);
	if(data->iptype == IPA_IP_v4)
	{
		client_info_v4_.erase(data->ipv4_addr);
	}
	else
	{
		client_info_v6_.erase(v6_addr);
	}
	return;
}

int IPACM_LanToLan::clear_peer_list(client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("Client is NULL.\n");
		return IPACM_FAILURE;
	}

	bool err_flag;
	peer_info_list::iterator client_it;
	peer_info_list::iterator peer_it;
	client_info* peer;

	for(client_it = client->peer.begin(); client_it != client->peer.end(); client_it++)
	{
		err_flag = true;
		peer = client_it->peer_pointer;
		for(peer_it = peer->peer.begin(); peer_it != peer->peer.end(); peer_it++)
		{
			if(peer_it->peer_pointer == client)
			{
				peer->peer.erase(peer_it);
				err_flag = false;
				break;
			}
		}
		if(err_flag == true)
		{
			IPACMERR("Failed to find peer info.\n");
			return IPACM_FAILURE;
		}
	}
	client->peer.clear();
	return IPACM_SUCCESS;
}

void IPACM_LanToLan::handle_client_power_save(ipacm_event_lan_client* data)
{
	if(data == NULL)
	{
		IPACMERR("No client info is found.\n");
		return;
	}
	if(data->mac_addr == NULL || data->ipv6_addr == NULL || data->p_iface == NULL)
	{
		IPACMERR("Event data is not populated properly.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d.\n", data->iptype);
		return;
	}

	IPACMDBG("Client power save info: iface %s, iptype: %d, mac: 0x%02x%02x%02x%02x%02x%02x, v4_addr: 0x%08x, v6_addr: 0x%08x%08x%08x%08x \n",
		data->p_iface->dev_name, data->iptype,
		data->mac_addr[0], data->mac_addr[1], data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5],
		data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	client_info* client_ptr;
	uint64_t v6_addr;

	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->ipv4_addr) == 0)//if not found the client, return
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v4_[data->ipv4_addr];
		client_ptr = &client;
	}
	else
	{
		memcpy(&v6_addr, &(data->ipv6_addr[2]), sizeof(uint64_t));
		if(client_info_v6_.count(v6_addr) == 0)	//if not found the client, insert it in table
		{
			IPACMERR("The client is not found the client, return.\n");
			return;
		}
		IPACMDBG("The client is found.\n");
		client_info& client = client_info_v6_[v6_addr];
		client_ptr = &client;
	}

	if(remove_flt_rules(data->iptype, client_ptr) == IPACM_FAILURE)
	{
		IPACMERR("Failed to remove flt rules when power save.\n");
		return;
	}
	client_ptr->is_active = false;
	client_ptr->is_powersave = true;
	return;
}

void IPACM_LanToLan::handle_new_connection(ipacm_event_connection* data)
{
	if(data == NULL)
	{
		IPACMERR("No connection info is found.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d.\n", data->iptype);
		return;
	}

	IPACMDBG("New connection info: IP type: %d, src_v4_addr: 0x%08x, dst_v4_addr: 0x%08x\n", data->iptype, data->src_ipv4_addr, data->dst_ipv4_addr);
	IPACMDBG("src_v6_addr: 0x%08x%08x%08x%08x, dst_v6_addr: 0x%08x%08x%08x%08x", data->src_ipv6_addr[0], data->src_ipv6_addr[1], data->src_ipv6_addr[2],
				data->src_ipv6_addr[3], data->dst_ipv6_addr[0], data->dst_ipv6_addr[1], data->dst_ipv6_addr[2], data->dst_ipv6_addr[3]);

	client_info* src_client_ptr;
	client_info* dst_client_ptr;

	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->src_ipv4_addr) == 0 || client_info_v4_.count(data->dst_ipv4_addr) == 0)
		{
			IPACMERR("Either source or destination is not in table.\n");
			return;
		}
		client_info& src_client = client_info_v4_[data->src_ipv4_addr];
		src_client_ptr = &src_client;
		client_info& dst_client = client_info_v4_[data->dst_ipv4_addr];
		dst_client_ptr = &dst_client;
	}
	else //ipv6 case
	{
		uint64_t src_ipv6_addr;
		uint64_t dst_ipv6_addr;
		memcpy(&src_ipv6_addr, &(data->src_ipv6_addr[2]), sizeof(uint64_t));
		memcpy(&dst_ipv6_addr, &(data->dst_ipv6_addr[2]), sizeof(uint64_t));

		if(client_info_v6_.count(src_ipv6_addr) == 0 || client_info_v6_.count(dst_ipv6_addr) == 0)
		{
			IPACMERR("Either source or destination is not in table.\n");
			return;
		}
		client_info& src_client = client_info_v6_[src_ipv6_addr];
		src_client_ptr = &src_client;
		client_info& dst_client = client_info_v6_[dst_ipv6_addr];
		dst_client_ptr = &dst_client;
	}

	IPACMDBG("Both src and dst are already in table.\n");
	bool is_first_connection;
	is_first_connection = add_connection(src_client_ptr, dst_client_ptr);

	if(is_first_connection && src_client_ptr->is_active && dst_client_ptr->is_active)
	{
		IPACMDBG("This is first connection, add offload links.\n");

		if(add_offload_link(data->iptype, src_client_ptr, dst_client_ptr) == IPACM_FAILURE)
		{
			IPACMERR("Failed to add offload link for src->dst direction.\n");
			return;
		}
		if(add_offload_link(data->iptype, dst_client_ptr, src_client_ptr) == IPACM_FAILURE)
		{
			IPACMERR("Failed to add offload link for dst->src direction.\n");
			return;
		}

		if(data->iptype == IPA_IP_v4)
		{
			num_offload_pair_v4_ ++;
			IPACMDBG("Added offload links, now num_offload_pair_v4_: %d\n", num_offload_pair_v4_);
		}
		else
		{
			num_offload_pair_v6_ ++;
			IPACMDBG("Added offload links, now num_offload_pair_v6_: %d\n", num_offload_pair_v6_);
		}
	}
	return;
}

//If need to insert an entry in peer list, return true, otherwise return false
bool IPACM_LanToLan::add_connection(client_info* src_client, client_info* dst_client)
{
	if(src_client == NULL || dst_client == NULL)
	{
		IPACMERR("Either source or dest client is NULL.\n");
		return false;
	}

	peer_info_list::iterator it;
	peer_info new_peer;
	bool ret = false;

	for(it = src_client->peer.begin(); it != src_client->peer.end(); it++)
	{
		if(it->peer_pointer == dst_client)
		{
			it->num_connection++;
			IPACMDBG("Find dst client entry in peer list, connection count: %d\n", it->num_connection);
			break;
		}
	}
	if(it == src_client->peer.end())
	{
		IPACMDBG("Not finding dst client entry, insert a new one in peer list.\n");
		new_peer.peer_pointer = dst_client;
		new_peer.num_connection = 1;
		src_client->peer.push_back(new_peer);
		ret = true;
	}

	for(it = dst_client->peer.begin(); it != dst_client->peer.end(); it++)
	{
		if(it->peer_pointer == src_client)
		{
			it->num_connection++;
			IPACMDBG("Find dst client entry in peer list, connection count: %d\n", it->num_connection);
			break;
		}
	}
	if(it == dst_client->peer.end())
	{
		IPACMDBG("Not finding src client entry, insert a new one in peer list.\n");
		new_peer.peer_pointer = src_client;
		new_peer.num_connection = 1;
		dst_client->peer.push_back(new_peer);
		ret = true;
	}
	return ret;
}

void IPACM_LanToLan::handle_del_connection(ipacm_event_connection* data)
{
	if(data == NULL)
	{
		IPACMERR("No connection info is found.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d.\n", data->iptype)
		return;
	}

	IPACMDBG("Del connection info: IP type: %d, src_v4_addr: 0x%08x, dst_v4_addr: 0x%08x\n", data->iptype, data->src_ipv4_addr, data->dst_ipv4_addr);
	IPACMDBG("src_v6_addr: 0x%08x%08x%08x%08x, dst_v6_addr: 0x%08x%08x%08x%08x", data->src_ipv6_addr[0], data->src_ipv6_addr[1], data->src_ipv6_addr[2],
				data->src_ipv6_addr[3], data->dst_ipv6_addr[0], data->dst_ipv6_addr[1], data->dst_ipv6_addr[2], data->dst_ipv6_addr[3]);

	bool res;
	uint64_t src_ipv6_addr, dst_ipv6_addr;
	client_info* src_client_ptr;
	client_info* dst_client_ptr;

	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->src_ipv4_addr) == 0
			|| client_info_v4_.count(data->dst_ipv4_addr) == 0)	//if not found the client
		{
			IPACMDBG("Not found either source or dest client, return.\n");
			return;
		}
		client_info& src_client = client_info_v4_[data->src_ipv4_addr];
		client_info& dst_client = client_info_v4_[data->dst_ipv4_addr];
		src_client_ptr = &src_client;
		dst_client_ptr = &dst_client;
	}
	else
	{
		memcpy(&src_ipv6_addr, &(data->src_ipv6_addr[2]), sizeof(uint64_t));
		memcpy(&dst_ipv6_addr, &(data->dst_ipv6_addr[2]), sizeof(uint64_t));
		if(client_info_v6_.count(src_ipv6_addr) == 0
			|| client_info_v6_.count(dst_ipv6_addr) == 0)//if not found the client
		{
			IPACMDBG("Not found either source or dest client, return.\n");
			return;
		}
		client_info& src_client = client_info_v6_[src_ipv6_addr];
		client_info& dst_client = client_info_v6_[dst_ipv6_addr];
		src_client_ptr = &src_client;
		dst_client_ptr = &dst_client;
	}

	res = remove_connection(src_client_ptr, dst_client_ptr);

	if(res && src_client_ptr->is_active && dst_client_ptr->is_active)
	{
		IPACMDBG("Erase link info for both src and dst entries.\n");
		erase_offload_link(data->iptype, src_client_ptr, dst_client_ptr);
	}
	else
	{
		if(res && src_client_ptr->is_powersave && (dst_client_ptr->is_active || dst_client_ptr->is_powersave))
		{
			IPACMDBG("Erase link info for both src and dst entries due to src powersave.\n");
			erase_offload_link(data->iptype, src_client_ptr, dst_client_ptr);
		}
		if(res && dst_client_ptr->is_powersave && (src_client_ptr->is_active || src_client_ptr->is_powersave))
		{
			IPACMDBG("Erase link info for both src and dst entries due to dst powersave.\n");
			erase_offload_link(data->iptype, dst_client_ptr, src_client_ptr);
		}
	}

	//if the src client is not active and not powersave mode, if peer list is empty, remove client entry
	if(res && src_client_ptr->is_active == false && src_client_ptr->is_powersave == false && src_client_ptr->peer.size() == 0)
	{
		IPACMDBG("Peer list of src is empty, remove src entry.\n");
		if(data->iptype == IPA_IP_v4)
		{
			client_info_v4_.erase(data->src_ipv4_addr);
		}
		else
		{
			client_info_v6_.erase(src_ipv6_addr);
		}
	}

	//if the dst client is not active and not powersave mode, if peer list is empty, remove client entry
	if(res && dst_client_ptr->is_active == false && dst_client_ptr->is_powersave == false && dst_client_ptr->peer.size() == 0)
	{
		IPACMDBG("Peer list of dst is empty, remove dst entry.\n");
		if(data->iptype == IPA_IP_v4)
		{
			client_info_v4_.erase(data->dst_ipv4_addr);
		}
		else
		{
			client_info_v6_.erase(dst_ipv6_addr);
		}
	}
	return;
}

//If need to remove an entry in peer list, return true, otherwise return false
bool IPACM_LanToLan::remove_connection(client_info* src_client, client_info* dst_client)
{
	if(src_client == NULL || dst_client == NULL)
	{
		IPACMERR("Either source or dest client is NULL.\n");
		return false;
	}

	peer_info_list::iterator it;
	bool ret = false;

	for(it = src_client->peer.begin(); it != src_client->peer.end(); it++)
	{
		if(it->peer_pointer == dst_client)
		{
			it->num_connection--;
			IPACMDBG("Find dst client entry in src peer list, connection count: %d\n", it->num_connection);
			if(it->num_connection == 0)
			{
				IPACMDBG("Need to remove dst entry in src peer list");
				ret = true;
			}
			break;
		}
	}
	if(ret == true)
	{
		src_client->peer.erase(it);
	}

	ret = false;
	for(it = dst_client->peer.begin(); it != dst_client->peer.end(); it++)
	{
		if(it->peer_pointer == src_client)
		{
			it->num_connection--;
			IPACMDBG("Find src client entry in dst peer list, connection count: %d\n", it->num_connection);
			if(it->num_connection == 0)
			{
				IPACMDBG("Need to remove src entry in dst peer list");
				ret = true;
			}
			break;
		}
	}
	if(ret == true)
	{
		dst_client->peer.erase(it);
	}
	return ret;
}

void IPACM_LanToLan::erase_offload_link(ipa_ip_type iptype, client_info* src_client, client_info* dst_client)
{
	if(src_client == NULL || dst_client == NULL)
	{
		IPACMERR("Either source or dest client is NULL.\n");
		return;
	}

	offload_link_info_list::iterator it;
	int res_src = IPACM_FAILURE, res_dst = IPACM_FAILURE;

	for(it = src_client->link.begin(); it != src_client->link.end(); it++)
	{
		if(it->peer_pointer == dst_client)
		{
			res_src = IPACM_SUCCESS;
			IPACMDBG("Find dst client entry in src link list\n");
			res_src = del_offload_link(iptype, src_client->p_iface, dst_client->p_iface, &(*it));
			src_client->link.erase(it);
			break;
		}
	}

	for(it = dst_client->link.begin(); it != dst_client->link.end(); it++)
	{
		if(it->peer_pointer == src_client)
		{
			res_dst = IPACM_SUCCESS;
			IPACMDBG("Find src client entry in dst link list\n");
			res_dst = del_offload_link(iptype, dst_client->p_iface, src_client->p_iface, &(*it));
			dst_client->link.erase(it);
			break;
		}
	}

	if(res_src == IPACM_SUCCESS && res_dst == IPACM_SUCCESS)
	{
		if(iptype == IPA_IP_v4)
		{
			num_offload_pair_v4_ --;
			IPACMDBG("Decrease num of v4 offload pairs to %d\n", num_offload_pair_v4_);
		}
		else
		{
			num_offload_pair_v6_ --;
			IPACMDBG("Decrease num of v6 offload pairs to %d\n", num_offload_pair_v6_);
		}
	}
	return;
}

void IPACM_LanToLan::generate_new_connection(ipa_ip_type iptype, client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("Client is NULL.\n");
		return;
	}

	IPACMDBG("Generate new connection events for IP type %d\n", iptype);

	int num = 0;
	ipacm_cmd_q_data evt;
	ipacm_event_connection* new_conn;
	ipacm_iface_type client_type, peer_type;
	client_type = IPACM_Iface::ipacmcfg->iface_table[client->p_iface->ipa_if_num].if_cat;

	IPACMDBG("Client's iface type is %d.\n", client_type);

	if(iptype == IPA_IP_v4)
	{
		client_table_v4::iterator it;
		for(it = client_info_v4_.begin(); it != client_info_v4_.end(); it++)
		{
			peer_type = IPACM_Iface::ipacmcfg->iface_table[it->second.p_iface->ipa_if_num].if_cat;
			if(peer_type != client_type && it->second.is_active == true)
			{
				IPACMDBG("Find a qualified peer to generate new_conn event.\n");
				IPACMDBG("Peer's iface type is %d.\n", peer_type);
				new_conn = (ipacm_event_connection*)malloc(sizeof(ipacm_event_connection));
				if(new_conn == NULL)
				{
					IPACMERR("Failed to allocate memory for new_connection event.\n");
					return;
				}
				memset(new_conn, 0, sizeof(ipacm_event_connection));
				new_conn->iptype = IPA_IP_v4;
				new_conn->src_ipv4_addr = client->ip.ipv4_addr;
				new_conn->dst_ipv4_addr = it->second.ip.ipv4_addr;

				memset(&evt, 0, sizeof(evt));
				evt.event = IPA_LAN_TO_LAN_NEW_CONNECTION;
				evt.evt_data = (void*)new_conn;
				IPACM_EvtDispatcher::PostEvt(&evt);
				num++;
			}
		}
	}
	else if(iptype == IPA_IP_v6)
	{
		client_table_v6::iterator it;
		for(it = client_info_v6_.begin(); it != client_info_v6_.end(); it++)
		{
			peer_type = IPACM_Iface::ipacmcfg->iface_table[it->second.p_iface->ipa_if_num].if_cat;
			if(peer_type != client_type && it->second.is_active == true)
			{
				IPACMDBG("Find a qualified peer to generate new_conn event.\n");
				IPACMDBG("Peer's iface type is %d.\n", peer_type);
				new_conn = (ipacm_event_connection*)malloc(sizeof(ipacm_event_connection));
				if(new_conn == NULL)
				{
					IPACMERR("Failed to allocate memory for new_connection event.\n");
					return;
				}
				memset(new_conn, 0, sizeof(ipacm_event_connection));
				new_conn->iptype = IPA_IP_v6;
				memcpy(new_conn->src_ipv6_addr, client->ip.ipv6_addr, sizeof(client->ip.ipv6_addr));
				memcpy(new_conn->dst_ipv6_addr, it->second.ip.ipv6_addr, sizeof(it->second.ip.ipv6_addr));

				memset(&evt, 0, sizeof(evt));
				evt.event = IPA_LAN_TO_LAN_NEW_CONNECTION;
				evt.evt_data = (void*)new_conn;
				IPACM_EvtDispatcher::PostEvt(&evt);
				num++;
			}
		}
	}
	else
	{
		IPACMERR("IP type is not expected.\n");
	}
	IPACMDBG("Generate %d new connection events in total.\n", num);
	return;
}

void IPACM_LanToLan::handle_client_power_recover(ipacm_event_lan_client* data)
{
	if(data == NULL)
	{
		IPACMERR("No client info is found.\n");
		return;
	}
	if(data->mac_addr == NULL || data->ipv6_addr == NULL || data->p_iface == NULL)
	{
		IPACMERR("Event data is not populated properly.\n");
		return;
	}
	if(data->iptype != IPA_IP_v4 && data->iptype != IPA_IP_v6)
	{
		IPACMERR("IP type is not expected: %d\n", data->iptype);
		return;
	}

	IPACMDBG("New client info: iface %s, iptype: %d, mac: 0x%02x%02x%02x%02x%02x%02x, v4_addr: 0x%08x, v6_addr: 0x%08x%08x%08x%08x \n",
		data->p_iface->dev_name, data->iptype,
		data->mac_addr[0], data->mac_addr[1], data->mac_addr[2], data->mac_addr[3], data->mac_addr[4], data->mac_addr[5],
		data->ipv4_addr, data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);

	client_info* client_ptr;
	if(data->iptype == IPA_IP_v4)
	{
		if(client_info_v4_.count(data->ipv4_addr) == 0)
		{
			IPACMERR("Client is not found.\n");
			return;
		}
		client_info& client = client_info_v4_[data->ipv4_addr];
		client_ptr = &client;
	}
	else
	{
		uint64_t v6_addr;
		memcpy(&v6_addr, &(data->ipv6_addr[2]), sizeof(uint64_t));
		if(client_info_v6_.count(v6_addr) == 0)
		{
			IPACMERR("Client is not found.\n");
			return;
		}
		client_info& client = client_info_v6_[v6_addr];
		client_ptr = &client;
	}

	if(client_ptr->is_active == true || client_ptr->is_powersave != true)	//the client is in wrong state
	{
		IPACMERR("The client is in wrong state: active %d, powersave %d.\n", client_ptr->is_active, client_ptr->is_powersave);
		return;
	}
	else
	{
		if(add_flt_rules(data->iptype, client_ptr) == IPACM_FAILURE)
		{
			IPACMERR("Failed to add back flt rules when power recovery.\n");
			return;
		}
		client_ptr->is_active =  true;
		client_ptr->is_powersave = false;

		check_potential_link(data->iptype, client_ptr);
		generate_new_connection(data->iptype, client_ptr);
	}
	IPACMDBG("There are %d clients in v4 table and %d clients in v6 table.\n", client_info_v4_.size(), client_info_v6_.size());
	return;
}

//This function is called when power save: remove filtering rules only
int IPACM_LanToLan::remove_flt_rules(ipa_ip_type iptype, client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("No client info is found.\n");
		return IPACM_FAILURE;
	}

	bool err_flag;
	offload_link_info_list::iterator client_it;
	offload_link_info_list::iterator peer_it;
	client_info* peer;

	for(client_it = client->link.begin(); client_it != client->link.end(); client_it++)
	{
		if(client->p_iface->del_lan2lan_flt_rule(iptype, client_it->flt_rule_hdl) == IPACM_FAILURE)
		{
			IPACMERR("Failed to delete client's filtering rule.\n");
		}

		err_flag = true;
		peer = client_it->peer_pointer;
		for(peer_it = peer->link.begin(); peer_it != peer->link.end(); peer_it++)
		{
			if(peer_it->peer_pointer == client)
			{
				if(peer->p_iface->del_lan2lan_flt_rule(iptype, peer_it->flt_rule_hdl) == IPACM_FAILURE)
				{
					IPACMERR("Failed to delete peer's offload link.\n");
				}
				err_flag = false;
				break;
			}
		}
		if(err_flag)
		{
			IPACMERR("Unable to find corresponding offload link in peer's entry.\n");
			return IPACM_FAILURE;
		}
	}
	return IPACM_SUCCESS;
}

int IPACM_LanToLan::add_flt_rules(ipa_ip_type iptype, client_info* client)
{
	if(client == NULL)
	{
		IPACMERR("No client info is found.\n");
		return IPACM_FAILURE;
	}

	bool err_flag;
	offload_link_info_list::iterator client_it;
	offload_link_info_list::iterator peer_it;
	client_info* peer;

	for(client_it = client->link.begin(); client_it != client->link.end(); client_it++)
	{
		peer = client_it->peer_pointer;
		if(client->p_iface->add_lan2lan_flt_rule(iptype, client->ip.ipv4_addr, peer->ip.ipv4_addr,
			client->ip.ipv6_addr, peer->ip.ipv6_addr, &(client_it->flt_rule_hdl)) == IPACM_FAILURE)
		{
			IPACMERR("Failed to add client's filtering rule.\n");
			return IPACM_FAILURE;
		}

		err_flag = true;
		for(peer_it = peer->link.begin(); peer_it != peer->link.end(); peer_it++)
		{
			if(peer_it->peer_pointer == client)
			{
				if(peer->p_iface->add_lan2lan_flt_rule(iptype, peer->ip.ipv4_addr, client->ip.ipv4_addr,
					peer->ip.ipv6_addr, client->ip.ipv6_addr, &(peer_it->flt_rule_hdl)) == IPACM_FAILURE)
				{
					IPACMERR("Failed to delete peer's offload link.\n");
					return IPACM_FAILURE;
				}
				err_flag = false;
				break;
			}
		}
		if(err_flag)
		{
			IPACMERR("Unable to find corresponding offload link in peer's entry.\n");
			return IPACM_FAILURE;
		}
	}
	return IPACM_SUCCESS;
}

