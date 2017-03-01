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
#define LOG_TAG "LocSvc_GnssAPIClient"

#include <log_util.h>
#include <loc_cfg.h>

#include "GnssAPIClient.h"

static void convertGpsLocation(Location& in, GpsLocation& out);
static void convertGpsSvStatus(GnssSvNotification& in, GpsSvStatus& out);
static void convertGnssConstellationType(GnssSvType& in, GnssConstellationType& out);
static void convertGnssSvStatus(GnssSvNotification& in, GnssSvStatus& out);
static void convertGpsMeasurement(GnssMeasurementsData& in, GpsMeasurement& out);
static void convertGpsClock(GnssMeasurementsClock& in, GpsClock& out);
static void convertGpsData(GnssMeasurementsNotification& in, GpsData& out);
static void convertGnssClock(GnssMeasurementsClock& in, GnssClock& out);
static void convertGnssData(GnssMeasurementsNotification& in, GnssData& out);

GnssAPIClient::GnssAPIClient(GpsCallbacks* gpsCb,
        GpsNiCallbacks* niCb,
        GpsMeasurementCallbacks* measurementCb) :
    LocationAPIClientBase(),
    mGpsCallbacks(nullptr),
    mGpsNiCallbacks(nullptr),
    mLocationCapabilitiesMask(0),
    mGpsMeasurementCallbacks(nullptr)
{
    LOC_LOGD("%s]: (%p %p %p)", __func__, gpsCb, niCb, measurementCb);
    pthread_mutex_init(&mLock, nullptr);

    // set default LocationOptions.
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = 1000;
    mLocationOptions.minDistance = 0;
    mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;

    gnssUpdateCallbacks(gpsCb, niCb, measurementCb);
}

GnssAPIClient::~GnssAPIClient()
{
    LOC_LOGD("%s]: ()", __func__);
    pthread_mutex_destroy(&mLock);
}

// for GpsInterface
void GnssAPIClient::gnssUpdateCallbacks(GpsCallbacks* gpsCb,
        GpsNiCallbacks* niCb,
        GpsMeasurementCallbacks* measurementCb)
{
    LOC_LOGD("%s]: (%p %p %p)", __func__, gpsCb, niCb, measurementCb);

    mGpsCallbacks = gpsCb;
    mGpsNiCallbacks = niCb;
    mGpsMeasurementCallbacks = measurementCb;

    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    if (mGpsCallbacks && mGpsCallbacks->location_cb) {
        locationCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
        };
    }

    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;

    locationCallbacks.gnssNiCb = nullptr;
    if (mGpsNiCallbacks && mGpsNiCallbacks->notify_cb) {
        locationCallbacks.gnssNiCb = [this](uint32_t id, GnssNiNotification gnssNiNotification) {
            onGnssNiCb(id, gnssNiNotification);
        };
    }

    locationCallbacks.gnssSvCb = nullptr;
    if (mGpsCallbacks && mGpsCallbacks->sv_status_cb) {
        locationCallbacks.gnssSvCb = [this](GnssSvNotification gnssSvNotification) {
            onGnssSvCb(gnssSvNotification);
        };
    }

    locationCallbacks.gnssNmeaCb = nullptr;
    if (mGpsCallbacks && mGpsCallbacks->nmea_cb) {
        locationCallbacks.gnssNmeaCb = [this](GnssNmeaNotification gnssNmeaNotification) {
            onGnssNmeaCb(gnssNmeaNotification);
        };
    }

    locationCallbacks.gnssMeasurementsCb = nullptr;
    if (mGpsMeasurementCallbacks &&
            (mGpsMeasurementCallbacks->measurement_callback ||
             mGpsMeasurementCallbacks->gnss_measurement_callback)) {
        locationCallbacks.gnssMeasurementsCb =
            [this](GnssMeasurementsNotification gnssMeasurementsNotification) {
                onGnssMeasurementsCb(gnssMeasurementsNotification);
            };
    }

    locAPISetCallbacks(locationCallbacks);
}

int GnssAPIClient::gnssStart()
{
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    locAPIStartTracking(mLocationOptions);
    return retVal;
}

int GnssAPIClient::gnssStop()
{
    LOC_LOGD("%s]: ()", __func__);
    int retVal = 0;
    locAPIStopTracking();
    return retVal;
}

