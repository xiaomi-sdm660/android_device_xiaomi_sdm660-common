/******************************************************************************
  @file:  loc_eng.cpp
  @brief:

  DESCRIPTION
    This file defines the implemenation for GPS hardware abstraction layer.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
Copyright (c) 2009, QUALCOMM USA, INC.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

·         Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 

·         Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 

·         Neither the name of the QUALCOMM USA, INC.  nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  -----------------------------------------------------------------------------

******************************************************************************/

/*=====================================================================
$Header: $
$DateTime: $
$Author: $
======================================================================*/

#define LOG_NDDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <rpc/rpc.h>
#include "loc_api_rpc_glue.h"
#include "loc_apicb_appinit.h"

#include <cutils/properties.h>
#include <cutils/sched_policy.h>
#include <utils/SystemClock.h>

#include <loc_eng.h>
#include <loc_eng_ni.h>

#define LOG_TAG "lib_locapi"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

#define DEBUG_MOCK_NI 0

// Function declarations for sLocEngInterface
static int  loc_eng_init(GpsCallbacks* callbacks);
static int  loc_eng_start();
static int  loc_eng_stop();
static int  loc_eng_set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time);
static void loc_eng_cleanup();
static int  loc_eng_inject_time(GpsUtcTime time, int64_t timeReference, int uncertainty);
static int  loc_eng_inject_location(double latitude, double longitude, float accuracy);
static void loc_eng_delete_aiding_data (GpsAidingData f);
static const void* loc_eng_get_extension(const char* name);

// Function declarations for sLocEngAGpsInterface
static void loc_eng_agps_init(AGpsCallbacks* callbacks);
static int loc_eng_agps_data_conn_open(const char* apn);
static int loc_eng_agps_data_conn_closed();
static int loc_eng_agps_data_conn_failed();
static int loc_eng_agps_set_server(AGpsType type, const char* hostname, int port);


static int32 loc_event_cb (rpc_loc_client_handle_type client_handle, 
                           rpc_loc_event_mask_type loc_event, 
                           const rpc_loc_event_payload_u_type* loc_event_payload);
static void loc_eng_report_position (const rpc_loc_parsed_position_s_type *location_report_ptr);
static void loc_eng_report_sv (const rpc_loc_gnss_info_s_type *gnss_report_ptr);
static void loc_eng_report_status (const rpc_loc_status_event_s_type *status_report_ptr);
static void loc_eng_report_nmea (const rpc_loc_nmea_report_s_type *nmea_report_ptr);
static void loc_eng_process_conn_request (const rpc_loc_server_request_s_type *server_request_ptr);

static void loc_eng_process_deferred_action (void* arg);
static void loc_eng_process_atl_deferred_action (int flags);
static void loc_eng_delete_aiding_data_deferred_action (void);

static int set_agps_server();

// Defines the GpsInterface in gps.h
static const GpsInterface sLocEngInterface =
{
    sizeof(GpsInterface),
    loc_eng_init,
    loc_eng_start,
    loc_eng_stop,
    loc_eng_cleanup,
    loc_eng_inject_time,
    loc_eng_inject_location,
    loc_eng_delete_aiding_data,
    loc_eng_set_position_mode,
    loc_eng_get_extension,
};

static const AGpsInterface sLocEngAGpsInterface =
{
    sizeof(AGpsInterface),
    loc_eng_agps_init,
    loc_eng_agps_data_conn_open,
    loc_eng_agps_data_conn_closed,
    loc_eng_agps_data_conn_failed,
    loc_eng_agps_set_server,
};

// Global data structure for location engine
loc_eng_data_s_type loc_eng_data;

/*===========================================================================
FUNCTION    gps_get_hardware_interface

DESCRIPTION
   Returns the GPS hardware interaface based on LOC API
   if GPS is enabled.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
const GpsInterface* gps_get_hardware_interface ()
{
    char propBuf[PROPERTY_VALUE_MAX];

    // check to see if GPS should be disabled
    property_get("gps.disable", propBuf, "");
    if (propBuf[0] == '1')
    {
        LOGD("gps_get_interface returning NULL because gps.disable=1\n");
        return NULL;
    }

    return &sLocEngInterface;
}

/*===========================================================================
FUNCTION    loc_eng_init

DESCRIPTION
   Initialize the location engine, this include setting up global datas
   and registers location engien with loc api service.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/

// fully shutting down the GPS is temporarily disabled to avoid intermittent BP crash
#define DISABLE_CLEANUP 1

static int loc_eng_init(GpsCallbacks* callbacks)
{
#if DISABLE_CLEANUP
    if (loc_eng_data.deferred_action_thread) {
        // already initialized
        return 0;
    }
#endif
    // Start the LOC api RPC service
    loc_api_glue_init ();

    callbacks->set_capabilities_cb(GPS_CAPABILITY_SCHEDULING | GPS_CAPABILITY_MSA | GPS_CAPABILITY_MSB);

    memset (&loc_eng_data, 0, sizeof (loc_eng_data_s_type));

    // LOC ENG module data initialization
    loc_eng_data.location_cb    = callbacks->location_cb;
    loc_eng_data.sv_status_cb   = callbacks->sv_status_cb;
    loc_eng_data.status_cb      = callbacks->status_cb;
    loc_eng_data.nmea_cb        = callbacks->nmea_cb;
    loc_eng_data.acquire_wakelock_cb = callbacks->acquire_wakelock_cb;
    loc_eng_data.release_wakelock_cb = callbacks->release_wakelock_cb;

    rpc_loc_event_mask_type event = RPC_LOC_EVENT_PARSED_POSITION_REPORT |
                                    RPC_LOC_EVENT_SATELLITE_REPORT |
                                    RPC_LOC_EVENT_LOCATION_SERVER_REQUEST |
                                    RPC_LOC_EVENT_ASSISTANCE_DATA_REQUEST |
                                    RPC_LOC_EVENT_IOCTL_REPORT |
                                    RPC_LOC_EVENT_STATUS_REPORT |
                                    RPC_LOC_EVENT_NMEA_POSITION_REPORT |
                                    RPC_LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;

    loc_eng_data.client_handle = loc_open (event, loc_event_cb);

    pthread_mutex_init (&(loc_eng_data.deferred_action_mutex), NULL);
    pthread_cond_init  (&(loc_eng_data.deferred_action_cond) , NULL);
     pthread_mutex_init (&(loc_eng_data.deferred_stop_mutex), NULL);

    loc_eng_data.loc_event = 0;
    loc_eng_data.deferred_action_flags = 0;
    memset (loc_eng_data.apn_name, 0, sizeof (loc_eng_data.apn_name));

    loc_eng_data.aiding_data_for_deletion = 0;
    loc_eng_data.engine_status = GPS_STATUS_NONE;

    // XTRA module data initialization
    loc_eng_data.xtra_module_data.download_request_cb = NULL;

    // IOCTL module data initialization
    loc_eng_data.ioctl_data.cb_is_selected  = FALSE;
    loc_eng_data.ioctl_data.cb_is_waiting   = FALSE;
    loc_eng_data.ioctl_data.client_handle   = RPC_LOC_CLIENT_HANDLE_INVALID;
    memset (&(loc_eng_data.ioctl_data.cb_payload),
            0,
            sizeof (rpc_loc_ioctl_callback_s_type));

    pthread_mutex_init (&(loc_eng_data.ioctl_data.cb_data_mutex), NULL);
    pthread_cond_init(&loc_eng_data.ioctl_data.cb_arrived_cond, NULL);

    loc_eng_data.deferred_action_thread = callbacks->create_thread_cb("loc_api",
                                            loc_eng_process_deferred_action, NULL);

    LOGD ("loc_eng_init called, client id = %d\n", (int32) loc_eng_data.client_handle);
    return 0;
}

/*===========================================================================
FUNCTION    loc_eng_cleanup

DESCRIPTION
   Cleans location engine. The location client handle will be released.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_cleanup()
{
#if DISABLE_CLEANUP
    return;
#else
    if (loc_eng_data.deferred_action_thread)
    {
        /* Terminate deferred action working thread */
        pthread_mutex_lock(&loc_eng_data.deferred_action_mutex);
        /* hold a wake lock while events are pending for deferred_action_thread */
        loc_eng_data.acquire_wakelock_cb();
        loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_QUIT;
        pthread_cond_signal(&loc_eng_data.deferred_action_cond);
        pthread_mutex_unlock(&loc_eng_data.deferred_action_mutex);

        void* ignoredValue;
        pthread_join(loc_eng_data.deferred_action_thread, &ignoredValue);
        loc_eng_data.deferred_action_thread = NULL;
    }

    // clean up
    (void) loc_close (loc_eng_data.client_handle);

    pthread_mutex_destroy (&loc_eng_data.deferred_action_mutex);
    pthread_cond_destroy  (&loc_eng_data.deferred_action_cond);

    pthread_mutex_destroy (&loc_eng_data.ioctl_data.cb_data_mutex);
    pthread_cond_destroy  (&loc_eng_data.ioctl_data.cb_arrived_cond);

