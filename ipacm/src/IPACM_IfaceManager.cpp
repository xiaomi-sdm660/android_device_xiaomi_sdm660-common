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
	IPACM_IfaceManager.cpp

	@brief
	This file implements the IPAM iface_manager functionality.

	@Author
	Skylar Chang

*/
#include <string.h>
#include <sys/ioctl.h>

#include <IPACM_IfaceManager.h>
#include <IPACM_EvtDispatcher.h>
#include <IPACM_Defs.h>
#include <IPACM_Wlan.h>
#include <IPACM_Lan.h>
#include <IPACM_Wan.h>
#include <IPACM_Iface.h>
#include <IPACM_Log.h>

iface_instances *IPACM_IfaceManager::head = NULL;

IPACM_IfaceManager::IPACM_IfaceManager() 
{
	IPACM_EvtDispatcher::registr(IPA_LINK_UP_EVENT, this); // skylar fix register name, class name
	return; //skylar no interface_id
}

void IPACM_IfaceManager::event_callback(ipa_cm_event_id event, void *param) //skylar rename:event_callback
{
	ipacm_event_data_fid *evt_data = (ipacm_event_data_fid *)param;
	switch(event)
	{

	case IPA_LINK_UP_EVENT:
		IPACMDBG("link up %d: \n", evt_data->if_index);
		create_iface_instance(evt_data->if_index);
		break;

	default:
		break;
	}
	return;
}

int IPACM_IfaceManager::create_iface_instance(int if_index)
{
	int ipa_interface_index;
	ipa_interface_index = IPACM_Iface::iface_ipa_index_query(if_index);

	/* check if duplicate instance*/
	if(SearchInstance(ipa_interface_index) == IPA_INSTANCE_NOT_FOUND)
	{
		/* IPA_INSTANCE_NOT_FOUND */
		switch(IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].if_cat)
		{

		case LAN_IF:
			{
				IPACMDBG("Creating Lan interface\n");
				IPACM_Lan *lan = new IPACM_Lan(ipa_interface_index);
				IPACM_EvtDispatcher::registr(IPA_ADDR_ADD_EVENT, lan);
				IPACM_EvtDispatcher::registr(IPA_ROUTE_ADD_EVENT, lan);
				IPACM_EvtDispatcher::registr(IPA_ROUTE_DEL_EVENT, lan);
				IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT, lan);
				IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT, lan);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_ENABLE, lan);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_DISABLE, lan);
				IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_UP, lan);
				IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_DOWN, lan);
				IPACM_EvtDispatcher::registr(IPA_LINK_DOWN_EVENT, lan);
				IPACMDBG("ipa_LAN (%s):ipa_index (%d) instance open/registr ok\n", lan->dev_name, lan->ipa_if_num);
				registr(ipa_interface_index, lan);
			}
			break;

		case WLAN_IF:
			{
				IPACMDBG("Creating WLan interface\n");
				IPACM_Wlan *wl = new IPACM_Wlan(ipa_interface_index);
				IPACM_EvtDispatcher::registr(IPA_ADDR_ADD_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_ROUTE_DEL_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_WLAN_CLIENT_ADD_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_WLAN_CLIENT_DEL_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_WLAN_CLIENT_POWER_SAVE_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_WLAN_CLIENT_RECOVER_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT, wl);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_ENABLE, wl);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_DISABLE, wl);
				IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_UP, wl);
				IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_DOWN, wl);
				IPACM_EvtDispatcher::registr(IPA_LINK_DOWN_EVENT, wl);
				IPACMDBG("ipa_WLAN (%s):ipa_index (%d) instance open/registr ok\n", wl->dev_name, wl->ipa_if_num);
				registr(ipa_interface_index, wl);
			}
			break;

		case WAN_IF:
			{
				IPACMDBG("Creating Wan interface\n");
				IPACM_Wan *w = new IPACM_Wan(ipa_interface_index);
				IPACM_EvtDispatcher::registr(IPA_ADDR_ADD_EVENT, w);
				IPACM_EvtDispatcher::registr(IPA_ROUTE_ADD_EVENT, w);
				IPACM_EvtDispatcher::registr(IPA_ROUTE_DEL_EVENT, w);
				IPACM_EvtDispatcher::registr(IPA_FIREWALL_CHANGE_EVENT, w);
				IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT, w);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_ENABLE, w);
				IPACM_EvtDispatcher::registr(IPA_SW_ROUTING_DISABLE, w);
				IPACM_EvtDispatcher::registr(IPA_LINK_DOWN_EVENT, w);
				IPACMDBG("ipa_WAN (%s):ipa_index (%d) instance open/registr ok\n", w->dev_name, w->ipa_if_num);
				registr(ipa_interface_index, w);
			}
			break;

		default:
			IPACMERR("Unhandled interface received\n");
			return IPACM_SUCCESS;
		}
	}
	return IPACM_SUCCESS;
}


int IPACM_IfaceManager::registr(int ipa_if_index, IPACM_Listener *obj)
{
	iface_instances *tmp = head,*nw;

	nw = (iface_instances *)malloc(sizeof(iface_instances));
	if(nw != NULL)
	{
		nw->ipa_if_index = ipa_if_index;
		nw->obj = obj;
		nw->next = NULL;
	}
	else
	{
		return IPACM_FAILURE;
	}

	if(head == NULL)
	{
		head = nw;
	}
	else
	{
		while(tmp->next)
		{
			tmp = tmp->next;
		}
		tmp->next = nw;
	}
	return IPACM_SUCCESS;
}

int IPACM_IfaceManager::deregistr(IPACM_Listener *param)
{
	iface_instances *tmp = head,*tmp1,*prev = head;

	while(tmp != NULL)
	{
		if(tmp->obj == param)
		{
			tmp1 = tmp;
			if(tmp == head)
			{
				head = head->next;
			}
			else if(tmp->next == NULL)
			{
				prev->next = NULL;
			}
			else
			{
				prev->next = tmp->next;
			}

			tmp = tmp->next;
			free(tmp1);
		}
		else
		{
			prev = tmp;
			tmp = tmp->next;
		}
	}
	return IPACM_SUCCESS;
}


int IPACM_IfaceManager::SearchInstance(int ipa_if_index)
{

	iface_instances *tmp = head;

	while(tmp != NULL)
	{
		if(ipa_if_index == tmp->ipa_if_index)
		{
			IPACMDBG("Find existed iface-instance name: %s\n",
							 IPACM_Iface::ipacmcfg->iface_table[ipa_if_index].iface_name);
			return IPA_INSTANCE_FOUND;
		}
		tmp = tmp->next;
	}

	IPACMDBG("No existed iface-instance name: %s,\n",
					 IPACM_Iface::ipacmcfg->iface_table[ipa_if_index].iface_name);

	return IPA_INSTANCE_NOT_FOUND;
}
