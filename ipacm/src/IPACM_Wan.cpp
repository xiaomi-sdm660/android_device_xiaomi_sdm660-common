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
		IPACM_Wan.cpp

		@brief
		This file implements the WAN iface functionality.

		@Author
		Skylar Chang

*/
#include <string.h>
#include <sys/ioctl.h>
#include <IPACM_Wan.h>
#include <IPACM_Xml.h>
#include <IPACM_Log.h>
#include "IPACM_EvtDispatcher.h"
#include <IPACM_IfaceManager.h>

bool IPACM_Wan::wan_up = false;

IPACM_Wan::IPACM_Wan(int iface_index) : IPACM_Iface(iface_index)
{
	num_firewall_v4 = 0;
	num_firewall_v6 = 0;

	wan_route_rule_v4_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
	wan_route_rule_v6_hdl = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));
	wan_route_rule_v6_hdl_a5 = (uint32_t *)calloc(iface_query->num_tx_props, sizeof(uint32_t));

	active_v4 = false;
	active_v6 = false;
  header_set_v4 = false;
  header_set_v6 = false;
	header_partial_default_wan_v4 = false;
	header_partial_default_wan_v6 = false;
	hdr_hdl_sta_v4 = 0;
	hdr_hdl_sta_v6 = 0;

	IPACMDBG(" IPACM->IPACM_Wan(%d) constructor: Tx:%d\n", ipa_if_num, iface_query->num_tx_props);
	return;
}

IPACM_Wan::~IPACM_Wan()
{
	IPACM_EvtDispatcher::deregistr(this);
	IPACM_IfaceManager::deregistr(this);
	return;
}

/* handle new_address event */
int IPACM_Wan::handle_addr_evt(ipacm_event_data_addr *data)
{
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	const int NUM_RULES = 1;
	    int num_ipv6_addr;
	int res = IPACM_SUCCESS;
		
	if (data->iptype == IPA_IP_v6)
	{
	    for(num_ipv6_addr=0;num_ipv6_addr<num_dft_rt_v6;num_ipv6_addr++)
	    {
               if((ipv6_addr[num_ipv6_addr][0] == data->ipv6_addr[0]) &&	
	           (ipv6_addr[num_ipv6_addr][1] == data->ipv6_addr[1]) &&	
	            (ipv6_addr[num_ipv6_addr][2] == data->ipv6_addr[2]) &&	
	                (ipv6_addr[num_ipv6_addr][3] == data->ipv6_addr[3]))
               {
	   		   IPACMDBG("find matched ipv6 address, index:%d \n", num_ipv6_addr);
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
	    	init_fl_rule(data->iptype); 
	    }
	    	
	    	num_dft_rt_v6++;	
    }
	else
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
		/* still need setup v4 default routing rule to A5*/
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
        IPACMDBG("ipv4 wan iface rt-rule hdll=0x%x\n", dft_rt_rule_hdl[0]);		
			/* initial multicast/broadcast/fragment filter rule */
			init_fl_rule(data->iptype);
		
		
		wan_v4_addr = data->ipv4_addr;
		IPACMDBG("Receved wan address:0x%x\n",wan_v4_addr);
		}

      IPACMDBG("number of default route rules %d\n", num_dft_rt_v6);

fail:
	free(rt_rule);

	return res;
}

