/******************************************************************************
  @file:  loc_eng_ni.cpp
  @brief:  module for network initiated interactions

  DESCRIPTION
     LOC_API network initiated operation support

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
  Copyright (c) 2009 QUALCOMM Incorporated.
  All Rights Reserved. QUALCOMM Proprietary and Confidential.
  -----------------------------------------------------------------------------
******************************************************************************/

/*=====================================================================
                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      -------------------------------------------------------
07/30/09   dx       Initial version

$Id:
======================================================================*/

#define LOG_NDDEBUG 0
#define LOG_NIDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <time.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>
#include <loc_eng.h>
#include <loc_eng_ni.h>

#define LOG_TAG "lib_locapi"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

/*=============================================================================
 *
 *                             DATA DECLARATION
 *
 *============================================================================*/

const GpsNiInterface sLocEngNiInterface =
{
    sizeof(GpsNiInterface),
    loc_eng_ni_init,
    loc_eng_ni_respond,
};

boolean loc_eng_ni_data_init = FALSE;
loc_eng_ni_data_s_type loc_eng_ni_data;

extern loc_eng_data_s_type loc_eng_data;

/*=============================================================================
 *
 *                             FUNCTION DECLARATIONS
 *
 *============================================================================*/

/*===========================================================================

FUNCTION respond_from_enum

DESCRIPTION
   Returns the name of the response

RETURN VALUE
   response name string

===========================================================================*/
static const char* respond_from_enum(rpc_loc_ni_user_resp_e_type resp)
{
    switch (resp)
    {
    case RPC_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT:
        return "accept";
    case RPC_LOC_NI_LCS_NOTIFY_VERIFY_DENY:
        return "deny";
    case RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP:
        return "no response";
    default:
        return NULL;
    }
}

/*===========================================================================

FUNCTION loc_ni_respond

DESCRIPTION
   Displays the NI request and awaits user input. If a previous request is
   in session, the new one is handled using sys.ni_default_response (if exists);
   otherwise, it is denied.

DEPENDENCY
   Do not lock the data by mutex loc_ni_lock

RETURN VALUE
   none

===========================================================================*/
static void loc_ni_respond(rpc_loc_ni_user_resp_e_type resp,
                    const rpc_loc_ni_event_s_type *request_pass_back
)
{
    LOGD("Sending NI response: %s\n", respond_from_enum(resp));

    rpc_loc_ioctl_data_u_type data;
    rpc_loc_ioctl_callback_s_type callback_payload;

    memcpy(&data.rpc_loc_ioctl_data_u_type_u.user_verify_resp.ni_event_pass_back,
            request_pass_back, sizeof (rpc_loc_ni_event_s_type));
    data.rpc_loc_ioctl_data_u_type_u.user_verify_resp.user_resp = resp;

    loc_eng_ioctl(
            loc_eng_data.client_handle,
            RPC_LOC_IOCTL_INFORM_NI_USER_RESPONSE,
            &data,
            LOC_IOCTL_DEFAULT_TIMEOUT,
            &callback_payload
    );
}

/*===========================================================================

FUNCTION loc_ni_fill_notif_verify_type

DESCRIPTION
   Fills need_notify, need_verify, etc.

RETURN VALUE
   none

===========================================================================*/
static boolean loc_ni_fill_notif_verify_type(GpsNiNotification *notif,
      rpc_loc_ni_notify_verify_e_type notif_priv)
{
    notif->notify_flags       = 0;
    notif->default_response   = GPS_NI_RESPONSE_NORESP;

    switch (notif_priv)
    {
    case RPC_LOC_NI_USER_NO_NOTIFY_NO_VERIFY:
        notif->notify_flags = 0;
        break;

    case RPC_LOC_NI_USER_NOTIFY_ONLY:
        notif->notify_flags = GPS_NI_NEED_NOTIFY;
        break;

    case RPC_LOC_NI_USER_NOTIFY_VERIFY_ALLOW_NO_RESP:
        notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
        notif->default_response = GPS_NI_RESPONSE_ACCEPT;
        break;

    case RPC_LOC_NI_USER_NOTIFY_VERIFY_NOT_ALLOW_NO_RESP:
        notif->notify_flags = GPS_NI_NEED_NOTIFY | GPS_NI_NEED_VERIFY;
        notif->default_response = GPS_NI_RESPONSE_DENY;
        break;

    case RPC_LOC_NI_USER_PRIVACY_OVERRIDE:
        notif->notify_flags = GPS_NI_PRIVACY_OVERRIDE;
        break;

    default:
        return FALSE;
    }

    return TRUE;
}

