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

#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_APIClientBase"

#include <log_util.h>
#include <loc_cfg.h>
#include "LocationAPIClientBase.h"

#define BATCHING_CONF_FILE "/etc/flp.conf"
#define GEOFENCE_SESSION_ID -1

LocationAPIClientBase::LocationAPIClientBase() :
    mTrackingCallback(nullptr),
    mBatchingCallback(nullptr),
    mGeofenceBreachCallback(nullptr),
    mLocationAPI(nullptr),
    mLocationControlAPI(nullptr),
    mBatchSize(-1),
    mEnabled(false)
{

    // use recursive mutex, in case callback come from the same thread
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mMutex, &attr);

    for (int i = 0; i < REQUEST_MAX; i++) {
        mRequestQueues[i] = nullptr;
    }

    memset(&mConfig, 0, sizeof(GnssConfig));
}

void LocationAPIClientBase::locAPISetCallbacks(LocationCallbacks& locationCallbacks)
{
    pthread_mutex_lock(&mMutex);

    if (locationCallbacks.geofenceBreachCb != nullptr) {
        mGeofenceBreachCallback = locationCallbacks.geofenceBreachCb;
        locationCallbacks.geofenceBreachCb =
            [this](GeofenceBreachNotification geofenceBreachNotification) {
                beforeGeofenceBreachCb(geofenceBreachNotification);
            };
    }

    locationCallbacks.capabilitiesCb =
        [this](LocationCapabilitiesMask capabilitiesMask) {
            onCapabilitiesCb(capabilitiesMask);
        };
    locationCallbacks.responseCb = [this](LocationError error, uint32_t id) {
        onResponseCb(error, id);
    };
    locationCallbacks.collectiveResponseCb =
        [this](size_t count, LocationError* errors, uint32_t* ids) {
            onCollectiveResponseCb(count, errors, ids);
        };

    if (mLocationAPI == nullptr ) {
        mLocationAPI = LocationAPI::createInstance(locationCallbacks);
    } else {
        mLocationAPI->updateCallbacks(locationCallbacks);
    }

    if (mLocationControlAPI == nullptr) {
        LocationControlCallbacks locationControlCallbacks;
        locationControlCallbacks.size = sizeof(LocationControlCallbacks);

        locationControlCallbacks.responseCb =
            [this](LocationError error, uint32_t id) {
                onCtrlResponseCb(error, id);
            };
        locationControlCallbacks.collectiveResponseCb =
            [this](size_t count, LocationError* errors, uint32_t* ids) {
                onCtrlCollectiveResponseCb(count, errors, ids);
            };

        mLocationControlAPI = LocationControlAPI::createInstance(locationControlCallbacks);
    }

    pthread_mutex_unlock(&mMutex);
}

LocationAPIClientBase::~LocationAPIClientBase()
{
    pthread_mutex_lock(&mMutex);

    if (mLocationAPI) {
        mLocationAPI->destroy();
        mLocationAPI = nullptr;
    }
    if (mLocationControlAPI) {
        mLocationControlAPI->destroy();
        mLocationControlAPI = nullptr;
    }

    for (int i = 0; i < REQUEST_MAX; i++) {
        if (mRequestQueues[i]) {
            delete mRequestQueues[i];
            mRequestQueues[i] = nullptr;
        }
    }
    pthread_mutex_unlock(&mMutex);

    pthread_mutex_destroy(&mMutex);
}

uint32_t LocationAPIClientBase::locAPIStartTracking(LocationOptions& options)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
        if (requests) {
            delete requests;
            mRequestQueues[REQUEST_TRACKING] = nullptr;
        }
        uint32_t session = mLocationAPI->startTracking(options);
        LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, session);
        // onResponseCb might be called from other thread immediately after
        // startTracking returns, so we are not going to unlock mutex
        // until StartTrackingRequest is pushed into mRequestQueues[REQUEST_TRACKING]
        requests = new RequestQueue(session);
        requests->push(new StartTrackingRequest(*this));
        mRequestQueues[REQUEST_TRACKING] = requests;

        retVal = LOCATION_ERROR_SUCCESS;
    }
    pthread_mutex_unlock(&mMutex);

    return retVal;
}

