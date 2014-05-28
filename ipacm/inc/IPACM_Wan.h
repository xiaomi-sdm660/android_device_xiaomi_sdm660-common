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
#define IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV4 2
#define IPA_V2_NUM_DEFAULT_WAN_FILTER_RULE_IPV6 3

/* wan iface */
class IPACM_Wan : public IPACM_Iface
{

public:

	static bool wan_up;
	static bool wan_up_v6;

	IPACM_Wan(int iface_index, ipacm_wan_iface_type is_sta_mode);
	virtual ~IPACM_Wan();

	static bool isWanUP()
	{
		return wan_up;
	}

	static bool isWanUP_V6()
	{
		return wan_up_v6;
	}


	void event_callback(ipa_cm_event_id event,
											void *data);

	static struct ipa_flt_rule_add flt_rule_v4[IPA_MAX_FLT_RULE];
	static struct ipa_flt_rule_add flt_rule_v6[IPA_MAX_FLT_RULE];

	static int num_v4_flt_rule;
	static int num_v6_flt_rule;

	ipacm_wan_iface_type m_is_sta_mode;
	static bool backhaul_is_sta_mode;
	static bool is_ext_prop_set;

private:
	uint32_t *wan_route_rule_v4_hdl;
	uint32_t *wan_route_rule_v6_hdl;
	uint32_t *wan_route_rule_v6_hdl_a5;
	uint32_t hdr_hdl_sta_v4;
	uint32_t hdr_hdl_sta_v6;
	uint32_t firewall_hdl_v4[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t firewall_hdl_v6[IPACM_MAX_FIREWALL_ENTRIES];
	uint32_t dft_wan_fl_hdl[IPA_NUM_DEFAULT_WAN_FILTER_RULES];
	int num_firewall_v4,num_firewall_v6;
	uint32_t wan_v4_addr;
	bool active_v4;
	bool active_v6;
	bool header_set_v4;
	bool header_set_v6;
	bool header_partial_default_wan_v4;
	bool header_partial_default_wan_v6;

	static int num_ipv4_modem_pdn;

	static int num_ipv6_modem_pdn;

	int modem_ipv4_pdn_index;

	int modem_ipv6_pdn_index;

	bool is_default_gateway;

	/* IPACM firewall Configuration file*/
	IPACM_firewall_conf_t firewall_config;

	/* handle new_address event */
	int handle_addr_evt(ipacm_event_data_addr *data);

	/* wan default route/filter rule configuration */
	int handle_route_add_evt(ipa_ip_type iptype);

	/* construct complete ethernet header */
	int handle_header_add_evt(uint8_t mac_addr[6]);

	int config_dft_firewall_rules(ipa_ip_type iptype);

	int handle_route_del_evt(ipa_ip_type iptype);

	int del_dft_firewall_rules(ipa_ip_type iptype);

	int handle_down_evt();


	/*handle wan-iface down event */
	int handle_down_evt_ex();

	/* wan default route/filter rule delete */
	int handle_route_del_evt_ex(ipa_ip_type iptype);

	/* configure the initial firewall filter rules */
	int config_dft_firewall_rules_ex(struct ipa_flt_rule_add* rules, int rule_offset,
		ipa_ip_type iptype);

	/* init filtering rule in wan dl filtering table */
	int init_fl_rule_ex(ipa_ip_type iptype);

	/* add ICMP and ALG rules in wan dl filtering table */
	int add_icmp_alg_rules(struct ipa_flt_rule_add* rules, int rule_offset, ipa_ip_type iptype);

	/* query extended property */
	int query_ext_prop();

	ipa_ioc_query_intf_ext_props *ext_prop;

	int config_wan_firewall_rule(ipa_ip_type iptype);

	int del_wan_firewall_rule(ipa_ip_type iptype);

	int add_dft_filtering_rule(struct ipa_flt_rule_add* rules, int rule_offset, ipa_ip_type iptype);

	int install_wan_filtering_rule();

	void change_to_network_order(ipa_ip_type iptype, ipa_rule_attrib* attrib);

	int m_fd_ipa;

};

#endif /* IPACM_WAN_H */
