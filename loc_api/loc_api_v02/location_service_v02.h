/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
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

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
 *THIS IS AN AUTO GENERATED FILE. DO NOT ALTER IN ANY WAY
 *====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* This file was generated with Tool version 5.6
   It was generated on: Fri Oct 26 2012
   From IDL File: location_service_v02.idl */

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
#define LOC_V02_IDL_MINOR_VERS 0x11
/** Major Version Number of the qmi_idl_compiler used to generate this file */
#define LOC_V02_IDL_TOOL_VERS 0x05
/** Maximum Defined Message ID */
#define LOC_V02_MAX_MESSAGE_ID 0x006F;
/**
    @}
  */


/** @addtogroup loc_qmi_consts
    @{
  */

/**  Maximum string length for the Provider field in the application ID.  */
#define QMI_LOC_MAX_APP_ID_PROVIDER_LENGTH_V02 24

/**  Maximum string length for the Name field in the application ID.  */
#define QMI_LOC_MAX_APP_ID_NAME_LENGTH_V02 32

/**  Maximum string length for the Version field in the application ID.  */
#define QMI_LOC_MAX_APP_ID_VERSION_LENGTH_V02 8

/**  Maximum length of the list containing the SVs that were used to generate
     a position report.  */
#define QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02 80

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

/**  Maximum GNSS Measurement Engine Firmware Version String length.  */
#define QMI_LOC_GNSS_ME_VERSION_STRING_MAX_LENGTH_V02 127

/**  Maximum GNSS Measurement Engine Hosted Software Version String length.  */
#define QMI_LOC_GNSS_HOSTED_SW_VERSION_STRING_MAX_LENGTH_V02 127

/**  Maximum GNSS Measurement Engine Full Version String length.  */
#define QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 255

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

/**  Maximum length of the SUPL certificate. */
#define QMI_LOC_MAX_SUPL_CERT_LENGTH_V02 2000

/**  Maximum length of the network-initiated Geofence ID
     list  */
#define QMI_LOC_MAX_NI_GEOFENCE_ID_LIST_LENGTH_V02 16

/**  Maximum length of the injected network initiated message.  */
#define QMI_LOC_MAX_INJECTED_NETWORK_INITIATED_MESSAGE_LENGTH_V02 1024
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
  /**<   Revision that the control point is using.  */
}qmiLocInformClientRevisionReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocEventRegMaskT_v02;
#define QMI_LOC_EVENT_MASK_POSITION_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00000001ull) /**<  The control point must enable this mask to receive position report
       event indications.  */
#define QMI_LOC_EVENT_MASK_GNSS_SV_INFO_V02 ((qmiLocEventRegMaskT_v02)0x00000002ull) /**<  The control point must enable this mask to receive satellite report
       event indications. These reports are sent at a 1 Hz rate.  */
#define QMI_LOC_EVENT_MASK_NMEA_V02 ((qmiLocEventRegMaskT_v02)0x00000004ull) /**<  The control point must enable this mask to receive NMEA reports for
       position and satellites in view. The report is at a 1 Hz rate.  */
#define QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000008ull) /**<  The control point must enable this mask to receive NI notify verify request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_TIME_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000010ull) /**<  The control point must enable this mask to receive time injection request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000020ull) /**<  The control point must enable this mask to receive predicted orbits request
       event indications.  */
#define QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000040ull) /**<  The control point must enable this mask to receive position injection request
       event indications.  */
#define QMI_LOC_EVENT_MASK_ENGINE_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000080ull) /**<  The control point must enable this mask to receive engine state report
       event indications.  */
#define QMI_LOC_EVENT_MASK_FIX_SESSION_STATE_V02 ((qmiLocEventRegMaskT_v02)0x00000100ull) /**<  The control point must enable this mask to receive fix session status report
       event indications.  */
#define QMI_LOC_EVENT_MASK_WIFI_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000200ull) /**<  The control point must enable this mask to receive WiFi position request
       event indications.  */
#define QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS_V02 ((qmiLocEventRegMaskT_v02)0x00000400ull) /**<  The control point must enable this mask to receive notifications from the
       GPS engine indicating its readiness to accept data from the
       sensors (accelerometer, gyroscope, etc.).  */
#define QMI_LOC_EVENT_MASK_TIME_SYNC_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00000800ull) /**<  The control point must enable this mask to receive time-sync requests
       from the GPS engine. Time sync enables the GPS engine to synchronize
       its clock with the sensor processor's clock.  */
#define QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT_V02 ((qmiLocEventRegMaskT_v02)0x00001000ull) /**<  The control point must enable this mask to receive Stationary Position
       Indicator (SPI) streaming report indications.  */
#define QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ_V02 ((qmiLocEventRegMaskT_v02)0x00002000ull) /**<  The control point must enable this mask to receive location server
       requests. These requests are generated when the service wishes to
       establish a connection with a location server. */
#define QMI_LOC_EVENT_MASK_NI_GEOFENCE_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00004000ull) /**<  The control point must enable this mask to receive notifications
       related to network-initiated Geofences. These events notify the client
       when a network-initiated Geofence is added, deleted, or edited.  */
#define QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT_V02 ((qmiLocEventRegMaskT_v02)0x00008000ull) /**<  The control point must enable this mask to receive Geofence alerts.
       These alerts are generated to inform the client of the changes that may
       affect Geofence, e.g., if GPS is turned off or if the network is
       unavailable.  */
#define QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION_V02 ((qmiLocEventRegMaskT_v02)0x00010000ull) /**<  The control point must enable this mask to receive notifications when
       a Geofence is breached. These events are generated when the UE enters
       or leaves the perimeter of a Geofence.  */
#define QMI_LOC_EVENT_MASK_PEDOMETER_CONTROL_V02 ((qmiLocEventRegMaskT_v02)0x00020000ull) /**<  The control point must enable this mask to register for Pedometer
       control requests from the location engine. Location engine sends
       this event out to control the injection of pedometer reports.  */
#define QMI_LOC_EVENT_MASK_MOTION_DATA_CONTROL_V02 ((qmiLocEventRegMaskT_v02)0x00040000ull) /**<  The control point must enable this mask to register for motion data
       control requests from the location engine. Location engine sends
       this event out to control the injection of motion data.  */
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
 Refer to the definition of the following bitmasks:
      - QMI_LOC_EVENT_MASK_POSITION_REPORT (0x00000001) --  The control point must enable this mask to receive position report
       event indications.
      - QMI_LOC_EVENT_MASK_GNSS_SV_INFO (0x00000002) --  The control point must enable this mask to receive satellite report
       event indications. These reports are sent at a 1 Hz rate.
      - QMI_LOC_EVENT_MASK_NMEA (0x00000004) --  The control point must enable this mask to receive NMEA reports for
       position and satellites in view. The report is at a 1 Hz rate.
      - QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ (0x00000008) --  The control point must enable this mask to receive NI notify verify request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_TIME_REQ (0x00000010) --  The control point must enable this mask to receive time injection request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ (0x00000020) --  The control point must enable this mask to receive predicted orbits request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ (0x00000040) --  The control point must enable this mask to receive position injection request
       event indications.
      - QMI_LOC_EVENT_MASK_ENGINE_STATE (0x00000080) --  The control point must enable this mask to receive engine state report
       event indications.
      - QMI_LOC_EVENT_MASK_FIX_SESSION_STATE (0x00000100) --  The control point must enable this mask to receive fix session status report
       event indications.
      - QMI_LOC_EVENT_MASK_WIFI_REQ (0x00000200) --  The control point must enable this mask to receive WiFi position request
       event indications.
      - QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS (0x00000400) --  The control point must enable this mask to receive notifications from the
       GPS engine indicating its readiness to accept data from the
       sensors (accelerometer, gyroscope, etc.).
      - QMI_LOC_EVENT_MASK_TIME_SYNC_REQ (0x00000800) --  The control point must enable this mask to receive time-sync requests
       from the GPS engine. Time sync enables the GPS engine to synchronize
       its clock with the sensor processor's clock.
      - QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT (0x00001000) --  The control point must enable this mask to receive Stationary Position
       Indicator (SPI) streaming report indications.
      - QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ (0x00002000) --  The control point must enable this mask to receive location server
       requests. These requests are generated when the service wishes to
       establish a connection with a location server.
      - QMI_LOC_EVENT_MASK_NI_GEOFENCE_NOTIFICATION (0x00004000) --  The control point must enable this mask to receive notifications
       related to network-initiated Geofences. These events notify the client
       when a network-initiated Geofence is added, deleted, or edited.
      - QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT (0x00008000) --  The control point must enable this mask to receive Geofence alerts.
       These alerts are generated to inform the client of the changes that may
       affect Geofence, e.g., if GPS is turned off or if the network is
       unavailable.
      - QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION (0x00010000) --  The control point must enable this mask to receive notifications when
       a Geofence is breached. These events are generated when the UE enters
       or leaves the perimeter of a Geofence.
      - QMI_LOC_EVENT_MASK_PEDOMETER_CONTROL (0x00020000) --  The control point must enable this mask to register for Pedometer
       control requests from the location engine. Location engine sends
       this event out to control the injection of pedometer reports.
      - QMI_LOC_EVENT_MASK_MOTION_DATA_CONTROL (0x00040000) --  The control point must enable this mask to register for motion data
       control requests from the location engine. Location engine sends
       this event out to control the injection of motion data.

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
  eQMI_LOC_RECURRENCE_PERIODIC_V02 = 1, /**<  Request periodic position fixes.  */
  eQMI_LOC_RECURRENCE_SINGLE_V02 = 2, /**<  Request a single position fix.  */
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
  eQMI_LOC_ACCURACY_MED_V02 = 2, /**<  Medium accuracy.  */
  eQMI_LOC_ACCURACY_HIGH_V02 = 3, /**<  High accuracy.  */
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
  eQMI_LOC_INTERMEDIATE_REPORTS_ON_V02 = 1, /**<  Intermediate reports are turned on.  */
  eQMI_LOC_INTERMEDIATE_REPORTS_OFF_V02 = 2, /**<  Intermediate reports are turned off.   */
  QMILOCINTERMEDIATEREPORTSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocIntermediateReportStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  char applicationProvider[QMI_LOC_MAX_APP_ID_PROVIDER_LENGTH_V02 + 1];
  /**<   Application provider. */

  char applicationName[QMI_LOC_MAX_APP_ID_NAME_LENGTH_V02 + 1];
  /**<   Application name. */

  uint8_t applicationVersion_valid;
  /**<   Specifies whether the application version string contains
        a valid value: \begin{itemize1}
       \item    0x00 (FALSE) -- Application version string is invalid
       \item    0x01 (TRUE) -- Application version string is valid
       \vspace{-0.18in} \end{itemize1} */

  char applicationVersion[QMI_LOC_MAX_APP_ID_VERSION_LENGTH_V02 + 1];
  /**<   Application version. */
}qmiLocApplicationIdStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to initiate a GPS session. */
typedef struct {

  /* Mandatory */
  /*  Session ID  */
  uint8_t sessionId;
  /**<   ID of the session as identified by the control point. The session ID
       is reported back in the position reports. The control point must
       specify the same session ID in the QMI_LOC_STOP_REQ message. \n
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

       Valid values: \begin{itemize1}
       \item    0x00000001 -- LOW: Client requires low horizontal accuracy
       \item    0x00000002 -- MED: Client requires medium horizontal accuracy
       \item    0x00000003 -- HIGH: Client requires high horizontal accuracy
       \vspace{-0.18in}
       \end{itemize1}
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

       Valid values: \begin{itemize1}
       \item    0x00000001 -- ON: Client is interested in receiving intermediate
                              reports
       \item    0x00000002 -- OFF: Client is not interested in receiving
                              intermediate reports
       \vspace{-0.18in}
       \end{itemize1}
  */

  /* Optional */
  /*  Minimum Interval Between Position Reports */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Minimum time interval, specified by the control point, that must elapse between
       position reports. \n
       - Units: Milliseconds \n
       - Default: 1000 ms
  */

  /* Optional */
  /*  ID of the Application that Sent this Request */
  uint8_t applicationId_valid;  /**< Must be set to true if applicationId is being passed */
  qmiLocApplicationIdStructT_v02 applicationId;
  /**<   \n Application provider, name, and version.*/
}qmiLocStartReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to stop a GPS session. */
typedef struct {

  /* Mandatory */
  /*   Session ID */
  uint8_t sessionId;
  /**<   ID of the session that was specified in the Start request
       (QMI_LOC_START_REQ).\n
       - Range: 0 to 255 */
}qmiLocStopReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPosTechMaskT_v02;
#define QMI_LOC_POS_TECH_MASK_SATELLITE_V02 ((qmiLocPosTechMaskT_v02)0x00000001) /**<  Satellites were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_CELLID_V02 ((qmiLocPosTechMaskT_v02)0x00000002) /**<  Cell towers were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_WIFI_V02 ((qmiLocPosTechMaskT_v02)0x00000004) /**<  WiFi access points were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_SENSORS_V02 ((qmiLocPosTechMaskT_v02)0x00000008) /**<  Sensors were used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_REFERENCE_LOCATION_V02 ((qmiLocPosTechMaskT_v02)0x00000010) /**<  Reference Location was used to generate the fix.  */
#define QMI_LOC_POS_TECH_MASK_INJECTED_COARSE_POSITION_V02 ((qmiLocPosTechMaskT_v02)0x00000020) /**<  Coarse position injected into the location engine was used to
        generate the fix.   */
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
  eQMI_LOC_SESS_STATUS_PHONE_OFFLINE_V02 = 6, /**<  Fix request failed because the phone is offline.  */
  eQMI_LOC_SESS_STATUS_ENGINE_LOCKED_V02 = 7, /**<  Fix request failed because the engine is locked.  */
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
       - Units: Weeks */

  uint32_t gpsTimeOfWeekMs;
  /**<   Amount of time into the current GPS week. \n
       - Units: Milliseconds */
}qmiLocGPSTimeStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  float PDOP;
  /**<   Position dilution of precision.
       \begin{itemize1}
       \item    Range: 1 (highest accuracy) to 50 (lowest accuracy)
       \item    PDOP = square root of (HDOP^2 + VDOP^2)
       \vspace{-0.18in} \end{itemize1} */

  float HDOP;
  /**<   Horizontal dilution of precision.
       \begin{itemize1}
       \item    Range: 1 (highest accuracy) to 50 (lowest accuracy)
       \vspace{-0.18in} \end{itemize1} */

  float VDOP;
  /**<   Vertical dilution of precision.
       \begin{itemize1}
       \item    Range: 1 (highest accuracy) to 50 (lowest accuracy)
       \vspace{-0.18in} \end{itemize1} */
}qmiLocDOPStructT_v02;  /* Type */
/**
    @}
  */

typedef uint32_t qmiLocSensorUsageMaskT_v02;
#define QMI_LOC_SENSOR_MASK_USED_ACCEL_V02 ((qmiLocSensorUsageMaskT_v02)0x00000001) /**<  Bitmask to specify whether an accelerometer was used.  */
#define QMI_LOC_SENSOR_MASK_USED_GYRO_V02 ((qmiLocSensorUsageMaskT_v02)0x00000002) /**<  Bitmask to specify whether a gyroscope was used.  */
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
  /**<   Specifies which sensors were used in calculating the position in the
       position report.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- SENSOR_USED_ACCEL
       \item    0x00000002 -- SENSOR_USED_GYRO
       \vspace{-0.18in} \end{itemize1} */

  qmiLocSensorAidedMaskT_v02 aidingIndicatorMask;
  /**<   Specifies which results were aided by sensors.

       Valid bitmasks: \n
         - 0x00000001 -- AIDED_HEADING \n
         - 0x00000002 -- AIDED_SPEED \n
         - 0x00000004 -- AIDED_POSITION \n
         - 0x00000008 -- AIDED_VELOCITY */
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
  eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING_V02 = 12, /**<  Time is set by the sleep time tag provided by the GSM network  */
  eQMI_LOC_TIME_SRC_UNKNOWN_V02 = 13, /**<  Source of the time is unknown  */
  eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK_V02 = 14, /**<  Time is derived from system clock (better known as slow clock).
       GNSS time is maintained irrespective of the GNSS receiver state  */
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
  eQMI_LOC_RELIABILITY_MEDIUM_V02 = 3, /**<  Location reliability is medium; limited cross-check passed   */
  eQMI_LOC_RELIABILITY_HIGH_V02 = 4, /**<  Location reliability is high; strong cross-check passed  */
  QMILOCRELIABILITYENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocReliabilityEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends the position report to the control point. */