/*===========================================================================

FUNCTION hexcode

DESCRIPTION
   Converts a binary array into a Hex string. E.g., 1F 00 3F --> "1F003F"

RETURN VALUE
   bytes encoded

===========================================================================*/
static int hexcode(char *hexstring, int string_size, const char *data, int data_size)
{
    int i;
    for (i = 0; i < data_size; i++)
    {
        char ch = data[i];
        if (i*2 + 3 <= string_size)
        {
            snprintf(&hexstring[i*2], 3, "%02X", ch);
        }
        else {
            break;
        }
    }
    return i;
}

static GpsNiEncodingType convert_encoding_type(int loc_encoding)
{
    GpsNiEncodingType enc = GPS_ENC_UNKNOWN;

    switch (loc_encoding)
    {
    case RPC_LOC_NI_SUPL_UTF8:
        enc = GPS_ENC_SUPL_UTF8;
        break;
    case RPC_LOC_NI_SUPL_UCS2:
        enc = GPS_ENC_SUPL_UCS2;
        break;
    case RPC_LOC_NI_SUPL_GSM_DEFAULT:
        enc = GPS_ENC_SUPL_GSM_DEFAULT;
        break;
    default:
        break;
    }

    return enc;
}

/*===========================================================================

FUNCTION loc_ni_request_handler

DESCRIPTION
   Displays the NI request and awaits user input. If a previous request is
   in session, it is ignored.

RETURN VALUE
   none

===========================================================================*/
static void loc_ni_request_handler(const char *msg, const rpc_loc_ni_event_s_type *ni_req)
{
    GpsNiNotification notif;
    notif.size = sizeof(notif);
    strlcpy(notif.text, "[text]", sizeof notif.text);    // defaults
    strlcpy(notif.requestor_id, "[requestor id]", sizeof notif.requestor_id);

    /* If busy, use default or deny */
    if (loc_eng_ni_data.notif_in_progress)
    {
#if 0
        /* Cannot be here because the current thread is in RPC client */
        /* XXX Consider adding an event queue to process overlapped NI requests */
        loc_ni_user_resp_e_type response =
            sys.ni_default_resp == 1 /* accept */ ?
                    LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT :
                    LOC_NI_LCS_NOTIFY_VERIFY_DENY;

        loc_ni_respond(response, ni_req); */
#endif
        LOGW("loc_ni_request_handler, notification in progress, new NI request ignored, type: %d",
                ni_req->event);
    }
    else {
        /* Print notification */
        LOGD("NI Notification: %s, event: %d", msg, ni_req->event);

        pthread_mutex_lock(&loc_eng_ni_data.loc_ni_lock);

        /* Save request */
        memcpy(&loc_eng_ni_data.loc_ni_request, ni_req, sizeof loc_eng_ni_data.loc_ni_request);

        /* Set up NI response waiting */
        loc_eng_ni_data.notif_in_progress = TRUE;
        loc_eng_ni_data.current_notif_id = abs(rand());

        /* Fill in notification */
        notif.notification_id = loc_eng_ni_data.current_notif_id;

        const rpc_loc_ni_vx_notify_verify_req_s_type *vx_req;
        const rpc_loc_ni_supl_notify_verify_req_s_type *supl_req;
        const rpc_loc_ni_umts_cp_notify_verify_req_s_type *umts_cp_req;

        switch (ni_req->event)
        {
            case RPC_LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
                vx_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.vx_req;
                notif.ni_type     = GPS_NI_TYPE_VOICE;
                notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // vx_req->user_resp_timer_val;
                memset(notif.extras, 0, sizeof notif.extras);
                memset(notif.text, 0, sizeof notif.text);
                memset(notif.requestor_id, 0, sizeof notif.requestor_id);

                // Requestor ID
                hexcode(notif.requestor_id, sizeof notif.requestor_id,
                        vx_req->requester_id.requester_id,
                        vx_req->requester_id.requester_id_length);

                notif.text_encoding = 0; // No text and no encoding
                notif.requestor_id_encoding = convert_encoding_type(vx_req->encoding_scheme);

                // Set default_response & notify_flags
                loc_ni_fill_notif_verify_type(&notif, vx_req->notification_priv_type);

                break;

            case RPC_LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
                umts_cp_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.umts_cp_req;
                notif.ni_type     = GPS_NI_TYPE_UMTS_CTRL_PLANE;
                notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // umts_cp_req->user_response_timer;
                memset(notif.extras, 0, sizeof notif.extras);
                memset(notif.text, 0, sizeof notif.text);
                memset(notif.requestor_id, 0, sizeof notif.requestor_id);

                // Stores notification text
                hexcode(notif.text, sizeof notif.text,
#if (AMSS_VERSION==3200)
                        umts_cp_req->notification_text.notification_text_val,
#else
                        umts_cp_req->notification_text,
#endif
                        umts_cp_req->notification_length);

                // Stores requestor ID
                hexcode(notif.requestor_id, sizeof notif.requestor_id,
#if (AMSS_VERSION==3200)
                        umts_cp_req->requestor_id.requestor_id_string.requestor_id_string_val,
#else
                        umts_cp_req->requestor_id.requestor_id_string,
#endif
                        umts_cp_req->requestor_id.string_len);

                notif.text_encoding = convert_encoding_type(umts_cp_req->datacoding_scheme);
                notif.requestor_id_encoding = convert_encoding_type(umts_cp_req->datacoding_scheme);

                // Set default_response & notify_flags
                loc_ni_fill_notif_verify_type(&notif, umts_cp_req->notification_priv_type);

                break;

            case RPC_LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
                supl_req = &ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req;
                notif.ni_type     = GPS_NI_TYPE_UMTS_SUPL;
                notif.timeout     = LOC_NI_NO_RESPONSE_TIME; // supl_req->user_response_timer;
                memset(notif.extras, 0, sizeof notif.extras);
                memset(notif.text, 0, sizeof notif.text);
                memset(notif.requestor_id, 0, sizeof notif.requestor_id);

                // Client name
                if (supl_req->flags & RPC_LOC_NI_CLIENT_NAME_PRESENT)
                {
                    hexcode(notif.text, sizeof notif.text,
#if (AMSS_VERSION==3200)
                            supl_req->client_name.client_name_string.client_name_string_val,   /* buffer */
#else
                            supl_req->client_name.client_name_string,                          /* buffer */
#endif
                            supl_req->client_name.string_len                                   /* length */
                    );
                    LOGD("SUPL NI: client_name: %s len=%d", notif.text, supl_req->client_name.string_len);
                } else {
                    LOGD("SUPL NI: client_name not present.");
                }

                // Requestor ID
                if (supl_req->flags & RPC_LOC_NI_REQUESTOR_ID_PRESENT)
                {
                    hexcode(notif.requestor_id, sizeof notif.requestor_id,
#if (AMSS_VERSION==3200)
                            supl_req->requestor_id.requestor_id_string.requestor_id_string_val,  /* buffer */
#else
                            supl_req->requestor_id.requestor_id_string,                          /* buffer */
#endif
                            supl_req->requestor_id.string_len                                    /* length */
                    );
                    LOGD("SUPL NI: requestor_id: %s len=%d", notif.requestor_id, supl_req->requestor_id.string_len);
                } else {
                    LOGD("SUPL NI: requestor_id not present.");
                }

                // Encoding type
                if (supl_req->flags & RPC_LOC_NI_ENCODING_TYPE_PRESENT)
                {
                    notif.text_encoding = convert_encoding_type(supl_req->datacoding_scheme);
                    notif.requestor_id_encoding = convert_encoding_type(supl_req->datacoding_scheme);
                } else {
                    notif.text_encoding = notif.requestor_id_encoding = GPS_ENC_UNKNOWN;
                }

                // Set default_response & notify_flags
                loc_ni_fill_notif_verify_type(&notif, ni_req->payload.rpc_loc_ni_event_payload_u_type_u.supl_req.notification_priv_type);

                break;

            default:
                LOGE("loc_ni_request_handler, unknown request event: %d", ni_req->event);
                return;
        }

        /* Log requestor ID and text for debugging */
        LOGI("Notification: notif_type: %d, timeout: %d, default_resp: %d", notif.ni_type, notif.timeout, notif.default_response);
        LOGI("              requestor_id: %s (encoding: %d)", notif.requestor_id, notif.requestor_id_encoding);
        LOGI("              text: %s text (encoding: %d)", notif.text, notif.text_encoding);

        /* For robustness, always sets a timeout to clear up the notification status, even though
        * the OEM layer in java does not do so.
        **/
        loc_eng_ni_data.response_time_left = 5 + (notif.timeout != 0 ? notif.timeout : LOC_NI_NO_RESPONSE_TIME);
        LOGI("Automatically sends 'no response' in %d seconds (to clear status)\n", loc_eng_ni_data.response_time_left);

        pthread_mutex_unlock(&loc_eng_ni_data.loc_ni_lock);

        /* Notify callback */
        if (loc_eng_data.ni_notify_cb != NULL)
        {
            loc_eng_data.ni_notify_cb(&notif);
        }
    }
}

