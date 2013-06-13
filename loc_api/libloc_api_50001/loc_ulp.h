/* Copyright (c) 2013, The Linux Foundation. All rights reserved.
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
 *
 */

#ifndef __LOC_ULP_H__
#define __LOC_ULP_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <ctype.h>
#include <stdbool.h>
#include <hardware/gps.h>

#define ULP_ENGINE_INTERFACE      "ulp-engine-interface"
#define ULP_NETWORK_INTERFACE "ulp-network-interface"
#define ULP_RAW_CMD_INTERFACE      "ulp-raw-cmd"
#define ULP_PHONE_CONTEXT_INTERFACE "ulp-phone-context"

/** Location has valid source information. */
#define LOCATION_HAS_SOURCE_INFO   0x0020
/** GpsLocation has valid "is indoor?" flag */
#define GPS_LOCATION_HAS_IS_INDOOR   0x0040
/** GpsLocation has valid floor number */
#define GPS_LOCATION_HAS_FLOOR_NUMBER   0x0080
/** GpsLocation has valid map URL*/
#define GPS_LOCATION_HAS_MAP_URL   0x0100
/** GpsLocation has valid map index */
#define GPS_LOCATION_HAS_MAP_INDEX   0x0200

/** Sizes for indoor fields */
#define GPS_LOCATION_MAP_URL_SIZE 400
#define GPS_LOCATION_MAP_INDEX_SIZE 16

/** Position source is ULP */
#define ULP_LOCATION_IS_FROM_HYBRID   0x0001
/** Position source is GNSS only */
#define ULP_LOCATION_IS_FROM_GNSS   0x0002

#define ULP_MIN_INTERVAL_INVALID 0xffffffff

/** Represents recurrence of location */
typedef enum{
    ULP_LOC_RECURRENCE_PERIODIC = 0,
    ULP_LOC_RECURRENCE_SINGLE,
}UlpRecurrenceCriteria;

/** Represents horizontal accuracy options */
typedef enum {
    ULP_HORZ_ACCURACY_DONT_CARE = 0,
    ULP_HORZ_ACCURACY_LOW,
    ULP_HORZ_ACCURACY_MED,
    ULP_HORZ_ACCURACY_HIGH,
}UlpHorzAccuracyCriteria;

/** Represents accuracy options (for speed, altitude, and
 *  bearing) */
typedef enum {
    ULP_ACCURACY_DONT_CARE = 0,
    ULP_ACCURACY_LOW,
    ULP_ACCURACY_HIGH,
}UlpAccuracyCriteria;

/** Represents power consumption options */
typedef enum {
    ULP_POWER_REQ_DONT_CARE = 0,
    ULP_POWER_REQ_LOW,
    ULP_POWER_REQ_HIGH,
}UlpPowerCriteria;

/** Represents data usage options */
typedef enum {
    ULP_DATA_REQ_DONT_CARE = 0,
    ULP_DATA_ALLOW,
    ULP_DATA_DENY,
}UlpDataUsageCriteria;

/** Enable the reporting of altitude in location reports */
#define ULP_ENABLE_ALTITUDE_REPORT   0x01
/** Enable the reporting of speed in location reports */
#define ULP_ENABLE_SPEED_REPORT      0x02
/** Enable the reporting of bearing in location reports */
#define ULP_ENABLE_BEARING_REPORT    0x04

#define ULP_CRITERIA_HAS_ACTION                        0x00000001
#define ULP_CRITERIA_HAS_PROVIDER_SOURCE               0x00000002
#define ULP_CRITERIA_HAS_RECURRENCE_TYPE               0x00000004
#define ULP_CRITERIA_HAS_PREFERRED_RESPONSE_TIME       0x00000010
#define ULP_CRITERIA_HAS_MIN_INTERVAL                  0x00000020
#define ULP_CRITERIA_HAS_MIN_DISTANCE                  0x00000040
#define ULP_CRITERIA_HAS_MIN_DIST_SAMPLE_INTERVAL      0x00000080
#define ULP_CRITERIA_HAS_DESIRED_OUTPUT_PARAMETER      0x00000100
#define ULP_CRITERIA_HAS_PREFERRED_HORIZONTAL_ACCURACY 0x00000200
#define ULP_CRITERIA_HAS_PREFERRED_POWER_CONSUMPTION   0x00000400
#define ULP_CRITERIA_HAS_PREFERRED_ALTITUDE_ACCURACY   0x00000800
#define ULP_CRITERIA_HAS_PREFERRED_BEARING_ACCURACY    0x00001000
#define ULP_CRITERIA_HAS_PREFERRED_DATA_USAGE          0x00002000
#define ULP_CRITERIA_HAS_INTERMEDIATE_POS_REPORT_ENABLED    0x00004000

#define ULP_PROVIDER_SOURCE_GNSS                       0x00000001
#define ULP_PROVIDER_SOURCE_HYBRID                     0x00000002

#define ULP_ADD_CRITERIA     1
#define ULP_REMOVE_CRITERIA  2