void GnssAPIClient::gnssDeleteAidingData(GpsAidingData f)
{
    LOC_LOGD("%s]: (%02x)", __func__, f);
    GnssAidingData data;
    memset(&data, 0, sizeof (GnssAidingData));
    data.sv.svTypeMask = GNSS_AIDING_DATA_SV_TYPE_GPS |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS |
        GNSS_AIDING_DATA_SV_TYPE_QZSS |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO;

    if (f == GPS_DELETE_ALL)
        data.deleteAll = true;
    else {
        if (f & GPS_DELETE_EPHEMERIS) data.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS;
        if (f & GPS_DELETE_ALMANAC) data.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC;
        if (f & GPS_DELETE_POSITION) data.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION;
        if (f & GPS_DELETE_TIME) data.common.mask |= GNSS_AIDING_DATA_COMMON_TIME;
        if (f & GPS_DELETE_IONO) data.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE;
        if (f & GPS_DELETE_UTC) data.common.mask |= GNSS_AIDING_DATA_COMMON_UTC;
        if (f & GPS_DELETE_HEALTH) data.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH;
        if (f & GPS_DELETE_SVDIR) data.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION;
        if (f & GPS_DELETE_SVSTEER) data.sv.svMask |= GNSS_AIDING_DATA_SV_STEER;
        if (f & GPS_DELETE_SADATA) data.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA;
        if (f & GPS_DELETE_RTI) data.common.mask |= GNSS_AIDING_DATA_COMMON_RTI;
        if (f & GPS_DELETE_CELLDB_INFO) data.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB;
    }
    locAPIGnssDeleteAidingData(data);
}

int GnssAPIClient::gnssSetPositionMode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
        uint32_t min_interval, uint32_t preferred_accuracy, uint32_t preferred_time)
{
    LOC_LOGD("%s]: (%d %d %d %d %d)", __func__,
            mode, recurrence, min_interval, preferred_accuracy, preferred_time);
    int retVal = 0;
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = min_interval;
    mLocationOptions.minDistance = preferred_accuracy;
    if (mode == GPS_POSITION_MODE_STANDALONE)
        mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;
    else if (mode == GPS_POSITION_MODE_MS_BASED)
        mLocationOptions.mode = GNSS_SUPL_MODE_MSB;
    else if (mode == GPS_POSITION_MODE_MS_ASSISTED)
        mLocationOptions.mode = GNSS_SUPL_MODE_MSA;
    return retVal;
}

// for AGpsInterface
void GnssAPIClient::gnssAgnssSetServer(AGpsType type, const char *hostname, int port)
{
    LOC_LOGD("%s]: (%d %s %d)", __func__, type, hostname, port);
    GnssConfig data;
    memset(&data, 0, sizeof(GnssConfig));
    data.size = sizeof(GnssConfig);
    data.flags |= GNSS_CONFIG_FLAGS_SET_ASSISTANCE_DATA_VALID_BIT;

    memset(&data.assistanceServer, 0, sizeof(GnssConfigSetAssistanceServer));
    data.assistanceServer.size = sizeof(GnssConfigSetAssistanceServer);
    if (type == AGPS_TYPE_SUPL)
        data.assistanceServer.type = GNSS_ASSISTANCE_TYPE_SUPL;
    else if (type == AGPS_TYPE_C2K)
        data.assistanceServer.type = GNSS_ASSISTANCE_TYPE_C2K;
    data.assistanceServer.hostName = hostname;
    data.assistanceServer.port = port;

    locAPIGnssUpdateConfig(data);
}

// for GpsNiInterface
void GnssAPIClient::gnssNiRespond(int notif_id, GpsUserResponseType user_response)
{
    LOC_LOGD("%s]: (%d %d)", __func__, notif_id, user_response);
    GnssNiResponse data = GNSS_NI_RESPONSE_IGNORE;
    if (user_response == GPS_NI_RESPONSE_ACCEPT) data = GNSS_NI_RESPONSE_ACCEPT;
    else if (user_response == GPS_NI_RESPONSE_DENY) data = GNSS_NI_RESPONSE_DENY;
    else if (user_response == GPS_NI_RESPONSE_NORESP) data = GNSS_NI_RESPONSE_NO_RESPONSE;
    locAPIGnssNiResponse(notif_id, data);
}

