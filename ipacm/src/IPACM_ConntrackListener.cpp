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

#include <sys/ioctl.h>
#include <net/if.h>

#include "IPACM_ConntrackListener.h"
#include "IPACM_ConntrackClient.h"
#include "IPACM_EvtDispatcher.h"

IPACM_ConntrackListener::IPACM_ConntrackListener()
{
	 IPACMDBG("\n");

	 isCTReg = false;
	 WanUp = false;
	 nat_inst = NatApp::GetInstance();

	 NatIfaceCnt = 0;
	 pNatIfaces = NULL;
	 pConfig = NULL;

	 memset(nat_iface_ipv4_addr, 0, sizeof(nat_iface_ipv4_addr));
	 memset(nonnat_iface_ipv4_addr, 0, sizeof(nonnat_iface_ipv4_addr));

	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_UP, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WAN_DOWN, this);
	 IPACM_EvtDispatcher::registr(IPA_PROCESS_CT_MESSAGE, this);
	 IPACM_EvtDispatcher::registr(IPA_HANDLE_WLAN_UP, this);
	 IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT, this);
	 IPACM_EvtDispatcher::registr(IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT, this);
}

void IPACM_ConntrackListener::event_callback(ipa_cm_event_id evt,
																						 void *data)
{
	 ipacm_event_iface_up *wan_down = NULL;

	 if(data == NULL)
	 {
		 IPACMERR("Invalid Data\n");
		 return;
	 }

	 switch(evt)
	 {
	 case IPA_PROCESS_CT_MESSAGE:
			IPACMDBG("Received IPA_PROCESS_CT_MESSAGE event\n");
			ProcessCTMessage(data);
			break;

	 case IPA_HANDLE_WAN_UP:
			IPACMDBG("Received IPA_HANDLE_WAN_UP event\n");
			if(!isWanUp())
			{
				TriggerWANUp(data);
			}
			break;

	 case IPA_HANDLE_WAN_DOWN:
			IPACMDBG("Received IPA_HANDLE_WAN_DOWN event\n");
			wan_down = (ipacm_event_iface_up *)data;
			if(isWanUp())
			{
				TriggerWANDown(wan_down->ipv4_addr);
			}
			break;

	/* if wlan or lan comes up after wan interface, modify
		 tcp/udp filters to ignore local wlan or lan connections */
	 case IPA_HANDLE_WLAN_UP:
	 case IPA_HANDLE_LAN_UP:
			IPACMDBG("Received event: %d\n", evt);
			if(isWanUp())
			{
				 IPACM_ConntrackClient::UpdateUDPFilters(data);
				 IPACM_ConntrackClient::UpdateTCPFilters(data);
			}
			break; 

	 case IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT:
		 {
			 IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT event\n");
			 HandleNeighIpAddrAddEvt(data);
		 }
		 break;

	 case IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT:
		 {
			 IPACMDBG("Received IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT event\n");
			 HandleNeighIpAddrDelEvt(data);
		 }
		 break;

	 default:
			IPACMDBG("Ignore cmd %d\n", evt);
			break;
	 }
}