void IPACM_Wan::event_callback(ipa_cm_event_id event, void *param)
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
				IPACMDBG("ipa_WAN (%s):ipa_index (%d) instance close \n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ipa_if_num);
	                        /* reset the STA-iface category to unknown */
                                if((header_set_v4 == true) || (header_set_v6 == true))
                                {
	            	             IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].if_cat=UNKNOWN_IF;
                                }
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
					handle_addr_evt(data);
				}
			}
		}
		break;

	case IPA_ROUTE_ADD_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_ROUTE_ADD_EVENT\n");
				IPACMDBG("ipv4 addr 0x%x\n", data->ipv4_addr);
				IPACMDBG("ipv4 addr mask 0x%x\n", data->ipv4_addr_mask);

				/* The special below condition is to handle default gateway */
				if ((data->iptype == IPA_IP_v4) && (!data->ipv4_addr) && (!data->ipv4_addr_mask) && (active_v4 == false))
				{
					IPACMDBG("adding routing table\n");
				  handle_route_add_evt(data->iptype);
				}
				else if ((data->iptype == IPA_IP_v6) &&
								 (!data->ipv6_addr[0]) && (!data->ipv6_addr[1]) && (!data->ipv6_addr[2]) && (!data->ipv6_addr[3]) && (active_v6 == false))
				{
					IPACMDBG("\n get default v6 route (dst:00.00.00.00)\n");
					IPACMDBG(" IPV6 value: %08x:%08x:%08x:%08x \n",
									 data->ipv6_addr[0], data->ipv6_addr[1], data->ipv6_addr[2], data->ipv6_addr[3]);
				  	handle_route_add_evt(data->iptype);
				}
			}
		}
		break;

	case IPA_ROUTE_DEL_EVENT:
		{
			ipacm_event_data_addr *data = (ipacm_event_data_addr *)param;
			ipa_interface_index = iface_ipa_index_query(data->if_index);
			if (ipa_interface_index == ipa_if_num)
			{
				IPACMDBG("Received IPA_ROUTE_DEL_EVENT\n");
				if ((data->iptype == IPA_IP_v4) && (!data->ipv4_addr) && (!data->ipv4_addr_mask) && (active_v4 == true))
				{
					IPACMDBG("get del default v4 route (dst:0.0.0.0)\n");
					del_dft_firewall_rules(IPA_IP_v4);
					handle_route_del_evt(data->iptype);
					IPACM_Wan::wan_up = false;
				}
				else if ((data->iptype == IPA_IP_v6) && (!data->ipv6_addr[0]) && (!data->ipv6_addr[1]) && (!data->ipv6_addr[2]) && (!data->ipv6_addr[3]) && (active_v6 == true))
				{
					IPACMDBG("get del default v6 route (dst:00.00.00.00)\n");
					del_dft_firewall_rules(IPA_IP_v6);
					handle_route_del_evt(data->iptype);
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
				IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT in STA mode\n");
				handle_header_add_evt(data->mac_addr);
			}
		}
		break;

	case IPA_SW_ROUTING_ENABLE:
		IPACMDBG("Received IPA_SW_ROUTING_ENABLE\n");
		/* handle software routing enable event */
		handle_software_routing_enable();
		break;

	case IPA_SW_ROUTING_DISABLE:
		IPACMDBG("Received IPA_SW_ROUTING_DISABLE\n");
		/* handle software routing disable event */
		handle_software_routing_disable();
		break;

	case IPA_FIREWALL_CHANGE_EVENT:
		IPACMDBG("Received IPA_FIREWALL_CHANGE_EVENT\n");
		/* handle software routing enable event */
		if (active_v4)
		{
			del_dft_firewall_rules(IPA_IP_v4);
			config_dft_firewall_rules(IPA_IP_v4);
		}

		if (active_v6)
		{
			del_dft_firewall_rules(IPA_IP_v6);
			config_dft_firewall_rules(IPA_IP_v6);
		}
		break;

	default:
		break;
	}

	return;
}

