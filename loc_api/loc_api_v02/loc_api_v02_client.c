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

#include "qmi_client.h"
#include "qmi_idl_lib.h"
#include "qmi_cci_target_ext.h"

#if defined( _ANDROID_)
#include "qmi_cci_target.h"
#include "qmi_cci_common.h"
#elif defined(LOC_UTIL_TARGET_OFF_TARGET)
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#endif //_ANDROID_

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include "loc_api_v02_client.h"
#include "loc_api_v02_log.h"

#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_api_v02"

#include "loc_util_log.h"

#ifdef LOC_UTIL_TARGET_OFF_TARGET

// timeout in ms before send_msg_sync should return
#define LOC_CLIENT_ACK_TIMEOUT (5000)

// timeout in ms before a sync request should return
#define LOC_CLIENT_SYNC_REQ_TIMEOUT (5000)

#else

// timeout in ms before send_msg_sync should return
#define LOC_CLIENT_ACK_TIMEOUT (1000)

// timeout in ms before a sync request should return
#define LOC_CLIENT_SYNC_REQ_TIMEOUT (1000)

#endif //LOC_UTIL_TARGET_OFF_TARGET

//timeout in ms to wait for the service to come up
#define LOC_CLIENT_SERVICE_TIMEOUT_UNIT  (4000)
#define LOC_CLIENT_SERVICE_TIMEOUT_TOTAL  (60000)

/* Table to relate eventId, size and mask value used to enable the event*/
typedef struct
{
  uint32_t               eventId;
  size_t                 eventSize;
  locClientEventMaskType eventMask;
}locClientEventIndTableStructT;