void IPACM_ConntrackListener::HandleNeighIpAddrAddEvt(void *in_param)
{
	ipacm_event_data_all *data = (ipacm_event_data_all *)in_param;
	int fd = 0, len = 0, cnt, i, j;
	struct ifreq ifr;
	bool isNatIface = false;

	if(data->ipv4_addr == 0 || data->iptype != IPA_IP_v4)
	{
		IPACMDBG("Ignoring IPA_NEIGH_CLIENT_IP_ADDR_ADD_EVENT EVENT\n");
		return;
	}
	IPACMDBG("Received interface index %d with ip type:%d", data->if_index, data->iptype);
	IPACM_ConntrackClient::iptodot("and received ipv4 address", data->ipv4_addr);

	if(pConfig == NULL)
	{
		pConfig = IPACM_Config::GetInstance();
		if(pConfig == NULL)
		{
			IPACMERR("Unable to get Config instance\n");
			return;
		}
	}

	cnt = pConfig->GetNatIfacesCnt();
	if(NatIfaceCnt != cnt)
	{
		NatIfaceCnt = cnt;
		if(pNatIfaces != NULL)
		{
			free(pNatIfaces);
			pNatIfaces = NULL;
		}

		len = (sizeof(NatIfaces) * NatIfaceCnt);
		pNatIfaces = (NatIfaces *)malloc(len);
		if(pNatIfaces == NULL)
		{
			IPACMERR("Unable to allocate memory for non nat ifaces\n");
			return;
		}
		memset(pNatIfaces, 0, len);

		if(pConfig->GetNatIfaces(NatIfaceCnt, pNatIfaces) != 0)
		{
			IPACMERR("Unable to retrieve non nat ifaces\n");
			return;
		}

		IPACMDBG("Update %d Nat ifaces", NatIfaceCnt);
	}

	/* Search/Configure linux interface-index and map it to IPA interface-index */
	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		PERROR("get interface name socket create failed");
		return;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_ifindex = data->if_index;

	if(ioctl(fd, SIOCGIFNAME, &ifr) < 0)
	{
		PERROR("call_ioctl_on_dev: ioctl failed:");
		close(fd);
		return;
	}
	close(fd);

	for(i = 0; i < NatIfaceCnt; i++)
	{
		if(strncmp(ifr.ifr_name,
							 pNatIfaces[i].iface_name,
							 sizeof(pNatIfaces[i].iface_name)) == 0)
		{
			/* copy the ipv4 address to filter out downlink connections
				 ignore downlink after listening connection event from
				 conntrack as it is not destinated to private ip address */
			IPACMDBG("Interface (%s) is nat\n", ifr.ifr_name);
			for(j = 0; j < MAX_NAT_IFACES; j++)
			{
				/* check if duplicate NAT ip */
				if(nat_iface_ipv4_addr[j] == data->ipv4_addr)
					break;

				if(nat_iface_ipv4_addr[j] == 0)
				{
					nat_iface_ipv4_addr[j] = data->ipv4_addr;
					nat_inst->FlushTempEntries(data->ipv4_addr, true);
					break;
				}
			}

			isNatIface = true;
			IPACMDBG("Nating connections of Interface (%s), entry (%d)\n", pNatIfaces[i].iface_name, j);
			IPACM_ConntrackClient::iptodot("with ipv4 address", nat_iface_ipv4_addr[j]);
			break;
		}
	}

	/* Cache the non nat iface ip address */
	if(isNatIface != true)
	{
		for(i = 0; i < MAX_NAT_IFACES; i++)
		{
			if(nonnat_iface_ipv4_addr[i] == 0)
			{
				nonnat_iface_ipv4_addr[i] = data->ipv4_addr;
				nat_inst->FlushTempEntries(data->ipv4_addr, false);
				break;
			}
		}
	}

}

void IPACM_ConntrackListener::HandleNeighIpAddrDelEvt(void *in_param)
{
	ipacm_event_data_all *data = (ipacm_event_data_all *)in_param;
	int cnt;

	if(data->ipv4_addr == 0 || data->iptype != IPA_IP_v4)
	{
		IPACMDBG("Ignoring IPA_NEIGH_CLIENT_IP_ADDR_DEL_EVENT EVENT\n");
		return;
	}
	IPACMDBG("Received interface index %d with ip type:%d", data->if_index, data->iptype);
	IPACMDBG("Entering NAT entry deletion checking\n");

	for(cnt = 0; cnt<MAX_NAT_IFACES; cnt++)
	{
		if(nat_iface_ipv4_addr[cnt] == data->ipv4_addr)
		{
			IPACMDBG("Reseting ct filters of Interface (%d), entry (%d)\n", data->if_index, cnt);
			IPACM_ConntrackClient::iptodot("with ipv4 address", nat_iface_ipv4_addr[cnt]);
			nat_iface_ipv4_addr[cnt] = 0;
		}

		if(nonnat_iface_ipv4_addr[cnt] == data->ipv4_addr)
		{
			IPACMDBG("Reseting ct filters of Interface (%d), entry (%d)\n", data->if_index, cnt);
			IPACM_ConntrackClient::iptodot("with ipv4 address", nonnat_iface_ipv4_addr[cnt]);
			nonnat_iface_ipv4_addr[cnt] = 0;
		}
	}

	nat_inst->FlushTempEntries(data->ipv4_addr, false);
	return;
}

