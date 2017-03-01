/* Copyright (c) 2011-2017, The Linux Foundation. All rights reserved.
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
#define LOG_TAG "LocSvc_afw"

#include <hardware/gps.h>
#include <LocDualContext.h>
#include <loc_target.h>

#include "GnssAPIClient.h"

extern "C" const GpsGeofencingInterface* get_gps_geofence_interface();

static GnssAPIClient* sClient = nullptr;
static GpsCallbacks sGpsCallbacks;
static GpsCallbacks* pGpsCallbacks  = nullptr;

static GpsNiCallbacks sGpsNiCallbacks;
static GpsNiCallbacks* pGpsNiCallbacks = nullptr;

static GpsMeasurementCallbacks sGpsMeasurementCallbacks;
static GpsMeasurementCallbacks* pGpsMeasurementCallbacks = nullptr;

typedef struct {
    bool pending;
    AGpsType type;
    char *hostname;
    int port;
} AgnssServerPack;
static AgnssServerPack pendingAgnssServer = {
    false,
    AGPS_TYPE_SUPL,
    nullptr,
    0
};

typedef struct {
    bool pending;
    char* config_data;
    int32_t length;
} ConfigurationPack;
static ConfigurationPack pendingConfiguration = {
    false,
    nullptr,
    0
};

static GnssAPIClient* getClient() {
    if (!sClient &&
            // in order to create a GnssClient to call gnssUpdateConfig,
            // one of pGpsCallbacks and pGpsNiCallbacks should not be nullptr.
            (pGpsCallbacks != nullptr || pGpsNiCallbacks != nullptr)) {
        sClient = new GnssAPIClient(pGpsCallbacks, pGpsNiCallbacks, pGpsMeasurementCallbacks);
        if (sClient) {
            sClient->locAPIEnable(LOCATION_TECHNOLOGY_TYPE_GNSS);
            if (pendingAgnssServer.pending && pendingAgnssServer.hostname) {
                sClient->gnssAgnssSetServer(pendingAgnssServer.type,
                        pendingAgnssServer.hostname,
                        pendingAgnssServer.port);
                pendingAgnssServer.pending = false;
                free(pendingAgnssServer.hostname);
                pendingAgnssServer.hostname = nullptr;
            }
            if (pendingConfiguration.pending && pendingConfiguration.config_data) {
                sClient->gnssConfigurationUpdate(pendingConfiguration.config_data,
                        pendingConfiguration.length);
                pendingConfiguration.pending = false;
                free(pendingConfiguration.config_data);
                pendingConfiguration.config_data = nullptr;
            }
        }
    }
    if (!sClient) {
        LOC_LOGE("%s:%d] get GnssAPIClient failed", __func__, __LINE__);
    }
    return sClient;
}

/*===========================================================================
  Functions and variables for sGpsInterface
===========================================================================*/
static int loc_init(GpsCallbacks* callbacks) {
    ENTRY_LOG_CALLFLOW();
    int retVal = -1;
    if (callbacks) {
        GnssAPIClient* client = getClient();
        // backup callbacks in case *callbacks is a stack variable
        pGpsCallbacks = &sGpsCallbacks;
        pGpsCallbacks->size = callbacks->size;
        pGpsCallbacks->location_cb = callbacks->location_cb;
        pGpsCallbacks->status_cb = callbacks->status_cb;
        pGpsCallbacks->sv_status_cb = callbacks->sv_status_cb;
        pGpsCallbacks->nmea_cb = callbacks->nmea_cb;
        pGpsCallbacks->set_capabilities_cb = callbacks->set_capabilities_cb;
        pGpsCallbacks->set_system_info_cb = callbacks->set_system_info_cb;
        pGpsCallbacks->gnss_sv_status_cb = callbacks->gnss_sv_status_cb;

        // create MsgTask
        pGpsCallbacks->create_thread_cb = callbacks->create_thread_cb;
        loc_core::LocDualContext::getLocFgContext(
                (LocThread::tCreate)pGpsCallbacks->create_thread_cb,
                nullptr, loc_core::LocDualContext::mLocationHalName, false);

        // will never call these cbs
        pGpsCallbacks->acquire_wakelock_cb = nullptr;
        pGpsCallbacks->release_wakelock_cb = nullptr;
        pGpsCallbacks->request_utc_time_cb = nullptr;

        // we can't create GnssAPIClient before GpsCallbacks or GpsNiCallbacks is set
        if (client) {
            client->gnssUpdateCallbacks(pGpsCallbacks, pGpsNiCallbacks, pGpsMeasurementCallbacks);
        }

        retVal = 0;
    }
    return retVal;
}
static int  loc_start() {
    ENTRY_LOG_CALLFLOW();
    int retVal = -1;
    GnssAPIClient* client = getClient();
    if (client)
        retVal = client->gnssStart();
    return retVal;
}
static int  loc_stop() {
    ENTRY_LOG_CALLFLOW();
    int retVal = -1;
    GnssAPIClient* client = getClient();
    if (client)
        retVal = client->gnssStop();
    return retVal;
}
static void loc_cleanup() {
    ENTRY_LOG_CALLFLOW();
    if (sClient) {
        sClient->locAPIDisable();
    }
    pGpsCallbacks = nullptr;
    pGpsNiCallbacks = nullptr;
    pGpsMeasurementCallbacks = nullptr;

    pendingAgnssServer.pending = false;
    if (pendingAgnssServer.hostname) {
        free(pendingAgnssServer.hostname);
        pendingAgnssServer.hostname = nullptr;
    }
    pendingConfiguration.pending = false;
    if (pendingConfiguration.config_data) {
        free(pendingConfiguration.config_data);
        pendingConfiguration.config_data = nullptr;
    }
}
static int  loc_inject_time(GpsUtcTime /*time*/, int64_t /*timeReference*/, int /*uncertainty*/) {
    return -1;
}
static int  loc_inject_location(double /*latitude*/, double /*longitude*/, float /*accuracy*/) {
    return -1;
}
static void loc_delete_aiding_data(GpsAidingData f) {
    ENTRY_LOG_CALLFLOW();
    GnssAPIClient* client = getClient();
    if (client)
        client->gnssDeleteAidingData(f);
}
static int  loc_set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
                                  uint32_t min_interval, uint32_t preferred_accuracy,
                                  uint32_t preferred_time) {
    ENTRY_LOG_CALLFLOW();
    int retVal = -1;
    GnssAPIClient* client = getClient();
    if (client)
        retVal = client->gnssSetPositionMode(mode, recurrence,
                min_interval, preferred_accuracy, preferred_time);
    return retVal;
}
static const void* loc_get_extension(const char* name);

