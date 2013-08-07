/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
 *
 * Not a Contribution.
 *
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef GPS_EXTENDED_H
#define GPS_EXTENDED_H

/** AGPS type */
typedef int16_t AGpsExtType;
#define AGPS_TYPE_INVALID       -1
#define AGPS_TYPE_ANY           0
#define AGPS_TYPE_SUPL          1
#define AGPS_TYPE_C2K           2
#define AGPS_TYPE_WWAN_ANY      3
#define AGPS_TYPE_WIFI          4

/** SSID length */
#define SSID_BUF_SIZE (32+1)

typedef int16_t AGpsBearerType;
#define AGPS_APN_BEARER_INVALID    -1
#define AGPS_APN_BEARER_IPV4        0
#define AGPS_APN_BEARER_IPV6        1
#define AGPS_APN_BEARER_IPV4V6      2

#define GPS_DELETE_ALMANAC_CORR     0x00001000
#define GPS_DELETE_FREQ_BIAS_EST    0x00002000
#define GPS_DELETE_EPHEMERIS_GLO    0x00004000
#define GPS_DELETE_ALMANAC_GLO      0x00008000
#define GPS_DELETE_SVDIR_GLO        0x00010000
#define GPS_DELETE_SVSTEER_GLO      0x00020000
#define GPS_DELETE_ALMANAC_CORR_GLO 0x00040000
#define GPS_DELETE_TIME_GPS         0x00080000
#define GPS_DELETE_TIME_GLO         0x00100000

/** GPS extended callback structure. */
typedef struct {
    /** set to sizeof(GpsCallbacks) */
    size_t      size;
    gps_set_capabilities set_capabilities_cb;
    gps_acquire_wakelock acquire_wakelock_cb;
    gps_release_wakelock release_wakelock_cb;
    gps_create_thread create_thread_cb;
    gps_request_utc_time request_utc_time_cb;
} GpsExtCallbacks;

/** Callback to report the xtra server url to the client.
 *  The client should use this url when downloading xtra unless overwritten
 *  in the gps.conf file
 */
typedef void (* report_xtra_server)(const char*, const char*, const char*);

/** Callback structure for the XTRA interface. */
typedef struct {
    gps_xtra_download_request download_request_cb;
    gps_create_thread create_thread_cb;
    report_xtra_server report_xtra_server_cb;
} GpsXtraExtCallbacks;

/** Represents the status of AGPS. */
typedef struct {
    /** set to sizeof(AGpsExtStatus) */
    size_t          size;

    AGpsExtType type;
    AGpsStatusValue status;
    uint32_t        ipv4_addr;
    char            ipv6_addr[16];
    char            ssid[SSID_BUF_SIZE];
    char            password[SSID_BUF_SIZE];
} AGpsExtStatus;

/** Callback with AGPS status information.
 *  Can only be called from a thread created by create_thread_cb.
 */
typedef void (* agps_status_extended)(AGpsExtStatus* status);

/** Callback structure for the AGPS interface. */
typedef struct {
    agps_status_extended status_cb;
    gps_create_thread create_thread_cb;
} AGpsExtCallbacks;


/** GPS NI callback structure. */
typedef struct
{
    /**
     * Sends the notification request from HAL to GPSLocationProvider.
     */
    gps_ni_notify_callback notify_cb;
    gps_create_thread create_thread_cb;
} GpsNiExtCallbacks;

#endif /* GPS_EXTENDED_H */

