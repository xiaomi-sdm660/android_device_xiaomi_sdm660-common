
/******************************************************************************
  @file:  loc_eng.cpp
  @brief:

  DESCRIPTION
    This file defines the implemenation for GPS hardware abstraction layer.

  INITIALIZATION AND SEQUENCING REQUIREMENTS

  -----------------------------------------------------------------------------
Copyright (c) 2009, QUALCOMM USA, INC.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

·         Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer. 

·         Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution. 

·         Neither the name of the QUALCOMM USA, INC.  nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  -----------------------------------------------------------------------------

******************************************************************************/

/*=====================================================================
$Header: $
$DateTime: $
$Author: $
======================================================================*/
#define LOG_NDDEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>

#include <hardware/gps.h>

#include <loc_eng.h>

#define LOG_TAG "lib_locapi"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

// Function declarations
static boolean loc_eng_ioctl_setup_cb(
    rpc_loc_client_handle_type    handle,
    rpc_loc_ioctl_e_type          ioctl_type
);

static boolean loc_eng_ioctl_wait_cb(
    int                            timeout_msec,  // Timeout in this number of msec
    rpc_loc_ioctl_callback_s_type *cb_data_ptr    // Output parameter for IOCTL calls
);

/*===========================================================================

FUNCTION    loc_eng_ioctl

DESCRIPTION
   This function calls loc_ioctl and waits for the callback result before
   returning back to the user.

DEPENDENCIES
   N/A

RETURN VALUE
   TRUE                 if successful
   FALSE                if failed

SIDE EFFECTS
   N/A

===========================================================================*/
boolean loc_eng_ioctl(
    rpc_loc_client_handle_type           handle,
    rpc_loc_ioctl_e_type                 ioctl_type,
    rpc_loc_ioctl_data_u_type*           ioctl_data_ptr,
    uint32                               timeout_msec,
    rpc_loc_ioctl_callback_s_type       *cb_data_ptr
    )
{
    boolean                    ret_val;
    int                        rpc_ret_val;
    loc_eng_ioctl_data_s_type *ioctl_cb_data_ptr;

    LOGV ("loc_eng_ioctl: client = %d, ioctl_type = %d, cb_data =0x%x\n", (int32) handle, ioctl_type, (uint32) cb_data_ptr);

    ioctl_cb_data_ptr = &(loc_eng_data.ioctl_data);
    // Select the callback we are waiting for
    ret_val = loc_eng_ioctl_setup_cb (handle, ioctl_type);

    if (ret_val == TRUE) 
    {
        rpc_ret_val =  loc_ioctl (handle,
                                    ioctl_type,
                                    ioctl_data_ptr);

        LOGV ("loc_eng_ioctl: loc_ioctl returned %d \n", rpc_ret_val);

        if (rpc_ret_val == RPC_LOC_API_SUCCESS)
        {
            // Wait for the callback of loc_ioctl
            ret_val = loc_eng_ioctl_wait_cb (timeout_msec, cb_data_ptr);
        }
        else
        {
            ret_val = FALSE;
        }
    }

    // Reset the state when we are done
    pthread_mutex_lock(&ioctl_cb_data_ptr->cb_data_mutex);
    ioctl_cb_data_ptr->cb_is_selected = FALSE;
    ioctl_cb_data_ptr->cb_is_waiting  = FALSE;
    ioctl_cb_data_ptr->cb_has_arrived = FALSE;
    pthread_mutex_unlock(&ioctl_cb_data_ptr->cb_data_mutex);

    return ret_val;
}


/*===========================================================================

FUNCTION    loc_eng_ioctl_setup_cb

DESCRIPTION
   Selects which callback is going to be waited for

DEPENDENCIES
   N/A

RETURN VALUE
   TRUE                 if successful
   FALSE                if failed

SIDE EFFECTS
   N/A

===========================================================================*/
static boolean loc_eng_ioctl_setup_cb(
    rpc_loc_client_handle_type    handle,
    rpc_loc_ioctl_e_type          ioctl_type
    )
{
    boolean ret_val;
    loc_eng_ioctl_data_s_type *ioctl_cb_data_ptr;

    ioctl_cb_data_ptr = &(loc_eng_data.ioctl_data);

    pthread_mutex_lock(&ioctl_cb_data_ptr->cb_data_mutex);
    if (ioctl_cb_data_ptr->cb_is_selected == TRUE)
    {
        LOGD ("loc_eng_ioctl_setup_cb: ERROR, another ioctl in progress \n");
        ret_val = FALSE;
    }
    else
    {
        ioctl_cb_data_ptr->cb_is_selected = TRUE;
        ioctl_cb_data_ptr->cb_is_waiting  = FALSE;
        ioctl_cb_data_ptr->cb_has_arrived = FALSE;
        ioctl_cb_data_ptr->client_handle  = handle;
        ioctl_cb_data_ptr->ioctl_type     = ioctl_type;
        memset (&(ioctl_cb_data_ptr->cb_payload), 0, sizeof (rpc_loc_ioctl_callback_s_type));
        ret_val = TRUE;
    }
    pthread_mutex_unlock(&ioctl_cb_data_ptr->cb_data_mutex);

    return ret_val;
}