static const GpsInterface sGpsInterface = {
   sizeof(GpsInterface),
   loc_init,
   loc_start,
   loc_stop,
   loc_cleanup,
   loc_inject_time,
   loc_inject_location,
   loc_delete_aiding_data,
   loc_set_position_mode,
   loc_get_extension
};

/*===========================================================================
  Functions and variables for sAGpsInterface
===========================================================================*/
static void loc_agps_init(AGpsCallbacks* /*callbacks*/) {
}
static int  loc_agps_open(const char* /*apn*/) {
    return -1;
}
static int  loc_agps_closed() { return -1; }
static int  loc_agps_open_failed() { return -1; }
static int  loc_agps_set_server(AGpsType type, const char *hostname, int port) {
    GnssAPIClient* client = getClient();
    if (client)
        client->gnssAgnssSetServer(type, hostname, port);
    else {
        // client is not ready yet
        if (pendingAgnssServer.hostname)
            free(pendingAgnssServer.hostname);
        pendingAgnssServer.type = type;
        pendingAgnssServer.hostname = strdup(hostname);
        pendingAgnssServer.port = port;
        pendingAgnssServer.pending = true;
    }
    return 0;
}
static int  loc_agps_open_with_apniptype(const char* /*apn*/, ApnIpType /*apnIpType*/) {
    return -1;
}

static const AGpsInterface sAGpsInterface = {
   sizeof(AGpsInterface),
   loc_agps_init,
   loc_agps_open,
   loc_agps_closed,
   loc_agps_open_failed,
   loc_agps_set_server,
   loc_agps_open_with_apniptype
};