// for GpsMeasurementInterface
void GnssAPIClient::gnssMeasurementClose() {
    LOC_LOGD("%s]: ()", __func__);
    pthread_mutex_lock(&mLock);
    mGpsMeasurementCallbacks = nullptr;
    pthread_mutex_unlock(&mLock);
}

// for GnssConfigurationInterface
void GnssAPIClient::gnssConfigurationUpdate(const char* config_data, int32_t length)
{
    LOC_LOGD("%s]: (%s %d)", __func__, config_data, length);
    int n = 10;
    uint8_t flags[n];
    memset(&flags, 0, sizeof(uint8_t) * n);

    GnssConfig data;
    memset(&data, 0, sizeof(GnssConfig));
    data.size = sizeof(GnssConfig);

    const loc_param_s_type gnssConfTable[] =
    {
        {"GPS_LOCK",                            &data.gpsLock,                      flags+0, 'n'},
        {"SUPL_VER",                            &data.suplVersion,                  flags+1, 'n'},
        //{"ASSISTANCE_SERVER",                 &data.assistanceServer,             nullptr, 's'},
        {"LPP_PROFILE",                         &data.lppProfile,                   flags+3, 'n'},
        {"LPPE_CP_TECHNOLOGY",                  &data.lppeControlPlaneMask,         flags+4, 'n'},
        {"LPPE_UP_TECHNOLOGY",                  &data.lppeUserPlaneMask,            flags+5, 'n'},
        {"A_GLONASS_POS_PROTOCOL_SELECT",       &data.aGlonassPositionProtocolMask, flags+6, 'n'},
        {"USE_EMERGENCY_PDN_FOR_EMERGENCY_SUPL",&data.emergencyPdnForEmergencySupl, flags+7, 'n'},
        {"SUPL_ES",                             &data.suplEmergencyServices,        flags+8, 'n'},
        {"SUPL_MODE",                           &data.suplModeMask,                 flags+9, 'n'},
    };
    UTIL_UPDATE_CONF(config_data, length, gnssConfTable);

    for (int i = 0; i < n; i++) {
        if (flags[i] != 0)
            data.flags |= (0x1 << i);
    }
    locAPIGnssUpdateConfig(data);
}

// callbacks
void GnssAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask)
{
    LOC_LOGD("%s]: (%02x)", __func__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
    if (mGpsCallbacks && mGpsCallbacks->set_capabilities_cb) {
        uint32_t data = 0;
        if ((capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT))
            data |= GPS_CAPABILITY_SCHEDULING;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GEOFENCE_BIT)
            data |= GPS_CAPABILITY_GEOFENCING;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT)
            data |= GPS_CAPABILITY_MEASUREMENTS;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
            data |= GPS_CAPABILITY_MSB;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
            data |= GPS_CAPABILITY_MSA;
        mGpsCallbacks->set_capabilities_cb(data);
    }
    if (mGpsCallbacks && mGpsCallbacks->set_system_info_cb) {
        GnssSystemInfo info;
        info.size = sizeof(GnssSystemInfo);
        info.year_of_hw = 2015;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT) {
            info.year_of_hw = 2017;
        }
        LOC_LOGV("%s:%d] set_system_info_cb (%d)", __func__, __LINE__, info.year_of_hw);
        mGpsCallbacks->set_system_info_cb(&info);
    }
}

void GnssAPIClient::onTrackingCb(Location location)
{
    LOC_LOGD("%s]: (flags: %02x)", __func__, location.flags);
    if (mGpsCallbacks && mGpsCallbacks->location_cb) {
        GpsLocation data;
        convertGpsLocation(location, data);
        mGpsCallbacks->location_cb(&data);
    }
}

