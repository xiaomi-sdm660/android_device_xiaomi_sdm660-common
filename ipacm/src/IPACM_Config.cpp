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
		IPACM_Config.cpp

		@brief
		This file implements the IPACM Configuration from XML file

		@Author
		Skylar Chang

*/
#include <IPACM_Config.h>
#include <IPACM_Log.h>
#include <IPACM_Iface.h>

IPACM_Config *IPACM_Config::pInstance = NULL;

IPACM_Config::IPACM_Config()
{
	iface_table = NULL;
	alg_table = NULL;
	memset(&private_subnet_table, 0, sizeof(private_subnet_table));

	ipa_num_ipa_interfaces = 0;
	ipa_num_private_subnet = 0;
	ipa_num_alg_ports = 0;
	ipa_nat_max_entries = 0;

	memset(&rt_tbl_default_v4, 0, sizeof(rt_tbl_default_v4));
	memset(&rt_tbl_lan_v4, 0, sizeof(rt_tbl_lan_v4));
	memset(&rt_tbl_wan_v4, 0, sizeof(rt_tbl_wan_v4));
	memset(&rt_tbl_v6, 0, sizeof(rt_tbl_v6));
	memset(&rt_tbl_wan_v6, 0, sizeof(rt_tbl_wan_v6));

	IPACMDBG(" create IPACM_Config constructor\n");
	return;
}

int IPACM_Config::Init(void)
{
	/* Read IPACM Config file */
	char	IPACM_config_file[IPA_MAX_FILE_LEN];
	IPACM_conf_t	*cfg;
	cfg = (IPACM_conf_t *)malloc(sizeof(IPACM_conf_t));
	uint32_t subnet_addr;
	uint32_t subnet_mask;
	int i, ret = IPACM_SUCCESS;

	strncpy(IPACM_config_file, "/etc/IPACM_cfg.xml", sizeof(IPACM_config_file));


	IPACMDBG("\n IPACM XML file is %s \n", IPACM_config_file);
	if (IPACM_SUCCESS == ipacm_read_cfg_xml(IPACM_config_file, cfg))
	{
		IPACMDBG("\n IPACM XML read OK \n");
	}
	else
	{
		IPACMERR("\n IPACM XML read failed \n");
		ret = IPACM_FAILURE;
		goto fail;
	}

	/* Construct IPACM Iface table */
	ipa_num_ipa_interfaces = cfg->iface_config.num_iface_entries;
	iface_table = (ipa_ifi_dev_name_t *)calloc(ipa_num_ipa_interfaces,
																						 sizeof(ipa_ifi_dev_name_t));

	for (i = 0; i < cfg->iface_config.num_iface_entries; i++)
	{
		strncpy(iface_table[i].iface_name, cfg->iface_config.iface_entries[i].iface_name, sizeof(iface_table[i].iface_name));
		iface_table[i].if_cat = cfg->iface_config.iface_entries[i].if_cat;
		IPACMDBG("IPACM_Config::iface_table[%d] = %s, cat=%d\n", i, iface_table[i].iface_name, iface_table[i].if_cat);
	}

	/* Construct IPACM Private_Subnet table */
	ipa_num_private_subnet = cfg->private_subnet_config.num_subnet_entries;

	for (i = 0; i < cfg->private_subnet_config.num_subnet_entries; i++)
	{
		memcpy(&private_subnet_table[i].subnet_addr,
					 &cfg->private_subnet_config.private_subnet_entries[i].subnet_addr,
					 sizeof(cfg->private_subnet_config.private_subnet_entries[i].subnet_addr));

		memcpy(&private_subnet_table[i].subnet_mask,
					 &cfg->private_subnet_config.private_subnet_entries[i].subnet_mask,
					 sizeof(cfg->private_subnet_config.private_subnet_entries[i].subnet_mask));

		subnet_addr = htonl(private_subnet_table[i].subnet_addr);
		IPACMDBG("%dst::private_subnet_table= %s \n ", i,
						 inet_ntoa(*(struct in_addr *)&(subnet_addr)));

		subnet_mask =  htonl(private_subnet_table[i].subnet_mask);
		IPACMDBG("%dst::private_subnet_table= %s \n ", i,
						 inet_ntoa(*(struct in_addr *)&(subnet_mask)));
	}

	/* Construct IPACM ALG table */
	ipa_num_alg_ports = cfg->alg_config.num_alg_entries;
	alg_table = (ipacm_alg *)calloc(ipa_num_alg_ports,
																sizeof(ipacm_alg));

	for (i = 0; i < cfg->alg_config.num_alg_entries; i++)
	{
		//strncpy(alg_table[i].protocol, cfg->alg_config.alg_entries[i].protocol, sizeof(alg_table[i].protocol));
		alg_table[i].protocol = cfg->alg_config.alg_entries[i].protocol;
		alg_table[i].port = cfg->alg_config.alg_entries[i].port;
		IPACMDBG("IPACM_Config::ipacm_alg[%d] = %d, port=%d\n", i, alg_table[i].protocol, alg_table[i].port);
	}

	ipa_nat_max_entries = cfg->nat_max_entries;
	IPACMDBG("Nat Maximum Entries %d\n", ipa_nat_max_entries);

	/* Construct the routing table ictol name in iface static member*/
	rt_tbl_default_v4.ip = IPA_IP_v4;
	strncpy(rt_tbl_default_v4.name, V4_DEFAULT_ROUTE_TABLE_NAME, sizeof(rt_tbl_default_v4.name));

	rt_tbl_lan_v4.ip = IPA_IP_v4;
	strncpy(rt_tbl_lan_v4.name, V4_LAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_lan_v4.name));

	rt_tbl_wan_v4.ip = IPA_IP_v4;
	strncpy(rt_tbl_wan_v4.name, V4_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan_v4.name));

	rt_tbl_v6.ip = IPA_IP_v6;
	strncpy(rt_tbl_v6.name, V6_COMMON_ROUTE_TABLE_NAME, sizeof(rt_tbl_v6.name));

	rt_tbl_wan_v6.ip = IPA_IP_v6;
	strncpy(rt_tbl_wan_v6.name, V6_WAN_ROUTE_TABLE_NAME, sizeof(rt_tbl_wan_v6.name));

fail:
	free(cfg);

	return ret;
}

IPACM_Config* IPACM_Config::GetInstance()
{
	int res = IPACM_SUCCESS;

	if (pInstance == NULL)
	{
		pInstance = new IPACM_Config();

		res = pInstance->Init();
		if (res != IPACM_SUCCESS)
		{
			delete pInstance;
			return NULL;
		}
	}

	return pInstance;
}

int IPACM_Config::GetAlgPorts(int nPorts, ipacm_alg *pAlgPorts)
{
	if (nPorts <= 0 || pAlgPorts == NULL)
	{
		IPACMERR("Invalid input\n");
		return -1;
	}

	for (int cnt = 0; cnt < nPorts; cnt++)
	{
		pAlgPorts[cnt].protocol = alg_table[cnt].protocol;
		pAlgPorts[cnt].port = alg_table[cnt].port;
	}

	return 0;
}
