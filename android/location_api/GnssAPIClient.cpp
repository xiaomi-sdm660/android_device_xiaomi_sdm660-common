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

#include "LocationUtil.h"
#include "GnssAPIClient.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

static void convertGnssSvStatus(GnssSvNotification& in, IGnssCallback::GnssSvStatus& out);

GnssAPIClient::GnssAPIClient(const sp<IGnssCallback>& gpsCb,
    const sp<IGnssNiCallback>& niCb) :
    LocationAPIClientBase(),
    mGnssCbIface(nullptr),
    mGnssNiCbIface(nullptr),
    mLocationCapabilitiesMask(0)
{
    LOC_LOGD("%s]: (%p %p)", __FUNCTION__, &gpsCb, &niCb);

    // set default LocationOptions.
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = 1000;
    mLocationOptions.minDistance = 0;
    mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;

    gnssUpdateCallbacks(gpsCb, niCb);
}

GnssAPIClient::~GnssAPIClient()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
}

// for GpsInterface
void GnssAPIClient::gnssUpdateCallbacks(const sp<IGnssCallback>& gpsCb,
    const sp<IGnssNiCallback>& niCb)
{
    LOC_LOGD("%s]: (%p %p)", __FUNCTION__, &gpsCb, &niCb);

    mGnssCbIface = gpsCb;
    mGnssNiCbIface = niCb;

    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    if (mGnssCbIface != nullptr) {
        locationCallbacks.trackingCb = [this](Location location) {
            onTrackingCb(location);
        };
    }

    locationCallbacks.batchingCb = nullptr;
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;

    locationCallbacks.gnssNiCb = nullptr;
    if (mGnssNiCbIface != nullptr) {
        locationCallbacks.gnssNiCb = [this](uint32_t id, GnssNiNotification gnssNiNotification) {
            onGnssNiCb(id, gnssNiNotification);
        };
    }

    locationCallbacks.gnssSvCb = nullptr;
    if (mGnssCbIface != nullptr) {
        locationCallbacks.gnssSvCb = [this](GnssSvNotification gnssSvNotification) {
            onGnssSvCb(gnssSvNotification);
        };
    }

    locationCallbacks.gnssNmeaCb = nullptr;
    if (mGnssCbIface != nullptr) {
        locationCallbacks.gnssNmeaCb = [this](GnssNmeaNotification gnssNmeaNotification) {
            onGnssNmeaCb(gnssNmeaNotification);
        };
    }

    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

bool GnssAPIClient::gnssStart()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
    bool retVal = true;
    locAPIStartTracking(mLocationOptions);
    return retVal;
}

bool GnssAPIClient::gnssStop()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
    bool retVal = true;
    locAPIStopTracking();
    return retVal;
}

void GnssAPIClient::gnssDeleteAidingData(IGnss::GnssAidingData aidingDataFlags)
{
    LOC_LOGD("%s]: (%02hx)", __FUNCTION__, aidingDataFlags);
    GnssAidingData data;
    memset(&data, 0, sizeof (GnssAidingData));
    data.sv.svTypeMask = GNSS_AIDING_DATA_SV_TYPE_GPS |
        GNSS_AIDING_DATA_SV_TYPE_GLONASS |
        GNSS_AIDING_DATA_SV_TYPE_QZSS |
        GNSS_AIDING_DATA_SV_TYPE_BEIDOU |
        GNSS_AIDING_DATA_SV_TYPE_GALILEO;

    if (aidingDataFlags == IGnss::GnssAidingData::DELETE_ALL)
        data.deleteAll = true;
    else {
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_EPHEMERIS)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_EPHEMERIS;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_ALMANAC)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_ALMANAC;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_POSITION)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_POSITION;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_TIME)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_TIME;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_IONO)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_IONOSPHERE;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_UTC)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_UTC;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_HEALTH)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_HEALTH;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_SVDIR)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_DIRECTION;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_SVSTEER)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_STEER;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_SADATA)
            data.sv.svMask |= GNSS_AIDING_DATA_SV_SA_DATA;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_RTI)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_RTI;
        if (aidingDataFlags & IGnss::GnssAidingData::DELETE_CELLDB_INFO)
            data.common.mask |= GNSS_AIDING_DATA_COMMON_CELLDB;
    }
    locAPIGnssDeleteAidingData(data);
}

