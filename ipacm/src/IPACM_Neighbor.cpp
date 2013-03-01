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
	IPACM_Neighbor.cpp

	@brief
	This file implements the functionality of handling IPACM Neighbor events.

	@Author
	Skylar Chang

*/

#include <sys/ioctl.h>
#include <IPACM_Neighbor.h>
#include <IPACM_EvtDispatcher.h>
#include "IPACM_Defs.h"
#include "IPACM_Log.h"


IPACM_Neighbor::IPACM_Neighbor()
{
	num_neighbor_client = 0;
	IPACM_EvtDispatcher::registr(IPA_NEW_NEIGH_EVENT, this);
	IPACM_EvtDispatcher::registr(IPA_DEL_NEIGH_EVENT, this);
	return;
}

void IPACM_Neighbor::event_callback(ipa_cm_event_id event, void *param)
{
	ipacm_event_data_all *data = NULL;
	int i, ipa_interface_index;
	ipacm_cmd_q_data evt_data;
	int num_neighbor_client_temp = num_neighbor_client;

	IPACMDBG("Recieved event %d\n", event);

	data = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
	memcpy(data, param, sizeof(ipacm_event_data_all));

	ipa_interface_index = IPACM_Iface::iface_ipa_index_query(data->if_index);
	/*No that interface existed in ipa list*/
	if(ipa_interface_index==-1)
	  return;
	
	if (data->iptype == IPA_IP_v4)
	{
		/* construct IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT command and insert to command-queue*/
		if (event == IPA_NEW_NEIGH_EVENT) 
			evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT;
		else 
			evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT;

		memcpy(&evt_data.evt_data, &data, sizeof(evt_data.evt_data));
		IPACM_EvtDispatcher::PostEvt(&evt_data);
		IPACMDBG("Posted event %d with %s\n", evt_data.event,
						 IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);
	}
	else
	{
		if ((data->ipv6_addr[0]) || (data->ipv6_addr[1]) || (data->ipv6_addr[2]) || (data->ipv6_addr[3]))
		{
			/* check if iface is not bridge0*/
			if (strcmp(IPA_VIRTUAL_IFACE_NAME, IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name) == 0)
			{
				/* searh if seen this client or not*/
				for (i = 0; i < num_neighbor_client_temp; i++)
				{
					if (memcmp(neighbor_client[i].mac_addr, data->mac_addr, sizeof(neighbor_client[i].mac_addr)) == 0)
					{
						data->if_index = neighbor_client[i].iface_index;
						/* construct IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT command and insert to command-queue */
						if (event == IPA_NEW_NEIGH_EVENT) evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT;
						else evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT;
						//evt_data.evt_data=data;
						memcpy(&evt_data.evt_data, &data, sizeof(evt_data.evt_data));
						IPACM_EvtDispatcher::PostEvt(&evt_data);
						/* ask for replaced iface name*/
						ipa_interface_index = IPACM_Iface::iface_ipa_index_query(data->if_index);
						/*No that interface existed in ipa list*/
	          if(ipa_interface_index==-1)
	            return;
						
						IPACMDBG("Posted event %d, with %s\n",
										 evt_data.event,
										 IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);

						/* delete that entry*/
						for (; i < num_neighbor_client_temp - 1; i++)
						{
							memcpy(neighbor_client[i].mac_addr, neighbor_client[i + 1].mac_addr, sizeof(neighbor_client[i].mac_addr));
							neighbor_client[i].v6_addr[0] = neighbor_client[i + 1].v6_addr[0];
							neighbor_client[i].v6_addr[1] = neighbor_client[i + 1].v6_addr[1];
							neighbor_client[i].v6_addr[2] = neighbor_client[i + 1].v6_addr[2];
							neighbor_client[i].v6_addr[3] = neighbor_client[i + 1].v6_addr[3];
							neighbor_client[i].iface_index = neighbor_client[i + 1].iface_index;
						}
						num_neighbor_client -= 1;
						break;

					};
				}

				/* cannot find neighbor client*/
				if (i == num_neighbor_client_temp)
				{
					IPACMDBG("ipv6 with bridge0 with mac, not seen before\n");
					if (num_neighbor_client_temp < IPA_MAX_NUM_NEIGHBOR_CLIENTS)
					{
						memcpy(neighbor_client[num_neighbor_client_temp].mac_addr,
									 data->mac_addr,
									 sizeof(data->mac_addr));
						neighbor_client[num_neighbor_client_temp].v6_addr[0] = data->ipv6_addr[0];
						neighbor_client[num_neighbor_client_temp].v6_addr[1] = data->ipv6_addr[1];
						neighbor_client[num_neighbor_client_temp].v6_addr[2] = data->ipv6_addr[2];
						neighbor_client[num_neighbor_client_temp].v6_addr[3] = data->ipv6_addr[3];
						IPACMDBG("Copy bridge0 MAC %02x:%02x:%02x:%02x:%02x:%02x\n, total client: %d\n",
										 neighbor_client[num_neighbor_client_temp].mac_addr[0],
										 neighbor_client[num_neighbor_client_temp].mac_addr[1],
										 neighbor_client[num_neighbor_client_temp].mac_addr[2],
										 neighbor_client[num_neighbor_client_temp].mac_addr[3],
										 neighbor_client[num_neighbor_client_temp].mac_addr[4],
										 neighbor_client[num_neighbor_client_temp].mac_addr[5],
										 num_neighbor_client_temp);
						num_neighbor_client++;
						return;
					}
					else
					{
						IPACMERR("error:  neighbor client oversize!");
						return;
					}
				}
			}
			else
			{
				/* construct IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT command and insert to command-queue */
				if (event == IPA_NEW_NEIGH_EVENT) 
					evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT;
				else 
					evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT;

				memcpy(&evt_data.evt_data, &data, sizeof(evt_data.evt_data));
				//evt_data.evt_data=data;
				IPACM_EvtDispatcher::PostEvt(&evt_data);
				IPACMDBG("Posted event %d with %s\n",
								 evt_data.event,
								 IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);


			}
		}
		else
		{
			/*no ipv6 in data searh if seen this client or not*/
			for (i = 0; i < num_neighbor_client_temp; i++)
			{
				if (memcmp(neighbor_client[i].mac_addr, data->mac_addr, sizeof(neighbor_client[i].mac_addr)) == 0)
				{
					data->ipv6_addr[0] = neighbor_client[i].v6_addr[0];
					data->ipv6_addr[1] = neighbor_client[i].v6_addr[1];
					data->ipv6_addr[2] = neighbor_client[i].v6_addr[2];
					data->ipv6_addr[3] = neighbor_client[i].v6_addr[3];

					/* check if iface is not bridge0*/
					if (strcmp(IPA_VIRTUAL_IFACE_NAME, IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name) != 0)
					{
						/* construct IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT command and insert to command-queue */
						if (event == IPA_NEW_NEIGH_EVENT) evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT;
						else evt_data.event = IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT;
						memcpy(&evt_data.evt_data, &data, sizeof(evt_data.evt_data));
						IPACM_EvtDispatcher::PostEvt(&evt_data);
						IPACMDBG("Posted event %d with %s\n",
										 evt_data.event,
										 IPACM_Iface::ipacmcfg->iface_table[ipa_interface_index].iface_name);


						/* delete that entry*/
						for (; i <= num_neighbor_client_temp; i++)
						{
							memcpy(neighbor_client[i].mac_addr, neighbor_client[i + 1].mac_addr, sizeof(neighbor_client[i].mac_addr));
							neighbor_client[i].v6_addr[0] = neighbor_client[i + 1].v6_addr[0];
							neighbor_client[i].v6_addr[1] = neighbor_client[i + 1].v6_addr[1];
							neighbor_client[i].v6_addr[2] = neighbor_client[i + 1].v6_addr[2];
							neighbor_client[i].v6_addr[3] = neighbor_client[i + 1].v6_addr[3];
							neighbor_client[i].iface_index = neighbor_client[i + 1].iface_index;
						}
						num_neighbor_client -= 1;
					};
					break;
				};
			}
			/* not find client */
			if (i == num_neighbor_client_temp)
			{
				IPACMDBG("ipv6 with bridge0 with mac, not seen before\n");
				if (num_neighbor_client_temp < IPA_MAX_NUM_NEIGHBOR_CLIENTS)
				{
					memcpy(neighbor_client[num_neighbor_client_temp].mac_addr,
								 data->mac_addr,
								 sizeof(data->mac_addr));
					neighbor_client[num_neighbor_client_temp].iface_index = data->if_index;
					IPACMDBG("Copy wlan-iface client MAC %02x:%02x:%02x:%02x:%02x:%02x\n, total client: %d\n",
									 neighbor_client[num_neighbor_client_temp].mac_addr[0],
									 neighbor_client[num_neighbor_client_temp].mac_addr[1],
									 neighbor_client[num_neighbor_client_temp].mac_addr[2],
									 neighbor_client[num_neighbor_client_temp].mac_addr[3],
									 neighbor_client[num_neighbor_client_temp].mac_addr[4],
									 neighbor_client[num_neighbor_client_temp].mac_addr[5],
									 num_neighbor_client);
					num_neighbor_client++;
					return;
				}
				else
				{
					IPACMDBG("error:  neighbor client oversize!");
					return;
				}
			};
		}
	}

	return;
}

