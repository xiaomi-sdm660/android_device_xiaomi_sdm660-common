/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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
#ifndef LOC_API_BASE_H
#define LOC_API_BASE_H

#include <stddef.h>
#include <ctype.h>
#include <gps_extended.h>
#include <MsgTask.h>

#define smaller_of(a, b) (((a) > (b)) ? (b) : (a))
#define MAX_APN_LEN 100

// This will be overridden by the individual adapters
// if necessary.
#define DEFAULT_IMPL(rtv)                                     \
{                                                             \
    LOC_LOGW("%s: default implementation invoked", __func__); \
    return rtv;                                               \
}

namespace loc_core {

int hexcode(char *hexstring, int string_size,
            const char *data, int data_size);
int decodeAddress(char *addr_string, int string_size,
                  const char *data, int data_size);

enum loc_api_adapter_err {
    LOC_API_ADAPTER_ERR_SUCCESS             = 0,
    LOC_API_ADAPTER_ERR_GENERAL_FAILURE     = 1,
    LOC_API_ADAPTER_ERR_UNSUPPORTED         = 2,
    LOC_API_ADAPTER_ERR_INVALID_HANDLE      = 4,
    LOC_API_ADAPTER_ERR_INVALID_PARAMETER   = 5,
    LOC_API_ADAPTER_ERR_ENGINE_BUSY         = 6,
    LOC_API_ADAPTER_ERR_PHONE_OFFLINE       = 7,
    LOC_API_ADAPTER_ERR_TIMEOUT             = 8,
    LOC_API_ADAPTER_ERR_SERVICE_NOT_PRESENT = 9,

    LOC_API_ADAPTER_ERR_ENGINE_DOWN         = 100,
    LOC_API_ADAPTER_ERR_FAILURE,
    LOC_API_ADAPTER_ERR_UNKNOWN
};

enum loc_api_adapter_event_index {
    LOC_API_ADAPTER_REPORT_POSITION = 0,       // Position report comes in loc_parsed_position_s_type
    LOC_API_ADAPTER_REPORT_SATELLITE,          // Satellite in view report
    LOC_API_ADAPTER_REPORT_NMEA_1HZ,           // NMEA report at 1HZ rate
    LOC_API_ADAPTER_REPORT_NMEA_POSITION,      // NMEA report at position report rate
    LOC_API_ADAPTER_REQUEST_NI_NOTIFY_VERIFY,  // NI notification/verification request
    LOC_API_ADAPTER_REQUEST_ASSISTANCE_DATA,   // Assistance data, eg: time, predicted orbits request
    LOC_API_ADAPTER_REQUEST_LOCATION_SERVER,   // Request for location server
    LOC_API_ADAPTER_REPORT_IOCTL,              // Callback report for loc_ioctl
    LOC_API_ADAPTER_REPORT_STATUS,             // Misc status report: eg, engine state
    LOC_API_ADAPTER_REQUEST_WIFI,              //
    LOC_API_ADAPTER_SENSOR_STATUS,             //
    LOC_API_ADAPTER_REQUEST_TIME_SYNC,         //
    LOC_API_ADAPTER_REPORT_SPI,                //
    LOC_API_ADAPTER_REPORT_NI_GEOFENCE,        //
    LOC_API_ADAPTER_GEOFENCE_GEN_ALERT,        //
    LOC_API_ADAPTER_REPORT_GENFENCE_BREACH,    //
    LOC_API_ADAPTER_PEDOMETER_CTRL,            //
    LOC_API_ADAPTER_MOTION_CTRL,               //

