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

#include "stdint.h"
#include "qmi_idl_lib_internal.h"
#include "location_service_v02.h"
#include "common_v01.h"


/*Type Definitions*/
static const uint8_t qmiLocGPSTimeStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocGPSTimeStructT_v02, gpsWeek),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGPSTimeStructT_v02, gpsTimeOfWeekMs),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocDOPStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, PDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, HDOP),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDOPStructT_v02, VDOP),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSensorUsageIndicatorStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorUsageIndicatorStructT_v02, usageMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorUsageIndicatorStructT_v02, aidingIndicatorMask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSvInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, validMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, system),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, gnssSvId),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, healthStatus),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, svStatus),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, svInfoMask),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, elevation),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, azimuth),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSvInfoStructT_v02, snr),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiVxNotifyVerifyStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, posQosIncl),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, posQos),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, numFixes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, timeBetweenFixes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, posMode),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, encodingScheme),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, requestorId),
  QMI_LOC_NI_MAX_REQUESTOR_ID_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, requestorId) - QMI_IDL_OFFSET8(qmiLocNiVxNotifyVerifyStructT_v02, requestorId_len),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiVxNotifyVerifyStructT_v02, userRespTimerInSeconds),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiSuplFormattedStringStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplFormattedStringStructT_v02, formatType),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplFormattedStringStructT_v02, formattedString),
  QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocNiSuplFormattedStringStructT_v02, formattedString) - QMI_IDL_OFFSET8(qmiLocNiSuplFormattedStringStructT_v02, formattedString_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiSuplQopStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplQopStructT_v02, validMask),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplQopStructT_v02, horizontalAccuracy),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplQopStructT_v02, verticalAccuracy),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplQopStructT_v02, maxLocAge),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplQopStructT_v02, delay),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocIpV4AddrStructType_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocIpV4AddrStructType_v02, addr),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocIpV4AddrStructType_v02, port),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocIpV6AddrStructType_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocIpV6AddrStructType_v02, addr),
  QMI_LOC_IPV6_ADDR_LENGTH_V02,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocIpV6AddrStructType_v02, port),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiSuplServerInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplServerInfoStructT_v02, suplServerAddrTypeMask),

   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiSuplServerInfoStructT_v02, ipv4Addr),
 7, 0,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiSuplServerInfoStructT_v02, ipv6Addr),
 8, 0,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocNiSuplServerInfoStructT_v02, urlAddr),
  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiSuplNotifyVerifyStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiSuplNotifyVerifyStructT_v02, valid_flags),

   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiSuplNotifyVerifyStructT_v02, suplServerInfo),
 9, 0,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, suplSessionId),
  QMI_LOC_NI_SUPL_SLP_SESSION_ID_BYTE_LENGTH_V02,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, suplHash),
  QMI_LOC_NI_SUPL_HASH_LENGTH_V02,

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, posMethod),

  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, dataCodingScheme),

  QMI_IDL_FLAGS_OFFSET_IS_16 |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, requestorId),
 5, 0,
  QMI_IDL_FLAGS_OFFSET_IS_16 |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, clientName),
 5, 0,
  QMI_IDL_FLAGS_OFFSET_IS_16 |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, suplQop),
 6, 0,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiSuplNotifyVerifyStructT_v02, userResponseTimer),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiUmtsCpCodedStringStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpCodedStringStructT_v02, dataCodingScheme),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpCodedStringStructT_v02, codedString),
  QMI_LOC_NI_CODEWORD_MAX_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpCodedStringStructT_v02, codedString) - QMI_IDL_OFFSET8(qmiLocNiUmtsCpCodedStringStructT_v02, codedString_len),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiUmtsCpNotifyVerifyStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, valid_flags),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, invokeId),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, dataCodingScheme),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, notificationText),
  QMI_LOC_NI_MAX_CLIENT_NAME_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, notificationText) - QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, notificationText_len),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, clientAddress),
  QMI_LOC_NI_MAX_EXT_CLIENT_ADDRESS_V02,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, clientAddress) - QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, clientAddress_len),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, locationType),

   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, requestorId),
 11, 0,
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, codewordString),
 11, 0,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, lcsServiceTypeId),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUmtsCpNotifyVerifyStructT_v02, userResponseTimer),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocNiVxServiceInteractionStructT_data_v02[] = {
   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiVxServiceInteractionStructT_v02, niVxReq),
 4, 0,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiVxServiceInteractionStructT_v02, serviceInteractionType),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocAssistanceServerUrlStructT_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocAssistanceServerUrlStructT_v02, serverUrl),
  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02,

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocTimeServerListStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, delayThreshold),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList),
  QMI_LOC_MAX_NTP_SERVERS_V02,
  QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList) - QMI_IDL_OFFSET8(qmiLocTimeServerListStructT_v02, timeServerList_len),
 14, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsAllowedSizesStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsAllowedSizesStructT_v02, maxFileSizeInBytes),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsAllowedSizesStructT_v02, maxPartSize),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsServerListStructT_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList),
  QMI_LOC_MAX_PREDICTED_ORBITS_SERVERS_V02,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocPredictedOrbitsServerListStructT_v02, serverList_len),
 14, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSensorControlConfigSamplingSpecStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorControlConfigSamplingSpecStructT_v02, samplesPerBatch),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorControlConfigSamplingSpecStructT_v02, batchesPerSecond),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocSensorReadyStatusStructT_data_v02[] = {
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSensorReadyStatusStructT_v02, injectEnable),

   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSensorReadyStatusStructT_v02, dataFrequency),
 18, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocPredictedOrbitsDataValidityStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsDataValidityStructT_v02, startTimeInUTC),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocPredictedOrbitsDataValidityStructT_v02, durationHours),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocAltitudeSrcInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, source),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, linkage),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocAltitudeSrcInfoStructT_v02, coverage),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocDeleteSvInfoStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSvInfoStructT_v02, gnssSvId),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSvInfoStructT_v02, system),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSvInfoStructT_v02, deleteSvInfoMask),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocWifiFixTimeStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixTimeStructT_v02, wifiPositionTime),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocWifiFixPosStructT_data_v02[] = {
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixPosStructT_v02, lat),

  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixPosStructT_v02, lon),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixPosStructT_v02, hepe),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixPosStructT_v02, numApsUsed),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiFixPosStructT_v02, fixErrorCode),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocWifiApInfoStructT_data_v02[] = {
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiApInfoStructT_v02, macAddr),
  QMI_LOC_WIFI_MAC_ADDR_LENGTH_V02,

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiApInfoStructT_v02, rssi),

  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiApInfoStructT_v02, channel),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocWifiApInfoStructT_v02, apQualifier),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLoc3AxisSensorSampleStructT_data_v02[] = {
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleStructT_v02, timeOffset),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleStructT_v02, xAxis),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleStructT_v02, yAxis),

  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleStructT_v02, zAxis),

  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLoc3AxisSensorSampleListStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleListStructT_v02, timeOfFirstSample),

  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleListStructT_v02, flags),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleListStructT_v02, sensorData),
  QMI_LOC_SENSOR_DATA_MAX_SAMPLES_V02,
  QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleListStructT_v02, sensorData) - QMI_IDL_OFFSET8(qmiLoc3AxisSensorSampleListStructT_v02, sensorData_len),
 26, 0,
  QMI_IDL_FLAG_END_VALUE
};

static const uint8_t qmiLocApnProfilesStructT_data_v02[] = {
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocApnProfilesStructT_v02, pdnType),

  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocApnProfilesStructT_v02, apnName),
  QMI_LOC_MAX_APN_NAME_LENGTH_V02,

  QMI_IDL_FLAG_END_VALUE
};

/*Message Definitions*/
static const uint8_t qmiLocGenRespMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGenRespMsgT_v02, resp),
  0, 1
};

static const uint8_t qmiLocInformClientRevisionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformClientRevisionReqMsgT_v02, revision)
};

static const uint8_t qmiLocRegEventsReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocRegEventsReqMsgT_v02, eventRegMask)
};

static const uint8_t qmiLocStartReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, sessionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, fixRecurrence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, horizontalAccuracyLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, intermediateReportState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval) - QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval_valid)),
  0x13,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocStartReqMsgT_v02, minInterval)
};

static const uint8_t qmiLocStopReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocStopReqMsgT_v02, sessionId)
};

static const uint8_t qmiLocEventPositionReportIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, sessionStatus),

  0x02,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, sessionId),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude_valid)),
  0x10,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude_valid)),
  0x11,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor_valid)),
  0x13,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMinor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor_valid)),
  0x14,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseSemiMajor),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth_valid)),
  0x15,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horUncEllipseOrientAzimuth),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence_valid)),
  0x16,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability_valid)),
  0x17,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal_valid)),
  0x18,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedHorizontal),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc_valid)),
  0x19,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x1A,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x1B,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc_valid)),
  0x1C,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence_valid)),
  0x1D,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability_valid)),
  0x1E,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical_valid)),
  0x1F,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, speedVertical),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading_valid)),
  0x20,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, heading),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc_valid)),
  0x21,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, headingUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation_valid)),
  0x22,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, magneticDeviation),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask_valid)),
  0x23,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, technologyMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP_valid)),
  0x24,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, DOP),
  1, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc_valid)),
  0x25,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds_valid)),
  0x26,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, leapSeconds),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime_valid)),
  0x27,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, gpsTime),
  0, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc) - QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc_valid)),
  0x28,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventPositionReportIndMsgT_v02, timeUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, timeSrc) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, timeSrc_valid)),
  0x29,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, timeSrc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage_valid)),
  0x2A,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, sensorDataUsage),
  2, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, fixId) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventPositionReportIndMsgT_v02, fixId_valid)),
  0x2B,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventPositionReportIndMsgT_v02, fixId)
};

static const uint8_t qmiLocEventGnssSvInfoIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, altitudeAssumed),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList) - QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList),
  QMI_LOC_SV_INFO_LIST_MAX_SIZE_V02,
  QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList) - QMI_IDL_OFFSET8(qmiLocEventGnssSvInfoIndMsgT_v02, svList_len),
  3, 0
};

static const uint8_t qmiLocEventNmeaIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocEventNmeaIndMsgT_v02, nmea),
  QMI_LOC_NMEA_STRING_MAX_LENGTH_V02
};

static const uint8_t qmiLocEventNiNotifyVerifyReqIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, notificationType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxInd) - QMI_IDL_OFFSET8(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxInd_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxInd),
  4, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiSuplInd) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiSuplInd_valid)),
  0x11,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiSuplInd),
  10, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiUmtsCpInd) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiUmtsCpInd_valid)),
  0x12,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiUmtsCpInd),
  12, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxServiceInteractionInd) - QMI_IDL_OFFSET16RELATIVE(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxServiceInteractionInd_valid)),
  0x13,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocEventNiNotifyVerifyReqIndMsgT_v02, NiVxServiceInteractionInd),
  13, 0
};

static const uint8_t qmiLocEventInjectTimeReqIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo) - QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectTimeReqIndMsgT_v02, timeServerInfo),
  15, 0
};

static const uint8_t qmiLocEventInjectPredictedOrbitsReqIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, allowedSizes),
  16, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02, serverList),
  17, 0
};

static const uint8_t qmiLocEventInjectPositionReqIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPositionReqIndMsgT_v02, latitude),

  0x02,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPositionReqIndMsgT_v02, longitude),

  0x03,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPositionReqIndMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x04,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventInjectPositionReqIndMsgT_v02, timestampUtc)
};

static const uint8_t qmiLocEventEngineStateIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventEngineStateIndMsgT_v02, engineState)
};

static const uint8_t qmiLocEventFixSessionStateIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId) - QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventFixSessionStateIndMsgT_v02, sessionId)
};

static const uint8_t qmiLocEventWifiReqIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventWifiReqIndMsgT_v02, requestType),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventWifiReqIndMsgT_v02, tbfInMs) - QMI_IDL_OFFSET8(qmiLocEventWifiReqIndMsgT_v02, tbfInMs_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventWifiReqIndMsgT_v02, tbfInMs)
};

static const uint8_t qmiLocEventSensorStreamingReadyStatusIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, accelReady) - QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, accelReady_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, accelReady),
  19, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, gyroReady) - QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, gyroReady_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02, gyroReady),
  19, 0
};

static const uint8_t qmiLocEventTimeSyncReqIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventTimeSyncReqIndMsgT_v02, refCounter)
};

static const uint8_t qmiLocEventSetSpiStreamingReportIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventSetSpiStreamingReportIndMsgT_v02, enable)
};

static const uint8_t qmiLocEventLocationServerConnectionReqIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventLocationServerConnectionReqIndMsgT_v02, connHandle),

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventLocationServerConnectionReqIndMsgT_v02, requestType),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocEventLocationServerConnectionReqIndMsgT_v02, wwanType)
};