static locClientEventIndTableStructT locClientEventIndTable[]= {

  // position report ind
  { QMI_LOC_EVENT_POSITION_REPORT_IND_V02,
    sizeof(qmiLocEventPositionReportIndMsgT_v02),
    QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 },

  // satellite report ind
  { QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02,
    sizeof(qmiLocEventGnssSvInfoIndMsgT_v02),
    QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 },

  // NMEA report ind
  { QMI_LOC_EVENT_NMEA_IND_V02,
    sizeof(qmiLocEventNmeaIndMsgT_v02),
    QMI_LOC_EVENT_MASK_NMEA_V02 },

  //NI event ind
  { QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02,
    sizeof(qmiLocEventNiNotifyVerifyReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02 },

  //Time Injection Request Ind
  { QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02,
    sizeof(qmiLocEventInjectTimeReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02 },

  //Predicted Orbits Injection Request
  { QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02,
    sizeof(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02 },

  //Position Injection Request Ind
  { QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02,
    sizeof(qmiLocEventInjectPositionReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02 } ,

  //Engine State Report Ind
  { QMI_LOC_EVENT_ENGINE_STATE_IND_V02,
    sizeof(qmiLocEventEngineStateIndMsgT_v02),
    QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 },

  //Fix Session State Report Ind
  { QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02,
    sizeof(qmiLocEventFixSessionStateIndMsgT_v02),
    QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02 },

  //Wifi Request Indication
  { QMI_LOC_EVENT_WIFI_REQ_IND_V02,
    sizeof(qmiLocEventWifiReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_WIFI_REQ_V02 },

  //Sensor Streaming Ready Status Ind
  { QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02,
    sizeof(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02),
    QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS_V02 },

  // Time Sync Request Indication
  { QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02,
    sizeof(qmiLocEventTimeSyncReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_TIME_SYNC_REQ_V02 },

  //Set Spi Streaming Report Event
  { QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02,
    sizeof(qmiLocEventSetSpiStreamingReportIndMsgT_v02),
    QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT_V02 },

  //Location Server Connection Request event
  { QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02,
    sizeof(qmiLocEventLocationServerConnectionReqIndMsgT_v02),
    QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02 }
};

/* table to relate the respInd Id with its size */
typedef struct
{
  uint32_t respIndId;
  size_t   respIndSize;
}locClientRespIndTableStructT;

static locClientRespIndTableStructT locClientRespIndTable[]= {

  // get service revision ind
  { QMI_LOC_GET_SERVICE_REVISION_IND_V02,
    sizeof(qmiLocGetServiceRevisionIndMsgT_v02)},

  // Get Fix Criteria Resp Ind
  { QMI_LOC_GET_FIX_CRITERIA_IND_V02,
     sizeof(qmiLocGetFixCriteriaIndMsgT_v02)},

  // NI User Resp In
  { QMI_LOC_NI_USER_RESPONSE_IND_V02,
    sizeof(qmiLocNiUserRespIndMsgT_v02)},

  //Inject Predicted Orbits Data Resp Ind
  { QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02,
    sizeof(qmiLocInjectPredictedOrbitsDataIndMsgT_v02)},

  //Get Predicted Orbits Data Src Resp Ind
  { QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02,
    sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02)},

  // Get Predicted Orbits Data Validity Resp Ind
   { QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02,
     sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02)},

   // Inject UTC Time Resp Ind
   { QMI_LOC_INJECT_UTC_TIME_IND_V02,
     sizeof(qmiLocInjectUtcTimeIndMsgT_v02)},

   //Inject Position Resp Ind
   { QMI_LOC_INJECT_POSITION_IND_V02,
     sizeof(qmiLocInjectPositionIndMsgT_v02)},

   //Set Engine Lock Resp Ind
   { QMI_LOC_SET_ENGINE_LOCK_IND_V02,
     sizeof(qmiLocSetEngineLockIndMsgT_v02)},

   //Get Engine Lock Resp Ind
   { QMI_LOC_GET_ENGINE_LOCK_IND_V02,
     sizeof(qmiLocGetEngineLockIndMsgT_v02)},

   //Set SBAS Config Resp Ind
   { QMI_LOC_SET_SBAS_CONFIG_IND_V02,
     sizeof(qmiLocSetSbasConfigIndMsgT_v02)},

   //Get SBAS Config Resp Ind
   { QMI_LOC_GET_SBAS_CONFIG_IND_V02,
     sizeof(qmiLocGetSbasConfigIndMsgT_v02)},

   //Set NMEA Types Resp Ind
   { QMI_LOC_SET_NMEA_TYPES_IND_V02,
     sizeof(qmiLocSetNmeaTypesIndMsgT_v02)},

   //Get NMEA Types Resp Ind
   { QMI_LOC_GET_NMEA_TYPES_IND_V02,
     sizeof(qmiLocGetNmeaTypesIndMsgT_v02)},

   //Set Low Power Mode Resp Ind
   { QMI_LOC_SET_LOW_POWER_MODE_IND_V02,
     sizeof(qmiLocSetLowPowerModeIndMsgT_v02)},

   //Get Low Power Mode Resp Ind
   { QMI_LOC_GET_LOW_POWER_MODE_IND_V02,
     sizeof(qmiLocGetLowPowerModeIndMsgT_v02)},

   //Set Server Resp Ind
   { QMI_LOC_SET_SERVER_IND_V02,
     sizeof(qmiLocSetServerIndMsgT_v02)},

   //Get Server Resp Ind
   { QMI_LOC_GET_SERVER_IND_V02,
     sizeof(qmiLocGetServerIndMsgT_v02)},

    //Delete Assist Data Resp Ind
   { QMI_LOC_DELETE_ASSIST_DATA_IND_V02,
     sizeof(qmiLocDeleteAssistDataIndMsgT_v02)},

   //Set XTRA-T Session Control Resp Ind
   { QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02,
     sizeof(qmiLocSetXtraTSessionControlIndMsgT_v02)},

   //Get XTRA-T Session Control Resp Ind
   { QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02,
     sizeof(qmiLocGetXtraTSessionControlIndMsgT_v02)},

   //Inject Wifi Position Resp Ind
   { QMI_LOC_INJECT_WIFI_POSITION_IND_V02,
     sizeof(qmiLocInjectWifiPositionIndMsgT_v02)},

   //Notify Wifi Status Resp Ind
   { QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02,
     sizeof(qmiLocNotifyWifiStatusIndMsgT_v02)},

   //Get Registered Events Resp Ind
   { QMI_LOC_GET_REGISTERED_EVENTS_IND_V02,
     sizeof(qmiLocGetRegisteredEventsIndMsgT_v02)},

   //Set Operation Mode Resp Ind
   { QMI_LOC_SET_OPERATION_MODE_IND_V02,
     sizeof(qmiLocSetOperationModeIndMsgT_v02)},

   //Get Operation Mode Resp Ind
   { QMI_LOC_GET_OPERATION_MODE_IND_V02,
     sizeof(qmiLocGetOperationModeIndMsgT_v02)},

   //Set SPI Status Resp Ind
   { QMI_LOC_SET_SPI_STATUS_IND_V02,
     sizeof(qmiLocSetSpiStatusIndMsgT_v02)},

   //Inject Sensor Data Resp Ind
   { QMI_LOC_INJECT_SENSOR_DATA_IND_V02,
     sizeof(qmiLocInjectSensorDataIndMsgT_v02)},

   //Inject Time Sync Data Resp Ind
   { QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02,
     sizeof(qmiLocInjectTimeSyncDataIndMsgT_v02)},

   //Set Cradle Mount config Resp Ind
   { QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02,
     sizeof(qmiLocSetCradleMountConfigIndMsgT_v02)},

   //Get Cradle Mount config Resp Ind
   { QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02,
     sizeof(qmiLocGetCradleMountConfigIndMsgT_v02)},

   //Set External Power config Resp Ind
   { QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02,
     sizeof(qmiLocSetExternalPowerConfigIndMsgT_v02)},

   //Get External Power config Resp Ind
   { QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02,
     sizeof(qmiLocGetExternalPowerConfigIndMsgT_v02)},

   //Location server connection status
   { QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02,
     sizeof(qmiLocInformLocationServerConnStatusIndMsgT_v02)},

   //Set Protocol Config Parameters
   { QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02,
     sizeof(qmiLocSetProtocolConfigParametersIndMsgT_v02)},

   //Get Protocol Config Parameters
   { QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02,
     sizeof(qmiLocGetProtocolConfigParametersIndMsgT_v02)},

   //Set Sensor Control Config
   { QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02,
     sizeof(qmiLocSetSensorControlConfigIndMsgT_v02)},

   //Get Sensor Control Config
   { QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02,
     sizeof(qmiLocGetSensorControlConfigIndMsgT_v02)},

   //Set Sensor Properties
   { QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02,
     sizeof(qmiLocSetSensorPropertiesIndMsgT_v02)},

   //Get Sensor Properties
   { QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02,
     sizeof(qmiLocGetSensorPropertiesIndMsgT_v02)},

   //Set Sensor Performance Control Config
   { QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02,
     sizeof(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02)},

   //Get Sensor Performance Control Config
   { QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02,
     sizeof(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02)},
   //Inject SUPL certificate
   { QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02,
     sizeof(qmiLocInjectSuplCertificateIndMsgT_v02) },

   //Delete SUPL certificate
   { QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02,
     sizeof(qmiLocDeleteSuplCertificateIndMsgT_v02) },

   // Set Position Engine Config
   { QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02,
     sizeof(qmiLocSetPositionEngineConfigParametersIndMsgT_v02)},

   // Get Position Engine Config
   { QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02,
     sizeof(qmiLocGetPositionEngineConfigParametersIndMsgT_v02)}

};


/** whether indication is an event or a response */
typedef enum { eventIndType =0, respIndType = 1 } locClientIndEnumT;


/** @struct locClientInternalState
 */

typedef struct
{
 // client cookie
  void *pClientCookie;
  //QCCI handle for this control point
  qmi_client_type userHandle;

  // callbacks registered by the clients
  locClientEventIndCbType eventCallback;
  locClientRespIndCbType respCallback;
  locClientErrorCbType   errorCallback;

  // the event mask the client has registered for
  locClientEventMaskType eventRegMask;

}locClientCallbackDataType;

/*===========================================================================
 *
 *                          FUNCTION DECLARATION
 *
 *==========================================================================*/

/** locClientGetSizeAndTypeByIndId
 *  @brief this function gets the size and the type (event,
 *         response)of the indication structure from its ID
 *  @param [in]  indId  ID of the indication
 *  @param [out] type   event or response indication
 *  @param [out] size   size of the indications
 *
 *  @return true if the ID was found, false otherwise */

static bool locClientGetSizeAndTypeByIndId (uint32_t indId, size_t *pIndSize,
                                         locClientIndEnumT *pIndType)
{
  // look in the event table
  if(true == locClientGetSizeByEventIndId(indId, pIndSize))
  {
    *pIndType = eventIndType;

    LOC_LOGV("%s:%d]: indId %d is an event size = %d\n", __func__, __LINE__,
                  indId, (uint32_t)*pIndSize);
    return true;
  }

  //else look in response table
  if(true == locClientGetSizeByRespIndId(indId, pIndSize))
  {
    *pIndType = respIndType;

    LOC_LOGV("%s:%d]: indId %d is a resp size = %d\n", __func__, __LINE__,
                  indId, (uint32_t)*pIndSize);
    return true;
  }

  // Id not found
  LOC_LOGW("%s:%d]: indId %d not found\n", __func__, __LINE__, indId);
  return false;
}

/** isClientRegisteredForEvent
*  @brief checks the mask to identify if the client has
*         registered for the specified event Id
*  @param [in] eventIndId
*  @param [in] eventRegMask
*  @return true if client regstered for event; else false */

static bool isClientRegisteredForEvent(
    locClientEventMaskType eventRegMask,
    uint32_t eventIndId)
{
  size_t idx = 0, eventIndTableSize = 0;

  // look in the event table
  eventIndTableSize =
    (sizeof(locClientEventIndTable)/sizeof(locClientEventIndTableStructT));

  for(idx=0; idx<eventIndTableSize; idx++ )
  {
    if(eventIndId == locClientEventIndTable[idx].eventId)
    {
      LOC_LOGV("%s:%d]: eventId %d registered mask = %llu, "
                    "eventMask = %llu\n", __func__, __LINE__,
                     eventIndId, eventRegMask,
                     locClientEventIndTable[idx].eventMask);

      return((
          eventRegMask & locClientEventIndTable[idx].eventMask)?
          true:false);
    }
  }
  LOC_LOGW("%s:%d]: eventId %d not found\n", __func__, __LINE__,
                 eventIndId);
  return false;
}

/** convertQmiResponseToLocStatus
 @brief converts a qmiLocGenRespMsgT to locClientStatusEnumType*
 @param [in] pResponse; pointer to the response received from
        QMI_LOC service.
 @return locClientStatusEnumType corresponding to the
         response.
*/

static locClientStatusEnumType convertQmiResponseToLocStatus(
  qmiLocGenRespMsgT_v02 *pResponse)
{
  locClientStatusEnumType status =  eLOC_CLIENT_FAILURE_INTERNAL;

  // if result == SUCCESS don't look at error code
  if(pResponse->resp.result == QMI_RESULT_SUCCESS )
  {
    status  = eLOC_CLIENT_SUCCESS;
  }
  else
  {
    switch(pResponse->resp.error)
    {
      case QMI_ERR_MALFORMED_MSG_V01:
        status = eLOC_CLIENT_FAILURE_INVALID_PARAMETER;
        break;

      case QMI_ERR_DEVICE_IN_USE_V01:
        status = eLOC_CLIENT_FAILURE_ENGINE_BUSY;
        break;

      default:
        status = eLOC_CLIENT_FAILURE_INTERNAL;
        break;
    }
  }
  LOC_LOGV("%s:%d]: result = %d, error = %d, status = %d\n",
                __func__, __LINE__, pResponse->resp.result,
                pResponse->resp.error, status);
  return status;
}

/** convertQmiErrorToLocError
 @brief converts a qmi service error type to
        locClientErrorEnumType
 @param [in] error received QMI service.
 @return locClientErrorEnumType corresponding to the error.
*/

static locClientErrorEnumType convertQmiErrorToLocError(
  qmi_client_error_type error)
{
  locClientErrorEnumType locError ;
  switch(error)
  {
    case QMI_SERVICE_ERR:
      locError = eLOC_CLIENT_ERROR_SERVICE_UNAVAILABLE;
      break;

    default:
      locError = eLOC_CLIENT_ERROR_SERVICE_UNAVAILABLE;
      break;
  }
  LOC_LOGV("%s:%d]: qmi error = %d, loc error = %d\n",
                __func__, __LINE__, error, locError);
  return locError;
}

/** locClienHandlePosReportInd
 *  @brief Validates a position report ind
 *  @param [in] msg_id
 *  @param [in] ind_buf
 *  @param [in] ind_buf_len
 *  @return true if pos report is valid, false otherwise
*/
static bool locClientHandlePosReportInd
(
 uint32_t        msg_id,
 const void*     ind_buf,
 uint32_t        ind_buf_len
)
{
  // validate position report
  qmiLocEventPositionReportIndMsgT_v02 *posReport =
    (qmiLocEventPositionReportIndMsgT_v02 *)ind_buf;

  LOC_LOGV ("%s:%d]: len = %d lat = %f, lon = %f, alt = %f\n",
                 __func__, __LINE__, ind_buf_len,
                 posReport->latitude, posReport->longitude,
                 posReport->altitudeWrtEllipsoid);

  return true;
}
//-----------------------------------------------------------------------------

/** locClientHandleSatReportInd
 *  @brief Validates a satellite report indication. Dk
 *  @param [in] msg_id
 *  @param [in] ind_buf
 *  @param [in] ind_buf_len
 *  @return true if sat report is valid, false otherwise
*/
static bool locClientHandleSatReportInd
(
 uint32_t        msg_id,
 const void*     ind_buf,
 uint32_t        ind_buf_len
)
{
  // validate sat reports
  unsigned int idx = 0;
  qmiLocEventGnssSvInfoIndMsgT_v02 *satReport =
    (qmiLocEventGnssSvInfoIndMsgT_v02 *)ind_buf;

  LOC_LOGV ("%s:%d]: len = %u , altitude assumed = %u, num SV's = %u"
                 " validity = %d \n ", __func__, __LINE__,
                 ind_buf_len, satReport->altitudeAssumed,
                 satReport->svList_len, satReport->svList_valid);
  //Log SV report
  for( idx = 0; idx <satReport->svList_len; idx++ )
  {
    LOC_LOGV("%s:%d]: valid_mask = %x, prn = %u, system = %d, "
                  "status = %d \n", __func__, __LINE__,
                  satReport->svList[idx].validMask, satReport->svList[idx].gnssSvId,
                  satReport->svList[idx].system, satReport->svList[idx].svStatus);
  }

   return true;
}


/** locClientHandleNmeaReportInd
 *  @brief Validate a NMEA report indication.
 *  @param [in] msg_id
 *  @param [in] ind_buf
 *  @param [in] ind_buf_len
 *  @return true if nmea report is valid, false otherwise
*/


static bool locClientHandleNmeaReportInd
(
 uint32_t        msg_id,
 const void*     ind_buf,
 uint32_t        ind_buf_len
)
{
 // validate NMEA report
  return true;
}

/** locClientHandleGetServiceRevisionRespInd
 *  @brief Handles a Get Service Revision Rresponse indication.
 *  @param [in] msg_id
 *  @param [in] ind_buf
 *  @param [in] ind_buf_len
 *  @return true if service revision is valid, false otherwise
*/

static bool locClientHandleGetServiceRevisionRespInd
(
 uint32_t        msg_id,
 const void*     ind_buf,
 uint32_t        ind_buf_len
)
{
  LOC_LOGV("%s:%d] :\n", __func__, __LINE__);
  return true;
}

/** locClientHandleIndication
 *  @brief looks at each indication and calls the appropriate
 *         validation handler
 *  @param [in] indId
 *  @param [in] indBuffer
 *  @param [in] indSize
 *  @return true if indication was validated; else false */

static bool locClientHandleIndication(
  uint32_t        indId,
  void*           indBuffer,
  size_t          indSize
 )
{
  bool status = false;
  switch(indId)
  {
    // handle the event indications
    //-------------------------------------------------------------------------

    // handle position report
    case QMI_LOC_EVENT_POSITION_REPORT_IND_V02:
    {
      status = locClientHandlePosReportInd(indId, indBuffer, indSize);
      break;
    }
    // handle satellite report
    case QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02:
    {
      status = locClientHandleSatReportInd(indId, indBuffer, indSize);
      break;
    }

    // handle NMEA report
    case QMI_LOC_EVENT_NMEA_IND_V02:
    {
      status = locClientHandleNmeaReportInd(indId, indBuffer, indSize);
      break;
    }

    // handle NI Notify Verify Request Ind
    case QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02:
    {
     // locClientHandleNiReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle Time Inject request Ind
    case QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02:
    {
     // locClientHandleTimeInjectReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle XTRA data Inject request Ind
    case QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02:
    {
     // locClientHandleXtraInjectReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle position inject request Ind
    case QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02:
    {
     // locClientHandlePositionInjectReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle engine state Ind
    case QMI_LOC_EVENT_ENGINE_STATE_IND_V02:
    {
     // locClientHandleEngineStateInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle fix session state Ind
    case QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02:
    {
     // locClientHandleFixSessionStateInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle Wifi request Ind
    case QMI_LOC_EVENT_WIFI_REQ_IND_V02:
    {
     // locClientHandleWifiReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle sensor streaming ready status Ind
    case QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02:
    {
     // locClientHandleSensorStreamingReadyInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle time sync  Ind
    case QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02:
    {
     // locClientHandleTimeSyncReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    // handle set streaming report ind
    case QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02:
    {
     // locClientHandleSetSpiStreamingInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02:
    {
      //locClientHandleLocServerConnReqInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    //-------------------------------------------------------------------------

    // handle the response indications
    //-------------------------------------------------------------------------

    // Get service Revision response indication
    case QMI_LOC_GET_SERVICE_REVISION_IND_V02:
    {
      status = locClientHandleGetServiceRevisionRespInd(indId,
                                                        indBuffer, indSize);
      break;
    }
    // predicted orbits data response indication
    case QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02:
    {
      //locClientHandleInjectPredictedOrbitsDataInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_INJECT_SENSOR_DATA_IND_V02 :
    {
      //locClientHandleInjectSensorDataInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02:
    {
      //locClientHandleGetProtocolConfigParametersInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02:
    {
      //locClientHandleSetProtocolConfigParametersInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02:
    {
      //locClientHandleGetExtPowerConfigInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02:
    {
      //locClientHandleGetCradleMountConfigInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02:
    {
      //locClientHandleGetSensorControlConfigInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02:
    {
      //locClientHandleGetSensorPerformanceControlConfigInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02:
    {
      //locClientHandleGetSensorPropertiesInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02:
    {
      //locClientHandleSetSensorPerformanceControlConfigInd(user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02:
    {
    // locClientHandleSetPositionEngineConfigParam(
    //     user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }

    case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02:
    {
      // locClientHandleSetPositionEngineConfigParam(
      //     user_handle, msg_id, ind_buf, ind_buf_len);
      status = true;
      break;
    }
    // for indications that only have a "status" field
    case QMI_LOC_NI_USER_RESPONSE_IND_V02:
    case QMI_LOC_INJECT_UTC_TIME_IND_V02:
    case QMI_LOC_INJECT_POSITION_IND_V02:
    case QMI_LOC_SET_ENGINE_LOCK_IND_V02:
    case QMI_LOC_SET_SBAS_CONFIG_IND_V02:
    case QMI_LOC_SET_NMEA_TYPES_IND_V02:
    case QMI_LOC_SET_LOW_POWER_MODE_IND_V02:
    case QMI_LOC_SET_SERVER_IND_V02:
    case QMI_LOC_DELETE_ASSIST_DATA_IND_V02:
    case QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02:
    case QMI_LOC_INJECT_WIFI_POSITION_IND_V02:
    case QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02:
    case QMI_LOC_SET_OPERATION_MODE_IND_V02:
    case QMI_LOC_SET_SPI_STATUS_IND_V02:
    case QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02:
    case QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02:
    case QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02:
    case QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02:
    case QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02:
    case QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02:
    case QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02:
    case QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02:
    {
      status = true;
      break;
    }
    default:
      LOC_LOGW("%s:%d]: unknown ind id %d\n", __func__, __LINE__,
                   (uint32_t)indId);
      status = false;
      break;
  }
  return status;
}


/** locClientErrorCb
 *  @brief handles the QCCI error events, this is called by the
 *         QCCI infrastructure when the service is no longer
 *         available.
 *  @param [in] user handle
 *  @param [in] error
 *  @param [in] *err_cb_data
 */

static void locClientErrorCb
(
  qmi_client_type user_handle,
  qmi_client_error_type error,
  void *err_cb_data
)
{
  locClientCallbackDataType* pCallbackData =
        (locClientCallbackDataType *)err_cb_data;

  /* copy the errorCallback function pointer from the callback
   * data to local variable. This is to protect against the race
   * condition between open/close and error callback.
   */
  locClientErrorCbType localErrorCallback =
      pCallbackData->errorCallback;

  LOC_LOGD("%s:%d]: Service Error %d received, pCallbackData = %p\n",
      __func__, __LINE__, error, err_cb_data);

  /* call the error callback
   * To avoid calling the errorCallback after locClientClose
   * is called, check pCallbackData->errorCallback again here
   */

  if( (NULL != pCallbackData) &&
      (NULL != localErrorCallback) &&
      (NULL != pCallbackData->errorCallback) )
  {
    //invoke the error callback for the corresponding client
    localErrorCallback(
        (locClientHandleType)pCallbackData,
        convertQmiErrorToLocError(error),
        pCallbackData->pClientCookie);
  }
}


/** locClientIndCb
 *  @brief handles the indications sent from the service, if a
 *         response indication was received then the it is sent
 *         to the response callback. If a event indication was
 *         received then it is sent to the event callback
 *  @param [in] user handle
 *  @param [in] msg_id
 *  @param [in] ind_buf
 *  @param [in] ind_buf_len
 *  @param [in] ind_cb_data */

static void locClientIndCb
(
 qmi_client_type                user_handle,
 unsigned int                   msg_id,
 void                           *ind_buf,
 unsigned int                   ind_buf_len,
 void                           *ind_cb_data
)
{
  locClientIndEnumT indType;
  size_t indSize = 0;
  qmi_client_error_type rc ;
  locClientCallbackDataType* pCallbackData =
      (locClientCallbackDataType *)ind_cb_data;

  LOC_LOGV("%s:%d]: Indication: msg_id=%d buf_len=%d pCallbackData = %p\n",
                __func__, __LINE__, (uint32_t)msg_id, ind_buf_len,
                pCallbackData);

  // check callback data
  if(NULL == pCallbackData)
  {
    LOC_LOGE("%s:%d]: invalid callback data", __func__, __LINE__);
    return;
  }

  // check user handle
  if(memcmp(&pCallbackData->userHandle, &user_handle, sizeof(user_handle)))
  {
    LOC_LOGE("%s:%d]: invalid user_handle got 0x%x expected 0x%x\n",
        __func__, __LINE__,
        user_handle, pCallbackData->userHandle);
    return;
  }
  // Get the indication size and type ( eventInd or respInd)
  if( true == locClientGetSizeAndTypeByIndId(msg_id, &indSize, &indType))
  {
    void *indBuffer = NULL;

    // if the client did not register for this event then just drop it
     if( (eventIndType == indType) &&
         ( (NULL == pCallbackData->eventCallback) ||
         (false == isClientRegisteredForEvent(pCallbackData->eventRegMask, msg_id)) ) )
    {
       LOC_LOGW("%s:%d]: client is not registered for event %d\n",
                     __func__, __LINE__, (uint32_t)msg_id);
       return;
    }

    // decode the indication
    indBuffer = malloc(indSize);

    if(NULL == indBuffer)
    {
      LOC_LOGE("%s:%d]: memory allocation failed\n", __func__, __LINE__);
      return;
    }

    // decode the indication
    rc = qmi_client_message_decode(
        user_handle,
        QMI_IDL_INDICATION,
        msg_id,
        ind_buf,
        ind_buf_len,
        indBuffer,
        indSize);

    if( rc == QMI_NO_ERR )
    {
      //validate indication
      if (true == locClientHandleIndication(msg_id, indBuffer, indSize))
      {
        if(eventIndType == indType)
        {
          locClientEventIndUnionType eventIndUnion;

          /* copy the eventCallback function pointer from the callback
           * data to local variable. This is to protect against the race
           * condition between open/close and indication callback.
           */
           locClientEventIndCbType localEventCallback =
               pCallbackData->eventCallback;

          // dummy event
          eventIndUnion.pPositionReportEvent =
            (qmiLocEventPositionReportIndMsgT_v02 *)indBuffer;

          /* call the event callback
           * To avoid calling the eventCallback after locClientClose
           * is called, check pCallbackData->eventCallback again here
           */
          if((NULL != localEventCallback) &&
              (NULL != pCallbackData->eventCallback))
          {
            localEventCallback(
                (locClientHandleType)pCallbackData,
                msg_id,
                eventIndUnion,
                pCallbackData->pClientCookie);
          }
        }
        else if(respIndType == indType)
        {
          locClientRespIndUnionType respIndUnion;

          /* copy the respCallback function pointer from the callback
           * data to local variable. This is to protect against the race
           * condition between open/close and indication callback.
           */
          locClientRespIndCbType localRespCallback =
              pCallbackData->respCallback;

          // dummy to suppress compiler warnings
          respIndUnion.pDeleteAssistDataInd =
            (qmiLocDeleteAssistDataIndMsgT_v02 *)indBuffer;

          /* call the response callback
           * To avoid calling the respCallback after locClientClose
           * is called, check pCallbackData->respCallback again here
           */
          if((NULL != localRespCallback) &&
              (NULL != pCallbackData->respCallback))
          {
            localRespCallback(
                (locClientHandleType)pCallbackData,
                msg_id,
                respIndUnion,
                pCallbackData->pClientCookie);
          }
        }
      }
      else // error handling indication
      {
        LOC_LOGE("%s:%d]: Error handling the indication %d\n",
                      __func__, __LINE__, (uint32_t)msg_id);
      }
    }
    else
    {
      LOC_LOGE("%s:%d]: Error decoding indication %d\n",
                    __func__, __LINE__, rc);
    }
    if(indBuffer)
    {
      free (indBuffer);
    }
  }
  else // Id not found
  {
    LOC_LOGE("%s:%d]: Error indication not found %d\n",
                  __func__, __LINE__,(uint32_t)msg_id);
  }
  return;
}


/** locClientRegisterEventMask
 *  @brief registers the event mask with loc service
 *  @param [in] clientHandle
 *  @param [in] eventRegMask
 *  @return true if indication was validated; else false */

static bool locClientRegisterEventMask(
    locClientHandleType clientHandle,
    locClientEventMaskType eventRegMask)
{
  locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
  locClientReqUnionType reqUnion;
  qmiLocRegEventsReqMsgT_v02 regEventsReq;

  memset(&regEventsReq, 0, sizeof(regEventsReq));

  regEventsReq.eventRegMask = eventRegMask;
  reqUnion.pRegEventsReq = &regEventsReq;

  status = locClientSendReq(clientHandle,
                            QMI_LOC_REG_EVENTS_REQ_V02,
                            reqUnion);

  if(eLOC_CLIENT_SUCCESS != status )
  {
    LOC_LOGE("%s:%d] status %s\n", __func__, __LINE__,
             loc_get_v02_client_status_name(status) );
    return false;
  }

  return true;
}

/**  validateRequest
  @brief validates the input request
  @param [in] reqId       request ID
  @param [in] reqPayload  Union of pointers to message payload
  @param [out] ppOutData  Pointer to void *data if successful
  @param [out] pOutLen    Pointer to length of data if succesful.
  @return false on failure, true on Success
*/

static bool validateRequest(
  uint32_t                    reqId,
  const locClientReqUnionType reqPayload,
  void                        **ppOutData,
  uint32_t                    *pOutLen )

{
  bool noPayloadFlag = false;

  LOC_LOGV("%s:%d]: reqId = %d\n", __func__, __LINE__, reqId);
  switch(reqId)
  {
    case QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInformClientRevisionReqMsgT_v02);
      break;
    }

    case QMI_LOC_REG_EVENTS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocRegEventsReqMsgT_v02);
       break;
    }

    case QMI_LOC_START_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocStartReqMsgT_v02);
       break;
    }

    case QMI_LOC_STOP_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocStopReqMsgT_v02);
       break;
    }

    case QMI_LOC_NI_USER_RESPONSE_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocNiUserRespReqMsgT_v02);
       break;
    }

    case QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectPredictedOrbitsDataReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_UTC_TIME_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectUtcTimeReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_POSITION_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectPositionReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_ENGINE_LOCK_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetEngineLockReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SBAS_CONFIG_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetSbasConfigReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_NMEA_TYPES_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetNmeaTypesReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_LOW_POWER_MODE_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetLowPowerModeReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SERVER_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetServerReqMsgT_v02);
      break;
    }

    case QMI_LOC_DELETE_ASSIST_DATA_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocDeleteAssistDataReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetXtraTSessionControlReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_WIFI_POSITION_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectWifiPositionReqMsgT_v02);
      break;
    }

    case QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocNotifyWifiStatusReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_OPERATION_MODE_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetOperationModeReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SPI_STATUS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetSpiStatusReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_SENSOR_DATA_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectSensorDataReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectTimeSyncDataReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetCradleMountConfigReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetExternalPowerConfigReqMsgT_v02);
      break;
    }

    case QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInformLocationServerConnStatusReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetProtocolConfigParametersReqMsgT_v02);
      break;
    }

    case QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocGetProtocolConfigParametersReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetSensorControlConfigReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetSensorPropertiesReqMsgT_v02);
      break;
    }

    case QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02);
      break;
    }

    case QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocInjectSuplCertificateReqMsgT_v02);
      break;
    }
    case QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocDeleteSuplCertificateReqMsgT_v02);
      break;
    }
    case QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocSetPositionEngineConfigParametersReqMsgT_v02);
      break;
    }
    case QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02:
    {
      *pOutLen = sizeof(qmiLocGetPositionEngineConfigParametersReqMsgT_v02);
      break;
    }

    // ALL requests with no payload
    case QMI_LOC_GET_SERVICE_REVISION_REQ_V02:
    case QMI_LOC_GET_FIX_CRITERIA_REQ_V02:
    case QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02:
    case QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02:
    case QMI_LOC_GET_ENGINE_LOCK_REQ_V02:
    case QMI_LOC_GET_SBAS_CONFIG_REQ_V02:
    case QMI_LOC_GET_NMEA_TYPES_REQ_V02:
    case QMI_LOC_GET_LOW_POWER_MODE_REQ_V02:
    case QMI_LOC_GET_SERVER_REQ_V02:
    case QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02:
    case QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02:
    case QMI_LOC_GET_OPERATION_MODE_REQ_V02:
    case QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02:
    case QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02:
    case QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02:
    case QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02:
    case QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02:
    {
      noPayloadFlag = true;
      break;
    }

    default:
      LOC_LOGW("%s:%d]: Error unknown reqId=%d\n", __func__, __LINE__,
                    reqId);
      return false;
  }
  if(true == noPayloadFlag)
  {
    *ppOutData = NULL;
    *pOutLen = 0;
  }
  else
  {
    //set dummy pointer for request union
    *ppOutData = (void*) reqPayload.pInformClientRevisionReq;
  }
  LOC_LOGV("%s:%d]: reqId=%d, len = %d\n", __func__, __LINE__,
                reqId, *pOutLen);
  return true;
}

