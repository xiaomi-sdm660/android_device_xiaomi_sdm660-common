/* Copyright (c) 2011, Code Aurora Forum. All rights reserved.
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
 *     * Neither the name of Code Aurora Forum, Inc. nor the names of its
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include <hardware/gps.h>
#include <utils/SystemClock.h>
#include "LocApiV02Adapter.h"
#include "loc_api_v02_client.h"
#include "loc_api_v02_log.h"
#include "loc_api_sync_req.h"
#include "LocApiAdapter.h"

#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_adapter"
#include "loc_util_log.h"


/* Default session id ; TBD needs incrementing for each */
#define LOC_API_V02_DEF_SESSION_ID (1)

/* Default minimium interval in ms */
#define LOC_API_V02_DEF_MIN_INTERVAL (1000)

/* Default horizontal accuracy  in meters*/
#define LOC_API_V02_DEF_HORZ_ACCURACY (50)

/* Default timeout in ms; TBD: needs implementing */
#define LOC_API_V02_DEF_TIMEOUT (120000)

/* UMTS CP Address key*/
#define LOC_NI_NOTIF_KEY_ADDRESS           "Address"

/* GPS SV Id offset */
#define GPS_SV_ID_OFFSET        (1)

/* GLONASS SV Id offset */
#define GLONASS_SV_ID_OFFSET    (65)

/* SV ID range */
#define SV_ID_RANGE             (32)



/* static event callbacks that call the LocApiV02Adapter callbacks*/

/* global event callback, call the eventCb function in loc api adapter v02
   instance */
static void globalEventCb(locClientHandleType clientHandle,
                          uint32_t eventId,
                          const locClientEventIndUnionType eventPayload,
                          void*  pClientCookie)
{
  MODEM_LOG_CALLFLOW(%s, loc_get_v02_event_name(eventId));
  LocApiV02Adapter *locApiV02AdapterInstance =
      (LocApiV02Adapter *)pClientCookie;

  LOC_LOGV ("%s:%d] client = %p, event id = %d, client cookie ptr = %p\n",
                  __func__,  __LINE__,  clientHandle, eventId, pClientCookie);

  // return if null is passed
  if( NULL == locApiV02AdapterInstance)
  {
    LOC_LOGE ("%s:%d] NULL object passed : client = %p, event id = %d\n",
                  __func__,  __LINE__,  clientHandle, eventId);
    return;
  }
  locApiV02AdapterInstance->eventCb(clientHandle,
                                    eventId,
                                    eventPayload);
}

/* global response callback, it calls the sync request process
   indication function to unblock the request that is waiting on this
   response indication*/
static void globalRespCb(locClientHandleType clientHandle,
                         uint32_t respId,
                         const locClientRespIndUnionType respPayload,
                         void*  pClientCookie)
{
  MODEM_LOG_CALLFLOW(%s, loc_get_v02_event_name(respId));
  LocApiV02Adapter *locApiV02AdapterInstance =
        (LocApiV02Adapter *)pClientCookie;


  LOC_LOGV ("%s:%d] client = %p, resp id = %d, client cookie ptr = %p\n",
                  __func__,  __LINE__,  clientHandle, respId, pClientCookie);

  if( NULL == locApiV02AdapterInstance)
  {
    LOC_LOGE ("%s:%d] NULL object passed : client = %p, resp id = %d\n",
                  __func__,  __LINE__,  clientHandle, respId);
    return;
  }
    // process the sync call
    // use pDeleteAssistDataInd as a dummy pointer
  loc_sync_process_ind(clientHandle, respId,
                       (void *)respPayload.pDeleteAssistDataInd);
}

/* global error callback, it will call the handle service down
   function in the loc api adapter instance. */
static void globalErrorCb (locClientHandleType clientHandle,
                           locClientErrorEnumType errorId,
                           void *pClientCookie)
{
  LocApiV02Adapter *locApiV02AdapterInstance =
          (LocApiV02Adapter *)pClientCookie;

  LOC_LOGV ("%s:%d] client = %p, error id = %d\n, client cookie ptr = %p\n",
                  __func__,  __LINE__,  clientHandle, errorId, pClientCookie);
  if( NULL == locApiV02AdapterInstance)
  {
    LOC_LOGE ("%s:%d] NULL object passed : client = %p, error id = %d\n",
                  __func__,  __LINE__,  clientHandle, errorId);
    return;
  }
  locApiV02AdapterInstance->errorCb(clientHandle,
                                    errorId);
}

/* global structure containing the callbacks */
locClientCallbacksType globalCallbacks =
{
    sizeof(locClientCallbacksType),
    globalEventCb,
    globalRespCb,
    globalErrorCb
};

/* Constructor for LocApiV02Adapter */
LocApiV02Adapter :: LocApiV02Adapter(LocEng &locEng):
  LocApiAdapter(locEng), clientHandle( LOC_CLIENT_INVALID_HANDLE_VALUE),
  eventMask(convertMask(locEng.eventMask)), navigating(false),
   fixCriteria (LOC_POSITION_MODE_MS_BASED, GPS_POSITION_RECURRENCE_PERIODIC,
                LOC_API_V02_DEF_MIN_INTERVAL, LOC_API_V02_DEF_HORZ_ACCURACY,
                LOC_API_V02_DEF_TIMEOUT )
{
  // initialize loc_sync_req interface
  loc_sync_req_init();
}

/* Destructor for LocApiV02Adapter */
LocApiV02Adapter :: ~LocApiV02Adapter()
{
  deInitLocClient();
}

/* close Loc API V02 client */
int LocApiV02Adapter :: deInitLocClient()
{
  return ( eLOC_CLIENT_SUCCESS == locClientClose(&clientHandle)) ? 0 : -1 ;
}

/* Initialize a loc api v02 client */
enum loc_api_adapter_err LocApiV02Adapter :: reinit()
{
  locClientStatusEnumType status;
  /* If the client is already open close it first */
  if(LOC_CLIENT_INVALID_HANDLE_VALUE != clientHandle)
  {
     status = locClientClose(&clientHandle);
     if( eLOC_CLIENT_SUCCESS != status)
     {
        LOC_LOGE ("%s:%d]: locClientClose failed, status = %d\n", __func__,
                     __LINE__, status);
        return (LOC_API_ADAPTER_ERR_FAILURE);
     }
  }

  LOC_LOGV ("%s:%d]: reference to this = %p passed in \n", __func__,
                       __LINE__, this);
  /* initialize the loc api v02 interface, note that
     the locClientOpen() function will block if the
     service is unavailable for a fixed time out */

  status = locClientOpen(
    eventMask, &globalCallbacks, &clientHandle, (void *)this);

  if (eLOC_CLIENT_SUCCESS != status ||
        clientHandle == LOC_CLIENT_INVALID_HANDLE_VALUE )
  {
      LOC_LOGE ("%s:%d]: locClientOpen failed, status = %s\n", __func__,
                __LINE__, loc_get_v02_client_status_name(status));
      return (LOC_API_ADAPTER_ERR_FAILURE);
  }

  // return SUCCESS
  return (LOC_API_ADAPTER_ERR_SUCCESS);
}

/* start positioning session */
enum loc_api_adapter_err LocApiV02Adapter :: startFix()
{
  locClientStatusEnumType status;
  locClientReqUnionType req_union;

  qmiLocStartReqMsgT_v02 start_msg;

  qmiLocSetOperationModeReqMsgT_v02 set_mode_msg;
  qmiLocSetOperationModeIndMsgT_v02 set_mode_ind;

    // clear all fields, validity masks
  memset (&start_msg, 0, sizeof(start_msg));
  memset (&set_mode_msg, 0, sizeof(set_mode_msg));
  memset (&set_mode_ind, 0, sizeof(set_mode_ind));

  LOC_LOGV("%s:%d]: start \n", __func__, __LINE__);

  // fill in the start request
  switch(fixCriteria.mode)
  {
    case LOC_POSITION_MODE_MS_BASED:
      set_mode_msg.operationMode = eQMI_LOC_OPER_MODE_MSB_V02;
      break;

    case LOC_POSITION_MODE_MS_ASSISTED:
      set_mode_msg.operationMode = eQMI_LOC_OPER_MODE_MSA_V02;
      break;

    case LOC_POSITION_MODE_RESERVED_4:
      set_mode_msg.operationMode = eQMI_LOC_OPER_MODE_CELL_ID_V02;
        break;

    default:
      set_mode_msg.operationMode = eQMI_LOC_OPER_MODE_STANDALONE_V02;
      break;
  }

