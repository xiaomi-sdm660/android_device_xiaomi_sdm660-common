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
#ifndef LOC_SERVICE_H
#define LOC_SERVICE_H
/**
  @file location_service_v02.h

  @brief This is the public header file which defines the loc service Data structures.

  This header file defines the types and structures that were defined in
  loc. It contains the constant values defined, enums, structures,
  messages, and service message IDs (in that order) Structures that were
  defined in the IDL as messages contain mandatory elements, optional
  elements, a combination of mandatory and optional elements (mandatory
  always come before optionals in the structure), or nothing (null message)

  An optional element in a message is preceded by a uint8_t value that must be
  set to true if the element is going to be included. When decoding a received
  message, the uint8_t values will be set to true or false by the decode
  routine, and should be checked before accessing the values that they
  correspond to.

  Variable sized arrays are defined as static sized arrays with an unsigned
  integer (32 bit) preceding it that must be set to the number of elements
  in the array that are valid. For Example:

  uint32_t test_opaque_len;
  uint8_t test_opaque[16];

  If only 4 elements are added to test_opaque[] then test_opaque_len must be
  set to 4 before sending the message.  When decoding, the _len value is set
  by the decode routine and should be checked so that the correct number of
  elements in the array will be accessed.

*/

/** @defgroup loc_qmi_consts Constant values defined in the IDL */
/** @defgroup loc_qmi_msg_ids Constant values for QMI message IDs */
/** @defgroup loc_qmi_enums Enumerated types used in QMI messages */
/** @defgroup loc_qmi_messages Structures sent as QMI messages */
/** @defgroup loc_qmi_aggregates Aggregate types used in QMI messages */
/** @defgroup loc_qmi_accessor Accessor for QMI service object */
/** @defgroup loc_qmi_version Constant values for versioning information */

#include <stdint.h>
#include "qmi_idl_lib.h"
#include "common_v01.h"


#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup loc_qmi_version
    @{
  */
/** Major Version Number of the IDL used to generate this file */
#define LOC_V02_IDL_MAJOR_VERS 0x02
/** Revision Number of the IDL used to generate this file */
#define LOC_V02_IDL_MINOR_VERS 0x04
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOC_V02_IDL_TOOL_VERS 0x02
/** Maximum Defined Message ID */
#define LOC_V02_MAX_MESSAGE_ID 0x005F;
/**
    @}
  */


/** @addtogroup loc_qmi_consts
    @{
  */

/**  Maximum number of satellites in the satellite report.  */
#define QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02 80

/**  Maximum NMEA string length.  */
#define QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 200

/**  Maximum length of the requestor ID string.  */
#define QMI_LOC_NI_MAX_REQUESTOR_ID_LENGTH_V02 200

/**  Session ID byte length.  */
#define QMI_LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH_V02 4

/**  Maximum client name length allowed.  */
#define QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02 64

/**  Maximum URL length accepted by the location engine.  */
#define QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 255

/**  IPV6 address length in bytes.  */
#define QMI_LOC_IPV6_ADDR_LENGTH_V02 8

/**  SUPL hash length.  */
#define QMI_LOC_NI_SUPL_HASH_LENGTH_V02 8

/**  Maximum client address length allowed.  */
#define QMI_LOC_NI_MAX_EXT_CLIENT_ADDRESS_V02 20

/**  Maximum codeword length allowed.  */
#define QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02 20

/**  Maximum number of NTP Servers sent out with this event. */
#define QMI_LOC_MAX_NTP_SERVERS_V02 3

/**  Maximum number of predicted orbits servers supported in the location
     engine.  */
#define QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02 3

/**  Maximum part length that can be injected. The client should
     also look at the maxPartSize field in the predicted orbits injection
     request indication and pick the minimum of the two.   */
#define QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02 1024

/**  Maximum length of the delete SV information list  */
#define QMI_LOC_DELETE_MAX_SV_INFO_LENGTH_V02 128

/**  MAC address length in bytes.  */
#define QMI_LOC_WIFI_MAC_ADDR_LENGTH_V02 6

/**  Maximum number of APs that the sender can report.  */
#define QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02 50

/**  Maximum number of samples that can be injected in a TLV.  */
#define QMI_LOC_SENSOR_DATA_MAX_SAMPLES_V02 50

/**  Maximum APN string length allowed.  */
#define QMI_LOC_MAX_APN_NAME_LENGTH_V02 100

/**  Maximum APN profiles supported. */
#define QMI_LOC_MAX_APN_PROFILES_V02 6

/**  Maximum length of SUPL CERT. */
#define QMI_LOC_MAX_SUPL_CERT_LENGTH_V02 2000
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Response Message; Generic response definition. This message is used to tell
                    clients whether their message was accepted for further
                    processing or rejected. */
typedef struct {

  /* Mandatory */
  /*  Result Code */
  qmi_response_type_v01 resp;
}qmiLocGenRespMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Informs the service of the minor revision of the interface
                    definition that the control point implements. */
typedef struct {

  /* Mandatory */
  /*  Revision */
  uint32_t revision;
  /**<   Revision that the control point is using. \n
       - Type: Unsigned integer   */
}qmiLocInformClientRevisionReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocEventRegMaskT_v02;
#define QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00000001) /**<  The control point must enable this mask to receive position report
       event indications.  */
#define QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 ((qmiLocEventRegMaskT_v02)0x00000002) /**<  The control point must enable this mask to receive satellite report
       event indications. These reports are sent at a 1 Hz rate.  */
#define QMI_LOC_EVENT_MASK_NMEA_V02 ((qmiLocEventRegMaskT_v02)0x00000004) /**<  The control point must enable this mask to receive NMEA reports for
       position and satellites in view. The report is at a 1 Hz rate.  */
#define QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000008) /**<  The control point must enable this mask to receive NI notify verify request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000010) /**<  The control point must enable this mask to receive time injection request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000020) /**<  The control point must enable this mask to receive predicted orbits request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000040) /**<  The control point must enable this mask to receive position injection request
       event indications.  */
#define QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000080) /**<  The control point must enable this mask to receive engine state report
       event indications.  */
#define QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000100) /**<  The control point must enable this mask to receive fix session status report
       event indications.  */
#define QMI_LOC_EVENT_MASK_WIFI_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000200) /**<  The control point must enable this mask to receive WiFi position request
       event indications.  */
#define QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS_V02 ((qmiLocEventRegMaskT_v02)0x00000400) /**<  The control point must enable this mask to receive notifications from the
       GPS engine indicating its readiness to accept data from the
       sensors (accelerometer, gyrometer, etc.).  */
#define QMI_LOC_EVENT_MASK_TIME_SYNC_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000800) /**<  The control point must enable this mask to receive time-sync requests from
       the GPS engine. Time sync enables the GPS engine to synchronize
       its clock with the sensor processor's clock.  */
#define QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00001000) /**<  The control point must enable this mask to receive Stationary Position
     Indicator (SPI) streaming report indications.  */
#define QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00002000) /**<  The control point must enable this mask to receive location server requests.
     These requests are generated when the service wishes to establish a
     connection with a location server. */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to register for events from the
                    location subsystem.  */
typedef struct {

  /* Mandatory */
  /*  Event Registration Mask */
  qmiLocEventRegMaskT_v02 eventRegMask;
  /**<   Specifies the events that the control point is interested in receiving.
       Refer to the definition of the following bitmasks: \n

       - 0x00000001 -- POSITION_REPORT \n
       - 0x00000002 -- GNSS_SV_INFO \n
       - 0x00000004 -- NMEA \n
       - 0x00000008 -- NI_NOTIFY_VERIFY_REQ \n
       - 0x00000010 -- INJECT_TIME_REQ \n
       - 0x00000020 -- INJECT_PREDICTED_ORBITS_REQ \n
       - 0x00000040 -- INJECT_POSITION_REQ \n
       - 0x00000080 -- ENGINE_STATE \n
       - 0x00000100 -- FIX_SESSION_STATE \n
       - 0x00000200 -- WIFI_REQ \n
       - 0x00000400 -- SENSOR_STREAMING_READY_STATUS \n
       - 0x00000800 -- TIME_SYNC_REQ \n
       - 0x00001000 -- SET_SPI_STREAMING_REPORT \n
       - 0x00002000 -- LOCATION_SERVER_CONNECTION_REQ

       Multiple events can be registered by ORing the individual masks and
       sending them in this TLV. All unused bits in this mask must be set to 0.
   */
}qmiLocRegEventsReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCFIXRECURRENCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_RECURRENCE_PERIODIC_V02 = 1, /**<  Request periodic position fixes.
 Request a single position fix.  */
  eQMI_LOC_RECURRENCE_SINGLE_V02 = 2,
  QMILOCFIXRECURRENCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocFixRecurrenceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCACCURACYLEVELENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ACCURACY_LOW_V02 = 1, /**<  Low accuracy.  */
  eQMI_LOC_ACCURACY_MED_V02 = 2, /**<  Medium accuracy.
 High accuracy.  */
  eQMI_LOC_ACCURACY_HIGH_V02 = 3,
  QMILOCACCURACYLEVELENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAccuracyLevelEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCINTERMEDIATEREPORTSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02 = 1, /**<  Intermediate reports are turned on.
 Intermediate reports are turned off.   */
  eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02 = 2,
  QMILOCINTERMEDIATEREPORTSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocIntermediateReportStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; The control point sends this message when it wants to
                    initiate a GPS session. */
typedef struct {

  /* Mandatory */
  /*  Session ID  */
  uint8_t sessionId;
  /**<   ID of the session as identified by the control point. The session ID
       is reported back in the position reports. The control point must
       specify the same session ID in the QMI_LOC_STOP_REQ message. \n
       - Type: Unsigned integer \n
       - Range: 0 to 255
   */

  /* Optional */
  /*  Recurrence Type */
  uint8_t fixRecurrence_valid;  /**< Must be set to true if fixRecurrence is being passed */
  qmiLocFixRecurrenceEnumT_v02 fixRecurrence;
  /**<   Specifies the type of session in which the control point is interested.
       If this TLV is not specified, recurrence defaults to SINGLE.

       Valid values: \n
         - 0x00000001 -- Request periodic fixes \n
         - 0x00000002 -- Request a single fix
   */

  /* Optional */
  /*  Horizontal Accuracy */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Specifies the horizontal accuracy level required by the control point.
       If not specified, accuracy defaults to LOW.

       Valid values: \n
         - 0x00000001 -- LOW: Client requires low horizontal accuracy.\n
         - 0x00000002 -- MED: Client requires medium horizontal accuracy.\n
         - 0x00000003 -- HIGH: Client requires high horizontal accuracy.
    */

  /* Optional */
  /*  Enable/Disable Intermediate Reports */
  uint8_t intermediateReportState_valid;  /**< Must be set to true if intermediateReportState is being passed */
  qmiLocIntermediateReportStateEnumT_v02 intermediateReportState;
  /**<   Specifies if the control point is interested in receiving intermediate
       reports. The control point must explicitly set this field to OFF if it
       does not wish to receive intermediate position reports. Intermediate
       position reports are generated at 1 Hz and are ON by default. If
       intermediate reports are turned ON, the client receives position reports
       even if the accuracy criteria are not met. The status in such a position
       report is set to IN_PROGRESS in order for the control point to identify
       intermediate reports.

       Valid values: \n
         - 0x00000001 -- ON: Client is interested in receiving intermediate reports \n
         - 0x00000002 -- OFF: Client is not interested in receiving intermediate reports
   */

  /* Optional */
  /*  Minimum Interval Between Position Reports */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Minimum time interval, specified by the control point, that must elapse between
       position reports. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds \n
       - Default: 1000 ms
   */
}qmiLocStartReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; The control point sends this message when it wants to
                    stop a GPS session. */
typedef struct {

  /* Mandatory */
  /*   Session ID */
  uint8_t sessionId;
  /**<   ID of the session that was specified in the Start request
       (QMI_LOC_START_REQ).\n
       - Type: Unsigned integer \n
       - Range: 0 to 255  */
}qmiLocStopReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPosTechMaskT_v02;
#define QMI_LOC_POS_TECH_MASK_SATELLITE_V02 ((qmiLocPosTechMaskT_v02)0x00000001) /**<  Satellites were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_CELLID_V02 ((qmiLocPosTechMaskT_v02)0x00000002) /**<  Cell towers were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_WIFI_V02 ((qmiLocPosTechMaskT_v02)0x00000004) /**<  WiFi access points were used to generate the fix.  */
/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSESSIONSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SESS_STATUS_SUCCESS_V02 = 0, /**<  Session was successful.       */
  eQMI_LOC_SESS_STATUS_IN_PROGRESS_V02 = 1, /**<  Session still in progress. Further position reports will be
       generated until either the fix criteria specified by the client
       are met or the client response timeout occurs.    */
  eQMI_LOC_SESS_STATUS_GENERAL_FAILURE_V02 = 2, /**<  Session failed.   */
  eQMI_LOC_SESS_STATUS_TIMEOUT_V02 = 3, /**<  Fix request failed because the session timed out.       */
  eQMI_LOC_SESS_STATUS_USER_END_V02 = 4, /**<  Fix request failed because the session was ended by the user.       */
  eQMI_LOC_SESS_STATUS_BAD_PARAMETER_V02 = 5, /**<  Fix request failed due to bad parameters in the request.  */
  eQMI_LOC_SESS_STATUS_PHONE_OFFLINE_V02 = 6, /**<  Fix request failed because the phone is offline.
 Fix request failed because the engine is locked.  */
  eQMI_LOC_SESS_STATUS_ENGINE_LOCKED_V02 = 7,
  QMILOCSESSIONSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSessionStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t gpsWeek;
  /**<   Current GPS week as calculated from midnight, Jan. 6, 1980. \n
       - Type: Unsigned integer \n
       - Units: Weeks  */

  uint32_t gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */
}qmiLocGPSTimeStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  float PDOP;
  /**<   Position dilution of precision.\n
       - Type: Floating point \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)\n
       - PDOP = square root of (HDOP^2 + VDOP^2)        */

  float HDOP;
  /**<   Horizontal dilution of precision.\n
       - Type: Floating point \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */

  float VDOP;
  /**<   Vertical dilution of precision.\n
       - Type: Floating point. \n
       - Range: 1 (highest accuracy) to 50 (lowest accuracy)  */
}qmiLocDOPStructT_v02;  /* Type */
/**
    @}
  */

typedef uint32_t qmiLocSensorUsageMaskT_v02;
#define QMI_LOC_SENSOR_MASK_USED_ACCEL_V02 ((qmiLocSensorUsageMaskT_v02)0x00000001) /**<  Bitmask to specify whether an accelerometer was used.  */
#define QMI_LOC_SENSOR_MASK_USED_GYRO_V02 ((qmiLocSensorUsageMaskT_v02)0x00000002) /**<  Bitmask to specify whether a gyrometer was used.  */
typedef uint32_t qmiLocSensorAidedMaskT_v02;
#define QMI_LOC_SENSOR_AIDED_MASK_HEADING_V02 ((qmiLocSensorAidedMaskT_v02)0x00000001) /**<  Bitmask to specify whether a sensor was used to calculate heading.  */
#define QMI_LOC_SENSOR_AIDED_MASK_SPEED_V02 ((qmiLocSensorAidedMaskT_v02)0x00000002) /**<  Bitmask to specify whether a sensor was used to calculate speed.  */
#define QMI_LOC_SENSOR_AIDED_MASK_POSITION_V02 ((qmiLocSensorAidedMaskT_v02)0x00000004) /**<  Bitmask to specify whether a sensor was used to calculate position.  */
#define QMI_LOC_SENSOR_AIDED_MASK_VELOCITY_V02 ((qmiLocSensorAidedMaskT_v02)0x00000008) /**<  Bitmask to specify whether a sensor was used to calculate velocity.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSensorUsageMaskT_v02 usageMask;
  /**<   Specifies which sensors are used.

       Valid bitmasks are specified by the following constants: \n
         - 0x00000001 -- SENSOR_USED_ACCEL \n
         - 0x00000002 -- SENSOR_USED_GYRO  */

  qmiLocSensorAidedMaskT_v02 aidingIndicatorMask;
  /**<   Specifies which results are aided by sensors.

       Valid bitmasks are specified by the following constants: \n
         - 0x00000001 -- AIDED_HEADING \n
         - 0x00000002 -- AIDED_SPEED \n
         - 0x00000004 -- AIDED_POSITION \n
         - 0x00000008 -- AIDED_VELOCITY  */
}qmiLocSensorUsageIndicatorStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCTIMESOURCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_TIME_SRC_INVALID_V02 = 0, /**<  Invalid time.  */
  eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER_V02 = 1, /**<  Time is set by the 1x system.  */
  eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING_V02 = 2, /**<  Time is set by WCDMA/GSM time tagging (i.e.,
       associating network time with GPS time).  */
  eQMI_LOC_TIME_SRC_EXTERNAL_INPUT_V02 = 3, /**<  Time is set by an external injection.  */
  eQMI_LOC_TIME_SRC_TOW_DECODE_V02 = 4, /**<  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite.  */
  eQMI_LOC_TIME_SRC_TOW_CONFIRMED_V02 = 5, /**<  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites.  */
  eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED_V02 = 6, /**<  Both time of the week and the GPS week number are known.  */
  eQMI_LOC_TIME_SRC_NAV_SOLUTION_V02 = 7, /**<  Time is set by the position engine after the fix is obtained.  */
  eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME_V02 = 8, /**<  Time is set by the position engine after performing SFT.
       This is done when the clock time uncertainty is large.  */
  eQMI_LOC_TIME_SRC_GLO_TOW_DECODE_V02 = 9, /**<  Time is set after decoding GLO satellites  */
  eQMI_LOC_TIME_SRC_TIME_TRANSFORM_V02 = 10, /**<  Time is set after transforming the GPS to GLO time  */
  eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING_V02 = 11, /**<  Time is set by the sleep time tag provided by the WCDMA network  */
  eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02 = 12, /**<  Time is set by the sleep time tag provided by the GSM network
 Source of the time is unknown  */
  eQMI_LOC_TIME_SRC_UNKNOWN_V02 = 13,
  QMILOCTIMESOURCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocTimeSourceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCRELIABILITYENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_RELIABILITY_NOT_SET_V02 = 0, /**<  Location reliability is not set  */
  eQMI_LOC_RELIABILITY_VERY_LOW_V02 = 1, /**<  Location reliability is very low; use it at your own risk  */
  eQMI_LOC_RELIABILITY_LOW_V02 = 2, /**<  Location reliability is low; little or no cross-checking is possible  */
  eQMI_LOC_RELIABILITY_MEDIUM_V02 = 3, /**<  Location reliability is medium; limited cross-check passed
 Location reliability is high; strong cross-check passed  */
  eQMI_LOC_RELIABILITY_HIGH_V02 = 4,
  QMILOCRELIABILITYENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocReliabilityEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; This message is used to send the position report to the
                    control point. */