/** locClientQmiCtrlPointInit
 @brief wait for the service to come up or timeout; when the
        service comes up initialize the control point and set
        internal handle and indication callback.
 @param pQmiClient,
*/

static locClientStatusEnumType locClientQmiCtrlPointInit(
    locClientCallbackDataType *pLocClientCbData)
{
  qmi_client_type clnt, notifier;
  bool notifierInitFlag = false;
  locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
  // instances of this service
  qmi_service_info *pServiceInfo = NULL;

  do
  {
  uint32_t num_services = 0, num_entries = 0;
  qmi_client_error_type rc = QMI_NO_ERR;
  bool nosignal = false;

  // Get the service object for the qmiLoc Service
  qmi_idl_service_object_type locClientServiceObject =
    loc_get_service_object_v02();

  // Verify that qmiLoc_get_service_object did not return NULL
  if (NULL == locClientServiceObject)
  {
      LOC_LOGE("%s:%d]: qmiLoc_get_service_object_v02 failed\n" ,
                  __func__, __LINE__ );
       status = eLOC_CLIENT_FAILURE_INTERNAL;
       break;
  }

    // get the service addressing information
    rc = qmi_client_get_service_list( locClientServiceObject, NULL, NULL,
                                      &num_services);
    LOC_LOGV("%s:%d]: qmi_client_get_service_list() first try rc %d, "
             "num_services %d", __func__, __LINE__, rc, num_services);

    if (rc != QMI_NO_ERR) {
        // bummer, service list is not up.
        // We need to try again after a timed wait
        qmi_client_os_params os_params;
        int timeout = 0;

        // register for service notification
        rc = qmi_client_notifier_init(locClientServiceObject, &os_params, &notifier);
        notifierInitFlag = (NULL != notifier);

        if (rc != QMI_NO_ERR) {
            LOC_LOGE("%s:%d]: qmi_client_notifier_init failed %d\n",
                     __func__, __LINE__, rc);
            status = eLOC_CLIENT_FAILURE_INTERNAL;
            break;
        }

        do {
            QMI_CCI_OS_SIGNAL_CLEAR(&os_params);
            /* If service is not up wait on a signal until the service is up
             * or a timeout occurs. */
            QMI_CCI_OS_SIGNAL_WAIT(&os_params, LOC_CLIENT_SERVICE_TIMEOUT_UNIT);
            nosignal = QMI_CCI_OS_SIGNAL_TIMED_OUT(&os_params);

            // get the service addressing information
            rc = qmi_client_get_service_list(locClientServiceObject, NULL, NULL,
                                             &num_services);

            timeout += LOC_CLIENT_SERVICE_TIMEOUT_UNIT;

            LOC_LOGV("%s:%d]: qmi_client_get_service_list() rc %d, nosignal %d, "
                     "total timeout %d", __func__, __LINE__, rc, nosignal, timeout);
        } while (timeout < LOC_CLIENT_SERVICE_TIMEOUT_TOTAL && nosignal && rc != QMI_NO_ERR);
    }

    if (0 == num_services || rc != QMI_NO_ERR) {
        if (!nosignal) {
            LOC_LOGE("%s:%d]: qmi_client_get_service_list failed even though"
                     "service is up !!!  Error %d \n", __func__, __LINE__, rc);
            status = eLOC_CLIENT_FAILURE_INTERNAL;
        } else {
            LOC_LOGE("%s:%d]: qmi_client_get_service_list failed after retries,"
                     " final Err %d", __func__, __LINE__, rc);
            status = eLOC_CLIENT_FAILURE_TIMEOUT;
        }
        break;
    }

    pServiceInfo =
      (qmi_service_info *)malloc(num_services * sizeof(qmi_service_info));

    if(NULL == pServiceInfo)
    {
      LOC_LOGE("%s:%d]: could not allocate memory for serviceInfo !!\n",
               __func__, __LINE__);

      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
    }

    //set the number of entries to get equal to the total number of
    //services.
    num_entries = num_services;
    //populate the serviceInfo
    rc = qmi_client_get_service_list( locClientServiceObject, pServiceInfo,
                                      &num_entries, &num_services);


    LOC_LOGV("%s:%d]: qmi_client_get_service_list()"
                  " returned %d num_entries = %d num_services = %d\n",
                  __func__, __LINE__,
                   rc, num_entries, num_services);

    if(rc != QMI_NO_ERR)
    {
      LOC_LOGE("%s:%d]: qmi_client_get_service_list Error %d \n",
                    __func__, __LINE__, rc);

      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
    }

  LOC_LOGV("%s:%d]: passing the pointer %p to qmi_client_init \n",
                    __func__, __LINE__, pLocClientCbData);

  // initialize the client
    //sent the address of the first service found
    // if IPC router is present, this will go to the service instance
    // enumerated over IPC router, else it will go over the next transport where
    // the service was enumerated.
    rc = qmi_client_init(&pServiceInfo[0], locClientServiceObject,
                       locClientIndCb, (void *) pLocClientCbData,
                       NULL, &clnt);

  if(rc != QMI_NO_ERR)
  {
    LOC_LOGE("%s:%d]: qmi_client_init error %d\n",
                  __func__, __LINE__, rc);

      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
  }

  LOC_LOGV("%s:%d]: passing the pointer %p to"
                "qmi_client_register_error_cb \n",
                 __func__, __LINE__, pLocClientCbData);

  // register error callback
  rc  = qmi_client_register_error_cb(clnt,
      locClientErrorCb, (void *) pLocClientCbData);

  if( QMI_NO_ERR != rc)
  {
    LOC_LOGE("%s:%d]: could not register QCCI error callback error:%d\n",
                  __func__, __LINE__, rc);

      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
  }

  // copy the clnt handle returned in qmi_client_init
  memcpy(&(pLocClientCbData->userHandle), &clnt, sizeof(qmi_client_type));

    status = eLOC_CLIENT_SUCCESS;

  } while(0);

  /* release the notifier handle */
  if(true == notifierInitFlag)
  {
    qmi_client_release(notifier);
  }

  if(NULL != pServiceInfo)
  {
    free((void *)pServiceInfo);
  }

  return status;
}
//----------------------- END INTERNAL FUNCTIONS ----------------------------------------

