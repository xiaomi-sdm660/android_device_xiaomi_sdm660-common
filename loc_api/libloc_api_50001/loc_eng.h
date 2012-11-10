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

#ifndef LOC_ENG_H
#define LOC_ENG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Uncomment to keep all LOG messages (LOGD, LOGI, LOGV, etc.)
#define MAX_NUM_ATL_CONNECTIONS  2
// Define boolean type to be used by libgps on loc api module
typedef unsigned char boolean;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <loc.h>
#include <loc_eng_xtra.h>
#include <loc_eng_ni.h>
#include <loc_eng_agps.h>
#include <loc_cfg.h>
#include <loc_log.h>
#include <log_util.h>
#include <loc_eng_msg.h>
#include <loc_eng_agps.h>
#include <LocApiAdapter.h>

// The data connection minimal open time
#define DATA_OPEN_MIN_TIME        1  /* sec */

// The system sees GPS engine turns off after inactive for this period of time
#define GPS_AUTO_OFF_TIME         2  /* secs */
#define SUCCESS              TRUE
#define FAILURE                 FALSE
#define INVALID_ATL_CONNECTION_HANDLE -1

#define MAX_APN_LEN 100
#define MAX_URL_LEN 256
#define smaller_of(a, b) (((a) > (b)) ? (b) : (a))

enum loc_mute_session_e_type {
   LOC_MUTE_SESS_NONE = 0,
   LOC_MUTE_SESS_WAIT,
   LOC_MUTE_SESS_IN_SESSION
};

// Module data
typedef struct
{
    LocApiAdapter                 *client_handle;
    loc_location_cb_ext            location_cb;
    gps_status_callback            status_cb;
    loc_sv_status_cb_ext           sv_status_cb;
    agps_status_callback           agps_status_cb;
    gps_nmea_callback              nmea_cb;
    gps_ni_notify_callback         ni_notify_cb;
    gps_acquire_wakelock           acquire_wakelock_cb;
    gps_release_wakelock           release_wakelock_cb;
    boolean                        intermediateFix;
    AGpsStatusValue                agps_status;
    // used to defer stopping the GPS engine until AGPS data calls are done
    boolean                        agps_request_pending;
    boolean                        stop_request_pending;
    loc_eng_xtra_data_s_type       xtra_module_data;
    loc_eng_ni_data_s_type         loc_eng_ni_data;

    boolean                        navigating;

    // AGPS state machines
    AgpsStateMachine*              agnss_nif;
    AgpsStateMachine*              internet_nif;

    // GPS engine status
    GpsStatusValue                 engine_status;
    GpsStatusValue                 fix_session_status;

    // Aiding data information to be deleted, aiding data can only be deleted when GPS engine is off
    GpsAidingData                  aiding_data_for_deletion;

    void*                          context;

    loc_eng_msg_position_mode      position_mode;

    // For muting session broadcast
    loc_mute_session_e_type        mute_session_state;

    // Address buffers, for addressing setting before init
    int    supl_host_set;
    char   supl_host_buf[101];
    int    supl_port_buf;
    int    c2k_host_set;
    char   c2k_host_buf[101];
    int    c2k_port_buf;
    int    mpc_host_set;
    char   mpc_host_buf[101];
    int    mpc_port_buf;
} loc_eng_data_s_type;


int  loc_eng_init(loc_eng_data_s_type &loc_eng_data,
                  LocCallbacks* callbacks,
                  LOC_API_ADAPTER_EVENT_MASK_T event);
int  loc_eng_start(loc_eng_data_s_type &loc_eng_data);
int  loc_eng_stop(loc_eng_data_s_type &loc_eng_data);
void loc_eng_cleanup(loc_eng_data_s_type &loc_eng_data);
int  loc_eng_inject_time(loc_eng_data_s_type &loc_eng_data,
                         GpsUtcTime time, int64_t timeReference,
                         int uncertainty);
int  loc_eng_inject_location(loc_eng_data_s_type &loc_eng_data,
                             double latitude, double longitude,
                             float accuracy);
void loc_eng_delete_aiding_data(loc_eng_data_s_type &loc_eng_data,
                                GpsAidingData f);
int  loc_eng_set_position_mode(loc_eng_data_s_type &loc_eng_data,
                               LocPositionMode mode, GpsPositionRecurrence recurrence,
                               uint32_t min_interval, uint32_t preferred_accuracy,
                               uint32_t preferred_time);
const void* loc_eng_get_extension(loc_eng_data_s_type &loc_eng_data,
                                  const char* name);
#ifdef QCOM_FEATURE_ULP
int  loc_eng_update_criteria(loc_eng_data_s_type &loc_eng_data,
                             UlpLocationCriteria criteria);
#endif

void loc_eng_agps_init(loc_eng_data_s_type &loc_eng_data,
                       AGpsCallbacks* callbacks);
#ifdef QCOM_FEATURE_IPV6
int  loc_eng_agps_open(loc_eng_data_s_type &loc_eng_data, AGpsType agpsType,
                      const char* apn, AGpsBearerType bearerType);
int  loc_eng_agps_closed(loc_eng_data_s_type &loc_eng_data, AGpsType agpsType);
int  loc_eng_agps_open_failed(loc_eng_data_s_type &loc_eng_data, AGpsType agpsType);
#else
int  loc_eng_agps_open(loc_eng_data_s_type &loc_eng_data, const char* apn);
int  loc_eng_agps_closed(loc_eng_data_s_type &loc_eng_data);
int  loc_eng_agps_open_failed(loc_eng_data_s_type &loc_eng_data);
#endif

int  loc_eng_set_server_proxy(loc_eng_data_s_type &loc_eng_data,
                              LocServerType type, const char *hostname, int port);


void loc_eng_agps_ril_update_network_availability(loc_eng_data_s_type &loc_eng_data,
                                                  int avaiable, const char* apn);


bool loc_eng_inject_raw_command(loc_eng_data_s_type &loc_eng_data,
                                char* command, int length);


void loc_eng_mute_one_session(loc_eng_data_s_type &loc_eng_data);

int loc_eng_xtra_init (loc_eng_data_s_type &loc_eng_data,
                       GpsXtraCallbacks* callbacks);

int loc_eng_xtra_inject_data(loc_eng_data_s_type &loc_eng_data,
                             char* data, int length);

extern void loc_eng_ni_init(loc_eng_data_s_type &loc_eng_data,
                            GpsNiCallbacks *callbacks);
extern void loc_eng_ni_respond(loc_eng_data_s_type &loc_eng_data,
                               int notif_id, GpsUserResponseType user_response);
extern void loc_eng_ni_request_handler(loc_eng_data_s_type &loc_eng_data,
                                   const GpsNiNotification *notif,
                                   const void* passThrough);
extern void loc_eng_ni_reset_on_engine_restart(loc_eng_data_s_type &loc_eng_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // LOC_ENG_H