  req_union.pSetOperationModeReq = &set_mode_msg;

  // send the mode first, before the start message.
  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_OPERATION_MODE_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_OPERATION_MODE_IND_V02,
                             &set_mode_ind); // NULL?

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != set_mode_ind.status)
  {
    LOC_LOGE ("%s:%d]: set opertion mode failed status = %s, "
                   "ind..status = %s\n", __func__, __LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(set_mode_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE; // error
  }

  if(fixCriteria.min_interval > 0)
  {
    start_msg.minInterval_valid = 1;
    start_msg.minInterval = fixCriteria.min_interval;
  }

  start_msg.horizontalAccuracyLevel_valid = 1;

  if (fixCriteria.preferred_accuracy <= 100)
  {
    // fix needs high accuracy
    start_msg.horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_HIGH_V02;
  }
  else if (fixCriteria.preferred_accuracy <= 1000)
  {
    //fix needs med accuracy
    start_msg.horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_MED_V02;
  }
  else
  {
    //fix needs low accuracy
    start_msg.horizontalAccuracyLevel =  eQMI_LOC_ACCURACY_LOW_V02;
  }

  start_msg.fixRecurrence_valid = 1;
  if(GPS_POSITION_RECURRENCE_SINGLE == fixCriteria.recurrence)
  {
    start_msg.fixRecurrence = eQMI_LOC_RECURRENCE_SINGLE_V02;
  }
  else
  {
    start_msg.fixRecurrence = eQMI_LOC_RECURRENCE_PERIODIC_V02;
  }

  //dummy session id
  // TBD: store session ID, check for session id in pos reports.
  start_msg.sessionId = LOC_API_V02_DEF_SESSION_ID;;

  req_union.pStartReq = &start_msg;

  status = locClientSendReq (clientHandle, QMI_LOC_START_REQ_V02,
                            req_union );

  if( eLOC_CLIENT_SUCCESS == status)
  {
    navigating = true;
    return LOC_API_ADAPTER_ERR_SUCCESS;
  }

  // start_fix failed so MO fix is not in progress
  navigating = false;

  return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
}

/* stop a positioning session */
enum loc_api_adapter_err LocApiV02Adapter :: stopFix()
{
  locClientStatusEnumType status;
  locClientReqUnionType req_union;

  qmiLocStopReqMsgT_v02 stop_msg;

  LOC_LOGD(" %s:%d]: stop called \n", __func__, __LINE__);

  memset(&stop_msg, 0, sizeof(stop_msg));

  // dummy session id
  stop_msg.sessionId = LOC_API_V02_DEF_SESSION_ID;

  req_union.pStopReq = &stop_msg;

  status = locClientSendReq(clientHandle,
                            QMI_LOC_STOP_REQ_V02,
                            req_union);

  if( eLOC_CLIENT_SUCCESS == status)
  {
    navigating = false;
    return LOC_API_ADAPTER_ERR_SUCCESS;
  }

  LOC_LOGE("%s:%d]: error = %s\n",__func__, __LINE__,
           loc_get_v02_client_status_name(status));
  return (LOC_API_ADAPTER_ERR_GENERAL_FAILURE);
}

/* set the positioning fix criteria */
enum loc_api_adapter_err LocApiV02Adapter ::  setPositionMode(
  LocPositionMode mode, GpsPositionRecurrence recurrence,
  uint32_t min_interval, uint32_t preferred_accuracy,
  uint32_t preferred_time)
{

  LOC_LOGV ("%s:%d]: interval = %d, mode = %d, recurrence = %d, preferred_accuracy = %d\n",__func__, __LINE__,
                 min_interval, mode, recurrence, preferred_accuracy);

  //store the fix criteria
  fixCriteria.mode = mode;

  fixCriteria.recurrence = recurrence;

  if(min_interval == 0)
  {
    fixCriteria.min_interval = MIN_POSSIBLE_FIX_INTERVAL;
  }
  else
  {
    fixCriteria.min_interval = min_interval;
  }

  fixCriteria.preferred_accuracy = preferred_accuracy;

  fixCriteria.preferred_time = preferred_time;

  if(true == navigating)
  {
      //fix is in progress, send a restart
    LOC_LOGD ("%s:%d]: fix is in progress restarting the fix with new "
                   "criteria\n", __func__, __LINE__);

    return( startFix());
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* inject time into the position engine */
enum loc_api_adapter_err LocApiV02Adapter ::
    setTime(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocInjectUtcTimeReqMsgT_v02  inject_time_msg;
  qmiLocInjectUtcTimeIndMsgT_v02 inject_time_ind;

  memset(&inject_time_msg, 0, sizeof(inject_time_msg));

  inject_time_ind.status = eQMI_LOC_GENERAL_FAILURE_V02;

  inject_time_msg.timeUtc = time;

  inject_time_msg.timeUtc += (int64_t)(android::elapsedRealtime() - timeReference);

  inject_time_msg.timeUnc = uncertainty;

  req_union.pInjectUtcTimeReq = &inject_time_msg;

  LOC_LOGV ("%s:%d]: uncertainty = %d\n", __func__, __LINE__,
                 uncertainty);

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_INJECT_UTC_TIME_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INJECT_UTC_TIME_IND_V02,
                             &inject_time_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != inject_time_ind.status)
  {
    LOC_LOGE ("%s:%d] status = %s, ind..status = %s\n", __func__,  __LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(inject_time_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* inject position into the position engine */
enum loc_api_adapter_err LocApiV02Adapter ::
    injectPosition(double latitude, double longitude, float accuracy)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocInjectPositionReqMsgT_v02 inject_pos_msg;
  qmiLocInjectPositionIndMsgT_v02 inject_pos_ind;

  memset(&inject_pos_msg, 0, sizeof(inject_pos_msg));

  inject_pos_msg.latitude_valid = 1;
  inject_pos_msg.latitude = latitude;

  inject_pos_msg.longitude_valid = 1;
  inject_pos_msg.longitude = longitude;

  inject_pos_msg.horUncCircular_valid = 1;

  inject_pos_msg.horUncCircular = accuracy; //meters assumed

  inject_pos_msg.horConfidence_valid = 1;

  inject_pos_msg.horConfidence = 63; // 63% (1 std dev assumed)

    /* Log */
  LOC_LOGD("%s:%d]: Lat=%lf, Lon=%lf, Acc=%.2lf\n", __func__, __LINE__,
                inject_pos_msg.latitude, inject_pos_msg.longitude,
                inject_pos_msg.horUncCircular);

  req_union.pInjectPositionReq = &inject_pos_msg;

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_INJECT_POSITION_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INJECT_POSITION_IND_V02,
                             &inject_pos_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != inject_pos_ind.status)
  {
    LOC_LOGE ("%s:%d]: error! status = %s, inject_pos_ind.status = %s\n",
              __func__, __LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(inject_pos_ind.status));

   return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* delete assistance date */
enum loc_api_adapter_err LocApiV02Adapter ::  deleteAidingData(GpsAidingData f)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocDeleteAssistDataReqMsgT_v02 delete_req;
  qmiLocDeleteAssistDataIndMsgT_v02 delete_resp;

  memset(&delete_req, 0, sizeof(delete_req));
  memset(&delete_resp, 0, sizeof(delete_resp));

  if( f == GPS_DELETE_ALL )
  {
    delete_req.deleteAllFlag = true;
  }

  else
  {
    /* to keep track of svInfoList for GPS and GLO*/
    uint32_t curr_sv_len = 0;
    uint32_t curr_sv_idx = 0;
    uint32_t sv_id =  0;

    if((f & GPS_DELETE_EPHEMERIS ) || ( f & GPS_DELETE_ALMANAC ))
    {
      /* do delete for all GPS SV's */

      curr_sv_len += SV_ID_RANGE;

      sv_id = GPS_SV_ID_OFFSET;

      delete_req.deleteSvInfoList_valid = 1;

      delete_req.deleteSvInfoList_len = curr_sv_len;

      LOC_LOGV("%s:%d]: Delete GPS SV info for index %d to %d"
                    "and sv id %d to %d \n",
                    __func__, __LINE__, curr_sv_idx, curr_sv_len - 1,
                    sv_id, sv_id+SV_ID_RANGE);

      for( uint32_t i = curr_sv_idx; i< curr_sv_len ; i++, sv_id++ )
      {
        delete_req.deleteSvInfoList[i].gnssSvId = sv_id;

        delete_req.deleteSvInfoList[i].system = eQMI_LOC_SV_SYSTEM_GPS_V02;

        if(f & GPS_DELETE_EPHEMERIS )
        {
          // set ephemeris mask for all GPS SV's
          delete_req.deleteSvInfoList[i].deleteSvInfoMask |=
            QMI_LOC_MASK_DELETE_EPHEMERIS_V02;
        }

        if( f & GPS_DELETE_ALMANAC )
        {
          delete_req.deleteSvInfoList[i].deleteSvInfoMask |=
            QMI_LOC_MASK_DELETE_ALMANAC_V02;
        }
      }
      // increment the current index
      curr_sv_idx += SV_ID_RANGE;

    }

#ifdef QCOM_FEATURE_DELEXT
    if( f & GPS_DELETE_TIME_GPS )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GPS_TIME_V02;
    }
#endif
    if(f & GPS_DELETE_POSITION )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_POSITION_V02;
    }

    if(f & GPS_DELETE_TIME )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_TIME_V02;
    }

    if(f & GPS_DELETE_IONO )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_IONO_V02;
    }

    if(f & GPS_DELETE_UTC )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_UTC_V02;
    }

    if(f & GPS_DELETE_HEALTH )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_HEALTH_V02;
    }

    if(f & GPS_DELETE_SVDIR )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GPS_SVDIR_V02;
    }
    if(f & GPS_DELETE_SADATA )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_SADATA_V02;
    }
    if(f & GPS_DELETE_RTI )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_RTI_V02;
    }
    if(f & GPS_DELETE_CELLDB_INFO )
    {
      delete_req.deleteCellDbDataMask_valid = 1;
      delete_req.deleteCellDbDataMask =
        ( QMI_LOC_MASK_DELETE_CELLDB_POS_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02 |
          QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02) ;

    }
