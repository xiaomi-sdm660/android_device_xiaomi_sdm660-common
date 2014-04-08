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
	IPACM_Lan.h

	@brief
	This file implements the LAN iface definitions

	@Author
	Skylar Chang

*/
#ifndef IPACM_LAN_H
#define IPACM_LAN_H

#include <stdio.h>
#include <linux/msm_ipa.h>

#include "IPACM_CmdQueue.h"
#include "IPACM_Iface.h"
#include "IPACM_Routing.h"
#include "IPACM_Filtering.h"
#include "IPACM_Config.h"

#define IPA_MAX_NUM_UNICAST_ROUTE_RULES  6
#define IPA_WAN_DEFAULT_FILTER_RULE_HANDLES  1
#define IPA_PRIV_SUBNET_FILTER_RULE_HANDLES  3
#define MAX_WAN_UL_FILTER_RULES 20

/* store each lan-iface unicast routing rule and its handler*/
struct ipa_lan_rt_rule
{
	ipa_ip_type ip;
	uint32_t v4_addr;
	uint32_t v4_addr_mask;
	uint32_t v6_addr[4];
	uint32_t rt_rule_hdl[0];
};

/* lan iface */
class IPACM_Lan : public IPACM_Iface
{
public:

	IPACM_Lan(int iface_index);
	~IPACM_Lan();

	/* store lan's wan-up filter rule handlers */
	uint32_t lan_wan_fl_rule_hdl[IPA_WAN_DEFAULT_FILTER_RULE_HANDLES];

	/* store private-subnet filter rule handlers */
	uint32_t private_fl_rule_hdl[IPA_PRIV_SUBNET_FILTER_RULE_HANDLES];

	/* LAN-iface's callback function */
	void event_callback(ipa_cm_event_id event,
											void *data);

	virtual int handle_wan_up(ipa_ip_type ip_type);
											
	/* configure filter rule for wan_up event*/
	virtual int handle_wan_up_ex(ipacm_ext_prop* ext_prop, ipa_ip_type iptype);

	/* delete filter rule for wan_down event*/
	virtual int handle_wan_down(bool is_sta_mode);
	
	/* delete filter rule for wan_down event*/
	virtual int handle_wan_down_v6(bool is_sta_mode);

	/* configure private subnet filter rules*/
	virtual int handle_private_subnet(ipa_ip_type iptype);

	/* handle new_address event*/
	int handle_addr_evt(ipacm_event_data_addr *data);

	/* install UL filter rule from Q6 */
	int handle_uplink_filter_rule(ipacm_ext_prop* prop, ipa_ip_type iptype);



private:

	/* dynamically allocate lan iface's unicast routing rule structure */
	int rt_rule_len;

	ipa_lan_rt_rule *route_rule;

	uint32_t v6_addr[IPV6_NUM_ADDR][4];

        int ipv6_set;

	uint32_t ETH_hdr_hdl_v4, ETH_hdr_hdl_v6;
	
	bool ipv4_header_set;
	
	bool ipv6_header_set;	
		
	/* store the number of lan-iface's unicast routing rule */
	int num_uni_rt;

	inline ipa_lan_rt_rule* get_rt_ruleptr(ipa_lan_rt_rule *param, int cnt)
	{
	    char *ret = ((char *)param) + (rt_rule_len * cnt);
	    return (ipa_lan_rt_rule *)ret;
	}

	/* handle unicast routing rule add event for ipv4 */
	int handle_route_add_evt(ipacm_event_data_addr *data);

	/* handle unicast routing rule add event for ipv6 */
	int handle_route_add_evt_v6(ipacm_event_data_all *data);

	/* handle unicast routing rule del event for ipv4 */
	int handle_route_del_evt(ipacm_event_data_addr *data);

	/* handle unicast routing rule del event for ipv6 */
	int handle_route_del_evt_v6(ipacm_event_data_all *data);

	/* handle ETH client initial, construct full headers (tx property) */
	int handle_eth_hdr_init(uint8_t *mac_addr);
	
	/*handle wlan iface down event*/
	int handle_down_evt();

	/* store ipv4 UL filter rule handlers from Q6*/
	uint32_t wan_ul_fl_rule_hdl_v4[MAX_WAN_UL_FILTER_RULES];

	/* store ipv6 UL filter rule handlers from Q6*/
	uint32_t wan_ul_fl_rule_hdl_v6[MAX_WAN_UL_FILTER_RULES];

	int num_wan_ul_fl_rule_v4;
	int num_wan_ul_fl_rule_v6;
};

#endif /* IPACM_LAN_H */