typedef struct {

  /* Mandatory */
  /*   Session Status */
  qmiLocSessionStatusEnumT_v02 sessionStatus;
  /**<   Session status.

        Valid values: \n
          - 0x00000000 -- SESS_STATUS_SUCCESS \n
          - 0x00000001 -- SESS_STATUS_IN_PROGRESS \n
          - 0x00000002 -- SESS_STATUS_GENERAL_FAILURE \n
          - 0x00000003 -- SESS_STATUS_TIMEOUT \n
          - 0x00000004 -- SESS_STATUS_USER_END \n
          - 0x00000005 -- SESS_STATUS_BAD_PARAMETER \n
          - 0x00000006 -- SESS_STATUS_PHONE_OFFLINE \n
          - 0x00000007 -- SESS_STATUS_ENGINE_LOCKED
       */

  /* Mandatory */
  /*   Session ID */
  uint8_t sessionId;
  /**<    ID of the session that was specified in the Start request
        QMI_LOC_START_REQ. \n
        - Type: Unsigned integer \n
        - Range: 0 to 255  */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum). \n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude  */

  /* Optional */
  /*   Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude  */

  /* Optional */
  /*   Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Type: Floating point \n
       - Units: Decimal degrees \n
       - Range: 0 to 180  */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal uncertainty confidence.\n
       - Type: Unsigned integer \n
       - Units: Percent \n
       - Range: 0 to 99  */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \n
         - 0x00000000 -- RELIABILITY_NOT_SET \n
         - 0x00000001 -- RELIABILITY_VERY_LOW \n
         - 0x00000002 -- RELIABILITY_LOW \n
         - 0x00000003 -- RELIABILITY_MEDIUM \n
         - 0x00000004 -- RELIABILITY_HIGH
    */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t speedHorizontal_valid;  /**< Must be set to true if speedHorizontal is being passed */
  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Type: Floating point \n
       - Units: Meters/second  */

  /* Optional */
  /*  Speed Uncertainty */
  uint8_t speedUnc_valid;  /**< Must be set to true if speedUnc is being passed */
  float speedUnc;
  /**<   Speed uncertainty.\n
       - Type: Floating point \n
       - Units: Meters/second  */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Type: Floating point \n
       - Units: Meters \n
       - Range: -500 to 15883  */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Type: Unsigned integer \n
       - Units: Percent    \n
       - Range: 0 to 99  */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \n
          - 0x00000000 -- RELIABILITY_NOT_SET \n
          - 0x00000001 -- RELIABILITY_VERY_LOW \n
          - 0x00000002 -- RELIABILITY_LOW \n
          - 0x00000003 -- RELIABILITY_MEDIUM \n
          - 0x00000004 -- RELIABILITY_HIGH  */

  /* Optional */
  /*  Vertical Speed */
  uint8_t speedVertical_valid;  /**< Must be set to true if speedVertical is being passed */
  float speedVertical;
  /**<   Vertical speed.\n
         - Type: Floating point \n
         - Units: Meters/second  */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 359.999   */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: 0 to 359.999  */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. \n
      - Type: Floating point  */

  /* Optional */
  /*  Technology Used */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \n
         - 0x00000001 -- SATELLITE \n
         - 0x00000002 -- CELLID    \n
         - 0x00000004 -- WIFI  */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;
  /**<   \n Dilution of precision associated with this position.  */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds since Jan. 1, 1970  */

  /* Optional */
  /*  Leap Seconds */
  uint8_t leapSeconds_valid;  /**< Must be set to true if leapSeconds is being passed */
  uint8_t leapSeconds;
  /**<   Leap second information. If leapSeconds is not available,
         timestampUtc is calculated based on a hard-coded value
         for leap seconds. \n
         - Type: Unsigned integer \n
         - Units: Seconds  */

  /* Optional */
  /*  GPS Time \n */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \n The number of weeks since Jan. 5, 1980, and
       milliseconds into the current week.  */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Type: Floating point \n
       - Units: Milliseconds   */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source.

        Valid values: \n
          - 0x00000000 -- TIME_SRC_INVALID \n
          - 0x00000001 -- TIME_SRC_NETWORK_TIME_TRANSFER \n
          - 0x00000002 -- TIME_SRC_NETWORK_TIME_TAGGING \n
          - 0x00000003 -- TIME_SRC_EXTERNAL_ INPUT \n
          - 0x00000004 -- TIME_SRC_TOW_DECODE \n
          - 0x00000005 -- TIME_SRC_TOW_CONFIRMED \n
          - 0x00000006 -- TIME_SRC_TOW_AND_WEEK_CONFIRMED \n
          - 0x00000007 -- TIME_SRC_NAV_SOLUTION \n
          - 0x00000008 -- TIME_SRC_SOLVE_FOR_TIME  */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;
  /**<   \n Whether sensor data was used in computing the position in this
       position report.  */

  /* Optional */
  /*  Fix Count for This Session */
  uint8_t fixId_valid;  /**< Must be set to true if fixId is being passed */
  uint32_t fixId;
  /**<   Fix count for the session. Starts with 0 and increments by one
       for each successive position report for a particular session.  */
}qmiLocEventPositionReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSVSYSTEMENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SV_SYSTEM_GPS_V02 = 1, /**<  GPS satellite.  */
  eQMI_LOC_SV_SYSTEM_GALILEO_V02 = 2, /**<  GALILEO satellite.  */
  eQMI_LOC_SV_SYSTEM_SBAS_V02 = 3, /**<  SBAS satellite.  */
  eQMI_LOC_SV_SYSTEM_COMPASS_V02 = 4, /**<  COMPASS satellite.
 GLONASS satellite.  */
  eQMI_LOC_SV_SYSTEM_GLONASS_V02 = 5,
  QMILOCSVSYSTEMENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSvSystemEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSVSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SV_STATUS_IDLE_V02 = 1, /**<  SV is not being actively processed.  */
  eQMI_LOC_SV_STATUS_SEARCH_V02 = 2, /**<  The system is searching for this SV.
 SV is being tracked.  */
  eQMI_LOC_SV_STATUS_TRACK_V02 = 3,
  QMILOCSVSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSvStatusEnumT_v02;
/**
    @}
  */

typedef uint32_t qmiLocSvInfoValidMaskT_v02;
#define QMI_LOC_SV_INFO_MASK_VALID_SYSTEM_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000001) /**<  System field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_GNSS_SVID_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000002) /**<  gnssSvId field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_HEALTH_STATUS_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000004) /**<  healthStatus field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_PROCESS_STATUS_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000008) /**<  processStatus field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_SVINFO_MASK_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000010) /**<  svInfoMask field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_ELEVATION_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000020) /**<  Elevation field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_AZIMUTH_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000040) /**<  Azimuth field is valid in SV information.  */
#define QMI_LOC_SV_INFO_MASK_VALID_SNR_V02 ((qmiLocSvInfoValidMaskT_v02)0x00000080) /**<  SNR field is valid in SV information.  */
typedef uint8_t qmiLocSvInfoMaskT_v02;
#define QMI_LOC_SVINFO_MASK_HAS_EPHEMERIS_V02 ((qmiLocSvInfoMaskT_v02)0x01) /**<  Ephemeris is available for this SV.  */
#define QMI_LOC_SVINFO_MASK_HAS_ALMANAC_V02 ((qmiLocSvInfoMaskT_v02)0x02) /**<  Almanac is available for this SV.     */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSvInfoValidMaskT_v02 validMask;
  /**<   Bitmask indicating which of the fields in this TLV are valid.

         Valid bitmasks: \n
           - 0x00000001 -- VALID_SYSTEM \n
           - 0x00000002 -- VALID_GNSS_SVID \n
           - 0x00000004 -- VALID_HEALTH_STATUS \n
           - 0x00000008 -- VALID_PROCESS_STATUS \n
           - 0x00000010 -- VALID_SVINFO_MASK \n
           - 0x00000020 -- VALID_ELEVATION \n
           - 0x00000040 -- VALID_AZIMUTH \n
           - 0x00000080 -- VALID_SNR

     */

  qmiLocSvSystemEnumT_v02 system;
  /**<   Indicates to which constellation this SV belongs.

         Valid values: \n
           - 0x00000001 -- eQMI_LOC_SV_SYSTEM_GPS \n
           - 0x00000002 -- eQMI_LOC_SV_SYSTEM_GALILEO \n
           - 0x00000003 -- eQMI_LOC_SV_SYSTEM_SBAS \n
           - 0x00000004 -- eQMI_LOC_SV_SYSTEM_COMPASS \n
           - 0x00000005 -- eQMI_LOC_SV_SYSTEM_GLONASS
     */

  uint16_t gnssSvId;
  /**<   GNSS SV ID. \n

         - Type: Unsigned integer \n
         - Range: \n
             -- For GPS:     1 to 32 \n
             -- For SBAS:    33 to 64    \n
             -- For GLONASS: 65 to 96   */

  uint8_t healthStatus;
  /**<   Health status. \n
         - Type: Unsigned integer \n
         - Range: 0 = unhealthy; 1 = healthy  */

  qmiLocSvStatusEnumT_v02 svStatus;
  /**<   SV processing status. \n
         Valid values:\n
           - 0x00000001 -- SV_STATUS_IDLE \n
           - 0x00000002 -- SV_STATUS_SEARCH \n
           - 0x00000003 -- SV_STATUS_TRACK
     */

  qmiLocSvInfoMaskT_v02 svInfoMask;
  /**<   Whether almanac and ephemeris information is available. \n
         Valid bitmasks: \n
           - 0x01 -- SVINFO_HAS_EPHEMERIS \n
           - 0x02 -- SVINFO_HAS_ALMANAC
     */

  float elevation;
  /**<   SV elevation angle.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 90  */

  float azimuth;
  /**<   SV azimuth angle.\n
         - Type: Floating point \n
         - Units: Degrees \n
         - Range: 0 to 360  */

  float snr;
  /**<   SV signal-to-noise ratio. \n
         - Type: Floating point \n
         - Units: dB-Hz  */
}qmiLocSvInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used to send a satellite report to the control point. */
typedef struct {

  /* Mandatory */
  /*  Altitude Source */
  uint8_t altitudeAssumed;
  /**<   Altitude assumed or calculated:\n
         - 0x00 (FALSE) -- Valid altitude is calculated \n
         - 0x01 (TRUE) -- Valid altitude is assumed; there may not be enough
                          satellites to determine precise altitude  */

  /* Optional */
  /*  Satellite Info */
  uint8_t svList_valid;  /**< Must be set to true if svList is being passed */
  uint32_t svList_len;  /**< Must be set to # of elements in svList */
  qmiLocSvInfoStructT_v02 svList[QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   \n SV information list.  */
}qmiLocEventGnssSvInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used to send NMEA sentences to the control point. */
typedef struct {

  /* Mandatory */
  /*  NMEA String */
  char nmea[QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 + 1];
  /**<   NMEA string. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 201   */
}qmiLocEventNmeaIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNINOTIFYVERIFYENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_USER_NO_NOTIFY_NO_VERIFY_V02 = 1, /**<  No notification and no verification required.  */
  eQMI_LOC_NI_USER_NOTIFY_ONLY_V02 = 2, /**<  Notify only; no verification required.  */
  eQMI_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP_V02 = 3, /**<  Notify and verify, but no response required.  */
  eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02 = 4, /**<  Notify and verify, and require a response.
 Notify and Verify, and require a response.  */
  eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02 = 5,
  QMILOCNINOTIFYVERIFYENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiNotifyVerifyEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNIVXPOSMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_VX_MS_ASSISTED_ONLY_V02 = 1, /**<  MS-assisted only allowed.  */
  eQMI_LOC_NI_VX_MS_BASED_ONLY_V02 = 2, /**<  MS-based only allowed.  */
  eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02 = 3, /**<  MS-assisted preferred, but MS-based allowed.
 MS-based preferred, but MS-assisted allowed.  */
  eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02 = 4,
  QMILOCNIVXPOSMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiVxPosModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNIVXREQUESTORIDENCODINGSCHEMEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_VX_OCTET_V02 = 0, /**<  Encoding is OCTET.  */
  eQMI_LOC_NI_VX_EXN_PROTOCOL_MSG_V02 = 1, /**<  Encoding is EXN PROTOCOL MSG.  */
  eQMI_LOC_NI_VX_ASCII_V02 = 2, /**<  Encoding is ASCII.  */
  eQMI_LOC_NI_VX_IA5_V02 = 3, /**<  Encoding is IA5.  */
  eQMI_LOC_NI_VX_UNICODE_V02 = 4, /**<  Encoding is UNICODE.  */
  eQMI_LOC_NI_VX_SHIFT_JIS_V02 = 5, /**<  Encoding is SHIFT JIS.  */
  eQMI_LOC_NI_VX_KOREAN_V02 = 6, /**<  Encoding is KOREAN.  */
  eQMI_LOC_NI_VX_LATIN_HEBREW_V02 = 7, /**<  Encoding is LATIN HEBREW.  */
  eQMI_LOC_NI_VX_LATIN_V02 = 8, /**<  Encoding is LATIN.
 Encoding is GSM.  */
  eQMI_LOC_NI_VX_GSM_V02 = 9,
  QMILOCNIVXREQUESTORIDENCODINGSCHEMEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiVxRequestorIdEncodingSchemeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t posQosIncl;
  /**<   Whether quality of service is included:\n
         - 0x01 (TRUE) --  QoS is included \n
         - 0x00 (FALSE) -- QoS is not included  */

  uint8_t posQos;
  /**<   Position QoS timeout. \n
         - Type: Unsigned integer \n
         - Units: Seconds \n
         - Range: 0 to 255  */

  uint32_t numFixes;
  /**<   Number of fixes allowed. \n
         - Type: Unsigned integer  */

  uint32_t timeBetweenFixes;
  /**<   Time between fixes.\n
         - Type: Unsigned integer \n
         - Units: Seconds  */

  qmiLocNiVxPosModeEnumT_v02 posMode;
  /**<   Position mode.

         Valid values: \n
           - 0x00000001 -- NI_VX_MS_ASSISTED_ONLY \n
           - 0x00000002 -- NI_VX_MS_BASED_ONLY \n
           - 0x00000003 -- NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED \n
           - 0x00000004 -- NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED
     */

  qmiLocNiVxRequestorIdEncodingSchemeEnumT_v02 encodingScheme;
  /**<   VX encoding scheme.

         Valid values: \n
           - 0x00000000 -- NI_VX_OCTET \n
           - 0x00000001 -- NI_VX_EXN_PROTOCOL_MSG \n
           - 0x00000002 -- NI_VX_ASCII \n
           - 0x00000003 -- NI_VX_IA5 \n
           - 0x00000004 -- NI_VX_UNICODE \n
           - 0x00000005 -- NI_VX_SHIFT_JIS \n
           - 0x00000006 -- NI_VX_KOREAN \n
           - 0x00000007 -- NI_VX_LATIN_HEBREW \n
           - 0x00000008 -- NI_VX_LATIN \n
           - 0x00000009 -- NI_VX_GSM
     */

  uint32_t requestorId_len;  /**< Must be set to # of elements in requestorId */
  uint8_t requestorId[QMI_LOC_NI_MAX_REQUESTOR_ID_LENGTH_V02];
  /**<   Requestor ID. \n
       - Type:  Array of bytes \n
       - Maximum array length: 200
   */

  uint16_t userRespTimerInSeconds;
  /**<   Time to wait for the user to respond. \n
         - Type: Unsigned integer \n
         - Units: Seconds  */
}qmiLocNiVxNotifyVerifyStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNISUPLPOSMETHODENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_V02 = 1, /**<  Set assisted.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_V02 = 2, /**<  Set based.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETASSISTED_PREF_V02 = 3, /**<  Set assisted preferred.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_AGPS_SETBASED_PREF_V02 = 4, /**<  Set based preferred.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_AUTONOMOUS_GPS_V02 = 5, /**<  Standalone GPS.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_AFLT_V02 = 6, /**<  Advanced forward link trilateration.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_ECID_V02 = 7, /**<  Exclusive chip ID.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_EOTD_V02 = 8, /**<  Enhnaced observed time difference.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02 = 9, /**<  Observed time delay of arrival.
 No position.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02 = 10,
  QMILOCNISUPLPOSMETHODENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiSuplPosMethodEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNIDATACODINGSCHEMEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_SS_GERMAN_V02 = 12, /**<  Language is German.  */
  eQMI_LOC_NI_SS_ENGLISH_V02 = 13, /**<  Language is English.  */
  eQMI_LOC_NI_SS_ITALIAN_V02 = 14, /**<  Language is Italian.  */
  eQMI_LOC_NI_SS_FRENCH_V02 = 15, /**<  Language is French.  */
  eQMI_LOC_NI_SS_SPANISH_V02 = 16, /**<  Language is Spanish.  */
  eQMI_LOC_NI_SS_DUTCH_V02 = 17, /**<  Language is Dutch.   */
  eQMI_LOC_NI_SS_SWEDISH_V02 = 18, /**<  Language is Swedish.  */
  eQMI_LOC_NI_SS_DANISH_V02 = 19, /**<  Language is Danish.  */
  eQMI_LOC_NI_SS_PORTUGUESE_V02 = 20, /**<  Language is Portuguese.  */
  eQMI_LOC_NI_SS_FINNISH_V02 = 21, /**<  Language is Finnish.  */
  eQMI_LOC_NI_SS_NORWEGIAN_V02 = 22, /**<  Language is Norwegian.  */
  eQMI_LOC_NI_SS_GREEK_V02 = 23, /**<  Language is Greek.  */
  eQMI_LOC_NI_SS_TURKISH_V02 = 24, /**<  Language is Turkish.  */
  eQMI_LOC_NI_SS_HUNGARIAN_V02 = 25, /**<  Language is Hungarian.  */
  eQMI_LOC_NI_SS_POLISH_V02 = 26, /**<  Language is Polish.  */
  eQMI_LOC_NI_SS_LANGUAGE_UNSPEC_V02 = 27, /**<  Language is unspecified.  */
  eQMI_LOC_NI_SUPL_UTF8_V02 = 28, /**<  Encoding is UTF 8.  */
  eQMI_LOC_NI_SUPL_UCS2_V02 = 29, /**<  Encoding is UCS 2.
 Encoding is GSM default.  */
  eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02 = 30,
  QMILOCNIDATACODINGSCHEMEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiDataCodingSchemeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNISUPLFORMATENUMTYPE_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_SUPL_FORMAT_LOGICAL_NAME_V02 = 0, /**<  SUPL logical name format.  */
  eQMI_LOC_NI_SUPL_FORMAT_EMAIL_ADDRESS_V02 = 1, /**<  SUPL email address format.  */
  eQMI_LOC_NI_SUPL_FORMAT_MSISDN_V02 = 2, /**<  SUPL logical name format.  */
  eQMI_LOC_NI_SUPL_FORMAT_URL_V02 = 3, /**<  SUPL URL format.  */
  eQMI_LOC_NI_SUPL_FORMAT_SIP_URL_V02 = 4, /**<  SUPL SIP URL format.  */
  eQMI_LOC_NI_SUPL_FORMAT_MIN_V02 = 5, /**<  SUPL MIN format.  */
  eQMI_LOC_NI_SUPL_FORMAT_MDN_V02 = 6, /**<  SUPL MDN format.  */
  eQMI_LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY_V02 = 7, /**<  SUPL IMS public identity
 SUPL unknown format.  */
  eQMI_LOC_NI_SUPL_FORMAT_OSS_UNKNOWN_V02 = 2147483647,
  QMILOCNISUPLFORMATENUMTYPE_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiSuplFormatEnumType_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiSuplFormatEnumType_v02 formatType;
  /**<   Format of the formatted string.

        Valid values: \n
          - 0x00000000 -- FORMAT_LOGICAL_NAME \n
          - 0x00000001 -- FORMAT_EMAIL_ADDRESS \n
          - 0x00000002 -- FORMAT_MSISDN \n
          - 0x00000003 -- FORMAT_URL \n
          - 0x00000004 -- FORMAT_SIP_URL \n
          - 0x00000005 -- FORMAT_MIN \n
          - 0x00000006 -- FORMAT_MDN \n
          - 0x00000007 -- FORMAT_IMSPUBLIC_IDENTITY \n
          - 0x7FFFFFFF -- FORMAT_OSS_UNKNOWN
    */

  uint32_t formattedString_len;  /**< Must be set to # of elements in formattedString */
  uint8_t formattedString[QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02];
  /**<   Formatted string. \n
        - Type: Byte array \n
        - Maximum string length: 64
         */
}qmiLocNiSuplFormattedStringStructT_v02;  /* Type */
/**
    @}
  */

typedef uint8_t qmiLocSuplQopValidMaskT_v02;
#define QMI_LOC_NI_SUPL_MASK_QOP_HORZ_ACC_VALID_V02 ((qmiLocSuplQopValidMaskT_v02)0x01) /**<  Horizontal accuracy is valid in the Quality of Position (QoP).  */
#define QMI_LOC_NI_SUPL_MASK_QOP_VER_ACC_VALID_V02 ((qmiLocSuplQopValidMaskT_v02)0x02) /**<  Vertical accuracy is valid in the QoP.  */
#define QMI_LOC_NI_SUPL_MASK_QOP_MAXAGE_VALID_V02 ((qmiLocSuplQopValidMaskT_v02)0x04) /**<  Vertical accuracy is valid in the QoP.  */
#define QMI_LOC_NI_SUPL_MASK_QOP_DELAY_VALID_V02 ((qmiLocSuplQopValidMaskT_v02)0x08) /**<  Vertical accuracy is valid in the QoP.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSuplQopValidMaskT_v02 validMask;
  /**<   Bit field indicating which fields are valid in this value.

        Valid bitmasks: \n
          - 0x01 -- QOP_HORZ_ACC_VALID \n
          - 0x02 -- QOP_VER_ACC_VALID \n
          - 0x04 -- QOP_MAXAGE_VALID \n
          - 0x08 -- QOP_DELAY_VALID */

  uint8_t horizontalAccuracy;
  /**<   Horizontal accuracy. \n
        - Type: Unsigned integer \n
        - Units: Meters  */

  uint8_t verticalAccuracy;
  /**<   Vertical accuracy. \n
        - Type: Unsigned integer \n
        - Units: Meters  */

  uint16_t maxLocAge;
  /**<   Maximum age of the location if the engine sends a previously
        computed position. \n
        - Type: Unsigned integer \n
        - Units: Seconds  */

  uint8_t delay;
  /**<   Delay the server is willing to tolerate for the fix. \n
        - Type: Unsigned integer \n
        - Units: Seconds  */
}qmiLocNiSuplQopStructT_v02;  /* Type */
/**
    @}
  */