void LocationAPIClientBase::locAPIStopTracking()
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t session = 0;
        RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
        if (requests) {
            session = requests->getSession();
            if (session > 0) {
                requests->push(new StopTrackingRequest(*this));
                mLocationAPI->stopTracking(session);
            }
        }
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIUpdateTrackingOptions(LocationOptions& options)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t session = 0;
        RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
        if (requests) {
            session = requests->getSession();
            if (session > 0) {
                requests->push(new UpdateTrackingOptionsRequest(*this));
                mLocationAPI->updateTrackingOptions(session, options);
            }
        }
    }
    pthread_mutex_unlock(&mMutex);
}

int32_t LocationAPIClientBase::locAPIGetBatchSize()
{
    if (mBatchSize == -1) {
        const loc_param_s_type batching_conf_param_table[] =
        {
            {"BATCH_SIZE", &mBatchSize, nullptr, 'n'},
        };
        UTIL_READ_CONF(BATCHING_CONF_FILE, batching_conf_param_table);
        if (mBatchSize < 0) {
            // set mBatchSize to 0 if we got an illegal value from config file
            mBatchSize = 0;
        }
    }
    return mBatchSize;
}


uint32_t LocationAPIClientBase::locAPIStartSession(uint32_t id, uint32_t sessionMode,
        LocationOptions& options)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {

        if (mSessionMap.find(id) != mSessionMap.end()) {
            LOC_LOGE("%s:%d] session %d has already started.", __FUNCTION__, __LINE__, id);
            retVal = LOCATION_ERROR_ALREADY_STARTED;
        } else {
            uint32_t trackingSession = 0;
            uint32_t batchingSession = 0;

            if (sessionMode == SESSION_MODE_ON_FIX) {
                RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
                if (requests) {
                    delete requests;
                    mRequestQueues[REQUEST_TRACKING] = nullptr;
                }
                trackingSession = mLocationAPI->startTracking(options);
                LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, trackingSession);
                requests = new RequestQueue(trackingSession);
                requests->push(new StartTrackingRequest(*this));
                mRequestQueues[REQUEST_TRACKING] = requests;
            } else if (sessionMode == SESSION_MODE_ON_FULL) {
                RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
                if (requests) {
                    delete requests;
                    mRequestQueues[REQUEST_BATCHING] = nullptr;
                }
                batchingSession = mLocationAPI->startBatching(options);
                LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, batchingSession);
                requests = new RequestQueue(batchingSession);
                requests->push(new StartBatchingRequest(*this));
                mRequestQueues[REQUEST_BATCHING] = requests;
            }

            SessionEntity entity;
            entity.id = id;
            entity.trackingSession = trackingSession;
            entity.batchingSession = batchingSession;
            entity.sessionMode = sessionMode;
            mSessionMap[id] = entity;

            retVal = LOCATION_ERROR_SUCCESS;
        }

    }
    pthread_mutex_unlock(&mMutex);

    return retVal;
}

uint32_t LocationAPIClientBase::locAPIStopSession(uint32_t id)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {

        if (mSessionMap.find(id) != mSessionMap.end()) {
            SessionEntity entity = mSessionMap[id];

            uint32_t trackingSession = entity.trackingSession;
            uint32_t batchingSession = entity.batchingSession;
            uint32_t sMode = entity.sessionMode;

            mSessionMap.erase(id);

            if (sMode == SESSION_MODE_ON_FIX) {
                RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
                if (requests) {
                    requests->push(new StopTrackingRequest(*this));
                    mLocationAPI->stopTracking(trackingSession);
                }
            } else if (sMode == SESSION_MODE_ON_FULL) {
                RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
                if (requests) {
                    requests->push(new StopBatchingRequest(*this));
                    mLocationAPI->stopBatching(batchingSession);
                }
            }

            retVal = LOCATION_ERROR_SUCCESS;
        } else {
            retVal = LOCATION_ERROR_ID_UNKNOWN;
            LOC_LOGE("%s:%d] session %d is not exist.", __FUNCTION__, __LINE__, id);
        }

    }
    pthread_mutex_unlock(&mMutex);
    return retVal;
}

