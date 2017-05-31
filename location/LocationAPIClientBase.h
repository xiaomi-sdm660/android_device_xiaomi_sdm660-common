/* Copyright (c) 2017 The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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
 */

#ifndef LOCATION_API_CLINET_BASE_H
#define LOCATION_API_CLINET_BASE_H

#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <queue>
#include <map>

#include "LocationAPI.h"

enum SESSION_MODE {
    SESSION_MODE_NONE = 0,
    SESSION_MODE_ON_FULL,
    SESSION_MODE_ON_FIX,
};

enum REQUEST_TYPE {
    REQUEST_TRACKING = 0,
    REQUEST_BATCHING,
    REQUEST_GEOFENCE,
    REQUEST_NIRESPONSE,
    REQUEST_DELETEAIDINGDATA,
    REQUEST_CONTROL,
    REQUEST_CONFIG,
    REQUEST_MAX,
};

class LocationAPIClientBase
{
public:
    LocationAPIClientBase();
    virtual ~LocationAPIClientBase();
    LocationAPIClientBase(const LocationAPIClientBase&) = delete;
    LocationAPIClientBase& operator=(const LocationAPIClientBase&) = delete;

    void locAPISetCallbacks(LocationCallbacks& locationCallbacks);

    // LocationAPI
    uint32_t locAPIStartTracking(LocationOptions& options);
    void locAPIStopTracking();
    void locAPIUpdateTrackingOptions(LocationOptions& options);

    int32_t locAPIGetBatchSize();
    uint32_t locAPIStartSession(uint32_t id, uint32_t sessionMode,
            LocationOptions& options);
    uint32_t locAPIStopSession(uint32_t id);
    uint32_t locAPIUpdateSessionOptions(uint32_t id, uint32_t sessionMode,
            LocationOptions& options);
    void locAPIGetBatchedLocations(size_t count);

    uint32_t locAPIAddGeofences(size_t count, uint32_t* ids,
            GeofenceOption* options, GeofenceInfo* data);
    void locAPIRemoveGeofences(size_t count, uint32_t* ids);
    void locAPIModifyGeofences(size_t count, uint32_t* ids, GeofenceOption* options);
    void locAPIPauseGeofences(size_t count, uint32_t* ids);
    void locAPIResumeGeofences(size_t count, uint32_t* ids, GeofenceBreachTypeMask* mask);
    void locAPIRemoveAllGeofences();

    void locAPIGnssNiResponse(uint32_t id, GnssNiResponse response);
    uint32_t locAPIGnssDeleteAidingData(GnssAidingData& data);

    uint32_t locAPIEnable(LocationTechnologyType techType);
    void locAPIDisable();
    uint32_t locAPIGnssUpdateConfig(GnssConfig config);

    // callbacks
    void onResponseCb(LocationError error, uint32_t id);
    void onCollectiveResponseCb(size_t count, LocationError* errors, uint32_t* ids);

    void onCtrlResponseCb(LocationError error, uint32_t id);
    void onCtrlCollectiveResponseCb(size_t count, LocationError* errors, uint32_t* ids);

    void beforeGeofenceBreachCb(GeofenceBreachNotification geofenceBreachNotification);

    inline virtual void onCapabilitiesCb(LocationCapabilitiesMask /*capabilitiesMask*/) {}
    inline virtual void onGnssNmeaCb(GnssNmeaNotification /*gnssNmeaNotification*/) {}
    inline virtual void onGnssMeasurementsCb(
            GnssMeasurementsNotification /*gnssMeasurementsNotification*/) {}

    inline virtual void onTrackingCb(Location /*location*/) {}
    inline virtual void onGnssSvCb(GnssSvNotification /*gnssSvNotification*/) {}
    inline virtual void onStartTrackingCb(LocationError /*error*/) {}
    inline virtual void onStopTrackingCb(LocationError /*error*/) {}
    inline virtual void onUpdateTrackingOptionsCb(LocationError /*error*/) {}

    inline virtual void onGnssLocationInfoCb(
            GnssLocationInfoNotification /*gnssLocationInfoNotification*/) {}

    inline virtual void onBatchingCb(size_t /*count*/, Location* /*location*/) {}
    inline virtual void onStartBatchingCb(LocationError /*error*/) {}
    inline virtual void onStopBatchingCb(LocationError /*error*/) {}
    inline virtual void onUpdateBatchingOptionsCb(LocationError /*error*/) {}
    inline virtual void onGetBatchedLocationsCb(LocationError /*error*/) {}