/*===========================================================================
  Functions and variables for sGpsNiInterface
===========================================================================*/
static void loc_ni_init(GpsNiCallbacks *callbacks) {
    ENTRY_LOG_CALLFLOW();
    if (callbacks) {
        GnssAPIClient* client = getClient();
        pGpsNiCallbacks = &sGpsNiCallbacks;
        pGpsNiCallbacks->notify_cb = callbacks->notify_cb;
        pGpsNiCallbacks->create_thread_cb = callbacks->create_thread_cb;

        if (client) {
            client->gnssUpdateCallbacks(pGpsCallbacks, pGpsNiCallbacks, pGpsMeasurementCallbacks);
        }
    }
}
static void loc_ni_respond(int notif_id, GpsUserResponseType user_response) {
    ENTRY_LOG_CALLFLOW();
    GnssAPIClient* client = getClient();
    if (client)
        client->gnssNiRespond(notif_id, user_response);
}

static const GpsNiInterface sGpsNiInterface =
{
   sizeof(GpsNiInterface),
   loc_ni_init,
   loc_ni_respond,
};

/*===========================================================================
  Functions and variables for sGpsMeasurementInterface
===========================================================================*/
static int loc_gps_measurement_init(GpsMeasurementCallbacks* callbacks) {
    ENTRY_LOG_CALLFLOW();
    int retVal = -1;
    if (callbacks) {
        GnssAPIClient* client = getClient();
        pGpsMeasurementCallbacks = &sGpsMeasurementCallbacks;
        pGpsMeasurementCallbacks->size = sizeof(GpsMeasurementCallbacks);
        pGpsMeasurementCallbacks->measurement_callback = callbacks->measurement_callback;
        pGpsMeasurementCallbacks->gnss_measurement_callback = callbacks->gnss_measurement_callback;

        if (client) {
            client->gnssUpdateCallbacks(pGpsCallbacks, pGpsNiCallbacks, pGpsMeasurementCallbacks);
        }

        retVal = 0;
    }
    return retVal;
}
static void loc_gps_measurement_close() {
    ENTRY_LOG_CALLFLOW();
    GnssAPIClient* client = getClient();
    if (client)
        client->gnssMeasurementClose();
}

static const GpsMeasurementInterface sGpsMeasurementInterface =
{
    sizeof(GpsMeasurementInterface),
    loc_gps_measurement_init,
    loc_gps_measurement_close
};

/*===========================================================================
  Functions and variables for sGnssConfigurationInterface
===========================================================================*/
static void loc_configuration_update(const char* config_data, int32_t length) {
    ENTRY_LOG_CALLFLOW();
    GnssAPIClient* client = getClient();
    if (client)
        client->gnssConfigurationUpdate(config_data, length);
    else {
        // client is not ready yet
        if (pendingConfiguration.config_data)
            free(pendingConfiguration.config_data);
        pendingConfiguration.config_data = strdup(config_data);
        pendingConfiguration.length = length;
        pendingConfiguration.pending = true;
    }
}

static const GnssConfigurationInterface sGnssConfigurationInterface =
{
    sizeof(GnssConfigurationInterface),
    loc_configuration_update
};

// Function exposed to gps hal
extern "C" const GpsInterface* get_gps_interface()
{
    unsigned int target = loc_get_target();
    int gnssType = getTargetGnssType(target);
    if (gnssType == GNSS_NONE){
        LOC_LOGE("%s:%d] No GNSS HW on this target. Returning nullptr", __func__, __LINE__);
        return nullptr;
    }
    return &sGpsInterface;
}

const void* loc_get_extension(const char* name)
{
    const void* retVal = nullptr;

    LOC_LOGD("%s:%d] For Interface = %s\n", __func__, __LINE__, name);
    if (strcmp(name, AGPS_INTERFACE) == 0) {
        retVal = &sAGpsInterface;
    } else if (strcmp(name, GPS_NI_INTERFACE) == 0) {
        retVal = &sGpsNiInterface;
    } else if (strcmp(name, GPS_GEOFENCING_INTERFACE) == 0) {
        retVal = get_gps_geofence_interface();
    } else if (strcmp(name, GPS_MEASUREMENT_INTERFACE) == 0) {
        retVal = &sGpsMeasurementInterface;
    } else if (strcmp(name, GNSS_CONFIGURATION_INTERFACE) == 0) {
        retVal = &sGnssConfigurationInterface;
    }

    if (!retVal) {
        LOC_LOGE ("%s:%d] %s is not supported", __func__, __LINE__, name);
    }
    return retVal;
}