// Do not call this as it can result in the ARM9 crashing if it sends events while we are disabled
//    loc_apicb_app_deinit();
#endif
}


/*===========================================================================
FUNCTION    loc_eng_start

DESCRIPTION
   Starts the tracking session

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_eng_start()
{
    int ret_val;
    LOGD ("loc_eng_start\n");

    if (loc_eng_data.position_mode != GPS_POSITION_MODE_STANDALONE &&
            loc_eng_data.agps_server_host[0] != 0 &&
            loc_eng_data.agps_server_port != 0) {
        int result = set_agps_server();
        LOGD ("set_agps_server returned = %d\n", result);
    }

    ret_val = loc_start_fix (loc_eng_data.client_handle);

    if (ret_val != RPC_LOC_API_SUCCESS)
    {
        LOGD ("loc_eng_start returned error = %d\n", ret_val);
    }

    return 0;
}


/*===========================================================================
FUNCTION    loc_eng_stop

DESCRIPTION
   Stops the tracking session

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_eng_stop()
{
    int ret_val;

    LOGD ("loc_eng_stop\n");

    pthread_mutex_lock(&(loc_eng_data.deferred_stop_mutex));
    // work around problem with loc_eng_stop when AGPS requests are pending
    // we defer stopping the engine until the AGPS request is done
    if (loc_eng_data.agps_request_pending)
    {
        loc_eng_data.stop_request_pending = true;
        LOGD ("deferring stop until AGPS data call is finished\n");
        pthread_mutex_unlock(&(loc_eng_data.deferred_stop_mutex));
        return 0;
    }
    pthread_mutex_unlock(&(loc_eng_data.deferred_stop_mutex));

    ret_val = loc_stop_fix (loc_eng_data.client_handle);
    if (ret_val != RPC_LOC_API_SUCCESS)
    {
        LOGD ("loc_eng_stop returned error = %d\n", ret_val);
    }

    return 0;
}

static int loc_eng_set_gps_lock(rpc_loc_lock_e_type lock_type)
{
    rpc_loc_ioctl_data_u_type    ioctl_data;
    boolean                      ret_val;

    LOGD ("loc_eng_set_gps_lock mode, client = %d, lock_type = %d\n",
            (int32) loc_eng_data.client_handle, lock_type);

    ioctl_data.rpc_loc_ioctl_data_u_type_u.engine_lock = lock_type;
    ioctl_data.disc = RPC_LOC_IOCTL_SET_ENGINE_LOCK;

    ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                            RPC_LOC_IOCTL_SET_ENGINE_LOCK,
                            &ioctl_data,
                            LOC_IOCTL_DEFAULT_TIMEOUT,
                            NULL /* No output information is expected*/);

    if (ret_val != TRUE)
    {
        LOGD ("loc_eng_set_gps_lock mode failed\n");
    }

    return 0;
}