typedef struct {
    /** set to sizeof(UlpLocation) */
    size_t          size;
    GpsLocation     gpsLocation;
    /* Provider indicator for HYBRID or GPS */
    uint16_t        position_source;
    /*allows HAL to pass additional information related to the location */
    int             rawDataSize;         /* in # of bytes */
    void            * rawData;
    bool            is_indoor;
    float           floor_number;
    char            map_url[GPS_LOCATION_MAP_URL_SIZE];
    unsigned char   map_index[GPS_LOCATION_MAP_INDEX_SIZE];
} UlpLocation;

/** Callback with location information.
 */
typedef void (* ulp_location_callback)(UlpLocation* location);

/** ULP Engine callback structure. */
typedef struct {
    /** set to sizeof(UlpCallbacks) */
    size_t      size;
    ulp_location_callback location_cb;
} UlpEngineCallbacks;

typedef struct {

    uint32_t valid_mask;
    /* delete or add. This is a mandatory field */
    int action;
    /*via gps or hybrid provider*/
    int provider_source;
    /** PERIODIC or SINGLE */
    UlpRecurrenceCriteria recurrence_type;
    /** obtain position within the specified response time */
    uint32_t preferred_response_time;
    /** Send updates after the specified interval */
    uint32_t min_interval;
    /** Send updates after device moved a specified distance */
    float min_distance;
    uint32_t min_dist_sample_interval;
    /** Fields specfied in the mask should be reported in the
     *  position report (altitude, bearing and speed) */
    uint32_t desired_output_parameter;
    /** Desired accuracy for latitude, longitude */
    UlpHorzAccuracyCriteria preferred_horizontal_accuracy;
    /** Desired power consumption level */
    UlpPowerCriteria preferred_power_consumption;
    /** Desired accuracy for altitude */
    UlpAccuracyCriteria preferred_altitude_accuracy;
    /** Desired accuracy for bearing */
    UlpAccuracyCriteria preferred_bearing_accuracy;
    UlpDataUsageCriteria preferred_data_usage;
    bool intermediate_pos_report_enabled;
} UlpLocationCriteria;

/** Represents the Ulp Egine interface. */
typedef struct {
    /** set to sizeof(UlpEngineInterface) */
    size_t          size;
    /**
     * Opens the interface and provides the callback routines
     * to the implemenation of this interface.
     */
    int   (*init)( UlpEngineCallbacks* callbacks );

    /* set criterias of location requests */
    int (*update_criteria) (UlpLocationCriteria criteria );

    /** Starts navigating. */
    int   (*start)( void );

    /** Stops navigating. */
    int   (*stop)( void );
} UlpEngineInterface;


/** Extended interface for raw GPS command support. */
typedef struct {
    /** set to sizeof(ExtraCmdInterface) */
    size_t          size;
    /** Injects Android extra cmd into the ulp. Clarify if they are blocking calls */
    bool  (*inject_raw_cmd)(char* bundle, int bundle_length );

} InjectRawCmdInterface;

/** ULP Network Interface */
/** Request for network position status   */
#define ULP_NETWORK_POS_STATUS_REQUEST                      (0x01)
/** Request for periodic network positions */
#define ULP_NETWORK_POS_START_PERIODIC_REQUEST              (0x02)
/** Request last known location   */
#define ULP_NETWORK_POS_GET_LAST_KNOWN_LOCATION_REQUEST     (0x03)
/** Cancel request */
#define ULP_NETWORK_POS_STOP_REQUEST                        (0x04)

/** Position was obtained using Wifi Network  */
#define ULP_NETWORK_POSITION_SRC_WIFI      (0x01)
/** Position was obtained using Cell Network  */
#define ULP_NETWORK_POSITION_SRC_CELL      (0x02)
/** Position was obtained using an Unknown Network */
#define ULP_NETWORK_POSITION_SRC_UNKNOWN   (0x00)

/** Represents the ULP network request */
typedef struct {
    /** type of request */
    uint16_t  request_type;
    /** Desired time between network positions/measurements in ms.
    *   Shall be set to 0 if only one position is requested */
    int       interval_ms;
    /** network position source to be used */
    uint16_t  desired_position_source;
}UlpNetworkRequestPos;

/** Callback with network position request. */
typedef void (*ulp_network_location_request)(UlpNetworkRequestPos *req);

/** ULP Network callback structure. */
typedef struct {
        ulp_network_location_request ulp_network_location_request_cb;
} UlpNetworkLocationCallbacks;

/** represent a network position */
typedef struct  {
    /** source of the position (Wifi, Cell) */
    uint16_t pos_source;
    /** latitude in degrees */
    double latitude;
    /** longitude in degrees */
    double longitude;
    /** Horzizontal error estimate in meters */
    float HEPE;
} UlpNetworkPosition;

