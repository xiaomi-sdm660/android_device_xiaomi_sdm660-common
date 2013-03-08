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
#include "IPACM_Conntrack_NATApp.h"
#include "IPACM_ConntrackClient.h"

#define UDP_TIMEOUT_VALUE 30

/* NatApp class Implementation */
NatApp *NatApp::pInstance = NULL;
NatApp::NatApp()
{
	max_entries = 0;
	cache = NULL;

	nat_table_hdl = 0;
	pub_ip_addr = 0;

	curCnt = 0;

	pALGPorts = NULL;
	nALGPort = 0;

	ct = NULL;
	ct_hdl = NULL;
}

int NatApp::Init(void)
{
	IPACM_Config *pConfig;
	int size = 0;

	pConfig = IPACM_Config::GetInstance();
	if(pConfig == NULL)
	{
		IPACMERR("Unable to get Config instance\n");
		return -1;
	}

	max_entries = pConfig->GetNatMaxEntries();

	size = (sizeof(nat_table_entry) * max_entries);
	cache = (nat_table_entry *)malloc(size);
	if(cache == NULL)
	{
		IPACMERR("Unable to allocate memory for cache\n");
		goto fail;
	}
	IPACMDBG("Allocated %d bytes for config manager nat cache\n", size);
	memset(cache, 0, size);

	nALGPort = pConfig->GetAlgPortCnt();
	pALGPorts = (ipacm_alg *)malloc(sizeof(ipacm_alg) * nALGPort);
	if(pALGPorts == NULL)
	{
		IPACMERR("Unable to allocate memory for alg prots\n");
		goto fail;
	}
	memset(pALGPorts, 0, sizeof(ipacm_alg) * nALGPort);

	if(pConfig->GetAlgPorts(nALGPort, pALGPorts) != 0)
	{
		IPACMERR("Unable to retrieve ALG prots\n");
		goto fail;
	}

	IPACMDBG("Printing %d alg ports information\n", nALGPort);
	for(int cnt=0; cnt<nALGPort; cnt++)
	{
		IPACMDBG("%d: Proto[%d], port[%d]\n", cnt, pALGPorts[cnt].protocol, pALGPorts[cnt].port);
	}

	return 0;

fail:
	free(cache);
	free(pALGPorts);
	return -1;
}

NatApp* NatApp::GetInstance()
{
	if(pInstance == NULL)
	{
		pInstance = new NatApp();

		if(pInstance->Init())
		{
			delete pInstance;
			return NULL;
		}
	}

	return pInstance;
}

/* NAT APP related object function definitions */

int NatApp::AddTable(uint32_t pub_ip)
{
	int ret;
	IPACMDBG("%s() %d\n", __FUNCTION__, __LINE__);

	ret = ipa_nat_add_ipv4_tbl(pub_ip, max_entries, &nat_table_hdl);
	if(ret)
	{
		IPACMERR("unable to create nat table Error:%d\n", ret);
		return ret;
	}

	pub_ip_addr = pub_ip;
	return 0;
}

void NatApp::Reset()
{
	memset(cache, 0, sizeof(nat_table_entry) * max_entries);

	nat_table_hdl = 0;
	pub_ip_addr = 0;
	curCnt = 0;
}

int NatApp::DeleteTable(uint32_t pub_ip)
{
	int ret;
	IPACMDBG("%s() %d\n", __FUNCTION__, __LINE__);

	CHK_TBL_HDL();

	if(pub_ip_addr != pub_ip)
	{
		IPACMDBG("Public ip address is not matching\n");
		IPACMERR("unable to delete the nat table\n");
		return -1;
	}

	ret = ipa_nat_del_ipv4_tbl(nat_table_hdl);
	if(ret)
	{
		IPACMERR("unable to delete nat table Error: %d\n", ret);;
		return ret;
	}

	Reset();
	return 0;
}

/* Check for duplicate entries */
bool NatApp::ChkForDup(const nat_table_entry *rule)
{
	int cnt = 0;
	IPACMDBG("%s() %d\n", __FUNCTION__, __LINE__);

	for(; cnt < curCnt; cnt++)
	{
		if(cache[cnt].private_ip == rule->private_ip &&
			 cache[cnt].target_ip == rule->target_ip &&
			 cache[cnt].private_port ==  rule->private_port  &&
			 cache[cnt].target_port == rule->target_port &&
			 cache[cnt].protocol == rule->protocol)
		{
			IPACMDBG("Duplicate Rule\n");
			return true;
		}
	}

	return false;
}

