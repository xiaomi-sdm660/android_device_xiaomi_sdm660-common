/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
 */

#ifndef __PLATFORM_LIB_MACROS_H__
#define __PLATFORM_LIB_MACROS_H__

#ifdef USE_GLIB

#define strlcat g_strlcat
#define strlcpy g_strlcpy

#define ALOGE(format, x...) printf("E/%s (%d): " format "\n", LOG_TAG, getpid(), ##x)
#define ALOGW(format, x...) printf("W/%s (%d): " format "\n", LOG_TAG, getpid(), ##x)
#define ALOGI(format, x...) printf("I/%s (%d): " format "\n", LOG_TAG, getpid(), ##x)
#define ALOGD(format, x...) printf("D/%s (%d): " format "\n", LOG_TAG, getpid(), ##x)
#define ALOGV(format, x...) printf("V/%s (%d): " format "\n", LOG_TAG, getpid(), ##x)

#define GETTID_PLATFORM_LIB_ABSTRACTION (syscall(SYS_gettid))

#define LOC_EXT_CREATE_THREAD_CB_PLATFORM_LIB_ABSTRACTION createPthread
#define ELAPSED_MILLIS_SINCE_BOOT_PLATFORM_LIB_ABSTRACTION (elapsedMillisSinceBoot())


#else

#define GETTID_PLATFORM_LIB_ABSTRACTION (gettid())
#define LOC_EXT_CREATE_THREAD_CB_PLATFORM_LIB_ABSTRACTION android::AndroidRuntime::createJavaThread
#define ELAPSED_MILLIS_SINCE_BOOT_PLATFORM_LIB_ABSTRACTION  (android::elapsedRealtime())

#endif

#endif
