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
	IPACM_Config.h

	@brief
	This file implements the IPACM Configuration from XML file

	@Author
	Skylar Chang

*/
#ifndef IPACM_CONFIG_H
#define IPACM_CONFIG_H

#include "IPACM_Defs.h"
#include "IPACM_Xml.h"


typedef struct
{
  char iface_name[IPA_IFACE_NAME_LEN];
	uint32_t ipv4_addr;
}NonNatIfaces;

/* iface */
class IPACM_Config
{
public:

	/* Store interested interface and their configuration from XML file */
	ipa_ifi_dev_name_t *iface_table;

	/* Store interested ALG port from XML file */
	ipacm_alg *alg_table;

	/* Store private subnet configuration from XML file */
	ipa_private_subnet private_subnet_table[IPA_MAX_PRIVATE_SUBNET_ENTRIES];

	/* Store the non nat iface names */
	NonNatIfaces *pNonNatIfaces; 

	/* Store the number of interface IPACM read from XML file */
	int ipa_num_ipa_interfaces;

	int ipa_num_private_subnet;

	int ipa_num_alg_ports;

	int ipa_nat_max_entries;

	int ipa_non_nat_iface_entries;

	/* IPACM routing table name for v4/v6 */
	struct ipa_ioc_get_rt_tbl rt_tbl_lan_v4, rt_tbl_wan_v4, rt_tbl_default_v4, rt_tbl_v6, rt_tbl_wan_v6;

	/* To return the instance */
	static IPACM_Config* GetInstance();

	inline int GetAlgPortCnt()
	{
		return ipa_num_alg_ports;
	}

	int GetAlgPorts(int nPorts, ipacm_alg *pAlgPorts);
	inline int GetNatMaxEntries(void)
	{
		return ipa_nat_max_entries;
	}

	inline int GetNonNatIfacesCnt()
	{
		return ipa_non_nat_iface_entries;
	}
	int GetNonNatIfaces(int nPorts, NonNatIfaces *ifaces);

private:
	static IPACM_Config *pInstance;
	IPACM_Config(void);
	int Init(void);

};

#endif /* IPACM_CONFIG */
