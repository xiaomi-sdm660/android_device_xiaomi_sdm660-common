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
IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.Z
*/
/*!
  @file
  IPACM_Iface.cpp

  @brief
  This file implements the basis Iface functionality.

  @Author
  Skylar Chang

*/
#include <fcntl.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>

#include <IPACM_Netlink.h>
#include <IPACM_Iface.h>
#include <IPACM_Lan.h>
#include <IPACM_Wan.h>
#include <IPACM_Wlan.h>

const char *IPACM_Iface::DEVICE_NAME = "/dev/ipa";
IPACM_Routing IPACM_Iface::m_routing;
IPACM_Filtering IPACM_Iface::m_filtering;
IPACM_Header IPACM_Iface::m_header;

IPACM_Config *IPACM_Iface::ipacmcfg = IPACM_Config::GetInstance();

IPACM_Iface::IPACM_Iface(int iface_index)
{
	ip_type = IPACM_IP_NULL; /* initially set invalid */
	num_dft_rt_v6 = 0;
	softwarerouting_act = false;
	ipa_if_num = iface_index;

	iface_query = NULL;
	tx_prop = NULL;
	rx_prop = NULL;

	memcpy(dev_name,
				 IPACM_Iface::ipacmcfg->iface_table[iface_index].iface_name,
				 sizeof(IPACM_Iface::ipacmcfg->iface_table[iface_index].iface_name));

	memset(dft_v4fl_rule_hdl, 0, sizeof(dft_v4fl_rule_hdl));
	memset(dft_v6fl_rule_hdl, 0, sizeof(dft_v6fl_rule_hdl));

	memset(dft_rt_rule_hdl, 0, sizeof(dft_rt_rule_hdl));
	memset(software_routing_fl_rule_hdl, 0, sizeof(software_routing_fl_rule_hdl));
	memset(ipv6_addr, 0, sizeof(ipv6_addr));

	query_iface_property();
	IPACMDBG(" create iface-index(%d) constructor\n", ipa_if_num);
	return;
}