/*
 * qmiLocGetServiceRevisionReqMsgT is empty
 * static const uint8_t qmiLocGetServiceRevisionReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetServiceRevisionIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServiceRevisionIndMsgT_v02, revision)
};

/*
 * qmiLocGetFixCriteriaReqMsgT is empty
 * static const uint8_t qmiLocGetFixCriteriaReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetFixCriteriaIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, horizontalAccuracyLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, intermediateReportState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval) - QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetFixCriteriaIndMsgT_v02, minInterval)
};

static const uint8_t qmiLocNiUserRespReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUserRespReqMsgT_v02, userResp),

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUserRespReqMsgT_v02, notificationType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocNiUserRespReqMsgT_v02, NiVxPayload) - QMI_IDL_OFFSET8(qmiLocNiUserRespReqMsgT_v02, NiVxPayload_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocNiUserRespReqMsgT_v02, NiVxPayload),
  4, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiSuplPayload) - QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiSuplPayload_valid)),
  0x11,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiUserRespReqMsgT_v02, NiSuplPayload),
  10, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiUmtsCpPayload) - QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiUmtsCpPayload_valid)),
  0x12,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiUserRespReqMsgT_v02, NiUmtsCpPayload),
  12, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiVxServiceInteractionPayload) - QMI_IDL_OFFSET16RELATIVE(qmiLocNiUserRespReqMsgT_v02, NiVxServiceInteractionPayload_valid)),
  0x13,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocNiUserRespReqMsgT_v02, NiVxServiceInteractionPayload),
  13, 0
};

static const uint8_t qmiLocNiUserRespIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNiUserRespIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectPredictedOrbitsDataReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, totalSize),

  0x02,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, totalParts),

  0x03,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, partNum),

  0x04,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, partData),
  ((QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02) & 0xFF), ((QMI_LOC_MAX_PREDICTED_ORBITS_PART_LEN_V02) >> 8),
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, partData) - QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, partData_len),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, formatType) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, formatType_valid)),
  0x10,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectPredictedOrbitsDataReqMsgT_v02, formatType)
};

static const uint8_t qmiLocInjectPredictedOrbitsDataIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataIndMsgT_v02, partNum) - QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataIndMsgT_v02, partNum_valid)),
  0x10,
  QMI_IDL_GENERIC_2_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPredictedOrbitsDataIndMsgT_v02, partNum)
};

/*
 * qmiLocGetPredictedOrbitsDataSourceReqMsgT is empty
 * static const uint8_t qmiLocGetPredictedOrbitsDataSourceReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetPredictedOrbitsDataSourceIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, allowedSizes),
  16, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02, serverList),
  17, 0
};

/*
 * qmiLocGetPredictedOrbitsDataValidityReqMsgT is empty
 * static const uint8_t qmiLocGetPredictedOrbitsDataValidityReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetPredictedOrbitsDataValidityIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo) - QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02, validityInfo),
  20, 0
};

static const uint8_t qmiLocInjectUtcTimeReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeReqMsgT_v02, timeUtc),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeReqMsgT_v02, timeUnc)
};

static const uint8_t qmiLocInjectUtcTimeIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectUtcTimeIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectPositionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude_valid)),
  0x10,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, latitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude_valid)),
  0x11,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, longitude),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horUncCircular),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence_valid)),
  0x13,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability_valid)),
  0x14,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, horReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid_valid)),
  0x15,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtEllipsoid),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel_valid)),
  0x16,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altitudeWrtMeanSeaLevel),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc_valid)),
  0x17,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertUnc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence_valid)),
  0x18,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertConfidence),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability_valid)),
  0x19,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, vertReliability),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo_valid)),
  0x1A,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, altSourceInfo),
  21, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc_valid)),
  0x1B,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampUtc),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge_valid)),
  0x1C,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, timestampAge),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc) - QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc_valid)),
  0x1D,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionReqMsgT_v02, positionSrc)
};

static const uint8_t qmiLocInjectPositionIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectPositionIndMsgT_v02, status)
};

static const uint8_t qmiLocSetEngineLockReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetEngineLockReqMsgT_v02, lockType)
};

static const uint8_t qmiLocSetEngineLockIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetEngineLockIndMsgT_v02, status)
};

/*
 * qmiLocGetEngineLockReqMsgT is empty
 * static const uint8_t qmiLocGetEngineLockReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetEngineLockIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetEngineLockIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetEngineLockIndMsgT_v02, lockType) - QMI_IDL_OFFSET8(qmiLocGetEngineLockIndMsgT_v02, lockType_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetEngineLockIndMsgT_v02, lockType)
};

static const uint8_t qmiLocSetSbasConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSbasConfigReqMsgT_v02, sbasConfig)
};

static const uint8_t qmiLocSetSbasConfigIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSbasConfigIndMsgT_v02, status)
};

/*
 * qmiLocGetSbasConfigReqMsgT is empty
 * static const uint8_t qmiLocGetSbasConfigReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetSbasConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSbasConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSbasConfigIndMsgT_v02, sbasConfig) - QMI_IDL_OFFSET8(qmiLocGetSbasConfigIndMsgT_v02, sbasConfig_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSbasConfigIndMsgT_v02, sbasConfig)
};

static const uint8_t qmiLocSetNmeaTypesReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetNmeaTypesReqMsgT_v02, nmeaSentenceType)
};

static const uint8_t qmiLocSetNmeaTypesIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetNmeaTypesIndMsgT_v02, status)
};

/*
 * qmiLocGetNmeaTypesReqMsgT is empty
 * static const uint8_t qmiLocGetNmeaTypesReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetNmeaTypesIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetNmeaTypesIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetNmeaTypesIndMsgT_v02, nmeaSentenceType) - QMI_IDL_OFFSET8(qmiLocGetNmeaTypesIndMsgT_v02, nmeaSentenceType_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetNmeaTypesIndMsgT_v02, nmeaSentenceType)
};

static const uint8_t qmiLocSetLowPowerModeReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetLowPowerModeReqMsgT_v02, lowPowerMode)
};

static const uint8_t qmiLocSetLowPowerModeIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetLowPowerModeIndMsgT_v02, status)
};

/*
 * qmiLocGetLowPowerModeReqMsgT is empty
 * static const uint8_t qmiLocGetLowPowerModeReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetLowPowerModeIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLowPowerModeIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetLowPowerModeIndMsgT_v02, lowPowerMode) - QMI_IDL_OFFSET8(qmiLocGetLowPowerModeIndMsgT_v02, lowPowerMode_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetLowPowerModeIndMsgT_v02, lowPowerMode)
};

static const uint8_t qmiLocSetServerReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, serverType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv4Addr) - QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv4Addr_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv4Addr),
  7, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv6Addr) - QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv6Addr_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, ipv6Addr),
  8, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, urlAddr) - QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, urlAddr_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocSetServerReqMsgT_v02, urlAddr),
  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02
};

static const uint8_t qmiLocSetServerIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetServerIndMsgT_v02, status)
};

static const uint8_t qmiLocGetServerReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServerReqMsgT_v02, serverType),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServerReqMsgT_v02, serverAddrTypeMask) - QMI_IDL_OFFSET8(qmiLocGetServerReqMsgT_v02, serverAddrTypeMask_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServerReqMsgT_v02, serverAddrTypeMask)
};

static const uint8_t qmiLocGetServerIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, status),

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, serverType),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv4Addr) - QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv4Addr_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv4Addr),
  7, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv6Addr) - QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv6Addr_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, ipv6Addr),
  8, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, urlAddr) - QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, urlAddr_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_STRING,
  QMI_IDL_OFFSET8(qmiLocGetServerIndMsgT_v02, urlAddr),
  QMI_LOC_MAX_SERVER_ADDR_LENGTH_V02
};

static const uint8_t qmiLocDeleteAssistDataReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteAllFlag),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteSvInfoList) - QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteSvInfoList_valid)),
  0x10,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteSvInfoList),
  QMI_LOC_DELETE_MAX_SV_INFO_LENGTH_V02,
  QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteSvInfoList) - QMI_IDL_OFFSET8(qmiLocDeleteAssistDataReqMsgT_v02, deleteSvInfoList_len),
  22, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteGnssDataMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteGnssDataMask_valid)),
  0x11,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocDeleteAssistDataReqMsgT_v02, deleteGnssDataMask),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteCellDbDataMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteCellDbDataMask_valid)),
  0x12,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocDeleteAssistDataReqMsgT_v02, deleteCellDbDataMask),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteClockInfoMask) - QMI_IDL_OFFSET16RELATIVE(qmiLocDeleteAssistDataReqMsgT_v02, deleteClockInfoMask_valid)),
  0x13,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocDeleteAssistDataReqMsgT_v02, deleteClockInfoMask)
};

static const uint8_t qmiLocDeleteAssistDataIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteAssistDataIndMsgT_v02, status)
};

static const uint8_t qmiLocSetXtraTSessionControlReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraTSessionControlReqMsgT_v02, xtraTSessionControl)
};

static const uint8_t qmiLocSetXtraTSessionControlIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetXtraTSessionControlIndMsgT_v02, status)
};

/*
 * qmiLocGetXtraTSessionControlReqMsgT is empty
 * static const uint8_t qmiLocGetXtraTSessionControlReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetXtraTSessionControlIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl) - QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetXtraTSessionControlIndMsgT_v02, xtraTSessionControl)
};

static const uint8_t qmiLocInjectWifiPositionReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixTime) - QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixTime_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixTime),
  23, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixPosition) - QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixPosition_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, wifiFixPosition),
  24, 0,

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, apInfo) - QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, apInfo_valid)),
  0x12,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN |   QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, apInfo),
  QMI_LOC_WIFI_MAX_REPORTED_APS_PER_MSG_V02,
  QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, apInfo) - QMI_IDL_OFFSET8(qmiLocInjectWifiPositionReqMsgT_v02, apInfo_len),
  25, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectWifiPositionReqMsgT_v02, horizontalReliability) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectWifiPositionReqMsgT_v02, horizontalReliability_valid)),
  0x13,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectWifiPositionReqMsgT_v02, horizontalReliability)
};

static const uint8_t qmiLocInjectWifiPositionIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectWifiPositionIndMsgT_v02, status)
};

static const uint8_t qmiLocNotifyWifiStatusReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNotifyWifiStatusReqMsgT_v02, wifiStatus)
};

static const uint8_t qmiLocNotifyWifiStatusIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocNotifyWifiStatusIndMsgT_v02, status)
};

/*
 * qmiLocGetRegisteredEventsReqMsgT is empty
 * static const uint8_t qmiLocGetRegisteredEventsReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetRegisteredEventsIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask) - QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask_valid)),
  0x10,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetRegisteredEventsIndMsgT_v02, eventRegMask)
};

static const uint8_t qmiLocSetOperationModeReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetOperationModeReqMsgT_v02, operationMode)
};

static const uint8_t qmiLocSetOperationModeIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetOperationModeIndMsgT_v02, status)
};

/*
 * qmiLocGetOperationModeReqMsgT is empty
 * static const uint8_t qmiLocGetOperationModeReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetOperationModeIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetOperationModeIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetOperationModeIndMsgT_v02, operationMode) - QMI_IDL_OFFSET8(qmiLocGetOperationModeIndMsgT_v02, operationMode_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetOperationModeIndMsgT_v02, operationMode)
};

static const uint8_t qmiLocSetSpiStatusReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSpiStatusReqMsgT_v02, stationary),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSpiStatusReqMsgT_v02, confidenceStationary) - QMI_IDL_OFFSET8(qmiLocSetSpiStatusReqMsgT_v02, confidenceStationary_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSpiStatusReqMsgT_v02, confidenceStationary)
};

static const uint8_t qmiLocSetSpiStatusIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSpiStatusIndMsgT_v02, status)
};

static const uint8_t qmiLocInjectSensorDataReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, opaqueIdentifier) - QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, opaqueIdentifier_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, opaqueIdentifier),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, threeAxisAccelData) - QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, threeAxisAccelData_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataReqMsgT_v02, threeAxisAccelData),
  27, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET16RELATIVE(qmiLocInjectSensorDataReqMsgT_v02, threeAxisGyroData) - QMI_IDL_OFFSET16RELATIVE(qmiLocInjectSensorDataReqMsgT_v02, threeAxisGyroData_valid)),
  0x12,
  QMI_IDL_FLAGS_OFFSET_IS_16 | QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET16ARRAY(qmiLocInjectSensorDataReqMsgT_v02, threeAxisGyroData),
  27, 0
};

static const uint8_t qmiLocInjectSensorDataIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, opaqueIdentifier) - QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, opaqueIdentifier_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, opaqueIdentifier),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisAccelSamplesAccepted) - QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisAccelSamplesAccepted_valid)),
  0x11,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisAccelSamplesAccepted),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisGyroSamplesAccepted) - QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisGyroSamplesAccepted_valid)),
  0x12,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSensorDataIndMsgT_v02, threeAxisGyroSamplesAccepted)
};

static const uint8_t qmiLocInjectTimeSyncDataReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTimeSyncDataReqMsgT_v02, refCounter),

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTimeSyncDataReqMsgT_v02, sensorProcRxTime),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x03,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTimeSyncDataReqMsgT_v02, sensorProcTxTime)
};

static const uint8_t qmiLocInjectTimeSyncDataIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectTimeSyncDataIndMsgT_v02, status)
};

/*
 * qmiLocGetCradleMountConfigReqMsgT is empty
 * static const uint8_t qmiLocGetCradleMountConfigReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetCradleMountConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, cradleMountState) - QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, cradleMountState_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, cradleMountState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, confidenceCradleMountState) - QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, confidenceCradleMountState_valid)),
  0x11,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetCradleMountConfigIndMsgT_v02, confidenceCradleMountState)
};

static const uint8_t qmiLocSetCradleMountConfigReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetCradleMountConfigReqMsgT_v02, cradleMountState),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetCradleMountConfigReqMsgT_v02, confidenceCradleMountState) - QMI_IDL_OFFSET8(qmiLocSetCradleMountConfigReqMsgT_v02, confidenceCradleMountState_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetCradleMountConfigReqMsgT_v02, confidenceCradleMountState)
};

static const uint8_t qmiLocSetCradleMountConfigIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetCradleMountConfigIndMsgT_v02, status)
};

/*
 * qmiLocGetExternalPowerConfigReqMsgT is empty
 * static const uint8_t qmiLocGetExternalPowerConfigReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetExternalPowerConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetExternalPowerConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetExternalPowerConfigIndMsgT_v02, externalPowerState) - QMI_IDL_OFFSET8(qmiLocGetExternalPowerConfigIndMsgT_v02, externalPowerState_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetExternalPowerConfigIndMsgT_v02, externalPowerState)
};

static const uint8_t qmiLocSetExternalPowerConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetExternalPowerConfigReqMsgT_v02, externalPowerState)
};

static const uint8_t qmiLocSetExternalPowerConfigIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetExternalPowerConfigIndMsgT_v02, status)
};

static const uint8_t qmiLocInformLocationServerConnStatusReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, connHandle),

  0x02,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, requestType),

  0x03,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, statusType),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, apnProfile) - QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, apnProfile_valid)),
  0x10,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusReqMsgT_v02, apnProfile),
  28, 0
};

static const uint8_t qmiLocInformLocationServerConnStatusIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInformLocationServerConnStatusIndMsgT_v02, status)
};

static const uint8_t qmiLocSetProtocolConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplSecurity) - QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplSecurity_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplSecurity),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, vxVersion) - QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, vxVersion_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, vxVersion),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplVersion) - QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplVersion_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersReqMsgT_v02, suplVersion)
};

static const uint8_t qmiLocSetProtocolConfigParametersIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersIndMsgT_v02, failedProtocolConfigParamMask) - QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersIndMsgT_v02, failedProtocolConfigParamMask_valid)),
  0x10,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetProtocolConfigParametersIndMsgT_v02, failedProtocolConfigParamMask)
};

static const uint8_t qmiLocGetProtocolConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_8_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersReqMsgT_v02, getProtocolConfigParamMask)
};

static const uint8_t qmiLocGetProtocolConfigParametersIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplSecurity) - QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplSecurity_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplSecurity),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, vxVersion) - QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, vxVersion_valid)),
  0x11,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, vxVersion),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplVersion) - QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplVersion_valid)),
  0x12,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetProtocolConfigParametersIndMsgT_v02, suplVersion)
};

static const uint8_t qmiLocSetSensorControlConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorControlConfigReqMsgT_v02, sensorsUsage) - QMI_IDL_OFFSET8(qmiLocSetSensorControlConfigReqMsgT_v02, sensorsUsage_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorControlConfigReqMsgT_v02, sensorsUsage)
};

static const uint8_t qmiLocSetSensorControlConfigIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorControlConfigIndMsgT_v02, status)
};

/*
 * qmiLocGetSensorControlConfigReqMsgT is empty
 * static const uint8_t qmiLocGetSensorControlConfigReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetSensorControlConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorControlConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSensorControlConfigIndMsgT_v02, sensorsUsage) - QMI_IDL_OFFSET8(qmiLocGetSensorControlConfigIndMsgT_v02, sensorsUsage_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorControlConfigIndMsgT_v02, sensorsUsage)
};

static const uint8_t qmiLocSetSensorPropertiesReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorPropertiesReqMsgT_v02, gyroBiasVarianceRandomWalk) - QMI_IDL_OFFSET8(qmiLocSetSensorPropertiesReqMsgT_v02, gyroBiasVarianceRandomWalk_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPropertiesReqMsgT_v02, gyroBiasVarianceRandomWalk)
};

static const uint8_t qmiLocSetSensorPropertiesIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPropertiesIndMsgT_v02, status)
};

/*
 * qmiLocGetSensorPropertiesReqMsgT is empty
 * static const uint8_t qmiLocGetSensorPropertiesReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetSensorPropertiesIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPropertiesIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSensorPropertiesIndMsgT_v02, gyroBiasVarianceRandomWalk) - QMI_IDL_OFFSET8(qmiLocGetSensorPropertiesIndMsgT_v02, gyroBiasVarianceRandomWalk_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPropertiesIndMsgT_v02, gyroBiasVarianceRandomWalk)
};

static const uint8_t qmiLocSetSensorPerformanceControlConfigReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, performanceControlMode) - QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, performanceControlMode_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, performanceControlMode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, accelSamplingSpec) - QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, accelSamplingSpec_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, accelSamplingSpec),
  18, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, gyroSamplingSpec) - QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, gyroSamplingSpec_valid)),
  0x12,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02, gyroSamplingSpec),
  18, 0
};

static const uint8_t qmiLocSetSensorPerformanceControlConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02, failedConfiguration) - QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02, failedConfiguration_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02, failedConfiguration)
};

/*
 * qmiLocGetSensorPerformanceControlConfigReqMsgT is empty
 * static const uint8_t qmiLocGetSensorPerformanceControlConfigReqMsgT_data_v02[] = {
 * };
 */

