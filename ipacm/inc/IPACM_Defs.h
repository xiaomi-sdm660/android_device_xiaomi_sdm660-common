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
	IPACM_Defs.h

	@brief
	This file implements the common definitions amon all ifaces.

	@Author
	Skylar Chang

*/
#ifndef IPA_CM_DEFS_H
#define IPA_CM_DEFS_H

#include <linux/msm_ipa.h>
#include "IPACM_Log.h"

extern "C"
{
#include <libnetfilter_conntrack/libnetfilter_conntrack.h>
#include <libnetfilter_conntrack/libnetfilter_conntrack_tcp.h>
}


#define IPA_MAX_FILE_LEN  64
#define IPA_IFACE_NAME_LEN 16
#define IPA_ALG_PROTOCOL_NAME_LEN  10

#define IPA_WLAN_PARTIAL_HDR_OFFSET  12 // dst mac first then src mac
#define IPA_VIRTUAL_IFACE_NAME "bridge0"
#define IPA_WLAN_PARTIAL_HDR_NAME_v4  "IEEE802_3_v4"
#define IPA_WLAN_PARTIAL_HDR_NAME_v6  "IEEE802_3_v6"
#define IPA_WAN_PARTIAL_HDR_NAME_v4  "IEEE802_3_STA_v4"
#define IPA_WAN_PARTIAL_HDR_NAME_v6  "IEEE802_3_STA_v6"
#define IPA_MAX_IFACE_ENTRIES 15
#define IPA_MAX_PRIVATE_SUBNET_ENTRIES 3
#define IPA_MAX_ALG_ENTRIES 10
#define IPA_MAX_RM_ENTRY 6

#define V4_DEFAULT_ROUTE_TABLE_NAME  "ipa_dflt_rt"
#define V4_LAN_ROUTE_TABLE_NAME  "COMRTBLLANv4"
#define V4_WAN_ROUTE_TABLE_NAME  "WANRTBLv4"
#define V6_COMMON_ROUTE_TABLE_NAME  "COMRTBLv6"
#define V6_WAN_ROUTE_TABLE_NAME  "WANRTBLv6"



/*---------------------------------------------------------------------------
										Return values indicating error status
---------------------------------------------------------------------------*/

#define IPACM_SUCCESS                0         /* Successful operation   */
#define IPACM_FAILURE               -1         /* Unsuccessful operation */

#define IPACM_IP_NULL (ipa_ip_type)0xFF
#define IPACM_INVALID_INDEX (ipa_ip_type)0xFF

#define IPA_MAX_NUM_WIFI_CLIENTS  15
#define IPA_MAX_NUM_AMPDU_RULE  15
#define IPA_MAC_ADDR_SIZE  6

/*===========================================================================
										 GLOBAL DEFINITIONS AND DECLARATIONS
===========================================================================*/
typedef enum
{
	IPA_LINK_UP_EVENT = 1,                    /* 1 ipacm_event_data_fid */
	IPA_LINK_DOWN_EVENT,                      /* 2 ipacm_event_data_fid */
	IPA_ADDR_ADD_EVENT,                       /* 3 ipacm_event_data_addr */
	IPA_ADDR_DEL_EVENT,                       /* 4 no use */
	IPA_ROUTE_ADD_EVENT,                      /* 5 ipacm_event_data_addr */
	IPA_ROUTE_DEL_EVENT,                      /* 6 ipacm_event_data_addr */
	IPA_FIREWALL_CHANGE_EVENT,                /* 7 NULL */
	IPA_WLAN_AP_LINK_UP_EVENT,                /* 8 ipacm_event_data_mac */
	IPA_WLAN_STA_LINK_UP_EVENT,               /* 9 ipacm_event_data_mac */
	IPA_WLAN_CLIENT_ADD_EVENT,                /* 10 ipacm_event_data_mac */
	IPA_WLAN_CLIENT_DEL_EVENT,                /* 11 ipacm_event_data_mac */
	IPA_WLAN_CLIENT_POWER_SAVE_EVENT,         /* 12 ipacm_event_data_mac */
	IPA_WLAN_CLIENT_RECOVER_EVENT,            /* 13 ipacm_event_data_mac */
	IPA_NEW_NEIGH_EVENT,                      /* 14 ipacm_event_data_all */
	IPA_DEL_NEIGH_EVENT,                      /* 15 ipacm_event_data_all */
	IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT,       /* 16 ipacm_event_data_all */
	IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT,       /* 17 ipacm_event_data_all */
	IPA_SW_ROUTING_ENABLE,                    /* 18 NULL */
	IPA_SW_ROUTING_DISABLE,                   /* 19 NULL */
	IPA_PROCESS_CT_MESSAGE,                   /* 20 ipacm_ct_evt_data */
	IPA_HANDLE_WAN_UP,                        /* 21 ipacm_event_iface_up  */
	IPA_HANDLE_WAN_DOWN,                      /* 22 unsigned long  */
	IPA_HANDLE_WLAN_UP,                       /* 23 ipacm_event_iface_up */
	IPA_HANDLE_LAN_UP                         /* 24 ipacm_event_iface_up */
} ipa_cm_event_id;

typedef enum
{
	LAN_IF = 0,
	WLAN_IF,
	WAN_IF,
	VIRTUAL_IF,
	UNKNOWN_IF
} ipacm_iface_type;

typedef struct
{
	struct nf_conntrack *ct;
	enum nf_conntrack_msg_type type;
}ipacm_ct_evt_data;

typedef struct
{
	char iface_name[IPA_IFACE_NAME_LEN];
	ipacm_iface_type if_cat;
	int netlink_interface_index;
} ipa_ifi_dev_name_t;

typedef struct
{
	uint32_t subnet_addr;
	uint32_t subnet_mask;
} ipa_private_subnet;


typedef struct _ipacm_event_data_all
{
	enum ipa_ip_type iptype;
	int if_index;
	uint32_t  ipv4_addr;
	uint32_t  ipv6_addr[4];
	uint8_t mac_addr[6];
} ipacm_event_data_all;

typedef struct _ipacm_event_data_fid
{
	int if_index;
} ipacm_event_data_fid;

typedef struct _ipacm_event_data_addr
{
	enum ipa_ip_type iptype;
	int if_index;
	uint32_t  ipv4_addr;
	uint32_t  ipv4_addr_mask;
	uint32_t  ipv6_addr[4];
	uint32_t  ipv6_addr_mask[4];
} ipacm_event_data_addr;

typedef struct _ipacm_event_data_mac
{
	int if_index;
	uint8_t mac_addr[6];
} ipacm_event_data_mac;

typedef struct _ipacm_event_iface_up
{
	char ifname[IPA_IFACE_NAME_LEN];
	uint32_t ipv4_addr;
	uint32_t addr_mask;
}ipacm_event_iface_up;

#endif /* IPA_CM_DEFS_H */
