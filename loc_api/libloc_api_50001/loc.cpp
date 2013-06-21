/* Copyright (c) 2011-2013, The Linux Foundation. All rights reserved.
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

#define LOG_NDDEBUG 0
#define LOG_TAG "LocSvc_afw"

#include <hardware/gps.h>
#include <loc_ulp.h>
#include <loc_eng.h>
#include <loc_target.h>
#include <loc_log.h>
#include <msg_q.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <android_runtime/AndroidRuntime.h>

#include <cutils/properties.h>

//Globals defns
static const ulpInterface * loc_eng_ulp_inf = NULL;
static const ulpInterface * loc_eng_get_ulp_inf(void);
static gps_location_callback gps_loc_cb = NULL;
static gps_sv_status_callback gps_sv_cb = NULL;

static void local_loc_cb(UlpLocation* location, void* locExt);
static void local_status_cb(GpsStatus* status);
static void local_sv_cb(GpsSvStatus* sv_status, void* svExt);
static void local_nmea_cb(GpsUtcTime timestamp, const char* nmea, int length);
static void local_set_capabilities_cb(uint32_t capabilities);
static void local_acquire_wakelock_cb(void);
static void local_release_wakelock_cb(void);
static void local_request_utc_time_cb(void);

// Function declarations for sLocEngInterface
static int  loc_init(GpsCallbacks* callbacks);
static int  loc_hal_init(void);
static int  loc_start();
static int  loc_stop();
static void loc_cleanup();
static int  loc_inject_time(GpsUtcTime time, int64_t timeReference, int uncertainty);
static int  loc_inject_location(double latitude, double longitude, float accuracy);
static void loc_delete_aiding_data(GpsAidingData f);
static int  loc_set_position_mode(GpsPositionMode mode, GpsPositionRecurrence recurrence,
                                  uint32_t min_interval, uint32_t preferred_accuracy,
                                  uint32_t preferred_time);

//ULP/Hybrid provider Function definitions
static int loc_ulp_network_init(UlpNetworkLocationCallbacks *callbacks);
static int loc_ulp_send_network_position(UlpNetworkPositionReport *position_report);
static int loc_ulp_phone_context_init(UlpPhoneContextCallbacks *callback);
static int loc_ulp_phone_context_settings_update(UlpPhoneContextSettings *settings);
static int loc_ulp_engine_update_criteria(UlpLocationCriteria criteria);
static const void* loc_get_extension(const char* name);
static ulp_location_callback ulp_loc_cb = NULL;
static int loc_ulp_engine_init(UlpEngineCallbacks* callbacks);
static int  loc_ulp_engine_start();
static int  loc_ulp_engine_stop();


// Defines the GpsInterface in gps.h
static const GpsInterface sLocEngInterface =
{
   sizeof(GpsInterface),
   loc_init,
   loc_start,
   loc_stop,
   loc_cleanup,
   loc_inject_time,
   loc_inject_location,
   loc_delete_aiding_data,
   loc_set_position_mode,
   loc_get_extension
};

// Function declarations for sLocEngAGpsInterface
static void loc_agps_init(AGpsCallbacks* callbacks);
#ifdef FEATURE_IPV6
static int  loc_agps_open(AGpsType agpsType,
                          const char* apn, AGpsBearerType bearerType);
static int  loc_agps_closed(AGpsType agpsType);
static int  loc_agps_open_failed(AGpsType agpsType);
#else
static int  loc_agps_open(const char* apn);
static int  loc_agps_closed();
static int  loc_agps_open_failed();
#endif
static int  loc_agps_set_server(AGpsType type, const char *hostname, int port);

static const AGpsInterface sLocEngAGpsInterface =
{
   sizeof(AGpsInterface),
   loc_agps_init,
   loc_agps_open,
   loc_agps_closed,
   loc_agps_open_failed,
   loc_agps_set_server
};

static int loc_xtra_init(GpsXtraCallbacks* callbacks);
static int loc_xtra_inject_data(char* data, int length);

static const GpsXtraInterface sLocEngXTRAInterface =
{
    sizeof(GpsXtraInterface),
    loc_xtra_init,
    loc_xtra_inject_data
};

static void loc_ni_init(GpsNiCallbacks *callbacks);
static void loc_ni_respond(int notif_id, GpsUserResponseType user_response);

const GpsNiInterface sLocEngNiInterface =
{
   sizeof(GpsNiInterface),
   loc_ni_init,
   loc_ni_respond,
};

static void loc_agps_ril_init( AGpsRilCallbacks* callbacks );
static void loc_agps_ril_set_ref_location(const AGpsRefLocation *agps_reflocation, size_t sz_struct);
static void loc_agps_ril_set_set_id(AGpsSetIDType type, const char* setid);
static void loc_agps_ril_ni_message(uint8_t *msg, size_t len);
static void loc_agps_ril_update_network_state(int connected, int type, int roaming, const char* extra_info);
static void loc_agps_ril_update_network_availability(int avaiable, const char* apn);

static const AGpsRilInterface sLocEngAGpsRilInterface =
{
   sizeof(AGpsRilInterface),
   loc_agps_ril_init,
   loc_agps_ril_set_ref_location,
   loc_agps_ril_set_set_id,
   loc_agps_ril_ni_message,
   loc_agps_ril_update_network_state,
   loc_agps_ril_update_network_availability
};

static const UlpEngineInterface sLocEngUlpEngInterface =
{
   sizeof(UlpEngineInterface),
   loc_ulp_engine_init,
   loc_ulp_engine_update_criteria,
   loc_ulp_engine_start,
   loc_ulp_engine_stop
};

static bool loc_inject_raw_command(char* command, int length);

static const InjectRawCmdInterface sLocEngInjectRawCmdInterface =
{
   sizeof(InjectRawCmdInterface),
   loc_inject_raw_command
};

//ULP/Hybrid provider interfaces
static const UlpNetworkInterface sUlpNetworkInterface =
{
   sizeof(UlpNetworkInterface),
   loc_ulp_network_init,
   loc_ulp_send_network_position
};
static const UlpPhoneContextInterface sLocEngUlpPhoneContextInterface =
{
    sizeof(UlpPhoneContextInterface),
    loc_ulp_phone_context_init,
    loc_ulp_phone_context_settings_update
};

static loc_eng_data_s_type loc_afw_data;
static UlpCallbacks ulp_cb_data;
static LocCallbacks afw_cb_data;
static int gss_fd = 0;

/*===========================================================================
FUNCTION    gps_get_hardware_interface

DESCRIPTION
   Returns the GPS hardware interaface based on LOC API
   if GPS is enabled.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
const GpsInterface* gps_get_hardware_interface ()
{
    ENTRY_LOG_CALLFLOW();
    const GpsInterface* ret_val;

    char propBuf[PROPERTY_VALUE_MAX];

    // check to see if GPS should be disabled
    property_get("gps.disable", propBuf, "");
    if (propBuf[0] == '1')
    {
        LOC_LOGD("gps_get_interface returning NULL because gps.disable=1\n");
        ret_val = NULL;
    } else {
        ret_val = &sLocEngInterface;
    }

    if (0 != loc_hal_init()) {
        LOC_LOGE("HAL could not be initialized");
        ret_val = NULL;
    } else {
        ret_val = &sLocEngInterface;
    }

    loc_eng_read_config();

    //We load up libulp module at this point itself
    loc_eng_ulp_inf = loc_eng_get_ulp_inf();

    EXIT_LOG(%p, ret_val);
    return ret_val;
}

// for gps.c
extern "C" const GpsInterface* get_gps_interface()
{
    targetEnumType target = TARGET_OTHER;
    if (NULL == loc_afw_data.context) {
        loc_eng_read_config();

        //We load up libulp module at this point itself
        loc_eng_ulp_inf = loc_eng_get_ulp_inf();

        target = get_target();
        LOC_LOGD("Target name check returned %s", loc_get_target_name(target));
        //APQ8064 and APQ8030
        if((target == TARGET_APQ8064_STANDALONE) || (target == TARGET_APQ8030_STANDALONE)) {
            gps_conf.CAPABILITIES &= ~(GPS_CAPABILITY_MSA | GPS_CAPABILITY_MSB);
            gss_fd = open("/dev/gss", O_RDONLY);
            if (gss_fd < 0) {
                LOC_LOGE("GSS open failed: %s\n", strerror(errno));
            }
            else {
                LOC_LOGD("GSS open success! CAPABILITIES %0lx\n", gps_conf.CAPABILITIES);
            }
        }
        //MPQ8064
        else if(target == TARGET_MPQ8064) {
            LOC_LOGE("No GPS HW on this target (MPQ8064). Not returning interface");
            return NULL;
        }
        if (0 != loc_hal_init()) {
            LOC_LOGE("HAL could not be initialized");
            return NULL;
        }
    }
    return &sLocEngInterface;
}

static void loc_free_msg(void* msg)
{
    delete (loc_eng_msg*)msg;
}

void loc_ulp_msg_sender(void* loc_eng_data_p, void* msg)
{
    LocEngContext* loc_eng_context = (LocEngContext*)((loc_eng_data_s_type*)loc_eng_data_p)->context;
    msg_q_snd((void*)loc_eng_context->ulp_q, msg, loc_free_msg);
}

/*===========================================================================
FUNCTION    loc_hal_init

DESCRIPTION
   Initialize the location engine, this include setting up global datas
   and registers location engien with loc api service.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/Ax

===========================================================================*/
static int loc_hal_init(void)
{
    int retVal = -1;
    ENTRY_LOG();
    LOC_API_ADAPTER_EVENT_MASK_T event =
        LOC_API_ADAPTER_BIT_PARSED_POSITION_REPORT |
        LOC_API_ADAPTER_BIT_SATELLITE_REPORT |
        LOC_API_ADAPTER_BIT_LOCATION_SERVER_REQUEST |
        LOC_API_ADAPTER_BIT_ASSISTANCE_DATA_REQUEST |
        LOC_API_ADAPTER_BIT_IOCTL_REPORT |
        LOC_API_ADAPTER_BIT_STATUS_REPORT |
        LOC_API_ADAPTER_BIT_NMEA_1HZ_REPORT |
        LOC_API_ADAPTER_BIT_NI_NOTIFY_VERIFY_REQUEST;
    LocCallbacks clientCallbacks = {local_loc_cb, /* location_cb */
                                    local_status_cb, /* status_cb */
                                    local_sv_cb, /* sv_status_cb */
                                    local_nmea_cb, /* nmea_cb */
                                    local_set_capabilities_cb, /* set_capabilities_cb */
                                    local_acquire_wakelock_cb, /* acquire_wakelock_cb */
                                    local_release_wakelock_cb, /* release_wakelock_cb */
                                    (pthread_t (*)(const char*, void (*)(void*), void*))
                                    android::AndroidRuntime::createJavaThread, /* create_thread_cb */
                                    NULL, /* location_ext_parser */
                                    NULL, /* sv_ext_parser */
                                    local_request_utc_time_cb /* request_utc_time_cb */};

    if (loc_eng_ulp_inf == NULL)
        retVal = loc_eng_init(loc_afw_data, &clientCallbacks, event,
                              NULL);
    else
        retVal = loc_eng_init(loc_afw_data, &clientCallbacks, event,
                              loc_ulp_msg_sender);

    int ret_val1 = loc_eng_ulp_init(loc_afw_data, loc_eng_ulp_inf);
    //Initialize the cached min_interval
    loc_afw_data.min_interval_cached = ULP_MIN_INTERVAL_INVALID;
    LOC_LOGD("loc_eng_ulp_init returned %d\n",ret_val1);

    EXIT_LOG(%d, retVal);
    return retVal;
}