typedef uint8_t qmiLocServerAddrTypeMaskT_v02;
#define QMI_LOC_SERVER_ADDR_TYPE_IPV4_MASK_V02 ((qmiLocServerAddrTypeMaskT_v02)0x01) /**<  IPV4 server address type.  */
#define QMI_LOC_SERVER_ADDR_TYPE_IPV6_MASK_V02 ((qmiLocServerAddrTypeMaskT_v02)0x02) /**<  IPV6 server address type.  */
#define QMI_LOC_SERVER_ADDR_TYPE_URL_MASK_V02 ((qmiLocServerAddrTypeMaskT_v02)0x04) /**<  URL server address type.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t addr;
  /**<   IPV4 address. \n
       - Type: Unsigned integer  */

  uint16_t port;
  /**<   IPV4 port. \n
       - Type: Unsigned integer  */
}qmiLocIpV4AddrStructType_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t addr[QMI_LOC_IPV6_ADDR_LENGTH_V02];
  /**<   IPV6 address. \n
       - Type: Array of unsigned integers \n
       - Maximum length of the array: 8  */

  uint32_t port;
  /**<   IPV6 port. \n
       - Type: Unsigned integer  */
}qmiLocIpV6AddrStructType_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocServerAddrTypeMaskT_v02 suplServerAddrTypeMask;
  /**<   Mask specifying the valid fields in this value.

       Valid bitmasks: \n
         - 0x01 -- IPV4 \n
         - 0x02 -- IPV6 \n
         - 0x04 -- URL
   */

  qmiLocIpV4AddrStructType_v02 ipv4Addr;
  /**<   IPV4 address and port.  */

  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   IPV6 address and port.  */

  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 256
         */
}qmiLocNiSuplServerInfoStructT_v02;  /* Type */
/**
    @}
  */

typedef uint32_t qmiLocNiSuplNotifyVerifyValidMaskT_v02;
#define QMI_LOC_SUPL_SERVER_INFO_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000001) /**<  Mask to denote that the server information
      is present in an NI SUPL notify verify request event. This mask is set in
     the valid_flags field of a notify verify structure.  */
#define QMI_LOC_SUPL_SESSION_ID_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000002) /**<  Mask to denote that the SUPL session ID
       is present in an NI SUPL notify verify request event.
      This mask is set in the valid_flags field of a
      notify verify structure.   */
#define QMI_LOC_SUPL_HASH_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000004) /**<  Mask to denote that the SUPL hash is present
       in an NI notify verify request event.
      This mask is set in the valid_flags field of a
      notify verify structure.   */
#define QMI_LOC_SUPL_POS_METHOD_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000008) /**<  Mask to denote that the position method is present
       in an NI SUPL notify verify request event.
      This mask is set in the valid_flags field of a
      notify verify structure.  */
#define QMI_LOC_SUPL_DATA_CODING_SCHEME_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000010) /**<  Mask to denote that the data coding scheme
       is present in an NI SUPL notify verify request event.
      This mask is set in the valid_flags field of a
      notify verify structure.  */
#define QMI_LOC_SUPL_REQUESTOR_ID_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000020) /**<  Mask to denote that the requestor ID
       is present in an NI notify verify request event.
      This mask is set in the valid_flags field of a
      notify verify structure.  */
#define QMI_LOC_SUPL_CLIENT_NAME_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000040) /**<  Mask to denote that the requestor ID
       is present in an NI notify verify request event.
       This mask is set in the valid_flags field of a
       notify verify structure.  */
#define QMI_LOC_SUPL_QOP_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000080) /**<  Mask to denote that the quality of position
       is present in an NI notify verify request event.
       This mask is set in the valid_flags field of a
       notify verify structure.  */
#define QMI_LOC_SUPL_USER_RESP_TIMER_MASK_V02 ((qmiLocNiSuplNotifyVerifyValidMaskT_v02)0x00000100) /**<  Mask to denote that the user response timer
       is present in an NI notify verify request event.
       This mask is set in the valid_flags field of a
       notify verify structure.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiSuplNotifyVerifyValidMaskT_v02 valid_flags;
  /**<   Indicates which of the following fields are present in this value.

        Valid bitmasks: \n
          - 0x00000001 -- SUPL_SERVER_INFO \n
          - 0x00000002 -- SUPL_SESSION_ID \n
          - 0x00000004 -- SUPL_HASH \n
          - 0x00000008 -- SUPL_POS_METHOD \n
          - 0x00000010 -- SUPL_DATA_CODING_SCHEME \n
          - 0x00000020 -- SUPL_REQUESTOR_ID \n
          - 0x00000040 -- SUPL_CLIENT_NAME \n
          - 0x00000080 -- SUPL_QOP \n
          - 0x00000100 -- SUPL_USER_RESP_TIMER
   */

  qmiLocNiSuplServerInfoStructT_v02 suplServerInfo;
  /**<   SUPL server information.  */

  uint8_t suplSessionId[QMI_LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH_V02];
  /**<   SUPL session ID. \n
       - Type: Array of unsigned integers \n
       - Maximum length of the array: 4  */

  uint8_t suplHash[QMI_LOC_NI_SUPL_HASH_LENGTH_V02];
  /**<   Hash for SUPL_INIT; used to validate that the message was not
       corrupted. \n
       - Type: Array of unsigned integers \n
       - Length of the array: 8  */

  qmiLocNiSuplPosMethodEnumT_v02 posMethod;
  /**<   GPS mode to be used for the fix.

       Valid values: \n
         - 0x00000001 -- AGPS_SETASSISTED \n
         - 0x00000002 -- AGPS_SETBASED \n
         - 0x00000003 -- AGPS_SETASSISTED_PREF \n
         - 0x00000004 -- AGPS_SETBASED_PREF \n
         - 0x00000005 -- AUTONOMOUS_GPS \n
         - 0x00000006 -- AFLT \n
         - 0x00000007 -- ECID \n
         - 0x00000008 -- EOTD \n
         - 0x00000009 -- OTDOA \n
         - 0x0000000A -- NO_POSITION
   */

  qmiLocNiDataCodingSchemeEnumT_v02 dataCodingScheme;
  /**<   Data coding scheme applies to both the requestor ID and the client
       name.

       Valid values: \n
         - 0x0000000C -- NI_SS_GERMAN \n
         - 0x0000000D -- NI_SS_ENGLISH \n
         - 0x0000000E -- NI_SS_ITALIAN \n
         - 0x0000000F -- NI_SS_FRENCH \n
         - 0x00000010 -- NI_SS_SPANISH \n
         - 0x00000011 -- NI_SS_DUTCH \n
         - 0x00000012 -- NI_SS_SWEDISH \n
         - 0x00000013 -- NI_SS_DANISH \n
         - 0x00000014 -- NI_SS_PORTUGUESE \n
         - 0x00000015 -- NI_SS_FINNISH \n
         - 0x00000016 -- NI_SS_NORWEGIAN \n
         - 0x00000017 -- NI_SS_GREEK \n
         - 0x00000018 -- NI_SS_TURKISH \n
         - 0x00000019 -- NI_SS_HUNGARIAN \n
         - 0x0000001A -- NI_SS_POLISH \n
         - 0x0000001B -- NI_SS_LANGUAGE_UNSPEC \n
         - 0x0000001C -- NI_SUPL_UTF8 \n
         - 0x0000001D -- NI_SUPL_UCS2 \n
         - 0x0000001E -- NI_SUPL_GSM_DEFAULT
    */

  qmiLocNiSuplFormattedStringStructT_v02 requestorId;
  /**<   Requestor ID. The encoding scheme for requestor_id is specified in
       the dataCodingScheme field.  */

  qmiLocNiSuplFormattedStringStructT_v02 clientName;
  /**<   Client name. The encoding scheme for client_name is specified in
       the dataCodingScheme field.  */

  qmiLocNiSuplQopStructT_v02 suplQop;
  /**<   SUPL QoP.  */

  uint16_t userResponseTimer;
  /**<   Time to wait for the user to respond. \n
       - Type: Unsigned integer \n
       - Units: Seconds */
}qmiLocNiSuplNotifyVerifyStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNILOCATIONTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_LOCATIONTYPE_CURRENT_LOCATION_V02 = 1, /**<  Current location.  */
  eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02 = 2, /**<  Last known location; may be current location.
 Initial location.  */
  eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02 = 3,
  QMILOCNILOCATIONTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiLocationTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiDataCodingSchemeEnumT_v02 dataCodingScheme;
  /**<   Identifies the coding scheme of the coded string.

       Valid values: \n
         - 0x0000000C -- NI_SS_GERMAN \n
         - 0x0000000D -- NI_SS_ENGLISH \n
         - 0x0000000E -- NI_SS_ITALIAN \n
         - 0x0000000F -- NI_SS_FRENCH \n
         - 0x00000010 -- NI_SS_SPANISH \n
         - 0x00000011 -- NI_SS_DUTCH \n
         - 0x00000012 -- NI_SS_SWEDISH \n
         - 0x00000013 -- NI_SS_DANISH \n
         - 0x00000014 -- NI_SS_PORTUGUESE \n
         - 0x00000015 -- NI_SS_FINNISH \n
         - 0x00000016 -- NI_SS_NORWEGIAN \n
         - 0x00000017 -- NI_SS_GREEK \n
         - 0x00000018 -- NI_SS_TURKISH \n
         - 0x00000019 -- NI_SS_HUNGARIAN \n
         - 0x0000001A -- NI_SS_POLISH \n
         - 0x0000001B -- NI_SS_LANGUAGE_UNSPEC \n
         - 0x0000001C -- NI_SUPL_UTF8 \n
         - 0x0000001D -- NI_SUPL_UCS2 \n
         - 0x0000001E -- NI_SUPL_GSM_DEFAULT

   */

  uint32_t codedString_len;  /**< Must be set to # of elements in codedString */
  uint8_t codedString[QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02];
  /**<   Coded string. \n
       - Type: Array of bytes \n
       - Maximum string length: 20  */
}qmiLocNiUmtsCpCodedStringStructT_v02;  /* Type */
/**
    @}
  */

typedef uint16_t qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02;
#define QMI_LOC_UMTS_CP_INVOKE_ID_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0001) /**<  Mask to denote that the invoke ID
       is present in an NI notify verify request event.
      This mask is set in the valid flags field of a
      notify verify structure.  */
#define QMI_LOC_UMTS_CP_DATA_CODING_SCHEME_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0002) /**<  Mask to denote that the data coding scheme
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
      notify verify structure.  */
#define QMI_LOC_UMTS_CP_NOTIFICATION_TEXT_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0004) /**<  Mask to denote that the notification text
       is present in an NI notify verify request event.
      This mask is set in the valid flags field of a
      notify verify structure.  */
#define QMI_LOC_UMTS_CP_CLIENT_ADDRESS_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0008) /**<  Mask to denote that the client address
       is present in an NI notify verify request event.
      This mask is set in the valid flags field of a
      notify verify structure.  */
#define QMI_LOC_UMTS_CP_LOCATION_TYPE_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0010) /**<  Mask to denote that the location type
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
      notify verify structure.  */
#define QMI_LOC_UMTS_CP_REQUESTOR_ID_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0020) /**<  Mask to denote that the requestor ID
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
       notify verify structure.  */
#define QMI_LOC_UMTS_CP_CODEWORD_STRING_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0040) /**<  Mask to denote that the code word string
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
       notify verify structure.  */
#define QMI_LOC_UMTS_CP_SERVICE_TYPE_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0080) /**<  Mask to denote that the service type
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
       notify verify structure.  */
#define QMI_LOC_UMTS_CP_USER_RESP_TIMER_MASK_V02 ((qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02)0x0100) /**<  Mask to denote that the user response timer
       is present in an NI notify verify request event.
       This mask is set in the valid flags field of a
       notify verify structure.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiUmtsCpNotifyVerifyValidMaskT_v02 valid_flags;
  /**<   Fields that are valid in this value.

       Valid bitmasks: \n
         - 0x0001 -- INVOKE_ID_MASK \n
         - 0x0002 -- DATA_CODING_SCHEME_MASK \n
         - 0x0004 -- NOTIFICATION_TEXT_MASK \n
         - 0x0008 -- CLIENT_ADDRESS_MASK \n
         - 0x0010 -- LOCATION_TYPE_MASK \n
         - 0x0020 -- REQUESTOR_ID_MASK \n
         - 0x0040 -- CODEWORD_STRING_MASK \n
         - 0x0080 -- SERVICE_TYPE_MASK \n
         - 0x0100 -- USER_RESP_TIMER_MASK
   */

  uint8_t invokeId;
  /**<   Supplementary Services invoke ID. \n
       - Type: Unsigned integer  */

  qmiLocNiDataCodingSchemeEnumT_v02 dataCodingScheme;
  /**<   Type of data encoding scheme for the text.
       Applies to both the notification text and the client address.

       Valid values: \n
         - 0x0000000C -- NI_SS_GERMAN \n
         - 0x0000000D -- NI_SS_ENGLISH \n
         - 0x0000000E -- NI_SS_ITALIAN \n
         - 0x0000000F -- NI_SS_FRENCH \n
         - 0x00000010 -- NI_SS_SPANISH \n
         - 0x00000011 -- NI_SS_DUTCH \n
         - 0x00000012 -- NI_SS_SWEDISH \n
         - 0x00000013 -- NI_SS_DANISH \n
         - 0x00000014 -- NI_SS_PORTUGUESE \n
         - 0x00000015 -- NI_SS_FINNISH \n
         - 0x00000016 -- NI_SS_NORWEGIAN \n
         - 0x00000017 -- NI_SS_GREEK \n
         - 0x00000018 -- NI_SS_TURKISH \n
         - 0x00000019 -- NI_SS_HUNGARIAN \n
         - 0x0000001A -- NI_SS_POLISH \n
         - 0x0000001B -- NI_SS_LANGUAGE_UNSPEC \n
         - 0x0000001C -- NI_SUPL_UTF8 \n
         - 0x0000001D -- NI_SUPL_UCS2 \n
         - 0x0000001E -- NI_SUPL_GSM_DEFAULT
  */

  uint32_t notificationText_len;  /**< Must be set to # of elements in notificationText */
  uint8_t notificationText[QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02];
  /**<   Notification text; the encoding method is specified in
       dataCodingScheme. \n
       - Type: Array of bytes \n
       - Maximum array length: 64  */

  uint32_t clientAddress_len;  /**< Must be set to # of elements in clientAddress */
  uint8_t clientAddress[QMI_LOC_NI_MAX_EXT_CLIENT_ADDRESS_V02];
  /**<   Client address; the encoding method is specified in
       dataCodingScheme. \n
       - Maximum array length: 20  */

  qmiLocNiLocationTypeEnumT_v02 locationType;
  /**<   Location type.

       Valid values: \n
         - 0x00000001 -- CURRENT_LOCATION \n
         - 0x00000002 -- CURRENT_OR_LAST_KNOWN_LOCATION \n
         - 0x00000004 -- INITIAL_LOCATION
   */

  qmiLocNiUmtsCpCodedStringStructT_v02 requestorId;
  /**<   Requestor ID; the encoding method is specified in the
       qmiLocNiUmtsCpCodedStringStructT.dataCodingScheme field.  */

  qmiLocNiUmtsCpCodedStringStructT_v02 codewordString;
  /**<   Codeword string; the encoding method is specified in the
       qmiLocNiUmtsCpCodedStringStructT.dataCodingScheme field.  */

  uint8_t lcsServiceTypeId;
  /**<   Service type ID. \n
       - Type: Unsigned integer  */

  uint16_t userResponseTimer;
  /**<   Time to wait for the user to respond. \n
       - Type: Unsigned integer \n
       - Units: Seconds  */
}qmiLocNiUmtsCpNotifyVerifyStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNISERVICEINTERACTIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_SERVICE_INTERACTION_ONGOING_NI_INCOMING_MO_V02 = 1, /**<  Service interaction between ongoing NI and incoming MO sessions.  */
  QMILOCNISERVICEINTERACTIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiServiceInteractionEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiVxNotifyVerifyStructT_v02 niVxReq;
  /**<   Ongoing NI session request; this information is currently not filled.  */

  qmiLocNiServiceInteractionEnumT_v02 serviceInteractionType;
  /**<   Service interaction type specified in qmiLocNiServiceInteractionEnumT.

        Valid values: \n
          - 0x00000001 -- ONGOING_NI_INCOMING_MO
    */
}qmiLocNiVxServiceInteractionStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Indicates an NI notify/verify request to the control point. */
typedef struct {

  /* Mandatory */
  /*  Notification Type */
  qmiLocNiNotifyVerifyEnumT_v02 notificationType;
  /**<   Type of notification/verification performed.

       Valid values: \n
         - 0x00000001 -- NO_NOTIFY_NO_VERIFY \n
         - 0x00000002 -- NOTIFY_ONLY \n
         - 0x00000003 -- ALLOW_NO_RESP \n
         - 0x00000004 -- NOT_ALLOW_NO_RESP \n
         - 0x00000005 -- PRIVACY_OVERRIDE
   */

  /* Optional */
  /*  Network Initiated Vx Request */
  uint8_t NiVxInd_valid;  /**< Must be set to true if NiVxInd is being passed */
  qmiLocNiVxNotifyVerifyStructT_v02 NiVxInd;
  /**<   \n Optional NI Vx request payload.  */

  /* Optional */
  /*  Network Initiated SUPL Request */
  uint8_t NiSuplInd_valid;  /**< Must be set to true if NiSuplInd is being passed */
  qmiLocNiSuplNotifyVerifyStructT_v02 NiSuplInd;
  /**<   \n Optional NI SUPL request payload.  */

  /* Optional */
  /*  Network Initiated UMTS Control Plane Request */
  uint8_t NiUmtsCpInd_valid;  /**< Must be set to true if NiUmtsCpInd is being passed */
  qmiLocNiUmtsCpNotifyVerifyStructT_v02 NiUmtsCpInd;
  /**<   \n Optional NI UMTS-CP request payload.  */

  /* Optional */
  /*  Network Initiated Service Interaction Request */
  uint8_t NiVxServiceInteractionInd_valid;  /**< Must be set to true if NiVxServiceInteractionInd is being passed */
  qmiLocNiVxServiceInteractionStructT_v02 NiVxServiceInteractionInd;
  /**<   \n Optional NI service interaction payload.  */
}qmiLocEventNiNotifyVerifyReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  char serverUrl[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   Assistance server URL. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 256  */
}qmiLocAssistanceServerUrlStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t delayThreshold;
  /**<   The time server is to be skipped if a one-way delay to the server
       exceeds this threshold. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  uint32_t timeServerList_len;  /**< Must be set to # of elements in timeServerList */
  qmiLocAssistanceServerUrlStructT_v02 timeServerList[QMI_LOC_MAX_NTP_SERVERS_V02];
  /**<   List of Time Server URL's that are recommended by the service for time
       information, the list is ordered, the client is to use the first
       server specified in the list as the primary URL to fetch NTP time,
       the second one as secondary, and so on. \n
       - Maximum server list items: 3  */
}qmiLocTimeServerListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to inject time information. */
typedef struct {

  /* Optional */
  /*  Time Server Info */
  uint8_t timeServerInfo_valid;  /**< Must be set to true if timeServerInfo is being passed */
  qmiLocTimeServerListStructT_v02 timeServerInfo;
  /**<   \n Contains information about the time servers recommended by the
       location service for NTP time.  */
}qmiLocEventInjectTimeReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t maxFileSizeInBytes;
  /**<   Maximum allowable predicted orbits file size (in bytes). \n
         - Type: Unsigned integer  */

  uint32_t maxPartSize;
  /**<   Maximum allowable predicted orbits file chunk size (in bytes). \n
         - Type: Unsigned integer  */
}qmiLocPredictedOrbitsAllowedSizesStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t serverList_len;  /**< Must be set to # of elements in serverList */
  qmiLocAssistanceServerUrlStructT_v02 serverList[QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02];
  /**<   List of predicted orbits URLs. The list is ordered, so the client
       must use the first server specified in the list as the primary URL
       from which to download predicted orbits data, the second one as
       secondary, and so on. \n
       - Maximum number of servers that can be specified: 3  */
}qmiLocPredictedOrbitsServerListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to inject predicted orbits data. */
typedef struct {

  /* Mandatory */
  /*  Allowed Sizes */
  qmiLocPredictedOrbitsAllowedSizesStructT_v02 allowedSizes;
  /**<   \n Maximum part and file size allowed to be injected in the engine.  */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \n List of servers that can be used by the client to download
       predicted orbits data.  */
}qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to inject a position. */
typedef struct {

  /* Mandatory */
  /*  Latitude */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude  */

  /* Mandatory */
  /*  Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude  */

  /* Mandatory */
  /*  Circular Horizontal Uncertainty */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Mandatory */
  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds since Jan. 1, 1970  */
}qmiLocEventInjectPositionReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCENGINESTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ENGINE_STATE_ON_V02 = 1, /**<  Location engine is on.
 Location engine is off.  */
  eQMI_LOC_ENGINE_STATE_OFF_V02 = 2,
  QMILOCENGINESTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocEngineStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends the engine state to the control point. */