static const uint8_t qmiLocGetSensorPerformanceControlConfigIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, performanceControlMode) - QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, performanceControlMode_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, performanceControlMode),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, accelSamplingSpec) - QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, accelSamplingSpec_valid)),
  0x11,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, accelSamplingSpec),
  18, 0,

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, gyroSamplingSpec) - QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, gyroSamplingSpec_valid)),
  0x12,
  QMI_IDL_AGGREGATE,
  QMI_IDL_OFFSET8(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02, gyroSamplingSpec),
  18, 0
};

static const uint8_t qmiLocInjectSuplCertificateReqMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSuplCertificateReqMsgT_v02, suplCertId),

  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x02,
  QMI_IDL_FLAGS_IS_ARRAY | QMI_IDL_FLAGS_IS_VARIABLE_LEN | QMI_IDL_FLAGS_SZ_IS_16 |   QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSuplCertificateReqMsgT_v02, suplCertData),
  ((QMI_LOC_MAX_SUPL_CERT_LENGTH_V02) & 0xFF), ((QMI_LOC_MAX_SUPL_CERT_LENGTH_V02) >> 8),
  QMI_IDL_OFFSET8(qmiLocInjectSuplCertificateReqMsgT_v02, suplCertData) - QMI_IDL_OFFSET8(qmiLocInjectSuplCertificateReqMsgT_v02, suplCertData_len)
};

static const uint8_t qmiLocInjectSuplCertificateIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocInjectSuplCertificateIndMsgT_v02, status)
};