/*===========================================================================
FUNCTION    loc_cleanup

DESCRIPTION
   Cleans location engine. The location client handle will be released.

DEPENDENCIES
   None

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_cleanup()
{
    ENTRY_LOG();
    loc_eng_cleanup(loc_afw_data);
    gps_loc_cb = NULL;
    gps_sv_cb = NULL;

    /*
     * if (get_target() == TARGET_NAME_APQ8064_STANDALONE)
     * {
     *     close(gss_fd);
     *     LOC_LOGD("GSS shutdown.\n");
     * }
     */

    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION    loc_start

DESCRIPTION
   Starts the tracking session

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_start()
{
    ENTRY_LOG();
    int ret_val = loc_eng_start(loc_afw_data);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_stop

DESCRIPTION
   Stops the tracking session

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_stop()
{
    ENTRY_LOG();
    int ret_val = -1;
    if (loc_afw_data.ulp_initialized) {
        //ULP initialized so we need to simulate REMOVE_CRITERIA for
        //last client to libulp and we dont need to send loc_eng_stop
        UlpLocationCriteria native_criteria;
        native_criteria.valid_mask = (ULP_CRITERIA_HAS_ACTION | ULP_CRITERIA_HAS_PROVIDER_SOURCE | ULP_CRITERIA_HAS_RECURRENCE_TYPE |
                                  ULP_CRITERIA_HAS_MIN_INTERVAL);
        native_criteria.provider_source = ULP_PROVIDER_SOURCE_GNSS;
        native_criteria.min_distance = 0; //This is not used by ULP engine so leaving it 0 for now
        native_criteria.recurrence_type = loc_afw_data.recurrence_type_cached;
        loc_afw_data.recurrence_type_cached = ULP_LOC_RECURRENCE_PERIODIC;
        //For a GPS client horizontal_accuracy & power_consumption are irrelevant
        native_criteria.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_DONT_CARE;
        native_criteria.preferred_power_consumption = ULP_POWER_REQ_DONT_CARE;
        native_criteria.action = ULP_REMOVE_CRITERIA;
        native_criteria.min_interval = loc_afw_data.min_interval_cached;
        loc_afw_data.min_interval_cached = ULP_MIN_INTERVAL_INVALID;
        ret_val = loc_eng_update_criteria(loc_afw_data, native_criteria);
    } else {
        ret_val = loc_eng_stop(loc_afw_data);
    }
    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_set_position_mode

DESCRIPTION
   Sets the mode and fix frequency for the tracking session.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int  loc_set_position_mode(GpsPositionMode mode,
                                  GpsPositionRecurrence recurrence,
                                  uint32_t min_interval,
                                  uint32_t preferred_accuracy,
                                  uint32_t preferred_time)
{
    ENTRY_LOG();
    int ret_val = -1;
    if (!loc_afw_data.ulp_initialized) {
        LocPositionMode locMode;
        switch (mode) {
        case GPS_POSITION_MODE_MS_BASED:
            locMode = LOC_POSITION_MODE_MS_BASED;
            break;
        case GPS_POSITION_MODE_MS_ASSISTED:
            locMode = LOC_POSITION_MODE_MS_ASSISTED;
            break;
        default:
            locMode = LOC_POSITION_MODE_STANDALONE;
            break;
        }

        LocPosMode params(locMode, recurrence, min_interval,
                          preferred_accuracy, preferred_time, NULL, NULL);
        ret_val = loc_eng_set_position_mode(loc_afw_data, params);
    } else {
        //ULP initialized so suppress set_position_mode updates to loc_eng
        UlpLocationCriteria native_criteria;
        native_criteria.valid_mask = (ULP_CRITERIA_HAS_ACTION | ULP_CRITERIA_HAS_PROVIDER_SOURCE | ULP_CRITERIA_HAS_RECURRENCE_TYPE |
                                  ULP_CRITERIA_HAS_MIN_INTERVAL);
        native_criteria.provider_source = ULP_PROVIDER_SOURCE_GNSS;
        native_criteria.min_distance = 0; //This is not used by ULP engine so leaving it 0 for now

        if (LOC_POSITION_MODE_MS_ASSISTED == mode)
            native_criteria.recurrence_type = ULP_LOC_RECURRENCE_SINGLE;
        else
            native_criteria.recurrence_type = ULP_LOC_RECURRENCE_PERIODIC;

        //For a GPS client horizontal_accuracy & power_consumption are irrelevant
        native_criteria.preferred_horizontal_accuracy = ULP_HORZ_ACCURACY_DONT_CARE;
        native_criteria.preferred_power_consumption = ULP_POWER_REQ_DONT_CARE;
        native_criteria.action = ULP_ADD_CRITERIA;
        native_criteria.min_interval = min_interval;
        loc_afw_data.min_interval_cached = min_interval; //cache a copy
        loc_afw_data.recurrence_type_cached = native_criteria.recurrence_type; //cache a copy
        ret_val = loc_eng_update_criteria(loc_afw_data, native_criteria);
    }

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_inject_time

DESCRIPTION
   This is used by Java native function to do time injection.

DEPENDENCIES
   None

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_inject_time(GpsUtcTime time, int64_t timeReference, int uncertainty)
{
    ENTRY_LOG();
    int ret_val = loc_eng_inject_time(loc_afw_data, time, timeReference, uncertainty);
    EXIT_LOG(%d, ret_val);
    return ret_val;
}


/*===========================================================================
FUNCTION    loc_inject_location

DESCRIPTION
   This is used by Java native function to do location injection.

DEPENDENCIES
   None

RETURN VALUE
   0          : Successful
   error code : Failure

SIDE EFFECTS
   N/A
===========================================================================*/
static int loc_inject_location(double latitude, double longitude, float accuracy)
{
    static bool initialized = false;
    static bool enable_cpi = true;
    ENTRY_LOG();

    if(!initialized)
    {
        char value[PROPERTY_VALUE_MAX];
        memset(value, 0, sizeof(value));
        (void)property_get("persist.gps.qc_nlp_in_use", value, "0");
        if(0 == strcmp(value, "1"))
        {
            enable_cpi = false;
            LOC_LOGI("GPS HAL coarse position injection disabled");
        }
        else
        {
            LOC_LOGI("GPS HAL coarse position injection enabled");
        }
        initialized = true;
    }

    int ret_val = 0;
    if(enable_cpi)
    {
      ret_val = loc_eng_inject_location(loc_afw_data, latitude, longitude, accuracy);
    }
    EXIT_LOG(%d, ret_val);
    return ret_val;
}


/*===========================================================================
FUNCTION    loc_delete_aiding_data

DESCRIPTION
   This is used by Java native function to delete the aiding data. The function
   updates the global variable for the aiding data to be deleted. If the GPS
   engine is off, the aiding data will be deleted. Otherwise, the actual action
   will happen when gps engine is turned off.

DEPENDENCIES
   Assumes the aiding data type specified in GpsAidingData matches with
   LOC API specification.

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_delete_aiding_data(GpsAidingData f)
{
    ENTRY_LOG();
    loc_eng_delete_aiding_data(loc_afw_data, f);

    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION    loc_update_criteria

DESCRIPTION
   This is used to inform the ULP module of new unique criteria that are passed
   in by the applications
DEPENDENCIES
   N/A

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_ulp_engine_update_criteria(UlpLocationCriteria criteria)
{
    ENTRY_LOG();
    int ret_val = loc_eng_update_criteria(loc_afw_data, criteria);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_get_extension

DESCRIPTION
   Get the gps extension to support XTRA.

DEPENDENCIES
   N/A

RETURN VALUE
   The GPS extension interface.

SIDE EFFECTS
   N/A

===========================================================================*/
const void* loc_get_extension(const char* name)
{
    ENTRY_LOG();
    const void* ret_val = NULL;

   LOC_LOGD("%s:%d] For Interface = %s\n",__func__, __LINE__, name);
   if (strcmp(name, GPS_XTRA_INTERFACE) == 0)
   {
      ret_val = &sLocEngXTRAInterface;
   }

   else if (strcmp(name, AGPS_INTERFACE) == 0)
   {
      ret_val = &sLocEngAGpsInterface;
   }

   else if (strcmp(name, GPS_NI_INTERFACE) == 0)
   {
      ret_val = &sLocEngNiInterface;
   }

   else if (strcmp(name, AGPS_RIL_INTERFACE) == 0)
   {
       char baseband[PROPERTY_VALUE_MAX];
       property_get("ro.baseband", baseband, "msm");
       if (strcmp(baseband, "csfb") == 0)
       {
           ret_val = &sLocEngAGpsRilInterface;
       }
   }
   else if (strcmp(name, ULP_ENGINE_INTERFACE) == 0)
   {
      ret_val = &sLocEngUlpEngInterface;
   }
   else if (strcmp(name, ULP_RAW_CMD_INTERFACE) == 0)
   {
      ret_val = &sLocEngInjectRawCmdInterface;
   }
   else if(strcmp(name, ULP_PHONE_CONTEXT_INTERFACE) == 0)
   {
     ret_val = &sLocEngUlpPhoneContextInterface;
   }
   else if(strcmp(name, ULP_NETWORK_INTERFACE) == 0)
   {
     ret_val = &sUlpNetworkInterface;
   }

   else
   {
      LOC_LOGE ("get_extension: Invalid interface passed in\n");
   }
    EXIT_LOG(%p, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_agps_init

DESCRIPTION
   Initialize the AGps interface.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_agps_init(AGpsCallbacks* callbacks)
{
    ENTRY_LOG();
    loc_eng_agps_init(loc_afw_data, callbacks);
    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION    loc_agps_open

DESCRIPTION
   This function is called when on-demand data connection opening is successful.
It should inform ARM 9 about the data open result.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
#ifdef FEATURE_IPV6
static int loc_agps_open(AGpsType agpsType,
                         const char* apn, AGpsBearerType bearerType)
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_open(loc_afw_data, agpsType, apn, bearerType);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#else
static int loc_agps_open(const char* apn)
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_open(loc_afw_data, apn);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#endif

/*===========================================================================
FUNCTION    loc_agps_closed

DESCRIPTION
   This function is called when on-demand data connection closing is done.
It should inform ARM 9 about the data close result.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
#ifdef FEATURE_IPV6
static int loc_agps_closed(AGpsType agpsType)
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_closed(loc_afw_data, agpsType);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#else
static int loc_agps_closed()
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_closed(loc_afw_data);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#endif

/*===========================================================================
FUNCTION    loc_agps_open_failed

DESCRIPTION
   This function is called when on-demand data connection opening has failed.
It should inform ARM 9 about the data open result.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
#ifdef FEATURE_IPV6
int loc_agps_open_failed(AGpsType agpsType)
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_open_failed(loc_afw_data, agpsType);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#else
int loc_agps_open_failed()
{
    ENTRY_LOG();
    int ret_val = loc_eng_agps_open_failed(loc_afw_data);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}
#endif

/*===========================================================================
FUNCTION    loc_agps_set_server

DESCRIPTION
   If loc_eng_set_server is called before loc_eng_init, it doesn't work. This
   proxy buffers server settings and calls loc_eng_set_server when the client is
   open.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_agps_set_server(AGpsType type, const char* hostname, int port)
{
    ENTRY_LOG();
    LocServerType serverType;
    switch (type) {
    case AGPS_TYPE_SUPL:
        serverType = LOC_AGPS_SUPL_SERVER;
        break;
    case AGPS_TYPE_C2K:
        serverType = LOC_AGPS_CDMA_PDE_SERVER;
        break;
    }
    int ret_val = loc_eng_set_server_proxy(loc_afw_data, serverType, hostname, port);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_xtra_init

DESCRIPTION
   Initialize XTRA module.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_xtra_init(GpsXtraCallbacks* callbacks)
{
    ENTRY_LOG();
    int ret_val = loc_eng_xtra_init(loc_afw_data, callbacks);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}


/*===========================================================================
FUNCTION    loc_xtra_inject_data

DESCRIPTION
   Initialize XTRA module.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_xtra_inject_data(char* data, int length)
{
    ENTRY_LOG();
    int ret_val = loc_eng_xtra_inject_data(loc_afw_data, data, length);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_ni_init

DESCRIPTION
   This function initializes the NI interface

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_ni_init(GpsNiCallbacks *callbacks)
{
    ENTRY_LOG();
    loc_eng_ni_init(loc_afw_data, callbacks);
    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION    loc_ni_respond

DESCRIPTION
   This function sends an NI respond to the modem processor

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_ni_respond(int notif_id, GpsUserResponseType user_response)
{
    ENTRY_LOG();
    loc_eng_ni_respond(loc_afw_data, notif_id, user_response);
    EXIT_LOG(%s, VOID_RET);
}

// Below stub functions are members of sLocEngAGpsRilInterface
static void loc_agps_ril_init( AGpsRilCallbacks* callbacks ) {}
static void loc_agps_ril_set_ref_location(const AGpsRefLocation *agps_reflocation, size_t sz_struct) {}
static void loc_agps_ril_set_set_id(AGpsSetIDType type, const char* setid) {}
static void loc_agps_ril_ni_message(uint8_t *msg, size_t len) {}
static void loc_agps_ril_update_network_state(int connected, int type, int roaming, const char* extra_info) {}

/*===========================================================================
FUNCTION    loc_agps_ril_update_network_availability

DESCRIPTION
   Sets data call allow vs disallow flag to modem
   This is the only member of sLocEngAGpsRilInterface implemented.

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_agps_ril_update_network_availability(int available, const char* apn)
{
    ENTRY_LOG();
    loc_eng_agps_ril_update_network_availability(loc_afw_data, available, apn);
    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION    loc_inject_raw_command

DESCRIPTION
   This is used to send special test modem commands from the applications
   down into the HAL
DEPENDENCIES
   N/A

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static bool loc_inject_raw_command(char* command, int length)
{
    ENTRY_LOG();
    int ret_val = loc_eng_inject_raw_command(loc_afw_data, command, length);
    EXIT_LOG(%s, loc_logger_boolStr[ret_val!=0]);
    return ret_val;
}

static void local_loc_cb(UlpLocation* location, void* locExt)
{
    ENTRY_LOG();
    if (NULL != location) {
        CALLBACK_LOG_CALLFLOW("location_cb - from", %d, location->position_source);
        if (ULP_LOCATION_IS_FROM_GNSS == location->position_source ) {
            if (NULL != gps_loc_cb) {
                gps_loc_cb(&location->gpsLocation);
            } else {
                   LOC_LOGE("Error. GPS not enabled");
               }
        } else {
            if (NULL != ulp_loc_cb) {
                ulp_loc_cb(location);
            }
        }
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_sv_cb(GpsSvStatus* sv_status, void* svExt)
{
    ENTRY_LOG();
    if (NULL != gps_sv_cb) {
        CALLBACK_LOG_CALLFLOW("sv_status_cb -", %d, sv_status->num_svs);
        gps_sv_cb(sv_status);
    } else {
           LOC_LOGE("Error. GPS not enabled");
       }
    EXIT_LOG(%s, VOID_RET);
}

/*===========================================================================
FUNCTION loc_eng_get_ulp_inf

DESCRIPTION
   This function checks if ULP is enabled, and loads the libulp2.so and
   returns its interface

DEPENDENCIES
   None

RETURN VALUE
   interface pointer to libulp: no error
   NULL: errors

SIDE EFFECTS
   N/A

===========================================================================*/
const ulpInterface * loc_eng_get_ulp_inf(void)
{
    ENTRY_LOG();
    void *handle;
    const char *error;
    get_ulp_interface* get_ulp_inf;
    const ulpInterface* loc_eng_ulpInf = NULL;

    dlerror();    /* Clear any existing error */

    handle = dlopen ("libulp2.so", RTLD_NOW);

    if (!handle)
    {
        if ((error = dlerror()) != NULL)  {
            LOC_LOGE ("%s, dlopen for libulp.so failed, error = %s\n", __func__, error);
           }
        goto exit;
    }
    dlerror();    /* Clear any existing error */
    get_ulp_inf = (get_ulp_interface*) dlsym(handle, "ulp_get_interface");
    if ((error = dlerror()) != NULL)  {
        LOC_LOGE ("%s, dlsym for ulpInterface failed, error = %s\n", __func__, error);
        goto exit;
     }

    // Initialize the ULP interface
    loc_eng_ulpInf = get_ulp_inf();

exit:
    EXIT_LOG(%d, loc_eng_ulpInf == NULL);
    return loc_eng_ulpInf;
}

/*===========================================================================
FUNCTION    loc_ulp_network_init

DESCRIPTION
   Initialize the ULP network interface.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_ulp_phone_context_init(UlpPhoneContextCallbacks *callbacks)
{
    ENTRY_LOG();
    int ret_val = -1;
    ret_val = loc_eng_ulp_phone_context_init(loc_afw_data, callbacks);
    EXIT_LOG(%d, ret_val);
    return ret_val;
}
/*===========================================================================
FUNCTION    loc_ulp_phone_context_settings_update

DESCRIPTION
   This is used to inform the ULP module of phone settings changes carried out
   by the users
DEPENDENCIES
   N/A

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/

static int loc_ulp_phone_context_settings_update(UlpPhoneContextSettings *settings)
{
    ENTRY_LOG();
    int ret_val = -1;
    ret_val = loc_eng_ulp_phone_context_settings_update(loc_afw_data, settings);
    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_ulp_network_init

DESCRIPTION
   Initialize the ULP network interface.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_ulp_network_init(UlpNetworkLocationCallbacks *callbacks)
{
   ENTRY_LOG();
   int ret_val = -1;
   ret_val = loc_eng_ulp_network_init(loc_afw_data, callbacks);
   EXIT_LOG(%d, ret_val);
   return ret_val;
}

/*===========================================================================
FUNCTION    loc_eng_ulp_send_network_position

DESCRIPTION
   Ulp send data

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
int loc_ulp_send_network_position(UlpNetworkPositionReport *position_report)
{
    ENTRY_LOG();
    int ret_val = -1;
    ret_val = loc_eng_ulp_send_network_position(loc_afw_data, position_report);
    EXIT_LOG(%d, ret_val);
    return ret_val;
}

/*===========================================================================
FUNCTION    loc_ulp_engine_init

DESCRIPTION
   Initialize the ULP Engine interface.

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_ulp_engine_init(UlpEngineCallbacks* callbacks)
{
    int retVal = -1;
    ENTRY_LOG();
    if(callbacks == NULL) {
        LOC_LOGE("loc_ulp_engine_init failed. cb = NULL\n");
        EXIT_LOG(%d, retVal);
        return retVal;
    }
    //Intilize the ulp call back cache at this point
    memset(&ulp_cb_data, 0, sizeof(UlpCallbacks));
    ulp_loc_cb = callbacks->location_cb;
    retVal = 0;
    EXIT_LOG(%d, retVal);
    return retVal;
}

static int loc_ulp_engine_start()
{
    ENTRY_LOG();
    int ret_val = loc_eng_start(loc_afw_data);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}

static int loc_ulp_engine_stop()
{
    ENTRY_LOG();
    int ret_val = loc_eng_stop(loc_afw_data);

    EXIT_LOG(%d, ret_val);
    return ret_val;
}


/*===========================================================================
FUNCTION    loc_init

DESCRIPTION
   Registers the AFW call backs with the local tables

DEPENDENCIES
   None

RETURN VALUE
   0: success

SIDE EFFECTS
   N/Ax

===========================================================================*/
static int loc_init(GpsCallbacks* callbacks)
{
    int retVal = -1;
    ENTRY_LOG();
    if(callbacks == NULL) {
        LOC_LOGE(" loc_init. cb = NULL\n");
        EXIT_LOG(%d, retVal);
        return retVal;
    }
    memset(&afw_cb_data, NULL, sizeof (LocCallbacks));
    gps_loc_cb = callbacks->location_cb;
    afw_cb_data.status_cb = callbacks->status_cb;
    gps_sv_cb = callbacks->sv_status_cb;
    afw_cb_data.set_capabilities_cb = callbacks->set_capabilities_cb;
    afw_cb_data.acquire_wakelock_cb = callbacks->acquire_wakelock_cb;
    afw_cb_data.release_wakelock_cb = callbacks->release_wakelock_cb;
    afw_cb_data.request_utc_time_cb = callbacks->request_utc_time_cb;
    afw_cb_data.nmea_cb = callbacks->nmea_cb;
    if (NULL != callbacks->set_capabilities_cb) {
        callbacks->set_capabilities_cb(gps_conf.CAPABILITIES);
    }
    retVal = 0;
    EXIT_LOG(%d, retVal);
    return retVal;
}

static void local_nmea_cb(GpsUtcTime timestamp, const char* nmea, int length)
{
    ENTRY_LOG();
    if (0 != length) {
        if (NULL != afw_cb_data.nmea_cb) {
             afw_cb_data.nmea_cb(timestamp, nmea, length);
        } else
        {
            LOC_LOGE("Error. GPS not enabled");
        }
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_set_capabilities_cb(uint32_t capabilities)
{
    ENTRY_LOG();
    if (NULL != afw_cb_data.set_capabilities_cb) {
        afw_cb_data.set_capabilities_cb(capabilities);
    } else {
           LOC_LOGE("Error. GPS not enabled");
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_acquire_wakelock_cb(void)
{
    ENTRY_LOG();
    if (NULL != afw_cb_data.acquire_wakelock_cb) {
        afw_cb_data.acquire_wakelock_cb();
    } else {
        LOC_LOGE("Error. GPS not enabled");
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_release_wakelock_cb(void)
{
    ENTRY_LOG();
    if (NULL != afw_cb_data.release_wakelock_cb) {
        afw_cb_data.release_wakelock_cb();
    } else {
        LOC_LOGE("Error. GPS not enabled");
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_request_utc_time_cb(void)
{
    ENTRY_LOG();
    if (NULL != afw_cb_data.request_utc_time_cb) {
        afw_cb_data.request_utc_time_cb();
    } else {
        LOC_LOGE("Error. GPS not enabled");
    }
    EXIT_LOG(%s, VOID_RET);
}

static void local_status_cb(GpsStatus* status)
{
    ENTRY_LOG();
    if (NULL != status) {
        CALLBACK_LOG_CALLFLOW("status_callback - status", %d, status->status);
        if (NULL != afw_cb_data.status_cb) {
            afw_cb_data.status_cb(status);
        } else {
            LOC_LOGE("Error. GPS not enabled");
        }
    }
    EXIT_LOG(%s, VOID_RET);
}