/* software routing enable */
int IPACM_Iface::handle_software_routing_enable(void)
{

	int res = IPACM_SUCCESS;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	IPACMDBG("\n");
	if (softwarerouting_act == true)
	{
		IPACMDBG("already setup software_routing rule for (%s)iface ip-family %d\n", 
						     IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ip_type);
		return IPACM_SUCCESS;
	}

	if(rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

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
	m_pFilteringTable->num_rules = (uint8_t)1;


	/* Configuring Software-Routing Filtering Rule */
	memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

	flt_rule_entry.at_rear = false;
	flt_rule_entry.flt_rule_hdl = -1;
	flt_rule_entry.status = -1;
	flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
	memcpy(&flt_rule_entry.rule.attrib,
				 &rx_prop->rx[0].attrib,
				 sizeof(flt_rule_entry.rule.attrib));

	memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

	/* check iface is v4 or v6 or both*/
	if (ip_type == IPA_IP_MAX)
	{
		/* handle v4 */
		m_pFilteringTable->ip = IPA_IP_v4;
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMERR("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		software_routing_fl_rule_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;


		/* handle v6*/
		m_pFilteringTable->ip = IPA_IP_v6;
		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMDBG("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		software_routing_fl_rule_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
		softwarerouting_act = true;
	}
	else
	{
		if (ip_type == IPA_IP_v4) 
		{
			m_pFilteringTable->ip = IPA_IP_v4;
		}
		else 
		{
			m_pFilteringTable->ip = IPA_IP_v6;
		}

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else if (m_pFilteringTable->rules[0].status)
		{
			IPACMERR("adding flt rule failed status=0x%x\n", m_pFilteringTable->rules[0].status);
			res = IPACM_FAILURE;
			goto fail;
		}

		IPACMDBG("soft-routing flt rule hdl0=0x%x\n", m_pFilteringTable->rules[0].flt_rule_hdl);
		/* copy filter hdls */
		if (ip_type == IPA_IP_v4) 
		{
			software_routing_fl_rule_hdl[0] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}
		else 
		{
			software_routing_fl_rule_hdl[1] = m_pFilteringTable->rules[0].flt_rule_hdl;
		}
		softwarerouting_act = true;
	}

fail:
	free(m_pFilteringTable);

	return res;
}

/* software routing disable */
int IPACM_Iface::handle_software_routing_disable(void)
{
	int res = IPACM_SUCCESS;
	ipa_ip_type ip;
	uint32_t flt_hdl;

	if (softwarerouting_act == false)
	{
		IPACMDBG("already delete AMPDU software_routing rule for (%s)iface ip-family %d\n", IPACM_Iface::ipacmcfg->iface_table[ipa_if_num].iface_name, ip_type);
		return IPACM_SUCCESS;
	}	

	if (ip_type == IPA_IP_MAX)
	{
		/* ipv4 case */
		if (m_filtering.DeleteFilteringHdls(&software_routing_fl_rule_hdl[0],
																				IPA_IP_v4, 1) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}

		/* ipv6 case */
		if (m_filtering.DeleteFilteringHdls(&software_routing_fl_rule_hdl[1],
																				IPA_IP_v6, 1) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		softwarerouting_act = false;
	}
	else
	{
		if (ip_type == IPA_IP_v4)
		{
			ip = IPA_IP_v4;
		}
		else
		{
			ip = IPA_IP_v6;
		}


		if (ip_type == IPA_IP_v4)
		{
			flt_hdl = software_routing_fl_rule_hdl[0];
		}
		else
		{
			flt_hdl = software_routing_fl_rule_hdl[1];
		}

		if (m_filtering.DeleteFilteringHdls(&flt_hdl, ip, 1) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		softwarerouting_act = false;
	}

fail:
	return res;
}

/* Query ipa_interface_index by given linux interface_index */
int IPACM_Iface::iface_ipa_index_query
(
	 int interface_index
)
{
	int fd;
	int link = INVALID_IFACE;
	int i = 0;
	struct ifreq ifr;


	/* Search known linux interface-index and map to IPA interface-index*/
	for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_ipa_interfaces; i++)
	{
		if (interface_index == IPACM_Iface::ipacmcfg->iface_table[i].netlink_interface_index)
		{
			link = i;
			IPACMDBG("Interface (%s) found: linux(%d) ipa(%d) \n",
							 IPACM_Iface::ipacmcfg->iface_table[i].iface_name,
							 IPACM_Iface::ipacmcfg->iface_table[i].netlink_interface_index,
							 link);
			return link;
			break;
		}
	}

	/* Search/Configure linux interface-index and map it to IPA interface-index */
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		PERROR("get interface name socket create failed");
		return IPACM_FAILURE;
	}

	memset(&ifr, 0, sizeof(struct ifreq));

	ifr.ifr_ifindex = interface_index;
	IPACMDBG("Interface index %d\n", interface_index);

	if (ioctl(fd, SIOCGIFNAME, &ifr) < 0)
	{
		PERROR("call_ioctl_on_dev: ioctl failed:");
		close(fd);
		return IPACM_FAILURE;
	}
	close(fd);

	IPACMDBG("Received interface name %s\n", ifr.ifr_name);
	for (i = 0; i < IPACM_Iface::ipacmcfg->ipa_num_ipa_interfaces; i++)
	{
		if (strncmp(ifr.ifr_name, 
								IPACM_Iface::ipacmcfg->iface_table[i].iface_name,
								sizeof(IPACM_Iface::ipacmcfg->iface_table[i].iface_name)) == 0)
		{
			IPACMDBG("Interface (%s) linux(%d) mapped to ipa(%d) \n", ifr.ifr_name, 
							 IPACM_Iface::ipacmcfg->iface_table[i].netlink_interface_index, i);

			link = i;
			IPACM_Iface::ipacmcfg->iface_table[i].netlink_interface_index = interface_index;
			break;
		}
	}

	return link;
}

/*Query the IPA endpoint property */
int IPACM_Iface::query_iface_property(void)
{
	int res = IPACM_SUCCESS, fd = 0;
	uint32_t cnt=0;

	fd = open(DEVICE_NAME, O_RDWR);
	IPACMDBG("iface query-property \n");
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", DEVICE_NAME);
		return IPACM_FAILURE;
	}

	iface_query = (struct ipa_ioc_query_intf *)
		 calloc(1, sizeof(struct ipa_ioc_query_intf));

	IPACMDBG("iface name %s\n", dev_name);
	memcpy(iface_query->name, dev_name, sizeof(dev_name));

	if (ioctl(fd, IPA_IOC_QUERY_INTF, iface_query) < 0)
	{
		PERROR("ioctl IPA_IOC_QUERY_INTF failed\n");
		/* iface_query memory will free when iface-down*/
		res = IPACM_FAILURE;
	}

	if(iface_query->num_tx_props > 0)
	{
		tx_prop = (struct ipa_ioc_query_intf_tx_props *)
			 calloc(1, sizeof(struct ipa_ioc_query_intf_tx_props) +
							iface_query->num_tx_props * sizeof(struct ipa_ioc_tx_intf_prop));

	memcpy(tx_prop->name, dev_name, sizeof(tx_prop->name));
	tx_prop->num_tx_props = iface_query->num_tx_props;

	if (ioctl(fd, IPA_IOC_QUERY_INTF_TX_PROPS, tx_prop) < 0)
	{
		PERROR("ioctl IPA_IOC_QUERY_INTF_TX_PROPS failed\n");
		/* tx_prop memory will free when iface-down*/
		res = IPACM_FAILURE;
	}

		if (res != IPACM_FAILURE)
		{
			for (cnt = 0; cnt < tx_prop->num_tx_props; cnt++)
			{
				IPACMDBG("Tx(%d):attrib-mask:0x%x, ip-type: %d, dst_pipe: %d, header: %s\n",
								 cnt, tx_prop->tx[cnt].attrib.attrib_mask, tx_prop->tx[cnt].ip, tx_prop->tx[cnt].dst_pipe, tx_prop->tx[cnt].hdr_name);
			}
		}

	}

	if (iface_query->num_rx_props > 0)
	{
		rx_prop = (struct ipa_ioc_query_intf_rx_props *)
			 calloc(1, sizeof(struct ipa_ioc_query_intf_rx_props) +
							iface_query->num_rx_props * sizeof(struct ipa_ioc_rx_intf_prop));

	memcpy(rx_prop->name, dev_name,
				 sizeof(rx_prop->name));
	rx_prop->num_rx_props = iface_query->num_rx_props;

	if (ioctl(fd, IPA_IOC_QUERY_INTF_RX_PROPS, rx_prop) < 0)
	{
		PERROR("ioctl IPA_IOC_QUERY_INTF_RX_PROPS failed\n");
		/* rx_prop memory will free when iface-down*/
		res = IPACM_FAILURE;
	}

		if (res != IPACM_FAILURE)
		{
			for (cnt = 0; cnt < rx_prop->num_rx_props; cnt++)
			{
				IPACMDBG("Rx(%d):attrib-mask:0x%x, ip-type: %d, src_pipe: %d\n",
								 cnt, rx_prop->rx[cnt].attrib.attrib_mask, rx_prop->rx[cnt].ip, rx_prop->rx[cnt].src_pipe);
			}
		}
	}

	close(fd);
	return res;
}

/*Configure the initial filter rules */
int IPACM_Iface::init_fl_rule(ipa_ip_type iptype)
{

	int res = IPACM_SUCCESS, len = 0;
	struct ipa_flt_rule_add flt_rule_entry;
	ipa_ioc_add_flt_rule *m_pFilteringTable;

	if (rx_prop == NULL)
	{
		IPACMDBG("No rx properties registered for iface %s\n", dev_name);
		return IPACM_SUCCESS;
	}

	/* update the iface ip-type to be IPA_IP_v4, IPA_IP_v6 or both*/
	if (iptype == IPA_IP_v4)
	{
	
		if ((ip_type == IPA_IP_v4) || (ip_type == IPA_IP_MAX))
		{
			IPACMDBG(" interface(%s:%d) already in ip-type %d\n", dev_name, ipa_if_num, ip_type);
			return res;
		}
	
		if (ip_type == IPA_IP_v6) 
		{
			ip_type = IPA_IP_MAX;
		}
		else 
		{
			ip_type = IPA_IP_v4; 
		}

		IPACMDBG(" interface(%s:%d) now ip-type is %d\n", dev_name, ipa_if_num, ip_type);
	}
	else
	{

		if ((ip_type == IPA_IP_v6) || (ip_type == IPA_IP_MAX))
		{
			IPACMDBG(" interface(%s:%d) already in ip-type %d\n", dev_name, ipa_if_num, ip_type);
			return res;
		}
	
		if (ip_type == IPA_IP_v4) 
		{
			ip_type = IPA_IP_MAX;
		}
		else 
		{
			ip_type = IPA_IP_v6;
		}

		IPACMDBG(" interface(%s:%d) now ip-type is %d\n", dev_name, ipa_if_num, ip_type);
	}


	/* construct ipa_ioc_add_flt_rule with default filter rules */
	if (iptype == IPA_IP_v4)
	{
		len = sizeof(struct ipa_ioc_add_flt_rule) +
			 (IPV4_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_add));

		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
		if (!m_pFilteringTable)
		{
			IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = iptype;
		m_pFilteringTable->num_rules = (uint8_t)IPV4_DEFAULT_FILTERTING_RULES;

		/* Configuring Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;
		IPACMDBG("rx property attrib mask:0x%x\n", rx_prop->rx[0].attrib.attrib_mask);
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));

		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_FRAGMENT;
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xF0000000;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xE0000000;
		memcpy(&(m_pFilteringTable->rules[1]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		/* Configuring Broadcast Filtering Rule */
		flt_rule_entry.rule.attrib.u.v4.dst_addr_mask = 0xFFFFFFFF;
		flt_rule_entry.rule.attrib.u.v4.dst_addr = 0xFFFFFFFF;
		memcpy(&(m_pFilteringTable->rules[2]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (false == m_filtering.AddFilteringRule(m_pFilteringTable))
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			/* copy filter hdls */
			for (int i = 0; i < IPV4_DEFAULT_FILTERTING_RULES; i++)
			{
				if (m_pFilteringTable->rules[i].status == 0)
				{
					dft_v4fl_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Default v4 filter Rule %d HDL:0x%x\n", i, dft_v4fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failed adding default v4 Filtering rule %d\n", i);
				}
			}
		}
	}
	else
	{
		len = sizeof(struct ipa_ioc_add_flt_rule) +
			 (IPV6_DEFAULT_FILTERTING_RULES * sizeof(struct ipa_flt_rule_add));

		m_pFilteringTable = (struct ipa_ioc_add_flt_rule *)calloc(1, len);
		if (!m_pFilteringTable)
		{
			IPACMERR("Error Locate ipa_flt_rule_add memory...\n");
			return IPACM_FAILURE;
		}

		m_pFilteringTable->commit = 1;
		m_pFilteringTable->ep = rx_prop->rx[0].src_pipe;
		m_pFilteringTable->global = false;
		m_pFilteringTable->ip = iptype;
		m_pFilteringTable->num_rules = (uint8_t)IPV6_DEFAULT_FILTERTING_RULES;

		/* Configuring Fragment Filtering Rule */
		memset(&flt_rule_entry, 0, sizeof(struct ipa_flt_rule_add));

		flt_rule_entry.at_rear = true;
		flt_rule_entry.flt_rule_hdl = -1;
		flt_rule_entry.status = -1;
		flt_rule_entry.rule.action = IPA_PASS_TO_EXCEPTION;

		/* Configuring Multicast Filtering Rule */
		memcpy(&flt_rule_entry.rule.attrib,
					 &rx_prop->rx[0].attrib,
					 sizeof(flt_rule_entry.rule.attrib));
		flt_rule_entry.rule.attrib.attrib_mask |= IPA_FLT_DST_ADDR;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[0] = 0xFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr_mask[3] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[0] = 0XFF000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[1] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[2] = 0x00000000;
		flt_rule_entry.rule.attrib.u.v6.dst_addr[3] = 0X00000000;
		memcpy(&(m_pFilteringTable->rules[0]), &flt_rule_entry, sizeof(struct ipa_flt_rule_add));

		if (m_filtering.AddFilteringRule(m_pFilteringTable) == false)
		{
			IPACMERR("Error Adding Filtering rule, aborting...\n");
			res = IPACM_FAILURE;
			goto fail;
		}
		else
		{
			/* copy filter hdls */
			for (int i = 0;
					 i < IPV6_DEFAULT_FILTERTING_RULES;
					 i++)
			{
				if (m_pFilteringTable->rules[i].status == 0)
				{
					dft_v6fl_rule_hdl[i] = m_pFilteringTable->rules[i].flt_rule_hdl;
					IPACMDBG("Default v6 Filter Rule %d HDL:0x%x\n", i, dft_v6fl_rule_hdl[i]);
				}
				else
				{
					IPACMERR("Failing adding v6 default IPV6 rule %d\n", i);
				}
			}
		}
	}


fail:
	free(m_pFilteringTable);

	return res;
}
