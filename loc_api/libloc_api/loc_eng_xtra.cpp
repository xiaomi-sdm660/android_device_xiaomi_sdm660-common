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
#include <unistd.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include <pthread.h>

#include <rpc/rpc.h>
#include <loc_api_rpc_glue.h>

#include <loc_eng.h>

#define LOG_TAG "lib_locapi"
#include <utils/Log.h>

// comment this out to enable logging
// #undef LOGD
// #define LOGD(...) {}

#define LOC_XTRA_INJECT_DEFAULT_TIMEOUT (3100)
#define XTRA_BLOCK_SIZE                 (400)

static int qct_loc_eng_xtra_init (GpsXtraCallbacks* callbacks);
static int qct_loc_eng_inject_xtra_data(char* data, int length);

const GpsXtraInterface sLocEngXTRAInterface =
{
    sizeof(GpsXtraInterface),
    qct_loc_eng_xtra_init,
    qct_loc_eng_inject_xtra_data,
};

/*===========================================================================
FUNCTION    qct_loc_eng_xtra_init

DESCRIPTION
   Initialize XTRA module.

DEPENDENCIES
   N/A

RETURN VALUE
   0: success

SIDE EFFECTS
   N/A

===========================================================================*/
static int qct_loc_eng_xtra_init (GpsXtraCallbacks* callbacks)
{
    rpc_loc_event_mask_type event;
    loc_eng_xtra_data_s_type *xtra_module_data_ptr;

    xtra_module_data_ptr = &(loc_eng_data.xtra_module_data);
    xtra_module_data_ptr->download_request_cb = callbacks->download_request_cb;

    return 0;
}

/*===========================================================================
FUNCTION    qct_loc_eng_inject_xtra_data

DESCRIPTION
   Injects XTRA file into the engine. 

DEPENDENCIES
   N/A

RETURN VALUE
   0: success
   >0: failure

SIDE EFFECTS
   N/A

===========================================================================*/
static int qct_loc_eng_inject_xtra_data(char* data, int length)
{
    int     rpc_ret_val = RPC_LOC_API_GENERAL_FAILURE;
    boolean ret_val = 0;
    int     total_parts;
    uint8   part;
    uint16  part_len;
    uint16  len_injected;
    rpc_loc_ioctl_data_u_type            ioctl_data;
    rpc_loc_predicted_orbits_data_s_type *predicted_orbits_data_ptr;

    LOGV ("qct_loc_eng_inject_xtra_data, xtra size = %d, data ptr = 0x%x\n", length, (int) data);

    ioctl_data.disc = RPC_LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA;

    predicted_orbits_data_ptr = &(ioctl_data.rpc_loc_ioctl_data_u_type_u.predicted_orbits_data);
    predicted_orbits_data_ptr->format_type = RPC_LOC_PREDICTED_ORBITS_XTRA;
    predicted_orbits_data_ptr->total_size = length;
    total_parts = (length / XTRA_BLOCK_SIZE);
    if ((total_parts % XTRA_BLOCK_SIZE) != 0)
    {
        total_parts += 1;
    }
    predicted_orbits_data_ptr->total_parts = total_parts;

    len_injected = 0; // O bytes injected
    // XTRA injection starts with part 1
    for (part = 1; part <= total_parts; part++)
    {
        predicted_orbits_data_ptr->part = part;
        predicted_orbits_data_ptr->part_len = XTRA_BLOCK_SIZE;
        if (XTRA_BLOCK_SIZE > (length - len_injected))
        {
            predicted_orbits_data_ptr->part_len = length - len_injected;
        }
        predicted_orbits_data_ptr->data_ptr.data_ptr_len = predicted_orbits_data_ptr->part_len;
        predicted_orbits_data_ptr->data_ptr.data_ptr_val = data + len_injected;

        LOGV ("qct_loc_eng_inject_xtra_data, inject part = %d, len = %d, len = %d\n", predicted_orbits_data_ptr->part, predicted_orbits_data_ptr->part_len, predicted_orbits_data_ptr->data_ptr.data_ptr_len);
        LOGV ("qct_loc_eng_inject_xtra_data, total part = %d, len = %d \n", predicted_orbits_data_ptr->part, predicted_orbits_data_ptr->part_len);

        if (part < total_parts)
        {
            // No callback in this case
            rpc_ret_val = loc_ioctl (loc_eng_data.client_handle,
                                  RPC_LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA,
                                  &ioctl_data);

            if (rpc_ret_val != RPC_LOC_API_SUCCESS)
            {
                LOGE ("loc_ioctl for xtra returned %d \n", rpc_ret_val);
                ret_val = EINVAL; // return error
                break;
            }
        }
        else // part == total_parts
        {
            // Last part injection, will need to wait for callback
            ret_val = loc_eng_ioctl (loc_eng_data.client_handle,
                                  RPC_LOC_IOCTL_INJECT_PREDICTED_ORBITS_DATA,
                                  &ioctl_data,
                                  LOC_XTRA_INJECT_DEFAULT_TIMEOUT,
                                  NULL /* No output information is expected*/);
            break; // done with injection
        }

        len_injected += predicted_orbits_data_ptr->part_len;
        LOGV ("loc_ioctl for xtra len injected %d \n", len_injected);
    }

    return ret_val;
}