/*===========================================================================
FUNCTION    loc_eng_set_position_mode

DESCRIPTION
   Sets the mode and fix frequency for the tracking session.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_eng_set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time)
{
    rpc_loc_ioctl_data_u_type    ioctl_data;
    rpc_loc_fix_criteria_s_type *fix_criteria_ptr;
    boolean                      ret_val;

    LOGD ("loc_eng_set_position mode, client = %d, interval = %d, mode = %d\n",
            (int32) loc_eng_data.client_handle, min_interval, mode);

    loc_eng_data.position_mode = mode;
    ioctl_data.disc = RPC_LOC_IOCTL_SET_FIX_CRITERIA;

    fix_criteria_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.fix_criteria);
    fix_criteria_ptr->valid_mask = RPC_LOC_FIX_CRIT_VALID_PREFERRED_OPERATION_MODE |
                                   RPC_LOC_FIX_CRIT_VALID_RECURRENCE_TYPE;

    switch (mode) {
        case GPS_POSITION_MODE_MS_BASED:
            fix_criteria_ptr->preferred_operation_mode = RPC_LOC_OPER_MODE_MSB;
            break;
        case GPS_POSITION_MODE_MS_ASSISTED:
            fix_criteria_ptr->preferred_operation_mode = RPC_LOC_OPER_MODE_MSA;
            break;
        case GPS_POSITION_MODE_STANDALONE:
        default:
            fix_criteria_ptr->preferred_operation_mode = RPC_LOC_OPER_MODE_STANDALONE;
            break;
    }
    if (min_interval > 0) {
        fix_criteria_ptr->min_interval = min_interval;
        fix_criteria_ptr->valid_mask |= RPC_LOC_FIX_CRIT_VALID_MIN_INTERVAL;
    }
    if (preferred_accuracy > 0) {
        fix_criteria_ptr->preferred_accuracy = preferred_accuracy;
        fix_criteria_ptr->valid_mask |= RPC_LOC_FIX_CRIT_VALID_PREFERRED_ACCURACY;
    }
    if (preferred_time > 0) {
        fix_criteria_ptr->preferred_response_time = preferred_time;
        fix_criteria_ptr->valid_mask |= RPC_LOC_FIX_CRIT_VALID_PREFERRED_RESPONSE_TIME;
    }

    switch (recurrence) {
        case GPS_POSITION_RECURRENCE_SINGLE:
            fix_criteria_ptr->recurrence_type = RPC_LOC_SINGLE_FIX;
            break;
        case GPS_POSITION_RECURRENCE_PERIODIC:
        default:
            fix_criteria_ptr->recurrence_type = RPC_LOC_PERIODIC_FIX;
            break;
    }

    ret_val = loc_eng_ioctl(loc_eng_data.client_handle,
                            RPC_LOC_IOCTL_SET_FIX_CRITERIA,
                            &ioctl_data,
                            LOC_IOCTL_DEFAULT_TIMEOUT,
                            NULL /* No output information is expected*/);

    if (ret_val != TRUE)
    {
        LOGD ("loc_eng_set_position mode failed\n");
    }

    return 0;
}

/*===========================================================================
FUNCTION    loc_eng_inject_time

DESCRIPTION
   This is used by Java native function to do time injection.

DEPENDENCIES
   None

RETURN VALUE
   RPC_LOC_API_SUCCESS

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_eng_inject_time (GpsUtcTime time, int64_t timeReference, int uncertainty)
{
    rpc_loc_ioctl_data_u_type       ioctl_data;
    rpc_loc_assist_data_time_s_type *time_info_ptr;
    boolean                          ret_val;

    LOGD ("loc_eng_inject_time, uncertainty = %d\n", uncertainty);

    ioctl_data.disc = RPC_LOC_IOCTL_INJECT_UTC_TIME;

    time_info_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.assistance_data_time);
    time_info_ptr->time_utc = time;
    time_info_ptr->time_utc += (int64_t)(android::elapsedRealtime() - timeReference);
    time_info_ptr->uncertainty = uncertainty; // Uncertainty in ms

    ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                             RPC_LOC_IOCTL_INJECT_UTC_TIME,
                             &ioctl_data,
                             LOC_IOCTL_DEFAULT_TIMEOUT,
                             NULL /* No output information is expected*/);

    if (ret_val != TRUE)
    {
        LOGD ("loc_eng_inject_time failed\n");
    }

    return 0;
}

static int loc_eng_inject_location (double latitude, double longitude, float accuracy)
{
    /* not yet implemented */
    return 0;
}

/*===========================================================================
FUNCTION    loc_eng_delete_aiding_data

DESCRIPTION
   This is used by Java native function to delete the aiding data. The function
   updates the global variable for the aiding data to be deleted. If the GPS
   engine is off, the aiding data will be deleted. Otherwise, the actual action
   will happen when gps engine is turned off.

DEPENDENCIES
   Assumes the aiding data type specified in GpsAidingData matches with
   LOC API specification.

RETURN VALUE
   RPC_LOC_API_SUCCESS

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_delete_aiding_data (GpsAidingData f)
{
    pthread_mutex_lock(&(loc_eng_data.deferred_action_mutex));

    // Currently, LOC API only support deletion of all aiding data,
    if (f)
        loc_eng_data.aiding_data_for_deletion = GPS_DELETE_ALL;

    if ((loc_eng_data.engine_status != GPS_STATUS_SESSION_BEGIN) &&
        (loc_eng_data.aiding_data_for_deletion != 0))
    {
        /* hold a wake lock while events are pending for deferred_action_thread */
        loc_eng_data.acquire_wakelock_cb();
        loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_DELETE_AIDING;
        pthread_cond_signal(&(loc_eng_data.deferred_action_cond));

        // In case gps engine is ON, the assistance data will be deleted when the engine is OFF
    }

    pthread_mutex_unlock(&(loc_eng_data.deferred_action_mutex));
}

/*===========================================================================
FUNCTION    loc_eng_get_extension

DESCRIPTION
   Get the gps extension to support XTRA.

DEPENDENCIES
   N/A

RETURN VALUE
   The GPS extension interface.

SIDE EFFECTS
   N/A

===========================================================================*/
static const void* loc_eng_get_extension(const char* name)
{
    if (strcmp(name, GPS_XTRA_INTERFACE) == 0)
    {
        return &sLocEngXTRAInterface;
    }
    else if (strcmp(name, AGPS_INTERFACE) == 0)
    {
        return &sLocEngAGpsInterface;
    }
    else if (strcmp(name, GPS_NI_INTERFACE) == 0)
    {
        return &sLocEngNiInterface;
    }

    return NULL;
}

