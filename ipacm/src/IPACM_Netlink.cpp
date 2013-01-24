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
	IPACM_Netlink.cpp

	@brief
	This file implements the IPAM Netlink Socket Parer functionality.

	@Author
	Skylar Chang

*/
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include "IPACM_CmdQueue.h"
#include "IPACM_Defs.h"
#include "IPACM_Netlink.h"
#include "IPACM_EvtDispatcher.h"
#include "IPACM_Log.h"

char dev_pre_name[IF_NAME_LEN];
struct sockaddr_storage  dst_pre_addr;

int ipa_get_if_name(char *if_name, int if_index);
int find_mask(int ip_v4_last, int *mask_value);

#define NDA_RTA(r)  ((struct rtattr*)(((char*)(r)) + NLMSG_ALIGN(sizeof(struct ndmsg))))

/* Opens a netlink socket*/
static int ipa_nl_open_socket
(
	 ipa_nl_sk_info_t *sk_info,
	 int protocol,
	 unsigned int grps
	 )
{
	int *p_sk_fd;
	struct sockaddr_nl *p_sk_addr_loc;

	p_sk_fd = &(sk_info->sk_fd);
	p_sk_addr_loc = &(sk_info->sk_addr_loc);

	/* Open netlink socket for specified protocol */
	if((*p_sk_fd = socket(AF_NETLINK, SOCK_RAW, protocol)) < 0)
	{
		IPACMERR("cannot open netlink socket\n");
		return IPACM_FAILURE;
	}

	/* Initialize socket addresses to null */
	memset(p_sk_addr_loc, 0, sizeof(struct sockaddr_nl));

	/* Populate local socket address using specified groups */
	p_sk_addr_loc->nl_family = AF_NETLINK;
	p_sk_addr_loc->nl_pid = getpid();
	p_sk_addr_loc->nl_groups = grps;

	/* Bind socket to the local address, i.e. specified groups. This ensures
	 that multicast messages for these groups are delivered over this 
	 socket. */

	if(bind(*p_sk_fd,
					(struct sockaddr *)p_sk_addr_loc,
					sizeof(struct sockaddr_nl)) < 0)
	{
		IPACMDBG("Socket bind failed\n");
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/* Add fd to fdmap array and store read handler function ptr (up to MAX_NUM_OF_FD).*/
static int ipa_nl_addfd_map
(
	 ipa_nl_sk_fd_set_info_t *fd_set,
	 int fd,
	 ipa_sock_thrd_fd_read_f read_f
	 )
{
	if(fd_set->num_fd < MAX_NUM_OF_FD)
	{
		FD_SET(fd, &(fd_set->fdset));

		/* Add fd to fdmap array and store read handler function ptr */
		fd_set->sk_fds[fd_set->num_fd].sk_fd = fd;
		fd_set->sk_fds[fd_set->num_fd].read_func = read_f;

		/* Increment number of fds stored in fdmap */
		fd_set->num_fd++;
		if(fd_set->max_fd < fd)
		{
			fd_set->max_fd = fd;
		}
	}
	else
	{
		return IPACM_FAILURE;
	}

	return IPACM_SUCCESS;
}

/*  start socket listener */
static int ipa_nl_sock_listener_start
(
	 ipa_nl_sk_fd_set_info_t *sk_fd_set
	 )
{
	int i, ret;

	while(true)
	{
		if((ret = select(sk_fd_set->max_fd + 1, &(sk_fd_set->fdset), NULL, NULL, NULL)) < 0)
		{
			IPACMERR("ipa_nl select failed\n");
		}
		else
		{
			for(i = 0; i < sk_fd_set->num_fd; i++)
			{

				if(FD_ISSET(sk_fd_set->sk_fds[i].sk_fd, &(sk_fd_set->fdset)))
				{

					if(sk_fd_set->sk_fds[i].read_func)
					{
						if(IPACM_SUCCESS != ((sk_fd_set->sk_fds[i].read_func)(sk_fd_set->sk_fds[i].sk_fd)))
						{
							IPACMERR("Error on read callback[%d] fd=%d\n",
											 i,
											 sk_fd_set->sk_fds[i].sk_fd);
						}
					}
					else
					{
						IPACMDBG("No read function\n");
					}
				}

			} /* end of for loop*/
		} /* end of else */
	} /* end of while */

	return IPACM_SUCCESS;
}

/* allocate memory for ipa_nl__msg */
static struct msghdr* ipa_nl_alloc_msg
(
	 uint32_t msglen
	 )
{
	unsigned char *buf = NULL;
	struct sockaddr_nl *nladdr = NULL;
	struct iovec *iov = NULL;
	struct msghdr *msgh = NULL;

	if(IPA_NL_MSG_MAX_LEN < msglen)
	{
		IPACMERR("Netlink message exceeds maximum length\n");
		return NULL;
	}

	msgh = (struct msghdr *)malloc(sizeof(struct msghdr));
	if(msgh == NULL)
	{
		IPACMERR("Failed malloc for msghdr\n");
		free(msgh);
		return NULL;
	}

	nladdr = (struct sockaddr_nl *)malloc(sizeof(struct sockaddr_nl));
	if(nladdr == NULL)
	{
		IPACMERR("Failed malloc for sockaddr\n");
		free(nladdr);
		free(msgh);
		return NULL;
	}

	iov = (struct iovec *)malloc(sizeof(struct iovec));
	if(iov == NULL)
	{
		PERROR("Failed malloc for iovec");
		free(iov);
		free(nladdr);
		free(msgh);
		return NULL;
	}

	buf = (unsigned char *)malloc(msglen);
	if(buf == NULL)
	{
		IPACMERR("Failed malloc for mglen\n");
		free(buf);
		free(iov);
		free(nladdr);
		free(msgh);
		return NULL;
	}

	memset(nladdr, 0, sizeof(struct sockaddr_nl));
	nladdr->nl_family = AF_NETLINK;

	memset(msgh, 0x0, sizeof(struct msghdr));
	msgh->msg_name = nladdr;
	msgh->msg_namelen = sizeof(struct sockaddr_nl);
	msgh->msg_iov = iov;
	msgh->msg_iovlen = 1;

	memset(iov, 0x0, sizeof(struct iovec));
	iov->iov_base = buf;
	iov->iov_len = msglen;

	return msgh;
}

/* release IPA message */
static void ipa_nl_release_msg
(
	 struct msghdr *msgh
	 )
{
	unsigned char *buf = NULL;
	struct sockaddr_nl *nladdr = NULL;
	struct iovec *iov = NULL;

	if(NULL == msgh)
	{
		return;
	}

	nladdr = (struct sockaddr_nl *)msgh->msg_name;
	iov = msgh->msg_iov;
	if(msgh->msg_iov)
	{
		buf = (unsigned char *)msgh->msg_iov->iov_base;
	}

	free(buf);
	free(iov);
	free(nladdr);
	free(msgh);

	return;
}

/* receive and process nl message */
static int ipa_nl_recv
(
	 int              fd,
	 struct msghdr **msg_pptr,
	 unsigned int  *msglen_ptr
	 )
{
	struct msghdr *msgh = NULL;
	int rmsgl;

	msgh = ipa_nl_alloc_msg(IPA_NL_MSG_MAX_LEN);
	if(NULL == msgh)
	{
		IPACMERR("Failed to allocate NL message\n");
		goto error;
	}


	/* Receive message over the socket */
	rmsgl = recvmsg(fd, msgh, 0);

	/* Verify that something was read */
	if(rmsgl <= 0)
	{
		PERROR("NL recv error");
		goto error;
	}

	/* Verify that NL address length in the received message is expected value */
	if(sizeof(struct sockaddr_nl) != msgh->msg_namelen)
	{
		IPACMERR("rcvd msg with namelen != sizeof sockaddr_nl\n");
		goto error;
	}

	/* Verify that message was not truncated. This should not occur */
	if(msgh->msg_flags & MSG_TRUNC)
	{
		IPACMERR("Rcvd msg truncated!\n");
		goto error;
	}

	*msg_pptr    = msgh;
	*msglen_ptr = rmsgl;

	return IPACM_SUCCESS;

/* An error occurred while receiving the message. Free all memory before 
				 returning. */
error:
	ipa_nl_release_msg(msgh);
	*msg_pptr    = NULL;
	*msglen_ptr  = 0;

	return IPACM_FAILURE;
}

/* decode the rtm netlink message */
static int ipa_nl_decode_rtm_link
(
	 const char              *buffer,
	 unsigned int             buflen,
	 ipa_nl_link_info_t      *link_info
)
{
	struct rtattr *rtah = NULL;
	/* NL message header */
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;

	/* Extract the header data */
	link_info->metainfo = *(struct ifinfomsg *)NLMSG_DATA(nlh);
	buflen -= sizeof(struct nlmsghdr);
	rtah = IFA_RTA(NLMSG_DATA(nlh));

	return IPACM_SUCCESS;
}

/* Decode kernel address message parameters from Netlink attribute TLVs. */
static int ipa_nl_decode_rtm_addr
(
	 const char              *buffer,
	 unsigned int             buflen,
	 ipa_nl_addr_info_t   *addr_info
	 )
{
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
	struct rtattr *rtah = NULL;
	
	/* Extract the header data */
	addr_info->metainfo = *((struct ifaddrmsg *)NLMSG_DATA(nlh));
	buflen -= sizeof(struct nlmsghdr);

	/* Extract the available attributes */
	addr_info->attr_info.param_mask = IPA_NLA_PARAM_NONE;

	rtah = IFA_RTA(NLMSG_DATA(nlh));

	while(RTA_OK(rtah, buflen))
	{
		switch(rtah->rta_type)
		{

		case IFA_ADDRESS:
			addr_info->attr_info.prefix_addr.ss_family = addr_info->metainfo.ifa_family;
			memcpy(&addr_info->attr_info.prefix_addr.__ss_padding,
						 RTA_DATA(rtah),
						 sizeof(addr_info->attr_info.prefix_addr.__ss_padding));
			addr_info->attr_info.param_mask |= IPA_NLA_PARAM_PREFIXADDR;
			break;

		default:
			break;

		}
		/* Advance to next attribute */
		rtah = RTA_NEXT(rtah, buflen);
	}

	return IPACM_SUCCESS;
}

/* Decode kernel neighbor message parameters from Netlink attribute TLVs. */
static int ipa_nl_decode_rtm_neigh
(
	 const char              *buffer,
	 unsigned int             buflen,
	 ipa_nl_neigh_info_t   *neigh_info
	 )
{
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
	struct rtattr *rtah = NULL;
	
	/* Extract the header data */
	neigh_info->metainfo = *((struct ndmsg *)NLMSG_DATA(nlh));
	buflen -= sizeof(struct nlmsghdr);

	/* Extract the available attributes */
	neigh_info->attr_info.param_mask = IPA_NLA_PARAM_NONE;

	rtah = NDA_RTA(NLMSG_DATA(nlh));

	while(RTA_OK(rtah, buflen))
	{
		switch(rtah->rta_type)
		{

		case NDA_DST:
			neigh_info->attr_info.local_addr.ss_family = neigh_info->metainfo.ndm_family;
			memcpy(&neigh_info->attr_info.local_addr.__ss_padding,
						 RTA_DATA(rtah),
						 sizeof(neigh_info->attr_info.local_addr.__ss_padding));
			break;

		case NDA_LLADDR:
			memcpy(neigh_info->attr_info.lladdr_hwaddr.sa_data,
						 RTA_DATA(rtah),
						 sizeof(neigh_info->attr_info.lladdr_hwaddr.sa_data));
			break;

		default:
			break;

		}

		/* Advance to next attribute */
		rtah = RTA_NEXT(rtah, buflen);
	}

	return IPACM_SUCCESS;
}

/* Decode kernel route message parameters from Netlink attribute TLVs. */
static int ipa_nl_decode_rtm_route
(
	 const char              *buffer,
	 unsigned int             buflen,
	 ipa_nl_route_info_t   *route_info
	 )
{
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;  /* NL message header */
	struct rtattr *rtah = NULL;
	
	/* Extract the header data */
	route_info->metainfo = *((struct rtmsg *)NLMSG_DATA(nlh));
	buflen -= sizeof(struct nlmsghdr);

	route_info->attr_info.param_mask = IPA_RTA_PARAM_NONE;
	rtah = RTM_RTA(NLMSG_DATA(nlh));

	while(RTA_OK(rtah, buflen))
	{
		switch(rtah->rta_type)
		{

		case RTA_DST:
			if((route_info->metainfo.rtm_type == RTN_UNICAST) &&
				 (route_info->metainfo.rtm_protocol == RTPROT_BOOT) &&
				 (route_info->metainfo.rtm_scope == RT_SCOPE_LINK) &&
				 (route_info->metainfo.rtm_table == RT_TABLE_MAIN))
			{
				route_info->metainfo.rtm_type = RTN_BROADCAST;
				memcpy(&route_info->attr_info.dst_addr.__ss_padding,
							 dst_pre_addr.__ss_padding,
							 sizeof(route_info->attr_info.dst_addr.__ss_padding));
				route_info->attr_info.param_mask |= IPA_RTA_PARAM_DST;
			}
			else
			{
				route_info->attr_info.dst_addr.ss_family = route_info->metainfo.rtm_family;
				memcpy(&route_info->attr_info.dst_addr.__ss_padding,
							 RTA_DATA(rtah),
							 sizeof(route_info->attr_info.dst_addr.__ss_padding));
				route_info->attr_info.param_mask |= IPA_RTA_PARAM_DST;
			}
			break;

		case RTA_SRC:
			route_info->attr_info.src_addr.ss_family = route_info->metainfo.rtm_family;
			memcpy(&route_info->attr_info.src_addr.__ss_padding,
						 RTA_DATA(rtah),
						 sizeof(route_info->attr_info.src_addr.__ss_padding));
			route_info->attr_info.param_mask |= IPA_RTA_PARAM_SRC;
			break;

		case RTA_GATEWAY:
			route_info->attr_info.gateway_addr.ss_family = route_info->metainfo.rtm_family;
			memcpy(&route_info->attr_info.gateway_addr.__ss_padding,
						 RTA_DATA(rtah),
						 sizeof(route_info->attr_info.gateway_addr.__ss_padding));
			route_info->attr_info.param_mask |= IPA_RTA_PARAM_GATEWAY;
			break;

		case RTA_IIF:
			memcpy(&route_info->attr_info.iif_index,
						 RTA_DATA(rtah),
						 sizeof(route_info->attr_info.iif_index));
			route_info->attr_info.param_mask |= IPA_RTA_PARAM_IIF;
			break;

		case RTA_OIF:
			memcpy(&route_info->attr_info.oif_index,
						 RTA_DATA(rtah),
						 sizeof(route_info->attr_info.oif_index));
			route_info->attr_info.param_mask |= IPA_RTA_PARAM_OIF;
			break;

		case RTA_PRIORITY:
			memcpy(&route_info->attr_info.priority,
						 RTA_DATA(rtah),
						 sizeof(route_info->attr_info.priority));
			route_info->attr_info.param_mask |= IPA_RTA_PARAM_PRIORITY;
			break;

		default:
			break;

		}

		/* Advance to next attribute */
		rtah = RTA_NEXT(rtah, buflen);
	}

	return IPACM_SUCCESS;
}

/* decode the ipa nl-message */
static int ipa_nl_decode_nlmsg
(
	 const char   *buffer,
	 unsigned int  buflen,
	 ipa_nl_msg_t  *msg_ptr
	 )
{
	char dev_name[IF_NAME_LEN];
	int ret_val, mask_value, mask_index, mask_value_v6;
	struct nlmsghdr *nlh = (struct nlmsghdr *)buffer;

	uint32_t if_ipv4_addr =0, if_ipipv4_addr_mask =0, temp =0;

	ipacm_cmd_q_data evt_data;
	ipacm_event_data_all *data_all;
	ipacm_event_data_fid *data_fid;
	ipacm_event_data_addr *data_addr;


	while(NLMSG_OK(nlh, buflen))
	{
		IPACMDBG("Received msg:%d from netlink\n", nlh->nlmsg_type)
		switch(nlh->nlmsg_type)
		{
		case RTM_NEWLINK:
			msg_ptr->type = nlh->nlmsg_type;
			msg_ptr->link_event = true;
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_link(buffer, buflen, &(msg_ptr->nl_link_info)))
			{
				IPACMERR("Failed to decode rtm link message\n");
				return IPACM_FAILURE;
			}
			else
			{
				IPACMDBG("Got RTM_NEWLINK with below values\n");
				IPACMDBG("RTM_NEWLINK, ifi_change:%d\n", msg_ptr->nl_link_info.metainfo.ifi_change);
				IPACMDBG("RTM_NEWLINK, ifi_flags:%d\n", msg_ptr->nl_link_info.metainfo.ifi_flags);
				IPACMDBG("RTM_NEWLINK, ifi_index:%d\n", msg_ptr->nl_link_info.metainfo.ifi_index);
				IPACMDBG("RTM_NEWLINK, family:%d\n", msg_ptr->nl_link_info.metainfo.ifi_family);

				if(IFF_UP & msg_ptr->nl_link_info.metainfo.ifi_change)
				{
					IPACMDBG("\n GOT useful newlink event\n");
					ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_link_info.metainfo.ifi_index);
					if(msg_ptr->nl_link_info.metainfo.ifi_flags & IFF_UP)
					{
						IPACMDBG("Interface %s bring up with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
						/* post link up to command queue */
						evt_data.event = IPA_LINK_UP_EVENT;
						IPACMDBG("Posting IPA_LINK_UP_EVENT with if index: %d\n",
										 data_fid->if_index);
					}
					else
					{
						IPACMDBG("Interface %s bring down with IP-family: %d \n", dev_name, msg_ptr->nl_link_info.metainfo.ifi_family);
						/* post link down to command queue */
						evt_data.event = IPA_LINK_DOWN_EVENT;
						IPACMDBG("Posting IPA_LINK_DOWN_EVENT with if index: %d\n",
										 data_fid->if_index);
					}

					data_fid = (ipacm_event_data_fid *)malloc(sizeof(ipacm_event_data_fid));
					if(data_fid == NULL)
					{
						IPACMDBG("unable to allocate memory for event data_fid\n");
						return IPACM_FAILURE;
					}
					data_fid->if_index = msg_ptr->nl_link_info.metainfo.ifi_index;


					evt_data.evt_data = data_fid;
					IPACM_EvtDispatcher::PostEvt(&evt_data);

				}
			}
			break;

		case RTM_DELLINK:
			IPACMDBG("\n GOT dellink event\n");
			msg_ptr->type = nlh->nlmsg_type;
			msg_ptr->link_event = true;
			IPACMDBG("entering rtm decode\n");
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_link(buffer, buflen, &(msg_ptr->nl_link_info)))
			{
				IPACMERR("Failed to decode rtm link message\n");
				return IPACM_FAILURE;
			}
			else
			{
				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_link_info.metainfo.ifi_index);
				if(ret_val != IPACM_SUCCESS)
				{
					IPACMERR("Error while getting interface name\n");
				}
				IPACMDBG("Interface %s bring down \n", dev_name);

				/* post link down to command queue */
				evt_data.event = IPA_LINK_DOWN_EVENT;
				data_fid = (ipacm_event_data_fid *)malloc(sizeof(ipacm_event_data_fid));
				if(data_fid == NULL)
				{
					IPACMDBG("unable to allocate memory for event data_fid\n");
					return IPACM_FAILURE;
				}

				data_fid->if_index = msg_ptr->nl_link_info.metainfo.ifi_index;

				IPACMDBG("posting IPA_LINK_DOWN_EVENT with if idnex:%d\n",
								 data_fid->if_index);
				evt_data.evt_data = data_fid;
				IPACM_EvtDispatcher::PostEvt(&evt_data);
				/* finish command queue */
			}
			break;

		case RTM_NEWADDR:
			IPACMDBG("\n GOT RTM_NEWADDR event\n");
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_addr(buffer, buflen, &(msg_ptr->nl_addr_info)))
			{
				IPACMERR("Failed to decode rtm addr message\n");
				return IPACM_FAILURE;
			}
			else
			{
				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_addr_info.metainfo.ifa_index);
				if(ret_val != IPACM_SUCCESS)
				{
					IPACMERR("Error while getting interface name\n");
				}
				IPACMDBG("Interface %s \n", dev_name);

				data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
				if(data_addr == NULL)
				{
					IPACMDBG("unable to allocate memory for event data_addr\n");
					return IPACM_FAILURE;
				}

				if(AF_INET6 == msg_ptr->nl_addr_info.attr_info.prefix_addr.ss_family)
				{
					data_addr->iptype = IPA_IP_v6;
					IPACMDBG("IFA_ADDRESS:IPV6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding)[1] >> 48)));

					memcpy(data_addr->ipv6_addr,
								 msg_ptr->nl_addr_info.attr_info.prefix_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr));
				}
				else
				{
					data_addr->iptype = IPA_IP_v4;
					IPACMDBG("IFA_ADDRESS:IPV4 %d.%d.%d.%d\n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_addr_info.attr_info.prefix_addr).__ss_padding >> 24));
					
					memcpy(&data_addr->ipv4_addr,
								 msg_ptr->nl_addr_info.attr_info.prefix_addr.__ss_padding,
								 sizeof(data_addr->ipv4_addr));
					data_addr->ipv4_addr = ntohl(data_addr->ipv4_addr);
         
				}

				evt_data.event = IPA_ADDR_ADD_EVENT;
				data_addr->if_index = msg_ptr->nl_addr_info.metainfo.ifa_index;

				IPACMDBG("Posting IPA_ADDR_ADD_EVENT with if index:%d, ipv4 addr:0x%x\n",
								 data_addr->if_index,
								 data_addr->ipv4_addr);
				evt_data.evt_data = data_addr;
				IPACM_EvtDispatcher::PostEvt(&evt_data);
			}
			break;

		case RTM_NEWROUTE:

			if(IPACM_SUCCESS != ipa_nl_decode_rtm_route(buffer, buflen, &(msg_ptr->nl_route_info)))
			{
				IPACMERR("Failed to decode rtm route message\n");
				return IPACM_FAILURE;
			}

			IPACMDBG("In case RTM_NEWROUTE\n");
			IPACMDBG("rtm_type: %d\n", msg_ptr->nl_route_info.metainfo.rtm_type);
			IPACMDBG("ss_padding: %d\n", ((unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24)));
			IPACMDBG("rtm_type: %d\n", msg_ptr->nl_route_info.metainfo.rtm_type);
			IPACMDBG("protocol: %d\n", msg_ptr->nl_route_info.metainfo.rtm_protocol);
			IPACMDBG("rtm_scope: %d\n", msg_ptr->nl_route_info.metainfo.rtm_scope);
      IPACMDBG("rtm_table: %d\n", msg_ptr->nl_route_info.metainfo.rtm_table);
			IPACMDBG("rtm_family: %d\n", msg_ptr->nl_route_info.metainfo.rtm_family);
			IPACMDBG("param_mask: 0x%x\n", msg_ptr->nl_route_info.attr_info.param_mask);

			/* ipv4 interface route and its subnet mask	*/
			if((msg_ptr->nl_route_info.metainfo.rtm_type == RTN_BROADCAST) && 
				 ((unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24)) != 0)
			{
				if(IPACM_SUCCESS != 
					 find_mask((unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24), &mask_value))
				{
					IPACMERR("Failed to decode rtm_addroute message\n");
				}
				else
				{
					IPACMDBG("\n GOT useful RTM_NEWROUTE event\n");

					/* take care of subnet mask */
					if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
					{
						IPACMDBG("DST_ADDRESS:IPV4 %d.%d.%d.0\n",
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16));
						IPACMDBG("MASK:IPV4 255.255.255.%d\n", mask_value);
						ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
						if(ret_val != IPACM_SUCCESS)
						{
							IPACMERR("Error while getting interface name");
						}
						IPACMDBG("RTA_OIF, output Interface %s \n", dev_name);
						memcpy(&dst_pre_addr.__ss_padding,
									 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(dst_pre_addr.__ss_padding));
						memcpy(&dev_pre_name,
									 dev_name,
									 sizeof(dev_pre_name));
						IPACMDBG("save pre_DST_ADDRESS:IPV4 %d.%d.%d.%d %s\n",
										 (unsigned char)(*(unsigned int *)&(dst_pre_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(dst_pre_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(dst_pre_addr).__ss_padding >> 16),
										 (unsigned char)(*(unsigned int *)&(dst_pre_addr).__ss_padding >> 24), dev_pre_name);
					}

					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_PRIORITY)
					{
						IPACMDBG("route add -net %d.%d.%d.0 netmask 255.255.255.%d dev %s  metric %d \n",
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
										 mask_value,
										 dev_pre_name,
										 msg_ptr->nl_route_info.attr_info.priority);
					}
					else
					{
						IPACMDBG("route add -net %d.%d.%d.0 netmask 255.255.255.%d dev %s \n",
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
										 mask_value,
										 dev_name);
					}

					memcpy(&if_ipv4_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(if_ipv4_addr));
					if_ipv4_addr = (if_ipv4_addr << 8) >> 8;
					temp = (-1) << 8;
					temp = temp | mask_value;

					/* insert to command queue */
					evt_data.event = IPA_ROUTE_ADD_EVENT;

					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v4;
					data_addr->ipv4_addr = ntohl(if_ipv4_addr);
					data_addr->ipv4_addr_mask = ntohl(if_ipipv4_addr_mask);

					IPACMDBG("Posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv4 address 0x%x\n",
									 data_addr->if_index,
									 data_addr->ipv4_addr);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}
			}

			
			/* take care of route add default route & uniroute */
			if((msg_ptr->nl_route_info.metainfo.rtm_type == RTN_UNICAST) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_protocol == RTPROT_BOOT) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_scope == RT_SCOPE_UNIVERSE) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_table == RT_TABLE_MAIN))
			{
				IPACMDBG("\n GOT RTM_NEWROUTE event\n");

				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
				{
					ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
					if(ret_val != IPACM_SUCCESS)
					{
						IPACMERR("Error while getting interface name\n");
					}

					IPACMDBG("route add -host %d.%d.%d.%d gw %d.%d.%d.%d dev %s\n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 24),
									 dev_name
									 );

					/* insert to command queue */
					memcpy(&if_ipv4_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(if_ipv4_addr));
					temp = (-1);

					evt_data.event = IPA_ROUTE_ADD_EVENT;
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMDBG("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v4;
					data_addr->ipv4_addr = ntohl(if_ipv4_addr);
					data_addr->ipv4_addr_mask = ntohl(if_ipipv4_addr_mask);

					IPACMDBG("Posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv4 address 0x%x, mask:0x%x\n",
									 data_addr->if_index,
									 data_addr->ipv4_addr,
									 data_addr->ipv4_addr_mask);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */

				}
				else
				{
					ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
					if(ret_val != IPACM_SUCCESS)
					{
						IPACMERR("Error while getting interface name\n");
					}

					if(AF_INET6 == msg_ptr->nl_route_info.metainfo.rtm_family)
					{
						/* insert to command queue */
						data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
						if(data_addr == NULL)
						{
							IPACMERR("unable to allocate memory for event data_addr\n");
							return IPACM_FAILURE;
						}

						if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_PRIORITY)
						{
							IPACMDBG("ip -6 route add default dev %s metric %d\n",
											 dev_name,
											 msg_ptr->nl_route_info.attr_info.priority);
						}
						else
						{
							IPACMDBG("ip -6 route add default dev %s\n", dev_name);
						}

						memcpy(data_addr->ipv6_addr,
									 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv6_addr));

						memcpy(data_addr->ipv6_addr_mask,
									 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv6_addr_mask));

						evt_data.event = IPA_ROUTE_ADD_EVENT;
						data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
						data_addr->iptype = IPA_IP_v6;

						IPACMDBG("Posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv6 address\n", 
										 data_addr->if_index);
						evt_data.evt_data = data_addr;
						IPACM_EvtDispatcher::PostEvt(&evt_data);
						/* finish command queue */

					}
					else
					{
						IPACMDBG("route add default gw %d.%d.%d.%d dev %s dstIP: %d.%d.%d.%d\n",
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 16),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 24),
										 dev_name,
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24)
										 );

						/* insert to command queue */
						data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
						if(data_addr == NULL)
						{
							IPACMERR("unable to allocate memory for event data_addr\n");
							return IPACM_FAILURE;
						}

						memcpy(&if_ipv4_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(if_ipv4_addr));
						memcpy(&if_ipipv4_addr_mask, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(if_ipipv4_addr_mask));

						evt_data.event = IPA_ROUTE_ADD_EVENT;
						data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
						data_addr->iptype = IPA_IP_v4;
						data_addr->ipv4_addr = ntohl(if_ipv4_addr);
						data_addr->ipv4_addr_mask = ntohl(if_ipipv4_addr_mask);

            IPACMDBG("Posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv4 addr:0x%x and maxk: 0x%x\n",
										 data_addr->if_index,
										 data_addr->ipv4_addr,
										 data_addr->ipv4_addr_mask);
						evt_data.evt_data = data_addr;
						IPACM_EvtDispatcher::PostEvt(&evt_data);
						/* finish command queue */
					}
				}
			}

			/* ipv6 routing table */
			if((AF_INET6 == msg_ptr->nl_route_info.metainfo.rtm_family) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_type == RTN_UNICAST) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_protocol == RTPROT_KERNEL) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_table == RT_TABLE_MAIN))
			{
				IPACMDBG("\n GOT valid v6-RTM_NEWROUTE event\n");
				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
				if(ret_val != IPACM_SUCCESS)
				{
					IPACMERR("Error while getting interface name\n");
					return IPACM_FAILURE;
				}

				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
				{
					IPACMDBG("Route ADD IPV6 DST: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d, metric %d, dev %s\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 48)),
									 msg_ptr->nl_route_info.metainfo.rtm_dst_len,
									 msg_ptr->nl_route_info.attr_info.priority,
									 dev_name);

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMDBG("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					memcpy(data_addr->ipv6_addr, 
								 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr));

					mask_value_v6 = msg_ptr->nl_route_info.metainfo.rtm_dst_len;
					for(mask_index = 0; mask_index < 4; mask_index++)
					{
						if(mask_value_v6 >= 32)
						{
							mask_v6(32, &data_addr->ipv6_addr_mask[mask_index]);
							mask_value_v6 -= 32;
						}
						else
						{
							mask_v6(mask_value_v6, &data_addr->ipv6_addr_mask[mask_index]);
							mask_value_v6 = 0;
						}
					}

					IPACMDBG("ADD IPV6 MASK %d: %08x:%08x:%08x:%08x \n",
									 msg_ptr->nl_route_info.metainfo.rtm_dst_len,
									 data_addr->ipv6_addr_mask[0],
									 data_addr->ipv6_addr_mask[1],
									 data_addr->ipv6_addr_mask[2],
									 data_addr->ipv6_addr_mask[3]);

					evt_data.event = IPA_ROUTE_ADD_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v6;

					IPACMDBG("Posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv6 addr\n",
									 data_addr->if_index);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}
				
				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_GATEWAY)
				{
					IPACMDBG("Route ADD ::/0  Next Hop: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x, metric %d, dev %s\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 48)),
									 msg_ptr->nl_route_info.attr_info.priority,
									 dev_name);

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					memcpy(data_addr->ipv6_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr));
					memcpy(data_addr->ipv6_addr_mask, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr_mask));

					evt_data.event = IPA_ROUTE_ADD_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v6;

					IPACMDBG("posting IPA_ROUTE_ADD_EVENT with if index:%d, ipv6 address\n",
									 data_addr->if_index);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}

			}
			break;

		case RTM_DELROUTE:
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_route(buffer, buflen, &(msg_ptr->nl_route_info)))
			{
				IPACMDBG("Failed to decode rtm route message\n");
				return IPACM_FAILURE;
			}

			if((msg_ptr->nl_route_info.metainfo.rtm_type == RTN_BROADCAST) && 
				 ((unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24)) != 0)
			{
				if(IPACM_SUCCESS != find_mask((unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24), &mask_value))
				{
					IPACMERR("Failed to decode rtm_delroute message\n");
				}
				else
				{
					IPACMDBG("\n GOT useful RTM_DELROUTE event\n");

					/* take care of subnet mask */
					if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
					{
							 IPACMDBG("DST_ADDRESS:IPV4 %d.%d.%d.0\n",
												(unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
												(unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
												(unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16));
						IPACMDBG("MASK:IPV4 255.255.255.%d\n", mask_value);
						ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
						if(ret_val != IPACM_SUCCESS)
						{
							IPACMERR("Error while getting interface name\n");
						}
						IPACMDBG("RTA_OIF, output Interface %s \n", dev_name);
					}
					IPACMDBG("route del -net %d.%d.%d.0 netmask 255.255.255.%d dev %s \n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
									 mask_value,
									 dev_name);

					/* post event to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_PRIORITY)
					{
						IPACMDBG("Priority %d \n",
										 msg_ptr->nl_route_info.attr_info.priority);
					}
					else;

					memcpy(&if_ipv4_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(if_ipv4_addr));
					if_ipv4_addr = (if_ipv4_addr << 8) >> 8;
					temp = (-1) << 8;
					temp = temp | mask_value;
					if_ipipv4_addr_mask = ntohl(temp);

					evt_data.event = IPA_ROUTE_DEL_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v4;
					data_addr->ipv4_addr = ntohl(if_ipv4_addr);
					data_addr->ipv4_addr_mask = ntohl(if_ipipv4_addr_mask);

					IPACMDBG("Posting IPA_ROUTE_DEL_EVENT with ifindex:%d, ipv4 address 0x%x, mask:0x%x\n",
									 data_addr->if_index,
									 data_addr->ipv4_addr,
									 data_addr->ipv4_addr_mask);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}
			}

			/* take care of route delete of default route & uniroute */
			if((msg_ptr->nl_route_info.metainfo.rtm_type == RTN_UNICAST) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_protocol == RTPROT_BOOT) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_scope == 0) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_table == RT_TABLE_MAIN))
			{

				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
				{
					ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
					if(ret_val != IPACM_SUCCESS)
					{
						IPACMERR("Error while getting interface name\n");
					}
					IPACMDBG("route del -host %d.%d.%d.%d gw %d.%d.%d.%d dev %s\n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding >> 24),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 24),
									 dev_name
									 );

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}
					memcpy(&if_ipv4_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(if_ipv4_addr));
					temp = (-1);
					if_ipipv4_addr_mask = ntohl(temp);

					evt_data.event = IPA_ROUTE_DEL_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v4;
					data_addr->ipv4_addr = ntohl(if_ipv4_addr);
					data_addr->ipv4_addr_mask = ntohl(if_ipipv4_addr_mask);

					IPACMDBG("Posting event IPA_ROUTE_DEL_EVENT with if index:%d, ipv4 address 0x%x, mask:0x%x\n",
									 data_addr->if_index,
									 data_addr->ipv4_addr,
									 data_addr->ipv4_addr_mask);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}
				else
				{
					ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
					if(ret_val != IPACM_SUCCESS)
					{
						IPACMERR("Error while getting interface name\n");
					}

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					if(AF_INET6 == msg_ptr->nl_route_info.metainfo.rtm_family)
					{
						if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_PRIORITY)
						{
							IPACMDBG("ip -6 route del default dev %s metric %d\n",
											 dev_name,
											 msg_ptr->nl_route_info.attr_info.priority);
						}
						else
						{
							IPACMDBG("ip -6 route del default dev %s\n", dev_name);
						}
						memcpy(data_addr->ipv6_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv6_addr));
						memcpy(data_addr->ipv6_addr_mask, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv6_addr_mask));
						data_addr->iptype = IPA_IP_v6;
					}
					else
					{
						IPACMDBG("route del default gw %d.%d.%d.%d dev %s\n",
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 8),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 16),
										 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding >> 24),
										 dev_name);

						memcpy(&data_addr->ipv4_addr,
									 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv4_addr));
						data_addr->ipv4_addr = ntohl(data_addr->ipv4_addr);

						memcpy(&data_addr->ipv4_addr_mask,
									 msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
									 sizeof(data_addr->ipv4_addr_mask));
						data_addr->ipv4_addr_mask = ntohl(data_addr->ipv4_addr_mask);

						data_addr->iptype = IPA_IP_v4;
					}

					evt_data.event = IPA_ROUTE_DEL_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;

					IPACMDBG("Posting IPA_ROUTE_DEL_EVENT with if index:%d\n",
									 data_addr->if_index);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}
			}

			/* ipv6 routing table */
			if((AF_INET6 == msg_ptr->nl_route_info.metainfo.rtm_family) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_type == RTN_UNICAST) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_protocol == RTPROT_KERNEL) && 
				 (msg_ptr->nl_route_info.metainfo.rtm_table == RT_TABLE_MAIN))
			{
				IPACMDBG("\n GOT valid v6-RTM_DELROUTE event\n");
				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_route_info.attr_info.oif_index);
				if(ret_val != IPACM_SUCCESS)
				{
					IPACMERR("Error while getting interface name");
				}

				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_DST)
				{
					IPACMDBG("DEL IPV6 DST: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x/%d, metric %d, dev %s\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.dst_addr).__ss_padding)[1] >> 48)),
									 msg_ptr->nl_route_info.metainfo.rtm_dst_len,
									 msg_ptr->nl_route_info.attr_info.priority,
									 dev_name);

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}

					memcpy(data_addr->ipv6_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr));

					mask_value_v6 = msg_ptr->nl_route_info.metainfo.rtm_dst_len;
					for(mask_index = 0; mask_index < 4; mask_index++)
					{
						IPACMDBG("%dst %d \n",
										 mask_index,
										 mask_value_v6);
						if(mask_value_v6 >= 32)
						{
							mask_v6(32, &data_addr->ipv6_addr_mask[mask_index]);
							mask_value_v6 -= 32;
							IPACMDBG("%dst: %08x \n",
											 mask_index,
											 data_addr->ipv6_addr_mask[mask_index]);
						}
						else
						{
							mask_v6(mask_value_v6, data_addr->ipv6_addr_mask);
							mask_value_v6 = 0;
							IPACMDBG("%dst: %08x \n",
											 mask_index,
											 data_addr->ipv6_addr_mask[mask_index]);
						}
					}

					IPACMDBG("DEL IPV6 MASK 0st: %08x ",
									 data_addr->ipv6_addr_mask[0]);
					IPACMDBG("1st: %08x ",
									 data_addr->ipv6_addr_mask[1]);
					IPACMDBG("2st: %08x ",
									 data_addr->ipv6_addr_mask[2]);
					IPACMDBG("3st: %08x \n",
									 data_addr->ipv6_addr_mask[3]);

					evt_data.event = IPA_ROUTE_DEL_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v6;

					IPACMDBG("posting event IPA_ROUTE_DEL_EVENT with if index:%d, ipv4 address\n",
									 data_addr->if_index);
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}

				if(msg_ptr->nl_route_info.attr_info.param_mask & IPA_RTA_PARAM_GATEWAY)
				{
					IPACMDBG("DEL ::/0  Next Hop: %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x, metric %d, dev %s\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_route_info.attr_info.gateway_addr).__ss_padding)[1] >> 48)),
									 msg_ptr->nl_route_info.attr_info.priority,
									 dev_name);

					/* insert to command queue */
					data_addr = (ipacm_event_data_addr *)malloc(sizeof(ipacm_event_data_addr));
					if(data_addr == NULL)
					{
						IPACMERR("unable to allocate memory for event data_addr\n");
						return IPACM_FAILURE;
					}
					memcpy(data_addr->ipv6_addr, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr));
					memcpy(data_addr->ipv6_addr_mask, msg_ptr->nl_route_info.attr_info.dst_addr.__ss_padding,
								 sizeof(data_addr->ipv6_addr_mask));

					evt_data.event = IPA_ROUTE_DEL_EVENT;
					data_addr->if_index = msg_ptr->nl_route_info.attr_info.oif_index;
					data_addr->iptype = IPA_IP_v6;

					IPACMDBG("Posting IPA_ROUTE_DEL_EVENT with if index: %d, ipv6 addr\n",
									 data_addr->if_index)
					evt_data.evt_data = data_addr;
					IPACM_EvtDispatcher::PostEvt(&evt_data);
					/* finish command queue */
				}

			}
			break;

		case RTM_NEWNEIGH:
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_neigh(buffer, buflen, &(msg_ptr->nl_neigh_info)))
			{
				IPACMERR("Failed to decode rtm neighbor message\n");
				return IPACM_FAILURE;
			}

			IPACMDBG("RTM_NEWNEIGH, ndm_state:0x%x", msg_ptr->nl_neigh_info.metainfo.ndm_state)
			if((NUD_PERMANENT | NUD_STALE) & (msg_ptr->nl_neigh_info.metainfo.ndm_state))
			{
				IPACMDBG("\n GOT RTM_NEWNEIGH event\n");
				/* insert to command queue */
				data_all = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
				if(data_all == NULL)
				{
					IPACMERR("unable to allocate memory for event data_all\n");
					return IPACM_FAILURE;
				}

				if(AF_INET6 == msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family)
				{
					IPACMDBG("IPV6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 48)));

					memcpy(data_all->ipv6_addr, 
								 msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
								 sizeof(data_all->ipv6_addr));
					data_all->iptype = IPA_IP_v6;
				}
				else
				{
					IPACMDBG("IPV4 %d.%d.%d.%d\n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 24));


					memcpy(&data_all->ipv4_addr, 
								 msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
								 sizeof(data_all->ipv4_addr));
					data_all->ipv4_addr = ntohl(data_all->ipv4_addr);
					data_all->iptype = IPA_IP_v4;
				}

				memcpy(data_all->mac_addr,
							 msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
							 sizeof(data_all->mac_addr));
				evt_data.event = IPA_NEW_NEIGH_EVENT;
				data_all->if_index = msg_ptr->nl_neigh_info.metainfo.ndm_ifindex;

				IPACMDBG("posting IPA_NEW_NEIGH_EVENT with if index:%d\n",
								 data_all->if_index);
				evt_data.evt_data = data_all;
				IPACM_EvtDispatcher::PostEvt(&evt_data);
				/* finish command queue */

				IPACMDBG("NDA_LLADDR:MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[0],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[1],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[2],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[3],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[4],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[5]);

				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_neigh_info.metainfo.ndm_ifindex);
				if(ret_val != IPACM_SUCCESS)
				{
					IPACMERR("Error while getting interface index\n");
				}
				else
				{
					IPACMDBG("Interface %s \n", dev_name);
				}
			}
			break;

		case RTM_DELNEIGH:
			if(IPACM_SUCCESS != ipa_nl_decode_rtm_neigh(buffer, buflen, &(msg_ptr->nl_neigh_info)))
			{
				IPACMERR("Failed to decode rtm neighbor message\n");
				return IPACM_FAILURE;
			}

			if((NUD_PERMANENT | NUD_STALE) & (msg_ptr->nl_neigh_info.metainfo.ndm_state))
			{
				IPACMDBG("\n GOT RTM_DELNEIGH event\n");
				/* insert to command queue */
				data_all = (ipacm_event_data_all *)malloc(sizeof(ipacm_event_data_all));
				if(data_all == NULL)
				{
					IPACMERR("unable to allocate memory for event data_all\n");
					return IPACM_FAILURE;
				}

				if(AF_INET6 == msg_ptr->nl_neigh_info.attr_info.local_addr.ss_family)
				{
					IPACMDBG("IPV6 %04x:%04x:%04x:%04x:%04x:%04x:%04x:%04x\n",
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[0] >> 48)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1])),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 16)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 32)),
									 (uint16_t)(ntohs(((uint64_t *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding)[1] >> 48)));

					memcpy(data_all->ipv6_addr, msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
								 sizeof(data_all->ipv6_addr));
					data_all->iptype = IPA_IP_v6;
				}
				else
				{
					IPACMDBG("IPV4 %d.%d.%d.%d\n",
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 8),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 16),
									 (unsigned char)(*(unsigned int *)&(msg_ptr->nl_neigh_info.attr_info.local_addr).__ss_padding >> 24));


					memcpy(&data_all->ipv4_addr, msg_ptr->nl_neigh_info.attr_info.local_addr.__ss_padding,
								 sizeof(data_all->ipv4_addr));
					data_all->iptype = IPA_IP_v4;
				}

				memcpy(data_all->mac_addr,
							 msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr.sa_data,
							 sizeof(data_all->mac_addr));
				evt_data.event = IPA_DEL_NEIGH_EVENT;
				data_all->if_index = msg_ptr->nl_neigh_info.metainfo.ndm_ifindex;

				IPACMDBG("posting IPA_DEL_NEIGH_EVENT with if index:%d\n",
								 data_all->if_index);
				evt_data.evt_data = data_all;
				IPACM_EvtDispatcher::PostEvt(&evt_data);
				/* finish command queue */

				IPACMDBG("NDA_LLADDR:MAC %02x:%02x:%02x:%02x:%02x:%02x\n",
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[0],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[1],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[2],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[3],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[4],
								 (unsigned char)(msg_ptr->nl_neigh_info.attr_info.lladdr_hwaddr).sa_data[5]);

				ret_val = ipa_get_if_name(dev_name, msg_ptr->nl_neigh_info.metainfo.ndm_ifindex);
				if(ret_val != IPACM_SUCCESS)
				{
					//IPACM_LOG_MSG("Error while getting interface index");
				}
				else
				{
					IPACMDBG("Interface %s \n", dev_name);
				}
			}
			break;

		default:
			IPACMDBG(" ignore NL event %d!!!\n ", nlh->nlmsg_type);
			break;

		}
		nlh = NLMSG_NEXT(nlh, buflen);
	}

	return IPACM_SUCCESS;
}