typedef struct {

  /* Mandatory */
  /*  Engine State */
  qmiLocEngineStateEnumT_v02 engineState;
  /**<   Location engine state.

         Valid values: \n
           - 0x00000001 -- ON \n
           - 0x00000002 -- OFF
     */
}qmiLocEventEngineStateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCFIXSESSIONSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_FIX_SESSION_STARTED_V02 = 1, /**<  Location fix session has started.
 Location fix session has ended.  */
  eQMI_LOC_FIX_SESSION_FINISHED_V02 = 2,
  QMILOCFIXSESSIONSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocFixSessionStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends the fix session state to the control point. */
typedef struct {

  /* Mandatory */
  /*  Session State */
  qmiLocFixSessionStateEnumT_v02 sessionState;
  /**<   LOC fix session state.

         Valid values: \n
           - 0x00000001 -- STARTED \n
           - 0x00000002 -- FINISHED
     */

  /* Optional */
  /*  Session ID */
  uint8_t sessionId_valid;  /**< Must be set to true if sessionId is being passed */
  uint8_t sessionId;
  /**<   ID of the session that was specified in the Start request.
    This may not be specified for a fix session corresponding to
    a network-initiated request. \n
    - Type: Unsigned integer \n
    - Range: 0 to 255  */
}qmiLocEventFixSessionStateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCWIFIREQUESTENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_WIFI_START_PERIODIC_HI_FREQ_FIXES_V02 = 0, /**<  Start periodic fixes with high frequency.  */
  eQMI_LOC_WIFI_START_PERIODIC_KEEP_WARM_V02 = 1, /**<  Keep warm for low frequency fixes without data downloads.
 Stop periodic fixes request.  */
  eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02 = 2,
  QMILOCWIFIREQUESTENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocWifiRequestEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends a WiFi request to the control point. */
typedef struct {

  /* Mandatory */
  /*  Request Type */
  qmiLocWifiRequestEnumT_v02 requestType;
  /**<   Request type as specified in qmiWifiRequestEnumT.

        Valid values: \n
          - 0x00000000 -- START_PERIODIC_HI_FREQ_FIXES \n
          - 0x00000001 -- START_PERIODIC_KEEP_WARM \n
          - 0x00000002 -- STOP_PERIODIC_FIXES
    */

  /* Optional */
  /*  Time Between Fixes */
  uint8_t tbfInMs_valid;  /**< Must be set to true if tbfInMs is being passed */
  uint16_t tbfInMs;
  /**<   Time between fixes for a periodic request.\n
        - Type: Unsigned integer \n
        - Units: Milliseconds  */
}qmiLocEventWifiReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t samplesPerBatch;
  /**<   Specifies the number of samples per batch the GNSS location engine is to
       receive. The sensor sampling frequency can be computed as follows: \n

       samplingFrequency = samplesPerBatch * batchesPerSecond \n

       samplesPerBatch must be a non-zero positive value.
   */

  uint16_t batchesPerSecond;
  /**<   Number of sensor-data batches the GNSS location engine is to receive
       per second. The rate is specified in integral number of batches per
       second (Hz). \n

       batchesPerSecond must be a non-zero positive value.
   */
}qmiLocSensorControlConfigSamplingSpecStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t injectEnable;
  /**<   Whether the GNSS location engine is ready to accept data from this
       sensor.

       Valid values: \n
       - 0x01 (TRUE)  -- GNSS location engine is ready to accept sensor data \n
       - 0x00 (FALSE) -- GNSS location engine is not ready to accept sensor
                         data
   */

  qmiLocSensorControlConfigSamplingSpecStructT_v02 dataFrequency;
  /**<   Rate at which the GNSS engine would like the sensor to be sampled. \n
       The rate is specified in integral number of samples per second (Hz)\n
       and batches per second.
   */
}qmiLocSensorReadyStatusStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point if the GNSS location engine is
                    ready to accept sensor data. */
typedef struct {

  /* Optional */
  /*  Accelerometer Accept Ready */
  uint8_t accelReady_valid;  /**< Must be set to true if accelReady is being passed */
  qmiLocSensorReadyStatusStructT_v02 accelReady;
  /**<   \n Whether the GNSS location engine is ready to accept accelerometer
         sensor data.
    */

  /* Optional */
  /*  Gyrometer Accept Ready */
  uint8_t gyroReady_valid;  /**< Must be set to true if gyroReady is being passed */
  qmiLocSensorReadyStatusStructT_v02 gyroReady;
  /**<   \n Whether the GNSS location engine is ready to accept gyrometer sensor
         data.
   */
}qmiLocEventSensorStreamingReadyStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point to inject time synchronization
                    data.  */
typedef struct {

  /* Mandatory */
  /*  Opaque Time Sync Reference Counter */
  uint32_t refCounter;
  /**<   This TLV is sent to registered control points. It is sent by
        the location engine when it needs to synchronize location engine and
        control point (sensor processor) times.
        This TLV must be echoed back in the Time Sync Inject request. \n
        - Type: Unsigned integer  */
}qmiLocEventTimeSyncReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Requests the control point to enable Stationary Position
                    Indicator (SPI) streaming reports.  */
typedef struct {

  /* Mandatory */
  /*  Enable/Disable SPI Requests */
  uint8_t enable;
  /**<   Whether the client is to start or stop sending an SPI status stream.\n
       - 0x01 (TRUE)  -- Client is to start sending an SPI status stream\n
       - 0x00 (FALSE) -- Client is to stop sending an SPI status stream  */
}qmiLocEventSetSpiStreamingReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCWWANTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_WWAN_TYPE_INTERNET_V02 = 0, /**<  Bring up the WWAN type used for an Internet connection.
 Bring up the WWAN type used for AGNSS connections.  */
  eQMI_LOC_WWAN_TYPE_AGNSS_V02 = 1,
  QMILOCWWANTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocWWANTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSERVERREQUESTENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SERVER_REQUEST_OPEN_V02 = 1, /**<  Open a connection to the location server.
 Close a connection to the location server.  */
  eQMI_LOC_SERVER_REQUEST_CLOSE_V02 = 2,
  QMILOCSERVERREQUESTENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocServerRequestEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Requests the client to open or close a connection
                    to the assisted GPS location server.  */
typedef struct {

  /* Mandatory */
  /*  Connection Handle */
  uint32_t connHandle;
  /**<   Identifies a connection across Open and Close request events. \n
       - Type: Unsigned integer  */

  /* Mandatory */
  /*  Request Type */
  qmiLocServerRequestEnumT_v02 requestType;
  /**<   Open or close a connection to the location server.

       Valid values: \n
         - 0x00000001 -- OPEN \n
         - 0x00000002 -- CLOSE
   */

  /* Mandatory */
  /*  WWAN Type */
  qmiLocWWANTypeEnumT_v02 wwanType;
  /**<   Identifies the WWAN type for this request. \n
       Valid values: \n
         - 0x00000000 -- WWAN_TYPE_INTERNET \n
         - 0x00000001 -- WWAN_TYPE_AGNSS
    */
}qmiLocEventLocationServerConnectionReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUCCESS_V02 = 0, /**<  Request was completed successfully.       */
  eQMI_LOC_GENERAL_FAILURE_V02 = 1, /**<  Request failed because of a general failure.  */
  eQMI_LOC_UNSUPPORTED_V02 = 2, /**<  Request failed because it is unsupported.  */
  eQMI_LOC_INVALID_PARAMETER_V02 = 3, /**<  Request failed because it contained invalid parameters.    */
  eQMI_LOC_ENGINE_BUSY_V02 = 4, /**<  Request failed because the engine is busy.  */
  eQMI_LOC_PHONE_OFFLINE_V02 = 5, /**<  Request failed because the phone is offline.
 Request failed because it timed out.  */
  eQMI_LOC_TIMEOUT_V02 = 6,
  QMILOCSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocStatusEnumT_v02;
/**
    @}
  */

/*
 * qmiLocGetServiceRevisionReqMsgT is empty
 * typedef struct {
 * }qmiLocGetServiceRevisionReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Client can query the service revision using this message. */
typedef struct {

  /* Mandatory */
  /*  Get Revision Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Revision request.

        Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Mandatory */
  /*  Interface Definition Minor Revision */
  uint32_t revision;
  /**<   Revision of the service. This is the minor revision of the interface that
       the service implements. Minor revision updates of the service are always
       backward compatible. \n
       - Type: Unsigned integer  */
}qmiLocGetServiceRevisionIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetFixCriteriaReqMsgT is empty
 * typedef struct {
 * }qmiLocGetFixCriteriaReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the fix criteria from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Fix Criteria Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Fix Criteria request.

        Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Horizontal Accuracy */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level.

       Valid values: \n
         - 0x00000001 -- LOW: Client requires low horizontal accuracy. \n
         - 0x00000002 -- MED: Client requires medium horizontal accuracy. \n
         - 0x00000003 -- HIGH: Client requires high horizontal accuracy.
    */

  /* Optional */
  /*  Enable/Disable Intermediate Fixes */
  uint8_t intermediateReportState_valid;  /**< Must be set to true if intermediateReportState is being passed */
  qmiLocIntermediateReportStateEnumT_v02 intermediateReportState;
  /**<   Intermediate Report state (ON, OFF).\n
       The client must explicitly set this field to OFF to stop receiving
       intermediate position reports. Intermediate position reports are
       generated at \n 1 Hz and are ON by default. If intermediate reports
       are turned ON, the client receives position reports even if the
       accuracy criteria is not met. The status in the position report is
       set to IN_PROGRESS for intermediate reports. \n
       Valid values: \n
         - 0x00000001 -- ON: Client is interested in receiving intermediate reports \n
         - 0x00000002 -- OFF: Client is not interested in receiving intermediate reports
   */

  /* Optional */
  /*  Mimimum Interval Between Fixes */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Time that must elapse before alerting the client. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */
}qmiLocGetFixCriteriaIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNIUSERRESPENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT_V02 = 1, /**<  User accepted notify verify request.  */
  eQMI_LOC_NI_LCS_NOTIFY_VERIFY_DENY_V02 = 2, /**<  User denied notify verify request.
 User did not respond to notify verify request.  */
  eQMI_LOC_NI_LCS_NOTIFY_VERIFY_NORESP_V02 = 3,
  QMILOCNIUSERRESPENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiUserRespEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sends the NI user response back to the engine; success or
                      failure is reported in a separate indication. */
typedef struct {

  /* Mandatory */
  /*  User Response */
  qmiLocNiUserRespEnumT_v02 userResp;
  /**<   User accepted or denied.

       Valid values: \n
         - 0x00000001 -- NOTIFY_VERIFY_ACCEPT \n
         - 0x00000002 -- NOTIFY_VERIFY_DENY \n
         - 0x00000003 -- NOTIFY_VERIFY_NORESP
   */

  /* Mandatory */
  /*  Notification Type */
  qmiLocNiNotifyVerifyEnumT_v02 notificationType;
  /**<   Type of notification/verification performed.

       Valid values: \n
         - 0x00000001 -- NO_NOTIFY_NO_VERIFY \n
         - 0x00000002 -- NOTIFY_ONLY \n
         - 0x00000003 -- ALLOW_NO_RESP \n
         - 0x00000004 -- NOT_ALLOW_NO_RESP \n
         - 0x00000005 -- PRIVACY_OVERRIDE
   */

  /* Optional */
  /*  Network Initiated Vx Request */
  uint8_t NiVxPayload_valid;  /**< Must be set to true if NiVxPayload is being passed */
  qmiLocNiVxNotifyVerifyStructT_v02 NiVxPayload;
  /**<   \n Optional NI VX request payload.  */

  /* Optional */
  /*  Network Initiated SUPL Request */
  uint8_t NiSuplPayload_valid;  /**< Must be set to true if NiSuplPayload is being passed */
  qmiLocNiSuplNotifyVerifyStructT_v02 NiSuplPayload;
  /**<   \n Optional NI SUPL request payload.  */

  /* Optional */
  /*  Network Initiated UMTS Control Plane Request */
  uint8_t NiUmtsCpPayload_valid;  /**< Must be set to true if NiUmtsCpPayload is being passed */
  qmiLocNiUmtsCpNotifyVerifyStructT_v02 NiUmtsCpPayload;
  /**<   \n Optional NI UMTS-CP request payload.  */

  /* Optional */
  /*  Network Initiated Service Interaction Request */
  uint8_t NiVxServiceInteractionPayload_valid;  /**< Must be set to true if NiVxServiceInteractionPayload is being passed */
  qmiLocNiVxServiceInteractionStructT_v02 NiVxServiceInteractionPayload;
  /**<   \n Optional NI service interaction payload.          */
}qmiLocNiUserRespReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends the NI user response back to the engine; success or
                      failure is reported in a separate indication. */
typedef struct {

  /* Mandatory */
  /*  NI User Response Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the NI User Response request.

          Valid values: \n
            - 0x00000000 -- SUCCESS \n
            - 0x00000001 -- GENERAL_FAILURE \n
            - 0x00000002 -- UNSUPPORTED \n
            - 0x00000003 -- INVALID_PARAMETER \n
            - 0x00000004 -- ENGINE_BUSY \n
            - 0x00000005 -- PHONE_OFFLINE \n
            - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocNiUserRespIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCPREDICTEDORBITSDATAFORMATENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_PREDICTED_ORBITS_XTRA_V02 = 0, /**<  Default is QCOM-XTRA format.  */
  QMILOCPREDICTEDORBITSDATAFORMATENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPredictedOrbitsDataFormatEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects predicted orbits data.  */
typedef struct {

  /* Mandatory */
  /*  Total Size */
  uint32_t totalSize;
  /**<   Total size of the predicted orbits data to be injected. \n
        - Type: Unsigned integer \n
        - Units: Bytes  */

  /* Mandatory */
  /*  Total Parts */
  uint16_t totalParts;
  /**<   Total number of parts into which the predicted orbits data is
        divided. \n
        - Type: Unsigned integer  */

  /* Mandatory */
  /*  Part Number */
  uint16_t partNum;
  /**<   Number of the current predicted orbits data part; starts at 1. \n
        - Type: Unsigned integer  */

  /* Mandatory */
  /*  Data */
  uint32_t partData_len;  /**< Must be set to # of elements in partData */
  char partData[QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02];
  /**<   Predicted orbits data. \n
         - Type: Array of bytes \n
         - Maximum length of the array: 1024
     */

  /* Optional */
  /*  Format Type */
  uint8_t formatType_valid;  /**< Must be set to true if formatType is being passed */
  qmiLocPredictedOrbitsDataFormatEnumT_v02 formatType;
  /**<   Predicted orbits data format.

        Valid values: \n
          - 0x00000000 -- PREDICTED_ORBITS_XTRA
    */
}qmiLocInjectPredictedOrbitsDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects predicted orbits data.  */
typedef struct {

  /* Mandatory */
  /*  Data Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Data Injection request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Part Number */
  uint8_t partNum_valid;  /**< Must be set to true if partNum is being passed */
  uint16_t partNum;
  /**<   Number of the predicted orbits data part for which this indication
      is sent; starts at 1. \n
      - Type: Unsigned integer  */
}qmiLocInjectPredictedOrbitsDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetPredictedOrbitsDataSourceReqMsgT is empty
 * typedef struct {
 * }qmiLocGetPredictedOrbitsDataSourceReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the predicted orbits data source. */
typedef struct {

  /* Mandatory */
  /*  Predicted Oribits Data Source Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the query request for a predicted orbits data source.

         Valid values: \n
           - 0x00000000 -- SUCCESS \n
           - 0x00000001 -- GENERAL_FAILURE \n
           - 0x00000002 -- UNSUPPORTED \n
           - 0x00000003 -- INVALID_PARAMETER \n
           - 0x00000004 -- ENGINE_BUSY \n
           - 0x00000005 -- PHONE_OFFLINE \n
           - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Allowed Sizes */
  uint8_t allowedSizes_valid;  /**< Must be set to true if allowedSizes is being passed */
  qmiLocPredictedOrbitsAllowedSizesStructT_v02 allowedSizes;
  /**<   \n Maximum part and file size allowed to be injected in the engine.  */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \n List of servers that can be used by the client to download
       predicted orbits data.  */
}qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetPredictedOrbitsDataValidityReqMsgT is empty
 * typedef struct {
 * }qmiLocGetPredictedOrbitsDataValidityReqMsgT_v02;
 */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint64_t startTimeInUTC;
  /**<   Predicted orbits data is valid starting from this time. \n
       - Type: Unsigned integer \n
       - Units: Seconds (since Jan. 1, 1970)
         */

  uint16_t durationHours;
  /**<   Duration from the start time for which the data is valid.\n
       - Type: Unsigned integer \n
       - Units: Hours
         */
}qmiLocPredictedOrbitsDataValidityStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the predicted orbits data validity. */
typedef struct {

  /* Mandatory */
  /*  Predicted Orbits Data Validity Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the query request for predicted orbits data validity.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Validity Info */
  uint8_t validityInfo_valid;  /**< Must be set to true if validityInfo is being passed */
  qmiLocPredictedOrbitsDataValidityStructT_v02 validityInfo;
}qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects UTC time in the location engine.  */
typedef struct {

  /* Mandatory */
  /*  UTC Time */
  uint64_t timeUtc;
  /**<   UTC time since Jan. 1, 1970.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds
         */

  /* Mandatory */
  /*  Time Uncertainty */
  uint32_t timeUnc;
  /**<   Time uncertainty.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds
         */
}qmiLocInjectUtcTimeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects UTC time in the location engine.  */
typedef struct {

  /* Mandatory */
  /*  UTC Time Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the UTC Time Injection request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocInjectUtcTimeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_SRC_UNKNOWN_V02 = 0, /**<  Source is unknown.  */
  eQMI_LOC_ALT_SRC_GPS_V02 = 1, /**<  GPS is the source.  */
  eQMI_LOC_ALT_SRC_CELL_ID_V02 = 2, /**<  Cell ID provided the source.  */
  eQMI_LOC_ALT_SRC_ENHANCED_CELL_ID_V02 = 3, /**<  Source is enhanced cell ID.  */
  eQMI_LOC_ALT_SRC_WIFI_V02 = 4, /**<  WiFi is the source.  */
  eQMI_LOC_ALT_SRC_TERRESTRIAL_V02 = 5, /**<  Terrestrial source.  */
  eQMI_LOC_ALT_SRC_TERRESTRIAL_HYBRID_V02 = 6, /**<  Hybrid terrestrial source.  */
  eQMI_LOC_ALT_SRC_ALTITUDE_DATABASE_V02 = 7, /**<  Altitude database is the source.  */
  eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER_V02 = 8, /**<  Barometric altimeter is the source.
 Other sources.  */
  eQMI_LOC_ALT_SRC_OTHER_V02 = 9,
  QMILOCALTSRCENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCLINKAGEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_SRC_LINKAGE_NOT_SPECIFIED_V02 = 0, /**<  Not specified.  */
  eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INTERDEPENDENT_V02 = 1, /**<  Fully interdependent.  */
  eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG_V02 = 2, /**<  Depends on latitude and longitude.
 Fully independent.  */
  eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT_V02 = 3,
  QMILOCALTSRCLINKAGEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcLinkageEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCALTSRCUNCERTAINTYCOVERAGEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ALT_UNCERTAINTY_NOT_SPECIFIED_V02 = 0, /**<  Not specified.  */
  eQMI_LOC_ALT_UNCERTAINTY_POINT_V02 = 1, /**<  Altitude uncertainty is valid at the injected horizontal
       position coordinates only.
 Altitude uncertainty applies to the position of the device
       regardless of horizontal position (within the horizontal
       uncertainty region, if provided).  */
  eQMI_LOC_ALT_UNCERTAINTY_FULL_V02 = 2,
  QMILOCALTSRCUNCERTAINTYCOVERAGEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocAltSrcUncertaintyCoverageEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocAltSrcEnumT_v02 source;
  /**<   Specifies the source of the altitude.

       Valid values: \n
         - 0x00000000 -- ALT_SRC_UNKNOWN \n
         - 0x00000001 -- ALT_SRC_GPS \n
         - 0x00000002 -- ALT_SRC_CELL_ID \n
         - 0x00000003 -- ALT_SRC_ENHANCED_CELL_ID \n
         - 0x00000004 -- ALT_SRC_WIFI \n
         - 0x00000005 -- ALT_SRC_TERRESTRIAL \n
         - 0x00000006 -- ALT_SRC_TERRESTRIAL_HYBRID \n
         - 0x00000007 -- ALT_SRC_ALTITUDE_DATABASE \n
         - 0x00000008 -- ALT_SRC_BAROMETRIC_ALTIMETER \n
         - 0x00000009 -- ALT_SRC_OTHER
   */

  qmiLocAltSrcLinkageEnumT_v02 linkage;
  /**<   Specifies the dependency between the horizontal and
       altitude position components.

       Valid values: \n
         - 0x00000000 -- SRC_LINKAGE_NOT_SPECIFIED \n
         - 0x00000001 -- SRC_LINKAGE_FULLY_INTERDEPENDENT \n
         - 0x00000002 -- SRC_LINKAGE_DEPENDS_ON_LAT_LONG \n
         - 0x00000003 -- SRC_LINKAGE_FULLY_INDEPENDENT
   */

  qmiLocAltSrcUncertaintyCoverageEnumT_v02 coverage;
  /**<   Specifies the region of uncertainty.

        Valid values: \n
          - 0x00000000 -- UNCERTAINTY_NOT_SPECIFIED \n
          - 0x00000001 -- UNCERTAINTY_POINT: Altitude uncertainty is valid at the
                          injected horizontal position coordinates only. \n
          - 0x00000002 -- UNCERTAINTY_FULL: Altitude uncertainty applies to the
                          position of the device regardless of horizontal position
                          (within the horizontal uncertainty region, if provided).
   */
}qmiLocAltitudeSrcInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCPOSITIONSRCENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_POSITION_SRC_GNSS_V02 = 0, /**<  Position Source is GNSS.  */
  eQMI_LOC_POSITION_SRC_CELLID_V02 = 1, /**<  Position Source is Cell ID.  */
  eQMI_LOC_POSITION_SRC_ENH_CELLID_V02 = 2, /**<  Position Source is Enhanced Cell ID.  */
  eQMI_LOC_POSITION_SRC_WIFI_V02 = 3, /**<  Position Source is WiFi.  */
  eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02 = 4, /**<  Position Source is Terrestrial.  */
  eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02 = 5, /**<  Position Source is GNSS Terrestrial Hybrid.
 Other sources.   */
  eQMI_LOC_POSITION_SRC_OTHER_V02 = 6,
  QMILOCPOSITIONSRCENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocPositionSrcEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects a position to the location engine. */