#if DEBUG_MOCK_NI == 1
/*===========================================================================
FUNCTION    mock_ni

DESCRIPTION
   DEBUG tool: simulate an NI request

DEPENDENCIES
   N/A

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void* mock_ni(void* arg)
{
    static int busy = 0;

    if (busy) return NULL;

    busy = 1;

    sleep(5);

    rpc_loc_client_handle_type           client_handle;
    rpc_loc_event_mask_type              loc_event;
    rpc_loc_event_payload_u_type         payload;
    rpc_loc_ni_event_s_type             *ni_req;
    rpc_loc_ni_supl_notify_verify_req_s_type *supl_req;

    client_handle = (rpc_loc_client_handle_type) arg;

    loc_event = RPC_LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST;
    payload.disc = loc_event;

    ni_req = &payload.rpc_loc_event_payload_u_type_u.ni_request;
    ni_req->event = RPC_LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ;
    supl_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req;

    // Encodings for Spirent Communications
    char client_name[80]  = {0x53,0x78,0x5A,0x5E,0x76,0xD3,0x41,0xC3,0x77,
            0xBB,0x5D,0x77,0xA7,0xC7,0x61,0x7A,0xFA,0xED,0x9E,0x03};
    char requestor_id[80] = {0x53,0x78,0x5A,0x5E,0x76,0xD3,0x41,0xC3,0x77,
            0xBB,0x5D,0x77,0xA7,0xC7,0x61,0x7A,0xFA,0xED,0x9E,0x03};

    supl_req->flags = RPC_LOC_NI_CLIENT_NAME_PRESENT |
                      RPC_LOC_NI_REQUESTOR_ID_PRESENT |
                      RPC_LOC_NI_ENCODING_TYPE_PRESENT;

    supl_req->datacoding_scheme = RPC_LOC_NI_SUPL_GSM_DEFAULT;

    supl_req->client_name.data_coding_scheme = RPC_LOC_NI_SUPL_GSM_DEFAULT; // no coding
    supl_req->client_name.client_name_string.client_name_string_len = strlen(client_name);
    supl_req->client_name.client_name_string.client_name_string_val = client_name;
    supl_req->client_name.string_len = strlen(client_name);

    supl_req->requestor_id.data_coding_scheme = RPC_LOC_NI_SUPL_GSM_DEFAULT;
    supl_req->requestor_id.requestor_id_string.requestor_id_string_len = strlen(requestor_id);
    supl_req->requestor_id.requestor_id_string.requestor_id_string_val = requestor_id;
    supl_req->requestor_id.string_len = strlen(requestor_id);

    supl_req->notification_priv_type = RPC_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP;
    supl_req->user_response_timer = 10;

    loc_event_cb(client_handle, loc_event, &payload);

    busy = 0;

    return NULL;
}
#endif // DEBUG_MOCK_NI

/*===========================================================================
FUNCTION    loc_event_cb

DESCRIPTION
   This is the callback function registered by loc_open.

DEPENDENCIES
   N/A

RETURN VALUE
   RPC_LOC_API_SUCCESS

SIDE EFFECTS
   N/A

===========================================================================*/
static int32 loc_event_cb(
    rpc_loc_client_handle_type           client_handle,
    rpc_loc_event_mask_type              loc_event,
    const rpc_loc_event_payload_u_type*  loc_event_payload
    )
{
    LOGV ("loc_event_cb, client = %d, loc_event = 0x%x", (int32) client_handle, (uint32) loc_event);
    if (client_handle == loc_eng_data.client_handle)
    {
        pthread_mutex_lock(&loc_eng_data.deferred_action_mutex);
        loc_eng_data.loc_event = loc_event;
        memcpy(&loc_eng_data.loc_event_payload, loc_event_payload, sizeof(*loc_event_payload));

        /* hold a wake lock while events are pending for deferred_action_thread */
        loc_eng_data.acquire_wakelock_cb();
        loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_EVENT;
        pthread_cond_signal(&loc_eng_data.deferred_action_cond);
        pthread_mutex_unlock(&loc_eng_data.deferred_action_mutex);
    }
    else
    {
        LOGD ("loc client mismatch: received = %d, expected = %d \n", (int32) client_handle, (int32) loc_eng_data.client_handle);
    }

    return RPC_LOC_API_SUCCESS;
}

