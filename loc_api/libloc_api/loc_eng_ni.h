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
