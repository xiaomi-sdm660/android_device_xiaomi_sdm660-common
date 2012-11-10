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

#ifndef LOC_API_V_0_2_ADAPTER_H
#define LOC_API_V_0_2_ADAPTER_H

#include <LocApiAdapter.h>
#include "loc_api_v02_client.h"
#include <stdint.h>
#include <stdbool.h>

/* Fix criteria structure*/
struct LocApiV02FixCriteria
{
  LocPositionMode mode;
  GpsPositionRecurrence recurrence;
  uint32_t min_interval ;
  uint32_t preferred_accuracy ;
  uint32_t preferred_time;

  /* Constructor for the structure  */
  inline LocApiV02FixCriteria (LocPositionMode mode,
                               GpsPositionRecurrence recur,
                               uint32_t min_int,
                               uint32_t pref_acc,
                               uint32_t pref_time) :
    mode(mode), recurrence(recur), min_interval(min_int),
    preferred_accuracy(pref_acc), preferred_time(pref_time) {}
};

/* This class derives from the LocApiAdapter class.
   The members of this class are responsible for converting
   the Loc API V02 data structures into Loc Engine data structures.
   This class also implements some of the virtual functions that
   handle the requests from loc engine. */
class LocApiV02Adapter : public LocApiAdapter {

  /* loc api v02 handle*/
  locClientHandleType clientHandle;

  /* events the client is registered for */
  locClientEventMaskType eventMask;

  /* current session state */
  bool navigating;

  /* current fix criteria, for Loc aPI V02 this is
     stored when set_position_mode is called and
     sent out with Loc aPI V02 start message when the
     start_fix() is called by loc engine. */
  LocApiV02FixCriteria fixCriteria;

  /* Convert event mask from loc eng to loc_api_v02 format */
  locClientEventMaskType convertMask(LOC_API_ADAPTER_EVENT_MASK_T mask);

  /* Convert error from loc_api_v02 to loc eng format*/
  enum loc_api_adapter_err convertErr(locClientStatusEnumType status);

  /* convert Ni Encoding type from QMI_LOC to loc eng format */
  GpsNiEncodingType convertNiEncoding(
    qmiLocNiDataCodingSchemeEnumT_v02 loc_encoding);

  /*convert NI notify verify type from QMI LOC to loc eng format*/
  bool convertNiNotifyVerifyType (GpsNiNotification *notif,
      qmiLocNiNotifyVerifyEnumT_v02 notif_priv);

  /* close Loc API V02 client */
  int deInitLocClient();

  /* convert position report to loc eng format and send the converted
     position to loc eng */
  void reportPosition
    (const qmiLocEventPositionReportIndMsgT_v02 *location_report_ptr);

  /* convert satellite report to loc eng format and  send the converted
     report to loc eng */
  void reportSv (const qmiLocEventGnssSvInfoIndMsgT_v02 *gnss_report_ptr);

  /* convert engine state report to loc eng format and send the converted
     report to loc eng */
  void reportEngineState (
    const qmiLocEventEngineStateIndMsgT_v02 *engine_state_ptr);

  /* convert fix session report to loc eng format and send the converted
     report to loc eng */
  void reportFixSessionState (
    const qmiLocEventFixSessionStateIndMsgT_v02 *fix_session_state_ptr);

  /* convert NMEA report to loc eng format and send the converted
     report to loc eng */
  void reportNmea (const qmiLocEventNmeaIndMsgT_v02 *nmea_report_ptr);

  /* convert and report an ATL request to loc engine */
  void reportAtlRequest(
    const qmiLocEventLocationServerConnectionReqIndMsgT_v02
    *server_request_ptr);

  /* convert and report NI request to loc eng */
  void reportNiRequest(
    const qmiLocEventNiNotifyVerifyReqIndMsgT_v02 *ni_req_ptr);

public:
  LocApiV02Adapter(LocEng &locEng);
  ~LocApiV02Adapter();

  /* event callback registered with the loc_api v02 interface */
  void eventCb(locClientHandleType client_handle,
               uint32_t loc_event_id,
               locClientEventIndUnionType loc_event_payload);

  /* error callback, this function handles the  service unavailable
     error */
  void errorCb(locClientHandleType handle,
               locClientErrorEnumType errorId);

  virtual enum loc_api_adapter_err reinit();

  virtual enum loc_api_adapter_err startFix();

  virtual enum loc_api_adapter_err stopFix();

  virtual enum loc_api_adapter_err
    setPositionMode(LocPositionMode mode, GpsPositionRecurrence recurrence,
                    uint32_t min_interval, uint32_t preferred_accuracy,
                    uint32_t preferred_time);

  virtual enum loc_api_adapter_err
    setTime(GpsUtcTime time, int64_t timeReference, int uncertainty);

  virtual enum loc_api_adapter_err
    injectPosition(double latitude, double longitude, float accuracy);

  virtual enum loc_api_adapter_err
    deleteAidingData(GpsAidingData f);

  virtual enum loc_api_adapter_err
    informNiResponse(GpsUserResponseType userResponse,
                     const void* passThroughData);

  virtual enum loc_api_adapter_err
    setServer(const char* url, int len);
  virtual enum loc_api_adapter_err
    setServer(unsigned int ip, int port, LocServerType type);
  virtual enum loc_api_adapter_err
    setXtraData(char* data, int length);
#ifdef QCOM_FEATURE_IPV6
  virtual enum loc_api_adapter_err
    atlOpenStatus(int handle, int is_succ, char* apn, AGpsBearerType bear,
                   AGpsType agpsType);
#else
  virtual enum loc_api_adapter_err
    atlOpenStatus(int handle, int is_succ, char* apn,
                   AGpsType agpsType);
#endif
  virtual enum loc_api_adapter_err atlCloseStatus(int handle, int is_succ);
  virtual enum loc_api_adapter_err setSUPLVersion(uint32_t version);

  virtual enum loc_api_adapter_err
    setSensorControlConfig(int sensorUsage);

  virtual enum loc_api_adapter_err
    setSensorProperties(float gyroBiasVarianceRandomWalk);

  virtual enum loc_api_adapter_err
    setSensorPerfControlConfig(int controlMode,
                               int accelSamplesPerBatch,
                               int accelBatchesPerSec,
                               int gyroSamplesPerBatch,
                               int gyroBatchesPerSec);
};

#endif //LOC_API_V_0_2_ADAPTER_H