typedef struct {

  /* Mandatory */
  /*   Session Status */
  qmiLocSessionStatusEnumT_v02 sessionStatus;
  /**<   Session status.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- SESS_STATUS_SUCCESS
        \item    0x00000001 -- SESS_STATUS_IN_PROGRESS
        \item    0x00000002 -- SESS_STATUS_GENERAL_FAILURE
        \item    0x00000003 -- SESS_STATUS_TIMEOUT
        \item    0x00000004 -- SESS_STATUS_USER_END
        \item    0x00000005 -- SESS_STATUS_BAD_PARAMETER
        \item    0x00000006 -- SESS_STATUS_PHONE_OFFLINE
        \item    0x00000007 -- SESS_STATUS_ENGINE_LOCKED
        \vspace{-0.18in} \end{itemize1}
      */

  /* Mandatory */
  /*   Session ID */
  uint8_t sessionId;
  /**<    ID of the session that was specified in the Start request
        QMI_LOC_START_REQ. \n
        - Range: 0 to 255 */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -90.0 to 90.0   \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0   \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Units: Meters */

  /* Optional */
  /*  Elliptical Horizontal Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Units: Decimal degrees \n
       - Range: 0 to 180 */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal uncertainty confidence.\n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position. \n
       Valid values: \begin{itemize1}
       \item    0x00000000 -- RELIABILITY_NOT_SET
       \item    0x00000001 -- RELIABILITY_VERY_LOW
       \item    0x00000002 -- RELIABILITY_LOW
       \item    0x00000003 -- RELIABILITY_MEDIUM
       \item    0x00000004 -- RELIABILITY_HIGH
       \vspace{-0.18in} \end{itemize1}
   */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t speedHorizontal_valid;  /**< Must be set to true if speedHorizontal is being passed */
  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units: Meters/second */

  /* Optional */
  /*  Speed Uncertainty */
  uint8_t speedUnc_valid;  /**< Must be set to true if speedUnc is being passed */
  float speedUnc;
  /**<   3-D Speed uncertainty.\n
       - Units: Meters/second */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units: Meters \n
       - Range: -500 to 15883 */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Units: Meters */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units: Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence.\n
       - Units: Percent    \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position. \n
        Valid values: \begin{itemize1}
        \item    0x00000000 -- RELIABILITY_NOT_SET
        \item    0x00000001 -- RELIABILITY_VERY_LOW
        \item    0x00000002 -- RELIABILITY_LOW
        \item    0x00000003 -- RELIABILITY_MEDIUM
        \item    0x00000004 -- RELIABILITY_HIGH
        \vspace{-0.18in} \end{itemize1}*/

  /* Optional */
  /*  Vertical Speed */
  uint8_t speedVertical_valid;  /**< Must be set to true if speedVertical is being passed */
  float speedVertical;
  /**<   Vertical speed.\n
         - Units: Meters/second */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading.\n
         - Units: Degrees \n
         - Range: 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty.\n
       - Units: Degrees \n
       - Range: 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  /* Optional */
  /*  Technology Used */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \begin{itemize1}
       \item 0x00000001 -- SATELLITE
       \item 0x00000002 -- CELLID
       \item 0x00000004 -- WIFI
       \item 0x00000008 -- SENSORS
       \item 0x00000010 -- REFERENCE_LOCATION
       \item 0x00000020 -- INJECTED_COARSE_POSITION
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;
  /**<   \n Dilution of precision associated with this position. */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
       - Units: Milliseconds since Jan. 1, 1970 */

  /* Optional */
  /*  Leap Seconds */
  uint8_t leapSeconds_valid;  /**< Must be set to true if leapSeconds is being passed */
  uint8_t leapSeconds;
  /**<   Leap second information. If leapSeconds is not available,
         timestampUtc is calculated based on a hard-coded value
         for leap seconds. \n
         - Units: Seconds */

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;
  /**<   \n The number of weeks since Jan. 5, 1980, and
       milliseconds into the current week. */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Units: Milliseconds  */

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source. Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time.
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1x system.
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (i.e.,
       associating network time with GPS time).
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection.
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite.
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites.
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known.
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained.
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT.
       This is done when the clock time uncertainty is large.
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLO satellites
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLO time
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from system clock (better known as slow clock).
       GNSS time is maintained irrespective of the GNSS receiver state  */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;
  /**<   \n Whether sensor data was used in computing the position in this
       position report. */

  /* Optional */
  /*  Fix Count for This Session */
  uint8_t fixId_valid;  /**< Must be set to true if fixId is being passed */
  uint32_t fixId;
  /**<   Fix count for the session. Starts with 0 and increments by one
       for each successive position report for a particular session. */

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
      used for calculating this position report. The following
      information is associated with each SV ID: \begin{itemize1}
         \item    Range:    \begin{itemize1}
         \item    For GPS:     1 to 32
         \item    For SBAS:    33 to 64
         \item    For GLONASS: 65 to 96
         \item    For QZSS: 193 to 197
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */
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
  eQMI_LOC_SV_SYSTEM_COMPASS_V02 = 4, /**<  COMPASS satellite.  */
  eQMI_LOC_SV_SYSTEM_GLONASS_V02 = 5, /**<  GLONASS satellite.  */
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
  eQMI_LOC_SV_STATUS_SEARCH_V02 = 2, /**<  The system is searching for this SV.  */
  eQMI_LOC_SV_STATUS_TRACK_V02 = 3, /**<  SV is being tracked.  */
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

         Valid bitmasks: \begin{itemize1}
         \item    0x00000001 -- VALID_SYSTEM
         \item    0x00000002 -- VALID_GNSS_SVID
         \item    0x00000004 -- VALID_HEALTH_STATUS
         \item    0x00000008 -- VALID_PROCESS_STATUS
         \item    0x00000010 -- VALID_SVINFO_MASK
         \item    0x00000020 -- VALID_ELEVATION
         \item    0x00000040 -- VALID_AZIMUTH
         \item    0x00000080 -- VALID_SNR
         \vspace{-0.18in} \end{itemize1}  */

  qmiLocSvSystemEnumT_v02 system;
  /**<   Indicates to which constellation this SV belongs.

         Valid values: \begin{itemize1}
         \item    0x00000001 -- eQMI_LOC_SV_SYSTEM_GPS
         \item    0x00000002 -- eQMI_LOC_SV_SYSTEM_GALILEO
         \item    0x00000003 -- eQMI_LOC_SV_SYSTEM_SBAS
         \item    0x00000004 -- eQMI_LOC_SV_SYSTEM_COMPASS
         \item    0x00000005 -- eQMI_LOC_SV_SYSTEM_GLONASS
         \vspace{-0.18in} \end{itemize1} */

  uint16_t gnssSvId;
  /**<   GNSS SV ID.
         \begin{itemize1}
         \item Range:  \begin{itemize1}
           \item For GPS:      1 to 32
           \item For GLONASS: 1 to 32
           \item For SBAS:    120 to 151
         \end{itemize1} \end{itemize1}

        The GPS and GLONASS SVs can be disambiguated using the system field. */

  uint8_t healthStatus;
  /**<   Health status.
         \begin{itemize1}
         \item    Range: 0 to 1; 0 = unhealthy, \newline 1 = healthy
         \vspace{-0.18in} \end{itemize1}*/

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
         - Units: Degrees \n
         - Range: 0 to 90 */

  float azimuth;
  /**<   SV azimuth angle.\n
         - Units: Degrees \n
         - Range: 0 to 360 */

  float snr;
  /**<   SV signal-to-noise ratio. \n
         - Units: dB-Hz */
}qmiLocSvInfoStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends a satellite report to the control point. */
typedef struct {

  /* Mandatory */
  /*  Altitude Source */
  uint8_t altitudeAssumed;
  /**<   Whether altitude is assumed or calculated: \begin{itemize1}
         \item    0x00 (FALSE) -- Valid altitude is calculated
         \item    0x01 (TRUE) -- Valid altitude is assumed; there may not be
                                 enough satellites to determine precise altitude
          \vspace{-0.18in} \end{itemize1}*/

  /* Optional */
  /*  Satellite Info */
  uint8_t svList_valid;  /**< Must be set to true if svList is being passed */
  uint32_t svList_len;  /**< Must be set to # of elements in svList */
  qmiLocSvInfoStructT_v02 svList[QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02];
  /**<   \n SV information list. */
}qmiLocEventGnssSvInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Sends NMEA sentences to the control point */
typedef struct {

  /* Mandatory */
  /*  NMEA String */
  char nmea[QMI_LOC_NMEA_STRING_MAX_LENGTH_V02 + 1];
  /**<   NMEA string.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 201
       \vspace{-0.18in} \end{itemize1}*/
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
  eQMI_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP_V02 = 4, /**<  Notify and verify, and require a response.  */
  eQMI_LOC_NI_USER_NOTIFY_VERIFY_PRIVACY_OVERRIDE_V02 = 5, /**<  Notify and Verify, and require a response.  */
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
  eQMI_LOC_NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED_V02 = 3, /**<  MS-assisted preferred, but MS-based allowed.  */
  eQMI_LOC_NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED_V02 = 4, /**<  MS-based preferred, but MS-assisted allowed.  */
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
  eQMI_LOC_NI_VX_LATIN_V02 = 8, /**<  Encoding is LATIN.  */
  eQMI_LOC_NI_VX_GSM_V02 = 9, /**<  Encoding is GSM.  */
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
         - 0x00 (FALSE) -- QoS is not included */

  uint8_t posQos;
  /**<   Position QoS timeout. \n
         - Units: Seconds \n
         - Range: 0 to 255 */

  uint32_t numFixes;
  /**<   Number of fixes allowed. */

  uint32_t timeBetweenFixes;
  /**<   Time between fixes.\n
         - Units: Seconds */

  qmiLocNiVxPosModeEnumT_v02 posMode;
  /**<   Position mode.

         Valid values: \begin{itemize1}
         \item    0x00000001 -- NI_VX_MS_ASSISTED_ONLY
         \item    0x00000002 -- NI_VX_MS_BASED_ONLY
         \item    0x00000003 -- NI_VX_MS_ASSISTED_PREFERRED_MS_BASED_ALLOWED
         \item    0x00000004 -- NI_VX_MS_BASED_PREFERRED_MS_ASSISTED_ALLOWED
         \vspace{-0.18in} \end{itemize1}
    */

  qmiLocNiVxRequestorIdEncodingSchemeEnumT_v02 encodingScheme;
  /**<   VX encoding scheme.

         Valid values: \begin{itemize1}
         \item    0x00000000 -- NI_VX_OCTET
         \item    0x00000001 -- NI_VX_EXN_PROTOCOL_MSG
         \item    0x00000002 -- NI_VX_ASCII
         \item    0x00000003 -- NI_VX_IA5
         \item    0x00000004 -- NI_VX_UNICODE
         \item    0x00000005 -- NI_VX_SHIFT_JIS
         \item    0x00000006 -- NI_VX_KOREAN
         \item    0x00000007 -- NI_VX_LATIN_HEBREW
         \item    0x00000008 -- NI_VX_LATIN
         \item    0x00000009 -- NI_VX_GSM
         \vspace{-0.18in} \end{itemize1}
    */

  uint32_t requestorId_len;  /**< Must be set to # of elements in requestorId */
  uint8_t requestorId[QMI_LOC_NI_MAX_REQUESTOR_ID_LENGTH_V02];
  /**<   Requestor ID. \n
       - Type:  Array of bytes \n
       - Maximum array length: 200
  */

  uint16_t userRespTimerInSeconds;
  /**<   Time to wait for the user to respond. \n
         - Units: Seconds */
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
  eQMI_LOC_NI_SUPL_POSMETHOD_OTDOA_V02 = 9, /**<  Observed time delay of arrival.  */
  eQMI_LOC_NI_SUPL_POSMETHOD_NO_POSITION_V02 = 10, /**<  No position.  */
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
  eQMI_LOC_NI_SUPL_UCS2_V02 = 29, /**<  Encoding is UCS 2.  */
  eQMI_LOC_NI_SUPL_GSM_DEFAULT_V02 = 30, /**<  Encoding is GSM default.  */
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
  eQMI_LOC_NI_SUPL_FORMAT_IMSPUBLIC_IDENTITY_V02 = 7, /**<  SUPL IMS public identity    */
  eQMI_LOC_NI_SUPL_FORMAT_OSS_UNKNOWN_V02 = 2147483647, /**<  SUPL unknown format.  */
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

        Valid values: \begin{itemize1}
        \item    0x00000000 -- FORMAT_LOGICAL_NAME
        \item    0x00000001 -- FORMAT_EMAIL_ADDRESS
        \item    0x00000002 -- FORMAT_MSISDN
        \item    0x00000003 -- FORMAT_URL
        \item    0x00000004 -- FORMAT_SIP_URL
        \item    0x00000005 -- FORMAT_MIN
        \item    0x00000006 -- FORMAT_MDN
        \item    0x00000007 -- FORMAT_IMSPUBLIC_IDENTITY
        \item    0x7FFFFFFF -- FORMAT_OSS_UNKNOWN
        \vspace{-0.18in} \end{itemize1}
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
          - 0x08 -- QOP_DELAY_VALID*/

  uint8_t horizontalAccuracy;
  /**<   Horizontal accuracy. \n
        - Units: Meters */

  uint8_t verticalAccuracy;
  /**<   Vertical accuracy. \n
        - Units: Meters */

  uint16_t maxLocAge;
  /**<   Maximum age of the location if the engine sends a previously
        computed position. \n
        - Units: Seconds */

  uint8_t delay;
  /**<   Delay the server is willing to tolerate for the fix. \n
        - Units: Seconds */
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
  /**<   IPV4 address. */

  uint16_t port;
  /**<   IPV4 port. */
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
       - Maximum length of the array: 8 */

  uint32_t port;
  /**<   IPV6 port. */
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
  /**<   IPV4 address and port. */

  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   IPV6 address and port. */

  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 256
       \vspace{-0.18in} \end{itemize1}
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

        Valid bitmasks: \begin{itemize1}
        \item    0x00000001 -- SUPL_SERVER_INFO
        \item    0x00000002 -- SUPL_SESSION_ID
        \item    0x00000004 -- SUPL_HASH
        \item    0x00000008 -- SUPL_POS_METHOD
        \item    0x00000010 -- SUPL_DATA_CODING_SCHEME
        \item    0x00000020 -- SUPL_REQUESTOR_ID
        \item    0x00000040 -- SUPL_CLIENT_NAME
        \item    0x00000080 -- SUPL_QOP
        \item    0x00000100 -- SUPL_USER_RESP_TIMER
        \vspace{-0.18in} \end{itemize1}
  */

  qmiLocNiSuplServerInfoStructT_v02 suplServerInfo;
  /**<   SUPL server information. */

  uint8_t suplSessionId[QMI_LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH_V02];
  /**<   SUPL session ID. \n
       - Type: Array of unsigned integers \n
       - Maximum length of the array: 4 */

  uint8_t suplHash[QMI_LOC_NI_SUPL_HASH_LENGTH_V02];
  /**<   Hash for SUPL_INIT; used to validate that the message was not
       corrupted. \n
       - Type: Array of unsigned integers \n
       - Length of the array: 8 */

  qmiLocNiSuplPosMethodEnumT_v02 posMethod;
  /**<   GPS mode to be used for the fix.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- AGPS_SETASSISTED
       \item    0x00000002 -- AGPS_SETBASED
       \item    0x00000003 -- AGPS_SETASSISTED_PREF
       \item    0x00000004 -- AGPS_SETBASED_PREF
       \item    0x00000005 -- AUTONOMOUS_GPS
       \item    0x00000006 -- AFLT
       \item    0x00000007 -- ECID
       \item    0x00000008 -- EOTD
       \item    0x00000009 -- OTDOA
       \item    0x0000000A -- NO_POSITION
       \vspace{-0.18in} \end{itemize1}
  */

  qmiLocNiDataCodingSchemeEnumT_v02 dataCodingScheme;
  /**<   Data coding scheme applies to both the requestor ID and the client
       name.

       Valid values: \begin{itemize1}
       \item    0x0000000C -- NI_SS_GERMAN
       \item    0x0000000D -- NI_SS_ENGLISH
       \item    0x0000000E -- NI_SS_ITALIAN
       \item    0x0000000F -- NI_SS_FRENCH
       \item    0x00000010 -- NI_SS_SPANISH
       \item    0x00000011 -- NI_SS_DUTCH
       \item    0x00000012 -- NI_SS_SWEDISH
       \item    0x00000013 -- NI_SS_DANISH
       \item    0x00000014 -- NI_SS_PORTUGUESE
       \item    0x00000015 -- NI_SS_FINNISH
       \item    0x00000016 -- NI_SS_NORWEGIAN
       \item    0x00000017 -- NI_SS_GREEK
       \item    0x00000018 -- NI_SS_TURKISH
       \item    0x00000019 -- NI_SS_HUNGARIAN
       \item    0x0000001A -- NI_SS_POLISH
       \item    0x0000001B -- NI_SS_LANGUAGE_UNSPEC
       \item    0x0000001C -- NI_SUPL_UTF8
       \item    0x0000001D -- NI_SUPL_UCS2
       \item    0x0000001E -- NI_SUPL_GSM_DEFAULT
       \vspace{-0.18in} \end{itemize1}
   */

  qmiLocNiSuplFormattedStringStructT_v02 requestorId;
  /**<   Requestor ID. The encoding scheme for requestor_id is specified in
       the dataCodingScheme field. */

  qmiLocNiSuplFormattedStringStructT_v02 clientName;
  /**<   Client name. The encoding scheme for client_name is specified in
       the dataCodingScheme field. */

  qmiLocNiSuplQopStructT_v02 suplQop;
  /**<   SUPL QoP. */

  uint16_t userResponseTimer;
  /**<   Time to wait for the user to respond. \n
       - Units: Seconds*/
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
  eQMI_LOC_NI_LOCATIONTYPE_CURRENT_OR_LAST_KNOWN_LOCATION_V02 = 2, /**<  Last known location; may be current location.  */
  eQMI_LOC_NI_LOCATIONTYPE_INITIAL_LOCATION_V02 = 3, /**<  Initial location.  */
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

       Valid values: \begin{itemize1}
       \item    0x0000000C -- NI_SS_GERMAN
       \item    0x0000000D -- NI_SS_ENGLISH
       \item    0x0000000E -- NI_SS_ITALIAN
       \item    0x0000000F -- NI_SS_FRENCH
       \item    0x00000010 -- NI_SS_SPANISH
       \item    0x00000011 -- NI_SS_DUTCH
       \item    0x00000012 -- NI_SS_SWEDISH
       \item    0x00000013 -- NI_SS_DANISH
       \item    0x00000014 -- NI_SS_PORTUGUESE
       \item    0x00000015 -- NI_SS_FINNISH
       \item    0x00000016 -- NI_SS_NORWEGIAN
       \item    0x00000017 -- NI_SS_GREEK
       \item    0x00000018 -- NI_SS_TURKISH
       \item    0x00000019 -- NI_SS_HUNGARIAN
       \item    0x0000001A -- NI_SS_POLISH
       \item    0x0000001B -- NI_SS_LANGUAGE_UNSPEC
       \item    0x0000001C -- NI_SUPL_UTF8
       \item    0x0000001D -- NI_SUPL_UCS2
       \item    0x0000001E -- NI_SUPL_GSM_DEFAULT
       \vspace{-0.18in} \end{itemize1}

  */

  uint32_t codedString_len;  /**< Must be set to # of elements in codedString */
  uint8_t codedString[QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02];
  /**<   Coded string. \n
       - Type: Array of bytes \n
       - Maximum string length: 20 */
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

       Valid bitmasks: \begin{itemize1}
       \item    0x0001 -- INVOKE_ID_MASK
       \item    0x0002 -- DATA_CODING_SCHEME_MASK
       \item    0x0004 -- NOTIFICATION_TEXT_MASK
       \item    0x0008 -- CLIENT_ADDRESS_MASK
       \item    0x0010 -- LOCATION_TYPE_MASK
       \item    0x0020 -- REQUESTOR_ID_MASK
       \item    0x0040 -- CODEWORD_STRING_MASK
       \item    0x0080 -- SERVICE_TYPE_MASK
       \item    0x0100 -- USER_RESP_TIMER_MASK
       \vspace{-0.18in} \end{itemize1}
  */

  uint8_t invokeId;
  /**<   Supplementary Services invoke ID. */

  qmiLocNiDataCodingSchemeEnumT_v02 dataCodingScheme;
  /**<   Type of data encoding scheme for the text.
       Applies to both the notification text and the client address.

       Valid values: \begin{itemize1}
       \item    0x0000000C -- NI_SS_GERMAN
       \item    0x0000000D -- NI_SS_ENGLISH
       \item    0x0000000E -- NI_SS_ITALIAN
       \item    0x0000000F -- NI_SS_FRENCH
       \item    0x00000010 -- NI_SS_SPANISH
       \item    0x00000011 -- NI_SS_DUTCH
       \item    0x00000012 -- NI_SS_SWEDISH
       \item    0x00000013 -- NI_SS_DANISH
       \item    0x00000014 -- NI_SS_PORTUGUESE
       \item    0x00000015 -- NI_SS_FINNISH
       \item    0x00000016 -- NI_SS_NORWEGIAN
       \item    0x00000017 -- NI_SS_GREEK
       \item    0x00000018 -- NI_SS_TURKISH
       \item    0x00000019 -- NI_SS_HUNGARIAN
       \item    0x0000001A -- NI_SS_POLISH
       \item    0x0000001B -- NI_SS_LANGUAGE_UNSPEC
       \item    0x0000001C -- NI_SUPL_UTF8
       \item    0x0000001D -- NI_SUPL_UCS2
       \item    0x0000001E -- NI_SUPL_GSM_DEFAULT
       \vspace{-0.18in} \end{itemize1}
 */

  uint32_t notificationText_len;  /**< Must be set to # of elements in notificationText */
  uint8_t notificationText[QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02];
  /**<   Notification text; the encoding method is specified in
       dataCodingScheme. \n
       - Type: Array of bytes \n
       - Maximum array length: 64 */

  uint32_t clientAddress_len;  /**< Must be set to # of elements in clientAddress */
  uint8_t clientAddress[QMI_LOC_NI_MAX_EXT_CLIENT_ADDRESS_V02];
  /**<   Client address; the encoding method is specified in
       dataCodingScheme. \n
       - Maximum array length: 20 */

  qmiLocNiLocationTypeEnumT_v02 locationType;
  /**<   Location type.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- CURRENT_LOCATION
       \item    0x00000002 -- CURRENT_OR_LAST_KNOWN_LOCATION
       \item    0x00000004 -- INITIAL_LOCATION
       \vspace{-0.18in} \end{itemize1}
  */

  qmiLocNiUmtsCpCodedStringStructT_v02 requestorId;
  /**<   Requestor ID; the encoding method is specified in the
       qmiLocNiUmtsCpCodedStringStructT.dataCodingScheme field. */

  qmiLocNiUmtsCpCodedStringStructT_v02 codewordString;
  /**<   Codeword string; the encoding method is specified in the
       qmiLocNiUmtsCpCodedStringStructT.dataCodingScheme field. */

  uint8_t lcsServiceTypeId;
  /**<   Service type ID. */

  uint16_t userResponseTimer;
  /**<   Time to wait for the user to respond. \n
       - Units: Seconds */
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
  /**<   Ongoing NI session request; this information is currently not filled. */

  qmiLocNiServiceInteractionEnumT_v02 serviceInteractionType;
  /**<   Service interaction type specified in qmiLocNiServiceInteractionEnumT.

        Valid values: \begin{itemize1}
        \item    0x00000001 -- ONGOING_NI_INCOMING_MO
        \vspace{-0.18in} \end{itemize1}
   */
}qmiLocNiVxServiceInteractionStructT_v02;  /* Type */
/**
    @}
  */

typedef uint16_t qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02;
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WLAN_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0001) /**<  Denotes that WLAN measurements are allowed as part of location ID
       and multiple location IDs in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_GSM_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0002) /**<  Denotes that GSM measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WCDMA_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0004) /**<  Denotes that WCDMA measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_CDMA_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0008) /**<  Denotes that CDMA measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HRDP_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0010) /**<  Denotes that HRDP measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_UMB_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0020) /**<  Denotes that UMB measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.           */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_LTE_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0040) /**<  Denotes that LTE measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.   */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_WIMAX_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0080) /**<  Denotes that WIMAX measurements are allowed as part of location ID
       and multiple location ID in the SUPL_POS_INIT message.   */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_HISTORIC_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0100) /**<  Denotes that historical information is allowed as part of
       multiple location ID in the SUPL_POS_INIT message.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_SUPPORTED_NETWORK_NONSVRV_V02 ((qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02)0x0200) /**<  Denotes that information about nonserving cells is allowed
       as part of multiple location ID in the SUPL_POS_INIT message.    */
/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNISUPLVER2EXTTRIGGERTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_SINGLE_SHOT_V02 = -1, /**<  The SUPL INIT message indicates a request for a single shot
       triggered session.  */
  eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_PERIODIC_V02 = 0, /**<  The SUPL INIT message indicates a request for a periodic
       triggered session.  */
  eQMI_LOC_SUPL_VER_2_EXT_TRIGGER_TYPE_AREA_EVENT_V02 = 1, /**<  The SUPL INIT message indicates a request for an area event
       triggered session.  */
  QMILOCNISUPLVER2EXTTRIGGERTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiSuplVer2ExtTriggerTypeEnumT_v02;
/**
    @}
  */