typedef struct {

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -90.0 to 90.0 \n
       - Positive values indicate northern latitude \n
       - Negative values indicate southern latitude
     */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).\n
       - Type: Floating point \n
       - Units: Degrees \n
       - Range: -180.0 to 180.0 \n
       - Positive values indicate eastern longitude \n
       - Negative values indicate western longitude
    */

  /* Optional */
  /*  Circular Horizontal Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
        - Type: Floating point \n
        - Units: Meters  */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal confidence, as defined by  ETSI TS 101 109.\n
        - Type: Unsigned integer \n
        - Units: Percent (0 to 99) \n
        - 0 -- invalid value \n
        - 100 to 256 -- not used \n
        - If 100 is received, reinterpret to 99 \n
        This field must be specified together with horizontal uncertainty.
        If not specified, the default value will be 50.  */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \n
         - 0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET \n
         - 0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW \n
         - 0x00000002 -- eQMI_LOC_RELIABILITY_LOW \n
         - 0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM \n
         - 0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
       */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
        - Type: Floating point \n
        - Units: Meters; positive = height, negative = depth   */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Type: Floating point \n
       - Units: Meters  */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty. This is mandatory if either altitudeWrtEllipsoid
        or altitudeWrtMeanSeaLevel is specified.\n
        - Type: Floating point \n
        - Units: Meters  */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical confidence, as defined by  ETSI TS 101 109.\n
        - Type: Unsigned integer \n
        - Units: Percent (0-99) \n
        - 0 -- invalid value\n
        - 100 to 256 -- not used \n
        - If 100 is received, reinterpret to 99\n
        This field must be specified together with the vertical uncertainty.
        If not specified, the default value will be 50.  */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \n
          - 0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET \n
          - 0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW \n
          - 0x00000002 -- eQMI_LOC_RELIABILITY_LOW \n
          - 0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM \n
          - 0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
     */

  /* Optional */
  /*  Altitude Source Info */
  uint8_t altSourceInfo_valid;  /**< Must be set to true if altSourceInfo is being passed */
  qmiLocAltitudeSrcInfoStructT_v02 altSourceInfo;
  /**<   \n Specifies information regarding the altitude source.  */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
        - Type: Unsigned integer \n
        - Units: Milliseconds (since Jan. 1, 1970)  */

  /* Optional */
  /*  Position Age */
  uint8_t timestampAge_valid;  /**< Must be set to true if timestampAge is being passed */
  int32_t timestampAge;
  /**<   Position age, which is an estimate of how long ago this fix was made. \n
        - Type: Signed integer \n
        - Units: Milliseconds  */

  /* Optional */
  /*  Position Source */
  uint8_t positionSrc_valid;  /**< Must be set to true if positionSrc is being passed */
  qmiLocPositionSrcEnumT_v02 positionSrc;
  /**<   Source from where this position was obtained.
        Valid values: \n
          - 0x00000000 -- eQMI_LOC_POSITION_SRC_GNSS \n
          - 0x00000001 -- eQMI_LOC_POSITION_SRC_CELLID \n
          - 0x00000002 -- eQMI_LOC_POSITION_SRC_ENH_CELLID  \n
          - 0x00000003 -- eQMI_LOC_POSITION_SRC_WIFI \n
          - 0x00000004 -- eQMI_LOC_POSITION_SRC_TERRESTRIAL \n
          - 0x00000005 -- eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID \n
          - 0x00000006 -- eQMI_LOC_POSITION_SRC_OTHER \n

       If altitude is specified and altitude source is not specified, the engine
       assumes that altitude was obtained using the specified Position source.
       If altitude and altitude source both are specified then the engine assumes
       that only latitude, longitude were obtained using the specified Position
       Source.
    */
}qmiLocInjectPositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects a position to the location engine. */
typedef struct {

  /* Mandatory */
  /*  UTC Position Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the UTC Position Injection request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocInjectPositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCLOCKENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_LOCK_NONE_V02 = 1, /**<  Do not lock any position sessions.  */
  eQMI_LOC_LOCK_MI_V02 = 2, /**<  Lock mobile-initiated position sessions.  */
  eQMI_LOC_LOCK_MT_V02 = 3, /**<  Lock mobile-terminated position sessions.
 Lock all position sessions.  */
  eQMI_LOC_LOCK_ALL_V02 = 4,
  QMILOCLOCKENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocLockEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the location engine lock. */
typedef struct {

  /* Mandatory */
  /*  Lock Type */
  qmiLocLockEnumT_v02 lockType;
  /**<   Type of lock.

       Valid values: \n
         - 0x00000001 -- LOCK_NONE \n
         - 0x00000002 -- LOCK_MI \n
         - 0x00000003 -- LOCK_MT \n
         - 0x00000004 -- LOCK_ALL

   */
}qmiLocSetEngineLockReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sets the location engine lock. */
typedef struct {

  /* Mandatory */
  /*  Set Engine Lock Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Engine Lock request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetEngineLockIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetEngineLockReqMsgT is empty
 * typedef struct {
 * }qmiLocGetEngineLockReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the location engine lock. */
typedef struct {

  /* Mandatory */
  /*  Get Engine Lock Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Engine Lock request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Lock Type */
  uint8_t lockType_valid;  /**< Must be set to true if lockType is being passed */
  qmiLocLockEnumT_v02 lockType;
  /**<   Type of lock.

       Valid values: \n
         - 0x00000001 -- LOCK_NONE \n
         - 0x00000002 -- LOCK_MI \n
         - 0x00000003 -- LOCK_MT \n
         - 0x00000004 -- LOCK_ALL
   */
}qmiLocGetEngineLockIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the SBAS configuration. */
typedef struct {

  /* Mandatory */
  /*  SBAS Config */
  uint8_t sbasConfig;
  /**<   Whether SBAS configuration is enabled. \n
       - 0x01 (TRUE) -- SBAS configuration is enabled \n
       - 0x00 (FALSE) -- SBAS configuration is disabled  */
}qmiLocSetSbasConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sets the SBAS configuration. */
typedef struct {

  /* Mandatory */
  /*  Set SBAS Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set SBAS Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetSbasConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetSbasConfigReqMsgT is empty
 * typedef struct {
 * }qmiLocGetSbasConfigReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the SBAS configuration from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get SBAS Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get SBAS Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  SBAS Config */
  uint8_t sbasConfig_valid;  /**< Must be set to true if sbasConfig is being passed */
  uint8_t sbasConfig;
  /**<   Whether SBAS configuration is enabled. \n
       - 0x01 (TRUE) -- SBAS configuration is enabled \n
       - 0x00 (FALSE) -- SBAS configuration is disabled  */
}qmiLocGetSbasConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocNmeaSentenceMaskT_v02;
#define QMI_LOC_NMEA_MASK_GGA_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000001) /**<  Enable GGA type.  */
#define QMI_LOC_NMEA_MASK_RMC_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000002) /**<  Enable RMC type.  */
#define QMI_LOC_NMEA_MASK_GSV_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000004) /**<  Enable GSV type.  */
#define QMI_LOC_NMEA_MASK_GSA_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000008) /**<  Enable GSA type.  */
#define QMI_LOC_NMEA_MASK_VTG_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000010) /**<  Enable VTG type.  */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the NMEA types. */
typedef struct {

  /* Mandatory */
  /*  NMEA Sentence Types */
  qmiLocNmeaSentenceMaskT_v02 nmeaSentenceType;
  /**<   Bitmasks of NMEA types to enable.

       Valid bitmasks: \n
         - 0x00000001 -- NMEA_MASK_GGA \n
         - 0x00000002 -- NMEA_MASK_RMC \n
         - 0x00000004 -- NMEA_MASK_GSV \n
         - 0x00000008 -- NMEA_MASK_GSA \n
         - 0x00000010 -- NMEA_MASK_VTG
          */
}qmiLocSetNmeaTypesReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sets the NMEA types. */
typedef struct {

  /* Mandatory */
  /*  Set NMEA Types Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of Set NMEA Types request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetNmeaTypesIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetNmeaTypesReqMsgT is empty
 * typedef struct {
 * }qmiLocGetNmeaTypesReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the NMEA types from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get NMEA Types Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get NMEA Types request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  NMEA Sentence Types */
  uint8_t nmeaSentenceType_valid;  /**< Must be set to true if nmeaSentenceType is being passed */
  qmiLocNmeaSentenceMaskT_v02 nmeaSentenceType;
  /**<   NMEA types to enable.

       Valid bitmasks: \n
         - 0x0000ffff -- NMEA_MASK_ALL \n
         - 0x00000001 -- NMEA_MASK_GGA \n
         - 0x00000002 -- NMEA_MASK_RMC \n
         - 0x00000004 -- NMEA_MASK_GSV \n
         - 0x00000008 -- NMEA_MASK_GSA \n
         - 0x00000010 -- NMEA_MASK_VTG
          */
}qmiLocGetNmeaTypesIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Enables/disables Low Power Mode (LPM) configuration. */
typedef struct {

  /* Mandatory */
  /*  Enable Low Power Mode */
  uint8_t lowPowerMode;
  /**<   Whether to enable Low Power mode:\n
       - 0x01 (TRUE) -- Enable LPM \n
       - 0x00 (FALSE) -- Disable LPM  */
}qmiLocSetLowPowerModeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Enables/disables Low Power Mode (LPM) configuration. */
typedef struct {

  /* Mandatory */
  /*  Set LPM Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Low Power Mode request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetLowPowerModeIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetLowPowerModeReqMsgT is empty
 * typedef struct {
 * }qmiLocGetLowPowerModeReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the LPM status from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get LPM Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get LPM request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Enable/Disable LPM */
  uint8_t lowPowerMode_valid;  /**< Must be set to true if lowPowerMode is being passed */
  uint8_t lowPowerMode;
  /**<   Whether to enable Low Power mode:\n
       - 0x01 (TRUE) -- Enable LPM \n
       - 0x00 (FALSE) -- Disable LPM  */
}qmiLocGetLowPowerModeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSERVERTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SERVER_TYPE_CDMA_PDE_V02 = 1, /**<  Server type is CDMA PDE.  */
  eQMI_LOC_SERVER_TYPE_CDMA_MPC_V02 = 2, /**<  Server type is CDMA MPC.  */
  eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02 = 3, /**<  Server type is UMTS SLP.
 Server type is custom PDE.  */
  eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02 = 4,
  QMILOCSERVERTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocServerTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Specifies the A-GPS server type and address. */
typedef struct {

  /* Mandatory */
  /*  Server Type */
  qmiLocServerTypeEnumT_v02 serverType;
  /**<   Type of server.

        Valid values: \n
          - 0x00000001 -- CDMA_PDE \n
          - 0x00000002 -- CDMA_MPC \n
          - 0x00000003 -- UMTS_SLP \n
          - 0x00000004 -- CUSTOM_PDE
   */

  /* Optional */
  /*  IPV4 Address */
  uint8_t ipv4Addr_valid;  /**< Must be set to true if ipv4Addr is being passed */
  qmiLocIpV4AddrStructType_v02 ipv4Addr;
  /**<   \n IPV4 address and port.  */

  /* Optional */
  /*  IPV6 Address */
  uint8_t ipv6Addr_valid;  /**< Must be set to true if ipv6Addr is being passed */
  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   \n IPV6 address and port.  */

  /* Optional */
  /*  Uniform Resource Locator */
  uint8_t urlAddr_valid;  /**< Must be set to true if urlAddr is being passed */
  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL address. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 256
   */
}qmiLocSetServerReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Specifies the A-GPS server type and address. */
typedef struct {

  /* Mandatory */
  /*  Set Server Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Server request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetServerIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Gets the location server from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Server Type */
  qmiLocServerTypeEnumT_v02 serverType;
  /**<   Type of server, as defined in qmiLocServerTypeEnumT.

       Valid values: \n
         - 0x00000001 -- CDMA_PDE \n
         - 0x00000002 -- CDMA_MPC \n
         - 0x00000003 -- UMTS_SLP \n
         - 0x00000004 -- CUSTOM_PDE
   */

  /* Optional */
  /*  Server Address Type */
  uint8_t serverAddrTypeMask_valid;  /**< Must be set to true if serverAddrTypeMask is being passed */
  qmiLocServerAddrTypeMaskT_v02 serverAddrTypeMask;
  /**<   Type of address the client wants. If unspecified, the
       indication will contain all the types of addresses
       it has for the specified server type.

       Valid bitmasks: \n
         - 0x01 -- IPV4 \n
         - 0x02 -- IPV6 \n
         - 0x04 -- URL
   */
}qmiLocGetServerReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the location server from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Server Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Server request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Mandatory */
  /*  Server Type */
  qmiLocServerTypeEnumT_v02 serverType;
  /**<   Type of server, as defined in qmiLocServerTypeEnumT.

       Valid values: \n
         - 0x00000001 -- CDMA_PDE \n
         - 0x00000002 -- CDMA_MPC \n
         - 0x00000003 -- UMTS_SLP \n
         - 0x00000004 -- CUSTOM_PDE
   */

  /* Optional */
  /*  IPV4 Address */
  uint8_t ipv4Addr_valid;  /**< Must be set to true if ipv4Addr is being passed */
  qmiLocIpV4AddrStructType_v02 ipv4Addr;
  /**<   \n IPV4 address and port.  */

  /* Optional */
  /*  IPV6 Address */
  uint8_t ipv6Addr_valid;  /**< Must be set to true if ipv6Addr is being passed */
  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   \n IPV6 address and port.  */

  /* Optional */
  /*  Uniform Resource Locator */
  uint8_t urlAddr_valid;  /**< Must be set to true if urlAddr is being passed */
  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 256
   */
}qmiLocGetServerIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocDeleteGnssDataMaskT_v02;
#define QMI_LOC_MASK_DELETE_GPS_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000001) /**<  Mask to delete GPS SVDIR.   */
#define QMI_LOC_MASK_DELETE_GPS_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000002) /**<  Mask to delete GPS SVSTEER.   */
#define QMI_LOC_MASK_DELETE_GPS_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000004) /**<  Mask to delete GPS time.  */
#define QMI_LOC_MASK_DELETE_GPS_ALM_CORR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000008) /**<  Mask to delete almanac correlation.  */
#define QMI_LOC_MASK_DELETE_GLO_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000010) /**<  Mask to delete GLONASS SVDIR.   */
#define QMI_LOC_MASK_DELETE_GLO_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000020) /**<  Mask to delete GLONASS SVSTEER.   */
#define QMI_LOC_MASK_DELETE_GLO_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000040) /**<  Mask to delete GLONASS time.  */
#define QMI_LOC_MASK_DELETE_GLO_ALM_CORR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000080) /**<  Mask to delete GLONASS almanac correlation */
#define QMI_LOC_MASK_DELETE_SBAS_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000100) /**<  Mask to delete SBAS SVDIR  */
#define QMI_LOC_MASK_DELETE_SBAS_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000200) /**<  Mask to delete SBAS SVSTEER  */
#define QMI_LOC_MASK_DELETE_POSITION_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000400) /**<  Mask to delete position estimate  */
#define QMI_LOC_MASK_DELETE_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000800) /**<  Mask to delete time estimate  */
#define QMI_LOC_MASK_DELETE_IONO_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00001000) /**<  Mask to delete IONO  */
#define QMI_LOC_MASK_DELETE_UTC_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00002000) /**<  Mask to delete UTC estimate  */
#define QMI_LOC_MASK_DELETE_HEALTH_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00004000) /**<  Mask to delete SV health record  */
#define QMI_LOC_MASK_DELETE_SADATA_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00008000) /**<  Mask to delete SADATA  */
#define QMI_LOC_MASK_DELETE_RTI_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00010000) /**<  Mask to delete RTI  */
#define QMI_LOC_MASK_DELETE_SV_NO_EXIST_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00020000) /**<  Mask to delete SV_NO_EXIST  */
#define QMI_LOC_MASK_DELETE_FREQ_BIAS_EST_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00040000) /**<  Mask to delete frequency bias estimate  */
typedef uint32_t qmiLocDeleteCelldbDataMaskT_v02;
#define QMI_LOC_MASK_DELETE_CELLDB_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000001) /**<  Mask to delete cell database position  */
#define QMI_LOC_MASK_DELETE_CELLDB_LATEST_GPS_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000002) /**<  Mask to delete cell database latest GPS position  */
#define QMI_LOC_MASK_DELETE_CELLDB_OTA_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000004) /**<  Mask to delete cell database OTA position  */
#define QMI_LOC_MASK_DELETE_CELLDB_EXT_REF_POS_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000008) /**<  Mask to delete cell database external reference position  */
#define QMI_LOC_MASK_DELETE_CELLDB_TIMETAG_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000010) /**<  Mask to delete cell database time tag  */
#define QMI_LOC_MASK_DELETE_CELLDB_CELLID_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000020) /**<  Mask to delete cell database cell ID  */
#define QMI_LOC_MASK_DELETE_CELLDB_CACHED_CELLID_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000040) /**<  Mask to delete cell database cached cell ID  */
#define QMI_LOC_MASK_DELETE_CELLDB_LAST_SRV_CELL_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000080) /**<  Mask to delete cell database last service cell  */
#define QMI_LOC_MASK_DELETE_CELLDB_CUR_SRV_CELL_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000100) /**<  Mask to delete cell database current service cell  */
#define QMI_LOC_MASK_DELETE_CELLDB_NEIGHBOR_INFO_V02 ((qmiLocDeleteCelldbDataMaskT_v02)0x00000200) /**<  Mask to delete cell database neighbor information  */
typedef uint32_t qmiLocDeleteClockInfoMaskT_v02;
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000001) /**<  Mask to delete time estimate from clock information                 */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_FREQ_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000002) /**<  Mask to delete frequency estimate from clock information                */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_WEEK_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000004) /**<  Mask to delete week number from clock information            */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_RTC_TIME_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000008) /**<  Mask to delete RTC time from clock information               */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_TIME_TRANSFER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000010) /**<  Mask to delete time transfer from clock information        */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GPSTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000020) /**<  Mask to delete GPS time estimate from clock information         */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLOTIME_EST_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000040) /**<  Mask to delete GLONASS time estimate from clock information   */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLODAY_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000080) /**<  Mask to delete GLONASS day number from clock information          */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO4YEAR_NUMBER_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000100) /**<  Mask to delete GLONASS four year number from clock information   */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000200) /**<  Mask to delete GLONASS RF GRP delay from clock information     */
#define QMI_LOC_MASK_DELETE_CLOCK_INFO_DISABLE_TT_V02 ((qmiLocDeleteClockInfoMaskT_v02)0x00000400) /**<  Mask to delete disable TT from clock information   */
typedef uint8_t qmiLocDeleteSvInfoMaskT_v02;
#define QMI_LOC_MASK_DELETE_EPHEMERIS_V02 ((qmiLocDeleteSvInfoMaskT_v02)0x01) /**<  Delete ephemeris for the satellite  */
#define QMI_LOC_MASK_DELETE_ALMANAC_V02 ((qmiLocDeleteSvInfoMaskT_v02)0x02) /**<  Delete almanac for the satellite  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t gnssSvId;
  /**<   SV ID of the satellite whose data is to be deleted. \n
      - Type: Unsigned integer \n
      - Range: \n
          - For GPS:     1 to 32 \n
          - For SBAS:    33 to 64 \n
          - For GLONASS: 65 to 96    */

  qmiLocSvSystemEnumT_v02 system;
  /**<   Indicates to which constellation this SV belongs.
         Valid values: \n
           - 0x00000001 -- eQMI_LOC_SV_SYSTEM_GPS \n
           - 0x00000002 -- eQMI_LOC_SV_SYSTEM_GALILEO \n
           - 0x00000003 -- eQMI_LOC_SV_SYSTEM_SBAS \n
           - 0x00000004 -- eQMI_LOC_SV_SYSTEM_COMPASS \n
           - 0x00000005 -- eQMI_LOC_SV_SYSTEM_GLONASS
     */

  qmiLocDeleteSvInfoMaskT_v02 deleteSvInfoMask;
  /**<   Indicates if the ephemeris or almanac for a satellite
       is to be deleted. \n
       Valid values: \n
       - 0x01 -- DELETE_EPHEMERIS \n
       - 0x02 -- DELETE_ALMANAC
             */
}qmiLocDeleteSvInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; This command is used to delete the location engine
                    assistance data  */