/* Delete the entry from Nat table on connection close */
int NatApp::DeleteEntry(const nat_table_entry *rule)
{
	int cnt = 0;
	IPACMDBG("%s() %d\n", __FUNCTION__, __LINE__);

	CHK_TBL_HDL();

#ifdef IPACM_DEBUG
	IPACM_ConntrackClient::iptodot("Private IP", rule->private_ip);
	IPACM_ConntrackClient::iptodot("Target IP", rule->target_ip);
	IPACMDBG("Private Port: %d\t Target Port: %d\t", rule->private_port, rule->target_port);
	IPACMDBG("protocolcol: %d\n", rule->protocol);
#endif

	for(; cnt < max_entries; cnt++)
	{
		if(cache[cnt].private_ip == rule->private_ip &&
			 cache[cnt].target_ip == rule->target_ip &&
			 cache[cnt].private_port ==  rule->private_port  &&
			 cache[cnt].target_port == rule->target_port &&
			 cache[cnt].protocol == rule->protocol)
		{

			if(ipa_nat_del_ipv4_rule(nat_table_hdl, cache[cnt].rule_hdl) < 0)
			{
				IPACMERR("%s() %d\n", __FUNCTION__, __LINE__);
				return -1;
			}

			memset(&cache[cnt], 0, sizeof(cache[cnt]));
			curCnt--;
		}
	}

	return 0;
}

/* Add new entry to the nat table on new connection */
int NatApp::AddEntry(const nat_table_entry *rule)
{

	int cnt = 0;
	ipa_nat_ipv4_rule nat_rule;
	IPACMDBG("%s() %d\n", __FUNCTION__, __LINE__);

	CHK_TBL_HDL();

	if(isAlgPort(rule->protocol, rule->private_port) ||
		 isAlgPort(rule->protocol, rule->target_port))
	{
		IPACMERR("connection using ALG Port. Dont insert into nat table\n");
		return -1;
	}

	if(isPwrSaveIf(rule->private_ip) ||
		 isPwrSaveIf(rule->target_ip))
	{
		IPACMERR("Device is Power Save mode: Dont insert into nat table\n");
		return -1;
	}

	if(!ChkForDup(rule))
	{
		for(; cnt < max_entries; cnt++)
		{
			if(cache[cnt].private_ip == 0 &&
				 cache[cnt].target_ip == 0 &&
				 cache[cnt].private_port == 0  &&
				 cache[cnt].target_port == 0 &&
				 cache[cnt].protocol == 0)
			{
				break;
			}
		}

		if(max_entries == cnt)
		{
			IPACMERR("Error: Unable to add, reached maximum rules\n");
			return -1;
		}
		else
		{
			nat_rule.private_ip = rule->private_ip;
			nat_rule.target_ip = rule->target_ip;
			nat_rule.target_port = rule->target_port;
			nat_rule.private_port = rule->private_port;
			nat_rule.public_port = rule->public_port;
			nat_rule.protocol = rule->protocol;

			if(ipa_nat_add_ipv4_rule(nat_table_hdl, &nat_rule, &cache[cnt].rule_hdl) < 0)
			{
				IPACMERR("unable to add the rule\n");
				return -1;
			}

			cache[cnt].private_ip = rule->private_ip;
			cache[cnt].target_ip = rule->target_ip;
			cache[cnt].target_port = rule->target_port;
			cache[cnt].private_port = rule->private_port;
			cache[cnt].protocol = rule->protocol;
			cache[cnt].timestamp = 0;
			cache[cnt].public_port = rule->public_port;
			cache[cnt].dst_nat = rule->dst_nat;
			curCnt++;
		}

	}
	else
	{
		IPACMERR("Duplicate rule. Ignore it\n");
		return -1;
	}


#ifdef IPACM_DEBUG
	IPACMDBG("Added below rule successfully\n");
	IPACM_ConntrackClient::iptodot("Private IP", rule->private_ip);
	IPACM_ConntrackClient::iptodot("Target IP", rule->target_ip);
	IPACMDBG("Private Port:%d \t Target Port: %d\t", rule->private_port, rule->target_port);
	IPACMDBG("Public Port:%d\n", rule->public_port);
	IPACMDBG("protocol: %d\n", rule->protocol);
#endif

	return 0;
}