/*  Virtual function registered to receive incoming messages over the NETLINK routing socket*/
int ipa_nl_recv_msg(int fd)
{
	struct msghdr *msghdr = NULL;
	//struct sockaddr_nl *nladdr = NULL;
	struct iovec *iov = NULL;
	unsigned int msglen = 0;
	ipa_nl_msg_t *nlmsg = NULL;

	nlmsg = (ipa_nl_msg_t *)malloc(sizeof(ipa_nl_msg_t));
	if(NULL == nlmsg)
	{
		IPACMERR("Failed alloc of nlmsg \n");
		goto error;
	}
	else
	{
		if(IPACM_SUCCESS != ipa_nl_recv(fd, &msghdr, &msglen))
		{
			IPACMERR("Failed to receive nl message \n");
			goto error;
		}

		//nladdr = (struct sockaddr_nl *)msghdr->msg_name;
		iov = msghdr->msg_iov;

		memset(nlmsg, 0, sizeof(ipa_nl_msg_t));
		if(IPACM_SUCCESS != ipa_nl_decode_nlmsg((char *)iov->iov_base, msglen, nlmsg))
		{
			IPACMERR("Failed to decode nl message \n");
			goto error;
		}

		ipa_nl_release_msg(msghdr);
		free(nlmsg);
	}

	return IPACM_SUCCESS;

error:
	if(msghdr)
	{
		ipa_nl_release_msg(msghdr);
	}
	if(nlmsg)
	{
		free(nlmsg);
	}

	return IPACM_FAILURE;
}