/*===========================================================================
FUNCTION    loc_eng_report_position

DESCRIPTION
   Reports position information to the Java layer.

DEPENDENCIES
   N/A

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_report_position (const rpc_loc_parsed_position_s_type *location_report_ptr)
{
    GpsLocation location;

    LOGV ("loc_eng_report_position: location report, valid mask = 0x%x, sess status = %d\n",
         (uint32) location_report_ptr->valid_mask, location_report_ptr->session_status);

    memset (&location, 0, sizeof(location));
    location.size = sizeof(location);
    if (location_report_ptr->valid_mask & RPC_LOC_POS_VALID_SESSION_STATUS)
    {
        // Not a position report, return
        if (location_report_ptr->session_status == RPC_LOC_SESS_STATUS_SUCCESS)
        {
            if (location_report_ptr->valid_mask & RPC_LOC_POS_VALID_TIMESTAMP_UTC)
            {
                location.timestamp = location_report_ptr->timestamp_utc;
            }

            if ((location_report_ptr->valid_mask & RPC_LOC_POS_VALID_LATITUDE) &&
                (location_report_ptr->valid_mask & RPC_LOC_POS_VALID_LONGITUDE))
            {
                location.flags    |= GPS_LOCATION_HAS_LAT_LONG;
                location.latitude  = location_report_ptr->latitude;
                location.longitude = location_report_ptr->longitude;
            }

            if (location_report_ptr->valid_mask &  RPC_LOC_POS_VALID_ALTITUDE_WRT_ELLIPSOID )
            {
                location.flags    |= GPS_LOCATION_HAS_ALTITUDE;
                location.altitude = location_report_ptr->altitude_wrt_ellipsoid;
            }

            if ((location_report_ptr->valid_mask & RPC_LOC_POS_VALID_SPEED_HORIZONTAL) &&
                (location_report_ptr->valid_mask & RPC_LOC_POS_VALID_SPEED_VERTICAL))
            {
                location.flags    |= GPS_LOCATION_HAS_SPEED;
                location.speed = sqrt(location_report_ptr->speed_horizontal * location_report_ptr->speed_horizontal +
                                     location_report_ptr->speed_vertical * location_report_ptr->speed_vertical);
            }

            if (location_report_ptr->valid_mask &  RPC_LOC_POS_VALID_HEADING)
            {
                location.flags    |= GPS_LOCATION_HAS_BEARING;
                location.bearing = location_report_ptr->heading;
            }

            if (location_report_ptr->valid_mask & RPC_LOC_POS_VALID_HOR_UNC_CIRCULAR)
            {
                location.flags    |= GPS_LOCATION_HAS_ACCURACY;
                location.accuracy = location_report_ptr->hor_unc_circular;
            }

            if (loc_eng_data.location_cb != NULL)
            {
                LOGV ("loc_eng_report_position: fire callback\n");
                loc_eng_data.location_cb (&location);
            }
        }
        else
        {
            LOGV ("loc_eng_report_position: ignore position report when session status = %d\n", location_report_ptr->session_status);
        }
    }
    else
    {
        LOGV ("loc_eng_report_position: ignore position report when session status is not set\n");
    }
}

/*===========================================================================
FUNCTION    loc_eng_report_sv

DESCRIPTION
   Reports GPS satellite information to the Java layer.

DEPENDENCIES
   N/A

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_report_sv (const rpc_loc_gnss_info_s_type *gnss_report_ptr)
{
    GpsSvStatus     SvStatus;
    int             num_svs_max, i;
	const rpc_loc_sv_info_s_type *sv_info_ptr;

    LOGV ("loc_eng_report_sv: valid_mask = 0x%x, num of sv = %d\n",
            (uint32) gnss_report_ptr->valid_mask,
            gnss_report_ptr->sv_count);

    num_svs_max = 0;
    memset (&SvStatus, 0, sizeof (GpsSvStatus));
    if (gnss_report_ptr->valid_mask & RPC_LOC_GNSS_INFO_VALID_SV_COUNT)
    {
        num_svs_max = gnss_report_ptr->sv_count;
        if (num_svs_max > GPS_MAX_SVS)
        {
            num_svs_max = GPS_MAX_SVS;
        }
    }

    if (gnss_report_ptr->valid_mask & RPC_LOC_GNSS_INFO_VALID_SV_LIST)
    {
        SvStatus.num_svs = 0;

        for (i = 0; i < num_svs_max; i++)
        {
            sv_info_ptr = &(gnss_report_ptr->sv_list.sv_list_val[i]);
            if (sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_SYSTEM)
            {
                if (sv_info_ptr->system == RPC_LOC_SV_SYSTEM_GPS)
                {
                    SvStatus.sv_list[SvStatus.num_svs].size = sizeof(GpsSvStatus);
                    SvStatus.sv_list[SvStatus.num_svs].prn = sv_info_ptr->prn;

                    // We only have the data field to report gps eph and alm mask
                    if ((sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_HAS_EPH) &&
                        (sv_info_ptr->has_eph == 1))
                    {
                        SvStatus.ephemeris_mask |= (1 << (sv_info_ptr->prn-1));
                    }

                    if ((sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_HAS_ALM) &&
                        (sv_info_ptr->has_alm == 1))
                    {
                        SvStatus.almanac_mask |= (1 << (sv_info_ptr->prn-1));
                    }

                    if ((sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_PROCESS_STATUS) &&
                        (sv_info_ptr->process_status == RPC_LOC_SV_STATUS_TRACK))
                    {
                        SvStatus.used_in_fix_mask |= (1 << (sv_info_ptr->prn-1));
                    }
                }
                // SBAS: GPS RPN: 120-151,
                // In exteneded measurement report, we follow nmea standard, which is from 33-64.
                else if (sv_info_ptr->system == RPC_LOC_SV_SYSTEM_SBAS)
                {
                    SvStatus.sv_list[SvStatus.num_svs].prn = sv_info_ptr->prn + 33 - 120;
                }
                // Gloness: Slot id: 1-32
                // In extended measurement report, we follow nmea standard, which is 65-96
                else if (sv_info_ptr->system == RPC_LOC_SV_SYSTEM_GLONASS)
                {
                    SvStatus.sv_list[SvStatus.num_svs].prn = sv_info_ptr->prn + (65-1);
                }
                // Unsupported SV system
                else
                {
                    continue;
                }
            }

            if (sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_SNR)
            {
                SvStatus.sv_list[SvStatus.num_svs].snr = sv_info_ptr->snr;
            }

            if (sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_ELEVATION)
            {
                SvStatus.sv_list[SvStatus.num_svs].elevation = sv_info_ptr->elevation;
            }

            if (sv_info_ptr->valid_mask & RPC_LOC_SV_INFO_VALID_AZIMUTH)
            {
                SvStatus.sv_list[SvStatus.num_svs].azimuth = sv_info_ptr->azimuth;
            }

            SvStatus.num_svs++;
        }
    }

    LOGV ("num_svs = %d, eph mask = %d, alm mask = %d\n", SvStatus.num_svs, SvStatus.ephemeris_mask, SvStatus.almanac_mask );
    if ((SvStatus.num_svs != 0) && (loc_eng_data.sv_status_cb != NULL))
    {
        loc_eng_data.sv_status_cb(&SvStatus);
    }
}

/*===========================================================================
FUNCTION    loc_eng_report_status

DESCRIPTION
   Reports GPS engine state to Java layer.

DEPENDENCIES
   N/A

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_report_status (const rpc_loc_status_event_s_type *status_report_ptr)
{
    GpsStatus status;

    LOGV ("loc_eng_report_status: event = %d\n", status_report_ptr->event);

    memset (&status, 0, sizeof(status));
    status.size = sizeof(status);
    status.status = GPS_STATUS_NONE;
    if (status_report_ptr->event == RPC_LOC_STATUS_EVENT_ENGINE_STATE)
    {
        if (status_report_ptr->payload.rpc_loc_status_event_payload_u_type_u.engine_state == RPC_LOC_ENGINE_STATE_ON)
        {
            // GPS_STATUS_SESSION_BEGIN implies GPS_STATUS_ENGINE_ON
            status.status = GPS_STATUS_SESSION_BEGIN;
            loc_eng_data.status_cb(&status);
        }
        else if (status_report_ptr->payload.rpc_loc_status_event_payload_u_type_u.engine_state == RPC_LOC_ENGINE_STATE_OFF)
        {
            // GPS_STATUS_SESSION_END implies GPS_STATUS_ENGINE_OFF
            status.status = GPS_STATUS_ENGINE_OFF;
            loc_eng_data.status_cb(&status);
        }
    }

    pthread_mutex_lock(&loc_eng_data.deferred_action_mutex);
    loc_eng_data.engine_status = status.status;

    // Wake up the thread for aiding data deletion.
    if ((loc_eng_data.engine_status != GPS_STATUS_SESSION_BEGIN) &&
        (loc_eng_data.aiding_data_for_deletion != 0))
    {
        /* hold a wake lock while events are pending for deferred_action_thread */
        loc_eng_data.acquire_wakelock_cb();
        loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_DELETE_AIDING;
        pthread_cond_signal(&(loc_eng_data.deferred_action_cond));
        // In case gps engine is ON, the assistance data will be deleted when the engine is OFF
    }

    pthread_mutex_unlock(&loc_eng_data.deferred_action_mutex);
}

static void loc_eng_report_nmea (const rpc_loc_nmea_report_s_type *nmea_report_ptr)
{
    if (loc_eng_data.nmea_cb != NULL)
    {
        struct timeval tv;

        gettimeofday(&tv, (struct timezone *) NULL);
        long long now = tv.tv_sec * 1000LL + tv.tv_usec / 1000;

#if (AMSS_VERSION==3200)
        loc_eng_data.nmea_cb(now, nmea_report_ptr->nmea_sentences.nmea_sentences_val,
                nmea_report_ptr->nmea_sentences.nmea_sentences_len);
#else
        loc_eng_data.nmea_cb(now, nmea_report_ptr->nmea_sentences, nmea_report_ptr->length);
#endif
    }
}

