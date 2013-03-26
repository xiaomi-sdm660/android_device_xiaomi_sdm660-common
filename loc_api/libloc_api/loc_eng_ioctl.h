/* Copyright (c) 2009,2011 Code Aurora Forum. All rights reserved.
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
 *
 */

#ifndef LOC_ENG_IOCTL_H
#define LOC_ENG_IOCTL_H

// Module data
typedef struct loc_eng_ioctl_data_s_type
{
    // We are waiting for an ioctl callback
    boolean                       cb_is_selected;
    // The thread has been put in a wait state for an ioctl callback
    boolean                       cb_is_waiting;
    // Loc client handle that is waiting for the callback
    rpc_loc_client_handle_type    client_handle;
    // IOCTL type that the loc client is waiting for
    rpc_loc_ioctl_e_type          ioctl_type;
    // The IOCLT report has arrived for the waiting client
    boolean                       cb_has_arrived;
    // The payload for the RPC_LOC_EVENT_IOCTL_REPORT
    rpc_loc_ioctl_callback_s_type cb_payload;
    // Mutex to access this data structure
    pthread_mutex_t               cb_data_mutex;
    // LOC ioctl callback arrived mutex
    pthread_cond_t                cb_arrived_cond;
} loc_eng_ioctl_data_s_type;


extern boolean loc_eng_ioctl
(
    rpc_loc_client_handle_type           handle,
    rpc_loc_ioctl_e_type                 ioctl_type,
    rpc_loc_ioctl_data_u_type*           ioctl_data_ptr,
    uint32                               timeout_msec,
    rpc_loc_ioctl_callback_s_type       *cb_data_ptr
);

extern boolean loc_eng_ioctl_process_cb 
(
    rpc_loc_client_handle_type           client_handle,
    const rpc_loc_ioctl_callback_s_type *cb_data_ptr
);
#endif // LOC_ENG_IOCTL_H
