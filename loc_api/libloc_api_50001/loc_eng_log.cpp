/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation, nor the names of its
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

#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_eng"

#include "loc_log.h"
#include "loc_eng_log.h"
#include "loc_eng_msg_id.h"

static loc_name_val_s_type loc_eng_msgs[] =
{
    NAME_VAL( LOC_ENG_MSG_QUIT ),
    NAME_VAL( LOC_ENG_MSG_ENGINE_DOWN ),
    NAME_VAL( LOC_ENG_MSG_ENGINE_UP ),
    NAME_VAL( LOC_ENG_MSG_START_FIX ),
    NAME_VAL( LOC_ENG_MSG_STOP_FIX ),
    NAME_VAL( LOC_ENG_MSG_SET_POSITION_MODE ),
    NAME_VAL( LOC_ENG_MSG_SET_TIME ),
    NAME_VAL( LOC_ENG_MSG_INJECT_XTRA_DATA ),
    NAME_VAL( LOC_ENG_MSG_INJECT_LOCATION ),
    NAME_VAL( LOC_ENG_MSG_DELETE_AIDING_DATA ),
    NAME_VAL( LOC_ENG_MSG_SET_APN ),
    NAME_VAL( LOC_ENG_MSG_SET_SERVER_URL ),
    NAME_VAL( LOC_ENG_MSG_SET_SERVER_IPV4 ),
    NAME_VAL( LOC_ENG_MSG_ENABLE_DATA ),
    NAME_VAL( LOC_ENG_MSG_SUPL_VERSION ),
    NAME_VAL( LOC_ENG_MSG_SET_SENSOR_CONTROL_CONFIG ),
    NAME_VAL( LOC_ENG_MSG_SET_SENSOR_PROPERTIES ),
    NAME_VAL( LOC_ENG_MSG_SET_SENSOR_PERF_CONTROL_CONFIG ),
    NAME_VAL( LOC_ENG_MSG_MUTE_SESSION ),
    NAME_VAL( LOC_ENG_MSG_ATL_OPEN_SUCCESS ),
    NAME_VAL( LOC_ENG_MSG_ATL_CLOSED ),
    NAME_VAL( LOC_ENG_MSG_ATL_OPEN_FAILED ),
    NAME_VAL( LOC_ENG_MSG_REPORT_POSITION ),
    NAME_VAL( LOC_ENG_MSG_REPORT_SV ),
    NAME_VAL( LOC_ENG_MSG_REPORT_STATUS ),
    NAME_VAL( LOC_ENG_MSG_REPORT_NMEA ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_ATL ),
    NAME_VAL( LOC_ENG_MSG_RELEASE_ATL ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_BIT ),
    NAME_VAL( LOC_ENG_MSG_RELEASE_BIT ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_WIFI ),
    NAME_VAL( LOC_ENG_MSG_RELEASE_WIFI ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_NI ),
    NAME_VAL( LOC_ENG_MSG_INFORM_NI_RESPONSE ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_XTRA_DATA ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_TIME ),
    NAME_VAL( LOC_ENG_MSG_EXT_POWER_CONFIG ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_POSITION ),
    NAME_VAL( LOC_ENG_MSG_LPP_CONFIG ),
    NAME_VAL( LOC_ENG_MSG_A_GLONASS_PROTOCOL ),
    NAME_VAL( LOC_ENG_MSG_LOC_INIT ),
    NAME_VAL( LOC_ENG_MSG_REQUEST_SUPL_ES ),
    NAME_VAL( LOC_ENG_MSG_CLOSE_DATA_CALL)
};
static int loc_eng_msgs_num = sizeof(loc_eng_msgs) / sizeof(loc_name_val_s_type);

/* Find Android GPS status name */
const char* loc_get_msg_name(int id)
{
   return loc_get_name_from_val(loc_eng_msgs, loc_eng_msgs_num, (long) id);
}



