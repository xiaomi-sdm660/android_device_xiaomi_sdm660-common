/*
 * Copyright (c) 2017-2019, The Linux Foundation. All rights reserved.
 * Not a Contribution
 */
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
#include "GnssDummy.h"

namespace android {
namespace hardware {
namespace gnss {
namespace V1_0 {
namespace implementation {

Gnss::Gnss() {
}

Gnss::~Gnss() {
}

Return<bool> Gnss::setCallback(const sp<V1_0::IGnssCallback>& /*callback*/)  {
    return false;
}

Return<bool> Gnss::start()  {
    return false;
}

Return<bool> Gnss::stop()  {
    return false;
}

Return<void> Gnss::cleanup()  {
    return Void();
}

Return<bool> Gnss::injectLocation(double /*latitudeDegrees*/,
                                  double /*longitudeDegrees*/,
                                  float /*accuracyMeters*/)  {
    return false;
}

Return<bool> Gnss::injectTime(int64_t /*timeMs*/, int64_t /*timeReferenceMs*/,
                              int32_t /*uncertaintyMs*/) {
    return false;
}

Return<void> Gnss::deleteAidingData(V1_0::IGnss::GnssAidingData /*aidingDataFlags*/)  {
    return Void();
}

Return<bool> Gnss::setPositionMode(V1_0::IGnss::GnssPositionMode /*mode*/,
                                   V1_0::IGnss::GnssPositionRecurrence /*recurrence*/,
                                   uint32_t /*minIntervalMs*/,
                                   uint32_t /*preferredAccuracyMeters*/,
                                   uint32_t /*preferredTimeMs*/)  {
    return false;
}

Return<sp<V1_0::IAGnss>> Gnss::getExtensionAGnss()  {
    return nullptr;
}

Return<sp<V1_0::IGnssNi>> Gnss::getExtensionGnssNi()  {
    return nullptr;
}

Return<sp<V1_0::IGnssMeasurement>> Gnss::getExtensionGnssMeasurement() {
    return nullptr;
}

Return<sp<V1_0::IGnssConfiguration>> Gnss::getExtensionGnssConfiguration()  {
    return nullptr;
}

Return<sp<V1_0::IGnssGeofencing>> Gnss::getExtensionGnssGeofencing()  {
    return nullptr;
}

Return<sp<V1_0::IGnssBatching>> Gnss::getExtensionGnssBatching()  {
    return nullptr;
}

Return<sp<V1_0::IGnssDebug>> Gnss::getExtensionGnssDebug() {
    return nullptr;
}

Return<sp<V1_0::IAGnssRil>> Gnss::getExtensionAGnssRil() {
    return nullptr;
}

IGnss* HIDL_FETCH_IGnss(const char* /*hal*/) {
    IGnss* iface = nullptr;
    iface = new Gnss();
    return iface;
}

}  // namespace implementation
}  // namespace V1_0
}  // namespace gnss
}  // namespace hardware
}  // namespace android