void GnssAPIClient::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification)
{
    LOC_LOGD("%s]: (id: %d)", __func__, id);
    if (mGpsNiCallbacks && mGpsNiCallbacks->notify_cb) {
        GpsNiNotification data;
        memset(&data, 0, sizeof(GpsNiNotification));
        data.size = sizeof(GpsNiNotification);
        data.notification_id = id;

        if (gnssNiNotification.type == GNSS_NI_TYPE_VOICE)
            data.ni_type = GPS_NI_TYPE_VOICE;
        else if (gnssNiNotification.type == GNSS_NI_TYPE_SUPL)
            data.ni_type = GPS_NI_TYPE_UMTS_SUPL;
        else if (gnssNiNotification.type == GNSS_NI_TYPE_CONTROL_PLANE)
            data.ni_type = GPS_NI_TYPE_UMTS_CTRL_PLANE;
        // GNSS_NI_TYPE_EMERGENCY_SUPL not supported

        if (gnssNiNotification.options == GNSS_NI_OPTIONS_NOTIFICATION)
            data.notify_flags = GPS_NI_NEED_NOTIFY;
        else if (gnssNiNotification.options == GNSS_NI_OPTIONS_VERIFICATION)
            data.notify_flags = GPS_NI_NEED_VERIFY;
        else if (gnssNiNotification.options == GNSS_NI_OPTIONS_PRIVACY_OVERRIDE)
            data.notify_flags = GPS_NI_PRIVACY_OVERRIDE;

        data.timeout = gnssNiNotification.timeout;
        if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_ACCEPT)
            data.default_response = GPS_NI_RESPONSE_ACCEPT;
        else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_DENY)
            data.default_response = GPS_NI_RESPONSE_DENY;
        else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_NO_RESPONSE ||
                gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_IGNORE)
            data.default_response = GPS_NI_RESPONSE_NORESP;

        int len = GPS_NI_SHORT_STRING_MAXLEN < GNSS_NI_REQUESTOR_MAX
            ? GPS_NI_SHORT_STRING_MAXLEN : GNSS_NI_REQUESTOR_MAX;
        memcpy(data.requestor_id, gnssNiNotification.requestor, len);

        len = GPS_NI_LONG_STRING_MAXLEN < GNSS_NI_MESSAGE_ID_MAX
            ? GPS_NI_LONG_STRING_MAXLEN : GNSS_NI_MESSAGE_ID_MAX;
        memcpy(data.text, gnssNiNotification.message, len);

        if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_NONE)
            data.requestor_id_encoding = GPS_ENC_NONE;
        else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
            data.requestor_id_encoding = GPS_ENC_SUPL_GSM_DEFAULT;
        else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
            data.requestor_id_encoding = GPS_ENC_SUPL_UTF8;
        else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
            data.requestor_id_encoding = GPS_ENC_SUPL_UCS2;

        if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_NONE)
            data.text_encoding = GPS_ENC_NONE;
        else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
            data.text_encoding = GPS_ENC_SUPL_GSM_DEFAULT;
        else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
            data.text_encoding = GPS_ENC_SUPL_UTF8;
        else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
            data.text_encoding = GPS_ENC_SUPL_UCS2;
        data.text_encoding = gnssNiNotification.messageEncoding;

        memcpy(data.extras, gnssNiNotification.extras, len);

        mGpsNiCallbacks->notify_cb(&data);
    }
}

void GnssAPIClient::onGnssSvCb(GnssSvNotification gnssSvNotification)
{
    LOC_LOGD("%s]: (count: %zu)", __func__, gnssSvNotification.count);
    if (mGpsCallbacks && mGpsCallbacks->sv_status_cb) {
        GpsSvStatus data;
        convertGpsSvStatus(gnssSvNotification, data);
        mGpsCallbacks->sv_status_cb(&data);
    }
    if (mGpsCallbacks && mGpsCallbacks->gnss_sv_status_cb) {
        GnssSvStatus data;
        convertGnssSvStatus(gnssSvNotification, data);
        mGpsCallbacks->gnss_sv_status_cb(&data);
    }
}

void GnssAPIClient::onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification)
{
    if (mGpsCallbacks && mGpsCallbacks->nmea_cb) {
        mGpsCallbacks->nmea_cb((GpsUtcTime)gnssNmeaNotification.timestamp,
                gnssNmeaNotification.nmea, gnssNmeaNotification.length);
    }
}