void IPACM_ConntrackListener::TriggerWANUp(void *in_param)
{
	 ipacm_event_iface_up *wanup_data = (ipacm_event_iface_up *)in_param;

	 IPACMDBG("Recevied below informatoin during wanup:\n");
	 IPACMDBG("if_name:%s, ipv4_address:0x%x\n",
						wanup_data->ifname, wanup_data->ipv4_addr);

	 if(wanup_data->ipv4_addr == 0)
	 {
		 IPACMERR("Invalid ipv4 address,ignoring IPA_HANDLE_WAN_UP event\n");
		 return;
	 }

	 IPACM_ConntrackClient::iptodot("public ip address", wanup_data->ipv4_addr);
	 WanUp = true;
	 isStaMode = wanup_data->is_sta;
	 IPACMDBG("isStaMode: %d\n", isStaMode);

	 wan_ipaddr = wanup_data->ipv4_addr;
	 memcpy(wan_ifname, wanup_data->ifname, sizeof(wan_ifname));

	 if(nat_inst != NULL)
	 {
		 nat_inst->AddTable(wanup_data->ipv4_addr);
	 }

	 IPACMDBG("creating nat threads\n");
	 CreateNatThreads();
}

int IPACM_ConntrackListener::CreateNatThreads(void)
{
	 int ret;
	 pthread_t tcp_thread = 0, udp_thread = 0, udpcto_thread = 0, to_monitor_thread = 0;

	 if(isCTReg == false)
	 {

			if(!tcp_thread)
			{
				 ret = pthread_create(&tcp_thread, NULL, IPACM_ConntrackClient::TCPRegisterWithConnTrack, NULL);
				 if(0 != ret)
				 {
						IPACMERR("unable to create TCP conntrack event listner thread\n");
						PERROR("unable to create TCP conntrack\n");
						return -1;
				 }

				 IPACMDBG("created TCP conntrack event listner thread\n");
			}

			if(!udp_thread)
			{
				 ret = pthread_create(&udp_thread, NULL, IPACM_ConntrackClient::UDPRegisterWithConnTrack, NULL);
				 if(0 != ret)
				 {
						IPACMERR("unable to create UDP conntrack event listner thread\n");
						PERROR("unable to create UDP conntrack\n");
						goto error;
				 }

				 IPACMDBG("created UDP conntrack event listner thread\n");
			}

			if(!udpcto_thread)
			{
				 ret = pthread_create(&udpcto_thread, NULL, IPACM_ConntrackClient::UDPConnTimeoutUpdate, NULL);
				 if(0 != ret)
				 {
						IPACMERR("unable to create udp conn timeout thread\n");
						PERROR("unable to create udp conn timeout\n");
						goto error;
				 }

				 IPACMDBG("created upd conn timeout thread\n");
			}

			if(!to_monitor_thread)
			{
				ret = pthread_create(&to_monitor_thread, NULL, IPACM_ConntrackClient::TCPUDP_Timeout_monitor, NULL);
				 if(0 != ret)
				 {
						IPACMERR("unable to create tcp/udp timeout monitor thread\n");
						PERROR("unable to create tcp/udp timeout monitor\n");
						goto error;
				 }

				 IPACMDBG("created tcp/udp timeout monitor thread\n");
			}

			isCTReg = true;
	 }
	 return 0;

error:
	 if(tcp_thread)
	 {
			pthread_cancel(tcp_thread);
	 }

	 if(udp_thread)
	 {
			pthread_cancel(tcp_thread);
	 }

	 if(udpcto_thread)
	 {
			pthread_cancel(udpcto_thread);
	 }

	 if(to_monitor_thread)
	 {
		 pthread_cancel(to_monitor_thread);
	 }

	 return -1;
}