/** locClientOpen
  @brief Connects a location client to the location engine. If the connection
         is successful, returns a handle that the location client uses for
         future location operations.

  @param [in] eventRegMask     Mask of asynchronous events the client is
                               interested in receiving
  @param [in] eventIndCb       Function to be invoked to handle an event.
  @param [in] respIndCb        Function to be invoked to handle a response
                               indication.
  @param [out] locClientHandle Handle to be used by the client
                               for any subsequent requests.

  @return
  One of the following error codes:
  - eLOC_CLIENT_SUCCESS  -- If the connection is opened.
  - non-zero error code(see locClientStatusEnumType)--  On failure.
*/

locClientStatusEnumType locClientOpen (
  locClientEventMaskType         eventRegMask,
  const locClientCallbacksType*  pLocClientCallbacks,
  locClientHandleType*           pLocClientHandle,
  const void*                    pClientCookie)
{
  locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
  locClientCallbackDataType *pCallbackData = NULL;

  LOC_LOGV("%s:%d] \n", __func__, __LINE__);

  do
  {
    // check input parameters
    if( (NULL == pLocClientCallbacks) || (NULL == pLocClientHandle)
        || (NULL == pLocClientCallbacks->respIndCb) ||
        (pLocClientCallbacks->size != sizeof(locClientCallbacksType)))
    {
      LOC_LOGE("%s:%d]: Invalid parameters in locClientOpen\n",
                              __func__, __LINE__);
      status = eLOC_CLIENT_FAILURE_INVALID_PARAMETER;
      break;
    }

    // Allocate memory for the callback data
    pCallbackData =
        ( locClientCallbackDataType*)calloc(
            1, sizeof(locClientCallbackDataType));

    if(NULL == pCallbackData)
    {
      LOC_LOGE("%s:%d]: Could not allocate memory for callback data \n",
                        __func__, __LINE__);
      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
    }

    /* Initialize the QMI control point; this function will block
     * until a service is up or a timeout occurs. If the connection to
     * the service succeeds the callback data will be filled in with
     * a qmi_client value.
     */


    EXIT_LOG_CALLFLOW(%s, "loc client open");
    status = locClientQmiCtrlPointInit(pCallbackData);

    LOC_LOGV ("%s:%d] locClientQmiCtrlPointInit returned %d\n",
                    __func__, __LINE__, status);

    if(status != eLOC_CLIENT_SUCCESS)
    {
      free(pCallbackData);
      pCallbackData = NULL;
      break;
    }

     // set the handle to the callback data
    *pLocClientHandle = (locClientHandleType)pCallbackData;

    if(true != locClientRegisterEventMask(*pLocClientHandle,eventRegMask))
    {
      LOC_LOGE("%s:%d]: Error sending registration mask\n",
                  __func__, __LINE__);

      // release the client
      locClientClose(pLocClientHandle);

      status = eLOC_CLIENT_FAILURE_INTERNAL;
      break;
    }

    /* Initialize rest of the client structure now that the connection
     * to the service has been created successfully.
     */

    //fill in the event callback
     pCallbackData->eventCallback = pLocClientCallbacks->eventIndCb;

     //fill in the response callback
     pCallbackData->respCallback = pLocClientCallbacks->respIndCb;

     //fill in the error callback
     pCallbackData->errorCallback = pLocClientCallbacks->errorCb;

     //set the client event registration mask
     pCallbackData->eventRegMask = eventRegMask;

     // set the client cookie
     pCallbackData->pClientCookie = (void *)pClientCookie;

     LOC_LOGD("%s:%d]: returning handle = 0x%x, user_handle=0x%x, status = %d\n",
              __func__, __LINE__, *pLocClientHandle,
              pCallbackData->userHandle, status);

  }while(0);

  if(eLOC_CLIENT_SUCCESS != status)
  {
    *pLocClientHandle = LOC_CLIENT_INVALID_HANDLE_VALUE;
  }

  return(status);
}