uint32_t LocationAPIClientBase::locAPIUpdateSessionOptions(uint32_t id, uint32_t sessionMode,
        LocationOptions& options)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {

        if (mSessionMap.find(id) != mSessionMap.end()) {
            SessionEntity& entity = mSessionMap[id];

            uint32_t trackingSession = entity.trackingSession;
            uint32_t batchingSession = entity.batchingSession;
            uint32_t sMode = entity.sessionMode;

            if (sessionMode == SESSION_MODE_ON_FIX) {
                if (sMode == SESSION_MODE_ON_FIX) {
                    RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
                    if (requests) {
                        requests->push(new UpdateTrackingOptionsRequest(*this));
                        mLocationAPI->updateTrackingOptions(trackingSession, options);
                    }
                } else if (sMode == SESSION_MODE_ON_FULL) {
                    // stop batching
                    {
                        RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
                        if (requests) {
                            requests->push(new StopBatchingRequest(*this));
                            mLocationAPI->stopBatching(batchingSession);
                            batchingSession = 0;
                        }
                    }
                    // start tracking
                    {
                        RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
                        if (requests) {
                            delete requests;
                            mRequestQueues[REQUEST_TRACKING] = nullptr;
                        }
                        trackingSession = mLocationAPI->startTracking(options);
                        LOC_LOGI("%s:%d] start new session: %d",
                                __FUNCTION__, __LINE__, trackingSession);
                        requests = new RequestQueue(trackingSession);
                        requests->push(new StartTrackingRequest(*this));
                        mRequestQueues[REQUEST_TRACKING] = requests;
                    }
                }
            } else if (sessionMode == SESSION_MODE_ON_FULL) {
                if (sMode == SESSION_MODE_ON_FIX) {
                    // stop tracking
                    {
                        RequestQueue* requests = mRequestQueues[REQUEST_TRACKING];
                        if (requests) {
                            requests->push(new StopTrackingRequest(*this));
                            mLocationAPI->stopTracking(trackingSession);
                            trackingSession = 0;
                        }
                    }
                    // start batching
                    {
                        RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
                        if (requests) {
                            delete requests;
                            mRequestQueues[REQUEST_BATCHING] = nullptr;
                        }
                        batchingSession = mLocationAPI->startBatching(options);
                        LOC_LOGI("%s:%d] start new session: %d",
                                __FUNCTION__, __LINE__, batchingSession);
                        requests = new RequestQueue(batchingSession);
                        requests->push(new StartBatchingRequest(*this));
                        mRequestQueues[REQUEST_BATCHING] = requests;
                    }
                } else if (sMode == SESSION_MODE_ON_FULL) {
                    RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
                    requests = mRequestQueues[REQUEST_BATCHING];
                    if (requests) {
                        requests->push(new UpdateBatchingOptionsRequest(*this));
                        mLocationAPI->updateBatchingOptions(batchingSession, options);
                    }
                }
            }

            entity.trackingSession = trackingSession;
            entity.batchingSession = batchingSession;
            entity.sessionMode = sessionMode;

            retVal = LOCATION_ERROR_SUCCESS;
        } else {
            retVal = LOCATION_ERROR_ID_UNKNOWN;
            LOC_LOGE("%s:%d] session %d is not exist.", __FUNCTION__, __LINE__, id);
        }

    }
    pthread_mutex_unlock(&mMutex);
    return retVal;
}

void LocationAPIClientBase::locAPIGetBatchedLocations(size_t count)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t session = 0;
        RequestQueue* requests = mRequestQueues[REQUEST_BATCHING];
        if (requests) {
            session = requests->getSession();
            if (session > 0) {
                requests->push(new GetBatchedLocationsRequest(*this));
                mLocationAPI->getBatchedLocations(session, count);
            }
        }
    }
    pthread_mutex_unlock(&mMutex);
}

uint32_t LocationAPIClientBase::locAPIAddGeofences(
        size_t count, uint32_t* ids, GeofenceOption* options, GeofenceInfo* data)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (!requests) {
            // Create a new RequestQueue for Geofenceing if we've not had one.
            // The RequestQueue will be released when LocationAPIClientBase is released.
            requests = new RequestQueue(GEOFENCE_SESSION_ID);
            mRequestQueues[REQUEST_GEOFENCE] = requests;
        }
        uint32_t* sessions = mLocationAPI->addGeofences(count, options, data);
        if (sessions) {
            LOC_LOGI("%s:%d] start new sessions: %p", __FUNCTION__, __LINE__, sessions);
            requests->push(new AddGeofencesRequest(*this));

            for (size_t i = 0; i < count; i++) {
                mGeofenceBiDict.set(ids[i], sessions[i], options[i].breachTypeMask);
            }
            retVal = LOCATION_ERROR_SUCCESS;
        }
    }
    pthread_mutex_unlock(&mMutex);

    return retVal;
}