/*===========================================================================

FUNCTION loc_ni_process_user_response

DESCRIPTION
   Handles user input from the UI

RETURN VALUE
   error code (0 for successful, -1 for error)

===========================================================================*/
int loc_ni_process_user_response(GpsUserResponseType userResponse)
{
    LOGD("NI response from UI: %d", userResponse);

    rpc_loc_ni_user_resp_e_type resp;
    switch (userResponse)
    {
        case GPS_NI_RESPONSE_ACCEPT:
            resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_ACCEPT;
            break;
        case GPS_NI_RESPONSE_DENY:
            resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_DENY;
            break;
        case GPS_NI_RESPONSE_NORESP:
            resp = RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP;
            break;
        default:
            return -1;
    }

    loc_ni_respond(resp, &loc_eng_ni_data.loc_ni_request);

    /* Make the NI respond */
    pthread_mutex_lock(&loc_eng_ni_data.loc_ni_lock);
    loc_eng_ni_data.notif_in_progress = FALSE;
    loc_eng_ni_data.response_time_left = 0;
    loc_eng_ni_data.current_notif_id = -1;
    pthread_mutex_unlock(&loc_eng_ni_data.loc_ni_lock);

    return 0;
}

/*===========================================================================

FUNCTION loc_eng_ni_callback

DESCRIPTION
   Loc API callback handler

RETURN VALUE
   error code (0 for success)

===========================================================================*/
int loc_eng_ni_callback (
      rpc_loc_event_mask_type               loc_event,              /* event mask           */
      const rpc_loc_event_payload_u_type*   loc_event_payload       /* payload              */
)
{
    int rc = 0;
    const rpc_loc_ni_event_s_type *ni_req = &loc_event_payload->rpc_loc_event_payload_u_type_u.ni_request;
    if (loc_event == RPC_LOC_EVENT_NI_NOTIFY_VERIFY_REQUEST)
    {
        switch (ni_req->event)
        {
            case RPC_LOC_NI_EVENT_VX_NOTIFY_VERIFY_REQ:
                LOGI("VX Notification");
                loc_ni_request_handler("VX Notify", ni_req);
                break;

            case RPC_LOC_NI_EVENT_UMTS_CP_NOTIFY_VERIFY_REQ:
                LOGI("UMTS CP Notification\n");
                loc_ni_request_handler("UMTS CP Notify", ni_req);
                break;

            case RPC_LOC_NI_EVENT_SUPL_NOTIFY_VERIFY_REQ:
                LOGI("SUPL Notification\n");
                loc_ni_request_handler("SUPL Notify", ni_req);
                break;

            default:
                LOGE("Unknown NI event: %x\n", (int) ni_req->event);
                break;
        }
    }
    return rc;
}

