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
#define LOG_TAG "LocSvc_FlpAPIClient"

#include <log_util.h>
#include <loc_cfg.h>

#include "LocationUtil.h"
#include "FlpAPIClient.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

static void convertBatchOption(const IGnssBatching::Options& in, LocationOptions& out,
        LocationCapabilitiesMask mask);

FlpAPIClient::FlpAPIClient(const sp<IGnssBatchingCallback>& callback) :
    LocationAPIClientBase(),
    mGnssBatchingCbIface(callback),
    mDefaultId(42),
    mLocationCapabilitiesMask(0)
{
    LOC_LOGD("%s]: (%p)", __FUNCTION__, &callback);

    LocationCallbacks locationCallbacks;
    locationCallbacks.size = sizeof(LocationCallbacks);

    locationCallbacks.trackingCb = nullptr;
    locationCallbacks.batchingCb = nullptr;
    if (mGnssBatchingCbIface != nullptr) {
        locationCallbacks.batchingCb = [this](size_t count, Location* location) {
            onBatchingCb(count, location);
        };
    }
    locationCallbacks.geofenceBreachCb = nullptr;
    locationCallbacks.geofenceStatusCb = nullptr;
    locationCallbacks.gnssLocationInfoCb = nullptr;
    locationCallbacks.gnssNiCb = nullptr;
    locationCallbacks.gnssSvCb = nullptr;
    locationCallbacks.gnssNmeaCb = nullptr;
    locationCallbacks.gnssMeasurementsCb = nullptr;

    locAPISetCallbacks(locationCallbacks);
}

FlpAPIClient::~FlpAPIClient()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
}

int FlpAPIClient::flpGetBatchSize()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
    return locAPIGetBatchSize();
}

int FlpAPIClient::flpStartSession(const IGnssBatching::Options& opts)
{
    LOC_LOGD("%s]: (%lld %d)", __FUNCTION__,
            static_cast<long long>(opts.periodNanos), static_cast<uint8_t>(opts.flags));
    int retVal = -1;
    LocationOptions options;
    convertBatchOption(opts, options, mLocationCapabilitiesMask);
    uint32_t mode = 0;
    if (opts.flags == static_cast<uint8_t>(IGnssBatching::Flag::WAKEUP_ON_FIFO_FULL)) {
        mode = SESSION_MODE_ON_FULL;
    }
    if (locAPIStartSession(mDefaultId, mode, options) == LOCATION_ERROR_SUCCESS) {
        retVal = 1;
    }
    return retVal;
}

int FlpAPIClient::flpUpdateSessionOptions(const IGnssBatching::Options& opts)
{
    LOC_LOGD("%s]: (%lld %d)", __FUNCTION__,
            static_cast<long long>(opts.periodNanos), static_cast<uint8_t>(opts.flags));
    int retVal = -1;
    LocationOptions options;
    convertBatchOption(opts, options, mLocationCapabilitiesMask);

    uint32_t mode = 0;
    if (opts.flags == static_cast<uint8_t>(IGnssBatching::Flag::WAKEUP_ON_FIFO_FULL)) {
        mode = SESSION_MODE_ON_FULL;
    }
    if (locAPIUpdateSessionOptions(mDefaultId, mode, options) == LOCATION_ERROR_SUCCESS) {
        retVal = 1;
    }
    return retVal;
}

int FlpAPIClient::flpStopSession()
{
    LOC_LOGD("%s]: ", __FUNCTION__);
    int retVal = -1;
    if (locAPIStopSession(mDefaultId) == LOCATION_ERROR_SUCCESS) {
        retVal = 1;
    }
    return retVal;
}

void FlpAPIClient::flpGetBatchedLocation(int last_n_locations)
{
    LOC_LOGD("%s]: (%d)", __FUNCTION__, last_n_locations);
    locAPIGetBatchedLocations(last_n_locations);
}

void FlpAPIClient::flpFlushBatchedLocations()
{
    LOC_LOGD("%s]: ()", __FUNCTION__);
    locAPIGetBatchedLocations(SIZE_MAX);
}

void FlpAPIClient::onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask)
{
    LOC_LOGD("%s]: (%02x)", __FUNCTION__, capabilitiesMask);
    mLocationCapabilitiesMask = capabilitiesMask;
}

void FlpAPIClient::onBatchingCb(size_t count, Location* location)
{
    LOC_LOGD("%s]: (count: %zu)", __FUNCTION__, count);
    if (mGnssBatchingCbIface != nullptr && count > 0) {
        hidl_vec<GnssLocation> locationVec;
        locationVec.resize(count);
        for (size_t i = 0; i < count; i++) {
            convertGnssLocation(location[i], locationVec[i]);
        }
        mGnssBatchingCbIface->gnssLocationBatchCb(locationVec);
    }
}

static void convertBatchOption(const IGnssBatching::Options& in, LocationOptions& out,
        LocationCapabilitiesMask mask)
{
    memset(&out, 0, sizeof(LocationOptions));
    out.size = sizeof(LocationOptions);
    out.minInterval = (uint32_t)(in.periodNanos / 1000000L);
    out.minDistance = 0;
    out.mode = GNSS_SUPL_MODE_STANDALONE;
    if (mask & LOCATION_CAPABILITIES_GNSS_MSA_BIT)
        out.mode = GNSS_SUPL_MODE_MSA;
    if (mask & LOCATION_CAPABILITIES_GNSS_MSB_BIT)
        out.mode = GNSS_SUPL_MODE_MSB;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