#ifdef QCOM_FEATURE_DELEXT
    if(f & GPS_DELETE_ALMANAC_CORR )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GPS_ALM_CORR_V02;
    }
    if(f & GPS_DELETE_FREQ_BIAS_EST )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_FREQ_BIAS_EST_V02;
    }
    if ( (f & GPS_DELETE_EPHEMERIS_GLO ) || (f & GPS_DELETE_ALMANAC_GLO ))
    {
      /* do delete for all GLONASS SV's (65 - 96)
      */
      curr_sv_len += SV_ID_RANGE;

      sv_id = GLONASS_SV_ID_OFFSET;

      delete_req.deleteSvInfoList_valid = 1;

      delete_req.deleteSvInfoList_len = curr_sv_len;

      LOC_LOGV("%s:%d]: Delete GLO SV info for index %d to %d"
                    "and sv id %d to %d \n",
                    __func__, __LINE__, curr_sv_idx, curr_sv_len - 1,
                    sv_id, sv_id+SV_ID_RANGE);


      for( uint32_t i = curr_sv_idx; i< curr_sv_len ; i++, sv_id++ )
      {
        delete_req.deleteSvInfoList[i].gnssSvId = sv_id;

        delete_req.deleteSvInfoList[i].system = eQMI_LOC_SV_SYSTEM_GLONASS_V02;

        if(f & GPS_DELETE_EPHEMERIS )
        {
          // set ephemeris mask for all GPS SV's
          delete_req.deleteSvInfoList[i].deleteSvInfoMask |=
            QMI_LOC_MASK_DELETE_EPHEMERIS_V02;
        }

        if( f & GPS_DELETE_ALMANAC )
        {
          delete_req.deleteSvInfoList[i].deleteSvInfoMask |=
            QMI_LOC_MASK_DELETE_ALMANAC_V02;
        }
      }
      curr_sv_idx += SV_ID_RANGE;
    }

    if(f & GPS_DELETE_SVDIR_GLO )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GLO_SVDIR_V02;
    }

    if(f & GPS_DELETE_SVSTEER_GLO )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GLO_SVSTEER_V02;
    }

    if(f & GPS_DELETE_ALMANAC_CORR_GLO )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GLO_ALM_CORR_V02;
    }

    if(f & GPS_DELETE_TIME_GLO )
    {
      delete_req.deleteGnssDataMask_valid = 1;
      delete_req.deleteGnssDataMask |= QMI_LOC_MASK_DELETE_GLO_TIME_V02;
    }