/* wan default route/filter rule configuration */
int IPACM_Wan::handle_route_add_evt(ipa_ip_type iptype)
{

	/* add default WAN route */
	struct ipa_ioc_add_rt_rule *rt_rule;
	struct ipa_rt_rule_add *rt_rule_entry;
	struct ipa_ioc_get_hdr sRetHeader;
	uint32_t tx_index,cnt;
	const int NUM = 1;
	ipacm_cmd_q_data evt_data;
	struct ipa_ioc_copy_hdr sCopyHeader; /* checking if partial header*/

	IPACMDBG(" ip-type:%d\n", iptype);

	/* copy header from tx-property, see if partial or not */
	/* assume all tx-property uses the same header name for v4 or v6*/

	if(tx_prop == NULL)
	{
	  IPACMDBG("No tx properties, ignore default route setting\n");
	  return IPACM_SUCCESS;
	}
	
        for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
	{
		   if(tx_prop->tx[cnt].ip==iptype)
		   break;  	
	}	

	if(tx_prop->tx[cnt].hdr_name != NULL)
	{
	    memset(&sCopyHeader, 0, sizeof(sCopyHeader));
	    memcpy(sCopyHeader.name,
	    			 tx_prop->tx[cnt].hdr_name,
	    			 sizeof(sCopyHeader.name));
        
	    IPACMDBG("header name: %s\n", sCopyHeader.name);
	    if (m_header.CopyHeader(&sCopyHeader) == false)
	    {
	    	IPACMERR("ioctl copy header failed");
	    	return IPACM_FAILURE;
	    }        
	    IPACMDBG("header length: %d, paritial: %d\n", sCopyHeader.hdr_len, sCopyHeader.is_partial);
	    if(sCopyHeader.is_partial)
	    {
 	           IPACMDBG("Not setup default WAN routing rules cuz the header is not complete\n");
                   if(iptype==IPA_IP_v4)
		   {
		      header_partial_default_wan_v4 = true;
                   }
		   else		   
		   {
		      header_partial_default_wan_v6 = true;
		   }
		   return IPACM_SUCCESS;
	    }
	    else
	    {
                   if(iptype==IPA_IP_v4)
		   {
		      header_partial_default_wan_v4 = false;
                   }
		   else		   
		   {
		      header_partial_default_wan_v6 = false;
		   }
		   
	    }
         }

	rt_rule = (struct ipa_ioc_add_rt_rule *)
		 calloc(1, sizeof(struct ipa_ioc_add_rt_rule) +
						NUM * sizeof(struct ipa_rt_rule_add));

	if (!rt_rule)
	{
		IPACMERR("Error Locate ipa_ioc_add_rt_rule memory...\n");
		return IPACM_FAILURE;
	}

	rt_rule->commit = 1;
	rt_rule->num_rules = (uint8_t)NUM;
	rt_rule->ip = iptype;


	IPACMDBG(" WAN table created %s \n", rt_rule->rt_tbl_name);
	rt_rule_entry = &rt_rule->rules[0];
	rt_rule_entry->at_rear = true;

	for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
	{

		if(iptype != tx_prop->tx[tx_index].ip)
		{
			IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d no RT-rule added\n", 
							    tx_index, tx_prop->tx[tx_index].ip,iptype);		
			continue;
		}

	    if (iptype == IPA_IP_v4) 
	    {
	    	strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v4.name);
	    }
	    else 
	    {
	    	strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_v6.name);
	    }		
		
		if (tx_prop->tx[tx_index].hdr_name !=  NULL)
		{
			IPACMDBG(" TX- header hdl %s \n", tx_prop->tx[tx_index].hdr_name);
			memset(&sRetHeader, 0, sizeof(sRetHeader));
			strncpy(sRetHeader.name,
							tx_prop->tx[tx_index].hdr_name,
							sizeof(tx_prop->tx[tx_index].hdr_name));
			if (false == m_header.GetHeaderHandle(&sRetHeader))

			{
				IPACMERR("\n ioctl failed\n");
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
		if (iptype == IPA_IP_v4)
		{
			rt_rule_entry->rule.attrib.u.v4.dst_addr      = 0;
			rt_rule_entry->rule.attrib.u.v4.dst_addr_mask = 0;
		    
			if (false == m_routing.AddRoutingRule(rt_rule))
		    {
		    	IPACMERR("Routing rule addition failed!\n");
		    	free(rt_rule);
		    	return IPACM_FAILURE;
		    }
			wan_route_rule_v4_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
		    IPACMDBG("Got ipv4 wan-route rule hdl:0x%x,tx:%d,ip-type: %d \n",
						 wan_route_rule_v4_hdl[tx_index],
						 tx_index,
						 iptype);
		}
		else
		{
			rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;

			if (false == m_routing.AddRoutingRule(rt_rule))
		    {
		    	IPACMERR("Routing rule addition failed!\n");
		    	free(rt_rule);
		    	return IPACM_FAILURE;
		}
			wan_route_rule_v6_hdl[tx_index] = rt_rule_entry->rt_rule_hdl;
			IPACMDBG("Set ipv6 wan-route rule hdl for v6_lan_table:0x%x,tx:%d,ip-type: %d \n",
		                 wan_route_rule_v6_hdl[tx_index],
		                 tx_index,
		                 iptype);
            
			/* insert one more rule for WANRTBLv6 table*/							 
            strcpy(rt_rule->rt_tbl_name, IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.name);
            memset(rt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			rt_rule_entry->at_rear = true;
            rt_rule_entry->rule.dst = IPA_CLIENT_A5_LAN_WAN_CONS;  //go to A5
            rt_rule_entry->rule.attrib.attrib_mask = IPA_FLT_DST_ADDR;			
			rt_rule_entry->rule.attrib.u.v6.dst_addr[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr[3] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[0] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[1] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[2] = 0;
			rt_rule_entry->rule.attrib.u.v6.dst_addr_mask[3] = 0;

		if (false == m_routing.AddRoutingRule(rt_rule))
		{
			IPACMERR("Routing rule addition failed!\n");
			free(rt_rule);
			return IPACM_FAILURE;
		}
			wan_route_rule_v6_hdl_a5[tx_index] = rt_rule_entry->rt_rule_hdl;
			IPACMDBG("Set ipv6 wan-route rule hdl for v6_wan_table:0x%x,tx:%d,ip-type: %d \n",
		                 wan_route_rule_v6_hdl_a5[tx_index],
		                 tx_index,
		                 iptype);
		}

	}
	free(rt_rule);

	if (iptype == IPA_IP_v4)
	{
		ipacm_event_iface_up *wanup_data;

	    IPACM_Wan::wan_up = true;
		active_v4 = true;
		config_dft_firewall_rules(IPA_IP_v4);

		wanup_data = (ipacm_event_iface_up *)malloc(sizeof(ipacm_event_iface_up));
		if (wanup_data == NULL)
		{
			IPACMERR("Unable to allocate memory\n");
			return IPACM_FAILURE;
		}

		memcpy(wanup_data->ifname, dev_name, sizeof(wanup_data->ifname));
		wanup_data->ipv4_addr = wan_v4_addr;
		IPACMDBG("Posting IPA_HANDLE_WAN_UP with below information:\n");
		IPACMDBG("if_name:%s, ipv4_address:0x%x\n",
						 wanup_data->ifname, wanup_data->ipv4_addr);

		evt_data.event = IPA_HANDLE_WAN_UP;
		evt_data.evt_data = (void *)wanup_data;
		IPACM_EvtDispatcher::PostEvt(&evt_data);
	}
	else
	{
		active_v6 = true;
		config_dft_firewall_rules(IPA_IP_v6);
	}

	/* Add corresponding ipa_rm_resource_name of TX-endpoint up before IPV6 RT-rule set */
        IPACM_Iface::ipacmcfg->AddRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe],false);
	
	return IPACM_SUCCESS;
}