    inline virtual void onGeofenceBreachCb(
            GeofenceBreachNotification /*geofenceBreachNotification*/) {}
    inline virtual void onGeofenceStatusCb(
            GeofenceStatusNotification /*geofenceStatusNotification*/) {}
    inline virtual void onAddGeofencesCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}
    inline virtual void onRemoveGeofencesCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}
    inline virtual void onModifyGeofencesCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}
    inline virtual void onPauseGeofencesCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}
    inline virtual void onResumeGeofencesCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}

    inline virtual void onGnssNiCb(uint32_t /*id*/, GnssNiNotification /*gnssNiNotification*/) {}
    inline virtual void onGnssNiResponseCb(LocationError /*error*/) {}
    inline virtual void onGnssDeleteAidingDataCb(LocationError /*error*/) {}

    inline virtual void onEnableCb(LocationError /*error*/) {}
    inline virtual void onDisableCb(LocationError /*error*/) {}
    inline virtual void onGnssUpdateConfigCb(
            size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {}

private:
    // private inner classes
    typedef struct {
        uint32_t id;
        uint32_t trackingSession;
        uint32_t batchingSession;
        uint32_t sessionMode;
    } SessionEntity;

    class BiDict {
    public:
        BiDict() {
            pthread_mutex_init(&mBiDictMutex, nullptr);
        }
        ~BiDict() {
            pthread_mutex_destroy(&mBiDictMutex);
        }
        bool hasId(uint32_t id) {
            pthread_mutex_lock(&mBiDictMutex);
            bool ret = (mForwardMap.find(id) != mForwardMap.end());
            pthread_mutex_unlock(&mBiDictMutex);
            return ret;
        }
        void set(uint32_t id, uint32_t session, uint32_t type) {
            pthread_mutex_lock(&mBiDictMutex);
            mForwardMap[id] = session;
            mBackwardMap[session] = id;
            mTypeMap[session] = type;
            pthread_mutex_unlock(&mBiDictMutex);
        }
        void clear() {
            pthread_mutex_lock(&mBiDictMutex);
            mForwardMap.clear();
            mBackwardMap.clear();
            mTypeMap.clear();
            pthread_mutex_unlock(&mBiDictMutex);
        }
        void rmById(uint32_t id) {
            pthread_mutex_lock(&mBiDictMutex);
            mBackwardMap.erase(mForwardMap[id]);
            mTypeMap.erase(mForwardMap[id]);
            mForwardMap.erase(id);
            pthread_mutex_unlock(&mBiDictMutex);
        }
        void rmBySession(uint32_t session) {
            pthread_mutex_lock(&mBiDictMutex);
            mForwardMap.erase(mBackwardMap[session]);
            mBackwardMap.erase(session);
            mTypeMap.erase(session);
            pthread_mutex_unlock(&mBiDictMutex);
        }
        uint32_t getId(uint32_t session) {
            pthread_mutex_lock(&mBiDictMutex);
            uint32_t ret = 0;
            auto it = mBackwardMap.find(session);
            if (it != mBackwardMap.end()) {
                ret = it->second;
            }
            pthread_mutex_unlock(&mBiDictMutex);
            return ret;
        }
        uint32_t getSession(uint32_t id) {
            pthread_mutex_lock(&mBiDictMutex);
            uint32_t ret = 0;
            auto it = mForwardMap.find(id);
            if (it != mForwardMap.end()) {
                ret = it->second;
            }
            pthread_mutex_unlock(&mBiDictMutex);
            return ret;
        }
        uint32_t getType(uint32_t session) {
            pthread_mutex_lock(&mBiDictMutex);
            uint32_t ret = 0;
            auto it = mTypeMap.find(session);
            if (it != mTypeMap.end()) {
                ret = it->second;
            }
            pthread_mutex_unlock(&mBiDictMutex);
            return ret;
        }
        std::vector<uint32_t> getAllSessions() {
            std::vector<uint32_t> ret;
            pthread_mutex_lock(&mBiDictMutex);
            for (auto it = mBackwardMap.begin(); it != mBackwardMap.end(); it++) {
                ret.push_back(it->first);
            }
            pthread_mutex_unlock(&mBiDictMutex);
            return ret;
        }
    private:
        pthread_mutex_t mBiDictMutex;
        // mForwarMap mapping id->session
        std::map<uint32_t, uint32_t> mForwardMap;
        // mBackwardMap mapping session->id
        std::map<uint32_t, uint32_t> mBackwardMap;
        // mTypeMap mapping session->type
        std::map<uint32_t, uint32_t> mTypeMap;
    };

    class LocationAPIRequest {
    public:
        LocationAPIRequest(LocationAPIClientBase& API) : mAPI(API) {}
        virtual ~LocationAPIRequest() {}
        virtual void onResponse(LocationError /*error*/) {};
        virtual void onCollectiveResponse(
                size_t /*count*/, LocationError* /*errors*/, uint32_t* /*ids*/) {};
        LocationAPIClientBase& mAPI;
    };

    class StartTrackingRequest : public LocationAPIRequest {
    public:
        StartTrackingRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onStartTrackingCb(error);
        }
    };

    class StopTrackingRequest : public LocationAPIRequest {
    public:
        StopTrackingRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onStopTrackingCb(error);
        }
    };

    class UpdateTrackingOptionsRequest : public LocationAPIRequest {
    public:
        UpdateTrackingOptionsRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onUpdateTrackingOptionsCb(error);
        }
    };

    class StartBatchingRequest : public LocationAPIRequest {
    public:
        StartBatchingRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onStartBatchingCb(error);
        }
    };

    class StopBatchingRequest : public LocationAPIRequest {
    public:
        StopBatchingRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onStopBatchingCb(error);
        }
    };

    class UpdateBatchingOptionsRequest : public LocationAPIRequest {
    public:
        UpdateBatchingOptionsRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onUpdateBatchingOptionsCb(error);
        }
    };

    class GetBatchedLocationsRequest : public LocationAPIRequest {
    public:
        GetBatchedLocationsRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onGetBatchedLocationsCb(error);
        }
    };

    class AddGeofencesRequest : public LocationAPIRequest {
    public:
        AddGeofencesRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* sessions) {
            uint32_t *ids = (uint32_t*)malloc(sizeof(uint32_t) * count);
            for (size_t i = 0; i < count; i++) {
                ids[i] = mAPI.mGeofenceBiDict.getId(sessions[i]);
            }
            mAPI.onAddGeofencesCb(count, errors, ids);
            free(ids);
        }
    };

    class RemoveGeofencesRequest : public LocationAPIRequest {
    public:
        RemoveGeofencesRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* sessions) {
            uint32_t *ids = (uint32_t*)malloc(sizeof(uint32_t) * count);
            for (size_t i = 0; i < count; i++) {
                ids[i] = mAPI.mGeofenceBiDict.getId(sessions[i]);
                mAPI.mGeofenceBiDict.rmBySession(sessions[i]);
            }
            mAPI.onRemoveGeofencesCb(count, errors, ids);
            free(ids);
        }
    };

    class ModifyGeofencesRequest : public LocationAPIRequest {
    public:
        ModifyGeofencesRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* sessions) {
            uint32_t *ids = (uint32_t*)malloc(sizeof(uint32_t) * count);
            for (size_t i = 0; i < count; i++) {
                ids[i] = mAPI.mGeofenceBiDict.getId(sessions[i]);
            }
            mAPI.onModifyGeofencesCb(count, errors, ids);
            free(ids);
        }
    };

    class PauseGeofencesRequest : public LocationAPIRequest {
    public:
        PauseGeofencesRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* sessions) {
            uint32_t *ids = (uint32_t*)malloc(sizeof(uint32_t) * count);
            for (size_t i = 0; i < count; i++) {
                ids[i] = mAPI.mGeofenceBiDict.getId(sessions[i]);
            }
            mAPI.onPauseGeofencesCb(count, errors, ids);
            free(ids);
        }
    };

    class ResumeGeofencesRequest : public LocationAPIRequest {
    public:
        ResumeGeofencesRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* sessions) {
            uint32_t *ids = (uint32_t*)malloc(sizeof(uint32_t) * count);
            for (size_t i = 0; i < count; i++) {
                ids[i] = mAPI.mGeofenceBiDict.getId(sessions[i]);
            }
            mAPI.onResumeGeofencesCb(count, errors, ids);
            free(ids);
        }
    };

    class GnssNiResponseRequest : public LocationAPIRequest {
    public:
        GnssNiResponseRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onGnssNiResponseCb(error);
        }
    };

    class GnssDeleteAidingDataRequest : public LocationAPIRequest {
    public:
        GnssDeleteAidingDataRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onGnssDeleteAidingDataCb(error);
        }
    };

    class EnableRequest : public LocationAPIRequest {
    public:
        EnableRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onEnableCb(error);
        }
    };

    class DisableRequest : public LocationAPIRequest {
    public:
        DisableRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onResponse(LocationError error) {
            mAPI.onDisableCb(error);
        }
    };

    class GnssUpdateConfigRequest : public LocationAPIRequest {
    public:
        GnssUpdateConfigRequest(LocationAPIClientBase& API) : LocationAPIRequest(API) {}
        inline void onCollectiveResponse(size_t count, LocationError* errors, uint32_t* ids) {
            mAPI.onGnssUpdateConfigCb(count, errors, ids);
        }
    };

    class RequestQueue {
    public:
        RequestQueue(uint32_t session): mSession(session) {
        }
        ~RequestQueue() {
            LocationAPIRequest* request = nullptr;
            while (!mQueue.empty()) {
                request = mQueue.front();
                mQueue.pop();
                delete request;
            }
        }
        void push(LocationAPIRequest* request) {
            mQueue.push(request);
        }
        LocationAPIRequest* pop() {
            LocationAPIRequest* request = nullptr;
            if (!mQueue.empty()) {
                request = mQueue.front();
                mQueue.pop();
            }
            return request;
        }
        uint32_t getSession() { return mSession; }
    private:
        uint32_t mSession;
        std::queue<LocationAPIRequest*> mQueue;
    };

    LocationAPIRequest* getRequestBySession(uint32_t session);

private:
    pthread_mutex_t mMutex;

    trackingCallback mTrackingCallback;
    batchingCallback mBatchingCallback;
    geofenceBreachCallback mGeofenceBreachCallback;

    LocationAPI* mLocationAPI;
    LocationControlAPI* mLocationControlAPI;

    BiDict mGeofenceBiDict;
    RequestQueue* mRequestQueues[REQUEST_MAX];
    std::map<uint32_t, SessionEntity> mSessionMap;
    int32_t mBatchSize;
    bool mEnabled;

    GnssConfig mConfig;
};

#endif /* LOCATION_API_CLINET_BASE_H */