void GnssAPIClient::onGnssMeasurementsCb(GnssMeasurementsNotification gnssMeasurementsNotification)
{
    LOC_LOGD("%s]: (count: %zu)", __func__, gnssMeasurementsNotification.count);
    // we don't need to lock the mutext
    // if mGpsMeasurementCallbacks is set to nullptr
    if (mGpsMeasurementCallbacks) {
        pthread_mutex_lock(&mLock);
        if (mGpsMeasurementCallbacks) {
            if (mGpsMeasurementCallbacks->measurement_callback) {
                GpsData data;
                convertGpsData(gnssMeasurementsNotification, data);
                mGpsMeasurementCallbacks->measurement_callback(&data);
            }
            if (mGpsMeasurementCallbacks->gnss_measurement_callback) {
                GnssData data;
                convertGnssData(gnssMeasurementsNotification, data);
                mGpsMeasurementCallbacks->gnss_measurement_callback(&data);
            }
        }
        pthread_mutex_unlock(&mLock);
    }
}

void GnssAPIClient::onStartTrackingCb(LocationError error)
{
    LOC_LOGD("%s]: (%d)", __func__, error);
    if (error == LOCATION_ERROR_SUCCESS && mGpsCallbacks && mGpsCallbacks->status_cb) {
        GpsStatus data;
        data.size = sizeof(GpsStatus);
        data.status = GPS_STATUS_ENGINE_ON;
        mGpsCallbacks->status_cb(&data);
        data.status = GPS_STATUS_SESSION_BEGIN;
        mGpsCallbacks->status_cb(&data);
    }
}

void GnssAPIClient::onStopTrackingCb(LocationError error)
{
    LOC_LOGD("%s]: (%d)", __func__, error);
    if (error == LOCATION_ERROR_SUCCESS && mGpsCallbacks && mGpsCallbacks->status_cb) {
        GpsStatus data;
        data.size = sizeof(GpsStatus);
        data.status = GPS_STATUS_SESSION_END;
        mGpsCallbacks->status_cb(&data);
        data.status = GPS_STATUS_ENGINE_OFF;
        mGpsCallbacks->status_cb(&data);
    }
}

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

static void convertGpsSvStatus(GnssSvNotification& in, GpsSvStatus& out)
{
    memset(&out, 0, sizeof(GpsSvStatus));
    out.size = sizeof(GpsSvStatus);
    out.num_svs = in.count;
    int len = GPS_MAX_SVS < GNSS_SV_MAX ? GPS_MAX_SVS : GNSS_SV_MAX;
    for (int i = 0; i < len; i++) {
        GpsSvInfo& info = out.sv_list[i];
        info.size = sizeof(GpsSvInfo);
        info.prn = in.gnssSvs[i].svId;
        info.snr = in.gnssSvs[i].cN0Dbhz;
        info.elevation = in.gnssSvs[i].elevation;
        info.azimuth = in.gnssSvs[i].azimuth;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_EPHEMER_BIT)
            out.ephemeris_mask |= 0x1 < i;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_ALMANAC_BIT)
            out.almanac_mask |= 0x1 < i;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_USED_IN_FIX_BIT)
            out.used_in_fix_mask |= 0x1 < i;
    }
}

static void convertGnssConstellationType(GnssSvType& in, GnssConstellationType& out)
{
    switch(in) {
        case GNSS_SV_TYPE_GPS:
            out = GNSS_CONSTELLATION_GPS;
            break;
        case GNSS_SV_TYPE_SBAS:
            out = GNSS_CONSTELLATION_SBAS;
            break;
        case GNSS_SV_TYPE_GLONASS:
            out = GNSS_CONSTELLATION_GLONASS;
            break;
        case GNSS_SV_TYPE_QZSS:
            out = GNSS_CONSTELLATION_QZSS;
            break;
        case GNSS_SV_TYPE_BEIDOU:
            out = GNSS_CONSTELLATION_BEIDOU;
            break;
        case GNSS_SV_TYPE_GALILEO:
            out = GNSS_CONSTELLATION_GALILEO;
            break;
        default:
            out = GNSS_CONSTELLATION_UNKNOWN;
            break;
    }
}