void LocationAPIClientBase::locAPIRemoveGeofences(size_t count, uint32_t* ids)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t* sessions = (uint32_t*)malloc(sizeof(uint32_t) * count);

        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (requests) {
            size_t j = 0;
            for (size_t i = 0; i < count; i++) {
                sessions[j] = mGeofenceBiDict.getSession(ids[i]);
                if (sessions[j] > 0) {
                    j++;
                }
            }
            if (j > 0) {
                requests->push(new RemoveGeofencesRequest(*this));
                mLocationAPI->removeGeofences(j, sessions);
            }
        }

        free(sessions);
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIModifyGeofences(
        size_t count, uint32_t* ids, GeofenceOption* options)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t* sessions = (uint32_t*)malloc(sizeof(uint32_t) * count);

        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (requests) {
            size_t j = 0;
            for (size_t i = 0; i < count; i++) {
                sessions[j] = mGeofenceBiDict.getSession(ids[i]);
                if (sessions[j] > 0) {
                    mGeofenceBiDict.set(ids[i], sessions[j], options[i].breachTypeMask);
                    j++;
                }
            }
            if (j > 0) {
                requests->push(new ModifyGeofencesRequest(*this));
                mLocationAPI->modifyGeofences(j, sessions, options);
            }
        }

        free(sessions);
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIPauseGeofences(size_t count, uint32_t* ids)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t* sessions = (uint32_t*)malloc(sizeof(uint32_t) * count);

        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (requests) {
            size_t j = 0;
            for (size_t i = 0; i < count; i++) {
                sessions[j] = mGeofenceBiDict.getSession(ids[i]);
                if (sessions[j] > 0) {
                    j++;
                }
            }
            if (j > 0) {
                requests->push(new PauseGeofencesRequest(*this));
                mLocationAPI->pauseGeofences(j, sessions);
            }
        }

        free(sessions);
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIResumeGeofences(
        size_t count, uint32_t* ids, GeofenceBreachTypeMask* mask)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        uint32_t* sessions = (uint32_t*)malloc(sizeof(uint32_t) * count);

        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (requests) {
            size_t j = 0;
            for (size_t i = 0; i < count; i++) {
                sessions[j] = mGeofenceBiDict.getSession(ids[i]);
                if (sessions[j] > 0) {
                    if (mask) {
                        mGeofenceBiDict.set(ids[i], sessions[j], mask[i]);
                    }
                    j++;
                }
            }
            if (j > 0) {
                requests->push(new ResumeGeofencesRequest(*this));
                mLocationAPI->resumeGeofences(j, sessions);
            }
        }

        free(sessions);
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIRemoveAllGeofences()
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        std::vector<uint32_t> sessionsVec = mGeofenceBiDict.getAllSessions();
        size_t count = sessionsVec.size();
        uint32_t* sessions = (uint32_t*)malloc(sizeof(uint32_t) * count);

        RequestQueue* requests = mRequestQueues[REQUEST_GEOFENCE];
        if (requests) {
            size_t j = 0;
            for (size_t i = 0; i < count; i++) {
                sessions[j] = sessionsVec[i];
                if (sessions[j] > 0) {
                    j++;
                }
            }
            if (j > 0) {
                requests->push(new RemoveGeofencesRequest(*this));
                mLocationAPI->removeGeofences(j, sessions);
            }
        }

        free(sessions);
    }
    pthread_mutex_unlock(&mMutex);
}

void LocationAPIClientBase::locAPIGnssNiResponse(uint32_t id, GnssNiResponse response)
{
    pthread_mutex_lock(&mMutex);
    if (mLocationAPI) {
        RequestQueue* requests = mRequestQueues[REQUEST_NIRESPONSE];
        if (requests) {
            delete requests;
            mRequestQueues[REQUEST_NIRESPONSE] = nullptr;
        }
        uint32_t session = id;
        mLocationAPI->gnssNiResponse(id, response);
        LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, session);
        requests = new RequestQueue(session);
        requests->push(new GnssNiResponseRequest(*this));
        mRequestQueues[REQUEST_NIRESPONSE] = requests;
    }
    pthread_mutex_unlock(&mMutex);
}

