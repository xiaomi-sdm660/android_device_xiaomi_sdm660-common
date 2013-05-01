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
	IPACM_Filtering.cpp

	@brief
	This file implements the IPACM filtering functionality.

	@Author
	Skylar Chang

*/
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

#include "IPACM_Filtering.h"
#include <IPACM_Log.h>

const char *IPACM_Filtering::DEVICE_NAME = "/dev/ipa";

IPACM_Filtering::IPACM_Filtering()
{
	fd = open(DEVICE_NAME, O_RDWR);
	if (0 == fd)
	{
		IPACMERR("Failed opening %s.\n", DEVICE_NAME);
	}
}

IPACM_Filtering::~IPACM_Filtering()
{
	close(fd);
}

bool IPACM_Filtering::DeviceNodeIsOpened()
{
	return fd;
}

bool IPACM_Filtering::AddFilteringRule(struct ipa_ioc_add_flt_rule const *ruleTable)
{
	int retval = 0;

	IPACMDBG("Printing filter add attributes\n");
	IPACMDBG("ip type: %d\n", ruleTable->ip);
	IPACMDBG("Number of rules: %d\n", ruleTable->num_rules);
	IPACMDBG("End point: %d and global value: %d\n", ruleTable->ep, ruleTable->global);
	IPACMDBG("commit value: %d\n", ruleTable->commit);
	for (int cnt=0; cnt<ruleTable->num_rules; cnt++)
	{
		IPACMDBG("Filter rule:%d attrib mask: 0x%x\n",
						 cnt, 
						 ruleTable->rules[cnt].rule.attrib.attrib_mask);
	}

	retval = ioctl(fd, IPA_IOC_ADD_FLT_RULE, ruleTable);
	if (retval != 0)
	{
		IPACMERR("Failed adding Filtering rule %p\n", ruleTable);
		PERROR("unable to add filter rule:");

		for (int cnt = 0; cnt < ruleTable->num_rules; cnt++)
		{
			if (ruleTable->rules[cnt].status != 0)
			{
				IPACMERR("Adding Filter rule:%d failed with status:%d\n",
								 cnt, ruleTable->rules[cnt].status);
			}
		}
		return false;
	}

	for (int cnt = 0; cnt<ruleTable->num_rules; cnt++)
	{
		if(ruleTable->rules[cnt].status != 0)
		{
			IPACMERR("Adding Filter rule:%d failed with status:%d\n",
							 cnt, ruleTable->rules[cnt].status);
		}
	}

	IPACMDBG("Added Filtering rule %p\n", ruleTable);
	return true;
}

bool IPACM_Filtering::DeleteFilteringRule(struct ipa_ioc_del_flt_rule *ruleTable)
{
	int retval = 0;

	retval = ioctl(fd, IPA_IOC_DEL_FLT_RULE, ruleTable);
	if (retval != 0)
	{
		IPACMERR("Failed deleting Filtering rule %p\n", ruleTable);
		return false;
	}

	IPACMDBG("Deleted Filtering rule %p\n", ruleTable);
	return true;
}

bool IPACM_Filtering::Commit(enum ipa_ip_type ip)
{
	int retval = 0;

	retval = ioctl(fd, IPA_IOC_COMMIT_FLT, ip);
	if (retval != 0)
	{
		IPACMERR("failed committing Filtering rules.\n");
		return false;
	}

	IPACMDBG("Committed Filtering rules to IPA HW.\n");
	return true;
}

bool IPACM_Filtering::Reset(enum ipa_ip_type ip)
{
	int retval = 0;

	retval = ioctl(fd, IPA_IOC_RESET_FLT, ip);
	retval |= ioctl(fd, IPA_IOC_COMMIT_FLT, ip);
	if (retval)
	{
		IPACMERR("failed resetting Filtering block.\n");
		return false;
	}

	IPACMDBG("Reset command issued to IPA Filtering block.\n");
	return true;
}

bool IPACM_Filtering::DeleteFilteringHdls
(
	 uint32_t *flt_rule_hdls,
	 ipa_ip_type ip,
	 uint8_t num_rules
)
{
	struct ipa_ioc_del_flt_rule *flt_rule;
	bool res = true;
	int len = 0, cnt = 0;
        const uint8_t UNIT_RULES = 1;

	len = (sizeof(struct ipa_ioc_del_flt_rule)) + (UNIT_RULES * sizeof(struct ipa_flt_rule_del));
	flt_rule = (struct ipa_ioc_del_flt_rule *)malloc(len);
	if (flt_rule == NULL)
	{
		IPACMERR("unable to allocate memory for del filter rule\n");
		return false;
	}

	for (cnt = 0; cnt < num_rules; cnt++)
	{
	    memset(flt_rule, 0, len);
	    flt_rule->commit = 1;
	    flt_rule->num_hdls = UNIT_RULES;
	    flt_rule->ip = ip;

	    if (flt_rule_hdls[cnt] == 0)
	    {
		   IPACMERR("invalid filter handle passed, ignoring it: %d\n", cnt)
	    }
            else
	    {

		   flt_rule->hdl[0].status = -1;
		   flt_rule->hdl[0].hdl = flt_rule_hdls[cnt];
		   IPACMDBG("Deleting filter hdl:(0x%x) with ip type: %d\n", flt_rule_hdls[cnt], ip);

	           if (DeleteFilteringRule(flt_rule) == false)
	           {
		        PERROR("Filter rule deletion failed!\n");
		        res = false;
		        goto fail;
	           }
		   else
	           {

		        if (flt_rule->hdl[0].status != 0)
		        {
			     IPACMERR("Filter rule hdl 0x%x deletion failed with error:%d\n",
		        					 flt_rule->hdl[0].hdl, flt_rule->hdl[0].status);
			     res = false;
			     goto fail;
		        }
		   
		   }	   
	    }
	}

fail:
	free(flt_rule);

	return res;
}