static void convertGnssSvStatus(GnssSvNotification& in, GnssSvStatus& out)
{
    memset(&out, 0, sizeof(GnssSvStatus));
    out.size = sizeof(GnssSvStatus);
    out.num_svs = in.count;
    int len = GNSS_MAX_SVS < GNSS_SV_MAX ? GNSS_MAX_SVS : GNSS_SV_MAX;
    for (int i = 0; i < len; i++) {
        GnssSvInfo& info = out.gnss_sv_list[i];
        info.size = sizeof(GnssSvInfo);
        info.svid = in.gnssSvs[i].svId;
        convertGnssConstellationType(in.gnssSvs[i].type, info.constellation);
        info.c_n0_dbhz = in.gnssSvs[i].cN0Dbhz;
        info.elevation = in.gnssSvs[i].elevation;
        info.azimuth = in.gnssSvs[i].azimuth;
        info.flags = GNSS_SV_FLAGS_NONE;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_EPHEMER_BIT)
            info.flags |= GNSS_SV_FLAGS_HAS_EPHEMERIS_DATA;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_ALMANAC_BIT)
            info.flags |= GNSS_SV_FLAGS_HAS_ALMANAC_DATA;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_USED_IN_FIX_BIT)
            info.flags |= GNSS_SV_FLAGS_USED_IN_FIX;
    }
}

static void convertGpsMeasurement(GnssMeasurementsData& in, GpsMeasurement& out)
{
    memset(&out, 0, sizeof(GpsMeasurement));
    out.size = sizeof(GpsMeasurement);
    if (in.flags & GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_SNR;
    if (in.flags & GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_PSEUDORANGE;
    if (in.flags & GNSS_MEASUREMENTS_DATA_PSEUDORANGE_RATE_UNCERTAINTY_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_PSEUDORANGE_UNCERTAINTY;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_CARRIER_FREQUENCY;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_CARRIER_CYCLES;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_CARRIER_PHASE;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT)
        out.flags |= GPS_MEASUREMENT_HAS_CARRIER_PHASE_UNCERTAINTY;
    out.prn = in.svId;
    out.time_offset_ns = in.timeOffsetNs;
    out.state = GNSS_MEASUREMENT_STATE_UNKNOWN;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT)
        out.state |= GPS_MEASUREMENT_STATE_CODE_LOCK;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT)
        out.state |= GPS_MEASUREMENT_STATE_BIT_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT)
        out.state |= GPS_MEASUREMENT_STATE_SUBFRAME_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT)
        out.state |= GPS_MEASUREMENT_STATE_TOW_DECODED;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT)
        out.state |= GPS_MEASUREMENT_STATE_MSEC_AMBIGUOUS;
    out.received_gps_tow_ns = in.receivedSvTimeNs;
    out.received_gps_tow_uncertainty_ns = in.receivedSvTimeUncertaintyNs;
    out.c_n0_dbhz = in.carrierToNoiseDbHz;
    out.pseudorange_rate_mps = in.pseudorangeRateMps;
    out.pseudorange_rate_uncertainty_mps = in.pseudorangeRateUncertaintyMps;
    out.accumulated_delta_range_state = GNSS_ADR_STATE_UNKNOWN;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT)
        out.accumulated_delta_range_state |= GPS_ADR_STATE_VALID;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT)
        out.accumulated_delta_range_state |= GPS_ADR_STATE_RESET;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT)
        out.accumulated_delta_range_state |= GPS_ADR_STATE_CYCLE_SLIP;
    out.accumulated_delta_range_m = in.adrMeters;
    out.accumulated_delta_range_uncertainty_m = in.adrUncertaintyMeters;
    out.carrier_frequency_hz = in.carrierFrequencyHz;
    out.carrier_cycles = in.carrierCycles;
    out.carrier_phase = in.carrierPhase;
    out.carrier_phase_uncertainty = in.carrierPhaseUncertainty;
    out.loss_of_lock = GPS_LOSS_OF_LOCK_UNKNOWN;
    out.multipath_indicator = GPS_MULTIPATH_INDICATOR_UNKNOWN;
    if (in.multipathIndicator & GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_PRESENT)
        out.multipath_indicator |= GPS_MULTIPATH_INDICATOR_DETECTED;
    if (in.multipathIndicator & GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_NOT_PRESENT)
        out.multipath_indicator |= GPS_MULTIPATH_INDICATOR_NOT_USED;
    out.snr_db = in.signalToNoiseRatioDb;
}

