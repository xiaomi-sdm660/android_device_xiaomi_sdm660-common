/* Copyright (c) 2009,2011 Code Aurora Forum. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef LOC_UTIL_LOG_H
#define LOC_UTIL_LOG_H

#ifdef FEATURE_LOC_API_V02_QNX_MOD
//error logs
#define LOC_LOGE(...) printf(__VA_ARGS__)
//warning logs
#define LOC_LOGW(...) printf(__VA_ARGS__)
// debug logs
#define LOC_LOGD(...) printf(__VA_ARGS__)
//info logs
#define LOC_LOGI(...) printf(__VA_ARGS__)
//verbose logs
#define LOC_LOGV(...) printf(__VA_ARGS__)

#define MODEM_LOG_CALLFLOW(SPEC, VAL)
#define EXIT_LOG_CALLFLOW(SPEC, VAL)

#endif //FEATURE_LOC_API_V02_QNX_MOD

#ifdef LOC_UTIL_TARGET_OFF_TARGET

#include <stdio.h>

//error logs
#define LOC_LOGE(...) printf(__VA_ARGS__)
//warning logs
#define LOC_LOGW(...) printf(__VA_ARGS__)
// debug logs
#define LOC_LOGD(...) printf(__VA_ARGS__)
//info logs
#define LOC_LOGI(...) printf(__VA_ARGS__)
//verbose logs
#define LOC_LOGV(...) printf(__VA_ARGS__)

// get around strl*: not found in glibc
// TBD:look for presence of eglibc other libraries
// with strlcpy supported.
#define strlcpy(X,Y,Z) strcpy(X,Y)
#define strlcat(X,Y,Z) strcat(X,Y)

#define MODEM_LOG_CALLFLOW(SPEC, VAL)
#define EXIT_LOG_CALLFLOW(SPEC, VAL)

#elif defined(_ANDROID_)

#include <log_util.h>

#endif //LOC_UTIL_TARGET_OFF_TARGET


#endif //LOC_UTIL_LOG_H
