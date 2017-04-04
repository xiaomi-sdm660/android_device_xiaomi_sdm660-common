/*
 * Copyright (C) 2016 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "LocSvc_GnssDebugInterface"

#include <log/log.h>
#include <log_util.h>
#include "Gnss.h"
#include "GnssDebug.h"
#include "LocationUtil.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

using ::android::hardware::hidl_vec;

GnssDebug::GnssDebug(Gnss* gnss) : mGnss(gnss)
{
}

/*
 * This methods requests position, time and satellite ephemeris debug information
 * from the HAL.
 *
 * @return void
*/
Return<void> GnssDebug::getDebugData(getDebugData_cb _hidl_cb)
{
    LOC_LOGI("GnssDebug - 0317a");

    DebugData data = { };

    if((nullptr == mGnss) || (nullptr == mGnss->getGnssInterface())){
        LOC_LOGE("GnssDebug - Null GNSS interface");
        _hidl_cb(data);
        return Void();
    }

    // get debug report snapshot via hal interface
    GnssDebugReport reports = { };
    mGnss->getGnssInterface()->getDebugReport(reports);

    // location block
    data.position.valid                    = true;
    data.position.latitudeDegrees          = reports.mLocation.mLocation.latitude;
    data.position.longitudeDegrees         = reports.mLocation.mLocation.longitude;
    data.position.altitudeMeters           = reports.mLocation.mLocation.altitude;
    data.position.speedMetersPerSec        = (double)(reports.mLocation.mLocation.speed);
    data.position.bearingDegrees           = (double)(reports.mLocation.mLocation.bearing);
    data.position.horizontalAccuracyMeters = (double)(reports.mLocation.mLocation.accuracy);
    data.position.verticalAccuracyMeters   = reports.mLocation.verticalAccuracyMeters;
    data.position.speedAccuracyMetersPerSecond = reports.mLocation.speedAccuracyMetersPerSecond;
    data.position.bearingAccuracyDegrees   = reports.mLocation.bearingAccuracyDegrees;

    LOC_LOGV("GnssDebug - lat=%f lon=%f", data.position.latitudeDegrees, data.position.longitudeDegrees);

    timeval tv_now, tv_report;
    tv_report.tv_sec  = reports.mLocation.mLocation.timestamp / 1000ULL;
    tv_report.tv_usec = (reports.mLocation.mLocation.timestamp % 1000ULL) * 1000ULL;
    gettimeofday(&tv_now, NULL);
    data.position.ageSeconds =
        (tv_now.tv_sec - tv_report.tv_sec) + (float)((tv_now.tv_usec - tv_report.tv_usec)) / 1000000;

    LOC_LOGV("GnssDebug - time now=%lld:%lld", tv_now.tv_sec, tv_now.tv_usec);
    LOC_LOGV("GnssDebug - time rep=%lld:%lld", tv_report.tv_sec, tv_report.tv_usec);
    LOC_LOGV("GnssDebug - age=%f", data.position.ageSeconds);

    // time block
    data.time.timeEstimate      = reports.mTime.timeEstimate;
    data.time.timeUncertaintyNs = reports.mTime.timeUncertaintyNs;

    LOC_LOGV("GnssDebug - timeestimate=%lld", data.time.timeEstimate);

    // satellite data block
    SatelliteData s = { };
    std::vector<SatelliteData> s_array = { };

    for (uint32_t i=0; i<reports.mSatelliteInfo.size(); i++) {
        memset(&s, 0, sizeof(s));
        s.svid = reports.mSatelliteInfo[i].svid;
        convertGnssConstellationType(reports.mSatelliteInfo[i].constellation, s.constellation);
        s.ephemerisType = SatelliteEphemerisType::NOT_AVAILABLE;
        s.ephemerisAgeSeconds = reports.mSatelliteInfo[i].ephemerisAgeSeconds;
        s_array.push_back(s);
    }
    data.satelliteDataArray = s_array;
    LOC_LOGV("GnssDebug - satellite=%d", data.satelliteDataArray.size());

    // callback HIDL with collected debug data
    _hidl_cb(data);

    LOC_LOGV("GnssDebug - done");
    return Void();
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