static const uint8_t qmiLocDeleteSuplCertificateReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocDeleteSuplCertificateReqMsgT_v02, suplCertId) - QMI_IDL_OFFSET8(qmiLocDeleteSuplCertificateReqMsgT_v02, suplCertId_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSuplCertificateReqMsgT_v02, suplCertId)
};

static const uint8_t qmiLocDeleteSuplCertificateIndMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocDeleteSuplCertificateIndMsgT_v02, status)
};

static const uint8_t qmiLocSetPositionEngineConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, injectedPositionControl),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage_valid)),
  0x11,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, filterSvUsage),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData_valid)),
  0x12,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersReqMsgT_v02, storeAssistData)
};

static const uint8_t qmiLocSetPositionEngineConfigParametersIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask) - QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask_valid)),
  0x10,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocSetPositionEngineConfigParametersIndMsgT_v02, failedPositionEngineConfigParamMask)
};

static const uint8_t qmiLocGetPositionEngineConfigParametersReqMsgT_data_v02[] = {
  QMI_IDL_TLV_FLAGS_LAST_TLV | 0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersReqMsgT_v02, getPositionEngineConfigParamMask)
};

static const uint8_t qmiLocGetPositionEngineConfigParametersIndMsgT_data_v02[] = {
  0x01,
  QMI_IDL_GENERIC_4_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, status),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl_valid)),
  0x10,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, injectedPositionControl),

  QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage_valid)),
  0x11,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, filterSvUsage),

  QMI_IDL_TLV_FLAGS_LAST_TLV | QMI_IDL_TLV_FLAGS_OPTIONAL | (QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData) - QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData_valid)),
  0x12,
  QMI_IDL_GENERIC_1_BYTE,
  QMI_IDL_OFFSET8(qmiLocGetPositionEngineConfigParametersIndMsgT_v02, storeAssistData)
};

/* Type Table */
static const qmi_idl_type_table_entry  loc_type_table_v02[] = {
  {sizeof(qmiLocGPSTimeStructT_v02), qmiLocGPSTimeStructT_data_v02},
  {sizeof(qmiLocDOPStructT_v02), qmiLocDOPStructT_data_v02},
  {sizeof(qmiLocSensorUsageIndicatorStructT_v02), qmiLocSensorUsageIndicatorStructT_data_v02},
  {sizeof(qmiLocSvInfoStructT_v02), qmiLocSvInfoStructT_data_v02},
  {sizeof(qmiLocNiVxNotifyVerifyStructT_v02), qmiLocNiVxNotifyVerifyStructT_data_v02},
  {sizeof(qmiLocNiSuplFormattedStringStructT_v02), qmiLocNiSuplFormattedStringStructT_data_v02},
  {sizeof(qmiLocNiSuplQopStructT_v02), qmiLocNiSuplQopStructT_data_v02},
  {sizeof(qmiLocIpV4AddrStructType_v02), qmiLocIpV4AddrStructType_data_v02},
  {sizeof(qmiLocIpV6AddrStructType_v02), qmiLocIpV6AddrStructType_data_v02},
  {sizeof(qmiLocNiSuplServerInfoStructT_v02), qmiLocNiSuplServerInfoStructT_data_v02},
  {sizeof(qmiLocNiSuplNotifyVerifyStructT_v02), qmiLocNiSuplNotifyVerifyStructT_data_v02},
  {sizeof(qmiLocNiUmtsCpCodedStringStructT_v02), qmiLocNiUmtsCpCodedStringStructT_data_v02},
  {sizeof(qmiLocNiUmtsCpNotifyVerifyStructT_v02), qmiLocNiUmtsCpNotifyVerifyStructT_data_v02},
  {sizeof(qmiLocNiVxServiceInteractionStructT_v02), qmiLocNiVxServiceInteractionStructT_data_v02},
  {sizeof(qmiLocAssistanceServerUrlStructT_v02), qmiLocAssistanceServerUrlStructT_data_v02},
  {sizeof(qmiLocTimeServerListStructT_v02), qmiLocTimeServerListStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsAllowedSizesStructT_v02), qmiLocPredictedOrbitsAllowedSizesStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsServerListStructT_v02), qmiLocPredictedOrbitsServerListStructT_data_v02},
  {sizeof(qmiLocSensorControlConfigSamplingSpecStructT_v02), qmiLocSensorControlConfigSamplingSpecStructT_data_v02},
  {sizeof(qmiLocSensorReadyStatusStructT_v02), qmiLocSensorReadyStatusStructT_data_v02},
  {sizeof(qmiLocPredictedOrbitsDataValidityStructT_v02), qmiLocPredictedOrbitsDataValidityStructT_data_v02},
  {sizeof(qmiLocAltitudeSrcInfoStructT_v02), qmiLocAltitudeSrcInfoStructT_data_v02},
  {sizeof(qmiLocDeleteSvInfoStructT_v02), qmiLocDeleteSvInfoStructT_data_v02},
  {sizeof(qmiLocWifiFixTimeStructT_v02), qmiLocWifiFixTimeStructT_data_v02},
  {sizeof(qmiLocWifiFixPosStructT_v02), qmiLocWifiFixPosStructT_data_v02},
  {sizeof(qmiLocWifiApInfoStructT_v02), qmiLocWifiApInfoStructT_data_v02},
  {sizeof(qmiLoc3AxisSensorSampleStructT_v02), qmiLoc3AxisSensorSampleStructT_data_v02},
  {sizeof(qmiLoc3AxisSensorSampleListStructT_v02), qmiLoc3AxisSensorSampleListStructT_data_v02},
  {sizeof(qmiLocApnProfilesStructT_v02), qmiLocApnProfilesStructT_data_v02}
};

