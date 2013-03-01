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
	ipa_nat_logi.h

	@brief
	This file implements the IPAM log functionality.

	@Author
	

*/

#ifndef IPA_NAT_LOGI_H
#define IPA_NAT_LOGI_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <string.h>

#define NAT_LOG_SIZE 200

#define PERROR(fmt)   printf("%s:%d %s()", __FILE__, __LINE__, __FUNCTION__);\
                      perror(fmt);

#define IPADBG(fmt, ...) {\
                             int n =0; \
                             n = snprintf(nat_log_buf, sizeof(nat_log_buf), "%s:%d %s() ", __FILE__,  __LINE__, __FUNCTION__);\
                             snprintf((nat_log_buf+n), (sizeof(nat_log_buf)-n-1), fmt, ##__VA_ARGS__);\
                             log_nat_message(nat_log_buf);\
				  		             }


#define IPAERR(fmt, ...) {\
                             int n =0; \
                             n = snprintf(nat_log_buf, sizeof(nat_log_buf), "%s:%d %s() %s", __FILE__,  __LINE__, __FUNCTION__, "Error:");\
                             snprintf((nat_log_buf+n), (sizeof(nat_log_buf)-n-1), fmt, ##__VA_ARGS__);\
                             log_nat_message(nat_log_buf);\
				  		             }

extern void log_nat_message(char *msg);
extern char nat_log_buf[NAT_LOG_SIZE];

#ifdef __cplusplus
}
#endif

#endif /* IPA_NAT_LOGI_H */