/* wan default route/filter rule delete */
int IPACM_Wan::handle_route_del_evt(ipa_ip_type iptype)
{
	uint32_t tx_index;
	ipacm_cmd_q_data evt_data;

	IPACMDBG("got handle_route_del_evt with ip-family:%d \n", iptype);

	if(tx_prop == NULL)
	{
	  IPACMDBG("No tx properties, ignore delete default route setting\n");
	  return IPACM_SUCCESS;
	}		
	
	if (((iptype == IPA_IP_v4) && (active_v4 == true)) ||
			((iptype == IPA_IP_v6) && (active_v6 == true)))
	{

		/* Delete corresponding ipa_rm_resource_name of TX-endpoint after delete IPV4/V6 RT-rule */
	        IPACM_Iface::ipacmcfg->DelRmDepend(IPACM_Iface::ipacmcfg->ipa_client_rm_map_tbl[tx_prop->tx[0].dst_pipe]);	
	
		for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
		{
		
		    if(iptype != tx_prop->tx[tx_index].ip)
		    {
		    	IPACMDBG("Tx:%d, ip-type: %d conflict ip-type: %d, no RT-rule deleted\n", 
		    					    tx_index, tx_prop->tx[tx_index].ip,iptype);		
		    	continue;
		    }		
		
			if (iptype == IPA_IP_v4)
			{
		    	IPACMDBG("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n", 
		    					    tx_index, tx_prop->tx[tx_index].ip,iptype);		

				if (m_routing.DeleteRoutingHdl(wan_route_rule_v4_hdl[tx_index], IPA_IP_v4)
						== false)
				{
					IPACMDBG("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v4, wan_route_rule_v4_hdl[tx_index], tx_index);
					return IPACM_FAILURE;
				}

			}
			else
			{
		    	IPACMDBG("Tx:%d, ip-type: %d match ip-type: %d, RT-rule deleted\n", 
		    					    tx_index, tx_prop->tx[tx_index].ip,iptype);		

				if (m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl[tx_index], IPA_IP_v6)
						== false)
				{
					IPACMDBG("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n", IPA_IP_v6, wan_route_rule_v6_hdl[tx_index], tx_index);
					return IPACM_FAILURE;
				}
			   
			   if (m_routing.DeleteRoutingHdl(wan_route_rule_v6_hdl_a5[tx_index], IPA_IP_v6)
			   		== false)
			   {
			   	IPACMDBG("IP-family:%d, Routing rule(hdl:0x%x) deletion failed with tx_index %d!\n",IPA_IP_v6,wan_route_rule_v6_hdl_a5[tx_index],tx_index);
			   	return IPACM_FAILURE;
			   }
			   
			   
			}

		}

		if (iptype == IPA_IP_v4)
		{

			uint32_t *wan_ip_addr = (uint32_t *)malloc(sizeof(uint32_t));
			if (wan_ip_addr == NULL)
			{
				IPACMERR("unable to allocate memory\n");
				return IPACM_FAILURE;
			}

			*wan_ip_addr = wan_v4_addr;
			evt_data.event = IPA_HANDLE_WAN_DOWN;
			evt_data.evt_data = (void *)wan_ip_addr;
			/* Insert IPA_HANDLE_WAN_DOWN to command queue */
			IPACMDBG("posting IPA_HANDLE_WAN_DOWN for IPv4 \n");
			IPACM_EvtDispatcher::PostEvt(&evt_data);
			IPACMDBG("setup wan_up/active_v4= false \n"); 
			IPACM_Wan::wan_up = false;
			active_v4 = false;
		}
		else
		{
			IPACMDBG("setup wan_up_v6/active_v6= false \n");
			active_v6 = false; 
		}
	}
	else
	{
		IPACMDBG(" The default WAN routing rules are deleted already \n");
	}

	return IPACM_SUCCESS;
}