/*===========================================================================
FUNCTION    loc_eng_process_conn_request

DESCRIPTION
   Requests data connection to be brought up/tore down with the location server.

DEPENDENCIES
   N/A

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_process_conn_request (const rpc_loc_server_request_s_type *server_request_ptr)
{
    LOGD ("loc_event_cb: get loc event location server request, event = %d\n", server_request_ptr->event);

    // Signal DeferredActionThread to send the APN name
    pthread_mutex_lock(&loc_eng_data.deferred_action_mutex);

    // This implemenation is based on the fact that modem now at any time has only one data connection for AGPS at any given time
    if (server_request_ptr->event == RPC_LOC_SERVER_REQUEST_OPEN)
    {
        loc_eng_data.conn_handle = server_request_ptr->payload.rpc_loc_server_request_u_type_u.open_req.conn_handle;
        loc_eng_data.agps_status = GPS_REQUEST_AGPS_DATA_CONN;
        loc_eng_data.agps_request_pending = true;
    }
    else
    {
        loc_eng_data.conn_handle = server_request_ptr->payload.rpc_loc_server_request_u_type_u.close_req.conn_handle;
        loc_eng_data.agps_status = GPS_RELEASE_AGPS_DATA_CONN;
        loc_eng_data.agps_request_pending = false;
    }

    /* hold a wake lock while events are pending for deferred_action_thread */
    loc_eng_data.acquire_wakelock_cb();
    loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_AGPS_STATUS;
    pthread_cond_signal(&loc_eng_data.deferred_action_cond);
    pthread_mutex_unlock(&loc_eng_data.deferred_action_mutex);
}

/*===========================================================================
FUNCTION    loc_eng_agps_init

DESCRIPTION


DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_agps_init(AGpsCallbacks* callbacks)
{
    LOGV("loc_eng_agps_init\n");
    loc_eng_data.agps_status_cb = callbacks->status_cb;
}

static int loc_eng_agps_data_conn_open(const char* apn)
{
    int apn_len;
    LOGD("loc_eng_agps_data_conn_open: %s\n", apn);

    pthread_mutex_lock(&(loc_eng_data.deferred_action_mutex));

    if (apn != NULL)
    {
        apn_len = strlen (apn);

        if (apn_len >= sizeof(loc_eng_data.apn_name))
        {
            LOGD ("loc_eng_set_apn: error, apn name exceeds maximum lenght of 100 chars\n");
            apn_len = sizeof(loc_eng_data.apn_name) - 1;
        }

        memcpy (loc_eng_data.apn_name, apn, apn_len);
        loc_eng_data.apn_name[apn_len] = '\0';
    }

    /* hold a wake lock while events are pending for deferred_action_thread */
    loc_eng_data.acquire_wakelock_cb();
    loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_AGPS_DATA_SUCCESS;
    pthread_cond_signal(&(loc_eng_data.deferred_action_cond));
    pthread_mutex_unlock(&(loc_eng_data.deferred_action_mutex));
    return 0;
}

static int loc_eng_agps_data_conn_closed()
{
    LOGD("loc_eng_agps_data_conn_closed\n");
    pthread_mutex_lock(&(loc_eng_data.deferred_action_mutex));
    /* hold a wake lock while events are pending for deferred_action_thread */
    loc_eng_data.acquire_wakelock_cb();
    loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_AGPS_DATA_CLOSED;
    pthread_cond_signal(&(loc_eng_data.deferred_action_cond));
    pthread_mutex_unlock(&(loc_eng_data.deferred_action_mutex));
    return 0;
}

static int loc_eng_agps_data_conn_failed()
{
    LOGD("loc_eng_agps_data_conn_failed\n");

    pthread_mutex_lock(&(loc_eng_data.deferred_action_mutex));
    /* hold a wake lock while events are pending for deferred_action_thread */
    loc_eng_data.acquire_wakelock_cb();
    loc_eng_data.deferred_action_flags |= DEFERRED_ACTION_AGPS_DATA_FAILED;
    pthread_cond_signal(&(loc_eng_data.deferred_action_cond));
    pthread_mutex_unlock(&(loc_eng_data.deferred_action_mutex));
    return 0;
}

static int set_agps_server()
{
    rpc_loc_ioctl_data_u_type       ioctl_data;
    rpc_loc_server_info_s_type      *server_info_ptr;
    boolean                         ret_val;
    uint16                          port_temp;
    unsigned char                   *b_ptr;

    if (loc_eng_data.agps_server_host[0] == 0 || loc_eng_data.agps_server_port == 0)
        return -1;

    if (loc_eng_data.agps_server_address == 0) {
        struct hostent* he = gethostbyname(loc_eng_data.agps_server_host);
        if (he)
            loc_eng_data.agps_server_address = *(uint32_t *)he->h_addr_list[0];
    }
    if (loc_eng_data.agps_server_address == 0)
        return -1;

    b_ptr = (unsigned char*) (&loc_eng_data.agps_server_address);


    server_info_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.server_addr);
    ioctl_data.disc = RPC_LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR;
    server_info_ptr->addr_type = RPC_LOC_SERVER_ADDR_URL;
    server_info_ptr->addr_info.disc =  RPC_LOC_SERVER_ADDR_URL;

#if (AMSS_VERSION==3200)
    char   url[24];
    memset(url, 0, sizeof(url));
    snprintf(url, sizeof(url) - 1, "%d.%d.%d.%d:%d",
            (*(b_ptr + 0)  & 0x000000ff), (*(b_ptr+1) & 0x000000ff),
            (*(b_ptr + 2)  & 0x000000ff), (*(b_ptr+3) & 0x000000ff),
            (loc_eng_data.agps_server_port & (0x0000ffff)));

    server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.addr.addr_val = url;
    server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.addr.addr_len = strlen(url);
    LOGD ("set_agps_server, addr = %s\n", server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.addr.addr_val);
#else
    char* buf = server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.addr;
    int buf_len = sizeof(server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.addr);
    memset(buf, 0, buf_len);
    snprintf(buf, buf_len - 1, "%d.%d.%d.%d:%d",
            (*(b_ptr + 0)  & 0x000000ff), (*(b_ptr+1) & 0x000000ff),
            (*(b_ptr + 2)  & 0x000000ff), (*(b_ptr+3) & 0x000000ff),
            (loc_eng_data.agps_server_port & (0x0000ffff)));

    server_info_ptr->addr_info.rpc_loc_server_addr_u_type_u.url.length = buf_len;
    LOGD ("set_agps_server, addr = %s\n", buf);
