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
	IPACM_log.cpp

	@brief
	This file implements the IPAM log functionality.

	@Author
	Skylar Chang

*/
#include "IPACM_Log.h"
#include <stdlib.h>
#include <unistd.h>

/* write logging to a local file */
#define LOG_USE_FILE "/etc/IPACM_LOG_F"
#define FILE_NAME "/usr/ipacm_log.txt"

/* To use syslog for logging and
	 use logread cmd to read */
#define LOG_USE_SYS  "/etc/IPACM_LOG_S"

/* Maximum log file size. 1MB(1048576)  */
#define MAX_LOG_FILE_SIZE (1048576ul/4)

char log_buf[LOG_SIZE];

void logmessage(char *msg, int log_level)
{
#ifdef DEBUG
	 static FILE *fp = NULL;
	 static bool is_sys_log_open = false;
	 int log_sys = 0, log_file = 0;

	 printf("%s\n", msg);

	 if(access(LOG_USE_SYS, F_OK) != -1 )
	 {
		 log_sys = 1;
	 }
	 else if(access(LOG_USE_FILE, F_OK) != -1)
	 {
		 log_file = 1;
	 }
	 else
	 {
		 log_sys = 0;
		 log_file = 0;
	 }

	 if(log_sys)
	 {
		 if(is_sys_log_open == false)
		 {
			 setlogmask(LOG_UPTO(LOG_DEBUG));
			 openlog("IPACM", LOG_PID, LOG_LOCAL0);

			 is_sys_log_open = true;
		 }
		 syslog(log_level, msg);
	 }
	 else if(log_file)
	 {
		 if(fp == NULL)
		 {
				fp = fopen(FILE_NAME, "w+");
				if(fp  == NULL)
				{
					 printf("unable to open file\n");
					 return;
				}
		 }
		 
		 if(ftell(fp) > MAX_LOG_FILE_SIZE)
		 {
			 rewind(fp);
			 fprintf(fp, "====Rewinding=====\n");
		 }

		 fprintf(fp, msg);
		 fflush(fp);
	}
#endif
	return;
}