/** locClientClose
  @brief Disconnects a client from the location engine.
  @param [in] pLocClientHandle  Pointer to the handle returned by the
                                locClientOpen() function.
  @return
  One of the following error codes:
  - 0 (eLOC_CLIENT_SUCCESS) - On success.
  - non-zero error code(see locClientStatusEnumType) - On failure.
*/

locClientStatusEnumType locClientClose(
  locClientHandleType* pLocClientHandle)
{
  qmi_client_error_type rc = QMI_NO_ERR; //No error

  if(NULL == pLocClientHandle)
  {
    // invalid handle
    LOC_LOGE("%s:%d]: invalid pointer to handle \n",
                  __func__, __LINE__);

    return(eLOC_CLIENT_FAILURE_INVALID_PARAMETER);
  }

  // convert handle to callback data
  locClientCallbackDataType *pCallbackData =
      (locClientCallbackDataType *)(*pLocClientHandle);

  LOC_LOGV("%s:%d]:\n", __func__, __LINE__ );

  // check the input handle for sanity
  if(NULL == pCallbackData ||
     NULL == pCallbackData->userHandle)
  {
    // invalid handle
    LOC_LOGE("%s:%d]: invalid handle \n",
                  __func__, __LINE__);

    return(eLOC_CLIENT_FAILURE_INVALID_HANDLE);
  }

  LOC_LOGV("locClientClose releasing handle 0x%x, user handle 0x%x\n",
      *pLocClientHandle, pCallbackData->userHandle );

  // NEXT call goes out to modem. We log the callflow before it
  // actually happens to ensure the this comes before resp callflow
  // back from the modem, to avoid confusing log order. We trust
  // that the QMI framework is robust.
  EXIT_LOG_CALLFLOW(%s, "loc client close");
  // release the handle
  rc = qmi_client_release(pCallbackData->userHandle);
  if(QMI_NO_ERR != rc )
  {
    LOC_LOGW("%s:%d]: qmi_client_release error %d for client %p\n",
                   __func__, __LINE__, rc, pCallbackData->userHandle);
    return(eLOC_CLIENT_FAILURE_INTERNAL);
  }

  /* clear the memory allocated to callback data to minimize the chances
   *  of a race condition occurring between close and the indication
   *  callback
   */
  memset(pCallbackData, 0, sizeof(*pCallbackData));

  // free the memory assigned in locClientOpen
  free(pCallbackData);
  pCallbackData= NULL;

  // set the handle to invalid value
  *pLocClientHandle = LOC_CLIENT_INVALID_HANDLE_VALUE;

  return eLOC_CLIENT_SUCCESS;
}