/* Message Table */
static const qmi_idl_message_table_entry loc_message_table_v02[] = {
  {sizeof(qmiLocGenRespMsgT_v02), qmiLocGenRespMsgT_data_v02},
  {sizeof(qmiLocInformClientRevisionReqMsgT_v02), qmiLocInformClientRevisionReqMsgT_data_v02},
  {sizeof(qmiLocRegEventsReqMsgT_v02), qmiLocRegEventsReqMsgT_data_v02},
  {sizeof(qmiLocStartReqMsgT_v02), qmiLocStartReqMsgT_data_v02},
  {sizeof(qmiLocStopReqMsgT_v02), qmiLocStopReqMsgT_data_v02},
  {sizeof(qmiLocEventPositionReportIndMsgT_v02), qmiLocEventPositionReportIndMsgT_data_v02},
  {sizeof(qmiLocEventGnssSvInfoIndMsgT_v02), qmiLocEventGnssSvInfoIndMsgT_data_v02},
  {sizeof(qmiLocEventNmeaIndMsgT_v02), qmiLocEventNmeaIndMsgT_data_v02},
  {sizeof(qmiLocEventNiNotifyVerifyReqIndMsgT_v02), qmiLocEventNiNotifyVerifyReqIndMsgT_data_v02},
  {sizeof(qmiLocEventInjectTimeReqIndMsgT_v02), qmiLocEventInjectTimeReqIndMsgT_data_v02},
  {sizeof(qmiLocEventInjectPredictedOrbitsReqIndMsgT_v02), qmiLocEventInjectPredictedOrbitsReqIndMsgT_data_v02},
  {sizeof(qmiLocEventInjectPositionReqIndMsgT_v02), qmiLocEventInjectPositionReqIndMsgT_data_v02},
  {sizeof(qmiLocEventEngineStateIndMsgT_v02), qmiLocEventEngineStateIndMsgT_data_v02},
  {sizeof(qmiLocEventFixSessionStateIndMsgT_v02), qmiLocEventFixSessionStateIndMsgT_data_v02},
  {sizeof(qmiLocEventWifiReqIndMsgT_v02), qmiLocEventWifiReqIndMsgT_data_v02},
  {sizeof(qmiLocEventSensorStreamingReadyStatusIndMsgT_v02), qmiLocEventSensorStreamingReadyStatusIndMsgT_data_v02},
  {sizeof(qmiLocEventTimeSyncReqIndMsgT_v02), qmiLocEventTimeSyncReqIndMsgT_data_v02},
  {sizeof(qmiLocEventSetSpiStreamingReportIndMsgT_v02), qmiLocEventSetSpiStreamingReportIndMsgT_data_v02},
  {sizeof(qmiLocEventLocationServerConnectionReqIndMsgT_v02), qmiLocEventLocationServerConnectionReqIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetServiceRevisionIndMsgT_v02), qmiLocGetServiceRevisionIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetFixCriteriaIndMsgT_v02), qmiLocGetFixCriteriaIndMsgT_data_v02},
  {sizeof(qmiLocNiUserRespReqMsgT_v02), qmiLocNiUserRespReqMsgT_data_v02},
  {sizeof(qmiLocNiUserRespIndMsgT_v02), qmiLocNiUserRespIndMsgT_data_v02},
  {sizeof(qmiLocInjectPredictedOrbitsDataReqMsgT_v02), qmiLocInjectPredictedOrbitsDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectPredictedOrbitsDataIndMsgT_v02), qmiLocInjectPredictedOrbitsDataIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetPredictedOrbitsDataSourceIndMsgT_v02), qmiLocGetPredictedOrbitsDataSourceIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetPredictedOrbitsDataValidityIndMsgT_v02), qmiLocGetPredictedOrbitsDataValidityIndMsgT_data_v02},
  {sizeof(qmiLocInjectUtcTimeReqMsgT_v02), qmiLocInjectUtcTimeReqMsgT_data_v02},
  {sizeof(qmiLocInjectUtcTimeIndMsgT_v02), qmiLocInjectUtcTimeIndMsgT_data_v02},
  {sizeof(qmiLocInjectPositionReqMsgT_v02), qmiLocInjectPositionReqMsgT_data_v02},
  {sizeof(qmiLocInjectPositionIndMsgT_v02), qmiLocInjectPositionIndMsgT_data_v02},
  {sizeof(qmiLocSetEngineLockReqMsgT_v02), qmiLocSetEngineLockReqMsgT_data_v02},
  {sizeof(qmiLocSetEngineLockIndMsgT_v02), qmiLocSetEngineLockIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetEngineLockIndMsgT_v02), qmiLocGetEngineLockIndMsgT_data_v02},
  {sizeof(qmiLocSetSbasConfigReqMsgT_v02), qmiLocSetSbasConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetSbasConfigIndMsgT_v02), qmiLocSetSbasConfigIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetSbasConfigIndMsgT_v02), qmiLocGetSbasConfigIndMsgT_data_v02},
  {sizeof(qmiLocSetNmeaTypesReqMsgT_v02), qmiLocSetNmeaTypesReqMsgT_data_v02},
  {sizeof(qmiLocSetNmeaTypesIndMsgT_v02), qmiLocSetNmeaTypesIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetNmeaTypesIndMsgT_v02), qmiLocGetNmeaTypesIndMsgT_data_v02},
  {sizeof(qmiLocSetLowPowerModeReqMsgT_v02), qmiLocSetLowPowerModeReqMsgT_data_v02},
  {sizeof(qmiLocSetLowPowerModeIndMsgT_v02), qmiLocSetLowPowerModeIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetLowPowerModeIndMsgT_v02), qmiLocGetLowPowerModeIndMsgT_data_v02},
  {sizeof(qmiLocSetServerReqMsgT_v02), qmiLocSetServerReqMsgT_data_v02},
  {sizeof(qmiLocSetServerIndMsgT_v02), qmiLocSetServerIndMsgT_data_v02},
  {sizeof(qmiLocGetServerReqMsgT_v02), qmiLocGetServerReqMsgT_data_v02},
  {sizeof(qmiLocGetServerIndMsgT_v02), qmiLocGetServerIndMsgT_data_v02},
  {sizeof(qmiLocDeleteAssistDataReqMsgT_v02), qmiLocDeleteAssistDataReqMsgT_data_v02},
  {sizeof(qmiLocDeleteAssistDataIndMsgT_v02), qmiLocDeleteAssistDataIndMsgT_data_v02},
  {sizeof(qmiLocSetXtraTSessionControlReqMsgT_v02), qmiLocSetXtraTSessionControlReqMsgT_data_v02},
  {sizeof(qmiLocSetXtraTSessionControlIndMsgT_v02), qmiLocSetXtraTSessionControlIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetXtraTSessionControlIndMsgT_v02), qmiLocGetXtraTSessionControlIndMsgT_data_v02},
  {sizeof(qmiLocInjectWifiPositionReqMsgT_v02), qmiLocInjectWifiPositionReqMsgT_data_v02},
  {sizeof(qmiLocInjectWifiPositionIndMsgT_v02), qmiLocInjectWifiPositionIndMsgT_data_v02},
  {sizeof(qmiLocNotifyWifiStatusReqMsgT_v02), qmiLocNotifyWifiStatusReqMsgT_data_v02},
  {sizeof(qmiLocNotifyWifiStatusIndMsgT_v02), qmiLocNotifyWifiStatusIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetRegisteredEventsIndMsgT_v02), qmiLocGetRegisteredEventsIndMsgT_data_v02},
  {sizeof(qmiLocSetOperationModeReqMsgT_v02), qmiLocSetOperationModeReqMsgT_data_v02},
  {sizeof(qmiLocSetOperationModeIndMsgT_v02), qmiLocSetOperationModeIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetOperationModeIndMsgT_v02), qmiLocGetOperationModeIndMsgT_data_v02},
  {sizeof(qmiLocSetSpiStatusReqMsgT_v02), qmiLocSetSpiStatusReqMsgT_data_v02},
  {sizeof(qmiLocSetSpiStatusIndMsgT_v02), qmiLocSetSpiStatusIndMsgT_data_v02},
  {sizeof(qmiLocInjectSensorDataReqMsgT_v02), qmiLocInjectSensorDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectSensorDataIndMsgT_v02), qmiLocInjectSensorDataIndMsgT_data_v02},
  {sizeof(qmiLocInjectTimeSyncDataReqMsgT_v02), qmiLocInjectTimeSyncDataReqMsgT_data_v02},
  {sizeof(qmiLocInjectTimeSyncDataIndMsgT_v02), qmiLocInjectTimeSyncDataIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetCradleMountConfigIndMsgT_v02), qmiLocGetCradleMountConfigIndMsgT_data_v02},
  {sizeof(qmiLocSetCradleMountConfigReqMsgT_v02), qmiLocSetCradleMountConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetCradleMountConfigIndMsgT_v02), qmiLocSetCradleMountConfigIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetExternalPowerConfigIndMsgT_v02), qmiLocGetExternalPowerConfigIndMsgT_data_v02},
  {sizeof(qmiLocSetExternalPowerConfigReqMsgT_v02), qmiLocSetExternalPowerConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetExternalPowerConfigIndMsgT_v02), qmiLocSetExternalPowerConfigIndMsgT_data_v02},
  {sizeof(qmiLocInformLocationServerConnStatusReqMsgT_v02), qmiLocInformLocationServerConnStatusReqMsgT_data_v02},
  {sizeof(qmiLocInformLocationServerConnStatusIndMsgT_v02), qmiLocInformLocationServerConnStatusIndMsgT_data_v02},
  {sizeof(qmiLocSetProtocolConfigParametersReqMsgT_v02), qmiLocSetProtocolConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocSetProtocolConfigParametersIndMsgT_v02), qmiLocSetProtocolConfigParametersIndMsgT_data_v02},
  {sizeof(qmiLocGetProtocolConfigParametersReqMsgT_v02), qmiLocGetProtocolConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocGetProtocolConfigParametersIndMsgT_v02), qmiLocGetProtocolConfigParametersIndMsgT_data_v02},
  {sizeof(qmiLocSetSensorControlConfigReqMsgT_v02), qmiLocSetSensorControlConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetSensorControlConfigIndMsgT_v02), qmiLocSetSensorControlConfigIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetSensorControlConfigIndMsgT_v02), qmiLocGetSensorControlConfigIndMsgT_data_v02},
  {sizeof(qmiLocSetSensorPropertiesReqMsgT_v02), qmiLocSetSensorPropertiesReqMsgT_data_v02},
  {sizeof(qmiLocSetSensorPropertiesIndMsgT_v02), qmiLocSetSensorPropertiesIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetSensorPropertiesIndMsgT_v02), qmiLocGetSensorPropertiesIndMsgT_data_v02},
  {sizeof(qmiLocSetSensorPerformanceControlConfigReqMsgT_v02), qmiLocSetSensorPerformanceControlConfigReqMsgT_data_v02},
  {sizeof(qmiLocSetSensorPerformanceControlConfigIndMsgT_v02), qmiLocSetSensorPerformanceControlConfigIndMsgT_data_v02},
  {0, 0},
  {sizeof(qmiLocGetSensorPerformanceControlConfigIndMsgT_v02), qmiLocGetSensorPerformanceControlConfigIndMsgT_data_v02},
  {sizeof(qmiLocInjectSuplCertificateReqMsgT_v02), qmiLocInjectSuplCertificateReqMsgT_data_v02},
  {sizeof(qmiLocInjectSuplCertificateIndMsgT_v02), qmiLocInjectSuplCertificateIndMsgT_data_v02},
  {sizeof(qmiLocDeleteSuplCertificateReqMsgT_v02), qmiLocDeleteSuplCertificateReqMsgT_data_v02},
  {sizeof(qmiLocDeleteSuplCertificateIndMsgT_v02), qmiLocDeleteSuplCertificateIndMsgT_data_v02},
  {sizeof(qmiLocSetPositionEngineConfigParametersReqMsgT_v02), qmiLocSetPositionEngineConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocSetPositionEngineConfigParametersIndMsgT_v02), qmiLocSetPositionEngineConfigParametersIndMsgT_data_v02},
  {sizeof(qmiLocGetPositionEngineConfigParametersReqMsgT_v02), qmiLocGetPositionEngineConfigParametersReqMsgT_data_v02},
  {sizeof(qmiLocGetPositionEngineConfigParametersIndMsgT_v02), qmiLocGetPositionEngineConfigParametersIndMsgT_data_v02}
};

