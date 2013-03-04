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
#ifndef IPACM_WAN_H
#define IPACM_WAN_H

#include <stdio.h>
#include <IPACM_CmdQueue.h>
#include <linux/msm_ipa.h>
#include "IPACM_Routing.h"
#include "IPACM_Filtering.h"
#include <IPACM_Iface.h>
#include <IPACM_Defs.h>
#include <IPACM_Xml.h>

#define IPA_NUM_DEFAULT_WAN_FILTER_RULES 2

/* wan iface */
class IPACM_Wan : public IPACM_Iface
{

public:

	static bool wan_up;

	IPACM_Wan(int iface_index);
	~IPACM_Wan();

	static bool isWanUP()
	{
		return wan_up;
	}

	void event_callback(ipa_cm_event_id event,
											void *data);

private:
	uint32_t *wan_route_rule_v4_hdl;
	uint32_t *wan_route_rule_v6_hdl;
	uint32_t firewall_hdl_v4[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t firewall_hdl_v6[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t dft_wan_fl_hdl[IPA_NUM_DEFAULT_WAN_FILTER_RULES];
	int num_firewall_v4,num_firewall_v6;
	bool active_v4;
	bool active_v6;
	uint32_t wan_v4_addr;

	/* IPACM firewall Configuration file*/
	IPACM_firewall_conf_t firewall_config;

	/* handle new_address event */
	int handle_addr_evt(ipacm_event_data_addr *data);

	/* wan default route/filter rule configuration */
	int handle_route_add_evt(ipacm_event_data_addr *data);

	/* wan default route/filter rule delete */	
	int handle_route_del_evt(ipa_ip_type iptype);

	/* construct complete ethernet header */
	int handle_header_add_evt(uint8_t mac_addr[6]);

	/* configure the initial firewall filter rules */
	int config_dft_firewall_rules(ipa_ip_type iptype);

	/*clean firewall filter rules */
	int del_dft_firewall_rules(ipa_ip_type iptype);

	/*handle wan-iface down event */
	int handle_down_evt();

};

#endif /* IPACM_WAN_H */