    LOC_API_ADAPTER_EVENT_MAX
};

#define LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT   (1<<LOC_API_ADAPTER_REPORT_POSITION)
#define LOC_API_ADAPTER_BIT_SATELLITE_REPORT         (1<<LOC_API_ADAPTER_REPORT_SATELLITE)
#define LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT          (1<<LOC_API_ADAPTER_REPORT_NMEA_1HZ)
#define LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT     (1<<LOC_API_ADAPTER_REPORT_NMEA_POSITION)
#define LOC_API_ADAPTER_BIT_NI_NOTIFY_VERIFY_REQUEST (1<<LOC_API_ADAPTER_REQUEST_NI_NOTIFY_VERIFY)
#define LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST  (1<<LOC_API_ADAPTER_REQUEST_ASSISTANCE_DATA)
#define LOC_API_ADAPTER_BIT_LOCATION_SERVER_REQUEST  (1<<LOC_API_ADAPTER_REQUEST_LOCATION_SERVER)
#define LOC_API_ADAPTER_BIT_IOCTL_REPORT             (1<<LOC_API_ADAPTER_REPORT_IOCTL)
#define LOC_API_ADAPTER_BIT_STATUS_REPORT            (1<<LOC_API_ADAPTER_REPORT_STATUS)
#define LOC_API_ADAPTER_BIT_REQUEST_WIFI             (1<<LOC_API_ADAPTER_REQUEST_WIFI)
#define LOC_API_ADAPTER_BIT_SENSOR_STATUS            (1<<LOC_API_ADAPTER_SENSOR_STATUS)
#define LOC_API_ADAPTER_BIT_REQUEST_TIME_SYNC        (1<<LOC_API_ADAPTER_REQUEST_TIME_SYNC)
#define LOC_API_ADAPTER_BIT_REPORT_SPI               (1<<LOC_API_ADAPTER_REPORT_SPI)
#define LOC_API_ADAPTER_BIT_REPORT_NI_GEOFENCE       (1<<LOC_API_ADAPTER_REPORT_NI_GEOFENCE)
#define LOC_API_ADAPTER_BIT_GEOFENCE_GEN_ALERT       (1<<LOC_API_ADAPTER_GEOFENCE_GEN_ALERT)
#define LOC_API_ADAPTER_BIT_REPORT_GENFENCE_BREACH   (1<<LOC_API_ADAPTER_REPORT_GENFENCE_BREACH)
#define LOC_API_ADAPTER_BIT_PEDOMETER_CTRL           (1<<LOC_API_ADAPTER_PEDOMETER_CTRL)
#define LOC_API_ADAPTER_BIT_MOTION_CTRL              (1<<LOC_API_ADAPTER_MOTION_CTRL)

typedef unsigned int LOC_API_ADAPTER_EVENT_MASK_T;
#define MAX_ADAPTERS          10

#define TO_ALL_ADAPTERS(adapters, call)                                \
    for (int i = 0; i < MAX_ADAPTERS && NULL != (adapters)[i]; i++) {  \
        call;                                                          \
    }

#define TO_1ST_HANDLING_ADAPTER(adapters, call)                              \
    for (int i = 0; i <MAX_ADAPTERS && NULL != (adapters)[i] && !(call); i++);


class LocAdapterBase;
struct LocSsrMsg;

class LocApiBase {
    friend struct LocSsrMsg;
    friend class ContextBase;
    const LOC_API_ADAPTER_EVENT_MASK_T mExcludedMask;
    const MsgTask* mMsgTask;

    LocAdapterBase* mLocAdapters[MAX_ADAPTERS];

    static LocApiBase* create(const MsgTask* msgTask,
                              LOC_API_ADAPTER_EVENT_MASK_T exMask,
                              void* libHandle);

protected:
    virtual enum loc_api_adapter_err
        open(LOC_API_ADAPTER_EVENT_MASK_T mask);
    virtual enum loc_api_adapter_err
        close();

    LOC_API_ADAPTER_EVENT_MASK_T getEvtMask();
    LOC_API_ADAPTER_EVENT_MASK_T mMask;
    LocApiBase(const MsgTask* msgTask,
               LOC_API_ADAPTER_EVENT_MASK_T excludedMask);
    inline virtual ~LocApiBase() { close(); }
    bool isInSession();

public:
    inline virtual void* getSibling() { return NULL; }

    void addAdapter(LocAdapterBase* adapter);
    void removeAdapter(LocAdapterBase* adapter);