typedef uint16_t qmiLocNiSuplVer2ExtGnssTypeMaskT_v02;
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GPS_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0001) /**<  GPS is allowed to be used as the positioning technology.   */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GLONASS_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0002) /**<  GLONASS is allowed to be used as the positioning technology.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_GALILEO_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0004) /**<  Galileo is allowed to be used as the positioning technology.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_SBAS_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0008) /**<  SBAS is allowed to be used as the positioning technology.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_QZSS_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0010) /**<  QZSS is allowed to be used as the positioning technology.  */
#define QMI_LOC_SUPL_VER_2_EXT_MASK_GNSS_MODERN_GPS_V02 ((qmiLocNiSuplVer2ExtGnssTypeMaskT_v02)0x0020) /**<  Modern GPS is allowed to be used as the positioning technology.    */
/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocNiSuplVer2ExtSupportedNetworksMaskT_v02 supportedNetworksMask;
  /**<   Specifies which type of network measurements are allowed to be sent as
       part of the Location ID or Multiple Location IDs parameter in the
       SUPL_POS_INIT message (see \hyperref[S4]{[S4]}).

       Valid bitmasks: \begin{itemize1}
       \item    0x0001 -- SUPPORTED_NETWORK_WLAN
       \item    0x0002 -- SUPPORTED_NETWORK_GSM
       \item    0x0004 -- SUPPORTED_NETWORK_WCDMA
       \item    0x0008 -- SUPPORTED_NETWORK_CDMA
       \item    0x0010 -- SUPPORTED_NETWORK_HRDP
       \item    0x0020 -- SUPPORTED_NETWORK_UMB
       \item    0x0040 -- SUPPORTED_NETWORK_LTE
       \item    0x0080 -- SUPPORTED_NETWORK_WIMAX
       \item    0x0100 -- SUPPORTED_NETWORK_HISTORIC
       \item    0x0200 -- SUPPORTED_NETWORK_NONSVRV
       \vspace{-0.18in} \end{itemize1}
   */

  qmiLocNiSuplVer2ExtTriggerTypeEnumT_v02 triggerType;
  /**<   Specifies the type of session trigger requested in the
       SUPL_POS_INIT message (see \hyperref[S4]{[S4]}).

       Valid values: \begin{itemize1}
       \item    0xFFFFFFFF  -- TRIGGER_TYPE_SINGLE_SHOT
       \item    0x00000000  -- TRIGGER_TYPE_PERIODIC
       \item    0x00000001  -- TRIGGER_TYPE_AREA_EVENT
       \vspace{-0.18in} \end{itemize1}   */

  qmiLocNiSuplVer2ExtGnssTypeMaskT_v02 gnssType;
  /**<   Specifies which GNSS technologies are allowed as positioning
       technologies.

       Valid bitmasks: \n
        - 0x0001 -- GNSS_GPS \n
        - 0x0002 -- GNSS_GLONASS \n
        - 0x0004 -- GNSS_GALILEO \n
        - 0x0008 -- GNSS_SBAS \n
        - 0x0010 -- GNSS_QZSS \n
        - 0x0020 -- GNSS_MODERN_GPS
  */
}qmiLocNiSuplVer2ExtStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  char eslpUrl[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   The ESLP URL
       Maximum length: 255 bytes */
}qmiLocEmergencyNotificationStructT_v02;  /* Type */
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

       Valid values: \begin{itemize1}
       \item    0x00000001 -- NO_NOTIFY_NO_VERIFY
       \item    0x00000002 -- NOTIFY_ONLY
       \item    0x00000003 -- ALLOW_NO_RESP
       \item    0x00000004 -- NOT_ALLOW_NO_RESP
       \item    0x00000005 -- PRIVACY_OVERRIDE
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Network Initiated Vx Request */
  uint8_t NiVxInd_valid;  /**< Must be set to true if NiVxInd is being passed */
  qmiLocNiVxNotifyVerifyStructT_v02 NiVxInd;
  /**<   \n Optional NI Vx request payload. */

  /* Optional */
  /*  Network Initiated SUPL Request */
  uint8_t NiSuplInd_valid;  /**< Must be set to true if NiSuplInd is being passed */
  qmiLocNiSuplNotifyVerifyStructT_v02 NiSuplInd;
  /**<   \n Optional NI SUPL request payload. */

  /* Optional */
  /*  Network Initiated UMTS Control Plane Request */
  uint8_t NiUmtsCpInd_valid;  /**< Must be set to true if NiUmtsCpInd is being passed */
  qmiLocNiUmtsCpNotifyVerifyStructT_v02 NiUmtsCpInd;
  /**<   \n Optional NI UMTS-CP request payload. */

  /* Optional */
  /*  Network Initiated Service Interaction Request */
  uint8_t NiVxServiceInteractionInd_valid;  /**< Must be set to true if NiVxServiceInteractionInd is being passed */
  qmiLocNiVxServiceInteractionStructT_v02 NiVxServiceInteractionInd;
  /**<   \n Optional NI service interaction payload. */

  /* Optional */
  /*  Network Initiated SUPL Version 2 Extension */
  uint8_t NiSuplVer2ExtInd_valid;  /**< Must be set to true if NiSuplVer2ExtInd is being passed */
  qmiLocNiSuplVer2ExtStructT_v02 NiSuplVer2ExtInd;
  /**<   \n Optional NI SUPL Version 2 Extension payload. When present,
          this payload is to be used in conjunction with the SUPL
          indication payload. */

  /* Optional */
  /*  SUPL Emergency Notification */
  uint8_t suplEmergencyNotification_valid;  /**< Must be set to true if suplEmergencyNotification is being passed */
  qmiLocEmergencyNotificationStructT_v02 suplEmergencyNotification;
  /**<    This specifies that the corresponding NI notification is an
        emergency notification. Emergency notification
        can be given even without an ESLP address */
}qmiLocEventNiNotifyVerifyReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  char serverUrl[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   Assistance server URL.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 256
       \vspace{-0.18in} \end{itemize1} */
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
       - Units: Milliseconds */

  uint32_t timeServerList_len;  /**< Must be set to # of elements in timeServerList */
  qmiLocAssistanceServerUrlStructT_v02 timeServerList[QMI_LOC_MAX_NTP_SERVERS_V02];
  /**<   List of Time Server URL's that are recommended by the service for time
       information, the list is ordered, the client is to use the first
       server specified in the list as the primary URL to fetch NTP time,
       the second one as secondary, and so on. \n
       - Maximum server list items: 3 */
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
       location service for NTP time. */
}qmiLocEventInjectTimeReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t maxFileSizeInBytes;
  /**<   Maximum allowable predicted orbits file size (in bytes). */

  uint32_t maxPartSize;
  /**<   Maximum allowable predicted orbits file chunk size (in bytes). */
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
       - Maximum number of servers that can be specified: 3 */
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
  /**<   \n Maximum part and file size allowed to be injected in the engine. */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \n List of servers that can be used by the client to download
       predicted orbits data. */
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
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -90.0 to 90.0      \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Mandatory */
  /*  Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0     \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Mandatory */
  /*  Circular Horizontal Uncertainty */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units: Meters */

  /* Mandatory */
  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp.
       \begin{itemize1}
       \item    Units: Milliseconds since Jan. 1, 1970
       \vspace{-0.18in} \end{itemize1} */
}qmiLocEventInjectPositionReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCENGINESTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_ENGINE_STATE_ON_V02 = 1, /**<  Location engine is on.  */
  eQMI_LOC_ENGINE_STATE_OFF_V02 = 2, /**<  Location engine is off.  */
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
  eQMI_LOC_FIX_SESSION_STARTED_V02 = 1, /**<  Location fix session has started.  */
  eQMI_LOC_FIX_SESSION_FINISHED_V02 = 2, /**<  Location fix session has ended.  */
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
    - Range: 0 to 255 */
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
  eQMI_LOC_WIFI_START_PERIODIC_KEEP_WARM_V02 = 1, /**<  Keep warm for low frequency fixes without data downloads.  */
  eQMI_LOC_WIFI_STOP_PERIODIC_FIXES_V02 = 2, /**<  Stop periodic fixes request.  */
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

        Valid values: \begin{itemize1}
        \item    0x00000000 -- START_PERIODIC_HI_FREQ_FIXES
        \item    0x00000001 -- START_PERIODIC_KEEP_WARM
        \item    0x00000002 -- STOP_PERIODIC_FIXES
        \vspace{-0.18in} \end{itemize1}
   */

  /* Optional */
  /*  Time Between Fixes */
  uint8_t tbfInMs_valid;  /**< Must be set to true if tbfInMs is being passed */
  uint16_t tbfInMs;
  /**<   Time between fixes for a periodic request.\n
        - Units: Milliseconds */
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

       samplesPerBatch must be a nonzero positive value.
  */

  uint16_t batchesPerSecond;
  /**<   Number of sensor-data batches the GNSS location engine is to receive
       per second. The rate is specified in an integral number of batches per
       second (Hz). \n

       batchesPerSecond must be a nonzero positive value.
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

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE)  -- GNSS location engine is ready to accept sensor
                                data
       \item    0x00 (FALSE) -- GNSS location engine is not ready to accept
                                sensor data
        \vspace{-0.18in} \end{itemize1}
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
  /*  Gyroscope Accept Ready */
  uint8_t gyroReady_valid;  /**< Must be set to true if gyroReady is being passed */
  qmiLocSensorReadyStatusStructT_v02 gyroReady;
  /**<   \n Whether the GNSS location engine is ready to accept gyroscope sensor
         data.
   */

  /* Optional */
  /*  Accelerometer Temperature Accept Ready */
  uint8_t accelTemperatureReady_valid;  /**< Must be set to true if accelTemperatureReady is being passed */
  qmiLocSensorReadyStatusStructT_v02 accelTemperatureReady;
  /**<   \n Whether the GNSS location engine is ready to accept accelerometer
         temperature data.
   */

  /* Optional */
  /*  Gyroscope Temperature Accept Ready */
  uint8_t gyroTemperatureReady_valid;  /**< Must be set to true if gyroTemperatureReady is being passed */
  qmiLocSensorReadyStatusStructT_v02 gyroTemperatureReady;
  /**<   \n Whether the GNSS location engine is ready to accept gyroscope
         temperature data.
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
        This TLV must be echoed back in the Time Sync Inject request. */
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
  /**<   Whether the client is to start or stop sending an SPI status stream.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- Client is to start sending an SPI status stream
       \item    0x00 (FALSE) -- Client is to stop sending an SPI status stream
       \vspace{-0.18in} \end{itemize1}*/
}qmiLocEventSetSpiStreamingReportIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCWWANTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_WWAN_TYPE_INTERNET_V02 = 0, /**<  Bring up the WWAN type used for an Internet connection.  */
  eQMI_LOC_WWAN_TYPE_AGNSS_V02 = 1, /**<  Bring up the WWAN type used for AGNSS connections.  */
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
  eQMI_LOC_SERVER_REQUEST_OPEN_V02 = 1, /**<  Open a connection to the location server.  */
  eQMI_LOC_SERVER_REQUEST_CLOSE_V02 = 2, /**<  Close a connection to the location server.  */
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
  /**<   Identifies a connection across Open and Close request events. */

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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- WWAN_TYPE_INTERNET
       \item    0x00000001 -- WWAN_TYPE_AGNSS
       \vspace{-0.18in} \end{itemize1}
   */
}qmiLocEventLocationServerConnectionReqIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCNIGEOFENCEOPERATIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_NI_GEOFENCE_ADDED_V02 = 1, /**<  An NI Geofence was added.   */
  eQMI_LOC_NI_GEOFENCE_DELETED_V02 = 2, /**<  An NI Geofence was deleted.  */
  eQMI_LOC_NI_GEOFENCE_EDITED_V02 = 3, /**<  An NI Geofence was edited. The control point can query the
       Geofence to find the its current state.  */
  QMILOCNIGEOFENCEOPERATIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocNiGeofenceOperationEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Informs the control point about
                    network-initiated Geofences.  */
typedef struct {

  /* Mandatory */
  /*  Geofence ID  */
  uint32_t geofenceId;
  /**<   ID of the Geofence for which this
       notification was generated. */

  /* Mandatory */
  /*  Operation Type */
  qmiLocNiGeofenceOperationEnumT_v02 operationType;
  /**<   Operation for which this notification was generated.

       Valid values: \begin{itemize1}
       \item    0x00000001 --  NI_GEOFENCE_ADDED
       \item    0x00000002 --  NI_GEOFENCE_DELETED
       \item    0x00000003 --  NI_GEOFENCE_EDITED
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocEventNiGeofenceNotificationIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEGENALERTENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE_V02 = 1, /**<  GNSS is unavailable and GNSS position fixes
       cannot be used to monitor Geofences.   */
  eQMI_LOC_GEOFENCE_GEN_ALERT_GNSS_AVAILABLE_V02 = 2, /**<  GNSS is now available and GNSS postion fixes can
       be used to monitor Geofences.  */
  eQMI_LOC_GEOFENCE_GEN_ALERT_OOS_V02 = 3, /**<  The engine is out of service and no cell ID coverage
       information is available.  */
  eQMI_LOC_GEOFENCE_GEN_ALERT_TIME_INVALID_V02 = 4, /**<  The engine has an invalid time.  */
  QMILOCGEOFENCEGENALERTENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceGenAlertEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of the
                    Geofence status.  */
typedef struct {

  /* Mandatory */
  /*  Geofence General Alert  */
  qmiLocGeofenceGenAlertEnumT_v02 geofenceAlert;
  /**<   Specifies the Geofence general alert type.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_GEN_ALERT_GNSS_UNAVAILABLE
       \item    0x00000002 -- GEOFENCE_GEN_ALERT_GNSS_AVAILABLE
       \item    0x00000003 -- GEOFENCE_GEN_ALERT_OOS
       \item    0x00000004 -- GEOFENCE_GEN_ALERT_TIME_INVALID
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocEventGeofenceGenAlertIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEBREACHTYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_BREACH_TYPE_ENTERING_V02 = 1, /**<  Denotes that a client entered the Geofence.  */
  eQMI_LOC_GEOFENCE_BREACH_TYPE_LEAVING_V02 = 2, /**<  Denotes that a client left the Geofence.  */
  QMILOCGEOFENCEBREACHTYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceBreachTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  /*  UTC Timestamp */
  uint64_t timestampUtc;
  /**<   UTC timestamp.
       \begin{itemize1}
       \item    Units: Milliseconds since Jan. 1, 1970
       \vspace{-0.18in} \end{itemize1} */

  /*  Latitude */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -90.0 to 90.0       \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /*   Longitude */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0     \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /*  Horizontal Elliptical Uncertainty (Semi-Minor Axis) */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty.\n
       - Units: Meters */

  /*  Horizontal Elliptical Uncertainty (Semi-Major Axis) */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty.\n
       - Units: Meters */

  /*  Elliptical Horizontal Uncertainty Azimuth */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation.\n
       - Units: Decimal degrees \n
       - Range: 0 to 180 */

  /*  Horizontal Speed validity bit */
  uint8_t speedHorizontal_valid;
  /**<   Indicates whether the Horizontal speed field contains valid
       information.
       \begin{itemize1}
       \item    0x01 (TRUE)  --  Horizontal speed is valid
       \item    0x00 (FALSE) --  Horizontal speed is invalid
                                 and is to be ignored
       \vspace{-0.18in} \end{itemize1} */

  /*  Horizontal Speed */
  float speedHorizontal;
  /**<   Horizontal speed.\n
       - Units: Meters/second */

  /*  Altitude validity bit */
  uint8_t altitudeWrtEllipsoid_valid;
  /**<   Indicates whether the altitude field contains valid
       information.
       \begin{itemize1}
       \item    0x01 (TRUE)  --  Altitude field is valid
       \item    0x00 (FALSE) --  Altitude field is invalid
                                 and is to be ignored
       \vspace{-0.18in} \end{itemize1}
       */

  /*  Altitude With Respect to Ellipsoid */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units: Meters \n
       - Range: -500 to 15883 */

  /*  Vertical Uncertainty validity bit */
  uint8_t vertUnc_valid;
  /**<   Indicates whether the Vertical Uncertainty field contains valid
       information.
       \begin{itemize1}
       \item    0x01 (TRUE)  --  Vertical Uncertainty field is valid
       \item    0x00 (FALSE) --  Vertical Uncertainty field is invalid
                                 and is to be ignored
       \vspace{-0.18in} \end{itemize1} */

  /*  Vertical Uncertainty */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units: Meters */

  /*  Vertical Speed validity bit */
  uint8_t speedVertical_valid;
  /**<   Indicates whether the Vertical Speed field contains valid
       information.
       \begin{itemize1}
       \item    0x01 (TRUE)  --  Vertical Speed field is valid
       \item    0x00 (FALSE) --  Vertical Speed field is invalid
                                 and is to be ignored
       \vspace{-0.18in} \end{itemize1} */

  /*  Vertical Speed */
  float speedVertical;
  /**<   Vertical speed.\n
       - Units: Meters/second */

  /*  heading validity bit */
  uint8_t heading_valid;
  /**<   Indicates whether the Heading field contains valid
       information.
       \begin{itemize1}
       \item    0x01 (TRUE)  --  Heading field is valid
       \item    0x00 (FALSE) --  Heading field is invalid
                                 and is to be ignored
       \vspace{-0.18in} \end{itemize1} */

  /*  Heading */
  float heading;
  /**<   Heading.\n
        - Units: Degrees \n
        - Range: 0 to 359.999  */
}qmiLocGeofencePositionStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Notifies the control point of
                    a Geofence breach event.  */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   ID of the Geofence for which this
       notification was generated. */

  /* Mandatory */
  /*  Geofence Breach Type */
  qmiLocGeofenceBreachTypeEnumT_v02 breachType;
  /**<   The type of breach that generated this event.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_BREACH_TYPE_ENTERING
       \item    0x00000002 -- GEOFENCE_BREACH_TYPE_LEAVING
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Geofence Position */
  uint8_t geofencePosition_valid;  /**< Must be set to true if geofencePosition is being passed */
  qmiLocGeofencePositionStructT_v02 geofencePosition;
  /**<   \n Position of the client when it breached the Geofence.
       This TLV is included if the client configures the
       Geofence to report position. The position is reported
       at the same confidence level that was specified in the
       Add Circular Geofence request.  */
}qmiLocEventGeofenceBreachIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Recommends how pedometer reports should be
                    sent to the location engine  */
typedef struct {

  /* Mandatory */
  /*  Request Pedometer Data */
  uint8_t requestPedometerData;
  /**<   \n Whether GNSS location engine is requesting the client to
          send pedometer data.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- GNSS location engine is requesting
                                pedometer data
       \item    0x00 (FALSE) -- GNSS location engine is not requesting
                                pedometer data
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Reset Step Count */
  uint8_t resetStepCount_valid;  /**< Must be set to true if resetStepCount is being passed */
  uint8_t resetStepCount;
  /**<   Whether location engine desires the step count to be reset.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- Pedometer step count should be reset.
       \item    0x00 (FALSE) -- Pedometer step count should not be reset.
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Step Count Threshold */
  uint8_t stepCountThreshold_valid;  /**< Must be set to true if stepCountThreshold is being passed */
  uint32_t stepCountThreshold;
  /**<   Specifies the number of steps to be sampled in a pedometer report
       as recommended by the the location engine. If the threshold is set to 0
       then the location engine desires a pedometer report at every step event.
  */
}qmiLocEventPedometerControlIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Recommends how motion data reports should be
                    sent to the location engine   */