uint32_t LocationAPIClientBase::locAPIGnssDeleteAidingData(GnssAidingData& data)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mLocationControlAPI) {
        RequestQueue* requests = mRequestQueues[REQUEST_DELETEAIDINGDATA];
        if (requests) {
            delete requests;
            mRequestQueues[REQUEST_DELETEAIDINGDATA] = nullptr;
        }
        uint32_t session = mLocationControlAPI->gnssDeleteAidingData(data);
        LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, session);
        requests = new RequestQueue(session);
        requests->push(new GnssDeleteAidingDataRequest(*this));
        mRequestQueues[REQUEST_DELETEAIDINGDATA] = requests;

        retVal = LOCATION_ERROR_SUCCESS;
    }
    pthread_mutex_unlock(&mMutex);

    return retVal;
}

uint32_t LocationAPIClientBase::locAPIEnable(LocationTechnologyType techType)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    pthread_mutex_lock(&mMutex);
    if (mEnabled) {
        // just return success if already enabled
        retVal = LOCATION_ERROR_SUCCESS;
    } else if (mLocationControlAPI) {
        RequestQueue* requests = mRequestQueues[REQUEST_CONTROL];
        if (requests) {
            delete requests;
            mRequestQueues[REQUEST_CONTROL] = nullptr;
        }
        uint32_t session = mLocationControlAPI->enable(techType);
        LOC_LOGI("%s:%d] start new session: %d", __FUNCTION__, __LINE__, session);
        requests = new RequestQueue(session);
        mRequestQueues[REQUEST_CONTROL] = requests;
        if (requests) {
            requests->push(new EnableRequest(*this));
            retVal = LOCATION_ERROR_SUCCESS;
            mEnabled = true;
        }
    }
    pthread_mutex_unlock(&mMutex);

    return retVal;
}

void LocationAPIClientBase::locAPIDisable()
{
    pthread_mutex_lock(&mMutex);
    if (mEnabled && mLocationControlAPI) {
        uint32_t session = 0;
        RequestQueue* requests = mRequestQueues[REQUEST_CONTROL];
        if (requests) {
            session = requests->getSession();
            if (session > 0) {
                requests->push(new DisableRequest(*this));
                mLocationControlAPI->disable(session);
                mEnabled = false;
            }
        }
    }
    pthread_mutex_unlock(&mMutex);
}