/** locClientSendReq
  @brief Sends a message to the location engine. If the locClientSendMsg()
         function is successful, the client should expect an indication
         (except start, stop, event reg and sensor injection messages),
         through the registered callback in the locOpen() function. The
         indication will contain the status of the request and if status is a
         success, indication also contains the payload
         associated with response.
  @param [in] handle Handle returned by the locClientOpen()
              function.
  @param [in] reqId         message ID of the request
  @param [in] reqPayload   Payload of the request, can be NULL
                            if request has no payload

  @return
  One of the following error codes:
  - 0 (eLOC_CLIENT_SUCCESS ) - On success.
  - non-zero error code (see locClientStatusEnumType) - On failure.
*/

locClientStatusEnumType locClientSendReq(
  locClientHandleType      handle,
  uint32_t                 reqId,
  locClientReqUnionType    reqPayload )
{
  locClientStatusEnumType status = eLOC_CLIENT_SUCCESS;
  qmi_client_error_type rc = QMI_NO_ERR; //No error
  qmiLocGenRespMsgT_v02 resp;
  uint32_t reqLen = 0;
  void *pReqData = NULL;
  locClientCallbackDataType *pCallbackData =
        (locClientCallbackDataType *)handle;

  // check the input handle for sanity
   if(NULL == pCallbackData ||
      NULL == pCallbackData->userHandle )
   {
     // did not find the handle in the client List
     LOC_LOGE("%s:%d]: invalid handle \n",
                   __func__, __LINE__);

     return(eLOC_CLIENT_FAILURE_INVALID_HANDLE);
   }

  // validate that the request is correct
  if (validateRequest(reqId, reqPayload, &pReqData, &reqLen) == false)
  {

    LOC_LOGE("%s:%d] error invalid request\n", __func__,
                __LINE__);

    return(eLOC_CLIENT_FAILURE_INVALID_PARAMETER);
  }

  LOC_LOGV("%s:%d] sending reqId= %d, len = %d\n", __func__,
                __LINE__, reqId, reqLen);

  // NEXT call goes out to modem. We log the callflow before it
  // actually happens to ensure the this comes before resp callflow
  // back from the modem, to avoid confusing log order. We trust
  // that the QMI framework is robust.
  EXIT_LOG_CALLFLOW(%s, loc_get_v02_event_name(reqId));

  rc = qmi_client_send_msg_sync(
      pCallbackData->userHandle,
      reqId,
      pReqData,
      reqLen,
      &resp,
      sizeof(resp),
      LOC_CLIENT_ACK_TIMEOUT);

  LOC_LOGV("%s:%d] qmi_client_send_msg_sync returned %d\n", __func__,
                __LINE__, rc);

  if (rc != QMI_NO_ERR)
  {
    LOC_LOGE("%s:%d]: send_msg_sync error: %d\n",__func__, __LINE__, rc);
    return(eLOC_CLIENT_FAILURE_INTERNAL);
  }

  // map the QCCI response to Loc API v02 status
  status = convertQmiResponseToLocStatus(&resp);

  return(status);
}