/* construct complete ethernet header */
int IPACM_Wan::handle_header_add_evt(uint8_t mac_addr[6])
{
  #define WAN_IFACE_INDEX_LEN 2
    
	uint32_t tx_index,cnt;
	int res = IPACM_SUCCESS, len = 0;
	char index[WAN_IFACE_INDEX_LEN];
	struct ipa_ioc_copy_hdr sCopyHeader;
	struct ipa_ioc_add_hdr *pHeaderDescriptor = NULL;

	/* start of adding header */
	
	IPACMDBG("Received Client MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
					 mac_addr[0], mac_addr[1], mac_addr[2],
					 mac_addr[3], mac_addr[4], mac_addr[5]);

        if((header_set_v4 == true) || (header_set_v6 == true))
        {
	   IPACMDBG("Already add STA full header\n");
       return IPACM_SUCCESS;
	}					 
					 
	/* add header to IPA */
	len = sizeof(struct ipa_ioc_add_hdr) + (1 * sizeof(struct ipa_hdr_add));
	pHeaderDescriptor = (struct ipa_ioc_add_hdr *)calloc(1, len);
	if (pHeaderDescriptor == NULL)
	{
		IPACMERR("calloc failed to allocate pHeaderDescriptor\n");
		return IPACM_FAILURE;
	}

	/* copy partial header for v4 */
        for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
	{
		   if(tx_prop->tx[cnt].ip==IPA_IP_v4)
		   {		   
	               memset(&sCopyHeader, 0, sizeof(sCopyHeader));
	               memcpy(sCopyHeader.name,
				 tx_prop->tx[cnt].hdr_name,
				 sizeof(sCopyHeader.name));

	IPACMDBG("header name: %s from tx: %d\n", sCopyHeader.name,cnt);
	if (m_header.CopyHeader(&sCopyHeader) == false)
	{
		IPACMERR("ioctl copy header failed");
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
	memcpy(&pHeaderDescriptor->hdr[0].hdr[IPA_WLAN_PARTIAL_HDR_OFFSET], mac_addr,
					 IPA_MAC_ADDR_SIZE); /* only copy 6 bytes mac-address */				 
				 
	pHeaderDescriptor->commit = true;
	pHeaderDescriptor->num_hdrs = 1;

	memset(pHeaderDescriptor->hdr[0].name, 0,
				 sizeof(pHeaderDescriptor->hdr[0].name));

	sprintf(index, "%d", ipa_if_num);
	strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));

	strncat(pHeaderDescriptor->hdr[0].name,
					IPA_WAN_PARTIAL_HDR_NAME_v4,
					sizeof(IPA_WAN_PARTIAL_HDR_NAME_v4));

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
	else
	{
	        hdr_hdl_sta_v4 = pHeaderDescriptor->hdr[0].hdr_hdl;
		header_set_v4 = true;
	        IPACMDBG("add full header name: %s (%x)\n", pHeaderDescriptor->hdr[0].name, pHeaderDescriptor->hdr[0].hdr_hdl);
	}

	/* copy ipv4 full header to each TX endpoint property*/ 
	for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
	{
	   if(tx_prop->tx[tx_index].ip==IPA_IP_v4)
           {	  
		  memcpy(tx_prop->tx[tx_index].hdr_name, pHeaderDescriptor->hdr[0].name,
					 sizeof(tx_prop->tx[tx_index].hdr_name));
		                 IPACMDBG("replace full header name: %s (%x) in tx:%d\n", tx_prop->tx[tx_index].hdr_name, pHeaderDescriptor->hdr[0].hdr_hdl,tx_index);
                           }
	               }	
		        break; 
		   }
	}	

	  

	/* copy partial header for v6 */
        for (cnt=0; cnt<tx_prop->num_tx_props; cnt++)
	{
		   if(tx_prop->tx[cnt].ip == IPA_IP_v6)
		   {
	                 IPACMDBG("Got partial v6-header name from %d tx props\n", cnt);
	                 memset(&sCopyHeader, 0, sizeof(sCopyHeader));
	                 memcpy(sCopyHeader.name,
				           tx_prop->tx[cnt].hdr_name,
				                   sizeof(sCopyHeader.name));

	IPACMDBG("header name: %s from tx: %d\n", sCopyHeader.name,cnt);
	if (m_header.CopyHeader(&sCopyHeader) == false)
	{
		IPACMERR("ioctl copy header failed");
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
	                 memcpy(&pHeaderDescriptor->hdr[0].hdr[IPA_WLAN_PARTIAL_HDR_OFFSET], mac_addr,
	                 				 IPA_MAC_ADDR_SIZE); /* only copy 6 bytes mac-address */				 
	                 			 
	                 pHeaderDescriptor->commit = true;
	                 pHeaderDescriptor->num_hdrs = 1;
                     
	                 memset(pHeaderDescriptor->hdr[0].name, 0,
	                 			 sizeof(pHeaderDescriptor->hdr[0].name));
                     
	                 sprintf(index, "%d", ipa_if_num);
	                 strncpy(pHeaderDescriptor->hdr[0].name, index, sizeof(index));
	                 	
	                 strncat(pHeaderDescriptor->hdr[0].name,
	                 				IPA_WAN_PARTIAL_HDR_NAME_v6,
	                 				sizeof(IPA_WAN_PARTIAL_HDR_NAME_v6));
                     
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
	                 else
	                 {
			   header_set_v6 = true;
	                   hdr_hdl_sta_v6 = pHeaderDescriptor->hdr[0].hdr_hdl;
	                   IPACMDBG("add full header name: %s (%x)\n", pHeaderDescriptor->hdr[0].name, pHeaderDescriptor->hdr[0].hdr_hdl);
	                 }
	                   
	                 /* copy ipv6 full header to each TX endpoint property*/ 
	                 for (tx_index = 0; tx_index < iface_query->num_tx_props; tx_index++)
	                 {
	                    if(tx_prop->tx[tx_index].ip==IPA_IP_v6)
                        {	  
	                 	  memcpy(tx_prop->tx[tx_index].hdr_name, pHeaderDescriptor->hdr[0].name,
	                 				 sizeof(tx_prop->tx[tx_index].hdr_name));
	                 	  IPACMDBG("replace full header name: %s (%x) in tx:%d\n", tx_prop->tx[tx_index].hdr_name, pHeaderDescriptor->hdr[0].hdr_hdl,tx_index);
                        }
                     }	
	                 break;		   
	        }
	}	
                     
                     
    /* see if default routes are setup before constructing full header */
    if(header_partial_default_wan_v4 == true)
	{ 
	   handle_route_add_evt(IPA_IP_v4);	
	}
    
    if(header_partial_default_wan_v6 == true)
	{ 
	   handle_route_add_evt(IPA_IP_v6);	
	}
	
fail:
	free(pHeaderDescriptor);

	return res;
}