typedef struct {

  /* Mandatory */
  /*  Request Motion Data */
  uint8_t requestMotionData;
  /**<   \n Whether GNSS location engine is requesting the client to
          send motion data.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- GNSS location engine is requesting
                                motion data
       \item    0x00 (FALSE) -- GNSS location engine is not requesting
                                motion data
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocEventMotionDataControlIndMsgT_v02;  /* Message */
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
  eQMI_LOC_UNSUPPORTED_V02 = 2, /**<  Request failed because it is not supported.  */
  eQMI_LOC_INVALID_PARAMETER_V02 = 3, /**<  Request failed because it contained invalid parameters.    */
  eQMI_LOC_ENGINE_BUSY_V02 = 4, /**<  Request failed because the engine is busy.  */
  eQMI_LOC_PHONE_OFFLINE_V02 = 5, /**<  Request failed because the phone is offline.  */
  eQMI_LOC_TIMEOUT_V02 = 6, /**<  Request failed because it timed out.  */
  eQMI_LOC_CONFIG_NOT_SUPPORTED_V02 = 7, /**<  Request failed because an undefined configuration was requested  */
  eQMI_LOC_INSUFFICIENT_MEMORY_V02 = 8, /**<  Request failed because the engine could not allocate sufficent
       memory for the request.  */
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

        Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Mandatory */
  /*  Interface Definition Minor Revision */
  uint32_t revision;
  /**<   Revision of the service. This is the minor revision of the interface that
       the service implements. Minor revision updates of the service are always
       backward compatible. */

  /* Optional */
  /*  GNSS Measurement Engine Firmware Version String */
  uint8_t gnssMeFWVerString_valid;  /**< Must be set to true if gnssMeFWVerString is being passed */
  char gnssMeFWVerString[QMI_LOC_GNSS_ME_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Version of the GNSS measurement engine software running under the LOC API.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 128
       \vspace{0.1in} \end{itemize1}

       \textbf{Note:} This string is only provided on platforms that have
       a measurement engine that supports this version string. On all other
       platforms, this optional TLV is not provided. */

  /* Optional */
  /*  GNSS Hosted Software Version String */
  uint8_t gnssHostSWVerString_valid;  /**< Must be set to true if gnssHostSWVerString is being passed */
  char gnssHostSWVerString[QMI_LOC_GNSS_HOSTED_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Version of the GNSS hosted software running under the LOC API.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 128
       \vspace{0.1in}\end{itemize1}

       \textbf{Note:} This string is only provided on hosted architectures
       (measurement and position engine running on different processors) that
       support this version string. On all other platforms, this optional TLV
       is not provided. */

  /* Optional */
  /*  GNSS Software Version String */
  uint8_t gnssSWVerString_valid;  /**< Must be set to true if gnssSWVerString is being passed */
  char gnssSWVerString[QMI_LOC_GNSS_SW_VERSION_STRING_MAX_LENGTH_V02 + 1];
  /**<   Aggregate version of the GNSS software.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 256
       \vspace{-0.18in} \end{itemize1} */
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

        Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Horizontal Accuracy */
  uint8_t horizontalAccuracyLevel_valid;  /**< Must be set to true if horizontalAccuracyLevel is being passed */
  qmiLocAccuracyLevelEnumT_v02 horizontalAccuracyLevel;
  /**<   Horizontal accuracy level.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- LOW: Client requires low horizontal accuracy
       \item    0x00000002 -- MED: Client requires medium horizontal accuracy
       \item    0x00000003 -- HIGH: Client requires high horizontal accuracy
       \vspace{-0.18in} \end{itemize1}
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
       set to IN_PROGRESS for intermediate reports.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- ON: Client is interested in receiving intermediate
                              reports
       \item    0x00000002 -- OFF: Client is not interested in receiving
                              intermediate reports
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Minimum Interval Between Fixes */
  uint8_t minInterval_valid;  /**< Must be set to true if minInterval is being passed */
  uint32_t minInterval;
  /**<   Time that must elapse before alerting the client. \n
       - Units: Milliseconds */

  /* Optional */
  /*  ID of the Application that Sent the Position Request */
  uint8_t applicationId_valid;  /**< Must be set to true if applicationId is being passed */
  qmiLocApplicationIdStructT_v02 applicationId;
  /**<   \n Application provider, name, and version.*/
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
  eQMI_LOC_NI_LCS_NOTIFY_VERIFY_DENY_V02 = 2, /**<  User denied notify verify request.  */
  eQMI_LOC_NI_LCS_NOTIFY_VERIFY_NORESP_V02 = 3, /**<  User did not respond to notify verify request.  */
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

       Valid values: \begin{itemize1}
       \item    0x00000001 -- NOTIFY_VERIFY_ACCEPT
       \item    0x00000002 -- NOTIFY_VERIFY_DENY
       \item    0x00000003 -- NOTIFY_VERIFY_NORESP
       \vspace{-0.18in} \end{itemize1}
  */

  /* Mandatory */
  /*  Notification Type */
  qmiLocNiNotifyVerifyEnumT_v02 notificationType;
  /**<   Type of notification/verification performed.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- NO_NOTIFY_NO_VERIF
       \item    0x00000002 -- NOTIFY_ONLY
       \item    0x00000003 -- ALLOW_NO_RESP
       \item    0x00000004 -- NOT_ALLOW_NO_RESP
       \item    0x00000005 -- PRIVACY_OVERRIDE
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Network Initiated Vx Request */
  uint8_t NiVxPayload_valid;  /**< Must be set to true if NiVxPayload is being passed */
  qmiLocNiVxNotifyVerifyStructT_v02 NiVxPayload;
  /**<   \n Optional NI VX request payload. */

  /* Optional */
  /*  Network Initiated SUPL Request */
  uint8_t NiSuplPayload_valid;  /**< Must be set to true if NiSuplPayload is being passed */
  qmiLocNiSuplNotifyVerifyStructT_v02 NiSuplPayload;
  /**<   \n Optional NI SUPL request payload. */

  /* Optional */
  /*  Network Initiated UMTS Control Plane Request */
  uint8_t NiUmtsCpPayload_valid;  /**< Must be set to true if NiUmtsCpPayload is being passed */
  qmiLocNiUmtsCpNotifyVerifyStructT_v02 NiUmtsCpPayload;
  /**<   \n Optional NI UMTS-CP request payload. */

  /* Optional */
  /*  Network Initiated Service Interaction Request */
  uint8_t NiVxServiceInteractionPayload_valid;  /**< Must be set to true if NiVxServiceInteractionPayload is being passed */
  qmiLocNiVxServiceInteractionStructT_v02 NiVxServiceInteractionPayload;
  /**<   \n Optional NI service interaction payload.  */

  /* Optional */
  /*  Network Initiated SUPL Version 2 Extension */
  uint8_t NiSuplVer2ExtPayload_valid;  /**< Must be set to true if NiSuplVer2ExtPayload is being passed */
  qmiLocNiSuplVer2ExtStructT_v02 NiSuplVer2ExtPayload;
  /**<   \n Optional SUPL Version 2 Extension payload.       */

  /* Optional */
  /*  SUPL Emergency Notification */
  uint8_t suplEmergencyNotification_valid;  /**< Must be set to true if suplEmergencyNotification is being passed */
  qmiLocEmergencyNotificationStructT_v02 suplEmergencyNotification;
  /**<    SUPL Emergency notification payload. Emergency notification
        can be given even without an ESLP address  */
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

          Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
        - Units: Bytes */

  /* Mandatory */
  /*  Total Parts */
  uint16_t totalParts;
  /**<   Total number of parts into which the predicted orbits data is
        divided. */

  /* Mandatory */
  /*  Part Number */
  uint16_t partNum;
  /**<   Number of the current predicted orbits data part; starts at 1. */

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

        Valid values: \begin{itemize1}
        \item    0x00000000 -- PREDICTED_ORBITS_XTRA
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Part Number */
  uint8_t partNum_valid;  /**< Must be set to true if partNum is being passed */
  uint16_t partNum;
  /**<   Number of the predicted orbits data part for which this indication
      is sent; starts at 1. */
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
  /*  Predicted Orbits Data Source Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the query request for a predicted orbits data source.

         Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Allowed Sizes */
  uint8_t allowedSizes_valid;  /**< Must be set to true if allowedSizes is being passed */
  qmiLocPredictedOrbitsAllowedSizesStructT_v02 allowedSizes;
  /**<   \n Maximum part and file size allowed to be injected in the engine. */

  /* Optional */
  /*  Server List */
  uint8_t serverList_valid;  /**< Must be set to true if serverList is being passed */
  qmiLocPredictedOrbitsServerListStructT_v02 serverList;
  /**<   \n List of servers that can be used by the client to download
       predicted orbits data. */
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
       - Units: Seconds (since Jan. 1, 1970)
        */

  uint16_t durationHours;
  /**<   Duration from the start time for which the data is valid.\n
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
       - Units: Milliseconds */

  /* Mandatory */
  /*  Time Uncertainty */
  uint32_t timeUnc;
  /**<   Time uncertainty.\n
       - Units: Milliseconds */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_ALT_SRC_BAROMETRIC_ALTIMETER_V02 = 8, /**<  Barometric altimeter is the source.  */
  eQMI_LOC_ALT_SRC_OTHER_V02 = 9, /**<  Other sources.  */
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
  eQMI_LOC_ALT_SRC_LINKAGE_DEPENDS_ON_LAT_LONG_V02 = 2, /**<  Depends on latitude and longitude.  */
  eQMI_LOC_ALT_SRC_LINKAGE_FULLY_INDEPENDENT_V02 = 3, /**<  Fully independent.  */
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
       position coordinates only.  */
  eQMI_LOC_ALT_UNCERTAINTY_FULL_V02 = 2, /**<  Altitude uncertainty applies to the position of the device
       regardless of horizontal position (within the horizontal
       uncertainty region, if provided).  */
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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- ALT_SRC_UNKNOWN
       \item    0x00000001 -- ALT_SRC_GPS
       \item    0x00000002 -- ALT_SRC_CELL_ID
       \item    0x00000003 -- ALT_SRC_ENHANCED_CELL_ID
       \item    0x00000004 -- ALT_SRC_WIFI
       \item    0x00000005 -- ALT_SRC_TERRESTRIAL
       \item    0x00000006 -- ALT_SRC_TERRESTRIAL_HYBRID
       \item    0x00000007 -- ALT_SRC_ALTITUDE_DATABASE
       \item    0x00000008 -- ALT_SRC_BAROMETRIC_ALTIMETER
       \item    0x00000009 -- ALT_SRC_OTHER
       \vspace{-0.18in} \end{itemize1}
  */

  qmiLocAltSrcLinkageEnumT_v02 linkage;
  /**<   Specifies the dependency between the horizontal and
       altitude position components.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- SRC_LINKAGE_NOT_SPECIFIED
       \item    0x00000001 -- SRC_LINKAGE_FULLY_INTERDEPENDENT
       \item    0x00000002 -- SRC_LINKAGE_DEPENDS_ON_LAT_LONG
       \item    0x00000003 -- SRC_LINKAGE_FULLY_INDEPENDENT
       \vspace{-0.18in} \end{itemize1}
  */

  qmiLocAltSrcUncertaintyCoverageEnumT_v02 coverage;
  /**<   Specifies the region of uncertainty.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- UNCERTAINTY_NOT_SPECIFIED
        \item    0x00000001 -- UNCERTAINTY_POINT: Altitude uncertainty is valid
                               at the injected horizontal position coordinates
                               only.
        \item   0x00000002 -- UNCERTAINTY_FULL: Altitude uncertainty applies to
                              the position of the device regardless of
                              horizontal position (within the horizontal
                              uncertainty region, if provided).
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_POSITION_SRC_GNSS_V02 = 0, /**<  Position source is GNSS.  */
  eQMI_LOC_POSITION_SRC_CELLID_V02 = 1, /**<  Position source is Cell ID.  */
  eQMI_LOC_POSITION_SRC_ENH_CELLID_V02 = 2, /**<  Position source is Enhanced Cell ID.  */
  eQMI_LOC_POSITION_SRC_WIFI_V02 = 3, /**<  Position source is WiFi.  */
  eQMI_LOC_POSITION_SRC_TERRESTRIAL_V02 = 4, /**<  Position source is Terrestrial.  */
  eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID_V02 = 5, /**<  Position source is GNSS Terrestrial Hybrid.  */
  eQMI_LOC_POSITION_SRC_OTHER_V02 = 6, /**<  Other sources.   */
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
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -90.0 to 90.0       \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1}
    */

  /* Optional */
  /*  Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0     \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1}
   */

  /* Optional */
  /*  Circular Horizontal Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
        - Units: Meters */

  /* Optional */
  /*  Horizontal Confidence */
  uint8_t horConfidence_valid;  /**< Must be set to true if horConfidence is being passed */
  uint8_t horConfidence;
  /**<   Horizontal confidence, as defined by ETSI TS 101 109 (\hyperref[S4]{[S4]}).
        \begin{itemize1}
        \item    Units: Percent (0 to 99)
        \item    0 -- invalid value
        \item    100 to 256 -- not used
        \item    If 100 is received, reinterpret to 99
        \end{itemize1}
        This field must be specified together with horizontal uncertainty.
        If not specified, the default value will be 50. */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET
       \item    0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW
       \item    0x00000002 -- eQMI_LOC_RELIABILITY_LOW
       \item    0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM
       \item    0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
       \vspace{-0.18in} \end{itemize1}
      */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.
        \begin{itemize1}
        \item    Units: Meters    \begin{itemize1}
          \item    Positive = height
          \item    Negative = depth
        \vspace{-0.18in} \end{itemize1} \end{itemize1}*/

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level.\n
       - Units: Meters */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty. This is mandatory if either altitudeWrtEllipsoid
        or altitudeWrtMeanSeaLevel is specified.\n
        - Units: Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical confidence, as defined by  ETSI TS 101 109 (\hyperref[S4]{[S4]}).
        \begin{itemize1}
        \item    Units: Percent (0-99)
        \item    0 -- invalid value
        \item    100 to 256 -- not used
        \item    If 100 is received, reinterpret to 99
        \end{itemize1}
        This field must be specified together with the vertical uncertainty.
        If not specified, the default value will be 50. */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET
        \item    0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW
        \item    0x00000002 -- eQMI_LOC_RELIABILITY_LOW
        \item    0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM
        \item    0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
        \vspace{-0.18in} \end{itemize1}
    */

  /* Optional */
  /*  Altitude Source Info */
  uint8_t altSourceInfo_valid;  /**< Must be set to true if altSourceInfo is being passed */
  qmiLocAltitudeSrcInfoStructT_v02 altSourceInfo;
  /**<   \n Specifies information regarding the altitude source. */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp. \n
        - Units: Milliseconds (since Jan. 1, 1970) */

  /* Optional */
  /*  Position Age */
  uint8_t timestampAge_valid;  /**< Must be set to true if timestampAge is being passed */
  int32_t timestampAge;
  /**<   Position age, which is an estimate of how long ago this fix was made. \n
        - Units: Milliseconds */

  /* Optional */
  /*  Position Source */
  uint8_t positionSrc_valid;  /**< Must be set to true if positionSrc is being passed */
  qmiLocPositionSrcEnumT_v02 positionSrc;
  /**<   Source from which this position was obtained.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- eQMI_LOC_POSITION_SRC_GNSS
        \item    0x00000001 -- eQMI_LOC_POSITION_SRC_CELLID
        \item    0x00000002 -- eQMI_LOC_POSITION_SRC_ENH_CELLID
        \item    0x00000003 -- eQMI_LOC_POSITION_SRC_WIFI
        \item    0x00000004 -- eQMI_LOC_POSITION_SRC_TERRESTRIAL
        \item    0x00000005 -- eQMI_LOC_POSITION_SRC_GNSS_TERRESTRIAL_HYBRID
        \item    0x00000006 -- eQMI_LOC_POSITION_SRC_OTHER
        \end{itemize1} \vspace{0.05in}

       If altitude is specified and the altitude source is not specified, the engine
       assumes that the altitude was obtained using the specified position source. \n
       If both altitude and altitude source are specified, the engine assumes
       that only latitude and longitude were obtained using the specified position
       source.
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_LOCK_MT_V02 = 3, /**<  Lock mobile-terminated position sessions.  */
  eQMI_LOC_LOCK_ALL_V02 = 4, /**<  Lock all position sessions.  */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  /**<   Whether SBAS configuration is enabled.
       \begin{itemize1}
       \item    0x01 (TRUE) -- SBAS configuration is enabled
       \item    0x00 (FALSE) -- SBAS configuration is disabled
       \vspace{-0.18in} \end{itemize1}*/
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  SBAS Config */
  uint8_t sbasConfig_valid;  /**< Must be set to true if sbasConfig is being passed */
  uint8_t sbasConfig;
  /**<   Whether SBAS configuration is enabled.
       \begin{itemize1}
       \item    0x01 (TRUE) -- SBAS configuration is enabled
       \item    0x00 (FALSE) -- SBAS configuration is disabled
       \vspace{-0.18in} \end{itemize1}*/
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
#define QMI_LOC_NMEA_MASK_PQXFI_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000020) /**<  Enable PQXFI type.  */
#define QMI_LOC_NMEA_MASK_PSTIS_V02 ((qmiLocNmeaSentenceMaskT_v02)0x00000040) /**<  Enable PSTIS type.  */
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
         - 0x00000010 -- NMEA_MASK_VTG \n
         - 0x00000020 -- NMEA_MASK_PQXFI \n
         - 0x00000040 -- NMEA_MASK_PSTIS
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
         - 0x00000010 -- NMEA_MASK_VTG \n
         - 0x00000020 -- NMEA_MASK_PQXFI \n
         - 0x00000040 -- NMEA_MASK_PSTIS
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
       - 0x00 (FALSE) -- Disable LPM */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Enable/Disable LPM */
  uint8_t lowPowerMode_valid;  /**< Must be set to true if lowPowerMode is being passed */
  uint8_t lowPowerMode;
  /**<   Whether to enable Low Power mode:\n
       - 0x01 (TRUE) -- Enable LPM \n
       - 0x00 (FALSE) -- Disable LPM */
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
  eQMI_LOC_SERVER_TYPE_UMTS_SLP_V02 = 3, /**<  Server type is UMTS SLP.  */
  eQMI_LOC_SERVER_TYPE_CUSTOM_PDE_V02 = 4, /**<  Server type is custom PDE.  */
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
  /**<   \n IPV4 address and port. */

  /* Optional */
  /*  IPV6 Address */
  uint8_t ipv6Addr_valid;  /**< Must be set to true if ipv6Addr is being passed */
  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   \n IPV6 address and port. */

  /* Optional */
  /*  Uniform Resource Locator */
  uint8_t urlAddr_valid;  /**< Must be set to true if urlAddr is being passed */
  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL address.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 256
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  /**<   \n IPV4 address and port. */

  /* Optional */
  /*  IPV6 Address */
  uint8_t ipv6Addr_valid;  /**< Must be set to true if ipv6Addr is being passed */
  qmiLocIpV6AddrStructType_v02 ipv6Addr;
  /**<   \n IPV6 address and port. */

  /* Optional */
  /*  Uniform Resource Locator */
  uint8_t urlAddr_valid;  /**< Must be set to true if urlAddr is being passed */
  char urlAddr[QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02 + 1];
  /**<   URL.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 256
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocGetServerIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocDeleteGnssDataMaskT_v02;
#define QMI_LOC_MASK_DELETE_GPS_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000001ull) /**<  Mask to delete GPS SVDIR.   */
#define QMI_LOC_MASK_DELETE_GPS_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000002ull) /**<  Mask to delete GPS SVSTEER.   */
#define QMI_LOC_MASK_DELETE_GPS_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000004ull) /**<  Mask to delete GPS time.  */
#define QMI_LOC_MASK_DELETE_GPS_ALM_CORR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000008ull) /**<  Mask to delete almanac correlation.  */
#define QMI_LOC_MASK_DELETE_GLO_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000010ull) /**<  Mask to delete GLONASS SVDIR.   */
#define QMI_LOC_MASK_DELETE_GLO_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000020ull) /**<  Mask to delete GLONASS SVSTEER.   */
#define QMI_LOC_MASK_DELETE_GLO_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000040ull) /**<  Mask to delete GLONASS time.  */
#define QMI_LOC_MASK_DELETE_GLO_ALM_CORR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000080ull) /**<  Mask to delete GLONASS almanac correlation */
#define QMI_LOC_MASK_DELETE_SBAS_SVDIR_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000100ull) /**<  Mask to delete SBAS SVDIR  */
#define QMI_LOC_MASK_DELETE_SBAS_SVSTEER_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000200ull) /**<  Mask to delete SBAS SVSTEER  */
#define QMI_LOC_MASK_DELETE_POSITION_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000400ull) /**<  Mask to delete position estimate  */
#define QMI_LOC_MASK_DELETE_TIME_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00000800ull) /**<  Mask to delete time estimate  */
#define QMI_LOC_MASK_DELETE_IONO_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00001000ull) /**<  Mask to delete IONO  */
#define QMI_LOC_MASK_DELETE_UTC_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00002000ull) /**<  Mask to delete UTC estimate  */
#define QMI_LOC_MASK_DELETE_HEALTH_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00004000ull) /**<  Mask to delete SV health record  */
#define QMI_LOC_MASK_DELETE_SADATA_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00008000ull) /**<  Mask to delete SADATA  */
#define QMI_LOC_MASK_DELETE_RTI_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00010000ull) /**<  Mask to delete RTI  */
#define QMI_LOC_MASK_DELETE_SV_NO_EXIST_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00020000ull) /**<  Mask to delete SV_NO_EXIST  */
#define QMI_LOC_MASK_DELETE_FREQ_BIAS_EST_V02 ((qmiLocDeleteGnssDataMaskT_v02)0x00040000ull) /**<  Mask to delete frequency bias estimate  */
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
  /**<   SV ID of the satellite whose data is to be deleted.
       \begin{itemize1}
       \item    Range:    \begin{itemize1}
         \item    For GPS:     1 to 32
         \item    For SBAS:    33 to 64
         \item    For GLONASS: 65 to 96
       \vspace{-0.18in} \end{itemize1} \end{itemize1}  */

  qmiLocSvSystemEnumT_v02 system;
  /**<   Indicates to which constellation this SV belongs.

         Valid values: \begin{itemize1}
         \item    0x00000001 -- eQMI_LOC_SV_SYSTEM_GPS
         \item    0x00000002 -- eQMI_LOC_SV_SYSTEM_GALILEO
         \item    0x00000003 -- eQMI_LOC_SV_SYSTEM_SBAS
         \item    0x00000004 -- eQMI_LOC_SV_SYSTEM_COMPASS
         \item    0x00000005 -- eQMI_LOC_SV_SYSTEM_GLONASS
         \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE)  -- All assistance data is to be deleted; if
                                this flag is set, all the other information
                         contained in the optional fields for this
                                message are ignored
       \item    0x00 (FALSE) -- The optional fields in the message are to be
                                used to determine which data is to be deleted
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Delete SV Info */
  uint8_t deleteSvInfoList_valid;  /**< Must be set to true if deleteSvInfoList is being passed */
  uint32_t deleteSvInfoList_len;  /**< Must be set to # of elements in deleteSvInfoList */
  qmiLocDeleteSvInfoStructT_v02 deleteSvInfoList[QMI_LOC_DELETE_MAX_SV_INFO_LENGTH_V02];
  /**<   \n List of satellites for which the assistance data is to be deleted.
   */

  /* Optional */
  /*  Delete GNSS Data */
  uint8_t deleteGnssDataMask_valid;  /**< Must be set to true if deleteGnssDataMask is being passed */
  qmiLocDeleteGnssDataMaskT_v02 deleteGnssDataMask;
  /**<   Mask for the GNSS data that is to be deleted.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- DELETE_GPS_SVDIR
       \item    0x00000002 -- DELETE_GPS_SVSTEER
       \item    0x00000004 -- DELETE_GPS_TIME
       \item    0x00000008 -- DELETE_GPS_ALM_CORR
       \item    0x00000010 -- DELETE_GLO_SVDIR
       \item    0x00000020 -- DELETE_GLO_SVSTEER
       \item    0x00000040 -- DELETE_GLO_TIME
       \item    0x00000080 -- DELETE_GLO_ALM_CORR
       \item    0x00000100 -- DELETE_SBAS_SVDIR
       \item    0x00000200 -- DELETE_SBAS_SVSTEER
       \item    0x00000400 -- DELETE_POSITION
       \item    0x00000800 -- DELETE_TIME
       \item    0x00001000 -- DELETE_IONO
       \item    0x00002000 -- DELETE_UTC
       \item    0x00004000 -- DELETE_HEALTH
       \item    0x00008000 -- DELETE_SADATA
       \item    0x00010000 -- DELETE_RTI
       \item    0x00020000 -- DELETE_SV_NO_EXIST
       \item    0x00040000 -- DELETE_FREQ_BIAS_EST
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Delete Cell Database */
  uint8_t deleteCellDbDataMask_valid;  /**< Must be set to true if deleteCellDbDataMask is being passed */
  qmiLocDeleteCelldbDataMaskT_v02 deleteCellDbDataMask;
  /**<   Mask for the cell database assistance data that is to be deleted.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- DELETE_CELLDB_POS
       \item    0x00000002 -- DELETE_CELLDB_LATEST_GPS_POS
       \item    0x00000004 -- DELETE_CELLDB_OTA_POS
       \item    0x00000008 -- DELETE_CELLDB_EXT_REF_POS
       \item    0x00000010 -- DELETE_CELLDB_TIMETAG
       \item    0x00000020 -- DELETE_CELLDB_CELLID
       \item    0x00000040 -- DELETE_CELLDB_CACHED_CELLID
       \item    0x00000080 -- DELETE_CELLDB_LAST_SRV_CELL
       \item    0x00000100 -- DELETE_CELLDB_CUR_SRV_CELL
       \item    0x00000200 -- DELETE_CELLDB_NEIGHBOR_INFO
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Delete Clock Info */
  uint8_t deleteClockInfoMask_valid;  /**< Must be set to true if deleteClockInfoMask is being passed */
  qmiLocDeleteClockInfoMaskT_v02 deleteClockInfoMask;
  /**<   Mask for the clock information assistance data that is to be deleted.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- DELETE_CLOCK_INFO_TIME_EST
       \item    0x00000002 -- DELETE_CLOCK_INFO_FREQ_EST
       \item    0x00000004 -- DELETE_CLOCK_INFO_WEEK_NUMBER
       \item    0x00000008 -- DELETE_CLOCK_INFO_RTC_TIME
       \item    0x00000010 -- DELETE_CLOCK_INFO_TIME_TRANSFER
       \item    0x00000020 -- DELETE_CLOCK_INFO_GPSTIME_EST
       \item    0x00000040 -- DELETE_CLOCK_INFO_GLOTIME_EST
       \item    0x00000080 -- DELETE_CLOCK_INFO_GLODAY_NUMBER
       \item    0x00000100 -- DELETE_CLOCK_INFO_GLO4YEAR_NUMBER
       \item    0x00000200 -- DELETE_CLOCK_INFO_GLO_RF_GRP_DELAY
       \item    0x00000400 -- DELETE_CLOCK_INFO_DISABLE_TT
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
       - 0x00 (FALSE) -- Disable XTRA-T */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Enable/Disable XTRA-T */
  uint8_t xtraTSessionControl_valid;  /**< Must be set to true if xtraTSessionControl is being passed */
  uint8_t xtraTSessionControl;
  /**<   Whether to enable XTRA-T:\n
       - 0x01 (TRUE) -- Enable XTRA-T \n
       - 0x00 (FALSE) -- Disable XTRA-T */
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
        synchronized. */
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
   This may be because the APs found are not in the database.  */
  eQMI_LOC_WIFI_FIX_ERROR_UNKNOWN_V02 = 6, /**<  WiFi fix failed, but the cause could not be determined.  */
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
        - Units: Degrees */

  double lon;
  /**<   WiFi position longitude. \n
        - Type: Floating point \n
        - Units: Degrees */

  uint16_t hepe;
  /**<   WiFi position HEPE.\n
        - Units: Meters */

  uint8_t numApsUsed;
  /**<   Number of Access Points (AP) used to generate a fix. */

  qmiLocWifiFixErrorCodeEnumT_v02 fixErrorCode;
  /**<   WiFi position error code; set to 0 if the fix succeeds. This position
        is only used by a module if the value is 0. If there was a failure,
        the error code provided by the WiFi positioning system can be provided
        here.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- ERROR_SUCCESS
        \item    0x00000001 -- ERROR_WIFI_NOT_AVAILABLE
        \item    0x00000002 -- ERROR_NO_AP_FOUND
        \item    0x00000003 -- ERROR_UNAUTHORIZED
        \item    0x00000004 -- ERROR_SERVER_UNAVAILABLE
        \item    0x00000005 -- ERROR_LOCATION_CANNOT_BE_DETERMINED
        \item    0x00000006 -- ERROR_UNKNOWN
        \vspace{-0.18in} \end{itemize1}
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
        - Units: dBm (offset with +100 dB) */

  uint16_t channel;
  /**<   WiFi channel on which a beacon was received. */

  qmiLocWifiApQualifierMaskT_v02 apQualifier;
  /**<   A bitmask of Boolean qualifiers for APs.
        All unused bits in this mask must be set to 0.

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
  /**<   \n Time of WiFi position fix. */

  /* Optional */
  /*  WiFi Position */
  uint8_t wifiFixPosition_valid;  /**< Must be set to true if wifiFixPosition is being passed */
  qmiLocWifiFixPosStructT_v02 wifiFixPosition;
  /**<   \n WiFi position fix. */

  /* Optional */
  /*  WiFi Access Point Information */
  uint8_t apInfo_valid;  /**< Must be set to true if apInfo is being passed */
  uint32_t apInfo_len;  /**< Must be set to # of elements in apInfo */
  qmiLocWifiApInfoStructT_v02 apInfo[QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02];
  /**<   \n AP scan list. */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horizontalReliability_valid;  /**< Must be set to true if horizontalReliability is being passed */
  qmiLocReliabilityEnumT_v02 horizontalReliability;
  /**<   Specifies the reliability of the horizontal position.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- eQMI_LOC_RELIABILITY_NOT_SET
        \item    0x00000001 -- eQMI_LOC_RELIABILITY_VERY_LOW
        \item    0x00000002 -- eQMI_LOC_RELIABILITY_LOW
        \item    0x00000003 -- eQMI_LOC_RELIABILITY_MEDIUM
        \item    0x00000004 -- eQMI_LOC_RELIABILITY_HIGH
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_WIFI_STATUS_AVAILABLE_V02 = 1, /**<  WiFi is available. */
  eQMI_LOC_WIFI_STATUS_UNAVAILABLE_V02 = 2, /**<  WiFi is not available.  */
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

        Valid values: \begin{itemize1}
        \item    0x00000001 -- WIFI_STATUS_AVAILABLE
        \item    0x00000002 -- WIFI_STATUS_UNAVAILABLE
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Event Registration Mask */
  uint8_t eventRegMask_valid;  /**< Must be set to true if eventRegMask is being passed */
  qmiLocEventRegMaskT_v02 eventRegMask;
  /**<   Event registration mask.
 Valid bitmasks: \n
      - QMI_LOC_EVENT_MASK_POSITION_REPORT (0x00000001) --  The control point must enable this mask to receive position report
       event indications.
      - QMI_LOC_EVENT_MASK_GNSS_SV_INFO (0x00000002) --  The control point must enable this mask to receive satellite report
       event indications. These reports are sent at a 1 Hz rate.
      - QMI_LOC_EVENT_MASK_NMEA (0x00000004) --  The control point must enable this mask to receive NMEA reports for
       position and satellites in view. The report is at a 1 Hz rate.
      - QMI_LOC_EVENT_MASK_NI_NOTIFY_VERIFY_REQ (0x00000008) --  The control point must enable this mask to receive NI notify verify request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_TIME_REQ (0x00000010) --  The control point must enable this mask to receive time injection request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_PREDICTED_ORBITS_REQ (0x00000020) --  The control point must enable this mask to receive predicted orbits request
       event indications.
      - QMI_LOC_EVENT_MASK_INJECT_POSITION_REQ (0x00000040) --  The control point must enable this mask to receive position injection request
       event indications.
      - QMI_LOC_EVENT_MASK_ENGINE_STATE (0x00000080) --  The control point must enable this mask to receive engine state report
       event indications.
      - QMI_LOC_EVENT_MASK_FIX_SESSION_STATE (0x00000100) --  The control point must enable this mask to receive fix session status report
       event indications.
      - QMI_LOC_EVENT_MASK_WIFI_REQ (0x00000200) --  The control point must enable this mask to receive WiFi position request
       event indications.
      - QMI_LOC_EVENT_MASK_SENSOR_STREAMING_READY_STATUS (0x00000400) --  The control point must enable this mask to receive notifications from the
       GPS engine indicating its readiness to accept data from the
       sensors (accelerometer, gyroscope, etc.).
      - QMI_LOC_EVENT_MASK_TIME_SYNC_REQ (0x00000800) --  The control point must enable this mask to receive time-sync requests
       from the GPS engine. Time sync enables the GPS engine to synchronize
       its clock with the sensor processor's clock.
      - QMI_LOC_EVENT_MASK_SET_SPI_STREAMING_REPORT (0x00001000) --  The control point must enable this mask to receive Stationary Position
       Indicator (SPI) streaming report indications.
      - QMI_LOC_EVENT_MASK_LOCATION_SERVER_CONNECTION_REQ (0x00002000) --  The control point must enable this mask to receive location server
       requests. These requests are generated when the service wishes to
       establish a connection with a location server.
      - QMI_LOC_EVENT_MASK_NI_GEOFENCE_NOTIFICATION (0x00004000) --  The control point must enable this mask to receive notifications
       related to network-initiated Geofences. These events notify the client
       when a network-initiated Geofence is added, deleted, or edited.
      - QMI_LOC_EVENT_MASK_GEOFENCE_GEN_ALERT (0x00008000) --  The control point must enable this mask to receive Geofence alerts.
       These alerts are generated to inform the client of the changes that may
       affect Geofence, e.g., if GPS is turned off or if the network is
       unavailable.
      - QMI_LOC_EVENT_MASK_GEOFENCE_BREACH_NOTIFICATION (0x00010000) --  The control point must enable this mask to receive notifications when
       a Geofence is breached. These events are generated when the UE enters
       or leaves the perimeter of a Geofence.
      - QMI_LOC_EVENT_MASK_PEDOMETER_CONTROL (0x00020000) --  The control point must enable this mask to register for Pedometer
       control requests from the location engine. Location engine sends
       this event out to control the injection of pedometer reports.
      - QMI_LOC_EVENT_MASK_MOTION_DATA_CONTROL (0x00040000) --  The control point must enable this mask to register for motion data
       control requests from the location engine. Location engine sends
       this event out to control the injection of motion data.
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
  eQMI_LOC_OPER_MODE_STANDALONE_V02 = 4, /**<  Use Standalone mode.  */
  eQMI_LOC_OPER_MODE_CELL_ID_V02 = 5, /**<  Use cell ID. This mode is valid only for a GSM/UMTS network.  */
  eQMI_LOC_OPER_MODE_WWAN_V02 = 6, /**<  Use WWAN measurements to calculate the position. If this mode is
       set, AFLT will be used for 1x networks and OTDOA will be used
       for LTE networks.                  */
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
       \label{operationMode}

       Valid values: \begin{itemize1}
       \item    0x00000001 -- OPER_MODE_DEFAULT
       \item    0x00000002 -- OPER_MODE_MSB
       \item    0x00000003 -- OPER_MODE_MSA
       \item    0x00000004 -- OPER_MODE_STANDALONE
       \item    0x00000005 -- OPER_MODE_CELL_ID
       \item    0x00000006 -- OPER_MODE_WWAN
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Operation Mode */
  uint8_t operationMode_valid;  /**< Must be set to true if operationMode is being passed */
  qmiLocOperationModeEnumT_v02 operationMode;
  /**<   Current operation mode.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- OPER_MODE_DEFAULT
       \item    0x00000002 -- OPER_MODE_MSB
       \item    0x00000003 -- OPER_MODE_MSA
       \item    0x00000004 -- OPER_MODE_STANDALONE
       \item    0x00000005 -- OPER_MODE_CELL_ID
       \item    0x00000006 -- OPER_MODE_WWAN
       \vspace{-0.18in} \end{itemize1}
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
  /**<   Whether the device is stationary:
       \begin{itemize1}
       \item    0x00 (FALSE) -- Device is not stationary
       \item    0x01 (TRUE)  -- Device is stationary
       \vspace{-0.18in} \end{itemize1}*/

  /* Optional */
  /*  Confidence */
  uint8_t confidenceStationary_valid;  /**< Must be set to true if confidenceStationary is being passed */
  uint8_t confidenceStationary;
  /**<   Confidence in the Stationary state expressed as a percentage.\n
       - Range: 0 to 100 */
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
  /**<   Status of the SPI Status request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */
}qmiLocSetSpiStatusIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint8_t qmiLocSensorDataFlagMaskT_v02;
#define QMI_LOC_SENSOR_DATA_FLAG_SIGN_REVERSAL_V02 ((qmiLocSensorDataFlagMaskT_v02)0x01) /**<  Bitmask to specify that a sign reversal is required while interpreting
     the sensor data. Only applies to the accelerometer samples.  */
#define QMI_LOC_SENSOR_DATA_FLAG_SENSOR_TIME_IS_MODEM_TIME_V02 ((qmiLocSensorDataFlagMaskT_v02)0x02) /**<  Bitmask to specify that the sensor time stamp is the same as the modem
       time stamp.  */
/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSENSORDATATIMESOURCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED_V02 = 0, /**<  The sensor time source is unspecified */
  eQMI_LOC_SENSOR_TIME_SOURCE_COMMON_V02 = 1, /**<  The time source is common between the sensors and
       the location engine */
  QMILOCSENSORDATATIMESOURCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSensorDataTimeSourceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t timeOffset;
  /**<   Sample time offset. This time offset must be
       relative to the timestamp of the first sensor data sample.\n
       - Units: Milliseconds */

  float xAxis;
  /**<   Sensor x-axis sample. \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyroscope:     ( (rads)/(seconds) ) */

  float yAxis;
  /**<   Sensor y-axis sample. \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyroscope:     ( (rads)/(seconds) ) */

  float zAxis;
  /**<   Sensor z-axis sample. \n
       - Units Accelerometer: ( (meters)/(seconds^2) ) \n
       - Units Gyroscope:     ( (rads)/(seconds) ) */
}qmiLoc3AxisSensorSampleStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t timeOfFirstSample;
  /**<   Denotes a full 32-bit timestamp of the first (oldest) sample in this
       message.The timestamp is in the time reference scale that is
       used by the sensor time source.\n
       - Units: Milliseconds */

  qmiLocSensorDataFlagMaskT_v02 flags;
  /**<   Flags to indicate any deviation from the default measurement
       assumptions. All unused bits in this field must be set to 0.

       Valid bitmasks: \begin{itemize1}
       \item    0x01 -- SIGN_REVERSAL
       \item    0X02 -- SENSOR_TIME_IS_MODEM_TIME
       \vspace{-0.18in} \end{itemize1}        */

  uint32_t sensorData_len;  /**< Must be set to # of elements in sensorData */
  qmiLoc3AxisSensorSampleStructT_v02 sensorData[QMI_LOC_SENSOR_DATA_MAX_SAMPLES_V02];
  /**<   Variable length array to specify sensor samples. \n
       - Maximum length of the array: 50                             */
}qmiLoc3AxisSensorSampleListStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint16_t timeOffset;
  /**<   Sample time offset. This time offset must be
       relative to the timestamp of the first sensor sample.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds */

  float temperature;
  /**<   Sensor temperature. \n
       - Type: Floating point \n
       - Units: Degree Celsius \n
       - Range: -50 to +100.00 Degree Celsius */
}qmiLocSensorTemperatureSampleStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocSensorDataTimeSourceEnumT_v02 timeSource;
  /**<   Denotes the time source of the sensor data. Location service will use
 this field to identify the time reference used in the
 sensor data timestamps. Values :\n
      - eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED (0) --  The sensor time source is unspecified
      - eQMI_LOC_SENSOR_TIME_SOURCE_COMMON (1) --  The time source is common between the sensors and
       the location engine
 */

  uint32_t timeOfFirstSample;
  /**<   Denotes a full 32-bit timestamp of the first (oldest) sample in this
       message. The timestamp is in the time reference scale that is
       used by the sensor time source.\n
       - Type: Unsigned integer \n
       - Units: Milliseconds */

  uint32_t temperatureData_len;  /**< Must be set to # of elements in temperatureData */
  qmiLocSensorTemperatureSampleStructT_v02 temperatureData[QMI_LOC_SENSOR_DATA_MAX_SAMPLES_V02];
  /**<   Variable length array to specify sensor temperature samples. \n
       - Maximum length of the array: 50             */
}qmiLocSensorTemperatureSampleListStructT_v02;  /* Type */
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
       request. */

  /* Optional */
  /*  3-Axis Accelerometer Data */
  uint8_t threeAxisAccelData_valid;  /**< Must be set to true if threeAxisAccelData is being passed */
  qmiLoc3AxisSensorSampleListStructT_v02 threeAxisAccelData;
  /**<   \n Accelerometer sensor samples. */

  /* Optional */
  /*  3-Axis Gyroscope Data */
  uint8_t threeAxisGyroData_valid;  /**< Must be set to true if threeAxisGyroData is being passed */
  qmiLoc3AxisSensorSampleListStructT_v02 threeAxisGyroData;
  /**<   \n Gyroscope sensor samples. */

  /* Optional */
  /*  3-Axis Accelerometer Data Time Source */
  uint8_t threeAxisAccelDataTimeSource_valid;  /**< Must be set to true if threeAxisAccelDataTimeSource is being passed */
  qmiLocSensorDataTimeSourceEnumT_v02 threeAxisAccelDataTimeSource;
  /**<   Time source for the 3-axis accelerometer data. Location service will use
 this field to identify the time reference used in the accelerometer data
 timestamps. If not specified the location service will assume that the
 time source for the accelereometer data is unknown. Values: \n
      - eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED (0) --  The sensor time source is unspecified
      - eQMI_LOC_SENSOR_TIME_SOURCE_COMMON (1) --  The time source is common between the sensors and
       the location engine
 */

  /* Optional */
  /*  3-Axis Gyroscope Data Time Source */
  uint8_t threeAxisGyroDataTimeSource_valid;  /**< Must be set to true if threeAxisGyroDataTimeSource is being passed */
  qmiLocSensorDataTimeSourceEnumT_v02 threeAxisGyroDataTimeSource;
  /**<   Time source for the 3-axis gyroscope data. Location service will use
 this field to identify the time reference used in the gyroscope data
 timestamps.If not specified the locations ervice will assume that the
 time source for the gyroscope data is unknown. Values: \n
      - eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED (0) --  The sensor time source is unspecified
      - eQMI_LOC_SENSOR_TIME_SOURCE_COMMON (1) --  The time source is common between the sensors and
       the location engine
 */

  /* Optional */
  /*  Accelerometer Temperature Data */
  uint8_t accelTemperatureData_valid;  /**< Must be set to true if accelTemperatureData is being passed */
  qmiLocSensorTemperatureSampleListStructT_v02 accelTemperatureData;
  /**<   Accelerometer temperature samples. This data is optional and does not
       have to be included in the message along with accelerometer data. */

  /* Optional */
  /*  Gyroscope Temperature Data */
  uint8_t gyroTemperatureData_valid;  /**< Must be set to true if gyroTemperatureData is being passed */
  qmiLocSensorTemperatureSampleListStructT_v02 gyroTemperatureData;
  /**<   Gyroscope temperature samples. This data is optional and does not
       have to be included in the message along with gyroscope data. */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Opaque Identifier */
  uint8_t opaqueIdentifier_valid;  /**< Must be set to true if opaqueIdentifier is being passed */
  uint32_t opaqueIdentifier;
  /**<   Opaque identifier that was sent in by the client echoed
       so the client can relate the indication to the request. */

  /* Optional */
  /*  Accelerometer Samples Accepted */
  uint8_t threeAxisAccelSamplesAccepted_valid;  /**< Must be set to true if threeAxisAccelSamplesAccepted is being passed */
  uint8_t threeAxisAccelSamplesAccepted;
  /**<   Lets the client know how many 3-axis accelerometer samples
       were accepted. This field is present only if the accelerometer
       samples were sent in the request. */

  /* Optional */
  /*  Gyroscope Samples Accepted */
  uint8_t threeAxisGyroSamplesAccepted_valid;  /**< Must be set to true if threeAxisGyroSamplesAccepted is being passed */
  uint8_t threeAxisGyroSamplesAccepted;
  /**<   Lets the client know how many 3-axis gyroscope samples were
       accepted. This field is present only if the gyroscope
       samples were sent in the request. */

  /* Optional */
  /*  Accelerometer Samples Accepted */
  uint8_t accelTemperatureSamplesAccepted_valid;  /**< Must be set to true if accelTemperatureSamplesAccepted is being passed */
  uint8_t accelTemperatureSamplesAccepted;
  /**<   This field lets the client know how many accelerometer temperature
       samples were accepted. This field is present only if the accelerometer
       temperature samples were sent in the request. */

  /* Optional */
  /*  Gyroscope Temperature Samples Accepted */
  uint8_t gyroTemperatureSamplesAccepted_valid;  /**< Must be set to true if gyroTemperatureSamplesAccepted is being passed */
  uint8_t gyroTemperatureSamplesAccepted;
  /**<   This field lets the client know how many gyroscope temperature samples
       were accepted. This field is present only if the gyroscope
       temperature samples were sent in the request. */
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
       GNSS location engine requested time sync injection. */

  /* Mandatory */
  /*  Sensor Receive Time */
  uint32_t sensorProcRxTime;
  /**<   Value of the sensor time when the control point received the
       Time Sync Inject request from the GNSS location engine.

       Must be monotonically increasing, jitter @latexonly $\leq$ @endlatexonly 1
       millisecond, never stopping until the process is rebooted.\n
       - Units: Milliseconds */

  /* Mandatory */
  /*  Sensor Transmit Time */
  uint32_t sensorProcTxTime;
  /**<   Value of the sensor time when the control point injects this message
       for use by the GNSS location engine.

       Must be monotonically increasing, jitter @latexonly $\leq$ @endlatexonly 1
       millisecond, never stopping until the process is rebooted.\n
       - Units: Milliseconds */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_CRADLE_STATE_MOUNTED_V02 = 1, /**<  Device is not mounted on the cradle */
  eQMI_LOC_CRADLE_STATE_UNKNOWN_V02 = 2, /**<  Unknown cradle mount state */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Cradle Mount State */
  uint8_t cradleMountState_valid;  /**< Must be set to true if cradleMountState is being passed */
  qmiLocCradleMountStateEnumT_v02 cradleMountState;
  /**<   Cradle Mount state set by the control point.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- CRADLE_STATE_NOT_MOUNTED
       \item    0x00000001 -- CRADLE_STATE_MOUNTED
       \item    0x00000002 -- CRADLE_STATE_UNKNOWN
       \vspace{-0.18in} \end{itemize1}
         */

  /* Optional */
  /*  Cradle Mount Confidence */
  uint8_t confidenceCradleMountState_valid;  /**< Must be set to true if confidenceCradleMountState is being passed */
  uint8_t confidenceCradleMountState;
  /**<   Confidence of the Cradle Mount state expressed as a percentage.\n
       - Range: 0 to 100 */
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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- CRADLE_STATE_NOT_MOUNTED
       \item    0x00000001 -- CRADLE_STATE_MOUNTED
       \item    0x00000002 -- CRADLE_STATE_UNKNOWN
       \vspace{-0.18in} \end{itemize1}         */

  /* Optional */
  /*  Cradle Mount Confidence */
  uint8_t confidenceCradleMountState_valid;  /**< Must be set to true if confidenceCradleMountState is being passed */
  uint8_t confidenceCradleMountState;
  /**<   Confidence in the Cradle Mount state expressed as a percentage.\n
       - Range: 0 to 100 */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_EXTERNAL_POWER_CONNECTED_V02 = 1, /**<  Device is connected to an external power source.  */
  eQMI_LOC_EXTERNAL_POWER_UNKNOWN_V02 = 2, /**<  Unknown external power state.  */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  External Power State */
  uint8_t externalPowerState_valid;  /**< Must be set to true if externalPowerState is being passed */
  qmiLocExternalPowerConfigEnumT_v02 externalPowerState;
  /**<   Power state; injected by the control point.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- EXTERNAL_POWER_NOT_CONNECTED
       \item    0x00000001 -- EXTERNAL_POWER_CONNECTED
       \item    0x00000002 -- EXTERNAL_POWER_UNKNOWN
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- EXTERNAL_POWER_NOT_CONNECTED
       \item    0x00000001 -- EXTERNAL_POWER_CONNECTED
       \item    0x00000002 -- EXTERNAL_POWER_UNKNOWN
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_APN_PROFILE_PDN_TYPE_IPV4V6_V02 = 0x03, /**<  IPV4V6 PDN type.  */
  eQMI_LOC_APN_PROFILE_PDN_TYPE_PPP_V02 = 0x04, /**<  PPP PDN type.  */
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
  /**<   PDN type of the APN profile.

       Valid values: \n
         - 0x00000001 -- PDN_TYPE_IPV4 \n
         - 0x00000002 -- PDN_TYPE_IPV6 \n
         - 0x00000003 -- PDN_TYPE_IPV4V6 \n
         - 0x00000004 -- PDN_TYPE_PPP
  */

  char apnName[QMI_LOC_MAX_APN_NAME_LENGTH_V02 + 1];
  /**<   APN name.
       \begin{itemize1}
       \item    Type: NULL-terminated string
       \item    Maximum string length (including NULL terminator): 101
       \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_SERVER_REQ_STATUS_SUCCESS_V02 = 1, /**<  Location server request was successful.  */
  eQMI_LOC_SERVER_REQ_STATUS_FAILURE_V02 = 2, /**<  Location server request failed. */
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
       Location Server Connection request event. */

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
  /**<   \n Access Point Name (APN) profile information is present only when
       requestType is OPEN and statusType is SUCCESS. */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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
  eQMI_LOC_VX_VERSION_V1_ONLY_V02 = 1, /**<  V1 VX version.  */
  eQMI_LOC_VX_VERSION_V2_ONLY_V02 = 2, /**<  V2 VX version.  */
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
  eQMI_LOC_SUPL_VERSION_1_0_V02 = 1, /**<  SUPL version 1.0.  */
  eQMI_LOC_SUPL_VERSION_2_0_V02 = 2, /**<  SUPL version 2.0.  */
  QMILOCSUPLVERSIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSuplVersionEnumT_v02;