static void convertGpsClock(GnssMeasurementsClock& in, GpsClock& out)
{
    memset(&out, 0, sizeof(GpsClock));
    out.size = sizeof(GpsClock);
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT)
        out.flags |= GPS_CLOCK_HAS_LEAP_SECOND;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT)
        out.flags |= GPS_CLOCK_HAS_TIME_UNCERTAINTY;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT)
        out.flags |= GPS_CLOCK_HAS_FULL_BIAS;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT)
        out.flags |= GPS_CLOCK_HAS_BIAS;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT)
        out.flags |= GPS_CLOCK_HAS_BIAS_UNCERTAINTY;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT)
        out.flags |= GPS_CLOCK_HAS_DRIFT;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT)
        out.flags |= GPS_CLOCK_HAS_DRIFT_UNCERTAINTY;
    out.leap_second = in.leapSecond;
    out.type = GPS_CLOCK_TYPE_UNKNOWN;
    out.time_ns = in.timeNs;
    out.time_uncertainty_ns = in.timeUncertaintyNs;
    out.full_bias_ns = in.fullBiasNs;
    out.bias_ns = in.biasNs;
    out.bias_uncertainty_ns = in.biasUncertaintyNs;
    out.drift_nsps = in.driftNsps;
    out.drift_uncertainty_nsps = in.driftUncertaintyNsps;
}

static void convertGpsData(GnssMeasurementsNotification& in, GpsData& out)
{
    memset(&out, 0, sizeof(GpsData));
    out.size = sizeof(GpsData);
    out.measurement_count = in.count;
    int len = GPS_MAX_MEASUREMENT < GNSS_MEASUREMENTS_MAX
        ? GPS_MAX_MEASUREMENT : GNSS_MEASUREMENTS_MAX;
    for (int i = 0; i < len; i++) {
        convertGpsMeasurement(in.measurements[i], out.measurements[i]);
    }
    convertGpsClock(in.clock, out.clock);
}

static void convertGnssMeasurement(GnssMeasurementsData& in, GnssMeasurement& out)
{
    memset(&out, 0, sizeof(GnssMeasurement));
    out.size = sizeof(GnssMeasurement);
    if (in.flags & GNSS_MEASUREMENTS_DATA_SIGNAL_TO_NOISE_RATIO_BIT)
        out.flags |= GNSS_MEASUREMENT_HAS_SNR;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_FREQUENCY_BIT)
        out.flags |= GNSS_MEASUREMENT_HAS_CARRIER_FREQUENCY;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_CYCLES_BIT)
        out.flags |= GNSS_MEASUREMENT_HAS_CARRIER_CYCLES;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_BIT)
        out.flags |= GNSS_MEASUREMENT_HAS_CARRIER_PHASE;
    if (in.flags & GNSS_MEASUREMENTS_DATA_CARRIER_PHASE_UNCERTAINTY_BIT)
        out.flags |= GNSS_MEASUREMENT_HAS_CARRIER_PHASE_UNCERTAINTY;
    out.svid = in.svId;
    convertGnssConstellationType(in.svType, out.constellation);
    out.time_offset_ns = in.timeOffsetNs;
    out.state = GNSS_MEASUREMENT_STATE_UNKNOWN;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_CODE_LOCK_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_CODE_LOCK;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_BIT_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_BIT_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_SUBFRAME_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_SUBFRAME_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_TOW_DECODED_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_TOW_DECODED;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_MSEC_AMBIGUOUS_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_MSEC_AMBIGUOUS;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_SYMBOL_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_SYMBOL_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_GLO_STRING_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_GLO_STRING_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_GLO_TOD_DECODED_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_GLO_TOD_DECODED;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_BDS_D2_BIT_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_BDS_D2_BIT_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_BDS_D2_SUBFRAME_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_BDS_D2_SUBFRAME_SYNC;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_GAL_E1BC_CODE_LOCK_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_GAL_E1BC_CODE_LOCK;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_GAL_E1C_2ND_CODE_LOCK_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_GAL_E1C_2ND_CODE_LOCK;
    if (in.stateMask & GNSS_MEASUREMENTS_STATE_GAL_E1B_PAGE_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_GAL_E1B_PAGE_SYNC;
    if (in.stateMask &  GNSS_MEASUREMENTS_STATE_SBAS_SYNC_BIT)
        out.state |= GNSS_MEASUREMENT_STATE_SBAS_SYNC;
    out.received_sv_time_in_ns = in.receivedSvTimeNs;
    out.received_sv_time_uncertainty_in_ns = in.receivedSvTimeUncertaintyNs;
    out.c_n0_dbhz = in.carrierToNoiseDbHz;
    out.pseudorange_rate_mps = in.pseudorangeRateMps;
    out.pseudorange_rate_uncertainty_mps = in.pseudorangeRateUncertaintyMps;
    out.accumulated_delta_range_state = GNSS_ADR_STATE_UNKNOWN;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_VALID_BIT)
        out.accumulated_delta_range_state |= GNSS_ADR_STATE_VALID;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_RESET_BIT)
        out.accumulated_delta_range_state |= GNSS_ADR_STATE_RESET;
    if (in.adrStateMask & GNSS_MEASUREMENTS_ACCUMULATED_DELTA_RANGE_STATE_CYCLE_SLIP_BIT)
        out.accumulated_delta_range_state |= GNSS_ADR_STATE_CYCLE_SLIP;
    out.accumulated_delta_range_m = in.adrMeters;
    out.accumulated_delta_range_uncertainty_m = in.adrUncertaintyMeters;
    out.carrier_frequency_hz = in.carrierFrequencyHz;
    out.carrier_cycles = in.carrierCycles;
    out.carrier_phase = in.carrierPhase;
    out.carrier_phase_uncertainty = in.carrierPhaseUncertainty;
    out.multipath_indicator = GNSS_MULTIPATH_INDICATOR_UNKNOWN;
    if (in.multipathIndicator & GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_PRESENT)
        out.multipath_indicator |= GNSS_MULTIPATH_INDICATOR_PRESENT;
    if (in.multipathIndicator & GNSS_MEASUREMENTS_MULTIPATH_INDICATOR_NOT_PRESENT)
        out.multipath_indicator |= GNSS_MULTIPATH_INDICATOR_NOT_PRESENT;
    out.snr_db = in.signalToNoiseRatioDb;
}

