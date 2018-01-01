/*
 * Copyright (c) 2018, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * *    * Redistributions of source code must retain the above copyright
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

#ifndef __POWER_HELPER_H__
#define __POWER_HELPER_H__

#ifdef __cplusplus
extern "C" {
#endif

enum stats_type {
    //Platform Stats
    RPM_MODE_XO = 0,
    RPM_MODE_VMIN,
    RPM_MODE_MAX,
    XO_VOTERS_START = RPM_MODE_MAX,
    VOTER_APSS = XO_VOTERS_START,
    VOTER_MPSS,
    VOTER_ADSP,
    VOTER_SLPI,
    MAX_PLATFORM_STATS,

    //WLAN Stats
    WLAN_POWER_DEBUG_STATS = 0,
    MAX_WLAN_STATS,
};

enum subsystem_type {
    SUBSYSTEM_WLAN = 0,

    //Don't add any lines after this line
    SUBSYSTEM_COUNT
};

enum wlan_sleep_states {
    WLAN_STATE_ACTIVE = 0,
    WLAN_STATE_DEEP_SLEEP,

    //Don't add any lines after this line
    WLAN_STATES_COUNT
};

enum wlan_power_params {
    CUMULATIVE_SLEEP_TIME_MS = 0,
    CUMULATIVE_TOTAL_ON_TIME_MS,
    DEEP_SLEEP_ENTER_COUNTER,
    LAST_DEEP_SLEEP_ENTER_TSTAMP_MS,

    //Don't add any lines after this line
    WLAN_POWER_PARAMS_COUNT
};


#define PLATFORM_SLEEP_MODES_COUNT RPM_MODE_MAX

#define MAX_RPM_PARAMS 2
#define XO_VOTERS (MAX_PLATFORM_STATS - XO_VOTERS_START)
#define VMIN_VOTERS 0

struct stat_pair {
    enum stats_type stat;
    const char *label;
    const char **parameters;
    size_t num_parameters;
};

int extract_platform_stats(uint64_t *list);
int extract_wlan_stats(uint64_t *list);

#ifdef __cplusplus
}
#endif

#endif //__POWER_HELPER_H__