/* configure the initial firewall filter rules */
int IPACM_Wan::config_dft_firewall_rules(ipa_ip_type iptype)
{
	struct ipa_flt_rule_add flt_rule_entry;
	int i, rule_v4 = 0, rule_v6 = 0;

	IPACMDBG("ip-family: %d; \n", iptype);
	
	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}
		
	/* default firewall is disable and the rule action is drop */
	memset(&firewall_config, 0, sizeof(firewall_config));
	strncpy(firewall_config.firewall_config_file, "/etc/mobileap_firewall.xml", sizeof(firewall_config.firewall_config_file));

	if (firewall_config.firewall_config_file)
	{
		IPACMDBG("Firewall XML file is %s \n", firewall_config.firewall_config_file);
		if (IPACM_SUCCESS == IPACM_read_firewall_xml(firewall_config.firewall_config_file, &firewall_config))
		{
			IPACMDBG("QCMAP Firewall XML read OK \n");
	/* find the number of v4/v6 firewall rules */
	for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
	{
		if (firewall_config.extd_firewall_entries[i].ip_vsn == 4)
		{
			rule_v4++;
		}
		else
		{
			rule_v6++;
		}
	}
	IPACMDBG("firewall rule v4:%d v6:%d total:%d\n", rule_v4, rule_v6, firewall_config.num_extd_firewall_entries);
		}
		else
		{
			IPACMERR("QCMAP Firewall XML read failed, no that file, use default configuration \n");
		}
	}
	else
	{
		IPACMERR("No firewall xml mentioned \n");
		return IPACM_FAILURE;
	}

	/* construct ipa_ioc_add_flt_rule with N firewall rules */
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	if (iptype == IPA_IP_v4)
	{
		if (rule_v4 == 0)
		{
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								1 * sizeof(struct ipa_flt_rule_add));

			if (!m_pFilteringTable)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}
			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v4;
			m_pFilteringTable->num_rules = (uint8_t)1;

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_lan_v4) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			/* firewall disable, all traffic are allowed */
                        if(firewall_config.firewall_enable == true)
			{
			    flt_rule_entry.at_rear = true;			

			     /* default action for v4 is go DST_NAT unless user set to exception*/
                             if(firewall_config.rule_action_accept == true)
			    {
			        flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;			
			    }
			    else
			    {
			        flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
                            } 
		        }
			else
			{
			  flt_rule_entry.at_rear = true;
			flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
                        } 								

			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
			free(m_pFilteringTable);
		}
		else
		{
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								1 * sizeof(struct ipa_flt_rule_add)
								);

			if (m_pFilteringTable == NULL)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}

			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v4;
			m_pFilteringTable->num_rules = (uint8_t)1;

			IPACMDBG("Retreiving Routing handle for routing table name:%s\n",
							 IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.name);
			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_lan_v4))
			{
				IPACMERR("m_routing.GetRoutingTable(&rt_tbl_lan_v4=0x%p) Failed.\n", &IPACM_Iface::ipacmcfg->rt_tbl_lan_v4);
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			IPACMDBG("Routing handle for wan routing table:0x%x\n", IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl);

            if(firewall_config.firewall_enable == true)
            {			
			rule_v4 = 0;
			for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
			{
				if (firewall_config.extd_firewall_entries[i].ip_vsn == 4)
				{
					memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		    			flt_rule_entry.at_rear = true;
					flt_rule_entry.flt_rule_hdl = -1;
					flt_rule_entry.status = -1;
					flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;

					/* Accept v4 matched rules*/
                                        if(firewall_config.rule_action_accept == true)
			                {
			                     flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;			
			                }
			                else
			                {
			                     flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
                                        } 
					
					memcpy(&flt_rule_entry.rule.attrib,
								 &firewall_config.extd_firewall_entries[i].attrib,
								 sizeof(struct ipa_rule_attrib));

					IPACMDBG("rx property attrib mask: 0x%x\n", rx_prop->rx[0].attrib.attrib_mask);
					flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
					flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
					flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;

					/* check if the rule is define as TCP_UDP, split into 2 rules, 1 for TCP and 1 UDP */
					if (firewall_config.extd_firewall_entries[i].attrib.u.v4.protocol
							== IPACM_FIREWALL_IPPROTO_TCP_UDP)
					{
						/* insert TCP rule*/
						flt_rule_entry.rule.attrib.u.v4.protocol = IPACM_FIREWALL_IPPROTO_TCP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v4 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}

						/* insert UDP rule*/
						flt_rule_entry.rule.attrib.u.v4.protocol = IPACM_FIREWALL_IPPROTO_UDP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v4 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}
					}
					else
					{
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

						IPACMDBG("Filter rule attrib mask: 0x%x\n",
										 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v4 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n",
											 m_pFilteringTable->rules[rule_v4].flt_rule_hdl,
											 m_pFilteringTable->rules[rule_v4].status);
							firewall_hdl_v4[rule_v4] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v4++;
							rule_v4++;
						}
					}
				}
			} /* end of firewall ipv4 filter rule add for loop*/
            }
			/* configure default filter rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;

			/* firewall disable, all traffic are allowed */
                        if(firewall_config.firewall_enable == true)
			{
			     flt_rule_entry.at_rear = true;			

			     /* default action for v4 is go DST_NAT unless user set to exception*/
                             if(firewall_config.rule_action_accept == true)
			     {
			        flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;			
			     }
			     else
			     {
			       flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
                             } 
		        }
			else
			{
			    flt_rule_entry.at_rear = true;
			flt_rule_entry.rule.action = IPA_PASS_TO_DST_NAT;
                        } 
			
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_lan_v4.hdl;
			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
			flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
			flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0x00000000;
			flt_rule_entry.rule.attrib.u.v4.dst_addr = 0x00000000;

			memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

			IPACMDBG("Filter rule attrib mask: 0x%x\n",
							 m_pFilteringTable->rules[0].rule.attrib.attrib_mask);
			if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
			{
				IPACMERR("Error Adding RuleTable(0) to Filtering, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
			free(m_pFilteringTable);
		}

	}
	else
	{
		if (rule_v6 == 0)
		{
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								1 * sizeof(struct ipa_flt_rule_add));


			if (!m_pFilteringTable)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}
			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)1;

			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));
			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v6)) //rt_tbl_wan_v6 rt_tbl_v6
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_wan_v6) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;

			/* firewall disable, all traffic are allowed */
                        if(firewall_config.firewall_enable == true)
			{
			   flt_rule_entry.at_rear = true;			

			   /* default action for v6 is PASS_TO_ROUTE unless user set to exception*/
                           if(firewall_config.rule_action_accept == true)
			   {
			       flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;			
			   }
			   else
			   {
			       flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
                           } 
		        }
			else
			{
			  flt_rule_entry.at_rear = true;
			  flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
                        } 			

			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
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
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}

			/* copy filter hdls */
			dft_wan_fl_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
			free(m_pFilteringTable);
		}
		else
		{
			m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)
				 calloc(1,
								sizeof(struct ipa_ioc_add_flt_rule) +
								1 * sizeof(struct ipa_flt_rule_add)
								);

			if (!m_pFilteringTable)
			{
				IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
				return IPACM_FAILURE;
			}
			m_pFilteringTable->commit = 1;
			m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
			m_pFilteringTable->global = false;
			m_pFilteringTable->ip = IPA_IP_v6;
			m_pFilteringTable->num_rules = (uint8_t)1;

			if (false == m_routing.GetRoutingTable(&IPACM_Iface::ipacmcfg->rt_tbl_wan_v6))
			{
				IPACMERR("m_routing.GetRoutingTable(rt_tbl_wan_v6) Failed.\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}

            if(firewall_config.firewall_enable == true)
            {					    	
			rule_v6 = 0;
			for (i = 0; i < firewall_config.num_extd_firewall_entries; i++)
			{
				if (firewall_config.extd_firewall_entries[i].ip_vsn == 6)
				{
					memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		    			flt_rule_entry.at_rear = true;
					flt_rule_entry.flt_rule_hdl = -1;
					flt_rule_entry.status = -1;

				    /* matched rules for v6 go PASS_TO_ROUTE */
                                    if(firewall_config.rule_action_accept == true)
			            {
			                flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
					flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;
			            }
			            else
			            {
					flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
                                    } 										
					
					memcpy(&flt_rule_entry.rule.attrib,
								 &firewall_config.extd_firewall_entries[i].attrib,
								 sizeof(struct ipa_rule_attrib));
					flt_rule_entry.rule.attrib.attrib_mask |= rx_prop->rx[0].attrib.attrib_mask;
					flt_rule_entry.rule.attrib.meta_data_mask = rx_prop->rx[0].attrib.meta_data_mask;
					flt_rule_entry.rule.attrib.meta_data = rx_prop->rx[0].attrib.meta_data;

					/* check if the rule is define as TCP/UDP */
					if (firewall_config.extd_firewall_entries[i].attrib.u.v6.next_hdr == IPACM_FIREWALL_IPPROTO_TCP_UDP)
					{
						/* insert TCP rule*/
						flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_TCP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding Filtering rules, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v4 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
							firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v6++;
							rule_v6++;
						}

						/* insert UDP rule*/
						flt_rule_entry.rule.attrib.u.v6.next_hdr = IPACM_FIREWALL_IPPROTO_UDP;
						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding Filtering rules, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v6 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
							firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v6++;
							rule_v6++;
						}
					}
					else
					{

						memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));
						if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
						{
							IPACMERR("Error Adding Filtering rules, aborting...\n");
							free(m_pFilteringTable);
							return IPACM_FAILURE;
						}
						else
						{
							/* save v6 firewall filter rule handler */
							IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
							firewall_hdl_v6[rule_v6] = m_pFilteringTable->rules[0].flt_rule_hdl;
							num_firewall_v6++;
							rule_v6++;
						}
					}
				}
			} /* end of firewall ipv6 filter rule add for loop*/
            }

			/* setup default wan filter rule */
			memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

			flt_rule_entry.flt_rule_hdl = -1;
			flt_rule_entry.status = -1;
			flt_rule_entry.rule.rt_tbl_hdl = IPACM_Iface::ipacmcfg->rt_tbl_wan_v6.hdl;

			/* firewall disable, all traffic are allowed */
                        if(firewall_config.firewall_enable == true)
			{
			   flt_rule_entry.at_rear = true;			

			   /* default action for v6 is PASS_TO_ROUTE unless user set to exception*/
               if(firewall_config.rule_action_accept == true)
			   {
			        flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;			
			   }
			   else
			   {
			flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
                           } 
		        }
			else
			{
			  flt_rule_entry.at_rear = true;
			  flt_rule_entry.rule.action = IPA_PASS_TO_ROUTING;
                        } 			

			memcpy(&flt_rule_entry.rule.attrib,
						 &rx_prop->rx[0].attrib,
						 sizeof(struct ipa_rule_attrib));
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
				IPACMERR("Error Adding Filtering rules, aborting...\n");
				free(m_pFilteringTable);
				return IPACM_FAILURE;
			}
			else
			{
				IPACMDBG("flt rule hdl0=0x%x, status=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl, m_pFilteringTable->rules[0].status);
			}
			/* copy filter hdls*/
			dft_wan_fl_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
			free(m_pFilteringTable);
		}
	}
	return IPACM_SUCCESS;
}