#endif

    ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                            RPC_LOC_IOCTL_SET_UMTS_SLP_SERVER_ADDR,
                            &ioctl_data,
                            LOC_IOCTL_DEFAULT_TIMEOUT,
                            NULL /* No output information is expected*/);

    if (ret_val != TRUE)
    {
        LOGD ("set_agps_server failed\n");
        return -1;
    }
    else
    {
        LOGV ("set_agps_server successful\n");
        return 0;
    }
}

static int loc_eng_agps_set_server(AGpsType type, const char* hostname, int port)
{
    LOGD ("loc_eng_set_default_agps_server, type = %d, hostname = %s, port = %d\n", type, hostname, port);

    if (type != AGPS_TYPE_SUPL)
        return -1;

    strncpy(loc_eng_data.agps_server_host, hostname, sizeof(loc_eng_data.agps_server_host) - 1);
    loc_eng_data.agps_server_port = port;
    return 0;
}

/*===========================================================================
FUNCTION    loc_eng_delete_aiding_data_deferred_action

DESCRIPTION
   This is used to remove the aiding data when GPS engine is off.

DEPENDENCIES
   Assumes the aiding data type specified in GpsAidingData matches with
   LOC API specification.

RETURN VALUE
   RPC_LOC_API_SUCCESS

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_delete_aiding_data_deferred_action (void)
{
    // Currently, we only support deletion of all aiding data,
    // since the Android defined aiding data mask matches with modem,
    // so just pass them down without any translation
    rpc_loc_ioctl_data_u_type          ioctl_data;
    rpc_loc_assist_data_delete_s_type  *assist_data_ptr;
    boolean                             ret_val;

    ioctl_data.disc = RPC_LOC_IOCTL_DELETE_ASSIST_DATA;
    assist_data_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.assist_data_delete);
    assist_data_ptr->type = loc_eng_data.aiding_data_for_deletion;
    loc_eng_data.aiding_data_for_deletion = 0;

    memset (&(assist_data_ptr->reserved), 0, sizeof (assist_data_ptr->reserved));

    ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                             RPC_LOC_IOCTL_DELETE_ASSIST_DATA ,
                             &ioctl_data,
                             LOC_IOCTL_DEFAULT_TIMEOUT,
                             NULL);

    LOGD("loc_eng_ioctl for aiding data deletion returned %d, 1 for success\n", ret_val);
}

/*===========================================================================
FUNCTION    loc_eng_process_atl_deferred_action

DESCRIPTION
   This is used to inform the location engine of the processing status for
   data connection open/close request.

DEPENDENCIES
   None

RETURN VALUE
   RPC_LOC_API_SUCCESS

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_process_atl_deferred_action (int flags)
{
    rpc_loc_server_open_status_s_type  *conn_open_status_ptr;
    rpc_loc_server_close_status_s_type *conn_close_status_ptr;
    rpc_loc_ioctl_data_u_type           ioctl_data;
    boolean                             ret_val;
    int                                 agps_status = -1;

    LOGV("loc_eng_process_atl_deferred_action, agps_status = %d\n", loc_eng_data.agps_status);

    memset (&ioctl_data, 0, sizeof (rpc_loc_ioctl_data_u_type));
 
    if (flags & DEFERRED_ACTION_AGPS_DATA_CLOSED)
    {
        ioctl_data.disc = RPC_LOC_IOCTL_INFORM_SERVER_CLOSE_STATUS;
        conn_close_status_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.conn_close_status);
        conn_close_status_ptr->conn_handle = loc_eng_data.conn_handle;
        conn_close_status_ptr->close_status = RPC_LOC_SERVER_CLOSE_SUCCESS;
    }
    else
    {
        ioctl_data.disc = RPC_LOC_IOCTL_INFORM_SERVER_OPEN_STATUS;
        conn_open_status_ptr = &ioctl_data.rpc_loc_ioctl_data_u_type_u.conn_open_status;
        conn_open_status_ptr->conn_handle = loc_eng_data.conn_handle;
        if (flags & DEFERRED_ACTION_AGPS_DATA_SUCCESS)
        {
            conn_open_status_ptr->open_status = RPC_LOC_SERVER_OPEN_SUCCESS;
            // Both buffer are of the same maximum size, and the source is null terminated
            // strcpy (&(ioctl_data.rpc_loc_ioctl_data_u_type_u.conn_open_status.apn_name), &(loc_eng_data.apn_name));
#if (AMSS_VERSION==3200)
            conn_open_status_ptr->apn_name = loc_eng_data.apn_name;
#else
            memset(conn_open_status_ptr->apn_name, 0, sizeof(conn_open_status_ptr->apn_name));
            strncpy(conn_open_status_ptr->apn_name, loc_eng_data.apn_name,
                    sizeof(conn_open_status_ptr->apn_name) - 1);
#endif
            // Delay this so that PDSM ATL module will behave properly
            sleep (1);
            LOGD("loc_eng_ioctl for ATL with apn_name = %s\n", conn_open_status_ptr->apn_name);
        }
        else // data_connection_failed
        {
            conn_open_status_ptr->open_status = RPC_LOC_SERVER_OPEN_FAIL;
        }
        // Delay this so that PDSM ATL module will behave properly
        sleep (1);
    }

    ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                             ioctl_data.disc,
                             &ioctl_data,
                             LOC_IOCTL_DEFAULT_TIMEOUT,
                             NULL);

    LOGD("loc_eng_ioctl for ATL returned %d (1 for success)\n", ret_val);
}

/*===========================================================================
FUNCTION    loc_eng_process_loc_event

DESCRIPTION
   This is used to process events received from the location engine.

DEPENDENCIES
   None

RETURN VALUE
   N/A

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_process_loc_event (rpc_loc_event_mask_type loc_event,
        rpc_loc_event_payload_u_type* loc_event_payload)
{
    if (loc_event & RPC_LOC_EVENT_PARSED_POSITION_REPORT)
    {
        loc_eng_report_position (&(loc_event_payload->rpc_loc_event_payload_u_type_u.parsed_location_report));
    }

    if (loc_event & RPC_LOC_EVENT_SATELLITE_REPORT)
    {
        loc_eng_report_sv (&(loc_event_payload->rpc_loc_event_payload_u_type_u.gnss_report));
    }

    if (loc_event & RPC_LOC_EVENT_STATUS_REPORT)
    {
        loc_eng_report_status (&(loc_event_payload->rpc_loc_event_payload_u_type_u.status_report));
    }

    if (loc_event & RPC_LOC_EVENT_NMEA_POSITION_REPORT)
    {
        loc_eng_report_nmea (&(loc_event_payload->rpc_loc_event_payload_u_type_u.nmea_report));
    }

    // Android XTRA interface supports only XTRA download
    if (loc_event & RPC_LOC_EVENT_ASSISTANCE_DATA_REQUEST)
    {
        if (loc_event_payload->rpc_loc_event_payload_u_type_u.assist_data_request.event ==
                RPC_LOC_ASSIST_DATA_PREDICTED_ORBITS_REQ)
        {
            LOGD ("loc_event_cb: xtra download requst");

            // Call Registered callback
            if (loc_eng_data.xtra_module_data.download_request_cb != NULL)
            {
                loc_eng_data.xtra_module_data.download_request_cb ();
            }
        }
    }

    if (loc_event & RPC_LOC_EVENT_IOCTL_REPORT)
    {
        // Process the received RPC_LOC_EVENT_IOCTL_REPORT
        (void) loc_eng_ioctl_process_cb (loc_eng_data.client_handle,
                                &(loc_event_payload->rpc_loc_event_payload_u_type_u.ioctl_report));
    }

    if (loc_event & RPC_LOC_EVENT_LOCATION_SERVER_REQUEST)
    {
        loc_eng_process_conn_request (&(loc_event_payload->rpc_loc_event_payload_u_type_u.loc_server_request));
    }

    loc_eng_ni_callback(loc_event, loc_event_payload);

#if DEBUG_MOCK_NI == 1
    // DEBUG only
    if ((loc_event & RPC_LOC_EVENT_STATUS_REPORT) &&
        loc_event_payload->rpc_loc_event_payload_u_type_u.status_report.
        payload.rpc_loc_status_event_payload_u_type_u.engine_state
        == RPC_LOC_ENGINE_STATE_OFF)
    {
        // Mock an NI request
        pthread_t th;
        pthread_create (&th, NULL, mock_ni, (void*) client_handle);
    }
#endif /* DEBUG_MOCK_NI == 1 */
}

