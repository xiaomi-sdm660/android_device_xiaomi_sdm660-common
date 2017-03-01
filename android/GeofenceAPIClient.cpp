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
#define LOG_TAG "LocSvc_GeofenceApiClient"

#include <log_util.h>
#include <loc_cfg.h>

#include "GeofenceAPIClient.h"

static void convertGpsLocation(Location& in, GpsLocation& out)
{
    memset(&out, 0, sizeof(GpsLocation));
    out.size = sizeof(GpsLocation);
    if (in.flags & LOCATION_HAS_LAT_LONG_BIT)
        out.flags |= GPS_LOCATION_HAS_LAT_LONG;
    if (in.flags & LOCATION_HAS_ALTITUDE_BIT)
        out.flags |= GPS_LOCATION_HAS_ALTITUDE;
    if (in.flags & LOCATION_HAS_SPEED_BIT)
        out.flags |= GPS_LOCATION_HAS_SPEED;
    if (in.flags & LOCATION_HAS_BEARING_BIT)
        out.flags |= GPS_LOCATION_HAS_BEARING;
    if (in.flags & LOCATION_HAS_ACCURACY_BIT)
        out.flags |= GPS_LOCATION_HAS_ACCURACY;
    out.latitude = in.latitude;
    out.longitude = in.longitude;
    out.altitude = in.altitude;
    out.speed = in.speed;
    out.bearing = in.bearing;
    out.accuracy = in.accuracy;
    out.timestamp = (GpsUtcTime)in.timestamp;
}

GeofenceAPIClient::GeofenceAPIClient(GpsGeofenceCallbacks* cbs) :
    LocationAPIClientBase(),
    mGpsGeofenceCallbacks(cbs)
{
    LOC_LOGD("%s]: (%p)", __func__, cbs);

    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;

    locationCallbacks.geofenceBreachCb = nullptr;
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_transition_callback) {
        locationCallbacks.geofenceBreachCb =
            [this](GeofenceBreachNotification geofenceBreachNotification) {
                onGeofenceBreachCb(geofenceBreachNotification);
            };
    }

    locationCallbacks.geofenceStatusCb = nullptr;
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_status_callback) {
        locationCallbacks.geofenceStatusCb =
            [this](GeofenceStatusNotification geofenceStatusNotification) {
                onGeofenceStatusCb(geofenceStatusNotification);
            };
    }

    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

void GeofenceAPIClient::geofenceAdd(uint32_t geofence_id, double latitude, double longitude,
        double radius_meters, int last_transition, int monitor_transitions,
        int notification_responsiveness_ms, int unknown_timer_ms)
{
    LOC_LOGD("%s]: (%d %f %f %f %d %d %d %d)", __func__,
            geofence_id, latitude, longitude, radius_meters,
            last_transition, monitor_transitions, notification_responsiveness_ms, unknown_timer_ms);

    GeofenceOption options;
    memset(&options, 0, sizeof(GeofenceOption));
    options.size = sizeof(GeofenceOption);
    if (monitor_transitions & GPS_GEOFENCE_ENTERED)
        options.breachTypeMask |= GEOFENCE_BREACH_ENTER_BIT;
    if (monitor_transitions & GPS_GEOFENCE_EXITED)
        options.breachTypeMask |=  GEOFENCE_BREACH_EXIT_BIT;
    options.responsiveness = notification_responsiveness_ms;

    GeofenceInfo data;
    data.size = sizeof(GeofenceInfo);
    data.latitude = latitude;
    data.longitude = longitude;
    data.radius = radius_meters;

    locAPIAddGeofences(1, &geofence_id, &options, &data);
}

void GeofenceAPIClient::geofencePause(uint32_t geofence_id)
{
    LOC_LOGD("%s]: (%d)", __func__, geofence_id);
    locAPIPauseGeofences(1, &geofence_id);
}

void GeofenceAPIClient::geofenceResume(uint32_t geofence_id, int monitor_transitions)
{
    LOC_LOGD("%s]: (%d %d)", __func__, geofence_id, monitor_transitions);
    GeofenceBreachTypeMask mask = 0;
    if (monitor_transitions & GPS_GEOFENCE_ENTERED)
        mask |= GEOFENCE_BREACH_ENTER_BIT;
    if (monitor_transitions & GPS_GEOFENCE_EXITED)
        mask |=  GEOFENCE_BREACH_EXIT_BIT;
    locAPIResumeGeofences(1, &geofence_id, &mask);
}