void IPACM_ConntrackListener::TriggerWANDown(uint32_t wan_addr)
{
	 IPACMDBG("Deleting ipv4 nat table with ");
	 IPACM_ConntrackClient::iptodot("public ip address", wan_addr);
	 WanUp = false;

	 if(nat_inst != NULL)
	 {
		 nat_inst->DeleteTable(wan_addr);
	 }
}


void ParseCTMessage(struct nf_conntrack *ct)
{
	 uint32_t status;
	 IPACMDBG("Printing conntrack parameters\n");

	 IPACM_ConntrackClient::iptodot("ATTR_IPV4_SRC = ATTR_ORIG_IPV4_SRC:", nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC));
	 IPACM_ConntrackClient::iptodot("ATTR_IPV4_DST = ATTR_ORIG_IPV4_DST:", nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST));
	 IPACMDBG("ATTR_PORT_SRC = ATTR_ORIG_PORT_SRC: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC));
	 IPACMDBG("ATTR_PORT_DST = ATTR_ORIG_PORT_DST: 0x%x\n", nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST));

	 IPACM_ConntrackClient::iptodot("ATTR_REPL_IPV4_SRC:", nfct_get_attr_u32(ct, ATTR_REPL_IPV4_SRC));
	 IPACM_ConntrackClient::iptodot("ATTR_REPL_IPV4_DST:", nfct_get_attr_u32(ct, ATTR_REPL_IPV4_DST));
	 IPACMDBG("ATTR_REPL_PORT_SRC: 0x%x\n", nfct_get_attr_u16(ct, ATTR_REPL_PORT_SRC));
	 IPACMDBG("ATTR_REPL_PORT_DST: 0x%x\n", nfct_get_attr_u16(ct, ATTR_REPL_PORT_DST));

	 IPACM_ConntrackClient::iptodot("ATTR_SNAT_IPV4:", nfct_get_attr_u32(ct, ATTR_SNAT_IPV4));
	 IPACM_ConntrackClient::iptodot("ATTR_DNAT_IPV4:", nfct_get_attr_u32(ct, ATTR_DNAT_IPV4));
	 IPACMDBG("ATTR_SNAT_PORT: 0x%x\n", nfct_get_attr_u16(ct, ATTR_SNAT_PORT));
	 IPACMDBG("ATTR_DNAT_PORT: 0x%x\n", nfct_get_attr_u16(ct, ATTR_DNAT_PORT));

	 IPACMDBG("ATTR_MARK: 0x%x\n", nfct_get_attr_u32(ct, ATTR_MARK));
	 IPACMDBG("ATTR_USE: 0x%x\n", nfct_get_attr_u32(ct, ATTR_USE));
	 IPACMDBG("ATTR_ID: 0x%x\n", nfct_get_attr_u32(ct, ATTR_ID));

	 status = nfct_get_attr_u32(ct, ATTR_STATUS);
	 IPACMDBG("ATTR_STATUS: 0x%x\n", status);

	 if(IPS_SRC_NAT & status)
	 {
			IPACMDBG("IPS_SRC_NAT set\n");
	 }

	 if(IPS_DST_NAT & status)
	 {
			IPACMDBG("IPS_DST_NAT set\n");
	 }

	 if(IPS_SRC_NAT_DONE & status)
	 {
			IPACMDBG("IPS_SRC_NAT_DONE set\n");
	 }

	 if(IPS_DST_NAT_DONE & status)
	 {
			IPACMDBG(" IPS_DST_NAT_DONE set\n");
	 }

	 IPACMDBG("\n");
	 return;
}