#endif
  }

  req_union.pDeleteAssistDataReq = &delete_req;

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_DELETE_ASSIST_DATA_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_DELETE_ASSIST_DATA_IND_V02,
                             &delete_resp);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != delete_resp.status)
  {
    LOC_LOGE ("%s:%d]: error! status = %s, delete_resp.status = %s\n",
              __func__, __LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(delete_resp.status));

   return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* send NI user repsonse to the engine */
enum loc_api_adapter_err LocApiV02Adapter ::
    informNiResponse(GpsUserResponseType userResponse,
                     const void* passThroughData)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;

  qmiLocNiUserRespReqMsgT_v02 ni_resp;
  qmiLocNiUserRespIndMsgT_v02 ni_resp_ind;

  qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *request_pass_back =
    (qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *)passThroughData;

  memset(&ni_resp,0, sizeof(ni_resp));

  memset(&ni_resp_ind,0, sizeof(ni_resp_ind));

  switch (userResponse)
  {
    case GPS_NI_RESPONSE_ACCEPT:
      ni_resp.userResp = eQMI_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT_V02;
      break;
   case GPS_NI_RESPONSE_DENY:
      ni_resp.userResp = eQMI_LOC_NI_LCS_NOTIFY_VERIFY_DENY_V02;
      break;
   case GPS_NI_RESPONSE_NORESP:
      ni_resp.userResp = eQMI_LOC_NI_LCS_NOTIFY_VERIFY_NORESP_V02;
      break;
   default:
      return LOC_API_ADAPTER_ERR_INVALID_PARAMETER;
  }

  LOC_LOGV(" %s:%d]: NI response: %d\n", __func__, __LINE__,
                ni_resp.userResp);

  ni_resp.notificationType = request_pass_back->notificationType;

  // copy SUPL payload from request
  if(request_pass_back->NiSuplInd_valid == 1)
  {
     ni_resp.NiSuplPayload_valid = 1;
     memcpy(&(ni_resp.NiSuplPayload), &(request_pass_back->NiSuplInd),
            sizeof(qmiLocNiSuplNotifyVerifyStructT_v02));

  }
  // should this be an "else if"?? we don't need to decide

  // copy UMTS-CP payload from request
  if( request_pass_back->NiUmtsCpInd_valid == 1 )
  {
     ni_resp.NiUmtsCpPayload_valid = 1;
     memcpy(&(ni_resp.NiUmtsCpPayload), &(request_pass_back->NiUmtsCpInd),
            sizeof(qmiLocNiUmtsCpNotifyVerifyStructT_v02));
  }

  //copy Vx payload from the request
  if( request_pass_back->NiVxInd_valid == 1)
  {
     ni_resp.NiVxPayload_valid = 1;
     memcpy(&(ni_resp.NiVxPayload), &(request_pass_back->NiVxInd),
            sizeof(qmiLocNiVxNotifyVerifyStructT_v02));
  }

  // copy Vx service interaction payload from the request
  if(request_pass_back->NiVxServiceInteractionInd_valid == 1)
  {
     ni_resp.NiVxServiceInteractionPayload_valid = 1;
     memcpy(&(ni_resp.NiVxServiceInteractionPayload),
            &(request_pass_back->NiVxServiceInteractionInd),
            sizeof(qmiLocNiVxServiceInteractionStructT_v02));
  }

  req_union.pNiUserRespReq = &ni_resp;

  status = loc_sync_send_req (
     clientHandle, QMI_LOC_NI_USER_RESPONSE_REQ_V02,
     req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
     QMI_LOC_NI_USER_RESPONSE_IND_V02, &ni_resp_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
      eQMI_LOC_SUCCESS_V02 != ni_resp_ind.status)
  {
    LOC_LOGE ("%s:%d]: error! status = %s, ni_resp_ind.status = %s\n",
              __func__, __LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(ni_resp_ind.status));

   return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* Set UMTs SLP server URL */
enum loc_api_adapter_err LocApiV02Adapter :: setServer(
  const char* url, int len)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocSetServerReqMsgT_v02 set_server_req;
  qmiLocSetServerIndMsgT_v02 set_server_ind;

  if(len <=0 || len > sizeof(set_server_req.urlAddr))
  {
    LOC_LOGE("%s:%d]: len = %d greater than max allowed url length\n",
                  __func__, __LINE__, len);

    return LOC_API_ADAPTER_ERR_INVALID_PARAMETER;
  }

  memset(&set_server_req, 0, sizeof(set_server_req));

  LOC_LOGD("%s:%d]:, url = %s, len = %d\n", __func__, __LINE__, url, len);

  set_server_req.serverType = eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02;

  set_server_req.urlAddr_valid = 1;

  strlcpy(set_server_req.urlAddr, url, sizeof(set_server_req.urlAddr));

  req_union.pSetServerReq = &set_server_req;

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_SERVER_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_SERVER_IND_V02,
                             &set_server_ind);

  if (status != eLOC_CLIENT_SUCCESS ||
         eQMI_LOC_SUCCESS_V02 != set_server_ind.status)
  {
    LOC_LOGE ("%s:%d]: error status = %s, set_server_ind.status = %s\n",
              __func__,__LINE__,
              loc_get_v02_client_status_name(status),
              loc_get_v02_qmi_status_name(set_server_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

enum loc_api_adapter_err LocApiV02Adapter ::
    setServer(unsigned int ip, int port, LocServerType type)
{
  locClientReqUnionType req_union;
  locClientStatusEnumType status;
  qmiLocSetServerReqMsgT_v02 set_server_req;
  qmiLocSetServerIndMsgT_v02 set_server_ind;
  qmiLocServerTypeEnumT_v02 set_server_cmd;

  switch (type) {
  case LOC_AGPS_MPC_SERVER:
      set_server_cmd = eQMI_LOC_SERVER_TYPE_CDMA_MPC_V02;
      break;
  case LOC_AGPS_CUSTOM_PDE_SERVER:
      set_server_cmd = eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02;
      break;
  default:
      set_server_cmd = eQMI_LOC_SERVER_TYPE_CDMA_PDE_V02;
      break;
  }

  memset(&set_server_req, 0, sizeof(set_server_req));

  LOC_LOGD("%s:%d]:, ip = %u, port = %d\n", __func__, __LINE__, ip, port);

  set_server_req.serverType = set_server_cmd;
  set_server_req.ipv4Addr_valid = 1;
  set_server_req.ipv4Addr.addr = ip;
  set_server_req.ipv4Addr.port = port;

  req_union.pSetServerReq = &set_server_req;

  status = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_SERVER_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_SERVER_IND_V02,
                             &set_server_ind);

    if (status != eLOC_CLIENT_SUCCESS ||
         eQMI_LOC_SUCCESS_V02 != set_server_ind.status)
    {
      LOC_LOGE ("%s:%d]: error status = %s, set_server_ind.status = %s\n",
                __func__,__LINE__,
                loc_get_v02_client_status_name(status),
                loc_get_v02_qmi_status_name(set_server_ind.status));

      return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
    }

    return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* Inject XTRA data, this module breaks down the XTRA
   file into "chunks" and injects them one at a time */
enum loc_api_adapter_err LocApiV02Adapter :: setXtraData(
  char* data, int length)
{
  locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
  int     total_parts;
  uint8_t   part;
  uint16_t  len_injected;

  locClientReqUnionType req_union;
  qmiLocInjectPredictedOrbitsDataReqMsgT_v02 inject_xtra;
  qmiLocInjectPredictedOrbitsDataIndMsgT_v02 inject_xtra_ind;

  req_union.pInjectPredictedOrbitsDataReq = &inject_xtra;

  LOC_LOGD("%s:%d]: xtra size = %d\n", __func__, __LINE__, length);

  inject_xtra.formatType_valid = 1;
  inject_xtra.formatType = eQMI_LOC_PREDICTED_ORBITS_XTRA_V02;
  inject_xtra.totalSize = length;

  total_parts = ((length - 1) / QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02) + 1;

  inject_xtra.totalParts = total_parts;

  len_injected = 0; // O bytes injected

  // XTRA injection starts with part 1
  for (part = 1; part <= total_parts; part++)
  {
    inject_xtra.partNum = part;

    if (QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02 > (length - len_injected))
    {
      inject_xtra.partData_len = length - len_injected;
    }
    else
    {
      inject_xtra.partData_len = QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02;
    }

    // copy data into the message
    memcpy(inject_xtra.partData, data+len_injected, inject_xtra.partData_len);

    LOC_LOGD("[%s:%d] part %d/%d, len = %d, total injected = %d\n",
                  __func__, __LINE__,
                  inject_xtra.partNum, total_parts, inject_xtra.partData_len,
                  len_injected);

    status = loc_sync_send_req( clientHandle,
                                QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02,
                                req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                                QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02,
                                &inject_xtra_ind);

    if (status != eLOC_CLIENT_SUCCESS ||
        eQMI_LOC_SUCCESS_V02 != inject_xtra_ind.status ||
        inject_xtra.partNum != inject_xtra_ind.partNum)
    {
      LOC_LOGE ("%s:%d]: failed status = %s, inject_pos_ind.status = %s,"
                     " part num = %d, ind.partNum = %d\n", __func__, __LINE__,
                loc_get_v02_client_status_name(status),
                loc_get_v02_qmi_status_name(inject_xtra_ind.status),
                inject_xtra.partNum, inject_xtra_ind.partNum);

      return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
      break;
    }

    len_injected += inject_xtra.partData_len;
    LOC_LOGD("%s:%d]: XTRA injected length: %d\n", __func__, __LINE__,
        len_injected);
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}
#ifdef QCOM_FEATURE_IPV6
enum loc_api_adapter_err LocApiV02Adapter :: atlOpenStatus(
  int handle, int is_succ, char* apn, AGpsBearerType bear,
  AGpsType agpsType)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;
  qmiLocInformLocationServerConnStatusReqMsgT_v02 conn_status_req;
  qmiLocInformLocationServerConnStatusIndMsgT_v02 conn_status_ind;

  LOC_LOGD("%s:%d]: ATL open handle = %d, is_succ = %d, "
                "APN = [%s], bearer = %d \n",  __func__, __LINE__,
                handle, is_succ, apn, bear);

  memset(&conn_status_req, 0, sizeof(conn_status_req));
  memset(&conn_status_ind, 0, sizeof(conn_status_ind));

        // Fill in data
  conn_status_req.connHandle = handle;

  conn_status_req.requestType = eQMI_LOC_SERVER_REQUEST_OPEN_V02;

  if(is_succ)
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02;

    strlcpy(conn_status_req.apnProfile.apnName, apn,
            sizeof(conn_status_req.apnProfile.apnName) );

    switch(bear)
    {
      case AGPS_APN_BEARER_IPV4:
        conn_status_req.apnProfile.pdnType =
          eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4_V02;
        break;

      case AGPS_APN_BEARER_IPV6:
        conn_status_req.apnProfile.pdnType =
          eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV6_V02;
        break;

      case AGPS_APN_BEARER_IPV4V6:
        conn_status_req.apnProfile.pdnType =
          eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4V6_V02;
        break;

      default:
        LOC_LOGE("%s:%d]:invalid bearer type\n",__func__,__LINE__);
        return LOC_API_ADAPTER_ERR_INVALID_HANDLE;
    }

    conn_status_req.apnProfile_valid = 1;
  }
  else
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02;
  }

  req_union.pInformLocationServerConnStatusReq = &conn_status_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02,
                             &conn_status_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != conn_status_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(conn_status_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;

}
#else
enum loc_api_adapter_err LocApiV02Adapter :: atlOpenStatus(
  int handle, int is_succ, char* apn, AGpsType agpsType)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;
  qmiLocInformLocationServerConnStatusReqMsgT_v02 conn_status_req;
  qmiLocInformLocationServerConnStatusIndMsgT_v02 conn_status_ind;

  LOC_LOGD("%s:%d]: ATL open handle = %d, is_succ = %d, "
                "APN = [%s] \n",  __func__, __LINE__,
                handle, is_succ, apn);

  memset(&conn_status_req, 0, sizeof(conn_status_req));
  memset(&conn_status_ind, 0, sizeof(conn_status_ind));

        // Fill in data
  conn_status_req.connHandle = handle;

  conn_status_req.requestType = eQMI_LOC_SERVER_REQUEST_OPEN_V02;

  if(is_succ)
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02;

    strlcpy(conn_status_req.apnProfile.apnName, apn,
            sizeof(conn_status_req.apnProfile.apnName) );


    conn_status_req.apnProfile.pdnType =
      eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4_V02;

    conn_status_req.apnProfile_valid = 1;
  }
  else
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02;
  }

  req_union.pInformLocationServerConnStatusReq = &conn_status_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02,
                             &conn_status_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != conn_status_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(conn_status_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;

}
#endif
/* close atl connection */
enum loc_api_adapter_err LocApiV02Adapter :: atlCloseStatus(
  int handle, int is_succ)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;
  qmiLocInformLocationServerConnStatusReqMsgT_v02 conn_status_req;
  qmiLocInformLocationServerConnStatusIndMsgT_v02 conn_status_ind;

  LOC_LOGD("%s:%d]: ATL close handle = %d, is_succ = %d\n",
                 __func__, __LINE__,  handle, is_succ);

  memset(&conn_status_req, 0, sizeof(conn_status_req));
  memset(&conn_status_ind, 0, sizeof(conn_status_ind));

        // Fill in data
  conn_status_req.connHandle = handle;

  conn_status_req.requestType = eQMI_LOC_SERVER_REQUEST_CLOSE_V02;

  if(is_succ)
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02;
  }
  else
  {
    conn_status_req.statusType = eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02;
  }

  req_union.pInformLocationServerConnStatusReq = &conn_status_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02,
                             &conn_status_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != conn_status_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(conn_status_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* set the SUPL version */
enum loc_api_adapter_err LocApiV02Adapter :: setSUPLVersion(uint32_t version)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;

  qmiLocSetProtocolConfigParametersReqMsgT_v02 supl_config_req;
  qmiLocSetProtocolConfigParametersIndMsgT_v02 supl_config_ind;

  LOC_LOGD("%s:%d]: supl version = %d\n",  __func__, __LINE__, version);


  memset(&supl_config_req, 0, sizeof(supl_config_req));
  memset(&supl_config_ind, 0, sizeof(supl_config_ind));

   supl_config_req.suplVersion_valid = 1;
   // SUPL version from MSByte to LSByte:
   // (reserved)(major version)(minor version)(serviceIndicator)

   supl_config_req.suplVersion = (version == 0x00020000)?
     eQMI_LOC_SUPL_VERSION_2_0_V02 : eQMI_LOC_SUPL_VERSION_1_0_V02;

  req_union.pSetProtocolConfigParametersReq = &supl_config_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02,
                             &supl_config_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != supl_config_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(supl_config_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* set the Sensor Configuration */
enum loc_api_adapter_err LocApiV02Adapter :: setSensorControlConfig(int sensorsDisabled)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;

  qmiLocSetSensorControlConfigReqMsgT_v02 sensor_config_req;
  qmiLocSetSensorControlConfigIndMsgT_v02 sensor_config_ind;

  LOC_LOGD("%s:%d]: sensors disabled = %d\n",  __func__, __LINE__, sensorsDisabled);

  memset(&sensor_config_req, 0, sizeof(sensor_config_req));
  memset(&sensor_config_ind, 0, sizeof(sensor_config_ind));

  sensor_config_req.sensorsUsage_valid = 1;
  sensor_config_req.sensorsUsage = (sensorsDisabled == 1) ? eQMI_LOC_SENSOR_CONFIG_SENSOR_USE_DISABLE_V02
                                    : eQMI_LOC_SENSOR_CONFIG_SENSOR_USE_ENABLE_V02;

  req_union.pSetSensorControlConfigReq = &sensor_config_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02,
                             &sensor_config_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != sensor_config_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(sensor_config_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* set the Sensor Properties */
enum loc_api_adapter_err LocApiV02Adapter :: setSensorProperties(float gyroBiasVarianceRandomWalk)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;

  qmiLocSetSensorPropertiesReqMsgT_v02 sensor_prop_req;
  qmiLocSetSensorPropertiesIndMsgT_v02 sensor_prop_ind;

  LOC_LOGI("%s:%d]: sensors prop gyroBiasRandomWalk = %f\n",
                 __func__, __LINE__, gyroBiasVarianceRandomWalk);

  memset(&sensor_prop_req, 0, sizeof(sensor_prop_req));
  memset(&sensor_prop_ind, 0, sizeof(sensor_prop_ind));

  sensor_prop_req.gyroBiasVarianceRandomWalk_valid = 1;
  sensor_prop_req.gyroBiasVarianceRandomWalk = gyroBiasVarianceRandomWalk;

  req_union.pSetSensorPropertiesReq = &sensor_prop_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02,
                             &sensor_prop_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != sensor_prop_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(sensor_prop_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* set the Sensor Performance Config */
enum loc_api_adapter_err LocApiV02Adapter :: setSensorPerfControlConfig(int controlMode,
                                                                        int accelSamplesPerBatch, int accelBatchesPerSec,
                                                                        int gyroSamplesPerBatch, int gyroBatchesPerSec)
{
  locClientStatusEnumType result = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType req_union;

  qmiLocSetSensorPerformanceControlConfigReqMsgT_v02 sensor_perf_config_req;
  qmiLocSetSensorPerformanceControlConfigIndMsgT_v02 sensor_perf_config_ind;

  LOC_LOGD("%s:%d]: Sensor Perf Control Config (performanceControlMode)(%u) "
                "accel(#smp,#batches) (%u,%u) gyro(#smp,#batches) (%u,%u)\n",
                __FUNCTION__,
                __LINE__,
                controlMode,
                accelSamplesPerBatch,
                accelBatchesPerSec,
                gyroSamplesPerBatch,
                gyroBatchesPerSec
                );

  memset(&sensor_perf_config_req, 0, sizeof(sensor_perf_config_req));
  memset(&sensor_perf_config_ind, 0, sizeof(sensor_perf_config_ind));

  sensor_perf_config_req.performanceControlMode_valid = 1;
  sensor_perf_config_req.performanceControlMode = (qmiLocSensorPerformanceControlModeEnumT_v02)controlMode;
  sensor_perf_config_req.accelSamplingSpec_valid = 1;
  sensor_perf_config_req.accelSamplingSpec.batchesPerSecond = accelBatchesPerSec;
  sensor_perf_config_req.accelSamplingSpec.samplesPerBatch = accelSamplesPerBatch;
  sensor_perf_config_req.gyroSamplingSpec_valid = 1;
  sensor_perf_config_req.gyroSamplingSpec.batchesPerSecond = gyroBatchesPerSec;
  sensor_perf_config_req.gyroSamplingSpec.samplesPerBatch = gyroSamplesPerBatch;

  req_union.pSetSensorPerformanceControlConfigReq = &sensor_perf_config_req;

  result = loc_sync_send_req(clientHandle,
                             QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02,
                             req_union, LOC_ENGINE_SYNC_REQUEST_TIMEOUT,
                             QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02,
                             &sensor_perf_config_ind);

  if(result != eLOC_CLIENT_SUCCESS ||
     eQMI_LOC_SUCCESS_V02 != sensor_perf_config_ind.status)
  {
    LOC_LOGE ("%s:%d]: Error status = %s, ind..status = %s ",
              __func__, __LINE__,
              loc_get_v02_client_status_name(result),
              loc_get_v02_qmi_status_name(sensor_perf_config_ind.status));

    return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;
  }

  return LOC_API_ADAPTER_ERR_SUCCESS;
}

/* Convert event mask from loc eng to loc_api_v02 format */
locClientEventMaskType LocApiV02Adapter :: convertMask(
  LOC_API_ADAPTER_EVENT_MASK_T mask)
{
  locClientEventMaskType eventMask = 0;
  LOC_LOGD("%s:%d]: adapter mask = %u\n", __func__, __LINE__, mask);

  if(mask & LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT)
    eventMask |= QMI_LOC_EVENT_MASK_POSITION_REPORT_V02;

  if(mask & LOC_API_ADAPTER_BIT_SATELLITE_REPORT)
    eventMask |= QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02;

  /* treat NMEA_1Hz and NMEA_POSITION_REPORT the same*/
  if( (mask & LOC_API_ADAPTER_BIT_NMEA_POSITION_REPORT) ||
      (mask & LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT) )
    eventMask |= QMI_LOC_EVENT_MASK_NMEA_V02;

  if(mask & LOC_API_ADAPTER_BIT_NI_NOTIFY_VERIFY_REQUEST)
    eventMask |= QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02;

  if(mask & LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST)
  {
    // TBD: This needs to be decoupled in the HAL
    eventMask |= QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02;
    eventMask |= QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02;
    eventMask |= QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02;
  }

  if(mask & LOC_API_ADAPTER_BIT_STATUS_REPORT)
  {
    eventMask |= (QMI_LOC_EVENT_MASK_ENGINE_STATE_V02);
  }

  if(mask & LOC_API_ADAPTER_BIT_LOCATION_SERVER_REQUEST)
    eventMask |= QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02;

  return eventMask;
}

/* Convert error from loc_api_v02 to loc eng format*/
enum loc_api_adapter_err LocApiV02Adapter :: convertErr(
  locClientStatusEnumType status)
{
  switch( status)
  {
    case LOC_API_ADAPTER_ERR_SUCCESS:
      return LOC_API_ADAPTER_ERR_SUCCESS;

    case eLOC_CLIENT_FAILURE_GENERAL:
      return LOC_API_ADAPTER_ERR_GENERAL_FAILURE;

    case eLOC_CLIENT_FAILURE_UNSUPPORTED:
      return LOC_API_ADAPTER_ERR_UNSUPPORTED;

    case eLOC_CLIENT_FAILURE_INVALID_PARAMETER:
      return LOC_API_ADAPTER_ERR_INVALID_PARAMETER;

    case eLOC_CLIENT_FAILURE_ENGINE_BUSY:
      return LOC_API_ADAPTER_ERR_ENGINE_BUSY;

    case eLOC_CLIENT_FAILURE_PHONE_OFFLINE:
      return LOC_API_ADAPTER_ERR_PHONE_OFFLINE;

    case eLOC_CLIENT_FAILURE_TIMEOUT:
      return LOC_API_ADAPTER_ERR_TIMEOUT;

    case eLOC_CLIENT_FAILURE_INVALID_HANDLE:
      return LOC_API_ADAPTER_ERR_INVALID_HANDLE;

    case eLOC_CLIENT_FAILURE_SERVICE_NOT_PRESENT:
      return LOC_API_ADAPTER_ERR_SERVICE_NOT_PRESENT;

    default:
      return LOC_API_ADAPTER_ERR_FAILURE;
  }
}

/* convert position report to loc eng format and send the converted
   position to loc eng */

void LocApiV02Adapter :: reportPosition (
  const qmiLocEventPositionReportIndMsgT_v02 *location_report_ptr)
{
    GpsLocation location;

    memset(&location, 0, sizeof (GpsLocation));
    location.size = sizeof(location);
    // Process the position from final and intermediate reports

    if( (location_report_ptr->sessionStatus == eQMI_LOC_SESS_STATUS_SUCCESS_V02) ||
        (location_report_ptr->sessionStatus == eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02)
        )
    {
        // Latitude & Longitude
        if( (location_report_ptr->latitude_valid == 1 ) &&
            (location_report_ptr->longitude_valid == 1)  &&
            (location_report_ptr->latitude != 0 ||
             location_report_ptr->longitude!= 0))
        {
            location.flags  |= GPS_LOCATION_HAS_LAT_LONG;
            location.latitude  = location_report_ptr->latitude;
            location.longitude = location_report_ptr->longitude;

            // Time stamp (UTC)
            if(location_report_ptr->timestampUtc_valid == 1)
            {
                location.timestamp = location_report_ptr->timestampUtc;
            }

            // Altitude
            if(location_report_ptr->altitudeWrtEllipsoid_valid == 1  )
            {
                location.flags  |= GPS_LOCATION_HAS_ALTITUDE;
                location.altitude = location_report_ptr->altitudeWrtEllipsoid;
            }

            // Speed
            if((location_report_ptr->speedHorizontal_valid == 1) &&
               (location_report_ptr->speedVertical_valid ==1 ) )
            {
                location.flags  |= GPS_LOCATION_HAS_SPEED;
                location.speed = sqrt(
                    (location_report_ptr->speedHorizontal *
                     location_report_ptr->speedHorizontal) +
                    (location_report_ptr->speedVertical *
                     location_report_ptr->speedVertical) );
            }

            // Heading
            if(location_report_ptr->heading_valid == 1)
            {
                location.flags  |= GPS_LOCATION_HAS_BEARING;
                location.bearing = location_report_ptr->heading;
            }

            // Uncertainty (circular)
            if( (location_report_ptr->horUncCircular_valid ) )
            {
                location.flags  |= GPS_LOCATION_HAS_ACCURACY;
                location.accuracy = location_report_ptr->horUncCircular;
            }

            LocApiAdapter::reportPosition( location,
                                           locEngHandle.extPosInfo((void*)location_report_ptr),
                                           (location_report_ptr->sessionStatus
                                            == eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 ?
                                            LOC_SESS_INTERMEDIATE : LOC_SESS_SUCCESS));
        }
    }
    else
    {
        LocApiAdapter::reportPosition(location,
                                      NULL,
                                      LOC_SESS_FAILURE);

        LOC_LOGD("%s:%d]: Ignoring position report with sess status = %d, "
                      "fix id = %u\n", __func__, __LINE__,
                      location_report_ptr->sessionStatus,
                      location_report_ptr->fixId );
    }
}

/* convert satellite report to loc eng format and  send the converted
   report to loc eng */
void  LocApiV02Adapter :: reportSv (
  const qmiLocEventGnssSvInfoIndMsgT_v02 *gnss_report_ptr)
{
  GpsSvStatus      SvStatus;
  int              num_svs_max, i;
  const qmiLocSvInfoStructT_v02 *sv_info_ptr;

  LOC_LOGV ("%s:%d]: num of sv = %d\n", __func__, __LINE__,
                 gnss_report_ptr->svList_len);

  num_svs_max = 0;
  memset (&SvStatus, 0, sizeof (GpsSvStatus));
  if(gnss_report_ptr->svList_valid == 1)
  {
    num_svs_max = gnss_report_ptr->svList_len;
    if(num_svs_max > GPS_MAX_SVS)
    {
      num_svs_max = GPS_MAX_SVS;
    }
    SvStatus.num_svs = 0;
    for(i = 0; i < num_svs_max; i++)
    {
      sv_info_ptr = &(gnss_report_ptr->svList[i]);
      if((sv_info_ptr->validMask & QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02) &&
         (sv_info_ptr->validMask & QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02)
         && (sv_info_ptr->gnssSvId != 0 ))
      {
        if(sv_info_ptr->system == eQMI_LOC_SV_SYSTEM_GPS_V02)
        {
          SvStatus.sv_list[SvStatus.num_svs].size = sizeof(GpsSvStatus);
          SvStatus.sv_list[SvStatus.num_svs].prn = (int)sv_info_ptr->gnssSvId;

          // We only have the data field to report gps eph and alm mask
          if(sv_info_ptr->validMask &
             QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02)
          {
            if(sv_info_ptr->svInfoMask &
               QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02)
            {
              SvStatus.ephemeris_mask |= (1 << (sv_info_ptr->gnssSvId-1));
            }
            if(sv_info_ptr->svInfoMask &
               QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02)
            {
              SvStatus.almanac_mask |= (1 << (sv_info_ptr->gnssSvId-1));
            }
          }

          if((sv_info_ptr->validMask &
              QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02)
             &&
             (sv_info_ptr->svStatus == eQMI_LOC_SV_STATUS_TRACK_V02))
          {
            SvStatus.used_in_fix_mask |= (1 << (sv_info_ptr->gnssSvId-1));
          }
        }
        // SBAS: GPS RPN: 120-151,
        // In exteneded measurement report, we follow nmea standard,
        // which is from 33-64.
        else if(sv_info_ptr->system == eQMI_LOC_SV_SYSTEM_SBAS_V02)
        {
          SvStatus.sv_list[SvStatus.num_svs].prn =
            sv_info_ptr->gnssSvId + 33 - 120;
        }
        // Gloness: Slot id: 1-32
        // In extended measurement report, we follow nmea standard,
        // which is 65-96
        else if(sv_info_ptr->system == eQMI_LOC_SV_SYSTEM_GLONASS_V02)
        {
          SvStatus.sv_list[SvStatus.num_svs].prn =
            sv_info_ptr->gnssSvId + (65-1);
        }
        // Unsupported SV system
        else
        {
          continue;
        }
      }

      if(sv_info_ptr->validMask & QMI_LOC_SV_INFO_MASK_VALID_SNR_V02 )
      {
        SvStatus.sv_list[SvStatus.num_svs].snr = sv_info_ptr->snr;
      }

      if(sv_info_ptr->validMask & QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02)
      {
        SvStatus.sv_list[SvStatus.num_svs].elevation = sv_info_ptr->elevation;
      }

      if(sv_info_ptr->validMask & QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02)
      {
        SvStatus.sv_list[SvStatus.num_svs].azimuth = sv_info_ptr->azimuth;
      }

      SvStatus.num_svs++;
    }
  }

  if (SvStatus.num_svs != 0)
  {
    LOC_LOGV ("%s:%d]: firing SV callback\n", __func__, __LINE__);
    LocApiAdapter::reportSv(SvStatus,
                            locEngHandle.extSvInfo((void*)gnss_report_ptr));
  }
}

/* convert engine state report to loc eng format and send the converted
   report to loc eng */
void LocApiV02Adapter :: reportEngineState (
    const qmiLocEventEngineStateIndMsgT_v02 *engine_state_ptr)
{

  LOC_LOGV("%s:%d]: state = %d\n", __func__, __LINE__,
                 engine_state_ptr->engineState);

  if (engine_state_ptr->engineState == eQMI_LOC_ENGINE_STATE_ON_V02)
  {
    LocApiAdapter::reportStatus(GPS_STATUS_ENGINE_ON);
    LocApiAdapter::reportStatus(GPS_STATUS_SESSION_BEGIN);
  }
  else if (engine_state_ptr->engineState == eQMI_LOC_ENGINE_STATE_OFF_V02)
  {
    LocApiAdapter::reportStatus(GPS_STATUS_SESSION_END);
    LocApiAdapter::reportStatus(GPS_STATUS_ENGINE_OFF);
  }
  else
  {
      LocApiAdapter::reportStatus(GPS_STATUS_NONE);
  }

}

/* convert fix session state report to loc eng format and send the converted
   report to loc eng */
void LocApiV02Adapter :: reportFixSessionState (
    const qmiLocEventFixSessionStateIndMsgT_v02 *fix_session_state_ptr)
{
  GpsStatusValue status;
  LOC_LOGD("%s:%d]: state = %d\n", __func__, __LINE__,
                fix_session_state_ptr->sessionState);

  status = GPS_STATUS_NONE;
  if (fix_session_state_ptr->sessionState == eQMI_LOC_FIX_SESSION_STARTED_V02)
  {
    status = GPS_STATUS_SESSION_BEGIN;
  }
  else if (fix_session_state_ptr->sessionState
           == eQMI_LOC_FIX_SESSION_FINISHED_V02)
  {
    status = GPS_STATUS_SESSION_END;
  }
  LocApiAdapter::reportStatus(status);
}

/* convert NMEA report to loc eng format and send the converted
   report to loc eng */
void LocApiV02Adapter :: reportNmea (
  const qmiLocEventNmeaIndMsgT_v02 *nmea_report_ptr)
{

  LocApiAdapter::reportNmea(nmea_report_ptr->nmea,
                             strlen(nmea_report_ptr->nmea));

  LOC_LOGD("%s:%d]: $%c%c%c\n", __func__, __LINE__,
                  nmea_report_ptr->nmea[3], nmea_report_ptr->nmea[4],
                  nmea_report_ptr->nmea[5]);
}

/* convert and report an ATL request to loc engine */
void LocApiV02Adapter :: reportAtlRequest(
  const qmiLocEventLocationServerConnectionReqIndMsgT_v02 * server_request_ptr)
{
  uint32_t connHandle = server_request_ptr->connHandle;
  // service ATL open request; copy the WWAN type
  if(server_request_ptr->requestType == eQMI_LOC_SERVER_REQUEST_OPEN_V02 )
  {
    AGpsType agpsType;
#ifdef QCOM_FEATURE_IPV6
    switch(server_request_ptr->wwanType)
    {
      case eQMI_LOC_WWAN_TYPE_INTERNET_V02:
        agpsType = AGPS_TYPE_WWAN_ANY;
        break;

      case eQMI_LOC_WWAN_TYPE_AGNSS_V02:
        agpsType = AGPS_TYPE_SUPL;
        break;

      default:
        agpsType = AGPS_TYPE_WWAN_ANY;
        break;
    }
#else
    agpsType = AGPS_TYPE_SUPL;
#endif
    LocApiAdapter::requestATL(connHandle, agpsType);
  }

  // service the ATL close request
  else if (server_request_ptr->requestType == eQMI_LOC_SERVER_REQUEST_CLOSE_V02)
  {
    LocApiAdapter::releaseATL(connHandle);
  }
}

/* conver the NI report to loc eng format and send t loc engine */
void LocApiV02Adapter :: reportNiRequest(
    const qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *ni_req_ptr)
{
  GpsNiNotification notif;

  /* initialize the notification*/
  memset(notif.extras, 0, sizeof notif.extras);
  memset(notif.text, 0, sizeof notif.text);
  memset(notif.requestor_id, 0, sizeof notif.requestor_id);

  /* NI timeout gets overwritten in LocApiAdapter,
     initializing to 0 here */
  notif.timeout     = 0;

  notif.text_encoding = GPS_ENC_NONE ;

  notif.requestor_id_encoding = GPS_ENC_UNKNOWN;

  notif.notify_flags       = 0;

  notif.default_response   = GPS_NI_RESPONSE_NORESP;

  /*Handle Vx request */
  if(ni_req_ptr->NiVxInd_valid == 1)
  {
     const qmiLocNiVxNotifyVerifyStructT_v02 *vx_req = &(ni_req_ptr->NiVxInd);

     notif.ni_type     = GPS_NI_TYPE_VOICE;

     // Requestor ID, the requestor id recieved is NULL terminated
     hexcode(notif.requestor_id, sizeof notif.requestor_id,
             (char *)vx_req->requestorId, vx_req->requestorId_len );
  }

  /* Handle UMTS CP request*/
  else if(ni_req_ptr->NiUmtsCpInd_valid == 1)
  {
    const qmiLocNiUmtsCpNotifyVerifyStructT_v02 *umts_cp_req =
       &ni_req_ptr->NiUmtsCpInd;

    notif.ni_type     = GPS_NI_TYPE_UMTS_CTRL_PLANE;

    /* notificationText should always be a NULL terminated string */
    hexcode(notif.text, sizeof notif.text,
            (char *)umts_cp_req->notificationText,
            umts_cp_req->notificationText_len);

    /* Store requestor ID */
    hexcode(notif.requestor_id, sizeof(notif.requestor_id),
            (char *)umts_cp_req->requestorId.codedString,
            umts_cp_req->requestorId.codedString_len);

   /* convert encodings */
    notif.text_encoding = convertNiEncoding(umts_cp_req->dataCodingScheme);

    notif.requestor_id_encoding =
      convertNiEncoding(umts_cp_req->requestorId.dataCodingScheme);

    /* LCS address (using extras field) */
    if ( umts_cp_req->clientAddress_len != 0)
    {
      char lcs_addr[32]; // Decoded LCS address for UMTS CP NI

      // Copy LCS Address into notif.extras in the format: Address = 012345
      strlcat(notif.extras, LOC_NI_NOTIF_KEY_ADDRESS, sizeof (notif.extras));
      strlcat(notif.extras, " = ", sizeof notif.extras);
      int addr_len = 0;
      const char *address_source = NULL;
      address_source = (char *)umts_cp_req->clientAddress;
      // client Address is always NULL terminated
      addr_len = decodeAddress(lcs_addr, sizeof(lcs_addr), address_source,
                               umts_cp_req->clientAddress_len);

      // The address is ASCII string
      if (addr_len)
      {
        strlcat(notif.extras, lcs_addr, sizeof notif.extras);
      }
    }

  }
  else if(ni_req_ptr->NiSuplInd_valid == 1)
  {
    const qmiLocNiSuplNotifyVerifyStructT_v02 *supl_req =
      &ni_req_ptr->NiSuplInd;

    notif.ni_type     = GPS_NI_TYPE_UMTS_SUPL;

    // Client name
    if (supl_req->valid_flags & QMI_LOC_SUPL_CLIENT_NAME_MASK_V02)
    {
      hexcode(notif.text, sizeof(notif.text),
              (char *)supl_req->clientName.formattedString,
              supl_req->clientName.formattedString_len);
      LOC_LOGV("%s:%d]: SUPL NI: client_name: %s \n", __func__, __LINE__,
          notif.text);
    }
    else
    {
      LOC_LOGV("%s:%d]: SUPL NI: client_name not present.",
          __func__, __LINE__);
    }

    // Requestor ID
    if (supl_req->valid_flags & QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02)
    {
      hexcode(notif.requestor_id, sizeof notif.requestor_id,
              (char*)supl_req->requestorId.formattedString,
              supl_req->requestorId.formattedString_len );

      LOC_LOGV("%s:%d]: SUPL NI: requestor_id: %s \n", __func__, __LINE__,
          notif.requestor_id);
    }
    else
    {
      LOC_LOGV("%s:%d]: SUPL NI: requestor_id not present.",
          __func__, __LINE__);
    }

    // Encoding type
    if (supl_req->valid_flags & QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02)
    {
      notif.text_encoding = convertNiEncoding(supl_req->dataCodingScheme);

      notif.requestor_id_encoding = convertNiEncoding(supl_req->dataCodingScheme);
    }
    else
    {
      notif.text_encoding = notif.requestor_id_encoding = GPS_ENC_UNKNOWN;
    }

  } //ni_req_ptr->NiSuplInd_valid == 1

  else
  {
    LOC_LOGE("%s:%d]: unknown request event \n",__func__, __LINE__);
    return;
  }

  // Set default_response & notify_flags
  convertNiNotifyVerifyType(&notif, ni_req_ptr->notificationType);

  qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *ni_req_copy_ptr =
    (qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *)malloc(sizeof(*ni_req_copy_ptr));

  if( NULL != ni_req_copy_ptr)
  {
    memcpy(ni_req_copy_ptr, ni_req_ptr, sizeof(*ni_req_copy_ptr));

    LocApiAdapter::requestNiNotify(notif, (const void*)ni_req_copy_ptr);
  }
  else
  {
    LOC_LOGE("%s:%d]: Error copying NI request\n", __func__, __LINE__);
  }

}

/* convert Ni Encoding type from QMI_LOC to loc eng format */
GpsNiEncodingType LocApiV02Adapter ::convertNiEncoding(
  qmiLocNiDataCodingSchemeEnumT_v02 loc_encoding)
{
   GpsNiEncodingType enc = GPS_ENC_UNKNOWN;

   switch (loc_encoding)
   {
     case eQMI_LOC_NI_SUPL_UTF8_V02:
       enc = GPS_ENC_SUPL_UTF8;
       break;
     case eQMI_LOC_NI_SUPL_UCS2_V02:
       enc = GPS_ENC_SUPL_UCS2;
       break;
     case eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02:
       enc = GPS_ENC_SUPL_GSM_DEFAULT;
       break;
     case eQMI_LOC_NI_SS_LANGUAGE_UNSPEC_V02:
       enc = GPS_ENC_SUPL_GSM_DEFAULT; // SS_LANGUAGE_UNSPEC = GSM
       break;
     default:
       break;
   }

   return enc;
}

/*convert NI notify verify type from QMI LOC to loc eng format*/
bool LocApiV02Adapter :: convertNiNotifyVerifyType (
  GpsNiNotification *notif,
  qmiLocNiNotifyVerifyEnumT_v02 notif_priv)
{
  switch (notif_priv)
   {
   case eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02:
      notif->notify_flags = 0;
      break;

   case eQMI_LOC_NI_USER_NOTIFY_ONLY_V02:
      notif->notify_flags = GPS_NI_NEED_NOTIFY;
      break;

   case eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02:
      notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
      notif->default_response = GPS_NI_RESPONSE_ACCEPT;
      break;

   case eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02:
      notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
      notif->default_response = GPS_NI_RESPONSE_DENY;
      break;

   case eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02:
      notif->notify_flags = GPS_NI_PRIVACY_OVERRIDE;
      break;

   default:
      return false;
   }

   return true;
}

/* event callback registered with the loc_api v02 interface */
void LocApiV02Adapter :: eventCb(locClientHandleType clientHandle,
  uint32_t eventId, locClientEventIndUnionType eventPayload)
{
  LOC_LOGD("%s:%d]: event id = %d\n", __func__, __LINE__,
                eventId);

  switch(eventId)
  {
    //Position Report
    case QMI_LOC_EVENT_POSITION_REPORT_IND_V02:
      reportPosition(eventPayload.pPositionReportEvent);
      break;

    // Satellite report
    case QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02:
      reportSv(eventPayload.pGnssSvInfoReportEvent);
      break;

    // Status report
    case QMI_LOC_EVENT_ENGINE_STATE_IND_V02:
      reportEngineState(eventPayload.pEngineState);
      break;

    case QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02:
      reportFixSessionState(eventPayload.pFixSessionState);
      break;

    // NMEA
    case QMI_LOC_EVENT_NMEA_IND_V02:
      reportNmea(eventPayload.pNmeaReportEvent);
      break;

    // XTRA request
    case QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02:
      LOC_LOGD("%s:%d]: XTRA download request\n", __func__,
                    __LINE__);
      requestXtraData();
      break;

    // time request
    case QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02:
      LOC_LOGD("%s:%d]: Time request\n", __func__,
                    __LINE__);
      requestTime();
      break;

    //position request
    case QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02:
      LOC_LOGD("%s:%d]: Position request\n", __func__,
                    __LINE__);
      //requestPosition();
      break;

    // NI request
    case QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02:
      reportNiRequest(eventPayload.pNiNotifyVerifyReqEvent);
      break;

    // AGPS connection request
    case QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02:
      reportAtlRequest(eventPayload.pLocationServerConnReqEvent);
      break;
  }
}

/* Call the service LocApiAdapter down event*/
void LocApiV02Adapter :: errorCb(locClientHandleType handle,
                                 locClientErrorEnumType errorId)
{
  if(errorId == eLOC_CLIENT_ERROR_SERVICE_UNAVAILABLE)
  {
    LOC_LOGE("%s:%d]: Service unavailable error\n",
                  __func__, __LINE__);

    LocApiAdapter::handleEngineDownEvent();

    /* immediately send the engine up event so that
    the loc engine re-initializes the adapter and the
    loc-api_v02 interface */

    LocApiAdapter::handleEngineUpEvent();
  }
}

/* return a Loc API adapter */
extern "C" LocApiAdapter* getLocApiAdapter(LocEng &locEng)
{
 return(new LocApiV02Adapter(locEng));
}
