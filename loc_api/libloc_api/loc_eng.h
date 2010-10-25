/******************************************************************************
  @file:  loc_eng.h
  @brief:

  DESCRIPTION
    This file defines the global data structure used by this module.

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

#ifndef LOC_ENG_H
#define LOC_ENG_H

// Define boolean type to be used by libgps on loc api module
typedef unsigned char boolean;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#include <loc_eng_ioctl.h>
#include <loc_eng_xtra.h>
#include <hardware/gps.h>

#define LOC_IOCTL_DEFAULT_TIMEOUT 1000 // 1000 milli-seconds

enum {
    DEFERRED_ACTION_EVENT               = 0x01,
    DEFERRED_ACTION_DELETE_AIDING       = 0x02,
    DEFERRED_ACTION_AGPS_STATUS         = 0x04,
    DEFERRED_ACTION_AGPS_DATA_SUCCESS   = 0x08,
    DEFERRED_ACTION_AGPS_DATA_CLOSED    = 0x10,
    DEFERRED_ACTION_AGPS_DATA_FAILED    = 0x20,
    DEFERRED_ACTION_QUIT                = 0x40,
};

// Module data
typedef struct
{
    rpc_loc_client_handle_type  client_handle;

    gps_location_callback           location_cb;
    gps_status_callback             status_cb;
    gps_sv_status_callback          sv_status_cb;
    agps_status_callback            agps_status_cb;
    gps_nmea_callback               nmea_cb;
    gps_ni_notify_callback          ni_notify_cb;
    gps_acquire_wakelock            acquire_wakelock_cb;
    gps_release_wakelock            release_wakelock_cb;
    int                             agps_status;

    // used to defer stopping the GPS engine until AGPS data calls are done
    boolean                         agps_request_pending;
    boolean                         stop_request_pending;
    pthread_mutex_t                 deferred_stop_mutex;

    loc_eng_xtra_data_s_type        xtra_module_data;

    loc_eng_ioctl_data_s_type       ioctl_data;

    // data from loc_event_cb
    rpc_loc_event_mask_type         loc_event;
    rpc_loc_event_payload_u_type    loc_event_payload;

    // TBD:
    char                            agps_server_host[256];
    int                             agps_server_port;
    uint32                          agps_server_address;
    char                            apn_name[100];
    int                             position_mode;
    rpc_loc_server_connection_handle  conn_handle;

    // GPS engine status
    GpsStatusValue                  engine_status;

    // Aiding data information to be deleted, aiding data can only be deleted when GPS engine is off
    GpsAidingData                   aiding_data_for_deletion;

    // Data variables used by deferred action thread
    pthread_t                       deferred_action_thread;
    // Mutex used by deferred action thread
    pthread_mutex_t                 deferred_action_mutex;
    // Condition variable used by deferred action thread
    pthread_cond_t                  deferred_action_cond;

    // flags for pending events for deferred action thread
    int                             deferred_action_flags;
} loc_eng_data_s_type;
   
extern loc_eng_data_s_type loc_eng_data;

#endif // LOC_ENG_H