bool GnssAPIClient::gnssSetPositionMode(IGnss::GnssPositionMode mode,
        IGnss::GnssPositionRecurrence recurrence, uint32_t minIntervalMs,
        uint32_t preferredAccuracyMeters, uint32_t preferredTimeMs)
{
    LOC_LOGD("%s]: (%d %d %d %d %d)", __FUNCTION__,
            (int)mode, recurrence, minIntervalMs, preferredAccuracyMeters, preferredTimeMs);
    bool retVal = true;
    memset(&mLocationOptions, 0, sizeof(LocationOptions));
    mLocationOptions.size = sizeof(LocationOptions);
    mLocationOptions.minInterval = minIntervalMs;
    mLocationOptions.minDistance = preferredAccuracyMeters;
    if (mode == IGnss::GnssPositionMode::STANDALONE)
        mLocationOptions.mode = GNSS_SUPL_MODE_STANDALONE;
    else if (mode == IGnss::GnssPositionMode::MS_BASED)
        mLocationOptions.mode = GNSS_SUPL_MODE_MSB;
    else if (mode ==  IGnss::GnssPositionMode::MS_ASSISTED)
        mLocationOptions.mode = GNSS_SUPL_MODE_MSA;
    return retVal;
}

// for GpsNiInterface
void GnssAPIClient::gnssNiRespond(int32_t notifId,
        IGnssNiCallback::GnssUserResponseType userResponse)
{
    LOC_LOGD("%s]: (%d %d)", __FUNCTION__, notifId, static_cast<int>(userResponse));
    GnssNiResponse data = GNSS_NI_RESPONSE_IGNORE;
    if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT)
        data = GNSS_NI_RESPONSE_ACCEPT;
    else if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY)
        data = GNSS_NI_RESPONSE_DENY;
    else if (userResponse == IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP)
        data = GNSS_NI_RESPONSE_NO_RESPONSE;
    locAPIGnssNiResponse(notifId, data);
}

// for GnssConfigurationInterface
void GnssAPIClient::gnssConfigurationUpdate(const GnssConfig& gnssConfig)
{
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, gnssConfig.flags);
    locAPIGnssUpdateConfig(gnssConfig);
}

// callbacks
void GnssAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask)
{
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
    if (mGnssCbIface != nullptr) {
        uint32_t data = 0;
        if ((capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_TRACKING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_TIME_BASED_BATCHING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_TRACKING_BIT) ||
                (capabilitiesMask & LOCATION_CAPABILITIES_DISTANCE_BASED_BATCHING_BIT))
            data |= IGnssCallback::Capabilities::SCHEDULING;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GEOFENCE_BIT)
            data |= IGnssCallback::Capabilities::GEOFENCING;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT)
            data |= IGnssCallback::Capabilities::MEASUREMENTS;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
            data |= IGnssCallback::Capabilities::MSB;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
            data |= IGnssCallback::Capabilities::MSA;
        mGnssCbIface->gnssSetCapabilitesCb(data);
    }
    if (mGnssCbIface != nullptr) {
        IGnssCallback::GnssSystemInfo gnssInfo;
        gnssInfo.yearOfHw = 2015;
        if (capabilitiesMask & LOCATION_CAPABILITIES_GNSS_MEASUREMENTS_BIT) {
            gnssInfo.yearOfHw = 2017;
        }
        LOC_LOGV("%s:%d] set_system_info_cb (%d)", __FUNCTION__, __LINE__, gnssInfo.yearOfHw);
        mGnssCbIface->gnssSetSystemInfoCb(gnssInfo);
    }
}

void GnssAPIClient::onTrackingCb(Location location)
{
    LOC_LOGD("%s]: (flags: %02x)", __FUNCTION__, location.flags);
    if (mGnssCbIface != nullptr) {
        GnssLocation gnssLocation;
        convertGnssLocation(location, gnssLocation);
        mGnssCbIface->gnssLocationCb(gnssLocation);
    }
}

void GnssAPIClient::onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification)
{
    LOC_LOGD("%s]: (id: %d)", __FUNCTION__, id);

    if (mGnssNiCbIface == nullptr) {
        LOC_LOGE("%s]: mGnssNiCbIface is nullptr", __FUNCTION__);
        return;
    }

    IGnssNiCallback::GnssNiNotification notificationGnss;

    notificationGnss.notificationId = id;

    if (gnssNiNotification.type == GNSS_NI_TYPE_VOICE)
        notificationGnss.niType = IGnssNiCallback::GnssNiType::VOICE;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_SUPL)
        notificationGnss.niType = IGnssNiCallback::GnssNiType::UMTS_SUPL;
    else if (gnssNiNotification.type == GNSS_NI_TYPE_CONTROL_PLANE)
        notificationGnss.niType = IGnssNiCallback::GnssNiType::UMTS_CTRL_PLANE;
    // GNSS_NI_TYPE_EMERGENCY_SUPL not supported

    if (gnssNiNotification.options == GNSS_NI_OPTIONS_NOTIFICATION)
        notificationGnss.notifyFlags =
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_NOTIFY);
    else if (gnssNiNotification.options == GNSS_NI_OPTIONS_VERIFICATION)
        notificationGnss.notifyFlags =
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::NEED_VERIFY);
    else if (gnssNiNotification.options == GNSS_NI_OPTIONS_PRIVACY_OVERRIDE)
        notificationGnss.notifyFlags =
            static_cast<uint32_t>(IGnssNiCallback::GnssNiNotifyFlags::PRIVACY_OVERRIDE);

    notificationGnss.timeoutSec = gnssNiNotification.timeout;

    if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_ACCEPT)
        notificationGnss.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_ACCEPT;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_DENY)
        notificationGnss.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_DENY;
    else if (gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_NO_RESPONSE ||
            gnssNiNotification.timeoutResponse == GNSS_NI_RESPONSE_IGNORE)
        notificationGnss.defaultResponse = IGnssNiCallback::GnssUserResponseType::RESPONSE_NORESP;

    notificationGnss.requestorId = gnssNiNotification.requestor;

    notificationGnss.notificationMessage = gnssNiNotification.message;

    if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        notificationGnss.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        notificationGnss.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        notificationGnss.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.requestorEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        notificationGnss.requestorIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2;

    if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_NONE)
        notificationGnss.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_NONE;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_GSM_DEFAULT)
        notificationGnss.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_GSM_DEFAULT;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UTF8)
        notificationGnss.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UTF8;
    else if (gnssNiNotification.messageEncoding == GNSS_NI_ENCODING_TYPE_UCS2)
        notificationGnss.notificationIdEncoding =
            IGnssNiCallback::GnssNiEncodingType::ENC_SUPL_UCS2;

    mGnssNiCbIface->niNotifyCb(notificationGnss);
}