    // upward calls
    void handleEngineUpEvent();
    void handleEngineDownEvent();
    void reportPosition(UlpLocation &location,
                        GpsLocationExtended &locationExtended,
                        void* locationExt,
                        enum loc_sess_status status,
                        LocPosTechMask loc_technology_mask =
                                  LOC_POS_TECH_MASK_DEFAULT);
    void reportSv(GpsSvStatus &svStatus,
                  GpsLocationExtended &locationExtended,
                  void* svExt);
    void reportStatus(GpsStatusValue status);
    void reportNmea(const char* nmea, int length);
    void reportXtraServer(const char* url1, const char* url2,
                          const char* url3, const int maxlength);
    void requestXtraData();
    void requestTime();
    void requestLocation();
    void requestATL(int connHandle, AGpsType agps_type);
    void releaseATL(int connHandle);
    void requestSuplES(int connHandle);
    void reportDataCallOpened();
    void reportDataCallClosed();
    void requestNiNotify(GpsNiNotification &notify, const void* data);

    // downward calls
    // All below functions are to be defined by adapter specific modules:
    // RPC, QMI, etc.  The default implementation is empty.
    virtual enum loc_api_adapter_err
        startFix(const LocPosMode& posMode);
    virtual enum loc_api_adapter_err
        stopFix();
    virtual enum loc_api_adapter_err
        deleteAidingData(GpsAidingData f);
    virtual enum loc_api_adapter_err
        enableData(int enable);
    virtual enum loc_api_adapter_err
        setAPN(char* apn, int len);
    virtual enum loc_api_adapter_err
        injectPosition(double latitude, double longitude, float accuracy);
    virtual enum loc_api_adapter_err
        setTime(GpsUtcTime time, int64_t timeReference, int uncertainty);
    virtual enum loc_api_adapter_err
        setXtraData(char* data, int length);
    virtual enum loc_api_adapter_err
        requestXtraServer();
    virtual enum loc_api_adapter_err
        atlOpenStatus(int handle, int is_succ, char* apn, AGpsBearerType bear, AGpsType agpsType);
    virtual enum loc_api_adapter_err
        atlCloseStatus(int handle, int is_succ);
    virtual enum loc_api_adapter_err
        setPositionMode(const LocPosMode& posMode);
    virtual enum loc_api_adapter_err
        setServer(const char* url, int len);
    virtual enum loc_api_adapter_err
        setServer(unsigned int ip, int port,
                  LocServerType type);
    virtual enum loc_api_adapter_err
        informNiResponse(GpsUserResponseType userResponse, const void* passThroughData);
    virtual enum loc_api_adapter_err
        setSUPLVersion(uint32_t version);
    virtual enum loc_api_adapter_err
        setLPPConfig(uint32_t profile);
    virtual enum loc_api_adapter_err
        setSensorControlConfig(int sensorUsage);
    virtual enum loc_api_adapter_err
        setSensorProperties(bool gyroBiasVarianceRandomWalk_valid,
                            float gyroBiasVarianceRandomWalk,
                            bool accelBiasVarianceRandomWalk_valid,
                            float accelBiasVarianceRandomWalk,
                            bool angleBiasVarianceRandomWalk_valid,
                            float angleBiasVarianceRandomWalk,
                            bool rateBiasVarianceRandomWalk_valid,
                            float rateBiasVarianceRandomWalk,
                            bool velocityBiasVarianceRandomWalk_valid,
                            float velocityBiasVarianceRandomWalk);
    virtual enum loc_api_adapter_err
        setSensorPerfControlConfig(int controlMode,
                               int accelSamplesPerBatch,
                               int accelBatchesPerSec,
                               int gyroSamplesPerBatch,
                               int gyroBatchesPerSec,
                               int accelSamplesPerBatchHigh,
                               int accelBatchesPerSecHigh,
                               int gyroSamplesPerBatchHigh,
                               int gyroBatchesPerSecHigh,
                               int algorithmConfig);
    virtual enum loc_api_adapter_err
        setExtPowerConfig(int isBatteryCharging);
    virtual enum loc_api_adapter_err
        setAGLONASSProtocol(unsigned long aGlonassProtocol);
    virtual int initDataServiceClient();
    virtual int openAndStartDataCall();
    virtual void stopDataCall();
    virtual void closeDataCall();

    inline virtual void setInSession(bool inSession) {}
};

typedef LocApiBase* (getLocApi_t)(const MsgTask* msgTask,
                                  LOC_API_ADAPTER_EVENT_MASK_T exMask);

} // namespace loc_core

#endif //LOC_API_BASE_H