/**
    @}
  */

typedef uint32_t qmiLocLppConfigMaskT_v02;
#define QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE_V02 ((qmiLocLppConfigMaskT_v02)0x00000001) /**<  Enable user plane configuration for LTE Positioning Profile (LPP).  */
#define QMI_LOC_LPP_CONFIG_ENABLE_CONTROL_PLANE_V02 ((qmiLocLppConfigMaskT_v02)0x00000002) /**<  Enable control plane configuration for LPP.      */
typedef uint32_t qmiLocAssistedGlonassProtocolMaskT_v02;
#define QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRC_CP_V02 ((qmiLocAssistedGlonassProtocolMaskT_v02)0x00000001) /**<  Assisted GLONASS is supported over RRC in the control plane.  */
#define QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRLP_UP_V02 ((qmiLocAssistedGlonassProtocolMaskT_v02)0x00000002) /**<  Assisted GLONASS is supported over RRLP in the user plane.  */
#define QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_LPP_UP_V02 ((qmiLocAssistedGlonassProtocolMaskT_v02)0x00000004) /**<  Assisted GLONASS is supported over LPP in the user plane.
       The QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE should be set
       in the LPP configuration for this to take effect.  */
/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSUPLHASHALGOENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUPL_HASH_ALGO_SHA1_V02 = 0, /**<  SHA1 Hash Algorithm for SUPL Version 2.0 or higher  */
  eQMI_LOC_SUPL_HASH_ALGO_SHA256_V02 = 1, /**<  SHA-256 Hash Algorithm for SUPL Version 2.0 or higher  */
  QMILOCSUPLHASHALGOENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSuplHashAlgoEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCSUPLTLSVERSIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_SUPL_TLS_VERSION_1_0_V02 = 0, /**<  SUPL TLS Version 1.0  */
  eQMI_LOC_SUPL_TLS_VERSION_1_1_V02 = 1, /**<  SUPL TLS Version 1.1  */
  QMILOCSUPLTLSVERSIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSuplTlsVersionEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCEMERGENCYPROTOCOLENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_CP_V02 = 0, /**<  use Control plane protocol during emergency while on WCDMA  */
  eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_UP_V02 = 1, /**<  use SUPL 2.0 emergency services during emergency while on WCDMA  */
  QMILOCEMERGENCYPROTOCOLENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocEmergencyProtocolEnumT_v02;
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
  /**<   Indicates whether SUPL security is enabled.
       \begin{itemize1}
       \item    0x01 (TRUE) -- SUPL security is enabled
       \item    0x00 (FALSE) -- SUPL security is disabled
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  VX Version */
  uint8_t vxVersion_valid;  /**< Must be set to true if vxVersion is being passed */
  qmiLocVxVersionEnumT_v02 vxVersion;
  /**<   VX version.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- VX_VERSION_V1_ONLY
       \item    0x00000002 -- VX_VERSION_V2_ONLY
       \vspace{-0.18in} \end{itemize1}
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

  /* Optional */
  /*  LPP Configuration */
  uint8_t lppConfig_valid;  /**< Must be set to true if lppConfig is being passed */
  qmiLocLppConfigMaskT_v02 lppConfig;
  /**<   LTE Positioning Profile (LPP) configuration.

      Valid bitmasks: \begin{itemize1}
      \item    0x00000001 -- LPP_CONFIG_ENABLE_USER_PLANE
      \item    0x00000002 -- LPP_CONFIG_ENABLE_CONTROL_PLANE
      \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Assisted GLONASS Protocol Mask */
  uint8_t assistedGlonassProtocolMask_valid;  /**< Must be set to true if assistedGlonassProtocolMask is being passed */
  qmiLocAssistedGlonassProtocolMaskT_v02 assistedGlonassProtocolMask;
  /**<   Configures the protocols that the location service supports
 for assisted GLONASS.

 Valid bitmasks: \n
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRC_CP (0x00000001) --  Assisted GLONASS is supported over RRC in the control plane.
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRLP_UP (0x00000002) --  Assisted GLONASS is supported over RRLP in the user plane.
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_LPP_UP (0x00000004) --  Assisted GLONASS is supported over LPP in the user plane.
       The QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE should be set
       in the LPP configuration for this to take effect.
 */

  /* Optional */
  /*  SUPL Hash Algorithm */
  uint8_t suplHashAlgo_valid;  /**< Must be set to true if suplHashAlgo is being passed */
  qmiLocSuplHashAlgoEnumT_v02 suplHashAlgo;
  /**<   SUPL Hash Algorithm that needs to be used. Values: \n
      - eQMI_LOC_SUPL_HASH_ALGO_SHA1 (0) --  SHA1 Hash Algorithm for SUPL Version 2.0 or higher
      - eQMI_LOC_SUPL_HASH_ALGO_SHA256 (1) --  SHA-256 Hash Algorithm for SUPL Version 2.0 or higher
 */

  /* Optional */
  /*  SUPL TLS Version */
  uint8_t suplTlsVersion_valid;  /**< Must be set to true if suplTlsVersion is being passed */
  qmiLocSuplTlsVersionEnumT_v02 suplTlsVersion;
  /**<   SUPL TLS (Transport Layer Security) Version. This configuration is only
 applicable to SUPL 2.0 or higher as SUPL 1.0 always uses TLS version 1.0.
 Values: \n
      - eQMI_LOC_SUPL_TLS_VERSION_1_0 (0) --  SUPL TLS Version 1.0
      - eQMI_LOC_SUPL_TLS_VERSION_1_1 (1) --  SUPL TLS Version 1.1
 */

  /* Optional */
  /*  Emergency Protocol */
  uint8_t emergencyProtocol_valid;  /**< Must be set to true if emergencyProtocol is being passed */
  qmiLocEmergencyProtocolEnumT_v02 emergencyProtocol;
  /**<   Configures the protocol to be used during emergency.
 Note: Currently only can select on WCDMA. FOR GSM and 1x UE
 will always allow only Control plane NI trigger for positioning.
 For LTE, UE Can allow either SUPL or Control plane NI trigger.
 Values: \n
      - eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_CP (0) --  use Control plane protocol during emergency while on WCDMA
      - eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_UP (1) --  use SUPL 2.0 emergency services during emergency while on WCDMA
 */
}qmiLocSetProtocolConfigParametersReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint64_t qmiLocProtocolConfigParamMaskT_v02;
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000001ull) /**<  Mask for the SUPL security configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000002ull) /**<  Mask for the VX version configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000004ull) /**<  Mask for the SUPL version configuration parameter.       */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000008ull) /**<  Mask for the LPP configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000010ull) /**<  Mask for the assisted glonass configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000020ull) /**<  Mask for the SUPL Hash algorithm configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000040ull) /**<  Mask for the SUPL TLS version configuration parameter.  */
#define QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL_V02 ((qmiLocProtocolConfigParamMaskT_v02)0x0000000000000080ull) /**<  Mask for the emergency protocol configuration parameter.  */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedProtocolConfigParamMask_valid;  /**< Must be set to true if failedProtocolConfigParamMask is being passed */
  qmiLocProtocolConfigParamMaskT_v02 failedProtocolConfigParamMask;
  /**<   Identifies parameters that were not set successfully. This field
 is sent only if the status is not a success.

 Valid bitmasks: \n
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY (0x0000000000000001) --  Mask for the SUPL security configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION (0x0000000000000002) --  Mask for the VX version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION (0x0000000000000004) --  Mask for the SUPL version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG (0x0000000000000008) --  Mask for the LPP configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL (0x0000000000000010) --  Mask for the assisted glonass configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO (0x0000000000000020) --  Mask for the SUPL Hash algorithm configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION (0x0000000000000040) --  Mask for the SUPL TLS version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL (0x0000000000000080) --  Mask for the emergency protocol configuration parameter.
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
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_SECURITY (0x0000000000000001) --  Mask for the SUPL security configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_VX_VERSION (0x0000000000000002) --  Mask for the VX version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_VERSION (0x0000000000000004) --  Mask for the SUPL version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_LPP_CONFIG (0x0000000000000008) --  Mask for the LPP configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_ASSISTED_GLONASS_PROTOCOL (0x0000000000000010) --  Mask for the assisted glonass configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_HASH_ALGO (0x0000000000000020) --  Mask for the SUPL Hash algorithm configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_SUPL_TLS_VERSION (0x0000000000000040) --  Mask for the SUPL TLS version configuration parameter.
      - QMI_LOC_PROTOCOL_CONFIG_PARAM_MASK_EMERGENCY_PROTOCOL (0x0000000000000080) --  Mask for the emergency protocol configuration parameter.
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
    */

  /* Optional */
  /*  SUPL Security */
  uint8_t suplSecurity_valid;  /**< Must be set to true if suplSecurity is being passed */
  uint8_t suplSecurity;
  /**<   Indicates whether SUPL security is enabled.
       \begin{itemize1}
       \item    0x01 (TRUE) -- SUPL security is enabled
       \item    0x00 (FALSE) -- SUPL security is disabled
       \vspace{-0.18in} \end{itemize1}*/

  /* Optional */
  /*  VX Version */
  uint8_t vxVersion_valid;  /**< Must be set to true if vxVersion is being passed */
  qmiLocVxVersionEnumT_v02 vxVersion;
  /**<   VX version.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- VX_VERSION_V1_ONLY
       \item    0x00000002 -- VX_VERSION_V2_ONLY
       \vspace{-0.18in} \end{itemize1}
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

  /* Optional */
  /*  LPP Configuration */
  uint8_t lppConfig_valid;  /**< Must be set to true if lppConfig is being passed */
  qmiLocLppConfigMaskT_v02 lppConfig;
  /**<   LTE Positioning Profile (LPP) configuration.

      Valid bitmasks: \begin{itemize1}
      \item    0x00000001 -- LPP_CONFIG_ENABLE_USER_PLANE
      \item    0x00000002 -- LPP_CONFIG_ENABLE_CONTROL_PLANE
      \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Assisted GLONASS Protocol Mask */
  uint8_t assistedGlonassProtocolMask_valid;  /**< Must be set to true if assistedGlonassProtocolMask is being passed */
  qmiLocAssistedGlonassProtocolMaskT_v02 assistedGlonassProtocolMask;
  /**<   Assisted GLONASS Protocol mask.

 Valid bitmasks: \n
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRC_CP (0x00000001) --  Assisted GLONASS is supported over RRC in the control plane.
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_RRLP_UP (0x00000002) --  Assisted GLONASS is supported over RRLP in the user plane.
      - QMI_LOC_ASSISTED_GLONASS_PROTOCOL_MASK_LPP_UP (0x00000004) --  Assisted GLONASS is supported over LPP in the user plane.
       The QMI_LOC_LPP_CONFIG_ENABLE_USER_PLANE should be set
       in the LPP configuration for this to take effect.
 */

  /* Optional */
  /*  SUPL Hash Algorithm */
  uint8_t suplHashAlgo_valid;  /**< Must be set to true if suplHashAlgo is being passed */
  qmiLocSuplHashAlgoEnumT_v02 suplHashAlgo;
  /**<   SUPL Hash Algorithm that needs to be used. Values: \n
      - eQMI_LOC_SUPL_HASH_ALGO_SHA1 (0) --  SHA1 Hash Algorithm for SUPL Version 2.0 or higher
      - eQMI_LOC_SUPL_HASH_ALGO_SHA256 (1) --  SHA-256 Hash Algorithm for SUPL Version 2.0 or higher
 */

  /* Optional */
  /*  SUPL TLS Version */
  uint8_t suplTlsVersion_valid;  /**< Must be set to true if suplTlsVersion is being passed */
  qmiLocSuplTlsVersionEnumT_v02 suplTlsVersion;
  /**<   SUPL TLS (Transport Layer Security) Version. This configuration is only
 applicable to SUPL 2.0 or higher as SUPL 1.0 always uses TLS version 1.0.
 Values: \n
      - eQMI_LOC_SUPL_TLS_VERSION_1_0 (0) --  SUPL TLS Version 1.0
      - eQMI_LOC_SUPL_TLS_VERSION_1_1 (1) --  SUPL TLS Version 1.1
 */

  /* Optional */
  /*  Emergency Protocol  */
  uint8_t emergencyProtocol_valid;  /**< Must be set to true if emergencyProtocol is being passed */
  qmiLocEmergencyProtocolEnumT_v02 emergencyProtocol;
  /**<   The protocol to be used during emergency.
 Values: \n
      - eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_CP (0) --  use Control plane protocol during emergency while on WCDMA
      - eQMI_LOC_EMERGENCY_PROTOCOL_WCDMA_UP (1) --  use SUPL 2.0 emergency services during emergency while on WCDMA
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
  */
  eQMI_LOC_SENSOR_CONFIG_SENSOR_USE_DISABLE_V02 = 1, /**<  Inertial sensors are not to be used to aid heading and position
       improvement.  */
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SENSORS_USE_ENABLED:
                 Sensors data is to be requested whenever a position request is
                 received. If sensors data is injected, the GNSS location engine
                 attempts to improve the heading and positioning performance using
                 sensors. This is the default.
        \item    0x00000001 -- SENSORS_USE_DISABLED:
                 Inertial sensors are not to be used to aid in heading and
                 position improvement.
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Sensors Usage */
  uint8_t sensorsUsage_valid;  /**< Must be set to true if sensorsUsage is being passed */
  qmiLocSensorsControlConfigSensorUseEnumT_v02 sensorsUsage;
  /**<   Controls how sensors are used to aid the heading and positioning
       performance.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- SENSORS_USE_ENABLED:
                Sensors data is to be requested whenever a position request is
                received. If sensors data is injected, the GNSS location engine
                attempts to improve the heading and positioning performance using
                sensors. This is the default.
       \item    0x00000001 -- SENSORS_USE_DISABLED:
                Inertial sensors are not to be used to aid in the heading and
                position improvement.
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocGetSensorControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocSensorPropertiesMaskT_v02;
#define QMI_LOC_SENSOR_PROPERTIES_MASK_GYRO_BIAS_VARIANCE_RANDOM_WALK_V02 ((qmiLocSensorPropertiesMaskT_v02)0x00000001) /**<  Denotes the gyro bias variance random walk parameter.  */
#define QMI_LOC_SENSOR_PROPERTIES_MASK_VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY_V02 ((qmiLocSensorPropertiesMaskT_v02)0x00000002) /**<  Denotes the velocity random walk spectral density parameter.  */
#define QMI_LOC_SENSOR_PROPERTIES_MASK_ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY_V02 ((qmiLocSensorPropertiesMaskT_v02)0x00000004) /**<  Denotes the acceleration random walk spectral density parameter.  */
#define QMI_LOC_SENSOR_PROPERTIES_MASK_ANGLE_RANDOM_WALK_SPECTRAL_DENSITY_V02 ((qmiLocSensorPropertiesMaskT_v02)0x00000008) /**<  Denotes the angle random walk spectral density parameter.  */
#define QMI_LOC_SENSOR_PROPERTIES_MASK_RATE_RANDOM_WALK_SPECTRAL_DENSITY_V02 ((qmiLocSensorPropertiesMaskT_v02)0x00000010) /**<  Denotes the rate random walk spectral density parameter.  */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Sets the properties specific to the type of sensor used.
                    The control point must set sensor properties before they can be
                    used to aid in heading and positioning performance improvement.
   */
