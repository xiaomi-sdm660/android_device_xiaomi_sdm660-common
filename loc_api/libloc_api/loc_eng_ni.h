/******************************************************************************
  @file:  loc_eng_ni.cpp
  @brief:  module for network initiated interactions

  DESCRIPTION
     LOC_API network initiated operation support

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2009 QUALCOMM Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  -----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
07/30/09   dx       Initial version

$Id:
======================================================================*/

#ifndef LOC_ENG_NI_H
#define LOC_ENG_NI_H

#include <hardware/gps.h>

#define LOC_NI_NO_RESPONSE_TIME            20                      /* secs */

extern const GpsNiInterface sLocEngNiInterface;

typedef struct {
    pthread_mutex_t         loc_ni_lock;
    int                     response_time_left;       /* examine time for NI response */
    boolean                 notif_in_progress;        /* NI notification/verification in progress */
    rpc_loc_ni_event_s_type loc_ni_request;
    int                     current_notif_id;         /* ID to check against response */
} loc_eng_ni_data_s_type;

// Functions for sLocEngNiInterface
extern void loc_eng_ni_init(GpsNiCallbacks *callbacks);
extern void loc_eng_ni_respond(int notif_id, GpsUserResponseType user_response);

extern int loc_eng_ni_callback (
        rpc_loc_event_mask_type               loc_event,              /* event mask           */
        const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
);

#endif /* LOC_ENG_NI_H */