typedef struct {

  /* Mandatory */
  /*  Delete All */
  uint8_t deleteAllFlag;
  /**<   Whether all assistance data is to be deleted.
       Valid values: \n
       - 0x01 (TRUE)  -- All assistance data is to be deleted;
                         if this flag is set, all the other information
                         contained in the optional fields for this
                         message are ignored \n
       - 0x00 (FALSE) -- The optional fields in the message are to be used
                         to determine which data is to be deleted  */

  /* Optional */
  /*  Delete SV Info */
  uint8_t deleteSvInfoList_valid;  /**< Must be set to true if deleteSvInfoList is being passed */
  uint32_t deleteSvInfoList_len;  /**< Must be set to # of elements in deleteSvInfoList */
  qmiLocDeleteSvInfoStructT_v02 deleteSvInfoList[QMI_LOC_DELETE_MAX_SV_INFO_LENGTH_V02];
  /**<   \n List of satellites for which the assitance data is to be deleted.
    */

  /* Optional */
  /*  Delete GNSS Data */
  uint8_t deleteGnssDataMask_valid;  /**< Must be set to true if deleteGnssDataMask is being passed */
  qmiLocDeleteGnssDataMaskT_v02 deleteGnssDataMask;
  /**<   Mask for the GNSS data that is to be deleted.
       Valid values: \n
       - 0x00000001 -- DELETE_GPS_SVDIR \n
       - 0x00000002 -- DELETE_GPS_SVSTEER \n
       - 0x00000004 -- DELETE_GPS_TIME\n
       - 0x00000008 -- DELETE_GPS_ALM_CORR \n
       - 0x00000010 -- DELETE_GLO_SVDIR \n
       - 0x00000020 -- DELETE_GLO_SVSTEER \n
       - 0x00000040 -- DELETE_GLO_TIME \n
       - 0x00000080 -- DELETE_GLO_ALM_CORR \n
       - 0x00000100 -- DELETE_SBAS_SVDIR \n
       - 0x00000200 -- DELETE_SBAS_SVSTEER \n
       - 0x00000400 -- DELETE_POSITION \n
       - 0x00000800 -- DELETE_TIME \n
       - 0x00001000 -- DELETE_IONO \n
       - 0x00002000 -- DELETE_UTC \n
       - 0x00004000 -- DELETE_HEALTH \n
       - 0x00008000 -- DELETE_SADATA \n
       - 0x00010000 -- DELETE_RTI \n
       - 0x00020000 -- DELETE_SV_NO_EXIST \n
       - 0x00040000 -- DELETE_FREQ_BIAS_EST
   */

  /* Optional */
  /*  Delete Cell Database */
  uint8_t deleteCellDbDataMask_valid;  /**< Must be set to true if deleteCellDbDataMask is being passed */
  qmiLocDeleteCelldbDataMaskT_v02 deleteCellDbDataMask;
  /**<   Mask for the cell database assistance data that is to be deleted. \n
       Valid values: \n
       - 0x00000001 -- DELETE_CELLDB_POS \n
       - 0x00000002 -- DELETE_CELLDB_LATEST_GPS_POS \n
       - 0x00000004 -- DELETE_CELLDB_OTA_POS \n
       - 0x00000008 -- DELETE_CELLDB_EXT_REF_POS \n
       - 0x00000010 -- DELETE_CELLDB_TIMETAG \n
       - 0x00000020 -- DELETE_CELLDB_CELLID \n
       - 0x00000040 -- DELETE_CELLDB_CACHED_CELLID \n
       - 0x00000080 -- DELETE_CELLDB_LAST_SRV_CELL \n
       - 0x00000100 -- DELETE_CELLDB_CUR_SRV_CELL \n
       - 0x00000200 -- DELETE_CELLDB_NEIGHBOR_INFO
   */

  /* Optional */
  /*  Delete Clock Info */
  uint8_t deleteClockInfoMask_valid;  /**< Must be set to true if deleteClockInfoMask is being passed */
  qmiLocDeleteClockInfoMaskT_v02 deleteClockInfoMask;
  /**<   Mask for the clock information assistance data that is to be deleted. \n
       Valid values: \n
       - 0x00000001 -- DELETE_CLOCK_INFO_TIME_EST \n
       - 0x00000002 -- DELETE_CLOCK_INFO_FREQ_EST \n
       - 0x00000004 -- DELETE_CLOCK_INFO_WEEK_NUMBER \n
       - 0x00000008 -- DELETE_CLOCK_INFO_RTC_TIME \n
       - 0x00000010 -- DELETE_CLOCK_INFO_TIME_TRANSFER \n
       - 0x00000020 -- DELETE_CLOCK_INFO_GPSTIME_EST \n
       - 0x00000040 -- DELETE_CLOCK_INFO_GLOTIME_EST \n
       - 0x00000080 -- DELETE_CLOCK_INFO_GLODAY_NUMBER \n
       - 0x00000100 -- DELETE_CLOCK_INFO_GLO4YEAR_NUMBER \n
       - 0x00000200 -- DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY \n
       - 0x00000400 -- DELETE_CLOCK_INFO_DISABLE_TT
   */
}qmiLocDeleteAssistDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; This command is used to delete the location engine
                    assistance data  */
typedef struct {

  /* Mandatory */
  /*  Delete Assist Data Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Delete Assist Data request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocDeleteAssistDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Enables/disables XTRA-T session control. */
typedef struct {

  /* Mandatory */
  /*  Enable XTRA-T */
  uint8_t xtraTSessionControl;
  /**<   Whether to enable XTRA-T:\n
       - 0x01 (TRUE) -- Enable XTRA-T \n
       - 0x00 (FALSE) -- Disable XTRA-T  */
}qmiLocSetXtraTSessionControlReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Enables/disables XTRA-T session control. */
typedef struct {

  /* Mandatory */
  /*  Set XTRA-T Session Control Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set XTRA-T Session Control request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetXtraTSessionControlIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetXtraTSessionControlReqMsgT is empty
 * typedef struct {
 * }qmiLocGetXtraTSessionControlReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the XTRA-T session control value from the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Get XTRA-T Session Control Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get XTRA-T Session Control request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Enable/Disable XTRA-T */
  uint8_t xtraTSessionControl_valid;  /**< Must be set to true if xtraTSessionControl is being passed */
  uint8_t xtraTSessionControl;
  /**<   Whether to enable XTRA-T:\n
       - 0x01 (TRUE) -- Enable XTRA-T \n
       - 0x00 (FALSE) -- Disable XTRA-T  */
}qmiLocGetXtraTSessionControlIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t wifiPositionTime;
  /**<   Common counter (typically, the number of milliseconds since bootup).
        This field is only to be provided if the modem and host processors are
        synchronized. \n
        - Type: Unsigned integer  */
}qmiLocWifiFixTimeStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCWIFIFIXERRORCODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_WIFI_FIX_ERROR_SUCCESS_V02 = 0, /**<  WiFi fix is successful. */
  eQMI_LOC_WIFI_FIX_ERROR_WIFI_NOT_AVAILABLE_V02 = 1, /**<  WiFi fix failed because WiFi is not available on the device.  */
  eQMI_LOC_WIFI_FIX_ERROR_NO_AP_FOUND_V02 = 2, /**<  WiFi fix failed because no access points were found.  */
  eQMI_LOC_WIFI_FIX_ERROR_UNAUTHORIZED_V02 = 3, /**<  WiFi fix failed because the server denied access due to bad authorization
   code.  */
  eQMI_LOC_WIFI_FIX_ERROR_SERVER_UNAVAILABLE_V02 = 4, /**<  WiFi fix failed because the WiFi server was unavailable.  */
  eQMI_LOC_WIFI_FIX_ERROR_LOCATION_CANNOT_BE_DETERMINED_V02 = 5, /**<  WiFi fix failed even though APs were found and the server could be reached.
   This may be because the APs found are not in the database.
 WiFi fix failed, but the cause could not be determined.  */
  eQMI_LOC_WIFI_FIX_ERROR_UNKNOWN_V02 = 6,
  QMILOCWIFIFIXERRORCODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocWifiFixErrorCodeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  double lat;
  /**<   WiFi position latitude. \n
        - Type: Floating point \n
        - Units: Degrees  */

  double lon;
  /**<   WiFi position longitude. \n
        - Type: Floating point \n
        - Units: Degrees  */

  uint16_t hepe;
  /**<   WiFi position HEPE.\n
        - Type: Unsigned integer \n
        - Units: Meters  */

  uint8_t numApsUsed;
  /**<   Number of Access Points (AP) used to generate a fix. \n
        - Type: Unsigned integer  */

  qmiLocWifiFixErrorCodeEnumT_v02 fixErrorCode;
  /**<   WiFi position error code; set to 0 if the fix succeeds. This position
        is only used by a module if the value is 0. If there was a failure,
        the error code provided by the WiFi positioning system can be provided
        here.

        Valid values: \n
          - 0x00000000 -- ERROR_SUCCESS \n
          - 0x00000001 -- ERROR_WIFI_NOT_AVAILABLE \n
          - 0x00000002 -- ERROR_NO_AP_FOUND \n
          - 0x00000003 -- ERROR_UNAUTHORIZED \n
          - 0x00000004 -- ERROR_SERVER_UNAVAILABLE \n
          - 0x00000005 -- ERROR_LOCATION_CANNOT_BE_DETERMINED \n
          - 0x00000006 -- ERROR_UNKNOWN
    */
}qmiLocWifiFixPosStructT_v02;  /* Type */
/**
    @}
  */