typedef struct {

  /* Optional */
  /*  Gyro Bias Random Walk Variance */
  uint8_t gyroBiasVarianceRandomWalk_valid;  /**< Must be set to true if gyroBiasVarianceRandomWalk is being passed */
  float gyroBiasVarianceRandomWalk;
  /**<   Specifies the gyro bias random walk variance parameter as a positive
       floating-point value. This value has internal default value 1.0e-5 radian^2/second^4.
       The gyro bias variance random walk parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians^2/seconds^4

  */

  /* Optional */
  /*  Velocity Random Walk Spectral Density */
  uint8_t velocityRandomWalkSpectralDensity_valid;  /**< Must be set to true if velocityRandomWalkSpectralDensity is being passed */
  float velocityRandomWalkSpectralDensity;
  /**<   Specifies the velocity random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The velocity random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Meters/seconds^2/Hertz^0.5

  */

  /* Optional */
  /*  Acceleration Random Walk Spectral Density */
  uint8_t accelerationRandomWalkSpectralDensity_valid;  /**< Must be set to true if accelerationRandomWalkSpectralDensity is being passed */
  float accelerationRandomWalkSpectralDensity;
  /**<   Specifies the acceleration random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The acceleration random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Meters/seconds^3/Hertz^0.5

  */

  /* Optional */
  /*  Angle Random Walk Spectral Density */
  uint8_t angleRandomWalkSpectralDensity_valid;  /**< Must be set to true if angleRandomWalkSpectralDensity is being passed */
  float angleRandomWalkSpectralDensity;
  /**<   Specifies the angle random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The angle random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians/seconds/Hertz^0.5

  */

  /* Optional */
  /*  Rate Random Walk Spectral Density */
  uint8_t rateRandomWalkSpectralDensity_valid;  /**< Must be set to true if rateRandomWalkSpectralDensity is being passed */
  float rateRandomWalkSpectralDensity;
  /**<   Specifies the rate random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The rate random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians/seconds^2/Hertz^0.5

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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- SUCCESS
       \item    0x00000001 -- GENERAL_FAILURE
       \item    0x00000002 -- UNSUPPORTED
       \item    0x00000003 -- INVALID_PARAMETER
       \item    0x00000004 -- ENGINE_BUSY
       \item    0x00000005 -- PHONE_OFFLINE
       \item    0x00000006 -- TIMEOUT
       \item    0x00000007 -- CONFIG_NOT_SUPPORTED
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Failed Set Sensor Properties */
  uint8_t failedSensorPropertiesMask_valid;  /**< Must be set to true if failedSensorPropertiesMask is being passed */
  qmiLocSensorPropertiesMaskT_v02 failedSensorPropertiesMask;
  /**<   This field will be sent only if the status is not a success.
       Identifies the parameters that were not set successfully.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- GYRO_BIAS_VARIANCE_RANDOM_WALK
       \item    0x00000002 -- VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000004 -- ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000008 -- ANGLE_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000010 -- RATE_RANDOM_WALK_SPECTRAL_DENSITY
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocSetSensorPropertiesIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Retrieves the current sensor properties. */
typedef struct {

  /* Mandatory */
  /*  Sensor Properties Config Parameters */
  qmiLocSensorPropertiesMaskT_v02 getSensorPropertiesMask;
  /**<   Mask denoting the sensor properties parameters to be retrieved.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- GYRO_BIAS_VARIANCE_RANDOM_WALK
       \item    0x00000002 -- VELOCITY_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000004 -- ACCELERATION_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000008 -- ANGLE_RANDOM_WALK_SPECTRAL_DENSITY
       \item    0x00000010 -- RATE_RANDOM_WALK_SPECTRAL_DENSITY
       \vspace{-0.18in} \end{itemize1}
  */
}qmiLocGetSensorPropertiesReqMsgT_v02;  /* Message */
/**
    @}
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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- SUCCESS
       \item    0x00000001 -- GENERAL_FAILURE
       \item    0x00000002 -- UNSUPPORTED
       \item    0x00000003 -- INVALID_PARAMETER
       \item    0x00000004 -- ENGINE_BUSY
       \item    0x00000005 -- PHONE_OFFLINE
       \item    0x00000006 -- TIMEOUT
       \item    0x00000007 -- CONFIG_NOT_SUPPORTED
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Gyro Bias Random Walk Variance */
  uint8_t gyroBiasVarianceRandomWalk_valid;  /**< Must be set to true if gyroBiasVarianceRandomWalk is being passed */
  float gyroBiasVarianceRandomWalk;
  /**<   Specifies the gyro bias random walk variance parameter as a positive
       floating-point value. This value has internal default value 1.0e-5 radian^2/second^4.
       The gyro bias variance random walk parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians^2/seconds^4

  */

  /* Optional */
  /*  Velocity Random Walk Spectral Density */
  uint8_t velocityRandomWalkSpectralDensity_valid;  /**< Must be set to true if velocityRandomWalkSpectralDensity is being passed */
  float velocityRandomWalkSpectralDensity;
  /**<   Specifies the velocity random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The velocity random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Meters/seconds^2/Hertz^0.5

  */

  /* Optional */
  /*  Acceleration Random Walk Spectral Density */
  uint8_t accelerationRandomWalkSpectralDensity_valid;  /**< Must be set to true if accelerationRandomWalkSpectralDensity is being passed */
  float accelerationRandomWalkSpectralDensity;
  /**<   Specifies the acceleration random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The acceleration random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Meters/seconds^3/Hertz^0.5

  */

  /* Optional */
  /*  Angle Random Walk Spectral Density */
  uint8_t angleRandomWalkSpectralDensity_valid;  /**< Must be set to true if angleRandomWalkSpectralDensity is being passed */
  float angleRandomWalkSpectralDensity;
  /**<   Specifies the angle random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The angle random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians/seconds/Hertz^0.5

  */

  /* Optional */
  /*  Rate Random Walk Spectral Density */
  uint8_t rateRandomWalkSpectralDensity_valid;  /**< Must be set to true if rateRandomWalkSpectralDensity is being passed */
  float rateRandomWalkSpectralDensity;
  /**<   Specifies the rate random walk spectral density parameter as a positive
       floating-point value. This value does not have any internal defaults.
       The rate random walk spectral density parameter is derived from either the
       sensors data sheet or a sensors conformance test. \n
       - Units: Radians/seconds^2/Hertz^0.5

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
       inertial sensors       */
  eQMI_LOC_SENSOR_PERFORMANCE_CONTROL_MODE_FORCED_V02 = 1, /**<  Sensors usage is to be forced ON.
       This mode can be requested by the control point when
       power consumption is not a restriction to the use of
       inertial sensors.       */
  QMILOCSENSORPERFORMANCECONTROLMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocSensorPerformanceControlModeEnumT_v02;
/**
    @}
  */

typedef uint32_t qmiLocSensorAlgorithmMaskT_v02;
#define QMI_LOC_SENSOR_ALGORITHM_MASK_DISABLE_INS_POSITIONING_FILTER_V02 ((qmiLocSensorAlgorithmMaskT_v02)0x00000001) /**<  Inertial sensors are not to be used in Accelerometer-integrated fashion with
       GNSS. They can still be used for aiding in heading improvements.  */
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

       Valid values: \begin{itemize1}
       \item    0x00000000 -- AUTO: \n
                The GNSS location engine can decide when to request sensor data
                injection based on internal criteria. This is the default.
       \item    0x00000001 -- FORCED: \n
                The GNSS location engine must request use of sensors every time
                the GNSS location engine turns on.
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Accelerometer Sampling Specification */
  uint8_t accelSamplingSpec_valid;  /**< Must be set to true if accelSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpec;
  /**<   \n \vspace{0.06in} Sets the nominal rate at which the GNSS location
       engine is to request acceleration data to be used by the low data rate
       filter. The sensor data rate is specified in terms of the nominal number
       of samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 10 Hz sampling rate and 2 Hz batching rate.
  */

  /* Optional */
  /*  Gyroscope Sampling Specification */
  uint8_t gyroSamplingSpec_valid;  /**< Must be set to true if gyroSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpec;
  /**<   \n \vspace{0.06in} Sets the nominal rate at which the GNSS location
       engine is to request gyro data to be used by the high data rate filter.
       The sensor data rate is specified in terms of the nominal number of
       samples per batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 10 Hz sampling rate and 2 Hz batching rate.
  */

  /* Optional */
  /*  Algorithm Configuration */
  uint8_t algorithmConfig_valid;  /**< Must be set to true if algorithmConfig is being passed */
  qmiLocSensorAlgorithmMaskT_v02 algorithmConfig;
  /**<   Sets which sensor algorithms are to be used when processing sensor data.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- DISABLE_INS_POSITIONING_FILTER
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  High Data Rate Filter Accelerometer Sampling Specification */
  uint8_t accelSamplingSpecHigh_valid;  /**< Must be set to true if accelSamplingSpecHigh is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpecHigh;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       acceleration data to be used by the high data rate filter. The sensor
       data rate is    specified in terms of the nominal number of samples per
       batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 100 Hz sampling rate and 4 Hz batching rate.
  */

  /* Optional */
  /*  High Data Rate Filter Gyroscope Sampling Specification */
  uint8_t gyroSamplingSpecHigh_valid;  /**< Must be set to true if gyroSamplingSpecHigh is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpecHigh;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       gyro data to be used by the high data rate filter. The sensor data rate
       is specified in terms of the nominal number of samples per batch and the
       number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 100 Hz sampling rate and 4 Hz batching rate.
  */
}qmiLocSetSensorPerformanceControlConfigReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocSensorPerformanceControlConfigFailureMaskT_v02;
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_PERFORMANCE_MODE_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000001) /**<  Failed to set the performance mode.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_ACCEL_SAMPLING_SPEC_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000002) /**<  Failed to set the accelerometer sampling specification.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_GYRO_SAMPLING_SPEC_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000004) /**<  Failed to set the gyroscope sampling specification.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_ALGORITHM_CONFIG_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000008) /**<  Failed to set the algorithm configuration  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_ACCEL_SAMPLING_SPEC_HIGH_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000010) /**<  Failed to set the accelerometer sampling specification.  */
#define QMI_LOC_SENSOR_PERFORMANCE_CONTROL_CONFIG_PARAM_MASK_GYRO_SAMPLING_SPEC_HIGH_V02 ((qmiLocSensorPerformanceControlConfigFailureMaskT_v02)0x00000020) /**<  Failed to set the gyroscope sampling specification.  */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Provides fine-grained control of sensor based positioning
                    performance */