uint32_t LocationAPIClientBase::locAPIGnssUpdateConfig(GnssConfig config)
{
    uint32_t retVal = LOCATION_ERROR_GENERAL_FAILURE;
    if (memcmp(&mConfig, &config, sizeof(GnssConfig)) == 0) {
        LOC_LOGV("%s:%d] GnssConfig is identical to previous call", __FUNCTION__, __LINE__);
        retVal = LOCATION_ERROR_SUCCESS;
        return retVal;
    }

    pthread_mutex_lock(&mMutex);
    if (mLocationControlAPI) {

        memcpy(&mConfig, &config, sizeof(GnssConfig));

        uint32_t session = 0;
        RequestQueue* requests = mRequestQueues[REQUEST_CONFIG];
        uint32_t* idArray = mLocationControlAPI->gnssUpdateConfig(config);
        LOC_LOGV("%s:%d] gnssUpdateConfig return array: %p", __FUNCTION__, __LINE__, idArray);
        if (!requests && idArray != nullptr) {
            requests = new RequestQueue(idArray[0]);
            mRequestQueues[REQUEST_CONFIG] = requests;
        }
        if (requests) {
            requests->push(new GnssUpdateConfigRequest(*this));
            retVal = LOCATION_ERROR_SUCCESS;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return retVal;
}

void LocationAPIClientBase::beforeGeofenceBreachCb(
        GeofenceBreachNotification geofenceBreachNotification)
{
    if (mGeofenceBreachCallback == nullptr)
        return;
    uint32_t* ids = (uint32_t*)malloc(sizeof(uint32_t) * geofenceBreachNotification.count);
    uint32_t* backup = geofenceBreachNotification.ids;
    size_t n = geofenceBreachNotification.count;

    size_t count = 0;
    for (size_t i = 0; i < n; i++) {
        uint32_t id = mGeofenceBiDict.getId(geofenceBreachNotification.ids[i]);
        GeofenceBreachTypeMask type = mGeofenceBiDict.getType(geofenceBreachNotification.ids[i]);
        // if type == 0, we will not head into the fllowing block anyway.
        // so we don't need to check id and type
        if ((geofenceBreachNotification.type == GEOFENCE_BREACH_ENTER &&
            (type & GEOFENCE_BREACH_ENTER_BIT)) ||
            (geofenceBreachNotification.type == GEOFENCE_BREACH_EXIT &&
            (type & GEOFENCE_BREACH_EXIT_BIT))
           ) {
            ids[count] = id;
            count++;
        }
    }
    geofenceBreachNotification.count = count;
    geofenceBreachNotification.ids = ids;
    mGeofenceBreachCallback(geofenceBreachNotification);

    // restore ids
    geofenceBreachNotification.ids = backup;
    geofenceBreachNotification.count = n;
    free(ids);
}

void LocationAPIClientBase::onResponseCb(LocationError error, uint32_t id)
{
    if (error != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("%s:%d] ERROR: %d ID: %d", __FUNCTION__, __LINE__, error, id);
    } else {
        LOC_LOGV("%s:%d] error: %d id: %d", __FUNCTION__, __LINE__, error, id);
    }
    LocationAPIRequest* request = getRequestBySession(id);
    if (request) {
        request->onResponse(error);
        delete request;
    }
}

void LocationAPIClientBase::onCollectiveResponseCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    for (size_t i = 0; i < count; i++) {
        if (errors[i] != LOCATION_ERROR_SUCCESS) {
            LOC_LOGE("%s:%d] ERROR: %d ID: %d", __FUNCTION__, __LINE__, errors[i], ids[i]);
        } else {
            LOC_LOGV("%s:%d] error: %d id: %d", __FUNCTION__, __LINE__, errors[i], ids[i]);
        }
    }
    LocationAPIRequest* request = nullptr;
    pthread_mutex_lock(&mMutex);
    if (mRequestQueues[REQUEST_GEOFENCE] != nullptr) {
        request = mRequestQueues[REQUEST_GEOFENCE]->pop();
    }
    pthread_mutex_unlock(&mMutex);
    if (request) {
        request->onCollectiveResponse(count, errors, ids);
        delete request;
    }
}

void LocationAPIClientBase::onCtrlResponseCb(LocationError error, uint32_t id)
{
    if (error != LOCATION_ERROR_SUCCESS) {
        LOC_LOGE("%s:%d] ERROR: %d ID: %d", __FUNCTION__, __LINE__, error, id);
    } else {
        LOC_LOGV("%s:%d] error: %d id: %d", __FUNCTION__, __LINE__, error, id);
    }
    LocationAPIRequest* request = getRequestBySession(id);
    if (request) {
        request->onResponse(error);
        delete request;
    }
}

void LocationAPIClientBase::onCtrlCollectiveResponseCb(
        size_t count, LocationError* errors, uint32_t* ids)
{
    for (size_t i = 0; i < count; i++) {
        if (errors[i] != LOCATION_ERROR_SUCCESS) {
            LOC_LOGE("%s:%d] ERROR: %d ID: %d", __FUNCTION__, __LINE__, errors[i], ids[i]);
        } else {
            LOC_LOGV("%s:%d] error: %d id: %d", __FUNCTION__, __LINE__, errors[i], ids[i]);
        }
    }
    LocationAPIRequest* request = nullptr;
    pthread_mutex_lock(&mMutex);
    if (mRequestQueues[REQUEST_CONFIG] != nullptr) {
        request = mRequestQueues[REQUEST_CONFIG]->pop();
    }
    pthread_mutex_unlock(&mMutex);
    if (request) {
        request->onCollectiveResponse(count, errors, ids);
        delete request;
    }
}

LocationAPIClientBase::LocationAPIRequest*
LocationAPIClientBase::getRequestBySession(uint32_t session)
{
    pthread_mutex_lock(&mMutex);
    LocationAPIRequest* request = nullptr;
    for (int i = 0; i < REQUEST_MAX; i++) {
        if (i != REQUEST_GEOFENCE &&
                i != REQUEST_CONFIG &&
                mRequestQueues[i] &&
                mRequestQueues[i]->getSession() == session) {
            request = mRequestQueues[i]->pop();
            break;
        }
    }
    pthread_mutex_unlock(&mMutex);
    return request;
}