/* Predefine the Type Table Object */
static const qmi_idl_type_table_object loc_qmi_idl_type_table_object_v02;

/*Referenced Tables Array*/
static const qmi_idl_type_table_object *loc_qmi_idl_type_table_object_referenced_tables_v02[] =
{&loc_qmi_idl_type_table_object_v02, &common_qmi_idl_type_table_object_v01};

/*Type Table Object*/
static const qmi_idl_type_table_object loc_qmi_idl_type_table_object_v02 = {
  sizeof(loc_type_table_v02)/sizeof(qmi_idl_type_table_entry ),
  sizeof(loc_message_table_v02)/sizeof(qmi_idl_message_table_entry),
  1,
  loc_type_table_v02,
  loc_message_table_v02,
  loc_qmi_idl_type_table_object_referenced_tables_v02
};

/*Arrays of service_message_table_entries for commands, responses and indications*/
static const qmi_idl_service_message_table_entry loc_service_command_messages_v02[] = {
  {QMI_LOC_INFORM_CLIENT_REVISION_REQ_V02, TYPE16(0, 1), 7},
  {QMI_LOC_REG_EVENTS_REQ_V02, TYPE16(0, 2), 11},
  {QMI_LOC_START_REQ_V02, TYPE16(0, 3), 32},
  {QMI_LOC_STOP_REQ_V02, TYPE16(0, 4), 4},
  {QMI_LOC_GET_SERVICE_REVISION_REQ_V02, TYPE16(0, 19), 0},
  {QMI_LOC_GET_FIX_CRITERIA_REQ_V02, TYPE16(0, 21), 0},
  {QMI_LOC_NI_USER_RESPONSE_REQ_V02, TYPE16(0, 23), 1075},
  {QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_REQ_V02, TYPE16(0, 25), 1053},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_REQ_V02, TYPE16(0, 27), 0},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_REQ_V02, TYPE16(0, 29), 0},
  {QMI_LOC_INJECT_UTC_TIME_REQ_V02, TYPE16(0, 31), 18},
  {QMI_LOC_INJECT_POSITION_REQ_V02, TYPE16(0, 33), 112},
  {QMI_LOC_SET_ENGINE_LOCK_REQ_V02, TYPE16(0, 35), 7},
  {QMI_LOC_GET_ENGINE_LOCK_REQ_V02, TYPE16(0, 37), 0},
  {QMI_LOC_SET_SBAS_CONFIG_REQ_V02, TYPE16(0, 39), 4},
  {QMI_LOC_GET_SBAS_CONFIG_REQ_V02, TYPE16(0, 41), 0},
  {QMI_LOC_SET_NMEA_TYPES_REQ_V02, TYPE16(0, 43), 7},
  {QMI_LOC_GET_NMEA_TYPES_REQ_V02, TYPE16(0, 45), 0},
  {QMI_LOC_SET_LOW_POWER_MODE_REQ_V02, TYPE16(0, 47), 4},
  {QMI_LOC_GET_LOW_POWER_MODE_REQ_V02, TYPE16(0, 49), 0},
  {QMI_LOC_SET_SERVER_REQ_V02, TYPE16(0, 51), 297},
  {QMI_LOC_GET_SERVER_REQ_V02, TYPE16(0, 53), 11},
  {QMI_LOC_DELETE_ASSIST_DATA_REQ_V02, TYPE16(0, 55), 929},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_REQ_V02, TYPE16(0, 57), 4},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_REQ_V02, TYPE16(0, 59), 0},
  {QMI_LOC_INJECT_WIFI_POSITION_REQ_V02, TYPE16(0, 61), 694},
  {QMI_LOC_NOTIFY_WIFI_STATUS_REQ_V02, TYPE16(0, 63), 7},
  {QMI_LOC_GET_REGISTERED_EVENTS_REQ_V02, TYPE16(0, 65), 0},
  {QMI_LOC_SET_OPERATION_MODE_REQ_V02, TYPE16(0, 67), 7},
  {QMI_LOC_GET_OPERATION_MODE_REQ_V02, TYPE16(0, 69), 0},
  {QMI_LOC_SET_SPI_STATUS_REQ_V02, TYPE16(0, 71), 8},
  {QMI_LOC_INJECT_SENSOR_DATA_REQ_V02, TYPE16(0, 73), 1425},
  {QMI_LOC_INJECT_TIME_SYNC_DATA_REQ_V02, TYPE16(0, 75), 21},
  {QMI_LOC_SET_CRADLE_MOUNT_CONFIG_REQ_V02, TYPE16(0, 79), 11},
  {QMI_LOC_GET_CRADLE_MOUNT_CONFIG_REQ_V02, TYPE16(0, 77), 0},
  {QMI_LOC_SET_EXTERNAL_POWER_CONFIG_REQ_V02, TYPE16(0, 83), 7},
  {QMI_LOC_GET_EXTERNAL_POWER_CONFIG_REQ_V02, TYPE16(0, 81), 0},
  {QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_REQ_V02, TYPE16(0, 85), 129},
  {QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02, TYPE16(0, 87), 18},
  {QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_REQ_V02, TYPE16(0, 89), 11},
  {QMI_LOC_SET_SENSOR_CONTROL_CONFIG_REQ_V02, TYPE16(0, 91), 7},
  {QMI_LOC_GET_SENSOR_CONTROL_CONFIG_REQ_V02, TYPE16(0, 93), 0},
  {QMI_LOC_SET_SENSOR_PROPERTIES_REQ_V02, TYPE16(0, 95), 7},
  {QMI_LOC_GET_SENSOR_PROPERTIES_REQ_V02, TYPE16(0, 97), 0},
  {QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02, TYPE16(0, 99), 21},
  {QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_REQ_V02, TYPE16(0, 101), 0},
  {QMI_LOC_INJECT_SUPL_CERTIFICATE_REQ_V02, TYPE16(0, 103), 2009},
  {QMI_LOC_DELETE_SUPL_CERTIFICATE_REQ_V02, TYPE16(0, 105), 4},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02, TYPE16(0, 107), 12},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_REQ_V02, TYPE16(0, 109), 7}
};

static const qmi_idl_service_message_table_entry loc_service_response_messages_v02[] = {
  {QMI_LOC_INFORM_CLIENT_REVISION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_REG_EVENTS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_START_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_STOP_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SERVICE_REVISION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_FIX_CRITERIA_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_NI_USER_RESPONSE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_UTC_TIME_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_POSITION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_ENGINE_LOCK_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_ENGINE_LOCK_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SBAS_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SBAS_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_NMEA_TYPES_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_NMEA_TYPES_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_LOW_POWER_MODE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_LOW_POWER_MODE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SERVER_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SERVER_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_DELETE_ASSIST_DATA_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_WIFI_POSITION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_NOTIFY_WIFI_STATUS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_REGISTERED_EVENTS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_OPERATION_MODE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_OPERATION_MODE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SPI_STATUS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_SENSOR_DATA_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_TIME_SYNC_DATA_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_CRADLE_MOUNT_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_CRADLE_MOUNT_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_EXTERNAL_POWER_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_EXTERNAL_POWER_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SENSOR_CONTROL_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SENSOR_CONTROL_CONFIG_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SENSOR_PROPERTIES_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SENSOR_PROPERTIES_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_INJECT_SUPL_CERTIFICATE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_DELETE_SUPL_CERTIFICATE_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02, TYPE16(0, 0), 7},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_RESP_V02, TYPE16(0, 0), 7}
};

