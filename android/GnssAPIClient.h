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

#ifndef GNSS_API_CLINET_H
#define GNSS_API_CLINET_H

#include <hardware/gps.h>

#include <LocationAPIClientBase.h>

class GnssAPIClient : public LocationAPIClientBase
{
public:
    GnssAPIClient(GpsCallbacks* gpsCb,
            GpsNiCallbacks* niCb,
            GpsMeasurementCallbacks* measurementCb);
    virtual ~GnssAPIClient();
    GnssAPIClient(const GnssAPIClient&) = delete;
    GnssAPIClient& operator=(const GnssAPIClient&) = delete;

    // for GpsInterface
    void gnssUpdateCallbacks(GpsCallbacks* gpsCb,
            GpsNiCallbacks* niCb,
            GpsMeasurementCallbacks* measurementCb);
    int gnssStart();
    int gnssStop();
    void gnssDeleteAidingData(GpsAidingData f);
    int gnssSetPositionMode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
            uint32_t min_interval, uint32_t preferred_accuracy,
            uint32_t preferred_time);

    // for AGpsInterface
    void gnssAgnssSetServer(AGpsType type, const char *hostname, int port);

    // for GpsNiInterface
    void gnssNiRespond(int notif_id, GpsUserResponseType user_response);

    // for GpsMeasurementInterface
    void gnssMeasurementClose();

    // for GnssConfigurationInterface
    void gnssConfigurationUpdate(const char* config_data, int32_t length);

    inline LocationCapabilitiesMask gnssGetCapabilities() const {
        return mLocationCapabilitiesMask;
    }

    // callbacks we are interested in
    void onCapabilitiesCb(LocationCapabilitiesMask capabilitiesMask) final;
    void onTrackingCb(Location location) final;
    void onGnssNiCb(uint32_t id, GnssNiNotification gnssNiNotification) final;
    void onGnssSvCb(GnssSvNotification gnssSvNotification) final;
    void onGnssNmeaCb(GnssNmeaNotification gnssNmeaNotification) final;
    void onGnssMeasurementsCb(GnssMeasurementsNotification gnssMeasurementsNotification) final;

    void onStartTrackingCb(LocationError error) final;
    void onStopTrackingCb(LocationError error) final;

private:
    pthread_mutex_t mLock;

    GpsCallbacks* mGpsCallbacks;
    GpsNiCallbacks* mGpsNiCallbacks;

    LocationCapabilitiesMask mLocationCapabilitiesMask;

    GpsMeasurementCallbacks* mGpsMeasurementCallbacks;

    LocationOptions mLocationOptions;
};
#endif // GNSS_API_CLINET_H