static void convertGnssClock(GnssMeasurementsClock& in, GnssClock& out)
{
    memset(&out, 0, sizeof(GnssClock));
    out.size = sizeof(GnssClock);
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_LEAP_SECOND_BIT)
        out.flags |= GNSS_CLOCK_HAS_LEAP_SECOND;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_TIME_UNCERTAINTY_BIT)
        out.flags |= GNSS_CLOCK_HAS_TIME_UNCERTAINTY;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_FULL_BIAS_BIT)
        out.flags |= GNSS_CLOCK_HAS_FULL_BIAS;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_BIT)
        out.flags |= GNSS_CLOCK_HAS_BIAS;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_BIAS_UNCERTAINTY_BIT)
        out.flags |= GNSS_CLOCK_HAS_BIAS_UNCERTAINTY;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_BIT)
        out.flags |= GNSS_CLOCK_HAS_DRIFT;
    if (in.flags & GNSS_MEASUREMENTS_CLOCK_FLAGS_DRIFT_UNCERTAINTY_BIT)
        out.flags |= GNSS_CLOCK_HAS_DRIFT_UNCERTAINTY;
    out.leap_second = in.leapSecond;
    out.time_ns = in.timeNs;
    out.time_uncertainty_ns = in.timeUncertaintyNs;
    out.full_bias_ns = in.fullBiasNs;
    out.bias_ns = in.biasNs;
    out.bias_uncertainty_ns = in.biasUncertaintyNs;
    out.drift_nsps = in.driftNsps;
    out.drift_uncertainty_nsps = in.driftUncertaintyNsps;
    out.hw_clock_discontinuity_count = in.hwClockDiscontinuityCount;
}

static void convertGnssData(GnssMeasurementsNotification& in, GnssData& out)
{
    memset(&out, 0, sizeof(GnssData));
    out.size = sizeof(GnssData);
    out.measurement_count = in.count;
    int len = GNSS_MAX_MEASUREMENT < GNSS_MEASUREMENTS_MAX
        ? GNSS_MAX_MEASUREMENT : GNSS_MEASUREMENTS_MAX;
    for (int i = 0; i < len; i++) {
        convertGnssMeasurement(in.measurements[i], out.measurements[i]);
    }
    convertGnssClock(in.clock, out.clock);
}
