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

/** @file loc_api_v02_client.h
*/

#ifndef LOC_API_V02_CLIENT_H
#define LOC_API_V02_CLIENT_H

#ifdef __cplusplus
extern "C" {
#endif


/*=============================================================================
 *
 *                             DATA DECLARATION
 *
 *============================================================================*/

#include <stdint.h>
#include "location_service_v02.h"  //QMI LOC Service data types definitions
#include <stdbool.h>
#include <stddef.h>

/******************************************************************************
 *  Constants and configuration
 *****************************************************************************/

/** @addtogroup constants_macros
@{ */

/** Specific value of #locClientHandleType, indicating an invalid handle. */
#define LOC_CLIENT_INVALID_HANDLE_VALUE (NULL)

/** @} */ /* end_addtogroup constants_macros */

/** @addtogroup data_types
@{ */

/** Location client handle used to represent a specific client. Negative values
    are invalid handles. */
typedef void* locClientHandleType;

/** Data type for events and event masks.
    @newpage */
typedef uint64_t locClientEventMaskType;

/** Location client status values.
    @vertspace
*/
typedef enum
{
  eLOC_CLIENT_SUCCESS                              = 0,
  /**< Request was successful. */

  eLOC_CLIENT_FAILURE_GENERAL                      = 1,
  /**< Failed because of a general failure. */

  eLOC_CLIENT_FAILURE_UNSUPPORTED                  = 2,
  /**< Failed because the service does not support the command. */

  eLOC_CLIENT_FAILURE_INVALID_PARAMETER            = 3,
  /**< Failed because the request contained invalid parameters. */

  eLOC_CLIENT_FAILURE_ENGINE_BUSY                  = 4,
  /**< Failed because the engine is busy. */

  eLOC_CLIENT_FAILURE_PHONE_OFFLINE                = 5,
  /**< Failed because the phone is offline. */

  eLOC_CLIENT_FAILURE_TIMEOUT                      = 6,
  /**< Failed because of a timeout. */

  eLOC_CLIENT_FAILURE_SERVICE_NOT_PRESENT          = 7,
  /**< Failed because the service is not present. */

  eLOC_CLIENT_FAILURE_SERVICE_VERSION_UNSUPPORTED  = 8,
  /**< Failed because the service version is unsupported. */

  eLOC_CLIENT_FAILURE_CLIENT_VERSION_UNSUPPORTED  =  9,
  /**< Failed because the service does not support client version. */

  eLOC_CLIENT_FAILURE_INVALID_HANDLE               = 10,
  /**< Failed because an invalid handle was specified. */

  eLOC_CLIENT_FAILURE_INTERNAL                     = 11,
  /**< Failed because of an internal error in the service. */

  eLOC_CLIENT_FAILURE_NOT_INITIALIZED              = 12
  /**< Failed because the service has not been initialized. */

}locClientStatusEnumType;

/** Loc Client error values
*/

typedef enum
{
  eLOC_CLIENT_ERROR_SERVICE_UNAVAILABLE            = 1
  /**< Service is no longer available, the client should
       close the existing connection and reopen the connection
       upon getting this error.
       */
}locClientErrorEnumType;


/** @} */ /* end_addtogroup data_types */

/** @addtogroup send_request
@{ */

/** @brief Request messages the client can send to the location engine.

  The following requests do not have any data associated, so they do not have a
  payload structure defined:

  - GetServiceRevision
  - GetFixCriteria
  - GetPredictedOrbitsDataSource
  - GetPredictedOrbitsDataValidity
  - GetEngineLock
  - GetSbasConfigReq
  - GetRegisteredEvents
  - GetNmeaTypes
  - GetLowPowerMode
  - GetXtraTSessionControl
  - GetRegisteredEvents
  - GetOperationMode
  - GetCradleMountConfig
  - GetExternalPowerConfig
*/
typedef union
{
   const qmiLocInformClientRevisionReqMsgT_v02* pInformClientRevisionReq;
   /**< Notifies the service about the revision the client is using.

        The client does not receive any indications corresponding to this
        request.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02. */

   const qmiLocRegEventsReqMsgT_v02* pRegEventsReq;
   /**< Changes the events the client is interested in receiving.

        The client does not receive any indications corresponding to this
        request.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_REG_EVENTS_REQ_V02. */

   const qmiLocStartReqMsgT_v02* pStartReq;
   /**< Starts a positioning session.

        The client receives the following indications: position report,
        satellite report, fix session report, and NMEA report (if applicable).

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_START_REQ_V02. */

   const qmiLocStopReqMsgT_v02* pStopReq;
   /**< Stops a positioning session. The client receives a fix session report
        denoting that the fix session ended after this message was sent.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_STOP_REQ_V02. */

   const qmiLocNiUserRespReqMsgT_v02* pNiUserRespReq;
   /**< Informs the service about the user response for a network-initiated call.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_NI_USER_RESPONSE_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_NI_USER_RESPONSE_REQ_V02. */

   const qmiLocInjectPredictedOrbitsDataReqMsgT_v02* pInjectPredictedOrbitsDataReq;
   /**< Injects the predicted orbits data into the service.

        When all predicted orbits data parts have been injected, the client
        receives the following indication containing a response:
        QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02.

        The client injects successive data parts without waiting for this
        indication as long as locClientSendReq() returns successfully.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02. */

   const qmiLocInjectUtcTimeReqMsgT_v02* pInjectUtcTimeReq;
   /**< Injects UTC time into the service.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_UTC_TIME_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_UTC_TIME_REQ_V02. */

   const qmiLocInjectPositionReqMsgT_v02* pInjectPositionReq;
   /**< Injects a position into the service.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_POSITION_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_POSITION_REQ_V02. */

   const qmiLocSetEngineLockReqMsgT_v02* pSetEngineLockReq;
   /**< Sets the location engine lock.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_ENGINE_LOCK_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_ENGINE_LOCK_REQ_V02. */

   const qmiLocSetSbasConfigReqMsgT_v02* pSetSbasConfigReq;
   /**< Sets the SBAS configuration.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SBAS_CONFIG_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SBAS_CONFIG_REQ_V02 . */

   const qmiLocSetNmeaTypesReqMsgT_v02* pSetNmeaTypesReq;
   /**< Sets the NMEA types configuration.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_NMEA_TYPES_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_NMEA_TYPES_REQ_V02. */

   const qmiLocSetLowPowerModeReqMsgT_v02* pSetLowPowerModeReq;
   /**< Sets the Low Power mode configuration.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_LOW_POWER_MODE_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_LOW_POWER_MODE_REQ_V02. */

   const qmiLocSetServerReqMsgT_v02* pSetServerReq;
   /**< Sets the A-GPS server type and address.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SERVER_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SERVER_REQ_V02. */

   const qmiLocGetServerReqMsgT_v02* pGetServerReq;
   /**< Gets the A-GPS server type and address.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_GET_SERVER_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_GET_SERVER_REQ_V02. */

   const qmiLocDeleteAssistDataReqMsgT_v02* pDeleteAssistDataReq;
   /**< Deletes the aiding data from the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_DELETE_ASSIST_DATA_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_DELETE_ASSIST_DATA_REQ_V02. */

   const qmiLocSetXtraTSessionControlReqMsgT_v02* pSetXtraTSessionControlReq;
   /**< Sets XTRA-T session control in the engine. If the request is accepted by
        the service, the client receives the following indication containing a
        response: QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02. */

   const qmiLocInjectWifiPositionReqMsgT_v02* pInjectWifiPositionReq;
   /**< Injects a WiFi position into the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_WIFI_POSITION_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_WIFI_POSITION_REQ_V02. */

   const qmiLocNotifyWifiStatusReqMsgT_v02* pNotifyWifiStatusReq;
   /**< Notifies the engine about the WiFi status.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02. */

   const qmiLocSetOperationModeReqMsgT_v02* pSetOperationModeReq;
   /**< Sets the engine Operation mode.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_OPERATION_MODE_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_OPERATION_MODE_REQ_V02. */

   const qmiLocSetSpiStatusReqMsgT_v02* pSetSpiStatusReq;
   /**< Sends the stationary position status to the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SPI_STATUS_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SPI_STATUS_REQ_V02. */

   const qmiLocInjectSensorDataReqMsgT_v02* pInjectSensorDataReq;
   /**< Injects sensor data into the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_SENSOR_DATA_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_SENSOR_DATA_REQ_V02. */

   const qmiLocInjectTimeSyncDataReqMsgT_v02* pInjectTimeSyncReq;
   /**< Injects time synchronization information into the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02. */

   const qmiLocSetCradleMountConfigReqMsgT_v02* pSetCradleMountConfigReq;
   /**< Sets the cradle mount state information in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        SET_CRADLE_MOUNT_CONFIG_REQ_V02.

        To send this request, set the reqId field in locClientSendReq() to
        SET_CRADLE_MOUNT_CONFIG_IND_V02. */

   const qmiLocSetExternalPowerConfigReqMsgT_v02* pSetExternalPowerConfigReq;
   /**< Sets external power configuration state in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02. */

   const qmiLocInformLocationServerConnStatusReqMsgT_v02*
     pInformLocationServerConnStatusReq;
   /**< Informs the engine about the connection status to the location server.

        This can be sent in response to a
        QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02 request. The
        service sends back a QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02
        response indication for this request.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02.*/

   const qmiLocSetProtocolConfigParametersReqMsgT_v02*
     pSetProtocolConfigParametersReq;
   /**< Sets the protocol configuration parameters in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02. */

   const qmiLocGetProtocolConfigParametersReqMsgT_v02*
     pGetProtocolConfigParametersReq;
   /**< Retrieves protocol configuration parameters from the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02.
        @newpage */

      const qmiLocSetSensorControlConfigReqMsgT_v02*
     pSetSensorControlConfigReq;
   /**< Sets the sensor control configuration parameters in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02.
        @newpage */

   const qmiLocSetSensorPerformanceControlConfigReqMsgT_v02*
      pSetSensorPerformanceControlConfigReq;
   /**< Sets the sensor performance configuration parameters in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02.
        @newpage */

   const qmiLocSetSensorPropertiesReqMsgT_v02* pSetSensorPropertiesReq;
   /**< Sets the sensor properties in the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02.
        @newpage */

   const qmiLocInjectSuplCertificateReqMsgT_v02* pInjectSuplCertificateReq;
   /**< Injects a SUPL certificate into the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02.
        @newpage */

   const qmiLocDeleteSuplCertificateReqMsgT_v02* pDeleteSuplCertificateReq;
   /**< Deletes a SUPL certificate from the engine.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02.
        @newpage */

   const qmiLocSetPositionEngineConfigParametersReqMsgT_v02*
     pSetPositionEngineConfigParametersReq;
   /**< Sets Position Engine configuration.

        If the request is accepted by the service, the client receives the
        following indication containing a response:
        QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND _V02.

        To send this request, set the reqId field in locClientSendReq() to
        QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02.
        @newpage */

   const qmiLocGetPositionEngineConfigParametersReqMsgT_v02*
     pGetPositionEngineConfigParametersReq;
   /**< Gets Position Engine configuration.

         If the request is accepted by the service, the client receives the
         following indication containing a response:
         QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02.

         To send this request, set the reqId field in locClientSendReq() to
         QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02.
         @newpage */

}locClientReqUnionType;


/** @brief Event indications that are sent by the service.
*/
typedef union
{
   const qmiLocEventPositionReportIndMsgT_v02* pPositionReportEvent;
   /**< Contains the position information.

        This event is generated after QMI_LOC_START_REQ_V02 is sent. If
        periodic fix criteria is specified, this event is generated multiple
        times periodically at the specified rate until QMI_LOC_STOP_REQ_V02 is
        sent.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_POSITION_REPORT_IND_V02. */

   const qmiLocEventGnssSvInfoIndMsgT_v02* pGnssSvInfoReportEvent;
   /**< Contains the GNSS satellite information.

        This event is generated after QMI_LOC_START_REQ_V02 is sent. This event
        is generated at 1 Hz if the location engine is tracking satellites to
        make a location fix.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_GNSS_INFO_IND_V02. */

   const qmiLocEventNmeaIndMsgT_v02* pNmeaReportEvent;
   /**< Contains an NMEA report sentence. The entire NMEA report consisting
        of multiple sentences is sent at a 1Hz rate.
        This event is generated after QMI_LOC_START_REQ_V02 is sent.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_NMEA_IND_V02. */

   const qmiLocEventNiNotifyVerifyReqIndMsgT_v02* pNiNotifyVerifyReqEvent;
   /**< Notifies a location client when the network triggers a positioning request
        to the mobile.

        Upon getting this event, the location client displays the
        network-initiated fix request in a dialog and prompts the user to accept
        or deny the request. The client responds to this request with the message
        QMI_LOC_NI_USER_RESPONSE_REQ_V02.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02. */

   const qmiLocEventInjectTimeReqIndMsgT_v02* pInjectTimeReqEvent;
   /**< Asks the client for time assistance.

        The client responds to this request with the message
        QMI_LOC_INJECT_UTC_TIME_REQ_V02.

        The eventIndId field in the event indication callback is
        set to QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02. */

   const qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02*
         pInjectPredictedOrbitsReqEvent;
   /**< Asks the client for predicted orbits data assistance.

        The client responds to this request with the message
        QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02. */

   const qmiLocEventInjectPositionReqIndMsgT_v02* pInjectPositionReqEvent;
   /**< Asks the client for position assistance.

        The client responds to this request with the message
        QMI_LOC_INJECT_POSITION_REQ_V02.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02. */

   const qmiLocEventEngineStateIndMsgT_v02* pEngineState;
   /**< Sent by the engine whenever it turns on or off.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_ENGINE_STATE_IND_V02. */

   const qmiLocEventFixSessionStateIndMsgT_v02* pFixSessionState;
   /**< Sent by the engine when a location session begins or ends.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02. */

   const qmiLocEventWifiReqIndMsgT_v02* pWifiReqEvent;
   /**< Sent by the engine when it needs WiFi support.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_WIFI_REQ_IND_V02. */

   const qmiLocEventSensorStreamingReadyStatusIndMsgT_v02*
          pSensorStreamingReadyStatusEvent;
   /**< Notifies the client that the engine is ready to accept sensor data.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02. */

   const qmiLocEventTimeSyncReqIndMsgT_v02* pTimeSyncReqEvent;
   /**< Sent by the engine when it needs to synchronize its time with the sensor
        processor time.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02. */

   const qmiLocEventSetSpiStreamingReportIndMsgT_v02*
     pSetSpiStreamingReportEvent;
   /**< Asks the client to start/stop sending a Stationary Position Indicator
        (SPI) stream.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_SET_SPI_STREAMING_REPORT_IND_V02. */

   const qmiLocEventLocationServerConnectionReqIndMsgT_v02*
      pLocationServerConnReqEvent;
   /**< Sent by the engine to ask the client to open or close a connection to
        a location server.

        The client responds to this request by sending the
        QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02 message.

        The eventIndId field in the event indication callback is set to
        QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02.
        @newpage */

}locClientEventIndUnionType;


/** @brief Response indications that are sent by the service. */
typedef union
{
   const qmiLocGetServiceRevisionIndMsgT_v02* pGetServiceRevisionInd;
   /**< Response to the request, QMI_LOC_GET_SERVICE_REVISION_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SERVICE_REVISION_IND_V02. */

   const qmiLocGetFixCriteriaIndMsgT_v02* pGetFixCriteriaInd;
   /**< Response to the request, QMI_LOC_GET_FIX_CRITERIA_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_FIX_CRITERIA_IND_V02. */

   const qmiLocNiUserRespIndMsgT_v02* pNiUserRespInd;
   /**< Response to the request, QMI_LOC_NI_USER_RESPONSE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_NI_USER_RESPONSE_IND_V02. */

   const qmiLocInjectPredictedOrbitsDataIndMsgT_v02*
     pInjectPredictedOrbitsDataInd;
   /**< Sent after a predicted orbits data part has been successfully injected.

        The client waits for this indication before injecting the next part.
        This indication is sent in response to
        QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02. */

   const qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02*
      pGetPredictedOrbitsDataSourceInd;
   /**< Response to the request,
        QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02. */

   const qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02*
     pGetPredictedOrbitsDataValidityInd;
   /**< Response to the request,
        QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02. */

   const qmiLocInjectUtcTimeIndMsgT_v02* pInjectUtcTimeInd;
   /**< Response to the request, QMI_LOC_INJECT_UTC_TIME_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_UTC_TIME_IND_V02. */

   const qmiLocInjectPositionIndMsgT_v02* pInjectPositionInd;
   /**< Response to the request, QMI_LOC_INJECT_POSITION_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_POSITION_IND_V02. */

   const qmiLocSetEngineLockIndMsgT_v02* pSetEngineLockInd;
   /**< Response to the request, QMI_LOC_SET_ENGINE_LOCK_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_ENGINE_LOCK_IND_V02. */

   const qmiLocGetEngineLockIndMsgT_v02* pGetEngineLockInd;
   /**< Response to the request, QMI_LOC_GET_ENGINE_LOCK_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_ENGINE_LOCK_IND_V02. */

   const qmiLocSetSbasConfigIndMsgT_v02* pSetSbasConfigInd;
   /**< Response to the request, QMI_LOC_SET_SBAS_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SBAS_CONFIG_IND_V02. */

   const qmiLocGetSbasConfigIndMsgT_v02* pGetSbasConfigInd;
   /**< Response to the request, QMI_LOC_GET_SBAS_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SBAS_CONFIG_IND_V02. */

   const qmiLocSetNmeaTypesIndMsgT_v02* pSetNmeaTypesInd;
   /**< Response to the request, QMI_LOC_SET_NMEA_TYPES_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_NMEA_TYPES_IND_V02. */

   const qmiLocGetNmeaTypesIndMsgT_v02* pGetNmeaTypesInd;
   /**< Response to the request, QMI_LOC_GET_NMEA_TYPES_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_NMEA_TYPES_IND_V02. */

   const qmiLocSetLowPowerModeIndMsgT_v02* pSetLowPowerModeInd;
   /**< Response to the request, QMI_LOC_SET_LOW_POWER_MODE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_LOW_POWER_MODE_IND_V02. */

   const qmiLocGetLowPowerModeIndMsgT_v02* pGetLowPowerModeInd;
   /**< Response to the request, QMI_LOC_GET_LOW_POWER_MODE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_LOW_POWER_MODE_IND_V02. */

   const qmiLocSetServerIndMsgT_v02* pSetServerInd;
   /**< Response to the request, QMI_LOC_SET_SERVER_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SERVER_IND_V02. */

   const qmiLocGetServerIndMsgT_v02* pGetServerInd;
   /**< Response to the request, QMI_LOC_GET_SERVER_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SERVER_IND_V02. */

   const qmiLocDeleteAssistDataIndMsgT_v02* pDeleteAssistDataInd;
   /**< Response to the request, QMI_LOC_DELETE_ASSIST_DATA_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_DELETE_ASSIST_DATA_IND_V02. */

   const qmiLocSetXtraTSessionControlIndMsgT_v02* pSetXtraTSessionControlInd;
   /**< Response to the request, QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02. */

   const qmiLocGetXtraTSessionControlIndMsgT_v02* pGetXtraTSessionControlInd;
   /**< Response to the request, QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02. */

   const qmiLocInjectWifiPositionIndMsgT_v02* pInjectWifiPositionInd;
   /**< Response to the request, QMI_LOC_INJECT_WIFI_POSITION_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_WIFI_POSITION_IND_V02. */

   const qmiLocNotifyWifiStatusIndMsgT_v02* pNotifyWifiStatusInd;
   /**< Response to the request, QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02. */

   const qmiLocGetRegisteredEventsIndMsgT_v02* pGetRegisteredEventsInd;
   /**< Response to the request, QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_REGISTERED_EVENTS_IND_V02. */

   const qmiLocSetOperationModeIndMsgT_v02* pSetOperationModeInd;
   /**< Response to the request, QMI_LOC_SET_OPERATION_MODE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_OPERATION_MODE_IND_V02. */

   const qmiLocGetOperationModeIndMsgT_v02* pGetOperationModeInd;
   /**< Response to the request, QMI_LOC_GET_OPERATION_MODE_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_OPERATION_MODE_IND_V02. */

   const qmiLocSetSpiStatusIndMsgT_v02* pSetSpiStatusInd;
   /**< Response to the request, QMI_LOC_SET_SPI_STATUS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SPI_STATUS_IND_V02. */

   const qmiLocInjectSensorDataIndMsgT_v02* pInjectSensorDataInd;
   /**< Response to the request, QMI_LOC_INJECT_SENSOR_DATA_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_SENSOR_DATA_IND_V02. */

   const qmiLocInjectTimeSyncDataIndMsgT_v02* pInjectTimeSyncDataInd;
   /**< Response to the request,  QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02. */

   const qmiLocSetCradleMountConfigIndMsgT_v02* pSetCradleMountConfigInd;
   /**< Response to the request, QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02. */

   const qmiLocGetCradleMountConfigIndMsgT_v02* pGetCradleMountConfigInd;
   /**< Response to the request, QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02. */

   const qmiLocSetExternalPowerConfigIndMsgT_v02* pSetExternalPowerConfigInd;
   /**< Response to the request, QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02. */

   const qmiLocGetExternalPowerConfigIndMsgT_v02* pGetExternalPowerConfigInd;
   /**< Response to the request, QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02. */

   const qmiLocInformLocationServerConnStatusIndMsgT_v02*
     pInformLocationServerConnStatusInd;
   /**< Response to the request,
        QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02.*/

   const qmiLocSetProtocolConfigParametersIndMsgT_v02*
     pSetProtocolConfigParametersInd;
   /**< Response to the request,
        QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02. */

   const qmiLocGetProtocolConfigParametersIndMsgT_v02*
     pGetProtocolConfigParametersInd;
   /**< Response to the request,
        QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02.
        @newpage */

   const qmiLocSetSensorControlConfigIndMsgT_v02* pSetSensorControlConfigInd;
   /**< Response to the request,
        QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02.
        @newpage */

   const qmiLocGetSensorControlConfigIndMsgT_v02* pGetSensorControlConfigInd;
   /**< Response to the request,
        QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02.
        @newpage */

   const qmiLocSetSensorPropertiesIndMsgT_v02* pSetSensorPropertiesInd;
   /**< Response to the request,
        QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02.
        @newpage */

   const qmiLocGetSensorPropertiesIndMsgT_v02* pGetSensorPropertiesInd;
   /**< Response to the request,
        QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02.
        @newpage */

   const qmiLocSetSensorPerformanceControlConfigIndMsgT_v02*
     pSetSensorPerformanceControlConfigInd;
   /**< Response to the request,
        QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02.
        @newpage */

   const qmiLocGetSensorPerformanceControlConfigIndMsgT_v02*
     pGetSensorPerformanceControlConfigInd;
   /**< Response to the request,
        QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02.

        The respIndId field in the response indication callback is set to
        QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02.
        @newpage */

   const qmiLocInjectSuplCertificateIndMsgT_v02* pInjectSuplCertificateInd;
   /**< Response to the request,
        QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02
        The respIndId field in the response indication callback is set to
        QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02.
        @newpage */

   const qmiLocDeleteSuplCertificateIndMsgT_v02* pDeleteSuplCertificateInd;
   /**< Response to the request,
        QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02.
        The respIndId field in the response indication callback is set to
        QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02.
        @newpage */

   const qmiLocSetPositionEngineConfigParametersIndMsgT_v02*
     pSetPositionEngineConfigParametersInd;
   /**< Response to the request,
        QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02
        The respIndId field in the response indication callback is set to
        QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02.
        @newpage */

   const qmiLocGetPositionEngineConfigParametersIndMsgT_v02*
     pGetPositionEngineConfigParametersInd;
    /**< Response to the request,
        QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02
        The respIndId field in the response indication callback is set to
        QMI_LOC_ET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02.
        @newpage */

}locClientRespIndUnionType;

/** @} */ /* end_addtogroup send_request */

/** @addtogroup data_types
@{ */

/** Location event indication callback function type. The Location service can
    generate two types of indications:

  - Asynchronous events indications, such as time injection request and satellite
    reports. The client specifies the asynchronous events it is interested in
    receiving through the event mask (see locClientOpen()).
  - Response indications that are generated as a response to a request. For
    example, the QMI_LOC_GET_FIX_CRITERIA_REQ_V02 request generates the
    indication, QMI_LOC_GET_FIX_CRITERIA_IND_V02.

  This callback handles the asynchronous event indications.

  @vertspace
  @param handle            Location client for this event. Only the client who
                           registered for the corresponding event receives
                           this callback.
  @param eventIndId        ID of the event indication.
  @param eventIndPayload   Event indication payload.
  @param pClientCookie     Pointer to the cookie the client specified during
                           registration.
*/
typedef void (*locClientEventIndCbType)(
      locClientHandleType handle,
      uint32_t eventIndId,
      const locClientEventIndUnionType eventIndPayload,
      void *pClientCookie
);

/** Location response indication callback function type. The Location service can
    generate two types of indications:

  - Asynchronous events indications, such as time injection request and satellite
    reports. The client specifies the asynchronous events it is interested in
    receiving through the event mask (see locClientOpen()).
  - Response indications that are generated as a response to a request. For
    example, the QMI_LOC_GET_FIX_CRITERIA_REQ_V02 request generates the
    indication, QMI_LOC_GET_FIX_CRITERIA_IND_V02.

  This callback handles the response indications.

  @vertspace
  @param handle           Location client who sent the request for which this
                          response indication is generated.
  @param respIndId        ID of the response. It is the same value as the ID
                          of request sent to the engine.
  @param respIndPayload   Payload of the response indication.
  @param pClientCookie    Pointer to the cookie the client specified during
                          registration.

  @newpage
*/
typedef void  (*locClientRespIndCbType)(
      locClientHandleType handle,
      uint32_t respIndId,
      const locClientRespIndUnionType respIndPayload,
      void *pClientCookie
);

/** Location error callback function type. This function will be
    called to inform the client that the service is no longer
    available. When the client receives this callback it must
    close the existing connection  and reopen the client connection.


  @vertspace
  @param handle           Location client who sent the request for which this
                          error indication is generated.
  @param errorId          Error ID.
  @param errorCbdata      Payload associated with the error indication.

  @newpage
*/
typedef void  (*locClientErrorCbType)(
      locClientHandleType handle,
      locClientErrorEnumType errorId,
      void* pClientCookie
 );


/** Callback functions to be registered during locClientOpen
*/

typedef struct
{
    uint32_t size;
    /**< Size of the structure */
    locClientEventIndCbType eventIndCb;
    /**< Event Indication callback */
    locClientRespIndCbType respIndCb;
    /**< Response Indication callback */
    locClientErrorCbType errorCb;
    /**< Error Indication callback */
}locClientCallbacksType;

/** @} */ /* end_addtogroup data_types */

/*===========================================================================
 *
 *                          FUNCTION DECLARATION
 *
 *==========================================================================*/

/** @addtogroup open_client
@{ */
/*==========================================================================
    locClientOpen */
/**
  @latexonly\label{hdr:locClientOpenFunction}@endlatexonly Connects a location
  client to the location engine. If the connection is successful, this function
  returns a handle that the location client uses for future location operations.

  @datatype
  #locClientStatusEnumType\n
  #locClientEventMaskType\n
  #locClientCallbacksType *\n
  #locClientHandleType *\n
  #void *

  @vertspace
  @param[in]  eventRegMask          Mask of asynchronous events the client is
                                    interested in receiving.
  @param[in]  pLocClientCallbacks   Pointer to structure containing the
                                    callbacks.
  @param[out] pLocClientHandle      Pointer to the handle to be used by the
                                    client for any subsequent requests.
  @param[in]  pLocClientCookie      Pointer to a "cookie" to be returned to the
                                    client along with the callbacks.

  @return
  One of the following error codes:
  - eLOC_CLIENT_SUCCESS -- If the connection is opened.
  - Non-zero error code (see \ref locClientStatusEnumType) -- On failure.

  @dependencies
  None.
  @newpage
*/
extern locClientStatusEnumType locClientOpen (
      locClientEventMaskType            eventRegMask,
      const locClientCallbacksType*     pLocClientCallbacks,
      locClientHandleType*              pLocClientHandle,
      const void*                       pLocClientCookie
);

/** @} */ /* end_addtogroup open_client */

/** @addtogroup close_client
@{ */
/*==========================================================================
    locClientClose */
/**
  @latexonly\label{hdr:locClientCloseFunction}@endlatexonly Disconnects a client
  from the location engine and sets the handle to
  LOC_CLIENT_INVALID_HANDLE_VALUE.

  @datatype
  #locClientStatusEnumType\n
  #locClientHandleType

  @vertspace
  @param[in] pLocClientHandle  Pointer to the handle returned by the
                               locClientOpen() function.

  @return
  One of the following error codes:
  - 0 (eLOC_CLIENT_SUCCESS) -- On success.
  - Non-zero error code (see \ref locClientStatusEnumType) -- On failure.

  @dependencies
  None.
  @newpage
*/
extern locClientStatusEnumType locClientClose (
      locClientHandleType* pLocClientHandle
);

/** @} */ /* end_addtogroup close_client */

/** @addtogroup send_request
@{ */
/*=============================================================================
    locClientSendReq */
/**
  @latexonly\label{hdr:locClientSendReqFunction}@endlatexonly Sends a message to
  the location engine. If this function is successful, the client expects an
  indication (except start, stop, event registration, and sensor injection
  messages) through the registered callback in the locClientOpen() function.

  The indication contains the status of the request. If the status is a success,
  the indication also contains the payload associated with response.

  @datatype
  #locClientStatusEnumType\n
  #locClientHandleType\n
  #locClientReqUnionType

  @vertspace
  @param[in] handle        Handle returned by the locClientOpen() function.
  @param[in] reqId         QMI_LOC service message ID of the request.
  @param[in] pReqPayload   Payload of the request. This can be NULL if the request
                           has no payload.

  @return
  One of the following error codes:
  - 0 (eLOC_CLIENT_SUCCESS) -- On success.
  - Non-zero error code (see \ref locClientStatusEnumType) -- On failure.

  @dependencies
  None.

  @structure
  locClientReqUnionType
  @newpage
*/
extern locClientStatusEnumType locClientSendReq(
     locClientHandleType       handle,
     uint32_t                  reqId,
     locClientReqUnionType     reqPayload
);

/** @} */ /* end_addtogroup send_request */

/** @addtogroup get_size_event
@{ */
/*=============================================================================
    locClientGetSizeByEventIndId */
/**
  Gets the size of the event indication structure from a specified ID.

  @datatype
  #locClientGetSizeByEventIndId

  @vertspace
  @param[in]  eventIndId      Event indicator ID.
  @param[out] pEventIndSize   Pointer to the size of the structure.

  @return
  true -- The event ID was found.\n
  false -- Otherwise.

  @dependencies
  None.

  @structure
  locClientEventIndUnionType
  @newpage
*/
extern bool locClientGetSizeByEventIndId(
  uint32_t eventIndId,
  size_t *pEventIndSize);

/** @} */ /* end_addtogroup get_size_event */

/** @addtogroup get_size_response
@{ */
/*=============================================================================
    locClientGetSizeByRespIndId */

/**
  Gets the size of the response indication structure from a specified ID.

  @datatype
  #locClientGetSizeByRespIndId

  @vertspace
  @param[in]  respIndId      Response indicator ID.
  @param[out] pRespIndSize   Pointer to the size of the structure.

  @return
  true -- The response ID was found.\n
  false -- Otherwise.

  @dependencies
  None.

  @structure
  locClientRespIndUnionType
  @newpage
*/
extern bool locClientGetSizeByRespIndId(
  uint32_t respIndId,
  size_t *pRespIndSize);

/*=============================================================================*/

/** @} */ /* end_addtogroup get_size_response */

#ifdef __cplusplus
}
#endif

#endif /* LOC_API_V02_CLIENT_H*/