/*===========================================================================

FUNCTION loc_ni_thread_proc

===========================================================================*/
static void loc_ni_thread_proc(void *unused)
{
    LOGI("Starting Loc NI thread...\n");

    while (1)
    {
        /* wakes up every second to check timed out requests */
        sleep(1);

        pthread_mutex_lock(&loc_eng_ni_data.loc_ni_lock);

        if (loc_eng_ni_data.notif_in_progress && loc_eng_ni_data.response_time_left > 0)
        {
            loc_eng_ni_data.response_time_left--;
            if (loc_eng_ni_data.response_time_left <= 0)
            {
                loc_ni_respond(RPC_LOC_NI_LCS_NOTIFY_VERIFY_NORESP, &loc_eng_ni_data.loc_ni_request);
                loc_eng_ni_data.notif_in_progress = FALSE;
            }
        }

        pthread_mutex_unlock(&loc_eng_ni_data.loc_ni_lock);
    } /* while (1) */
}

/*===========================================================================
FUNCTION    loc_eng_ni_init

DESCRIPTION
   This function initializes the NI interface

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_eng_ni_init(GpsNiCallbacks *callbacks)
{
    LOGD("loc_eng_ni_init: entered.");

    if (!loc_eng_ni_data_init)
    {
        pthread_mutex_init(&loc_eng_ni_data.loc_ni_lock, NULL);
        callbacks->create_thread_cb("loc_api_ni", loc_ni_thread_proc, NULL);
        loc_eng_ni_data_init = TRUE;
    }

    loc_eng_ni_data.notif_in_progress = FALSE;
    loc_eng_ni_data.current_notif_id = -1;
    loc_eng_ni_data.response_time_left = 0;

    srand(time(NULL));
    loc_eng_data.ni_notify_cb = callbacks->notify_cb;
}

/*===========================================================================
FUNCTION    loc_eng_ni_respond

DESCRIPTION
   This function sends an NI respond to the modem processor

DEPENDENCIES
   NONE

RETURN VALUE
   None

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_eng_ni_respond(int notif_id, GpsUserResponseType user_response)
{
    if (notif_id == loc_eng_ni_data.current_notif_id && loc_eng_ni_data.notif_in_progress)
    {
        LOGI("loc_eng_ni_respond: send user response %d for notif %d", user_response, notif_id);
        loc_ni_process_user_response(user_response);
    } else {
        LOGE("loc_eng_ni_respond: notif_id %d mismatch or notification not in progress, response: %d",
            notif_id, user_response);
    }
}