void GeofenceAPIClient::geofenceRemove(uint32_t geofence_id)
{
    LOC_LOGD("%s]: (%d)", __func__, geofence_id);
    locAPIRemoveGeofences(1, &geofence_id);
}

// callbacks
void GeofenceAPIClient::onGeofenceBreachCb(GeofenceBreachNotification geofenceBreachNotification)
{
    LOC_LOGD("%s]: (%zu)", __func__, geofenceBreachNotification.count);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_transition_callback) {
        for (size_t i = 0; i < geofenceBreachNotification.count; i++) {
            GpsLocation location;
            convertGpsLocation(geofenceBreachNotification.location, location);

            uint32_t transition;
            if (geofenceBreachNotification.type == GEOFENCE_BREACH_ENTER)
                transition = GPS_GEOFENCE_ENTERED;
            else if (geofenceBreachNotification.type == GEOFENCE_BREACH_EXIT)
                transition = GPS_GEOFENCE_EXITED;
            else {
                // continue with other breach if transition is
                // nether GPS_GEOFENCE_ENTERED nor GPS_GEOFENCE_EXITED
                continue;
            }
            GpsUtcTime time = geofenceBreachNotification.timestamp;

            mGpsGeofenceCallbacks->geofence_transition_callback(geofenceBreachNotification.ids[i],
                    &location, transition, time);
        }
    }
}

void GeofenceAPIClient::onGeofenceStatusCb(GeofenceStatusNotification geofenceStatusNotification)
{
    LOC_LOGD("%s]: (%d)", __func__, geofenceStatusNotification.available);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_status_callback) {
        int32_t status = GPS_GEOFENCE_UNAVAILABLE;
        if (geofenceStatusNotification.available == GEOFENCE_STATUS_AVAILABILE_YES) {
            status = GPS_GEOFENCE_AVAILABLE;
        }
        mGpsGeofenceCallbacks->geofence_status_callback(status, nullptr);
    }
}

void GeofenceAPIClient::onAddGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_add_callback) {
        for (size_t i = 0; i < count; i++) {
            int32_t status = GPS_GEOFENCE_ERROR_GENERIC;
            if (errors[i] == LOCATION_ERROR_SUCCESS)
                status = GPS_GEOFENCE_OPERATION_SUCCESS;
            mGpsGeofenceCallbacks->geofence_add_callback(ids[i], status);
        }
    }
}

void GeofenceAPIClient::onRemoveGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_remove_callback) {
        for (size_t i = 0; i < count; i++) {
            int32_t status = GPS_GEOFENCE_ERROR_GENERIC;
            if (errors[i] == LOCATION_ERROR_SUCCESS)
                status = GPS_GEOFENCE_OPERATION_SUCCESS;
            else if (errors[i] == LOCATION_ERROR_ID_UNKNOWN)
                status = GPS_GEOFENCE_ERROR_ID_UNKNOWN;
            mGpsGeofenceCallbacks->geofence_remove_callback(ids[i], status);
        }
    }
}

void GeofenceAPIClient::onPauseGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_pause_callback) {
        for (size_t i = 0; i < count; i++) {
            int32_t status = GPS_GEOFENCE_ERROR_GENERIC;
            if (errors[i] == LOCATION_ERROR_SUCCESS)
                status = GPS_GEOFENCE_OPERATION_SUCCESS;
            else if (errors[i] == LOCATION_ERROR_ID_UNKNOWN)
                status = GPS_GEOFENCE_ERROR_ID_UNKNOWN;
            mGpsGeofenceCallbacks->geofence_pause_callback(ids[i], status);
        }
    }
}

void GeofenceAPIClient::onResumeGeofencesCb(size_t count, LocationError* errors, uint32_t* ids)
{
    LOC_LOGD("%s]: (%zu)", __func__, count);
    if (mGpsGeofenceCallbacks && mGpsGeofenceCallbacks->geofence_resume_callback) {
        for (size_t i = 0; i < count; i++) {
            int32_t status = GPS_GEOFENCE_ERROR_GENERIC;
            if (errors[i] == LOCATION_ERROR_SUCCESS)
                status = GPS_GEOFENCE_OPERATION_SUCCESS;
            else if (errors[i] == LOCATION_ERROR_ID_UNKNOWN)
                status = GPS_GEOFENCE_ERROR_ID_UNKNOWN;
            mGpsGeofenceCallbacks->geofence_resume_callback(ids[i], status);
        }
    }
}