typedef uint8_t qmiLocWifiApQualifierMaskT_v02;
#define QMI_LOC_WIFI_AP_QUALIFIER_BEING_USED_V02 ((qmiLocWifiApQualifierMaskT_v02)0x01) /**<  Access point is being used by the WPS.  */
#define QMI_LOC_WIFI_AP_QUALIFIER_HIDDEN_SSID_V02 ((qmiLocWifiApQualifierMaskT_v02)0x02) /**<  AP does not broadcast SSID.  */
#define QMI_LOC_WIFI_AP_QUALIFIER_PRIVATE_V02 ((qmiLocWifiApQualifierMaskT_v02)0x04) /**<  AP has encryption turned on.  */
#define QMI_LOC_WIFI_AP_QUALIFIER_INFRASTRUCTURE_MODE_V02 ((qmiLocWifiApQualifierMaskT_v02)0x08) /**<  AP is in infrastructure mode and not in ad-hoc/unknown mode.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint8_t macAddr[QMI_LOC_WIFI_MAC_ADDR_LENGTH_V02];
  /**<   Associated MAC address of the AP. \n
        - Type: Array of unsigned integers \n
        - Address length: 6
    */

  int32_t rssi;
  /**<   Receive signal strength indicator.\n
        - Type: Signed integer \n
        - Units: dBm (offset with +100 dB)  */

  uint16_t channel;
  /**<   WiFi channel on which a beacon was received. \n
        - Type: Unsigned integer  */

  qmiLocWifiApQualifierMaskT_v02 apQualifier;
  /**<   A bitmask of Boolean qualifiers for APs.
        All unused bits in this mask must be set to 0. \n
        Valid values: \n
          - 0x01 -- BEING_USED \n
          - 0x02 -- HIDDEN_SSID \n
          - 0x04 -- PRIVATE \n
          - 0x08 -- INFRASTRUCTURE_MODE
          */
}qmiLocWifiApInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects the WiFi position. */
typedef struct {

  /* Optional */
  /*  WiFi Fix Time */
  uint8_t wifiFixTime_valid;  /**< Must be set to true if wifiFixTime is being passed */
  qmiLocWifiFixTimeStructT_v02 wifiFixTime;
  /**<   \n Time of WiFi position fix.  */

  /* Optional */
  /*  WiFi Position */
  uint8_t wifiFixPosition_valid;  /**< Must be set to true if wifiFixPosition is being passed */
  qmiLocWifiFixPosStructT_v02 wifiFixPosition;
  /**<   \n WiFi position fix.  */

  /* Optional */
  /*  WiFi Access Point Information */
  uint8_t apInfo_valid;  /**< Must be set to true if apInfo is being passed */
  uint32_t apInfo_len;  /**< Must be set to # of elements in apInfo */
  qmiLocWifiApInfoStructT_v02 apInfo[QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02];
  /**<   \n AP scan list.  */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horizontalReliability_valid;  /**< Must be set to true if horizontalReliability is being passed */
  qmiLocReliabilityEnumT_v02 horizontalReliability;
  /**<   Specifies the reliability of the horizontal position.

        Valid values: \n
          - 0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET \n
          - 0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW \n
          - 0x00000002 -- eQMI_LOC_RELIABILITY_LOW \n
          - 0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM \n
          - 0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
       */
}qmiLocInjectWifiPositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects the WiFi position. */
typedef struct {

  /* Mandatory */
  /*  Inject WiFi Position Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject WiFi Position request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocInjectWifiPositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCWIFISTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_WIFI_STATUS_AVAILABLE_V02 = 1, /**<  WiFi is available.
 WiFi is not available.  */
  eQMI_LOC_WIFI_STATUS_UNAVAILABLE_V02 = 2,
  QMILOCWIFISTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocWifiStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Notifies the location engine of the WiFi status. */
typedef struct {

  /* Mandatory */
  /*  Availablility of WiFi */
  qmiLocWifiStatusEnumT_v02 wifiStatus;
  /**<   WiFi status information.

        Valid values: \n
          - 0x00000001 -- WIFI_STATUS_AVAILABLE \n
          - 0x00000002 -- WIFI_STATUS_UNAVAILABLE
   */
}qmiLocNotifyWifiStatusReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Notifies the location engine of the WiFi status. */
typedef struct {

  /* Mandatory */
  /*  Status of Notify WiFi Status Request */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Notify WiFi Status request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocNotifyWifiStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetRegisteredEventsReqMsgT is empty
 * typedef struct {
 * }qmiLocGetRegisteredEventsReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the mask of the events for which a client has
                    registered. */
typedef struct {

  /* Mandatory */
  /*  Get Registered Events Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Registered Events request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Event Registration Mask */
  uint8_t eventRegMask_valid;  /**< Must be set to true if eventRegMask is being passed */
  qmiLocEventRegMaskT_v02 eventRegMask;
  /**<   Event registration mask.

       Valid bitmasks: \n
         - 0x00000001 -- POSITION_REPORT \n
         - 0x00000002 -- GNSS_SV_INFO \n
         - 0x00000004 -- NMEA \n
         - 0x00000008 -- NI_NOTIFY_VERIFY_REQ \n
         - 0x00000010 -- INJECT_TIME_REQ \n
         - 0x00000020 -- INJECT_PREDICTED_ORBITS_REQ \n
         - 0x00000040 -- INJECT_POSITION_REQ \n
         - 0x00000080 -- ENGINE_STATE \n
         - 0x00000100 -- FIX_SESSION_STATE \n
         - 0x00000200 -- WIFI_REQ \n
         - 0x00000400 -- SENSOR_STREAMING_READY_STATUS \n
         - 0x00000800 -- TIME_SYNC_REQ \n
         - 0x00001000 -- SET_SPI_STREAMING_REPORT \n
         - 0x00002000 -- LOCATION_SERVER__CONNECTION_REQ
             */
}qmiLocGetRegisteredEventsIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCOPERATIONMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_OPER_MODE_DEFAULT_V02 = 1, /**<  Use the default engine mode.  */
  eQMI_LOC_OPER_MODE_MSB_V02 = 2, /**<  Use the MS-based mode.  */
  eQMI_LOC_OPER_MODE_MSA_V02 = 3, /**<  Use the MS-assisted mode.  */
  eQMI_LOC_OPER_MODE_STANDALONE_V02 = 4, /**<  Use Standalone mode.
 Use cell ID. For 1x, this mode corresponds to
       AFLT.  */
  eQMI_LOC_OPER_MODE_CELL_ID_V02 = 5,
  QMILOCOPERATIONMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocOperationModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Tells the engine to use the specified operation mode while
                    making the position fixes. This command is not to be used
                    by multiple clients concurrently. */
typedef struct {

  /* Mandatory */
  /*  Operation Mode */
  qmiLocOperationModeEnumT_v02 operationMode;
  /**<   Preferred operation mode.

       Valid values: \n
         - 0x00000001 -- OPER_MODE_DEFAULT \n
         - 0x00000002 -- OPER_MODE_MSB \n
         - 0x00000003 -- OPER_MODE_MSA \n
         - 0x00000004 -- OPER_MODE_STANDALONE \n
         - 0x00000005 -- OPER_MODE_CELL_ID
   */
}qmiLocSetOperationModeReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Tells the engine to use the specified operation mode while
                    making the position fixes. This command is not to be used
                    by multiple clients concurrently. */
typedef struct {

  /* Mandatory */
  /*  Set Operation Mode Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Operation Mode request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetOperationModeIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetOperationModeReqMsgT is empty
 * typedef struct {
 * }qmiLocGetOperationModeReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Gets the current operation mode from the engine. */
typedef struct {

  /* Mandatory */
  /*  Get Operation Mode Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Operation Mode request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Operation Mode */
  uint8_t operationMode_valid;  /**< Must be set to true if operationMode is being passed */
  qmiLocOperationModeEnumT_v02 operationMode;
  /**<   Current operation mode.

       Valid values: \n
         - 0x00000001 -- OPER_MODE_DEFAULT \n
         - 0x00000002 -- OPER_MODE_MSB \n
         - 0x00000003 -- OPER_MODE_MSA \n
         - 0x00000004 -- OPER_MODE_STANDALONE \n
         - 0x00000005 -- OPER_MODE_CELL_ID
   */
}qmiLocGetOperationModeIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to set the SPI status, which
                    indicates whether the device is stationary. */
typedef struct {

  /* Mandatory */
  /*  Stationary Status */
  uint8_t stationary;
  /**<   Whether the device is stationary:\n
       - 0x00 (FALSE) -- Device is not stationary \n
       - 0x01 (TRUE)  -- Device is stationary  */

  /* Optional */
  /*  Confidence */
  uint8_t confidenceStationary_valid;  /**< Must be set to true if confidenceStationary is being passed */
  uint8_t confidenceStationary;
  /**<   Confidence in the Stationary state expressed as a percentage.\n
       - Type: Unsigned integer \n
       - Range: 0 to 100  */
}qmiLocSetSpiStatusReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to set the SPI status, which
                    indicates whether the device is stationary. */
typedef struct {

  /* Mandatory */
  /*  Status of SPI Status Request */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the SPI Status request. \n
       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetSpiStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint8_t qmiLocSensorDataFlagMaskT_v02;
#define QMI_LOC_SENSOR_DATA_FLAG_SIGN_REVERSAL_V02 ((qmiLocSensorDataFlagMaskT_v02)0x01) /**<  Bitmask to specify that a sign reversal is required while interpreting
     the sensor data.  */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t timeOffset;
  /**<   Sample time offset. This time offset must be
       relative to the sensor time of the first sample.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  float xAxis;
  /**<   Sensor x-axis sample. \n
       - Type: Floating point \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyrometer:     ( (rads)/(seconds^2) )  */

  float yAxis;
  /**<   Sensor y-axis sample. \n
       - Type: Floating point \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyrometer:     ( (rads)/(seconds^2) )  */

  float zAxis;
  /**<   Sensor z-axis sample. \n
       - Type: Floating point \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyrometer:     ( (rads)/(seconds^2) )  */
}qmiLoc3AxisSensorSampleStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t timeOfFirstSample;
  /**<   Denotes a full 32-bit time tag of the first (oldest) sample in this
       message. \n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  qmiLocSensorDataFlagMaskT_v02 flags;
  /**<   Flags to indicate any deviation from the default measurement
       assumptions. All unused bits in this field must be set to 0.

       Valid bitmasks: \n
         - 0x01 -- SIGN REVERSAL
         */

  uint32_t sensorData_len;  /**< Must be set to # of elements in sensorData */
  qmiLoc3AxisSensorSampleStructT_v02 sensorData[QMI_LOC_SENSOR_DATA_MAX_SAMPLES_V02];
  /**<   Variable length array to specify sensor samples. \n
       - Maximum length of the array: 50                              */
}qmiLoc3AxisSensorSampleListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to inject sensor data into the
                    GNSS location engine. */
typedef struct {

  /* Optional */
  /*  Opaque Identifier */
  uint8_t opaqueIdentifier_valid;  /**< Must be set to true if opaqueIdentifier is being passed */
  uint32_t opaqueIdentifier;
  /**<   An opaque identifier that is sent in by the client that will be echoed
       in the indication so the client can relate the indication to the
       request. \n
       - Type: Unsigned integer  */

  /* Optional */
  /*  3-Axis Accelerometer Data */
  uint8_t threeAxisAccelData_valid;  /**< Must be set to true if threeAxisAccelData is being passed */
  qmiLoc3AxisSensorSampleListStructT_v02 threeAxisAccelData;
  /**<   \n Accelerometer sensor samples.  */

  /* Optional */
  /*  3-Axis Gyrometer Data */
  uint8_t threeAxisGyroData_valid;  /**< Must be set to true if threeAxisGyroData is being passed */
  qmiLoc3AxisSensorSampleListStructT_v02 threeAxisGyroData;
  /**<   \n Gyrometer sensor samples.  */
}qmiLocInjectSensorDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to inject sensor data into the
                    GNSS location engine. */
typedef struct {

  /* Mandatory */
  /*  Inject Sensor Data Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject Sensor Data request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Opaque Identifier */
  uint8_t opaqueIdentifier_valid;  /**< Must be set to true if opaqueIdentifier is being passed */
  uint32_t opaqueIdentifier;
  /**<   An opaque identifier that was sent in by the client echoed
       so the client can relate the indication to the request. \n
       - Type: Unsigned integer  */

  /* Optional */
  /*  Accelerometer Samples Accepted */
  uint8_t threeAxisAccelSamplesAccepted_valid;  /**< Must be set to true if threeAxisAccelSamplesAccepted is being passed */
  uint8_t threeAxisAccelSamplesAccepted;
  /**<   This field lets the client know how many 3-axis accelerometer samples
       were accepted.  */

  /* Optional */
  /*  Gyrometer Samples Accepted */
  uint8_t threeAxisGyroSamplesAccepted_valid;  /**< Must be set to true if threeAxisGyroSamplesAccepted is being passed */
  uint8_t threeAxisGyroSamplesAccepted;
  /**<   This field lets the client know how many 3-axis gyrometer samples were
       accepted.  */
}qmiLocInjectSensorDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to inject time sync data. */
typedef struct {

  /* Mandatory */
  /*  Reference Time Sync Counter */
  uint32_t refCounter;
  /**<   Must be set to the value that was sent to the control point when the
       GNSS location engine requested time sync injection. \n
       - Type: Unsigned integer  */

  /* Mandatory */
  /*  Sensor Receive Time */
  uint32_t sensorProcRxTime;
  /**<   The value of the sensor time when the control point received the
       Time Sync Inject request from the GNSS location engine.

       Must be monotonically increasing, jitter @latexonly $\leq$ @endlatexonly 1
       millisecond, never stopping until the process is rebooted.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */

  /* Mandatory */
  /*  Sensor Transmit Time */
  uint32_t sensorProcTxTime;
  /**<   The value of the sensor time when the control point injects this message
       for use by the GNSS location engine.

       Must be monotonically increasing, jitter @latexonly $\leq$ @endlatexonly 1
       millisecond, never stopping until the process is rebooted.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds  */
}qmiLocInjectTimeSyncDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to inject time sync data. */
typedef struct {

  /* Mandatory */
  /*  Inject Time Sync Data Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject Time Sync Data request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocInjectTimeSyncDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCCRADLEMOUNTSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_CRADLE_STATE_NOT_MOUNTED_V02 = 0, /**<  Device is mounted on the cradle */
  eQMI_LOC_CRADLE_STATE_MOUNTED_V02 = 1, /**<  Device is not mounted on the cradle
 Unknown cradle mount state */
  eQMI_LOC_CRADLE_STATE_UNKNOWN_V02 = 2,
  QMILOCCRADLEMOUNTSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocCradleMountStateEnumT_v02;
/**
    @}
  */

/*
 * qmiLocGetCradleMountConfigReqMsgT is empty
 * typedef struct {
 * }qmiLocGetCradleMountConfigReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to get the current
                    cradle mount configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Cradle Mount Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Cradle Mount Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Cradle Mount State */
  uint8_t cradleMountState_valid;  /**< Must be set to true if cradleMountState is being passed */
  qmiLocCradleMountStateEnumT_v02 cradleMountState;
  /**<   Cradle Mount state set by the control point.

       Valid values: \n
         - 0x00000000 -- CRADLE_STATE_NOT_MOUNTED \n
         - 0x00000001 -- CRADLE_STATE_MOUNTED \n
         - 0x00000002 -- CRADLE_STATE_UNKNOWN
          */

  /* Optional */
  /*  Cradle Mount Confidence */
  uint8_t confidenceCradleMountState_valid;  /**< Must be set to true if confidenceCradleMountState is being passed */
  uint8_t confidenceCradleMountState;
  /**<   Confidence of the Cradle Mount state expressed as a percentage.\n
       - Type: Unsigned integer \n
       - Range: 0 to 100  */
}qmiLocGetCradleMountConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to set the current
                    cradle mount configuration. */
typedef struct {

  /* Mandatory */
  /*  Cradle Mount State */
  qmiLocCradleMountStateEnumT_v02 cradleMountState;
  /**<   Cradle Mount state set by the control point.

       Valid values: \n
         - 0x00000000 -- CRADLE_STATE_NOT_MOUNTED \n
         - 0x00000001 -- CRADLE_STATE_MOUNTED \n
         - 0x00000002 -- CRADLE_STATE_UNKNOWN          */

  /* Optional */
  /*  Cradle Mount Confidence */
  uint8_t confidenceCradleMountState_valid;  /**< Must be set to true if confidenceCradleMountState is being passed */
  uint8_t confidenceCradleMountState;
  /**<   Confidence in the Cradle Mount state expressed as a percentage.\n
       - Type: Unsigned integer \n
       - Range: 0 to 100  */
}qmiLocSetCradleMountConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to set the current
                    cradle mount configuration. */
typedef struct {

  /* Mandatory */
  /*  Set Cradle Mount Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Cradle Mount Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetCradleMountConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCEXTERNALPOWERCONFIGENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_EXTERNAL_POWER_NOT_CONNECTED_V02 = 0, /**<  Device is not connected to an external power source.  */
  eQMI_LOC_EXTERNAL_POWER_CONNECTED_V02 = 1, /**<  Device is connected to an external power source.
 Unknown external power state.  */
  eQMI_LOC_EXTERNAL_POWER_UNKNOWN_V02 = 2,
  QMILOCEXTERNALPOWERCONFIGENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocExternalPowerConfigEnumT_v02;
/**
    @}
  */

/*
 * qmiLocGetExternalPowerConfigReqMsgT is empty
 * typedef struct {
 * }qmiLocGetExternalPowerConfigReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to get the current
                    external power configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Ext Power Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get External Power Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  External Power State */
  uint8_t externalPowerState_valid;  /**< Must be set to true if externalPowerState is being passed */
  qmiLocExternalPowerConfigEnumT_v02 externalPowerState;
  /**<   Power state; injected by the control point.

       Valid values: \n
         - 0x00000000 -- EXTERNAL_POWER_NOT_CONNECTED \n
         - 0x00000001 -- EXTERNAL_POWER_CONNECTED \n
         - 0x00000002 -- EXTERNAL_POWER_UNKNOWN
     */
}qmiLocGetExternalPowerConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to set the current
                    external power configuration. */
typedef struct {

  /* Mandatory */
  /*  External Power State */
  qmiLocExternalPowerConfigEnumT_v02 externalPowerState;
  /**<   Power state; injected by the control point.

       Valid values: \n
         - 0x00000000 -- EXTERNAL_POWER_NOT_CONNECTED \n
         - 0x00000001 -- EXTERNAL_POWER_CONNECTED \n
         - 0x00000002 -- EXTERNAL_POWER_UNKNOWN
     */
}qmiLocSetExternalPowerConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to set the current
                    external power configuration. */
typedef struct {

  /* Mandatory */
  /*  Set Ext Power Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set External Power Configuration request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetExternalPowerConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSERVERPDNENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4_V02 = 0x01, /**<  IPV4 PDN type.  */
  eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV6_V02 = 0x02, /**<  IPV6 PDN type.  */
  eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4V6_V02 = 0x03, /**<  IPV4V6 PDN type.
 PPP PDN type.  */
  eQMI_LOC_APN_PROFILE_PDN_TYPE_PPP_V02 = 0x04,
  QMILOCSERVERPDNENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocServerPDNEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocServerPDNEnumT_v02 pdnType;
  /**<   PDN type of the Access Point Name (APN) profile.

       Valid values: \n
         - 0x00000001 -- PDN_TYPE_IPV4 \n
         - 0x00000002 -- PDN_TYPE_IPV6 \n
         - 0x00000003 -- PDN_TYPE_IPV4V6 \n
         - 0x00000004 -- PDN_TYPE_PPP
   */

  char apnName[QMI_LOC_MAX_APN_NAME_LENGTH_V02 + 1];
  /**<   APN name. \n
       - Type: NULL-terminated string \n
       - Maximum string length (including NULL terminator): 101
   */
}qmiLocApnProfilesStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSERVERREQSTATUSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02 = 1, /**<  Location server request was successful.
 Location server request failed. */
  eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02 = 2,
  QMILOCSERVERREQSTATUSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocServerReqStatusEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to inform the service about the
                    status of the location server connection request that the
                    service may have sent via the
                    QMI_LOC_EVENT_LOCATION_SERVER_REQ_IND event. */
typedef struct {

  /* Mandatory */
  /*  Connection Handle */
  uint32_t connHandle;
  /**<   Connection handle that the service specified in the
       Location Server Connection request event. \n
       - Type: Unsigned integer  */

  /* Mandatory */
  /*  Request Type */
  qmiLocServerRequestEnumT_v02 requestType;
  /**<   Type of connection request service that was specified in the
       Location Server Connection Request event.

       Valid values: \n
         - 0x00000001 -- OPEN \n
         - 0x00000002 -- CLOSE
   */

  /* Mandatory */
  /*  Connection Status */
  qmiLocServerReqStatusEnumT_v02 statusType;
  /**<   Status of the Connection request.

       Valid values: \n
         - 0x00000001 -- STATUS_SUCCESS = 1 \n
         - 0x00000002 -- STATUS_FAILURE = 2

   */

  /* Optional */
  /*  APN Profile */
  uint8_t apnProfile_valid;  /**< Must be set to true if apnProfile is being passed */
  qmiLocApnProfilesStructT_v02 apnProfile;
  /**<   \n APN profile information is present only when requestType
       is OPEN and statusType is SUCCESS.  */
}qmiLocInformLocationServerConnStatusReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to inform the service about the
                    status of the location server connection request that the
                    service may have sent via the
                    QMI_LOC_EVENT_LOCATION_SERVER_REQ_IND event. */
typedef struct {

  /* Mandatory */
  /*  Status of Inform Loc Server Conn Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inform Location Server Connection Status request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocInformLocationServerConnStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCVXVERSIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_VX_VERSION_V1_ONLY_V02 = 1, /**<  V1 VX version.
 V2 VX version.  */
  eQMI_LOC_VX_VERSION_V2_ONLY_V02 = 2,
  QMILOCVXVERSIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocVxVersionEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSUPLVERSIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUPL_VERSION_1_0_V02 = 1, /**<  SUPL version 1.0.
 SUPL version 2.0.  */
  eQMI_LOC_SUPL_VERSION_2_0_V02 = 2,
  QMILOCSUPLVERSIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSuplVersionEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to configure parameters stored
                    in the nonvolatile memory. */
typedef struct {

  /* Optional */
  /*  SUPL Security */
  uint8_t suplSecurity_valid;  /**< Must be set to true if suplSecurity is being passed */
  uint8_t suplSecurity;
  /**<   Indicates whether SUPL security is enabled. \n
       - 0x01 (TRUE) -- SUPL security is enabled \n
       - 0x00 (FALSE) -- SUPL security is disabled  */

  /* Optional */
  /*  VX Version */
  uint8_t vxVersion_valid;  /**< Must be set to true if vxVersion is being passed */
  qmiLocVxVersionEnumT_v02 vxVersion;
  /**<   VX version.

       Valid values: \n
         - 0x00000001 -- VX_VERSION_V1_ONLY \n
         - 0x00000002 -- VX_VERSION_V2_ONLY
   */

  /* Optional */
  /*  SUPL Version */
  uint8_t suplVersion_valid;  /**< Must be set to true if suplVersion is being passed */
  qmiLocSuplVersionEnumT_v02 suplVersion;
  /**<   SUPL version.

        Valid values: \n
         - 0x00000001 -- SUPL_VERSION_1_0 \n
         - 0x00000002 -- SUPL_VERSION_2_0
   */
}qmiLocSetProtocolConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocProtocolConfigParamMaskT_v02;
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x00000001) /**<  Mask for the SUPL security configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x00000002) /**<  Mask for the VX version configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x00000004) /**<  Mask for the SUPL version configuration parameter.       */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to configure parameters stored
                    in the nonvolatile memory. */
typedef struct {

  /* Mandatory */
  /*  Set Config Params Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Configuration Parameters request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedProtocolConfigParamMask_valid;  /**< Must be set to true if failedProtocolConfigParamMask is being passed */
  qmiLocProtocolConfigParamMaskT_v02 failedProtocolConfigParamMask;
  /**<   This field is sent only if the status is not a success. And if it is
       not successful, this field will identify the parameters that were not
       set successfully.

       Valid bitmasks: \n
         - 0x0000000000000001 -- CONFIG_PARAM_MASK_SUPL_SECURITY \n
         - 0x0000000000000002 -- CONFIG_PARAM_MASK_VX_VERSION \n
         - 0x0000000000000004 -- CONFIG_PARAM_MASK_SUPL_VERSION
   */
}qmiLocSetProtocolConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to get the configuration
                    parameters stored in the nonvolatile memory. */
typedef struct {

  /* Mandatory */
  /*  Config Parameters */
  qmiLocProtocolConfigParamMaskT_v02 getProtocolConfigParamMask;
  /**<   Mask denoting the configuration parameters to be retrieved.

       Valid bitmasks: \n
         - 0x0000000000000001 -- CONFIG_PARAM_MASK_SUPL_SECURITY \n
         - 0x0000000000000002 -- CONFIG_PARAM_MASK_VX_VERSION \n
         - 0x0000000000000004 -- CONFIG_PARAM_MASK_SUPL_VERSION
   */
}qmiLocGetProtocolConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to get the configuration
                    parameters stored in the nonvolatile memory. */
typedef struct {

  /* Mandatory */
  /*  Get Config Params Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Configuration Parameters request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
     */

  /* Optional */
  /*  SUPL Security */
  uint8_t suplSecurity_valid;  /**< Must be set to true if suplSecurity is being passed */
  uint8_t suplSecurity;
  /**<   Indicates whether SUPL security is enabled. \n
       - 0x01 (TRUE) -- SUPL security is enabled \n
       - 0x00 (FALSE) -- SUPL security is disabled  */

  /* Optional */
  /*  VX Version */
  uint8_t vxVersion_valid;  /**< Must be set to true if vxVersion is being passed */
  qmiLocVxVersionEnumT_v02 vxVersion;
  /**<   VX version.

       Valid values: \n
         - 0x00000001 -- VX_VERSION_V1_ONLY \n
         - 0x00000002 -- VX_VERSION_V2_ONLY
   */

  /* Optional */
  /*  SUPL Version */
  uint8_t suplVersion_valid;  /**< Must be set to true if suplVersion is being passed */
  qmiLocSuplVersionEnumT_v02 suplVersion;
  /**<   SUPL version.

        Valid values: \n
         - 0x00000001 -- SUPL_VERSION_1_0 \n
         - 0x00000002 -- SUPL_VERSION_2_0
   */
}qmiLocGetProtocolConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSENSORSCONTROLCONFIGSENSORUSEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SENSOR_CONFIG_SENSOR_USE_ENABLE_V02 = 0, /**<  Sensors data should be requested whenever a position request is
       received. If sensor data are injected, the positioning engine
       attempts to improve the heading and positioning performance using sensors.
       This is the default.

 Inertial sensors are not to be used to aid heading and position
       improvement.  */
  eQMI_LOC_SENSOR_CONFIG_SENSOR_USE_DISABLE_V02 = 1,
  QMILOCSENSORSCONTROLCONFIGSENSORUSEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSensorsControlConfigSensorUseEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the sensor control configuration. */
typedef struct {

  /* Optional */
  /*  Sensors Usage */
  uint8_t sensorsUsage_valid;  /**< Must be set to true if sensorsUsage is being passed */
  qmiLocSensorsControlConfigSensorUseEnumT_v02 sensorsUsage;
  /**<   Controls how sensors are used to aid heading and positioning
       performance.

       Valid values:\n
        - 0x00000000 -- SENSORS USE ENABLED: \n
            Sensors data is to be requested whenever a position request is
            received. If sensors data is injected, the GNSS location engine
            attempts to improve the heading and positioning performance using
            sensors. This is the default. \n
        - 0x00000001 -- SENSORS USE DISABLED: \n
            Inertial sensors are not to be used to aid in heading and
            position improvement.
   */
}qmiLocSetSensorControlConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sets the sensor control configuration. */
typedef struct {

  /* Mandatory */
  /*  Set Sensor Control Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Sensor Control Configuration request.

       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetSensorControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetSensorControlConfigReqMsgT is empty
 * typedef struct {
 * }qmiLocGetSensorControlConfigReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Retrieves the current sensor control configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Sensor Control Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Sensors Control Configuration request.

       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Sensors Usage */
  uint8_t sensorsUsage_valid;  /**< Must be set to true if sensorsUsage is being passed */
  qmiLocSensorsControlConfigSensorUseEnumT_v02 sensorsUsage;
  /**<   Controls how sensors are used to aid the heading and positioning
       performance.

       Valid values:\n
        - 0x00000000 -- SENSORS USE ENABLED: \n
            Sensors data is to be requested whenever a position request is
            received. If sensors data is injected, the GNSS location engine
            attempts to improve the heading and positioning performance using
            sensors. This is the default. \n
        - 0x00000001 -- SENSORS USE DISABLED: \n
            Inertial sensors are not to be used to aid in the heading and
            position improvement.
   */
}qmiLocGetSensorControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the properties specific to the type of sensor used.
                    The control point must set sensor properties before they can be
                    used to aid in heading and positioning performance improvement.
   */
typedef struct {

  /* Optional */
  /*  Gyro Bias Variance */
  uint8_t gyroBiasVarianceRandomWalk_valid;  /**< Must be set to true if gyroBiasVarianceRandomWalk is being passed */
  float gyroBiasVarianceRandomWalk;
  /**<   Specifies the gyro bias random walk parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The gyro bias variance random walk parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: radians^2/second^4

   */
}qmiLocSetSensorPropertiesReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sets the properties specific to the type of sensor used.
                    The control point must set sensor properties before they can be
                    used to aid in heading and positioning performance improvement.
   */
typedef struct {

  /* Mandatory */
  /*  Set Sensor Properties Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Sensor Properties request.

       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */
}qmiLocSetSensorPropertiesIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetSensorPropertiesReqMsgT is empty
 * typedef struct {
 * }qmiLocGetSensorPropertiesReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Retrieves the current sensor properties. */
typedef struct {

  /* Mandatory */
  /*  Get Sensor Properties Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Sensors Properties request.

       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Gyro Bias Variance */
  uint8_t gyroBiasVarianceRandomWalk_valid;  /**< Must be set to true if gyroBiasVarianceRandomWalk is being passed */
  float gyroBiasVarianceRandomWalk;
  /**<   Specifies the gyro bias random walk parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The gyro bias variance random walk parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: radians^2/seconds^4

   */
}qmiLocGetSensorPropertiesIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSENSORPERFORMANCECONTROLMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SENSOR_PERFORMANCE_CONTROL_MODE_AUTO_V02 = 0, /**<  Sensors usage is to be determined by the GNSS location engine.
       This mode can optimize power consumption and give a
       power-balanced positioning and heading enhancement using
       inertial sensors
 Sensors usage is to be forced ON.
       This mode can be requested by the control point when
       power consumption is not a restriction to the use of
       inertial sensors.       */
  eQMI_LOC_SENSOR_PERFORMANCE_CONTROL_MODE_FORCED_V02 = 1,
  QMILOCSENSORPERFORMANCECONTROLMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSensorPerformanceControlModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Provides fine-grained control of sensor based positioning
                    performance */
typedef struct {

  /* Optional */
  /*  Sensor Performance Control Mode */
  uint8_t performanceControlMode_valid;  /**< Must be set to true if performanceControlMode is being passed */
  qmiLocSensorPerformanceControlModeEnumT_v02 performanceControlMode;
  /**<   Controls when sensors data is requested during GNSS fix processing.
       This field is relevant only when sensors have been enabled using the
       sensors control configuration.

       Valid values: \n
       - 0x00000000 -- AUTO: \n
         The GNSS location engine can decide when to request sensor data injection
         based on internal criteria. This is the default. \n
       - 0x00000001 -- FORCED: \n
         The GNSS location engine must request use of sensors every time the GNSS
         location engine turns on.
   */

  /* Optional */
  /*  Accelerometer Sampling Specification */
  uint8_t accelSamplingSpec_valid;  /**< Must be set to true if accelSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       acceleration data. The sensor data rate is specified in terms of the nominal
       number of samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine.
   */

  /* Optional */
  /*  Gyrometer Sampling Specification */
  uint8_t gyroSamplingSpec_valid;  /**< Must be set to true if gyroSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       gyro data. The sensor data rate is specified in terms of the nominal number of
       samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine.
   */
}qmiLocSetSensorPerformanceControlConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocSensorPerformanceControlConfigFailureMaskT_v02;
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_PERFORMANCE_MODE_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000001) /**<  Failed to set the performance mode.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_ACCEL_SAMPLING_SPEC_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000002) /**<  Failed to set the accelerometer sampling specification.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_GYRO_SAMPLING_SPEC_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000004) /**<  Failed to set the gyroscope sampling specification.  */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Provides fine-grained control of sensor based positioning
                    performance */
typedef struct {

  /* Mandatory */
  /*  Set Sensor Perf Control Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Sensor Performance Control Configuration request. \n
       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Failed Configuration */
  uint8_t failedConfiguration_valid;  /**< Must be set to true if failedConfiguration is being passed */
  qmiLocSensorPerformanceControlConfigFailureMaskT_v02 failedConfiguration;
  /**<   This field is sent only if the status is not a success.
       When sent, this field identifies which configuration failed. \n
       Valid bitmasks: \n
       - 0x0000000000000001 -- PERFORMANCE_MODE \n
       - 0x0000000000000002 -- ACCEL_SAMPLING_SPEC \n
       - 0x0000000000000004 -- GYRO_SAMPLING_SPEC
   */
}qmiLocSetSensorPerformanceControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocGetSensorPerformanceControlConfigReqMsgT is empty
 * typedef struct {
 * }qmiLocGetSensorPerformanceControlConfigReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Retrieves the current sensor performance control
                    configuration. */
typedef struct {

  /* Mandatory */
  /*  Get Sensor Perf Control Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Sensor Performance Control Configuration request. \n
       Valid values: \n
          - 0x00000000 -- SUCCESS \n
          - 0x00000001 -- GENERAL_FAILURE \n
          - 0x00000002 -- UNSUPPORTED \n
          - 0x00000003 -- INVALID_PARAMETER \n
          - 0x00000004 -- ENGINE_BUSY \n
          - 0x00000005 -- PHONE_OFFLINE \n
          - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Performance Control Mode */
  uint8_t performanceControlMode_valid;  /**< Must be set to true if performanceControlMode is being passed */
  qmiLocSensorPerformanceControlModeEnumT_v02 performanceControlMode;
  /**<   Controls when sensor data is requested during GNSS fix processing.
       This field is relevant only when sensors have been enabled using the
       sensor control configuration.

       Valid values: \n
       - 0x00000000 -- AUTO: \n
         The GNSS location engine can decide when to request sensor data injection
         based on internal criteria. This is the default. \n
       - 0x0000001 -- FORCED: \n
         The GNSS location engine must request use of the sensors every time the GNSS
         location engine turns on.
   */

  /* Optional */
  /*  Accelerometer Sampling Specification */
  uint8_t accelSamplingSpec_valid;  /**< Must be set to true if accelSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       acceleration data. The sensor data rate is specified in terms of the nominal number of
       samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine.
   */

  /* Optional */
  /*  Gyrometer Sampling Specification */
  uint8_t gyroSamplingSpec_valid;  /**< Must be set to true if gyroSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       gyro data. The sensor data rate is specified in terms of the nominal number of
       samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine.
   */
}qmiLocGetSensorPerformanceControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Inject a SUPL certificate to be used in AGNSS sessions. */
typedef struct {

  /* Mandatory */
  /*  SUPL Certificate Id */
  uint8_t suplCertId;
  /**<   Certificate Id of the Supl Certificate
       Type: Unsigned Integer
       Units: Bytes
       Range: 0-9   */

  /* Mandatory */
  /*  SUPL certificate Data */
  uint32_t suplCertData_len;  /**< Must be set to # of elements in suplCertData */
  uint8_t suplCertData[QMI_LOC_MAX_SUPL_CERT_LENGTH_V02];
  /**<   The supl Certificate contents
       Type: Array of bytes.
        - Maximum certificate size : 2000 bytes  */
}qmiLocInjectSuplCertificateReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Inject a SUPL certificate to be used in AGNSS sessions. */
typedef struct {

  /* Mandatory */
  /*  Supl Certificate Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the inject SUPL certificate request. \n

       Valid Values:
          - 0x00000000 -- SUCCESS\n
          - 0x00000001 -- GENERAL_FAILURE\n
          - 0x00000002 -- UNSUPPORTED\n
          - 0x00000003 -- INVALID_PARAMETER\n
          - 0x00000004 -- ENGINE_BUSY\n
          - 0x00000005 -- PHONE_OFFLINE\n
          - 0x00000006 -- LOC_TIMEOUT\n
   */
}qmiLocInjectSuplCertificateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Delete a SUPL certificate.  */
typedef struct {

  /* Optional */
  /*  SUPL Certificate Id */
  uint8_t suplCertId_valid;  /**< Must be set to true if suplCertId is being passed */
  uint8_t suplCertId;
  /**<   Certificate Id of the Supl Certificate to be deleted
       Type: Unsigned Integer \n
       Units: Bytes \n
       Range: 0-9 \n
       If suplCertId is not specified then
       "all" supl certificates will be deleted.  */
}qmiLocDeleteSuplCertificateReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Delete a SUPL certificate.  */
typedef struct {

  /* Mandatory */
  /*  SUPL certifcate Deletion Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the delete SUPL certificate request. \n

       Valid Values:
          - 0x00000000 -- SUCCESS\n
          - 0x00000001 -- GENERAL_FAILURE\n
          - 0x00000002 -- UNSUPPORTED\n
          - 0x00000003 -- INVALID_PARAMETER\n
          - 0x00000004 -- ENGINE_BUSY\n
          - 0x00000005 -- PHONE_OFFLINE\n
          - 0x00000006 -- LOC_TIMEOUT\n
   */
}qmiLocDeleteSuplCertificateIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPositionEngineConfigParamMaskT_v02;
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000001) /**<  This field denotes if the position engine uses the
       injected position in direct position calculation.  */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000002) /**<  This field denotes if the position engine filters the
       SV usage in the fix.  */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000004) /**<  This field denotes if the position engine stores assistance data
       in the persistent memory.       */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to configure position engine
                    functionality. */