typedef struct {

  /* Mandatory */
  /*  Set Sensor Perf Control Config Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Set Sensor Performance Control Configuration request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Failed Configuration */
  uint8_t failedConfiguration_valid;  /**< Must be set to true if failedConfiguration is being passed */
  qmiLocSensorPerformanceControlConfigFailureMaskT_v02 failedConfiguration;
  /**<   Identifies parameters that were not configured successfully. This field
       is sent only if the status is not a success.


       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- PERFORMANCE_MODE
       \item    0x00000002 -- ACCEL_SAMPLING_SPEC
       \item    0x00000004 -- GYRO_SAMPLING_SPEC
       \item    0x00000008 -- ALGORITHM_CONFIG
       \item    0x00000010 -- ACCEL_SAMPLING_SPEC_HIGH
       \item    0x00000020 -- GYRO_SAMPLING_SPEC_HIGH
       \vspace{-0.18in} \end{itemize1}
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
  /**<   Status of the Get Sensor Performance Control Configuration request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Performance Control Mode */
  uint8_t performanceControlMode_valid;  /**< Must be set to true if performanceControlMode is being passed */
  qmiLocSensorPerformanceControlModeEnumT_v02 performanceControlMode;
  /**<   Controls when sensor data is requested during GNSS fix processing.
       This field is relevant only when sensors have been enabled using the
       sensor control configuration.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- AUTO: \n
                The GNSS location engine can decide when to request sensor data
                injection based on internal criteria. This is the default.
       \item    0x0000001 -- FORCED: \n
                The GNSS location engine must request use of the sensors every time
                the GNSS location engine turns on.
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Accelerometer Sampling Specification */
  uint8_t accelSamplingSpec_valid;  /**< Must be set to true if accelSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       acceleration data to be used by the high data rate filter. The sensor
       data rate is specified in terms of the nominal number of samples per
       batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 10 Hz sampling rate and  2Hz batching rate.
  */

  /* Optional */
  /*  Gyroscope Sampling Specification */
  uint8_t gyroSamplingSpec_valid;  /**< Must be set to true if gyroSamplingSpec is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpec;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       gyro data to be used by the high data rate filter. The sensor data
       rate is specified in terms of the nominal number of samples per batch
       and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 10 Hz sampling rate and 2 Hz batching rate.
  */

  /* Optional */
  /*  Algorithm Configuration */
  uint8_t algorithmConfig_valid;  /**< Must be set to true if algorithmConfig is being passed */
  qmiLocSensorAlgorithmMaskT_v02 algorithmConfig;
  /**<   Informs which sensor algorithms are currently set.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- DISABLE_INS_POSITIONING_FILTER
       \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  High Data Rate Filter Accelerometer Sampling Specification */
  uint8_t accelSamplingSpecHigh_valid;  /**< Must be set to true if accelSamplingSpecHigh is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 accelSamplingSpecHigh;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       acceleration data to be used by the high data rate filter. The sensor
       data rate is    specified in terms of the nominal number of samples per
       batch and the number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 100 Hz sampling rate and 4 Hz batching rate.
  */

  /* Optional */
  /*  High Data Rate Filter Gyroscope Sampling Specification */
  uint8_t gyroSamplingSpecHigh_valid;  /**< Must be set to true if gyroSamplingSpecHigh is being passed */
  qmiLocSensorControlConfigSamplingSpecStructT_v02 gyroSamplingSpecHigh;
  /**<   \n Sets the nominal rate at which the GNSS location engine is to request
       gyro data to be used by the high data rate filter. The sensor data rate
       is specified in terms of the nominal number of samples per batch and the
       number of batches per second.
       However, the final control of the actual requested rate resides with
       the Sensors Manager Module/GNSS location engine. \n
       Default: 100 Hz sampling rate and 4 Hz batching rate.
  */
}qmiLocGetSensorPerformanceControlConfigIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects a SUPL certificate to be used in AGNSS sessions. */
typedef struct {

  /* Mandatory */
  /*  SUPL Certificate ID */
  uint8_t suplCertId;
  /**<   Certificate ID of the SUPL certificate. \n
       - Units: Bytes \n
       - Range: 0 to 9  */

  /* Mandatory */
  /*  SUPL Certificate Data */
  uint32_t suplCertData_len;  /**< Must be set to # of elements in suplCertData */
  uint8_t suplCertData[QMI_LOC_MAX_SUPL_CERT_LENGTH_V02];
  /**<   SUPL certificate contents. \n
       - Type: Array of bytes. \n
       - Maximum certificate size: 2000 bytes */
}qmiLocInjectSuplCertificateReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects a SUPL certificate to be used in AGNSS sessions. */
typedef struct {

  /* Mandatory */
  /*  SUPL Certificate Injection Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject SUPL Certificate request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */
}qmiLocInjectSuplCertificateIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Deletes a SUPL certificate.  */
typedef struct {

  /* Optional */
  /*  SUPL Certificate ID */
  uint8_t suplCertId_valid;  /**< Must be set to true if suplCertId is being passed */
  uint8_t suplCertId;
  /**<   Certificate ID of the SUPL certificate to be deleted. \n
       - Units: Bytes \n
       - Range: 0 to 9 \n
       If suplCertId is not specified,
       all SUPL certificates are deleted. */
}qmiLocDeleteSuplCertificateReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Deletes a SUPL certificate.  */
typedef struct {

  /* Mandatory */
  /*  SUPL Certificate Deletion Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Delete SUPL Certificate request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */
}qmiLocDeleteSuplCertificateIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocPositionEngineConfigParamMaskT_v02;
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_INJECTED_POSITION_CONTROL_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000001) /**<  Denotes whether the position engine uses the
       injected position in a direct position calculation.  */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_FILTER_SV_USAGE_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000002) /**<  Denotes whether the position engine filters the
       SV usage in the fix.  */
#define QMI_LOC_POSITION_ENGINE_CONFIG_PARAM_MASK_STORE_ASSIST_DATA_V02 ((qmiLocPositionEngineConfigParamMaskT_v02)0x00000004) /**<  Denotes whether the position engine stores assistance data
       in persistent memory.       */
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
  /**<   Controls how the injected position is used in the position engine.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- Use the injected position in a direct position
                               calculation
       \item    0x00 (FALSE) -- Do not use the injected position in a direct
                                position calculation
       \end{itemize1}
       The default value is TRUE.
  */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Controls whether SV usage is filtered in a position fix.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- Filter the usage of SVs in the fix
       \item    0x00 (FALSE) -- Do not filter the usage of SVs in the fix
       \end{itemize1}
       The default value is FALSE.
  */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Controls whether assistance data is to be stored in
       persistent memory.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- Store assistance data in persistent memory
       \item    0x00 (FALSE) -- Do not store assistance data in persistent memory
       \end{itemize1}
       The default value is TRUE.
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedPositionEngineConfigParamMask_valid;  /**< Must be set to true if failedPositionEngineConfigParamMask is being passed */
  qmiLocPositionEngineConfigParamMaskT_v02 failedPositionEngineConfigParamMask;
  /**<   Identifies the parameters that were not set successfully.
       This field is sent only if the status is other than SUCCESS.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- INJECTED_POSITION_CONTROL
       \item    0x00000002 -- FILTER_SV_USAGE
       \item    0x00000004 -- STORE_ASSIST_DATA
       \vspace{-0.18in} \end{itemize1}
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

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- INJECTED_POSITION_CONTROL
       \item    0x00000002 -- FILTER_SV_USAGE
       \item    0x00000004 -- STORE_ASSIST_DATA
       \vspace{-0.18in} \end{itemize1}
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

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
    */

  /* Optional */
  /*  Injected Position Control */
  uint8_t injectedPositionControl_valid;  /**< Must be set to true if injectedPositionControl is being passed */
  uint8_t injectedPositionControl;
  /**<   Specifies whether the injected position is used for a direct calculation
       in the position engine.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- The injected position is used in a direct
                               position calculation
       \item    0x00 (FALSE) -- The injected position is not used in a direct
                                position calculation
       \end{itemize1}
       The default value is TRUE.
  */

  /* Optional */
  /*  Filter SV Usage */
  uint8_t filterSvUsage_valid;  /**< Must be set to true if filterSvUsage is being passed */
  uint8_t filterSvUsage;
  /**<   Specifies whether SV usage is filtered in a position fix.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- SV usage is filtered in the fix
       \item    0x00 (FALSE) -- SV usage is not filtered in the fix
       \end{itemize1}
       The default value is FALSE.
  */

  /* Optional */
  /*  Store Assist Data */
  uint8_t storeAssistData_valid;  /**< Must be set to true if storeAssistData is being passed */
  uint8_t storeAssistData;
  /**<   Specifies whether assistance data is stored in persistent memory.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- Assistance data is stored in persistent memory
       \item    0x00 (FALSE) -- Assistance data is not stored in persistent
                                memory
       \end{itemize1}
       The default value is TRUE.
  */
}qmiLocGetPositionEngineConfigParametersIndMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint8_t qmiLocGeofenceBreachMaskT_v02;
#define QMI_LOC_GEOFENCE_BREACH_ENTERING_MASK_V02 ((qmiLocGeofenceBreachMaskT_v02)0x01) /**<  If this mask is set, a breach event is reported
       when the Geofence is entered.  */
#define QMI_LOC_GEOFENCE_BREACH_LEAVING_MASK_V02 ((qmiLocGeofenceBreachMaskT_v02)0x02) /**<  If this mask is set, a breach event is reported
       when the Geofence is exited.  */
/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCERESPONSIVENESSENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_LOW_V02 = 0x01, /**<  The Geofence is monitored for a breach at a
       lower rate. The gap between actual breach and
       the time it is reported is higher. This
       setting results in lower power usage.  */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_MED_V02 = 0x02, /**<  The Geofence is monitored for a breach at a
       medium rate. This is the default setting.  */
  eQMI_LOC_GEOFENCE_RESPONSIVENESS_HIGH_V02 = 0x03, /**<  The Geofence is monitored for a breach at a
       high rate. The gap between actual breach and
       the time it is reported is low. This results
       in higher power usage.  */
  QMILOCGEOFENCERESPONSIVENESSENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceResponsivenessEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCECONFIDENCEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_CONFIDENCE_LOW_V02 = 0x01, /**<  The Geofence engine indicates a breach with
       low confidence. This setting results in lower
       power usage. This setting can impact the "yield" because
       incorrect breach events may be sent.  */
  eQMI_LOC_GEOFENCE_CONFIDENCE_MED_V02 = 0x02, /**<  The Geofence engine indicates a breach with
       medium confidence. This is the default setting. */
  eQMI_LOC_GEOFENCE_CONFIDENCE_HIGH_V02 = 0x03, /**<  The Geofence engine indicates a breach with
       high confidence. This setting results in higher
       power usage. */
  QMILOCGEOFENCECONFIDENCEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceConfidenceEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  double latitude;
  /**<   Latitude of the center of the Geofence.*/

  double longitude;
  /**<   Longitude of the center of the Geofence.*/

  uint32_t radius;
  /**<   Radius of the circular Geofence in meters. */
}qmiLocCircularGeofenceArgsStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEPOSITIONENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_POSITION_INSIDE_V02 = 0x01, /**<  Position inside a Geofence.  */
  eQMI_LOC_GEOFENCE_POSITION_OUTSIDE_V02 = 0x02, /**<  Position outside a Geofence.    */
  QMILOCGEOFENCEPOSITIONENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofencePositionEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to add a circular Geofence. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned in the Add Circular Geofence
       indication. */

  /* Mandatory */
  /*  Circular Geofence Arguments */
  qmiLocCircularGeofenceArgsStructT_v02 circularGeofenceArgs;

  /* Mandatory */
  /*  Breach Event Mask */
  qmiLocGeofenceBreachMaskT_v02 breachMask;
  /**<   Specifies the breach events in which the client is interested.

       Valid values: \begin{itemize1}
       \item    0x01 -- GEOFENCE_BREACH_ENTERING_MASK
       \item    0x02 -- GEOFENCE_BREACH_LEAVING_MASK
       \vspace{-0.18in} \end{itemize1} */

  /* Mandatory */
  /*  Include Position in Breach Event */
  uint8_t includePosition;
  /**<   Specifies whether the Geofence engine is to include the position
       in a breach event.

       Valid values: \begin{itemize1}
       \item    0x01 (TRUE) -- Position will be reported with the breach event
       \item    0x00 (FALSE) -- Position will not be reported with the breach
                                event
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Responsiveness */
  uint8_t responsiveness_valid;  /**< Must be set to true if responsiveness is being passed */
  qmiLocGeofenceResponsivenessEnumT_v02 responsiveness;
  /**<   Specifies the rate of detection for a Geofence breach.
       This may impact the time lag between the actual breach event and
       when it is reported. This parameter has power implications
       and is to be fine-tuned to optimize power savings.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_RESPONSIVENESS_LOW
       \item    0x00000002 -- GEOFENCE_RESPONSIVENESS_MED
       \item    0x00000003 -- GEOFENCE_RESPONSIVENESS_HIGH
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Confidence */
  uint8_t confidence_valid;  /**< Must be set to true if confidence is being passed */
  qmiLocGeofenceConfidenceEnumT_v02 confidence;
  /**<   Given a breach event, the confidence determines the probability
       that the breach happened at the Geofence boundary.
       This parameter has power implications and
       is to be fine-tuned to optimize power savings.

       Valid values:  \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_CONFIDENCE_LOW
       \item    0x00000002 -- GEOFENCE_CONFIDENCE_MED
       \item    0x00000003 -- GEOFENCE_CONFIDENCE_HIGH
       \vspace{-0.18in} \end{itemize1} */
}qmiLocAddCircularGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to add a circular Geofence. */
typedef struct {

  /* Mandatory */
  /*  Add Circular Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Add Circular Geofence request.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- SUCCESS
       \item    0x00000001 -- GENERAL_FAILURE
       \item    0x00000002 -- UNSUPPORTED
       \item    0x00000003 -- INVALID_PARAMETER
       \item    0x00000004 -- ENGINE_BUSY
       \item    0x00000005 -- PHONE_OFFLINE
       \item    0x00000006 -- TIMEOUT
       \item    0x00000008 -- INSUFFICIENT_MEMORY
       \vspace{-0.18in} \end{itemize1}
    */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Add Circular
       Geofence request. This parameter will always be present
       if the status field is set to SUCCESS. */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Geofence identifier allocated by the engine.
       The client must include this identifier in all transactions
       pertaining to this Geofence. */
}qmiLocAddCircularGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to delete a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence that is to be deleted. */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned in the Delete Geofence
       indication. */
}qmiLocDeleteGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to delete a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Delete Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Delete Geofence request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence that was deleted. */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Delete
       Geofence request. This parameter will always be present
       if the status field is set to SUCCESS. */
}qmiLocDeleteGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCEORIGINENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_ORIGIN_NETWORK_V02 = 1, /**<  The Geofence was initiated by a network-initiated client.  */
  eQMI_LOC_GEOFENCE_ORIGIN_DEVICE_V02 = 2, /**<  The Geofence was initiated by the device.  */
  QMILOCGEOFENCEORIGINENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceOriginEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCGEOFENCESTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_GEOFENCE_STATE_ACTIVE_V02 = 1, /**<  The Geofence is being actively monitored.  */
  eQMI_LOC_GEOFENCE_STATE_SUSPEND_V02 = 2, /**<  The Geofence monitoring is suspended.  */
  QMILOCGEOFENCESTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocGeofenceStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to query a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence that is to be queried. */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned with the Query Geofence
       indication. */
}qmiLocQueryGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to query a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Query Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Query Geofence request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence that was queried. */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Query
       Geofence request. This parameter will always be present
       if the status field is set to SUCCESS. */

  /* Optional */
  /*  Geofence Origin */
  uint8_t geofenceOrigin_valid;  /**< Must be set to true if geofenceOrigin is being passed */
  qmiLocGeofenceOriginEnumT_v02 geofenceOrigin;
  /**<   Originator of the Geofence.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_ORIGIN_NETWORK
       \item    0x00000002 -- GEOFENCE_ORIGIN_DEVICE
       \vspace{-0.18in} \end{itemize1}
       */

  /* Optional */
  /*  Position with Respect to Geofence */
  uint8_t posWrtGeofence_valid;  /**< Must be set to true if posWrtGeofence is being passed */
  qmiLocGeofencePositionEnumT_v02 posWrtGeofence;
  /**<   Indicates if the client is currently inside or outside
       the Geofence.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_POSITION_INSIDE
       \item    0x00000002 -- GEOFENCE_POSITION_OUTSIDE
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Circular Geofence Parameters */
  uint8_t circularGeofenceArgs_valid;  /**< Must be set to true if circularGeofenceArgs is being passed */
  qmiLocCircularGeofenceArgsStructT_v02 circularGeofenceArgs;

  /* Optional */
  /*  Geofence State */
  uint8_t geofenceState_valid;  /**< Must be set to true if geofenceState is being passed */
  qmiLocGeofenceStateEnumT_v02 geofenceState;
  /**<   Specifies whether the Geofence is to be actively monitored.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_STATE_ACTIVE
       \item    0x00000002 -- GEOFENCE_STATE_SUSPEND
       \vspace{-0.18in} \end{itemize1} */
}qmiLocQueryGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to edit a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Geofence ID */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence to be edited. */

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Edit Geofence
       request. This parameter will always be present if the
       status field is set to SUCCESS.
 */

  /* Optional */
  /*  Geofence State */
  uint8_t geofenceState_valid;  /**< Must be set to true if geofenceState is being passed */
  qmiLocGeofenceStateEnumT_v02 geofenceState;
  /**<   Specifies whether the Geofence is to be actively monitored.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_STATE_ACTIVE
       \item    0x00000002 -- GEOFENCE_STATE_SUSPEND
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Breach Event Mask */
  uint8_t breachMask_valid;  /**< Must be set to true if breachMask is being passed */
  qmiLocGeofenceBreachMaskT_v02 breachMask;
  /**<   Specifies the breach events in which the client is interested.

       Valid values: \begin{itemize1}
       \item    0x01 -- GEOFENCE_BREACH_ENTERING_MASK
       \item    0x02 -- GEOFENCE_BREACH_LEAVING_MASK
       \vspace{-0.18in} \end{itemize1} */
}qmiLocEditGeofenceReqMsgT_v02;  /* Message */
/**
    @}
  */

typedef uint32_t qmiLocGeofenceConfigParamMaskT_v02;
#define QMI_LOC_GEOFENCE_PARAM_MASK_GEOFENCE_STATE_V02 ((qmiLocGeofenceConfigParamMaskT_v02)0x00000001) /**<  Mask for the Geofence state parameter.  */
#define QMI_LOC_GEOFENCE_PARAM_MASK_BREACH_MASK_V02 ((qmiLocGeofenceConfigParamMaskT_v02)0x00000002) /**<  Mask for Geofence breach mask parameter.  */
/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to edit a Geofence. */
typedef struct {

  /* Mandatory */
  /*  Edit Geofence Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Edit Geofence request.

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Geofence ID */
  uint8_t geofenceId_valid;  /**< Must be set to true if geofenceId is being passed */
  uint32_t geofenceId;
  /**<   Identifier for the Geofence that was edited. */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is specified in the Edit Geofence request. */

  /* Optional */
  /*  Failed Parameters */
  uint8_t failedParams_valid;  /**< Must be set to true if failedParams is being passed */
  qmiLocGeofenceConfigParamMaskT_v02 failedParams;
  /**<   Specified only when the status is not set to SUCCESS. If
       the mask corresponding to a field is set, it indicates that
       the Geofence parameter could not be edited.

       Valid values: \begin{itemize1}
       \item    0x00000001 -- GEOFENCE_PARAM_MASK_GEOFENCE_STATE
       \item    0x00000002 -- GEOFENCE_PARAM_MASK_BREACH_MASK
       \vspace{-0.18in} \end{itemize1} */
}qmiLocEditGeofenceIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to get the best available
                    position estimate from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The transaction ID
       is returned in the Get Best Available Position indication. */
}qmiLocGetBestAvailablePositionReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to get the best available
                    position estimate from the location engine. */