/*handle wan-iface down event */
int IPACM_Wan::handle_down_evt()
{
	int res = IPACM_SUCCESS;
	int i;

	IPACMDBG(" wan handle_down_evt \n");

	/* no iface address up, directly close iface*/
	if (ip_type == IPACM_IP_NULL)
	{
		goto fail;
	}

	/* make sure default routing rules and firewall rules are deleted*/
	if (active_v4)
	{
	   	if (rx_prop != NULL)
	        {
		del_dft_firewall_rules(IPA_IP_v4);
		}
		handle_route_del_evt(IPA_IP_v4);
		IPACMDBG("Delete default v4 routing rules\n");
	}

	if (active_v6)
	{
	   	if (rx_prop != NULL)
	        {
		del_dft_firewall_rules(IPA_IP_v6);
		}
		handle_route_del_evt(IPA_IP_v6);
		IPACMDBG("Delete default v6 routing rules\n");
	}

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

	/* delete default v6 RT rule */
	if (ip_type != IPA_IP_v4)
	{
		IPACMDBG("Delete default v6 routing rules\n");
		/* May have multiple ipv6 iface-routing rules*/
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

		IPACMDBG("finished delete default v6 RT rules\n ");
	}

	/* check software routing fl rule hdl */
	if (softwarerouting_act == true)
	{
		handle_software_routing_disable();
	}

	/* free filter rule handlers */
	if (ip_type != IPA_IP_v6 && rx_prop != NULL)
	{
		if (m_filtering.DeleteFilteringHdls(dft_v4fl_rule_hdl,
																				IPA_IP_v4,
																				IPV4_DEFAULT_FILTERTING_RULES) == false)
		{
			IPACMERR("Error Delete Filtering rules, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("finished delete default v4 filtering rules\n ");
	}


	if (ip_type != IPA_IP_v4 && rx_prop != NULL)
	{
		if (m_filtering.DeleteFilteringHdls(dft_v6fl_rule_hdl,
																				IPA_IP_v6,
																				IPV6_DEFAULT_FILTERTING_RULES) == false)
		{
			IPACMERR("ErrorDeleting Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("finished delete default v6 filtering rules\n ");
	}

	
	/* delete the complete header for STA mode*/
        if((header_set_v4 == true) || (header_set_v6 == true))
        {
                if (m_header.DeleteHeaderHdl(hdr_hdl_sta_v4) == false)
		{
		      IPACMERR("ErrorDeleting STA header for v4, aborting...\n");
		  	res = IPACM_FAILURE;
		  	goto fail;
		  }
		  
		  if (m_header.DeleteHeaderHdl(hdr_hdl_sta_v6) == false)
		  {
		     IPACMERR("ErrorDeleting STA header for v6, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
	}

fail:
	free(tx_prop);
	free(rx_prop);
	free(iface_query);

	return res;
}

/*clean firewall filter rules */
int IPACM_Wan::del_dft_firewall_rules(ipa_ip_type iptype)
{
	/* free v4 firewall filter rule */
	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}
	
	
	if ((iptype == IPA_IP_v4) && (active_v4 == true))
	{
		if (num_firewall_v4 != 0)
		{
			if (m_filtering.DeleteFilteringHdls(firewall_hdl_v4,
																					IPA_IP_v4, num_firewall_v4) == false)
			{
				IPACMERR("Error Deleting Filtering rules, aborting...\n");
				return IPACM_FAILURE;
			}
		}
		else
		{
			IPACMDBG("No ipv4 firewall rules, no need deleted\n");
		}

		if (m_filtering.DeleteFilteringHdls(dft_wan_fl_hdl,
																				IPA_IP_v4, 1) == false)
		{
			IPACMERR("Error Deleting Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}

		num_firewall_v4 = 0;
	}

	/* free v6 firewall filter rule */
	if ((iptype == IPA_IP_v6) && (active_v6 == true))
	{
		if (num_firewall_v6 != 0)
		{
			if (m_filtering.DeleteFilteringHdls(firewall_hdl_v6,
																					IPA_IP_v6, num_firewall_v6) == false)
			{
				IPACMERR("Error Deleting Filtering rules, aborting...\n");
				return IPACM_FAILURE;
			}
		}
		else
		{
			IPACMDBG("No ipv6 firewall rules, no need deleted\n");
		}

		if (m_filtering.DeleteFilteringHdls(&dft_wan_fl_hdl[1],
																				IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error Deleting Filtering rules, aborting...\n");
			return IPACM_FAILURE;
		}
		num_firewall_v6 = 0;
	}

	return IPACM_SUCCESS;
}
