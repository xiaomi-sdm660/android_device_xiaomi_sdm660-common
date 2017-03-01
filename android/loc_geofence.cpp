/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#define LOG_TAG "LocSvc_geofence"

#include <hardware/gps.h>
#include <GeofenceAPIClient.h>

static GeofenceAPIClient* sClient = nullptr;

/*===========================================================================
  Functions and variables for sGpsGeofencingInterface
===========================================================================*/
static GpsGeofenceCallbacks sGpsGeofenceCbs;

static void loc_geofence_init(GpsGeofenceCallbacks* callbacks) {
    if (callbacks && !sClient) {
        sGpsGeofenceCbs.geofence_transition_callback = callbacks->geofence_transition_callback;
        sGpsGeofenceCbs.geofence_status_callback = callbacks->geofence_status_callback;
        sGpsGeofenceCbs.geofence_add_callback = callbacks->geofence_add_callback;
        sGpsGeofenceCbs.geofence_remove_callback = callbacks->geofence_remove_callback;
        sGpsGeofenceCbs.geofence_pause_callback = callbacks->geofence_pause_callback;
        sGpsGeofenceCbs.geofence_resume_callback = callbacks->geofence_resume_callback;
        sGpsGeofenceCbs.create_thread_cb = callbacks->create_thread_cb;

        sClient = new GeofenceAPIClient(&sGpsGeofenceCbs);
    }
}
static void loc_add_geofence_area(int32_t geofence_id, double latitude, double longitude,
    double radius_meters, int last_transition, int monitor_transitions,
    int notification_responsiveness_ms, int unknown_timer_ms) {
    if (sClient)
        sClient->geofenceAdd(geofence_id, latitude, longitude,
                radius_meters, last_transition, monitor_transitions,
                notification_responsiveness_ms, unknown_timer_ms);
}
static void loc_pause_geofence(int32_t geofence_id) {
    if (sClient)
        sClient->geofencePause(geofence_id);
}
static void loc_resume_geofence(int32_t geofence_id, int monitor_transitions) {
    if (sClient)
        sClient->geofenceResume(geofence_id, monitor_transitions);
}
static void loc_remove_geofence_area(int32_t geofence_id) {
    if (sClient)
        sClient->geofenceRemove(geofence_id);
}

static const GpsGeofencingInterface sGpsGeofencingInterface =
{
    sizeof(GpsGeofencingInterface),
    loc_geofence_init,
    loc_add_geofence_area,
    loc_pause_geofence,
    loc_resume_geofence,
    loc_remove_geofence_area
};

// Function exposed to gps hal
extern "C" const GpsGeofencingInterface* get_gps_geofence_interface()
{
    return &sGpsGeofencingInterface;
}