void IPACM_ConntrackListener::ProcessCTMessage(void *param)
{
	 ipacm_ct_evt_data *evt_data = (ipacm_ct_evt_data *)param;
	 u_int8_t l4proto = 0; 

#ifdef IPACM_DEBUG
	 char buf[1024];

	 /* Process message and generate ioctl call to kernel thread */
	 nfct_snprintf(buf, sizeof(buf), evt_data->ct,
								 evt_data->type, NFCT_O_PLAIN, NFCT_OF_TIME);
	 IPACMDBG("%s\n", buf);
	 IPACMDBG("\n");

	 ParseCTMessage(evt_data->ct);
#endif

	 l4proto = nfct_get_attr_u8(evt_data->ct, ATTR_ORIG_L4PROTO);
	 if(IPPROTO_UDP != l4proto && IPPROTO_TCP != l4proto)
	 {
			IPACMDBG("Received unexpected protocl %d conntrack message\n", l4proto);
	 }
	 else
	 {
			ProcessTCPorUDPMsg(evt_data->ct, evt_data->type, l4proto);
	 }

	 /* Cleanup item that was allocated during the original CT callback */
	 nfct_destroy(evt_data->ct);
	 return;
}


/* conntrack send in host order and ipa expects in host order */
void IPACM_ConntrackListener::ProcessTCPorUDPMsg(
	 struct nf_conntrack *ct,
	 enum nf_conntrack_msg_type type,
	 u_int8_t l4proto)
{
	 nat_table_entry rule;
	 u_int8_t tcp_state;
	 uint32_t status = 0;
	 IPACM_Config *pConfig;
	 uint32_t orig_src_ip, orig_dst_ip;
	 bool isTempEntry = false;

	 pConfig = IPACM_Config::GetInstance();
	 if(pConfig == NULL)
	 {
		 IPACMERR("Unable to get Config instance\n");
	 }

	 IPACMDBG("Received type:%d with proto:%d\n", type, l4proto);
	 status = nfct_get_attr_u32(ct, ATTR_STATUS);

	 if(IPS_DST_NAT & status)
	 {
		 status = IPS_DST_NAT;
	 }
	 else if(IPS_SRC_NAT & status)
	 {
		 status = IPS_SRC_NAT;
	 }
	 else
	 {
		 IPACMDBG("Neither Destination nor Source nat flag Set\n");
		 orig_src_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC); 
		 orig_src_ip = ntohl(orig_src_ip);
		 if(orig_src_ip == 0)
		 {
			 IPACMERR("unable to retrieve orig src ip address\n");
			 return;
		 }

		 orig_dst_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST);
		 orig_dst_ip = ntohl(orig_dst_ip);
		 if(orig_dst_ip == 0)
		 {
			 IPACMERR("unable to retrieve orig dst ip address\n");
			 return;
		 }

		 if(orig_src_ip == wan_ipaddr)
		 {
			 IPACMDBG("orig src ip:0x%x equal to wan ip\n",orig_src_ip);
			 status = IPS_SRC_NAT;
		 }
		 else if(orig_dst_ip == wan_ipaddr)
		 {
			 IPACMDBG("orig Dst IP:0x%x equal to wan ip\n",orig_dst_ip);
			 status = IPS_DST_NAT;
		 }
		 else
		 {
			 IPACMDBG("Neither orig src ip:0x%x Nor orig Dst IP:0x%x equal to wan ip:0x%x\n",
						orig_src_ip, orig_dst_ip, wan_ipaddr);

			 if(pConfig != NULL)
			 {
				 if(pConfig->isPrivateSubnet(orig_src_ip) &&
						(IPS_SRC_NAT_DONE & status))
				 {
					 IPACMDBG("orig src ip:0x%x match private subnet\n",
							orig_src_ip);
					 status = IPS_SRC_NAT;
				 }
				 else
				 {
					 return;
				 }
			 }

		 }
	 }

	 if(IPS_DST_NAT == status)
	 {
			IPACMDBG("Destination NAT\n");
			rule.dst_nat = true;

			IPACMDBG("Parse reply tuple\n");
			rule.target_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC);
			rule.target_ip = ntohl(rule.target_ip);

			/* Retriev target/dst port */
			rule.target_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC);
			rule.target_port = ntohs(rule.target_port);
			if(0 == rule.target_port)
			{
				 IPACMDBG("unable to retrieve target port\n");
			}

			rule.public_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST);
			rule.public_port = ntohs(rule.public_port);

			/* Retriev src/private ip address */
			rule.private_ip = nfct_get_attr_u32(ct, ATTR_REPL_IPV4_SRC);
			rule.private_ip = ntohl(rule.private_ip);
			if(0 == rule.private_ip)
			{
				 IPACMDBG("unable to retrieve private ip address\n");
			}

			/* Retriev src/private port */
			rule.private_port = nfct_get_attr_u16(ct, ATTR_REPL_PORT_SRC);
			rule.private_port = ntohs(rule.private_port);
			if(0 == rule.private_port)
			{
				 IPACMDBG("unable to retrieve private port\n");
			}
	 }
	 else if(IPS_SRC_NAT == status)
	 {
			IPACMDBG("Source NAT\n");
			rule.dst_nat = false;

			/* Retriev target/dst ip address */
			IPACMDBG("Parse source tuple\n");
			rule.target_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_DST);
			rule.target_ip = ntohl(rule.target_ip);
			if(0 == rule.target_ip)
			{
				 IPACMDBG("unable to retrieve target ip address\n");
			}
			/* Retriev target/dst port */
			rule.target_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_DST);
			rule.target_port = ntohs(rule.target_port);
			if(0 == rule.target_port)
			{
				 IPACMDBG("unable to retrieve target port\n");
			}

			/* Retriev public port */
			rule.public_port = nfct_get_attr_u16(ct, ATTR_REPL_PORT_DST);
			rule.public_port = ntohs(rule.public_port);
			if(0 == rule.public_port)
			{
				 IPACMDBG("unable to retrieve public port\n");
			}

			/* Retriev src/private ip address */
			rule.private_ip = nfct_get_attr_u32(ct, ATTR_ORIG_IPV4_SRC);
			rule.private_ip = ntohl(rule.private_ip);
			if(0 == rule.private_ip)
			{
				 IPACMDBG("unable to retrieve private ip address\n");
			}

			/* Retriev src/private port */
			rule.private_port = nfct_get_attr_u16(ct, ATTR_ORIG_PORT_SRC);
			rule.private_port = ntohs(rule.private_port);
			if(0 == rule.private_port)
			{
				 IPACMDBG("unable to retrieve private port\n");
			}
	 }
	 else
	 {
		 IPACMDBG("Neither source Nor destination nat\n");
		 goto IGNORE;
	 }

	 /* Retrieve Protocol */
	 rule.protocol = nfct_get_attr_u8(ct, ATTR_REPL_L4PROTO);

	 if(rule.private_ip != wan_ipaddr)
	 {
		 int cnt;
		 for(cnt = 0; cnt < MAX_NAT_IFACES; cnt++)
		 {
			 if(nat_iface_ipv4_addr[cnt] != 0)
			 {
				 if(rule.private_ip == nat_iface_ipv4_addr[cnt] ||
						rule.target_ip == nat_iface_ipv4_addr[cnt])
				 {
					 IPACMDBG("matched nat_iface_ipv4_addr entry(%d)\n", cnt);
					 IPACM_ConntrackClient::iptodot("ProcessTCPorUDPMsg(): Nat entry match with ip addr",
																					nat_iface_ipv4_addr[cnt]);
					 break;
				 }
			 }
		 }

		 if(cnt == MAX_NAT_IFACES)
		 {
			 IPACMDBG("Not mtaching with nat ifaces\n")
			 if(pConfig == NULL)
			 {
				 goto IGNORE;
			 }

			 if(pConfig->isPrivateSubnet(rule.private_ip) ||
						pConfig->isPrivateSubnet(rule.target_ip))
			 {
				 IPACMDBG("Matching with Private subnet\n");
				 isTempEntry = true;
			 }
			 else
			 {
				 goto IGNORE;
			 }
		 }

	 }
	 else
	 {
		 if(isStaMode) {
			 IPACMDBG("In STA mode, ignore connections destinated to STA interface\n");
			 goto IGNORE;
		 }

		 IPACMDBG("For embedded connections add dummy nat rule\n");
                 IPACMDBG("Change private port %d to %d\n",
				rule.private_port, rule.public_port);
		 rule.private_port = rule.public_port;
	 }
	 
	 IPACMDBG("Nat Entry with below information will either be added or deleted\n");
	 IPACM_ConntrackClient::iptodot("target ip or dst ip", rule.target_ip);
	 IPACMDBG("target port or dst port: 0x%x Decimal:%d\n", rule.target_port, rule.target_port);
	 IPACM_ConntrackClient::iptodot("private ip or src ip", rule.private_ip);
	 IPACMDBG("private port or src port: 0x%x, Decimal:%d\n", rule.private_port, rule.private_port);
	 IPACMDBG("public port or reply dst port: 0x%x, Decimal:%d\n", rule.public_port, rule.public_port);
	 IPACMDBG("Protocol: %d, destination nat flag: %d\n", rule.protocol, rule.dst_nat);

	 if(IPPROTO_TCP == rule.protocol)
	 {
			if(nat_inst == NULL)
			{
				return;
			}

			tcp_state = nfct_get_attr_u8(ct, ATTR_TCP_STATE);
			if(TCP_CONNTRACK_ESTABLISHED == tcp_state)
			{
				 IPACMDBG("TCP state TCP_CONNTRACK_ESTABLISHED(%d)\n", tcp_state);
				 if(isTempEntry)
				 {
					 nat_inst->AddTempEntry(&rule);
				 }
				 else
				 {
					 nat_inst->AddEntry(&rule);
				 }
			}
			else if(TCP_CONNTRACK_FIN_WAIT == tcp_state ||
			        type == NFCT_T_DESTROY)
			{
				 IPACMDBG("TCP state TCP_CONNTRACK_FIN_WAIT(%d) "
						"or type NFCT_T_DESTROY(%d)\n", tcp_state, type);

				 if(isTempEntry)
				 {
					 nat_inst->DeleteTempEntry(&rule);
				 }
				 else
				 {
					 nat_inst->DeleteEntry(&rule);
				 }
			}
			else
			{
				 IPACMDBG("Ignore tcp state: %d and type: %d\n", tcp_state, type);
			}

	 }
	 else if(IPPROTO_UDP == rule.protocol)
	 {
			if(nat_inst == NULL)
			{
				return;
			}

			if(NFCT_T_NEW == type)
			{
				 IPACMDBG("New UDP connection at time %ld\n", time(NULL));
				 if(isTempEntry)
				 {
					 nat_inst->AddTempEntry(&rule);
				 }
				 else
				 {
					 nat_inst->AddEntry(&rule);
				 }
			}
			else if(NFCT_T_DESTROY == type)
			{
				 IPACMDBG("UDP connection close at time %ld\n", time(NULL));
				 if(isTempEntry)
				 {
					 nat_inst->DeleteTempEntry(&rule);
				 }
				 else
				 {
					 nat_inst->DeleteEntry(&rule);
				 }
			}
	 }
	 else
	 {
			IPACMDBG("Ignore protocol: %d and type: %d\n", rule.protocol, type);
	 }

	 return;

IGNORE:
	IPACMDBG("ignoring below Nat Entry\n");
	IPACM_ConntrackClient::iptodot("target ip or dst ip", rule.target_ip);
	IPACMDBG("target port or dst port: 0x%x Decimal:%d\n", rule.target_port, rule.target_port);
	IPACM_ConntrackClient::iptodot("private ip or src ip", rule.private_ip);
	IPACMDBG("private port or src port: 0x%x, Decimal:%d\n", rule.private_port, rule.private_port);
	IPACMDBG("public port or reply dst port: 0x%x, Decimal:%d\n", rule.public_port, rule.public_port);
	IPACMDBG("Protocol: %d, destination nat flag: %d\n", rule.protocol, rule.dst_nat);

	return;
}