void GnssAPIClient::onGnssSvCb(GnssSvNotification gnssSvNotification)
{
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, gnssSvNotification.count);
    if (mGnssCbIface != nullptr) {
        IGnssCallback::GnssSvStatus svStatus;
        convertGnssSvStatus(gnssSvNotification, svStatus);
        mGnssCbIface->gnssSvStatusCb(svStatus);
    }
}

void GnssAPIClient::onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification)
{
    if (mGnssCbIface != nullptr) {
        android::hardware::hidl_string nmeaString;
        nmeaString.setToExternal(gnssNmeaNotification.nmea, gnssNmeaNotification.length);
        mGnssCbIface->gnssNmeaCb(static_cast<GnssUtcTime>(gnssNmeaNotification.timestamp),
                nmeaString);
    }
}

void GnssAPIClient::onStartTrackingCb(LocationError error)
{
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    if (error == LOCATION_ERROR_SUCCESS && mGnssCbIface != nullptr) {
        mGnssCbIface->gnssStatusCb(IGnssCallback::GnssStatusValue::ENGINE_ON);
        mGnssCbIface->gnssStatusCb(IGnssCallback::GnssStatusValue::SESSION_BEGIN);
    }
}

void GnssAPIClient::onStopTrackingCb(LocationError error)
{
    LOC_LOGD("%s]: (%d)", __FUNCTION__, error);
    if (error == LOCATION_ERROR_SUCCESS && mGnssCbIface != nullptr) {
        mGnssCbIface->gnssStatusCb(IGnssCallback::GnssStatusValue::SESSION_END);
        mGnssCbIface->gnssStatusCb(IGnssCallback::GnssStatusValue::ENGINE_ON);
    }
}

static void convertGnssSvStatus(GnssSvNotification& in, IGnssCallback::GnssSvStatus& out)
{
    memset(&out, 0, sizeof(IGnssCallback::GnssSvStatus));
    out.numSvs = in.count;
    if (out.numSvs > static_cast<uint32_t>(GnssMax::SVS_COUNT)) {
        LOC_LOGW("%s]: Too many satellites %zd. Clamps to %d.",
                __FUNCTION__,  out.numSvs, GnssMax::SVS_COUNT);
        out.numSvs = static_cast<uint32_t>(GnssMax::SVS_COUNT);
    }
    for (size_t i = 0; i < out.numSvs; i++) {
        IGnssCallback::GnssSvInfo& info = out.gnssSvList[i];
        info.svid = in.gnssSvs[i].svId;
        convertGnssConstellationType(in.gnssSvs[i].type, info.constellation);
        info.cN0Dbhz = in.gnssSvs[i].cN0Dbhz;
        info.elevationDegrees = in.gnssSvs[i].elevation;
        info.azimuthDegrees = in.gnssSvs[i].azimuth;
        info.svFlag = static_cast<uint8_t>(IGnssCallback::GnssSvFlags::NONE);
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_EPHEMER_BIT)
            info.svFlag |= IGnssCallback::GnssSvFlags::HAS_EPHEMERIS_DATA;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_HAS_ALMANAC_BIT)
            info.svFlag |= IGnssCallback::GnssSvFlags::HAS_ALMANAC_DATA;
        if (in.gnssSvs[i].gnssSvOptionsMask & GNSS_SV_OPTIONS_USED_IN_FIX_BIT)
            info.svFlag |= IGnssCallback::GnssSvFlags::USED_IN_FIX;
    }
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