/*===========================================================================

FUNCTION    loc_eng_ioctl_wait_cb

DESCRIPTION
   Waits for a selected callback. The wait expires in timeout_msec.

   If the function is called before an existing wait has finished, it will
   immediately return EBUSY.

DEPENDENCIES
   N/A

RETURN VALUE
   TRUE                 if successful
   FALSE                if failed

SIDE EFFECTS
   N/A

===========================================================================*/
boolean loc_eng_ioctl_wait_cb(
    int                            timeout_msec,  // Timeout in this number of msec
    rpc_loc_ioctl_callback_s_type *cb_data_ptr
    )
{
    boolean ret_val = FALSE; // the return value of this function
    int rc;                  // return code from pthread calls

    struct timeval present_time;
    struct timespec expire_time;
    loc_eng_ioctl_data_s_type *ioctl_cb_data_ptr;

    ioctl_cb_data_ptr = &(loc_eng_data.ioctl_data);

    pthread_mutex_lock(&ioctl_cb_data_ptr->cb_data_mutex);

    do {
        if (ioctl_cb_data_ptr->cb_is_selected == FALSE)
        {
            LOGD ("loc_eng_ioctl_wait_cb: ERROR called when cb_is_waiting is set to FALSE \n");
            ret_val = FALSE;
            break;
        }

        // Calculate absolute expire time
        gettimeofday(&present_time, NULL);
        expire_time.tv_sec  = present_time.tv_sec;
        expire_time.tv_sec  +=  timeout_msec / 1000;
        if ((present_time.tv_usec + timeout_msec) >= 1000)
        {
            expire_time.tv_sec += 1;
        }
        expire_time.tv_nsec = (present_time.tv_usec + timeout_msec) % 1000 * 1000;

        // Special case where callback is issued before loc_ioctl ever returns
        if (ioctl_cb_data_ptr->cb_has_arrived == TRUE)
        {
            LOGD ("loc_eng_ioctl_wait_cb: cb has arrived without waiting \n");
            ret_val = TRUE;
            break;
        }
   
        ioctl_cb_data_ptr->cb_is_waiting = TRUE;
        // Wait for the callback until timeout expires
        rc = pthread_cond_timedwait(&ioctl_cb_data_ptr->cb_arrived_cond,
                                    &ioctl_cb_data_ptr->cb_data_mutex,
                                    &expire_time);

        if (rc == 0)
        {
            ret_val = TRUE;
        }
        else
        {
            ret_val = FALSE;
        }

        LOGV ("loc_eng_ioctl_wait_cb: pthread_cond_timedwait returned %d\n", rc);

    } while (0);

    // Process the ioctl callback data when IOCTL is successful
    if (ret_val == TRUE)
    {
        ioctl_cb_data_ptr = &(loc_eng_data.ioctl_data);
        if (ioctl_cb_data_ptr->cb_payload.status == RPC_LOC_API_SUCCESS)
        {
            ret_val = TRUE;
            if (cb_data_ptr != NULL)
            {
                memcpy (cb_data_ptr,
                        &(ioctl_cb_data_ptr->cb_payload),
                        sizeof (rpc_loc_ioctl_callback_s_type));
            }
        }
        else
        {
            ret_val = FALSE;
        }
    }

    pthread_mutex_unlock(&ioctl_cb_data_ptr->cb_data_mutex);

    LOGV ("loc_eng_ioctl_wait_cb: returned %d\n", ret_val);
    return ret_val;
}

/*===========================================================================

FUNCTION    loc_eng_ioctl_process_cb

DESCRIPTION
   This function process the IOCTL callback, parameter specifies the client 
   that receives the IOCTL callback.

DEPENDENCIES
   N/A

RETURN VALUE
   TRUE                 if successful
   FALSE                if failed

SIDE EFFECTS
   N/A

===========================================================================*/
boolean loc_eng_ioctl_process_cb (
    rpc_loc_client_handle_type           client_handle,
    const rpc_loc_ioctl_callback_s_type *cb_data_ptr
    )
{
    boolean ret_val = FALSE; // the return value of this function
    loc_eng_ioctl_data_s_type *ioctl_cb_data_ptr;
    ioctl_cb_data_ptr = &(loc_eng_data.ioctl_data);

    pthread_mutex_lock(&ioctl_cb_data_ptr->cb_data_mutex);
    if (client_handle != ioctl_cb_data_ptr->client_handle)
    {
        LOGD ("loc_eng_ioctl_process_cb: client handle mismatch, received = %d, expected = %d \n",
                (int32) client_handle, (int32) ioctl_cb_data_ptr->client_handle);
        ret_val = FALSE;
    }
    else if (cb_data_ptr->type != ioctl_cb_data_ptr->ioctl_type)
    {
        LOGD ("loc_eng_ioctl_process_cb: ioctl type mismatch, received = %d, expected = %d \n",
                 cb_data_ptr->type, ioctl_cb_data_ptr->ioctl_type);
        ret_val = FALSE;
    }
    else // both matches
    {
        memcpy (&(ioctl_cb_data_ptr->cb_payload),
                cb_data_ptr,
                sizeof (rpc_loc_ioctl_callback_s_type));

        ioctl_cb_data_ptr->cb_has_arrived = TRUE;

        LOGV ("loc_eng_ioctl_process_cb: callback arrived for client = %d, ioctl = %d, status = %d\n",
                (int32) ioctl_cb_data_ptr->client_handle, ioctl_cb_data_ptr->ioctl_type,
                (int32) ioctl_cb_data_ptr->cb_payload.status);

        ret_val = TRUE;
    }

    pthread_mutex_unlock(&ioctl_cb_data_ptr->cb_data_mutex);

    // Signal the waiting thread that callback has arrived
    if (ret_val == TRUE)
    {
        pthread_cond_signal (&ioctl_cb_data_ptr->cb_arrived_cond);
    }

    return ret_val;
}