/*===========================================================================
FUNCTION loc_eng_process_deferred_action

DESCRIPTION
   Main routine for the thread to execute certain commands
   that are not safe to be done from within an RPC callback.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_eng_process_deferred_action (void* arg)
{
    AGpsStatus      status;
    status.size = sizeof(status);
    status.type = AGPS_TYPE_SUPL;

    LOGD("loc_eng_process_deferred_action started\n");

    // make sure we do not run in background scheduling group
    set_sched_policy(gettid(), SP_FOREGROUND);

    // disable the GPS lock
    LOGD("Setting GPS privacy lock to RPC_LOC_LOCK_NONE\n");
    loc_eng_set_gps_lock(RPC_LOC_LOCK_NONE);

    while (1)
    {
        GpsAidingData   aiding_data_for_deletion;
        GpsStatusValue  engine_status;

        rpc_loc_event_mask_type         loc_event;
        rpc_loc_event_payload_u_type    loc_event_payload;

        // Wait until we are signalled to do a deferred action, or exit
        pthread_mutex_lock(&loc_eng_data.deferred_action_mutex);

        // If we have an event we should process it immediately,
        // otherwise wait until we are signalled
        if (loc_eng_data.deferred_action_flags == 0) {
            // do not hold a wake lock while waiting for an event...
            loc_eng_data.release_wakelock_cb();
            pthread_cond_wait(&loc_eng_data.deferred_action_cond,
                                &loc_eng_data.deferred_action_mutex);
            // but after we are signalled reacquire the wake lock
            // until we are done processing the event.
            loc_eng_data.acquire_wakelock_cb();
        }

        if (loc_eng_data.deferred_action_flags & DEFERRED_ACTION_QUIT)
        {
            pthread_mutex_unlock(&loc_eng_data.deferred_action_mutex);
            break;
        }

        // copy anything we need before releasing the mutex
        loc_event = loc_eng_data.loc_event;
        if (loc_event != 0) {
            memcpy(&loc_event_payload, &loc_eng_data.loc_event_payload, sizeof(loc_event_payload));
            loc_eng_data.loc_event = 0;
        }

        int flags = loc_eng_data.deferred_action_flags;
        loc_eng_data.deferred_action_flags = 0;
        engine_status = loc_eng_data.agps_status;
        aiding_data_for_deletion = loc_eng_data.aiding_data_for_deletion;
        status.status = loc_eng_data.agps_status;
        loc_eng_data.agps_status = 0;

        // perform all actions after releasing the mutex to avoid blocking RPCs from the ARM9
        pthread_mutex_unlock(&(loc_eng_data.deferred_action_mutex));

        if (loc_event != 0) {
            loc_eng_process_loc_event(loc_event, &loc_event_payload);
        }

        // send_delete_aiding_data must be done when GPS engine is off
        if ((engine_status != GPS_STATUS_SESSION_BEGIN) && (aiding_data_for_deletion != 0))
        {
            loc_eng_delete_aiding_data_deferred_action ();
        }

        if (flags & (DEFERRED_ACTION_AGPS_DATA_SUCCESS |
                     DEFERRED_ACTION_AGPS_DATA_CLOSED |
                     DEFERRED_ACTION_AGPS_DATA_FAILED))
        {
            loc_eng_process_atl_deferred_action(flags);

            pthread_mutex_lock(&(loc_eng_data.deferred_stop_mutex));
            // work around problem with loc_eng_stop when AGPS requests are pending
            // we defer stopping the engine until the AGPS request is done
            loc_eng_data.agps_request_pending = false;
            if (loc_eng_data.stop_request_pending)
            {
                LOGD ("handling deferred stop\n");
                if (loc_stop_fix(loc_eng_data.client_handle) != RPC_LOC_API_SUCCESS)
                {
                    LOGD ("loc_stop_fix failed!\n");
                }
            }
            pthread_mutex_unlock(&(loc_eng_data.deferred_stop_mutex));
        }

        if (status.status != 0 && loc_eng_data.agps_status_cb) {
            loc_eng_data.agps_status_cb(&status);
        }
    }

    // reenable the GPS lock
    LOGD("Setting GPS privacy lock to RPC_LOC_LOCK_ALL\n");
    loc_eng_set_gps_lock(RPC_LOC_LOCK_ALL);

    LOGD("loc_eng_process_deferred_action thread exiting\n");
    loc_eng_data.release_wakelock_cb();

    loc_eng_data.deferred_action_thread = 0;
}

// for gps.c
extern "C" const GpsInterface* get_gps_interface()
{
    return &sLocEngInterface;
}