static const qmi_idl_service_message_table_entry loc_service_indication_messages_v02[] = {
  {QMI_LOC_EVENT_POSITION_REPORT_IND_V02, TYPE16(0, 5), 224},
  {QMI_LOC_EVENT_GNSS_SV_INFO_IND_V02, TYPE16(0, 6), 2248},
  {QMI_LOC_EVENT_NMEA_IND_V02, TYPE16(0, 7), 203},
  {QMI_LOC_EVENT_NI_NOTIFY_VERIFY_REQ_IND_V02, TYPE16(0, 8), 1068},
  {QMI_LOC_EVENT_INJECT_TIME_REQ_IND_V02, TYPE16(0, 9), 776},
  {QMI_LOC_EVENT_INJECT_PREDICTED_ORBITS_REQ_IND_V02, TYPE16(0, 10), 783},
  {QMI_LOC_EVENT_INJECT_POSITION_REQ_IND_V02, TYPE16(0, 11), 40},
  {QMI_LOC_EVENT_ENGINE_STATE_IND_V02, TYPE16(0, 12), 7},
  {QMI_LOC_EVENT_FIX_SESSION_STATE_IND_V02, TYPE16(0, 13), 11},
  {QMI_LOC_EVENT_WIFI_REQ_IND_V02, TYPE16(0, 14), 12},
  {QMI_LOC_EVENT_SENSOR_STREAMING_READY_STATUS_IND_V02, TYPE16(0, 15), 16},
  {QMI_LOC_EVENT_TIME_SYNC_REQ_IND_V02, TYPE16(0, 16), 7},
  {QMI_LOC_EVENT_SET_SPI_STREAMING_REPORT_IND_V02, TYPE16(0, 17), 4},
  {QMI_LOC_EVENT_LOCATION_SERVER_CONNECTION_REQ_IND_V02, TYPE16(0, 18), 21},
  {QMI_LOC_GET_SERVICE_REVISION_IND_V02, TYPE16(0, 20), 14},
  {QMI_LOC_GET_FIX_CRITERIA_IND_V02, TYPE16(0, 22), 28},
  {QMI_LOC_NI_USER_RESPONSE_IND_V02, TYPE16(0, 24), 7},
  {QMI_LOC_INJECT_PREDICTED_ORBITS_DATA_IND_V02, TYPE16(0, 26), 12},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_SOURCE_IND_V02, TYPE16(0, 28), 790},
  {QMI_LOC_GET_PREDICTED_ORBITS_DATA_VALIDITY_IND_V02, TYPE16(0, 30), 20},
  {QMI_LOC_INJECT_UTC_TIME_IND_V02, TYPE16(0, 32), 7},
  {QMI_LOC_INJECT_POSITION_IND_V02, TYPE16(0, 34), 7},
  {QMI_LOC_SET_ENGINE_LOCK_IND_V02, TYPE16(0, 36), 7},
  {QMI_LOC_GET_ENGINE_LOCK_IND_V02, TYPE16(0, 38), 14},
  {QMI_LOC_SET_SBAS_CONFIG_IND_V02, TYPE16(0, 40), 7},
  {QMI_LOC_GET_SBAS_CONFIG_IND_V02, TYPE16(0, 42), 11},
  {QMI_LOC_SET_NMEA_TYPES_IND_V02, TYPE16(0, 44), 7},
  {QMI_LOC_GET_NMEA_TYPES_IND_V02, TYPE16(0, 46), 14},
  {QMI_LOC_SET_LOW_POWER_MODE_IND_V02, TYPE16(0, 48), 7},
  {QMI_LOC_GET_LOW_POWER_MODE_IND_V02, TYPE16(0, 50), 11},
  {QMI_LOC_SET_SERVER_IND_V02, TYPE16(0, 52), 7},
  {QMI_LOC_GET_SERVER_IND_V02, TYPE16(0, 54), 304},
  {QMI_LOC_DELETE_ASSIST_DATA_IND_V02, TYPE16(0, 56), 7},
  {QMI_LOC_SET_XTRA_T_SESSION_CONTROL_IND_V02, TYPE16(0, 58), 7},
  {QMI_LOC_GET_XTRA_T_SESSION_CONTROL_IND_V02, TYPE16(0, 60), 11},
  {QMI_LOC_INJECT_WIFI_POSITION_IND_V02, TYPE16(0, 62), 7},
  {QMI_LOC_NOTIFY_WIFI_STATUS_IND_V02, TYPE16(0, 64), 7},
  {QMI_LOC_GET_REGISTERED_EVENTS_IND_V02, TYPE16(0, 66), 18},
  {QMI_LOC_SET_OPERATION_MODE_IND_V02, TYPE16(0, 68), 7},
  {QMI_LOC_GET_OPERATION_MODE_IND_V02, TYPE16(0, 70), 14},
  {QMI_LOC_SET_SPI_STATUS_IND_V02, TYPE16(0, 72), 7},
  {QMI_LOC_INJECT_SENSOR_DATA_IND_V02, TYPE16(0, 74), 22},
  {QMI_LOC_INJECT_TIME_SYNC_DATA_IND_V02, TYPE16(0, 76), 7},
  {QMI_LOC_SET_CRADLE_MOUNT_CONFIG_IND_V02, TYPE16(0, 80), 7},
  {QMI_LOC_GET_CRADLE_MOUNT_CONFIG_IND_V02, TYPE16(0, 78), 18},
  {QMI_LOC_SET_EXTERNAL_POWER_CONFIG_IND_V02, TYPE16(0, 84), 7},
  {QMI_LOC_GET_EXTERNAL_POWER_CONFIG_IND_V02, TYPE16(0, 82), 14},
  {QMI_LOC_INFORM_LOCATION_SERVER_CONN_STATUS_IND_V02, TYPE16(0, 86), 7},
  {QMI_LOC_SET_PROTOCOL_CONFIG_PARAMETERS_IND_V02, TYPE16(0, 88), 18},
  {QMI_LOC_GET_PROTOCOL_CONFIG_PARAMETERS_IND_V02, TYPE16(0, 90), 25},
  {QMI_LOC_SET_SENSOR_CONTROL_CONFIG_IND_V02, TYPE16(0, 92), 7},
  {QMI_LOC_GET_SENSOR_CONTROL_CONFIG_IND_V02, TYPE16(0, 94), 14},
  {QMI_LOC_SET_SENSOR_PROPERTIES_IND_V02, TYPE16(0, 96), 7},
  {QMI_LOC_GET_SENSOR_PROPERTIES_IND_V02, TYPE16(0, 98), 14},
  {QMI_LOC_SET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02, TYPE16(0, 100), 14},
  {QMI_LOC_GET_SENSOR_PERFORMANCE_CONTROL_CONFIGURATION_IND_V02, TYPE16(0, 102), 28},
  {QMI_LOC_INJECT_SUPL_CERTIFICATE_IND_V02, TYPE16(0, 104), 7},
  {QMI_LOC_DELETE_SUPL_CERTIFICATE_IND_V02, TYPE16(0, 106), 7},
  {QMI_LOC_SET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02, TYPE16(0, 108), 14},
  {QMI_LOC_GET_POSITION_ENGINE_CONFIG_PARAMETERS_IND_V02, TYPE16(0, 110), 19}
};

/*Service Object*/
const struct qmi_idl_service_object loc_qmi_idl_service_object_v02 = {
  0x02,
  0x02,
  16,
  2248,
  { sizeof(loc_service_command_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(loc_service_response_messages_v02)/sizeof(qmi_idl_service_message_table_entry),
    sizeof(loc_service_indication_messages_v02)/sizeof(qmi_idl_service_message_table_entry) },
  { loc_service_command_messages_v02, loc_service_response_messages_v02, loc_service_indication_messages_v02},
  &loc_qmi_idl_type_table_object_v02
};

/* Service Object Accessor */
qmi_idl_service_object_type loc_get_service_object_internal_v02
 ( int32_t idl_maj_version, int32_t idl_min_version, int32_t library_version ){
  if ( LOC_V02_IDL_MAJOR_VERS != idl_maj_version || LOC_V02_IDL_MINOR_VERS != idl_min_version
       || LOC_V02_IDL_TOOL_VERS != library_version)
  {
    return NULL;
  }
  return (qmi_idl_service_object_type)&loc_qmi_idl_service_object_v02;
}