/** Represents access point information */
typedef struct {
    /** Mac adderess */
    char mac_addr[6];
    /** signal strength in dbM */
    int32_t rssi;
    /** Beacon channel for access point */
    uint16_t channel;

    /** Bit 0 = AP is used by WiFi positioning system
     *  Bit 1 = AP doesn't broadcast SSID Bit 2 = AP has encrption
     *  turned on Bit 3 = AP is in infrastructure mode and not in
     *  ad-hoc/unknown mode  */
    uint8_t ap_qualifier;
} UlpNetworkAccessPointInfo;

/** Represents Wifi information */
typedef struct {
      /** Number of APs in the calculated position (-1 means
      *  unknown) */
      uint8_t num_aps_in_pos;
      /** Information of the scanned ap's used in the position estimation*/
      UlpNetworkAccessPointInfo *ap_info;
} UlpNetworkWifiInfo;


/** Represent network landscape information */
typedef struct {
    /** network type Cell/Wifi */
    uint8_t network_type;
    /** network information */
    union {
        UlpNetworkWifiInfo wifi_info;
        uint32_t cell_info;
    } u;
} UlpNetworkLandscape;

/** network report valid flags */
/** fix time is valid */
#define ULP_NETWORK_POSITION_REPORT_HAS_FIX_TIME  (0x01)
/** position is valid */
#define ULP_NETWORK_POSITION_REPORT_HAS_POSITION  (0x02)
/** landscape is valid */
#define ULP_NETWORK_POSITION_REPORT_HAS_LANDSCAPE (0x04)

/** Represents the network position report */
typedef struct
{
    /** validity flags */
    uint16_t valid_flag;
    /** time fo network fix */
    GpsUtcTime fix_time;
    /** network position */
    UlpNetworkPosition position;
    /** network landscape */
    UlpNetworkLandscape landscape_info;
}UlpNetworkPositionReport;

/** represents ULP network interface extension */
typedef struct
{
    /** set to sizeof(UlpNetworkInterface) */
    size_t          size;
    /** initialize network interface */
    int ( *init)(UlpNetworkLocationCallbacks *callback);
    /** send network position */
    int ( *ulp_send_network_position)(UlpNetworkPositionReport *position_report);
}UlpNetworkInterface;

/** Information for the ULP Phone context interface */

/** the Location settings context supports only ON_CHANGE
 *  request type */
#define ULP_PHONE_CONTEXT_GPS_SETTING                 (0x01)
#define ULP_PHONE_CONTEXT_NETWORK_POSITION_SETTING    (0x02)
#define ULP_PHONE_CONTEXT_WIFI_SETTING                (0x04)
/** The battery charging state context supports only
 * ON_CHANGE request type */
#define ULP_PHONE_CONTEXT_BATTERY_CHARGING_STATE          (0x08)
#define ULP_PHONE_CONTEXT_AGPS_SETTING                    (0x010)
#define ULP_PHONE_CONTEXT_ENH_LOCATION_SERVICES_SETTING   (0x020)

/** return phone context only once */
#define ULP_PHONE_CONTEXT_REQUEST_TYPE_SINGLE         (0x01)
/** return phone context periodcially */
#define ULP_PHONE_CONTEXT_REQUEST_TYPE_PERIODIC       (0x02)
/** return phone context when it changes */
#define ULP_PHONE_CONTEXT_REQUEST_TYPE_ON_CHANGE      (0x03)


/** Represents ULP phone context request   */
typedef struct {
    /** context type requested */
    uint16_t    context_type;
    /** request type  */
    uint16_t    request_type;
    /** interval in ms if request type is periodic */
    int            interval_ms;
}UlpPhoneContextRequest;

/** Callback for phone context request. */
typedef void (*ulp_request_phone_context)(UlpPhoneContextRequest *req);

/** ULP Phone Context callback structure. */
typedef struct {
        ulp_request_phone_context ulp_request_phone_context_cb;
}UlpPhoneContextCallbacks;

/** Represents the phone context settings */
typedef struct {
    /** Phone context information type */
    uint16_t context_type;

    /** network information */
    /** gps setting */
    bool    is_gps_enabled;
    /** is network positioning enabled */
    bool    is_network_position_available;
    /** is wifi turned on */
    bool    is_wifi_setting_enabled;
    /** is battery being currently charged */
    bool    is_battery_charging;
    /* is agps enabled for single shot */
    bool    is_agps_enabled;
    /* is Enhanced Location Services enabled by user*/
    bool    is_enh_location_services_enabled;
} UlpPhoneContextSettings;

/** Represent the phone contxt interface */
typedef struct
{
    /** set to sizeof(UlpPhoneContextInterface) */
    size_t          size;
    /** Initialize, register callback */
    int (*init)(UlpPhoneContextCallbacks *callback);
    /** send the phone context settings */
    int (*ulp_phone_context_settings_update) (UlpPhoneContextSettings *settings );
}UlpPhoneContextInterface;


//int loc_update_criteria(UlpLocationCriteria criteria);
//const void* loc_get_extension(const char* name);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__LOC_ULP_H__