/*  get ipa interface name */
int ipa_get_if_name
(
	 char *if_name,
	 int if_index
	 )
{
	int fd;
	struct ifreq ifr;

	if((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
	{
		IPACMERR("get interface name socket create failed \n");
		return IPACM_FAILURE;
	}

	memset(&ifr, 0, sizeof(struct ifreq));
	ifr.ifr_ifindex = if_index;
	IPACMDBG("Interface index %d\n", if_index);

	if(ioctl(fd, SIOCGIFNAME, &ifr) < 0)
	{
		IPACMERR("call_ioctl_on_dev: ioctl failed:\n");
		close(fd);
		return IPACM_FAILURE;
	}

	(void)strncpy(if_name, ifr.ifr_name, sizeof(ifr.ifr_name));
	IPACMDBG("interface name %s\n", ifr.ifr_name);
	close(fd);

	return IPACM_SUCCESS;
}

/* Initialization routine for listener on NetLink sockets interface */
int ipa_nl_listener_init
(
	 unsigned int nl_type,
	 unsigned int nl_groups,
	 ipa_nl_sk_fd_set_info_t *sk_fdset,
	 ipa_sock_thrd_fd_read_f read_f
	 )
{
	ipa_nl_sk_info_t sk_info, sk_info2;
	int ret_val;

	memset(&sk_info, 0, sizeof(ipa_nl_sk_info_t));
	IPACMDBG("Entering IPA NL listener init\n");

	if(ipa_nl_open_socket(&sk_info, nl_type, nl_groups) == IPACM_SUCCESS)
	{
		IPACMDBG("IPA Open netlink socket succeeds\n");
	}
	else
	{
		IPACMERR("Netlink socket open failed\n");
		return IPACM_FAILURE;
	}

	/* Add NETLINK socket to the list of sockets that the listener 
					 thread should listen on. */

	if(ipa_nl_addfd_map(sk_fdset, sk_info.sk_fd, read_f) != IPACM_SUCCESS)
	{
		IPACMERR("cannot add nl routing sock for reading\n");
		return IPACM_FAILURE;
	}

	/* Start the socket listener thread */
	ret_val = ipa_nl_sock_listener_start(sk_fdset);

	if(ret_val != IPACM_SUCCESS)
	{
		IPACMERR("Failed to start NL listener\n");
	}

	return IPACM_SUCCESS;
}

/* find the newroute subnet mask */
int find_mask(int ip_v4_last, int *mask_value)
{

	switch(ip_v4_last)
	{

	case 3:
		*mask_value = 252;
		return IPACM_SUCCESS;
		break;

	case 7:
		*mask_value = 248;
		return IPACM_SUCCESS;
		break;

	case 15:
		*mask_value = 240;
		return IPACM_SUCCESS;
		break;

	case 31:
		*mask_value = 224;
		return IPACM_SUCCESS;
		break;

	case 63:
		*mask_value = 192;
		return IPACM_SUCCESS;
		break;

	case 127:
		*mask_value = 128;
		return IPACM_SUCCESS;
		break;

	case 255:
		*mask_value = 0;
		return IPACM_SUCCESS;
		break;

	default:
		return IPACM_FAILURE;
		break;

	}
}

/* map mask value for ipv6 */
int mask_v6(int index, uint32_t *mask)
{
	switch(index)
	{

	case 0:
		*mask = 0x00000000;
		return IPACM_SUCCESS;
		break;
	case 4:
		*mask = 0xf0000000;
		return IPACM_SUCCESS;
		break;
	case 8:
		*mask = 0xff000000;
		return IPACM_SUCCESS;
		break;
	case 12:
		*mask = 0xfff00000;
		return IPACM_SUCCESS;
		break;
	case 16:
		*mask = 0xffff0000;
		return IPACM_SUCCESS;
		break;
	case 20:
		*mask = 0xfffff000;
		return IPACM_SUCCESS;
		break;
	case 24:
		*mask = 0xffffff00;
		return IPACM_SUCCESS;
		break;
	case 28:
		*mask = 0xfffffff0;
		return IPACM_SUCCESS;
		break;
	case 32:
		*mask = 0xffffffff;
		return IPACM_SUCCESS;
		break;
	default:
		return IPACM_FAILURE;
		break;

	}
}