typedef struct {

  /* Optional */
  /*  Injected Position Control */
  uint8_t injectedPositionControl_valid;  /**< Must be set to true if injectedPositionControl is being passed */
  uint8_t injectedPositionControl;
  /**<   Controls how the injected position will be used in position engine.\n
       Valid values: \n
         - 0x01 (TRUE) -- Use the injected position in direct position
                          calculation. \n
         - 0x00 (FALSE) -- Do not use the injected position in direct position
                           calculation. \n
       The default value is TRUE, the injected position is used in direct
       position calculation by default.
   */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Controls if SV usage will be filtered in a position fix.
       Valid values:\n
         - 0x01 (TRUE) -- Filter the usage of SV's in the fix. \n
         - 0x00 (FALSE) -- Do not filter the usage of SV's in the fix. \n
       The default value is FALSE, the SV usage is not filtered in the
       fix by default.
   */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Controls whether assistance data should be stored in
       the persistent memory. \n
       Valid values:\n
         - 0x01 (TRUE) -- Store assistance data in persistent memory. \n
         - 0x00 (FALSE) -- Do not store assistance data in persistent memory. \n
       The default value is TRUE, the  assistance data  is stored in
       persistent memory by default.
   */
}qmiLocSetPositionEngineConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to configure position engine
                    functionality. */
typedef struct {

  /* Mandatory */
  /*  Set Position Engine Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Configuration Parameters request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
   */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedPositionEngineConfigParamMask_valid;  /**< Must be set to true if failedPositionEngineConfigParamMask is being passed */
  qmiLocPositionEngineConfigParamMaskT_v02 failedPositionEngineConfigParamMask;
  /**<   This field will be sent only if the status is not a success.
       Identifies the parameters that were not set successfully.

       Valid bitmasks: \n
         - 0x00000001 -- INJECTED_POSITION_CONTROL \n
         - 0x00000002 -- FILTER_SV_USAGE \n
         - 0x00000004 -- STORE_ASSIST_DATA
   */
}qmiLocSetPositionEngineConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to get the position engine
                    configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Config Parameters */
  qmiLocPositionEngineConfigParamMaskT_v02 getPositionEngineConfigParamMask;
  /**<   Mask denoting the configuration parameters to be retrieved.
       Valid bitmasks: \n
        - 0x00000001 -- INJECTED_POSITION_CONTROL \n
        - 0x00000002 -- FILTER_SV_USAGE \n
        - 0x00000004 -- STORE_ASSIST_DATA
   */
}qmiLocGetPositionEngineConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to get the position engine
                    configuration parameters. */
typedef struct {

  /* Mandatory */
  /*  Get Position Engine Configuration Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Configuration Parameters request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000005 -- PHONE_OFFLINE \n
         - 0x00000006 -- LOC_TIMEOUT
     */

  /* Optional */
  /*  Injected Position Control */
  uint8_t injectedPositionControl_valid;  /**< Must be set to true if injectedPositionControl is being passed */
  uint8_t injectedPositionControl;
  /**<   Specifies if the injected position is used for direct calculation
       in the position engine.\n
       Valid values: \n
         - 0x01 (TRUE) -- The injected position is used in direct position
                          calculation. \n
         - 0x00 (FALSE) -- The injected position is not used direct position
                           calculation. \n
       The default value is TRUE, the injected position is used in direct
       position calculation by default.
   */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Specifies if SV usage is filtered in a position fix.
       Valid values: \n
         - 0x01 (TRUE) -- SV usage is filtered in the fix. \n
         - 0x00 (FALSE) -- SV usage is not filtered in the fix. \n
       The default value is FALSE, the SV usage is not filtered in the
       fix by default.
   */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Specifies if assistance data  is stored in the persistent memory. \n
       Valid values: \n
       0x01 (TRUE) -- Assistance data is stored in persistent memory. \n
       0x00 (FALSE) -- Assistance data is not stored in persistent memory. \n
       The default value is TRUE, the  assistance data  is stored in
       persistent memory by default.
   */
}qmiLocGetPositionEngineConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

/*Service Message Definition*/
/** @addtogroup loc_qmi_msg_ids
    @{
  */
#define QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02 0x0020
#define QMI_LOC_INFORM_CLIENT_REVISION_RESP_V02 0x0020
#define QMI_LOC_REG_EVENTS_REQ_V02 0x0021
#define QMI_LOC_REG_EVENTS_RESP_V02 0x0021
#define QMI_LOC_START_REQ_V02 0x0022
#define QMI_LOC_START_RESP_V02 0x0022
#define QMI_LOC_STOP_REQ_V02 0x0023
#define QMI_LOC_STOP_RESP_V02 0x0023
#define QMI_LOC_EVENT_POSITION_REPORT_IND_V02 0x0024
#define QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02 0x0025
#define QMI_LOC_EVENT_NMEA_IND_V02 0x0026
#define QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02 0x0027
#define QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02 0x0028
#define QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02 0x0029
#define QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02 0x002A
#define QMI_LOC_EVENT_ENGINE_STATE_IND_V02 0x002B
#define QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02 0x002C
#define QMI_LOC_EVENT_WIFI_REQ_IND_V02 0x002D
#define QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02 0x002E
#define QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02 0x002F
#define QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02 0x0030
#define QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02 0x0031
#define QMI_LOC_GET_SERVICE_REVISION_REQ_V02 0x0032
#define QMI_LOC_GET_SERVICE_REVISION_RESP_V02 0x0032
#define QMI_LOC_GET_SERVICE_REVISION_IND_V02 0x0032
#define QMI_LOC_GET_FIX_CRITERIA_REQ_V02 0x0033
#define QMI_LOC_GET_FIX_CRITERIA_RESP_V02 0x0033
#define QMI_LOC_GET_FIX_CRITERIA_IND_V02 0x0033
#define QMI_LOC_NI_USER_RESPONSE_REQ_V02 0x0034
#define QMI_LOC_NI_USER_RESPONSE_RESP_V02 0x0034
#define QMI_LOC_NI_USER_RESPONSE_IND_V02 0x0034
#define QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02 0x0035
#define QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_RESP_V02 0x0035
#define QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02 0x0035
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02 0x0036
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_RESP_V02 0x0036
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02 0x0036
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02 0x0037
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_RESP_V02 0x0037
#define QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02 0x0037
#define QMI_LOC_INJECT_UTC_TIME_REQ_V02 0x0038
#define QMI_LOC_INJECT_UTC_TIME_RESP_V02 0x0038
#define QMI_LOC_INJECT_UTC_TIME_IND_V02 0x0038
#define QMI_LOC_INJECT_POSITION_REQ_V02 0x0039
#define QMI_LOC_INJECT_POSITION_RESP_V02 0x0039
#define QMI_LOC_INJECT_POSITION_IND_V02 0x0039
#define QMI_LOC_SET_ENGINE_LOCK_REQ_V02 0x003A
#define QMI_LOC_SET_ENGINE_LOCK_RESP_V02 0x003A
#define QMI_LOC_SET_ENGINE_LOCK_IND_V02 0x003A
#define QMI_LOC_GET_ENGINE_LOCK_REQ_V02 0x003B
#define QMI_LOC_GET_ENGINE_LOCK_RESP_V02 0x003B
#define QMI_LOC_GET_ENGINE_LOCK_IND_V02 0x003B
#define QMI_LOC_SET_SBAS_CONFIG_REQ_V02 0x003C
#define QMI_LOC_SET_SBAS_CONFIG_RESP_V02 0x003C
#define QMI_LOC_SET_SBAS_CONFIG_IND_V02 0x003C
#define QMI_LOC_GET_SBAS_CONFIG_REQ_V02 0x003D
#define QMI_LOC_GET_SBAS_CONFIG_RESP_V02 0x003D
#define QMI_LOC_GET_SBAS_CONFIG_IND_V02 0x003D
#define QMI_LOC_SET_NMEA_TYPES_REQ_V02 0x003E
#define QMI_LOC_SET_NMEA_TYPES_RESP_V02 0x003E
#define QMI_LOC_SET_NMEA_TYPES_IND_V02 0x003E
#define QMI_LOC_GET_NMEA_TYPES_REQ_V02 0x003F
#define QMI_LOC_GET_NMEA_TYPES_RESP_V02 0x003F
#define QMI_LOC_GET_NMEA_TYPES_IND_V02 0x003F
#define QMI_LOC_SET_LOW_POWER_MODE_REQ_V02 0x0040
#define QMI_LOC_SET_LOW_POWER_MODE_RESP_V02 0x0040
#define QMI_LOC_SET_LOW_POWER_MODE_IND_V02 0x0040
#define QMI_LOC_GET_LOW_POWER_MODE_REQ_V02 0x0041
#define QMI_LOC_GET_LOW_POWER_MODE_RESP_V02 0x0041
#define QMI_LOC_GET_LOW_POWER_MODE_IND_V02 0x0041
#define QMI_LOC_SET_SERVER_REQ_V02 0x0042
#define QMI_LOC_SET_SERVER_RESP_V02 0x0042
#define QMI_LOC_SET_SERVER_IND_V02 0x0042
#define QMI_LOC_GET_SERVER_REQ_V02 0x0043
#define QMI_LOC_GET_SERVER_RESP_V02 0x0043
#define QMI_LOC_GET_SERVER_IND_V02 0x0043
#define QMI_LOC_DELETE_ASSIST_DATA_REQ_V02 0x0044
#define QMI_LOC_DELETE_ASSIST_DATA_RESP_V02 0x0044
#define QMI_LOC_DELETE_ASSIST_DATA_IND_V02 0x0044
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02 0x0045
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_RESP_V02 0x0045
#define QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02 0x0045
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02 0x0046
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_RESP_V02 0x0046
#define QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02 0x0046
#define QMI_LOC_INJECT_WIFI_POSITION_REQ_V02 0x0047
#define QMI_LOC_INJECT_WIFI_POSITION_RESP_V02 0x0047
#define QMI_LOC_INJECT_WIFI_POSITION_IND_V02 0x0047
#define QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02 0x0048
#define QMI_LOC_NOTIFY_WIFI_STATUS_RESP_V02 0x0048
#define QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02 0x0048
#define QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02 0x0049
#define QMI_LOC_GET_REGISTERED_EVENTS_RESP_V02 0x0049
#define QMI_LOC_GET_REGISTERED_EVENTS_IND_V02 0x0049
#define QMI_LOC_SET_OPERATION_MODE_REQ_V02 0x004A
#define QMI_LOC_SET_OPERATION_MODE_RESP_V02 0x004A
#define QMI_LOC_SET_OPERATION_MODE_IND_V02 0x004A
#define QMI_LOC_GET_OPERATION_MODE_REQ_V02 0x004B
#define QMI_LOC_GET_OPERATION_MODE_RESP_V02 0x004B
#define QMI_LOC_GET_OPERATION_MODE_IND_V02 0x004B
#define QMI_LOC_SET_SPI_STATUS_REQ_V02 0x004C
#define QMI_LOC_SET_SPI_STATUS_RESP_V02 0x004C
#define QMI_LOC_SET_SPI_STATUS_IND_V02 0x004C
#define QMI_LOC_INJECT_SENSOR_DATA_REQ_V02 0x004D
#define QMI_LOC_INJECT_SENSOR_DATA_RESP_V02 0x004D
#define QMI_LOC_INJECT_SENSOR_DATA_IND_V02 0x004D
#define QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02 0x004E
#define QMI_LOC_INJECT_TIME_SYNC_DATA_RESP_V02 0x004E
#define QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02 0x004E
#define QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02 0x004F
#define QMI_LOC_SET_CRADLE_MOUNT_CONFIG_RESP_V02 0x004F
#define QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02 0x004F
#define QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02 0x0050
#define QMI_LOC_GET_CRADLE_MOUNT_CONFIG_RESP_V02 0x0050
#define QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02 0x0050
#define QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02 0x0051
#define QMI_LOC_SET_EXTERNAL_POWER_CONFIG_RESP_V02 0x0051
#define QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02 0x0051
#define QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02 0x0052
#define QMI_LOC_GET_EXTERNAL_POWER_CONFIG_RESP_V02 0x0052
#define QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02 0x0052
#define QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02 0x0053
#define QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_RESP_V02 0x0053
#define QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02 0x0053
#define QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02 0x0054
#define QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_RESP_V02 0x0054
#define QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02 0x0054
#define QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02 0x0055
#define QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_RESP_V02 0x0055
#define QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02 0x0055
#define QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02 0x0056
#define QMI_LOC_SET_SENSOR_CONTROL_CONFIG_RESP_V02 0x0056
#define QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02 0x0056
#define QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02 0x0057
#define QMI_LOC_GET_SENSOR_CONTROL_CONFIG_RESP_V02 0x0057
#define QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02 0x0057
#define QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02 0x0058
#define QMI_LOC_SET_SENSOR_PROPERTIES_RESP_V02 0x0058
#define QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02 0x0058
#define QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02 0x0059
#define QMI_LOC_GET_SENSOR_PROPERTIES_RESP_V02 0x0059
#define QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02 0x0059
#define QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02 0x005A
#define QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_RESP_V02 0x005A
#define QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02 0x005A
#define QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02 0x005B
#define QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_RESP_V02 0x005B
#define QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02 0x005B
#define QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02 0x005C
#define QMI_LOC_INJECT_SUPL_CERTIFICATE_RESP_V02 0x005C
#define QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02 0x005C
#define QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02 0x005D
#define QMI_LOC_DELETE_SUPL_CERTIFICATE_RESP_V02 0x005D
#define QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02 0x005D
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 0x005E
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02 0x005E
#define QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02 0x005E
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02 0x005F
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02 0x005F
#define QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02 0x005F
/**
    @}
  */

/* Service Object Accessor */
/** @addtogroup wms_qmi_accessor
    @{
  */
/** This function is used internally by the autogenerated code.  Clients should use the
   macro loc_get_service_object_v02( ) that takes in no arguments. */
qmi_idl_service_object_type loc_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version );

/** This macro should be used to get the service object */
#define loc_get_service_object_v02( ) \
          loc_get_service_object_internal_v02( \
            LOC_V02_IDL_MAJOR_VERS, LOC_V02_IDL_MINOR_VERS, \
            LOC_V02_IDL_TOOL_VERS )
/**
    @}
  */


#ifdef __cplusplus
}
#endif
#endif