/** locClientGetSizeByRespIndId
 *  @brief Get the size of the response indication structure,
 *         from a specified id
 *  @param [in]  respIndId
 *  @param [out] pRespIndSize
 *  @return true if resp ID was found; else false
*/

bool locClientGetSizeByRespIndId(uint32_t respIndId, size_t *pRespIndSize)
{
  size_t idx = 0, respIndTableSize = 0;
  respIndTableSize = (sizeof(locClientRespIndTable)/sizeof(locClientRespIndTableStructT));
  for(idx=0; idx<respIndTableSize; idx++ )
  {
    if(respIndId == locClientRespIndTable[idx].respIndId)
    {
      // found
      *pRespIndSize = locClientRespIndTable[idx].respIndSize;

      LOC_LOGV("%s:%d]: resp ind Id %d size = %d\n", __func__, __LINE__,
                    respIndId, (uint32_t)*pRespIndSize);
      return true;
    }
  }

  //not found
  return false;
}


/** locClientGetSizeByEventIndId
 *  @brief Gets the size of the event indication structure, from
 *         a specified id
 *  @param [in]  eventIndId
 *  @param [out] pEventIndSize
 *  @return true if event ID was found; else false
*/
bool locClientGetSizeByEventIndId(uint32_t eventIndId, size_t *pEventIndSize)
{
  size_t idx = 0, eventIndTableSize = 0;

  // look in the event table
  eventIndTableSize =
    (sizeof(locClientEventIndTable)/sizeof(locClientEventIndTableStructT));

  for(idx=0; idx<eventIndTableSize; idx++ )
  {
    if(eventIndId == locClientEventIndTable[idx].eventId)
    {
      // found
      *pEventIndSize = locClientEventIndTable[idx].eventSize;

      LOC_LOGV("%s:%d]: event ind Id %d size = %d\n", __func__, __LINE__,
                    eventIndId, (uint32_t)*pEventIndSize);
      return true;
    }
  }
  // not found
  return false;
}