void NatApp::UpdateCTUdpTs(nat_table_entry *rule, uint32_t new_ts)
{
	int ret;

#ifdef IPACM_DEBUG
	IPACM_ConntrackClient::iptodot("Private IP:", rule->private_ip);
	IPACM_ConntrackClient::iptodot("Target IP:",  rule->target_ip);
	IPACMDBG("Private Port: %d, Target Port: %d\n", rule->private_port, rule->target_port);
#endif

	if(!ct_hdl)
	{
		ct_hdl = nfct_open(CONNTRACK, 0);
		if(!ct_hdl)
		{
			PERROR("nfct_open");
			return;
		}
	}

	if(!ct)
	{
		ct = nfct_new();
		if(!ct)
		{
			PERROR("nfct_new");
			return;
		}
	}
	
	nfct_set_attr_u8(ct, ATTR_L3PROTO, AF_INET);
	nfct_set_attr_u8(ct, ATTR_L4PROTO, IPPROTO_UDP);
	nfct_set_attr_u32(ct, ATTR_TIMEOUT, UDP_TIMEOUT_VALUE);

	if(rule->dst_nat == false)
	{
		nfct_set_attr_u32(ct, ATTR_IPV4_SRC, htonl(rule->private_ip));
		nfct_set_attr_u16(ct, ATTR_PORT_SRC, htons(rule->private_port));

		nfct_set_attr_u32(ct, ATTR_IPV4_DST, htonl(rule->target_ip));
		nfct_set_attr_u16(ct, ATTR_PORT_DST, htons(rule->target_port));

		IPACMDBG("dst nat is not set\n");
	}
	else
	{
		nfct_set_attr_u32(ct, ATTR_IPV4_SRC, htonl(rule->target_ip));
		nfct_set_attr_u16(ct, ATTR_PORT_SRC, htons(rule->target_port));

		nfct_set_attr_u32(ct, ATTR_IPV4_DST, htonl(pub_ip_addr));
		nfct_set_attr_u16(ct, ATTR_PORT_DST, htons(rule->public_port));

		IPACMDBG("dst nat is set\n");
	}

	IPACM_ConntrackClient::iptodot("Source IP:", nfct_get_attr_u32(ct, ATTR_IPV4_SRC));
	IPACM_ConntrackClient::iptodot("Destination IP:",  nfct_get_attr_u32(ct, ATTR_IPV4_DST));
	IPACMDBG("Source Port: %d, Destination Port: %d\n",
					 nfct_get_attr_u16(ct, ATTR_PORT_SRC), nfct_get_attr_u16(ct, ATTR_PORT_DST)); 
	
	IPACMDBG("updating udp connection with time: %d\n", UDP_TIMEOUT_VALUE);
	ret = nfct_query(ct_hdl, NFCT_Q_UPDATE, ct);
	if(ret == -1)
	{
		PERROR("unable to update time stamp");
	}
	else
	{
		rule->timestamp = new_ts;
		IPACMDBG("Updated time stamp successfully\n");
	}

	return;
}

void NatApp::UpdateUDPTimeStamp()
{
	int cnt;
	uint32_t ts;

	for(cnt = 0; cnt < curCnt; cnt++)
	{
		ts = 0;
		if(IPPROTO_UDP == cache[cnt].protocol)
		{
			IPACMDBG("\n");
			if(ipa_nat_query_timestamp(nat_table_hdl, cache[cnt].rule_hdl, &ts) < 0)
			{
				IPACMERR("unable to retrieve timeout for rule hanle: %d\n", cache[cnt].rule_hdl);
				continue;
			}

			if(ts == cache[cnt].timestamp)
			{
				continue;
			}
			
			UpdateCTUdpTs(&cache[cnt], ts);
		} /* end of outer if */

	} /* end of for loop */

}

bool NatApp::isAlgPort(uint8_t proto, uint16_t port)
{
	int cnt;
	for(cnt = 0; cnt < nALGPort; cnt++)
	{
		if(proto == pALGPorts[cnt].protocol &&
			 port == pALGPorts[cnt].port)
		{
			return true;
		}
	}

	return false;
}

bool NatApp::isPwrSaveIf(uint32_t ip_addr)
{
	int cnt;

	for(cnt = 0; cnt < IPA_MAX_NUM_WIFI_CLIENTS; cnt++)
	{
		if(0 != PwrSaveIfs[cnt] &&
			 ip_addr == PwrSaveIfs[cnt])
		{
			return true;
		}
	}

	return false;
}

int NatApp::UpdatePwrSaveIf(uint32_t client_lan_ip)
{
	int cnt;

	CHK_TBL_HDL();

	for(cnt = 0; cnt < IPA_MAX_NUM_WIFI_CLIENTS; cnt++)
	{
		if(PwrSaveIfs[cnt] == 0)
		{
			PwrSaveIfs[cnt] = client_lan_ip;
		}
	}

	for(cnt = 0; cnt < curCnt; cnt++)
	{
		if(cache[cnt].private_ip == client_lan_ip)
		{
			if(ipa_nat_del_ipv4_rule(nat_table_hdl, cache[cnt].rule_hdl) < 0)
			{
				IPACMERR("unable to delete the rule\n");
				continue;
			}

			memset(&cache[cnt], 0, sizeof(cache[cnt]));
			curCnt--;
		}
	}

	return 0;
}

int NatApp::ResetPwrSaveIf(uint32_t client_lan_ip)
{
	int cnt;

	for(cnt = 0; cnt < IPA_MAX_NUM_WIFI_CLIENTS; cnt++)
	{
		if(PwrSaveIfs[cnt] == client_lan_ip)
		{
			PwrSaveIfs[cnt] = 0;
			return 0;
		}
	}

	return -1;
}