typedef struct {

  /* Mandatory */
  /*  Get Best Available Position Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get Best Available Position request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000006 -- TIMEOUT */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Get Best
       Available Position request. This parameter will
       always be present if the status field is set to
       SUCCESS. */

  /* Optional */
  /*  Latitude */
  uint8_t latitude_valid;  /**< Must be set to true if latitude is being passed */
  double latitude;
  /**<   Latitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item     Range: -90.0 to 90.0      \begin{itemize1}
         \item    Positive values indicate northern latitude
         \item    Negative values indicate southern latitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Longitude */
  uint8_t longitude_valid;  /**< Must be set to true if longitude is being passed */
  double longitude;
  /**<   Longitude (specified in WGS84 datum).
       \begin{itemize1}
       \item    Type: Floating point
       \item    Units: Degrees
       \item    Range: -180.0 to 180.0     \begin{itemize1}
         \item    Positive values indicate eastern longitude
         \item    Negative values indicate western longitude
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */

  /* Optional */
  /*   Circular Horizontal Position Uncertainty */
  uint8_t horUncCircular_valid;  /**< Must be set to true if horUncCircular is being passed */
  float horUncCircular;
  /**<   Horizontal position uncertainty (circular).\n
       - Units: Meters */

  /* Optional */
  /*  Altitude With Respect to Ellipsoid */
  uint8_t altitudeWrtEllipsoid_valid;  /**< Must be set to true if altitudeWrtEllipsoid is being passed */
  float altitudeWrtEllipsoid;
  /**<   Altitude with respect to the WGS84 ellipsoid.\n
       - Units: Meters \n
       - Range: -500 to 15883 */

  /* Optional */
  /*  Vertical Uncertainty */
  uint8_t vertUnc_valid;  /**< Must be set to true if vertUnc is being passed */
  float vertUnc;
  /**<   Vertical uncertainty.\n
       - Units: Meters */

  /* Optional */
  /*  UTC Timestamp */
  uint8_t timestampUtc_valid;  /**< Must be set to true if timestampUtc is being passed */
  uint64_t timestampUtc;
  /**<   UTC timestamp.
       \begin{itemize1}
       \item Units: Milliseconds since Jan. 1, 1970
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Time Uncertainty */
  uint8_t timeUnc_valid;  /**< Must be set to true if timeUnc is being passed */
  float timeUnc;
  /**<   Time uncertainty. \n
       - Units: Milliseconds  */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Minor Axis */
  uint8_t horUncEllipseSemiMinor_valid;  /**< Must be set to true if horUncEllipseSemiMinor is being passed */
  float horUncEllipseSemiMinor;
  /**<   Semi-minor axis of horizontal elliptical uncertainty. \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Semi-Major Axis */
  uint8_t horUncEllipseSemiMajor_valid;  /**< Must be set to true if horUncEllipseSemiMajor is being passed */
  float horUncEllipseSemiMajor;
  /**<   Semi-major axis of horizontal elliptical uncertainty. \n
       - Units: Meters */

  /* Optional */
  /*  Horizontal Elliptical Uncertainty Azimuth */
  uint8_t horUncEllipseOrientAzimuth_valid;  /**< Must be set to true if horUncEllipseOrientAzimuth is being passed */
  float horUncEllipseOrientAzimuth;
  /**<   Elliptical horizontal uncertainty azimuth of orientation. \n
       - Units: Decimal degrees \n
       - Range: 0 to 180 */

  /* Optional */
  /*  Horizontal Circular Confidence */
  uint8_t horCircularConfidence_valid;  /**< Must be set to true if horCircularConfidence is being passed */
  uint8_t horCircularConfidence;
  /**<   Horizontal circular uncertainty confidence. \n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Elliptical Confidence */
  uint8_t horEllipticalConfidence_valid;  /**< Must be set to true if horEllipticalConfidence is being passed */
  uint8_t horEllipticalConfidence;
  /**<   Horizontal elliptical uncertainty confidence. \n
       - Units: Percent \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Horizontal Reliability */
  uint8_t horReliability_valid;  /**< Must be set to true if horReliability is being passed */
  qmiLocReliabilityEnumT_v02 horReliability;
  /**<   Specifies the reliability of the horizontal position.

       Valid values: \begin{itemize1}
       \item    0x00000000 -- RELIABILITY_NOT_SET
       \item    0x00000001 -- RELIABILITY_VERY_LOW
       \item    0x00000002 -- RELIABILITY_LOW
       \item    0x00000003 -- RELIABILITY_MEDIUM
       \item    0x00000004 -- RELIABILITY_HIGH
       \vspace{-0.18in} \end{itemize1}    */

  /* Optional */
  /*  Horizontal Speed */
  uint8_t horSpeed_valid;  /**< Must be set to true if horSpeed is being passed */
  float horSpeed;
  /**<   Horizontal speed. \n
       - Units: Meters/second */

  /* Optional */
  /*  Horizontal Speed Uncertainty */
  uint8_t horSpeedUnc_valid;  /**< Must be set to true if horSpeedUnc is being passed */
  float horSpeedUnc;
  /**<   Horizontal speed uncertainty. \n
       - Units: Meters/second */

  /* Optional */
  /*  Altitude With Respect to Sea Level */
  uint8_t altitudeWrtMeanSeaLevel_valid;  /**< Must be set to true if altitudeWrtMeanSeaLevel is being passed */
  float altitudeWrtMeanSeaLevel;
  /**<   Altitude with respect to mean sea level. \n
       - Units: Meters */

  /* Optional */
  /*  Vertical Confidence */
  uint8_t vertConfidence_valid;  /**< Must be set to true if vertConfidence is being passed */
  uint8_t vertConfidence;
  /**<   Vertical uncertainty confidence. \n
       - Units: Percent    \n
       - Range: 0 to 99 */

  /* Optional */
  /*  Vertical Reliability */
  uint8_t vertReliability_valid;  /**< Must be set to true if vertReliability is being passed */
  qmiLocReliabilityEnumT_v02 vertReliability;
  /**<   Specifies the reliability of the vertical position.

        Valid values: \begin{itemize1}
        \item    0x00000000 -- RELIABILITY_NOT_SET
        \item    0x00000001 -- RELIABILITY_VERY_LOW
        \item    0x00000002 -- RELIABILITY_LOW
        \item    0x00000003 -- RELIABILITY_MEDIUM
        \item    0x00000004 -- RELIABILITY_HIGH
        \vspace{-0.18in} \end{itemize1}*/

  /* Optional */
  /*  Vertical Speed */
  uint8_t vertSpeed_valid;  /**< Must be set to true if vertSpeed is being passed */
  float vertSpeed;
  /**<   Vertical speed. \n
         - Units: Meters/second */

  /* Optional */
  /*  Vertical Speed Uncertainty */
  uint8_t vertSpeedUnc_valid;  /**< Must be set to true if vertSpeedUnc is being passed */
  float vertSpeedUnc;
  /**<   Vertical speed uncertainty. \n
       - Units: Meters/second */

  /* Optional */
  /*  Heading */
  uint8_t heading_valid;  /**< Must be set to true if heading is being passed */
  float heading;
  /**<   Heading. \n
         - Units: Degrees \n
         - Range: 0 to 359.999  */

  /* Optional */
  /*  Heading Uncertainty */
  uint8_t headingUnc_valid;  /**< Must be set to true if headingUnc is being passed */
  float headingUnc;
  /**<   Heading uncertainty. \n
       - Type: Floating point \n
       - Range: 0 to 359.999 */

  /* Optional */
  /*  Magnetic Deviation */
  uint8_t magneticDeviation_valid;  /**< Must be set to true if magneticDeviation is being passed */
  float magneticDeviation;
  /**<   Difference between the bearing to true north and the bearing shown
      on a magnetic compass. The deviation is positive when the magnetic
      north is east of true north. */

  /* Optional */
  /*  Technology Used Mask */
  uint8_t technologyMask_valid;  /**< Must be set to true if technologyMask is being passed */
  qmiLocPosTechMaskT_v02 technologyMask;
  /**<   Technology used in computing this fix.

       Valid bitmasks: \begin{itemize1}
       \item    0x00000001 -- SATELLITE
       \item    0x00000002 -- CELLID
       \item    0x00000004 -- WIFI
       \item    0x00000008 -- SENSORS
       \item    0x00000010 -- REFERENCE_LOCATION
       \item    0x00000020 -- INJECTED_COARSE_POSITION
       \vspace{-0.18in} \end{itemize1} */

  /* Optional */
  /*  Dilution of Precision */
  uint8_t DOP_valid;  /**< Must be set to true if DOP is being passed */
  qmiLocDOPStructT_v02 DOP;

  /* Optional */
  /*  GPS Time */
  uint8_t gpsTime_valid;  /**< Must be set to true if gpsTime is being passed */
  qmiLocGPSTimeStructT_v02 gpsTime;

  /* Optional */
  /*  Time Source */
  uint8_t timeSrc_valid;  /**< Must be set to true if timeSrc is being passed */
  qmiLocTimeSourceEnumT_v02 timeSrc;
  /**<   Time source.
 Valid values: \n
      - eQMI_LOC_TIME_SRC_INVALID (0) --  Invalid time.
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TRANSFER (1) --  Time is set by the 1x system.
      - eQMI_LOC_TIME_SRC_NETWORK_TIME_TAGGING (2) --  Time is set by WCDMA/GSM time tagging (i.e.,
       associating network time with GPS time).
      - eQMI_LOC_TIME_SRC_EXTERNAL_INPUT (3) --  Time is set by an external injection.
      - eQMI_LOC_TIME_SRC_TOW_DECODE (4) --  Time is set after decoding over-the-air GPS navigation data
       from one GPS satellite.
      - eQMI_LOC_TIME_SRC_TOW_CONFIRMED (5) --  Time is set after decoding over-the-air GPS navigation data
       from multiple satellites.
      - eQMI_LOC_TIME_SRC_TOW_AND_WEEK_CONFIRMED (6) --  Both time of the week and the GPS week number are known.
      - eQMI_LOC_TIME_SRC_NAV_SOLUTION (7) --  Time is set by the position engine after the fix is obtained.
      - eQMI_LOC_TIME_SRC_SOLVE_FOR_TIME (8) --  Time is set by the position engine after performing SFT.
       This is done when the clock time uncertainty is large.
      - eQMI_LOC_TIME_SRC_GLO_TOW_DECODE (9) --  Time is set after decoding GLO satellites
      - eQMI_LOC_TIME_SRC_TIME_TRANSFORM (10) --  Time is set after transforming the GPS to GLO time
      - eQMI_LOC_TIME_SRC_WCDMA_SLEEP_TIME_TAGGING (11) --  Time is set by the sleep time tag provided by the WCDMA network
      - eQMI_LOC_TIME_SRC_GSM_SLEEP_TIME_TAGGING (12) --  Time is set by the sleep time tag provided by the GSM network
      - eQMI_LOC_TIME_SRC_UNKNOWN (13) --  Source of the time is unknown
      - eQMI_LOC_TIME_SRC_SYSTEM_TIMETICK (14) --  Time is derived from system clock (better known as slow clock).
       GNSS time is maintained irrespective of the GNSS receiver state
 */

  /* Optional */
  /*  Sensor Data Usage */
  uint8_t sensorDataUsage_valid;  /**< Must be set to true if sensorDataUsage is being passed */
  qmiLocSensorUsageIndicatorStructT_v02 sensorDataUsage;

  /* Optional */
  /*  SVs Used to Calculate the Fix */
  uint8_t gnssSvUsedList_valid;  /**< Must be set to true if gnssSvUsedList is being passed */
  uint32_t gnssSvUsedList_len;  /**< Must be set to # of elements in gnssSvUsedList */
  uint16_t gnssSvUsedList[QMI_LOC_MAX_SV_USED_LIST_LENGTH_V02];
  /**<   Each entry in the list contains the SV ID of a satellite
       used for calculating this position report. The following
       information is associated with each SV ID: \begin{itemize1}
       \item    Range: \begin{itemize1}
         \item    For GPS:     1 to 32
         \item    For SBAS:    33 to 64
         \item    For GLONASS: 65 to 96
         \item    For QZSS:    193 to 197
       \vspace{-0.18in} \end{itemize1} \end{itemize1} */
}qmiLocGetBestAvailablePositionIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCMOTIONSTATEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_MOTION_STATE_UNKNOWN_V02 = 0, /**<  Device state is not known.  */
  eQMI_LOC_MOTION_STATE_STATIONARY_V02 = 1, /**<  Device state is Stationary.  */
  eQMI_LOC_MOTION_STATE_IN_MOTION_V02 = 2, /**<  Device state is In Motion.  */
  QMILOCMOTIONSTATEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocMotionStateEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCMOTIONMODEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_MOTION_MODE_UNKNOWN_V02 = 0, /**<  Device movement is not known.  */
  eQMI_LOC_MOTION_MODE_STATIONARY_V02 = 1, /**<  Device is not moving.  */
  eQMI_LOC_MOTION_MODE_PEDESTRIAN_UNKNOWN_V02 = 200, /**<  Device movement is in Pedestrian mode; nothing else is known about the movement.  */
  eQMI_LOC_MOTION_MODE_PEDESTRIAN_WALKING_V02 = 201, /**<  Device movement is in pedestrian Walking mode.  */
  eQMI_LOC_MOTION_MODE_PEDESTRIAN_RUNNING_V02 = 202, /**<  Device movement is in pedestrian Running mode.  */
  eQMI_LOC_MOTION_MODE_VEHICLE_UNKNOWN_V02 = 300, /**<  Device movement is in Vehicular mode; nothing else is known about the movement.  */
  QMILOCMOTIONMODEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocMotionModeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  qmiLocMotionStateEnumT_v02 motion_state;
  /**<   Current motion state of the user. \n

       Valid values: \begin{itemize1}
       \item eQMI_LOC_MOTION_STATE_UNKNOWN (0) -- Device state is not known.
       \item eQMI_LOC_MOTION_STATE_STATIONARY (1) -- Device state is Stationary.
       \item eQMI_LOC_MOTION_STATE_IN_MOTION (2) -- Device state is In Motion.
       \vspace{0.06in} \end{itemize1}

       Absolute rest and relative rest are both indicated by setting motion_state
       to Stationary. The relative rest state can be distinguished from absolute
       rest by reducing probability_of_state.
  */

  qmiLocMotionModeEnumT_v02 motion_mode;
  /**<   Modes of user motion. \n

       Valid values: \begin{itemize1}
       \item eQMI_LOC_MOTION_MODE_ UNKNOWN (0) -- Device movement is not known.
       \item eQMI_LOC_MOTION_MODE_ STATIONARY (1) -- Device is not moving.
       \item eQMI_LOC_MOTION_MODE_ PEDESTRIAN_UNKNOWN (200) -- Device movement
             is in Pedestrian mode; nothing else is known about the movement.
       \item eQMI_LOC_MOTION_MODE_ PEDESTRIAN_WALKING (201) -- Device movement
             is in Pedestrian Walking mode.
       \item eQMI_LOC_MOTION_MODE_ PEDESTRIAN_RUNNING (202) -- Device movement
             is in Pedestrian Running mode.
       \item eQMI_LOC_MOTION_MODE_ VEHICLE_UNKNOWN (300) -- Device movement is
             in Vehicular mode; nothing else is known about the movement.
       \vspace{0.06in} \end{itemize1}

       The motion_mode value is independent of the motion_state value.
  */

  float probability_of_state;
  /**<   Probability that the device is actually undergoing the motion state
       specified by the combination of the values of motion_state, motion_mode,
       and motion_sub_mode. \vspace{0.06in}

       This value is a floating point number in the range of 0 to 100, in
       units of percent probability. Any value greater than 99.9999 is
       applied as 99.9999. \vspace{0.06in}

       It is recommended that if a particular combination of motion_state and
       motion_mode cannot be determined with more than 50 percent confidence,
       that a more general statement of user motion be made.
       For example, if the mode of In-Motion + Pedestrian-Running can only be
       determined with 50 percent probability, and the simpler statement of In-Motion
       can be determined with 90 percent probability, it is recommended that this field
       be used to simply state In-Motion with 90 percent probability. \vspace{0.06in}

       If the motion_state is not known, the value in this field is not used.
  */

  uint16_t age;
  /**<   Age of the motion data in milliseconds at the time of injection.
  */

  uint16_t timeout;
  /**<   If the age of the motion data input exceeds the timeout value, the data
       will no longer be used. The timeout value is in units of milliseconds.
       Values in the range of 0 to 10000 are accepted. If 65535 is provided,
       the motion data input is applied until the next input is
       received. \vspace{0.06in}

       If the determination of motion data is an instantaneous observation
       and no notice is guaranteed to be given via the QMI on a change in the
       state of the motion data, it is recommended that this field be set to 0. \vspace{0.06in}

       If the determination of motion data is continuously monitored
       external to the QMI and an update is always applied to the QMI upon any
       change in state, a value of 65535 is used for this field.
       Note that in this case, if a certain mode is set and is not later
       unset (e.g., by sending in the request message with a user motion
       state of Unknown), the value is applied indefinitely.
  */
}qmiLocMotionDataStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Injects motion data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Motion Data */
  qmiLocMotionDataStructT_v02 motion_data;
  /**<   Current motion data of the client */
}qmiLocInjectMotionDataReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Injects motion data for MSM GPS service use. */
typedef struct {

  /* Mandatory */
  /*  Inject Motion Data Request Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject Motion Data request.

       Valid values: \begin{itemize1}
         \item 0x00000000 -- SUCCESS
         \item 0x00000001 -- GENERAL_FAILURE
         \item 0x00000002 -- UNSUPPORTED
         \item 0x00000003 -- INVALID_ PARAMETER
         \item 0x00000004 -- ENGINE_BUSY
         \item 0x00000006 -- TIMEOUT
         \vspace{-0.18in} \end{itemize1}
  */
}qmiLocInjectMotionDataIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to retrieve the list of network
                    initiated Geofence IDs. */
typedef struct {

  /* Mandatory */
  /*  Transaction ID */
  uint32_t transactionId;
  /**<   Identifies the transaction. The same transaction ID
       will be returned in the Get NI Geofence ID List indication. */
}qmiLocGetNiGeofenceIdListReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to retrieve the list of network
                    initiated Geofence IDs. */
typedef struct {

  /* Mandatory */
  /*  Get NI Geofence ID List status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Get NI Geofence ID List request.
       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000006 -- TIMEOUT */

  /* Optional */
  /*  Transaction ID */
  uint8_t transactionId_valid;  /**< Must be set to true if transactionId is being passed */
  uint32_t transactionId;
  /**<   Transaction ID that was specified in the Get NI
       Geofence ID List request. */

  /* Optional */
  /*  NI Geofence ID List */
  uint8_t niGeofenceIdList_valid;  /**< Must be set to true if niGeofenceIdList is being passed */
  uint32_t niGeofenceIdList_len;  /**< Must be set to # of elements in niGeofenceIdList */
  uint32_t niGeofenceIdList[QMI_LOC_MAX_NI_GEOFENCE_ID_LIST_LENGTH_V02];
  /**<   List containing the NI Geofence IDs.
       - Type: Array of unsigned 32-bit integers \n
       - Maximum NI Geofence ID List length: 16 */
}qmiLocGetNiGeofenceIdListIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_aggregates
    @{
  */
typedef struct {

  uint32_t MCC;
  /**<   GSM mobile country code. Refer to ITU-T E.212 specification */

  uint32_t MNC;
  /**<   GSM mobile network code. Refer to ITU-T E.212 specification*/

  uint32_t LAC;
  /**<   GSM location area code.Refer to ITU-T E.212 specification */

  uint32_t CID;
  /**<   GSM cell identification.Refer to ITU-T E.212 specification */
}qmiLocGSMCellIdStructT_v02;  /* Type */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used to inject GSM Cell information into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  GSM Cell ID */
  qmiLocGSMCellIdStructT_v02 gsmCellId;
  /**<   Identifies the GSM Cell the device is currently camped on.*/

  /* Mandatory */
  /*  Roaming Status */
  uint8_t roamingStatus;
  /**<   Whether the device is roaming.
       \begin{itemize1}
       \item    0x01 (TRUE)  -- The device is roaming.
       \item    0x00 (FALSE) -- The device is not roaming.
       \vspace{-0.18in} \end{itemize1}*/
}qmiLocInjectGSMCellInfoReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used to inject GSM Cell information into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Inject GSM Cell Info Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject GSM Cell Info request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000006 -- TIMEOUT */
}qmiLocInjectGSMCellInfoIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_enums
    @{
  */
typedef enum {
  QMILOCINJECTEDNETWORKINITIATEDMESSAGETYPEENUMT_MIN_ENUM_VAL_V02 = -2147483647, /**< To force a 32 bit signed enum.  Do not change or use*/
  eQMI_LOC_INJECTED_NETWORK_INITIATED_MESSAGE_TYPE_SUPL_V02 = 0, /**<  SUPL netwrok initiated message is being injected.  */
  QMILOCINJECTEDNETWORKINITIATEDMESSAGETYPEENUMT_MAX_ENUM_VAL_V02 = 2147483647 /**< To force a 32 bit signed enum.  Do not change or use*/
}qmiLocInjectedNetworkInitiatedMessageTypeEnumT_v02;
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used to inject a network initiated message into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Injected Network Initiated Message Type */
  qmiLocInjectedNetworkInitiatedMessageTypeEnumT_v02 injectedNIMessageType;
  /**<   Type of the network initiated message being injected
 Valid Values:
      - eQMI_LOC_INJECTED_NETWORK_INITIATED_MESSAGE_TYPE_SUPL (0) --  SUPL netwrok initiated message is being injected.  */

  /* Mandatory */
  /*  Injected Network Initiated Message */
  uint32_t injectedNIMessage_len;  /**< Must be set to # of elements in injectedNIMessage */
  uint8_t injectedNIMessage[QMI_LOC_MAX_INJECTED_NETWORK_INITIATED_MESSAGE_LENGTH_V02];
  /**<   Network initiated message body.
       If the inject NI message type is TYPE_SUPL, the message contains
       a SUPL INIT message as defined in OMA-TS-ULP-V2_0-20110527-C specification. */
}qmiLocInjectNetworkInitiatedMessageReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used to inject a network initiated message into the location
                    engine. */
typedef struct {

  /* Mandatory */
  /*  Inject Network Initiated Message Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Inject Network Initiated Message request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000006 -- TIMEOUT \n
         - 0x00000008 -- INSUFFICIENT_MEMORY \n */
}qmiLocInjectNetworkInitiatedMessageIndMsgT_v02;  /* Message */
/**
    @}
  */

/*
 * qmiLocWWANOutOfServiceNotificationReqMsgT is empty
 * typedef struct {
 * }qmiLocWWANOutOfServiceNotificationReqMsgT_v02;
 */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used to notify the location engine that the device is
                    no longer camped. */
typedef struct {

  /* Mandatory */
  /*  Notify WWAN Out of Service Status */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the Notify WWAN out of service request.

       Valid values: \n
         - 0x00000000 -- SUCCESS \n
         - 0x00000001 -- GENERAL_FAILURE \n
         - 0x00000002 -- UNSUPPORTED \n
         - 0x00000003 -- INVALID_PARAMETER \n
         - 0x00000004 -- ENGINE_BUSY \n
         - 0x00000006 -- TIMEOUT \n
         */
}qmiLocWWANOutOfServiceNotificationIndMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Request Message; Used by the control point to inject pedometer data
                    into the location engine. */
typedef struct {

  /* Mandatory */
  /*  Time Source */
  qmiLocSensorDataTimeSourceEnumT_v02 timeSource;
  /**<   Time source for the pedometer. Location service will use
 this field to identify the time reference used in the
 pedometer data timestamp. Values: \n
      - eQMI_LOC_SENSOR_TIME_SOURCE_UNSPECIFIED (0) --  The sensor time source is unspecified
      - eQMI_LOC_SENSOR_TIME_SOURCE_COMMON (1) --  The time source is common between the sensors and
       the location engine*/

  /* Mandatory */
  /*  Pedometer report timestamp */
  uint32_t timestamp;
  /**<   Timestamp of the last step event in this report, i.e timestamp
       of the step event that caused this report to be generated.
       The timestamp is in the time reference scale that is
       used by the pedometer time source. \n
       - Unit: Milliseconds */

  /* Mandatory */
  /*  Time Interval */
  uint32_t timeInterval;
  /**<   Time interval during which the step count was calculated. Subtracting
       timeInterval from the timestamp field will yield the the time when
       the step detection for the first step in this report started.
       Unit : Milliseconds. */

  /* Mandatory */
  /*  Step Count */
  uint32_t stepCount;
  /**<   Number for steps counted during the time interval.*/

  /* Optional */
  /*  Step Confidence */
  uint8_t stepConfidence_valid;  /**< Must be set to true if stepConfidence is being passed */
  uint8_t stepConfidence;
  /**<   Confidence associated with the step. This field is only applicable
       for a single step report, i.e if the stepCount is one.
       Range: 0 to 100.
       Note: The report will be ignored if confidence is 0. */

  /* Optional */
  /*  Step Count Uncertainty */
  uint8_t stepCountUncertainty_valid;  /**< Must be set to true if stepCountUncertainty is being passed */
  float stepCountUncertainty;
  /**<   Uncertainty (in steps) associated with the step count. */

  /* Optional */
  /*  Step Rate */
  uint8_t stepRate_valid;  /**< Must be set to true if stepRate is being passed */
  float stepRate;
  /**<   Current estimate for the rate of steps per second.
       Units: steps/second
       Range: >= 0.0
  */
}qmiLocPedometerReportReqMsgT_v02;  /* Message */
/**
    @}
  */

/** @addtogroup loc_qmi_messages
    @{
  */
/** Indication Message; Used by the control point to inject pedometer data
                    into the location engine. */
typedef struct {

  /* Mandatory */
  /*  Status of Pedometer report request */
  qmiLocStatusEnumT_v02 status;
  /**<   Status of the pedometer report request

       Valid values: \begin{itemize1}
        \item    0x00000000 -- SUCCESS
        \item    0x00000001 -- GENERAL_FAILURE
        \item    0x00000002 -- UNSUPPORTED
        \item    0x00000003 -- INVALID_PARAMETER
        \item    0x00000004 -- ENGINE_BUSY
        \item    0x00000005 -- PHONE_OFFLINE
        \item    0x00000006 -- TIMEOUT
        \vspace{-0.18in} \end{itemize1}
  */
}qmiLocPedometerReportIndMsgT_v02;  /* Message */
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
#define QMI_LOC_EVENT_NI_GEOFENCE_NOTIFICATION_IND_V02 0x0060
#define QMI_LOC_EVENT_GEOFENCE_GEN_ALERT_IND_V02 0x0061
#define QMI_LOC_EVENT_GEOFENCE_BREACH_NOTIFICATION_IND_V02 0x0062
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_REQ_V02 0x0063
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_RESP_V02 0x0063
#define QMI_LOC_ADD_CIRCULAR_GEOFENCE_IND_V02 0x0063
#define QMI_LOC_DELETE_GEOFENCE_REQ_V02 0x0064
#define QMI_LOC_DELETE_GEOFENCE_RESP_V02 0x0064
#define QMI_LOC_DELETE_GEOFENCE_IND_V02 0x0064
#define QMI_LOC_QUERY_GEOFENCE_REQ_V02 0x0065
#define QMI_LOC_QUERY_GEOFENCE_RESP_V02 0x0065
#define QMI_LOC_QUERY_GEOFENCE_IND_V02 0x0065
#define QMI_LOC_EDIT_GEOFENCE_REQ_V02 0x0066
#define QMI_LOC_EDIT_GEOFENCE_RESP_V02 0x0066
#define QMI_LOC_EDIT_GEOFENCE_IND_V02 0x0066
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_REQ_V02 0x0067
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_RESP_V02 0x0067
#define QMI_LOC_GET_BEST_AVAILABLE_POSITION_IND_V02 0x0067
#define QMI_LOC_INJECT_MOTION_DATA_REQ_V02 0x0068
#define QMI_LOC_INJECT_MOTION_DATA_RESP_V02 0x0068
#define QMI_LOC_INJECT_MOTION_DATA_IND_V02 0x0068
#define QMI_LOC_GET_NI_GEOFENCE_ID_LIST_REQ_V02 0x0069
#define QMI_LOC_GET_NI_GEOFENCE_ID_LIST_RESP_V02 0x0069
#define QMI_LOC_GET_NI_GEOFENCE_ID_LIST_IND_V02 0x0069
#define QMI_LOC_INJECT_GSM_CELL_INFO_REQ_V02 0x006A
#define QMI_LOC_INJECT_GSM_CELL_INFO_RESP_V02 0x006A
#define QMI_LOC_INJECT_GSM_CELL_INFO_IND_V02 0x006A
#define QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_REQ_V02 0x006B
#define QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_RESP_V02 0x006B
#define QMI_LOC_INJECT_NETWORK_INITIATED_MESSAGE_IND_V02 0x006B
#define QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_REQ_V02 0x006C
#define QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_RESP_V02 0x006C
#define QMI_LOC_WWAN_OUT_OF_SERVICE_NOTIFICATION_IND_V02 0x006C
#define QMI_LOC_EVENT_PEDOMETER_CONTROL_IND_V02 0x006D
#define QMI_LOC_EVENT_MOTION_DATA_CONTROL_IND_V02 0x006E
#define QMI_LOC_PEDOMETER_REPORT_REQ_V02 0x006F
#define QMI_LOC_PEDOMETER_REPORT_RESP_V02 0x006F
#define QMI_LOC_PEDOMETER_REPORT_IND_V02 0x006F
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

