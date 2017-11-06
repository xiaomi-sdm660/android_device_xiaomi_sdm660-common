/******************************************************************************
 *  Copyright (C) 2016-2017, The Linux Foundation. All rights reserved.
 *
 *  Not a Contribution
 *****************************************************************************/
/*****************************************************************************
 *  Copyright (C) 2009-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/

/*****************************************************************************
 *
 *  Filename:      audio_a2dp_hw.h
 *
 *  Description:
 *
 *****************************************************************************/
#ifndef BT_HOST_IPC_H
#define BT_HOST_IPC_H
#include <system/audio.h>
/*****************************************************************************
**  Constants & Macros
******************************************************************************/
#define BT_AUDIO_HARDWARE_INTERFACE "libbthost"

typedef enum {
    A2DP_CTRL_CMD_NONE,
    A2DP_CTRL_CMD_CHECK_READY,
    A2DP_CTRL_CMD_CHECK_STREAM_STARTED,
    A2DP_CTRL_CMD_START,
    A2DP_CTRL_CMD_STOP,
    A2DP_CTRL_CMD_SUSPEND,
    A2DP_CTRL_GET_AUDIO_CONFIG,
    A2DP_CTRL_CMD_OFFLOAD_START,
    A2DP_CTRL_CMD_OFFLOAD_SUPPORTED,
    A2DP_CTRL_CMD_OFFLOAD_NOT_SUPPORTED,
} tA2DP_CTRL_CMD;

typedef enum {
    A2DP_CTRL_ACK_SUCCESS,
    A2DP_CTRL_ACK_FAILURE,
    A2DP_CTRL_ACK_INCALL_FAILURE, /* Failure when in Call*/
    A2DP_CTRL_ACK_UNSUPPORTED,
    A2DP_CTRL_ACK_PENDING,
    A2DP_CTRL_ACK_DISCONNECT_IN_PROGRESS,
    A2DP_CTRL_SKT_DISCONNECTED,
    A2DP_CTRL_ACK_UNKNOWN,
} tA2DP_CTRL_ACK;


typedef enum {
    AUDIO_A2DP_STATE_STARTING,
    AUDIO_A2DP_STATE_STARTED,
    AUDIO_A2DP_STATE_STOPPING,
    AUDIO_A2DP_STATE_STOPPED,
    AUDIO_A2DP_STATE_SUSPENDED, /* need explicit set param call to resume (suspend=false) */
    AUDIO_A2DP_STATE_STANDBY    /* allows write to autoresume */
} a2dp_state_t;

typedef enum {
    A2DP_CTRL_GET_CODEC_CONFIG = 15,
    A2DP_CTRL_GET_MULTICAST_STATUS,
    A2DP_CTRL_GET_CONNECTION_STATUS,
} tA2DP_CTRL_EXT_CMD;

#define  MAX_CODEC_CFG_SIZE  30
struct a2dp_config {
    uint32_t                rate;
    uint32_t                channel_flags;
    int                     format;
};
struct a2dp_stream_common {
    pthread_mutex_t         lock;
    pthread_mutex_t         ack_lock;
    //int                     ctrl_fd;
    //int                     audio_fd;
    //size_t                  buffer_sz;
    //struct a2dp_config      cfg;
    a2dp_state_t            state;
    tA2DP_CTRL_ACK          ack_status;
    uint8_t                 multicast;
    uint8_t                 num_conn_dev;
    uint8_t                 codec_cfg[MAX_CODEC_CFG_SIZE];
    uint16_t                sink_latency;
};
/*
codec specific definitions
*/
#define CODEC_TYPE_SBC 0x00
#define CODEC_TYPE_AAC 0x02
#define NON_A2DP_CODEC_TYPE 0xFF
#define CODEC_OFFSET 3
#define VENDOR_ID_OFFSET 4
#define CODEC_ID_OFFSET (VENDOR_ID_OFFSET + 4)
#define CODEC_TYPE_PCM 0x05

#ifndef VENDOR_APTX
#define VENDOR_APTX 0x4F
#endif
#ifndef VENDOR_APTX_HD
#define VENDOR_APTX_HD 0xD7
#endif
#ifndef VENDOR_APTX_LL
#define VENDOR_APTX_LL 0x0A
#endif
#ifndef APTX_CODEC_ID
#define APTX_CODEC_ID 0x01
#endif
#ifndef APTX_HD_CODEC_ID
#define APTX_HD_CODEC_ID 0x24
#endif

#define A2D_SBC_FREQ_MASK 0xF0
#define A2D_SBC_CHN_MASK  0x0F
#define A2D_SBC_BLK_MASK  0xF0
#define A2D_SBC_SUBBAND_MASK 0x0C
#define A2D_SBC_ALLOC_MASK 0x03
#define A2D_SBC_SAMP_FREQ_16     0x80    /* b7:16  kHz */
#define A2D_SBC_SAMP_FREQ_32     0x40    /* b6:32  kHz */
#define A2D_SBC_SAMP_FREQ_44     0x20    /* b5:44.1kHz */
#define A2D_SBC_SAMP_FREQ_48     0x10    /* b4:48  kHz */
#define A2D_SBC_CH_MD_MONO       0x08    /* b3: mono */
#define A2D_SBC_CH_MD_DUAL       0x04    /* b2: dual */
#define A2D_SBC_CH_MD_STEREO     0x02    /* b1: stereo */
#define A2D_SBC_CH_MD_JOINT      0x01    /* b0: joint stereo */
#define A2D_SBC_BLOCKS_4         0x80    /* 4 blocks */
#define A2D_SBC_BLOCKS_8         0x40    /* 8 blocks */
#define A2D_SBC_BLOCKS_12        0x20    /* 12blocks */
#define A2D_SBC_BLOCKS_16        0x10    /* 16blocks */
#define A2D_SBC_SUBBAND_4        0x08    /* b3: 4 */
#define A2D_SBC_SUBBAND_8        0x04    /* b2: 8 */
#define A2D_SBC_ALLOC_MD_S       0x02    /* b1: SNR */
#define A2D_SBC_ALLOC_MD_L       0x01    /* b0: loundess */

/* APTX bitmask helper */
#define A2D_APTX_SAMP_FREQ_MASK  0xF0
#define A2D_APTX_SAMP_FREQ_48    0x10
#define A2D_APTX_SAMP_FREQ_44    0x20
#define A2D_APTX_CHAN_MASK       0x0F
#define A2D_APTX_CHAN_STEREO     0x02
#define A2D_APTX_CHAN_MONO       0x01


#define A2D_AAC_IE_OBJ_TYPE_MSK                0xF0    /* b7-b4 Object Type */
#define A2D_AAC_IE_OBJ_TYPE_MPEG_2_AAC_LC      0x80    /* b7:MPEG-2 AAC LC */
#define A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_LC      0x40    /* b7:MPEG-4 AAC LC */
#define A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_LTP     0x20    /* b7:MPEG-4 AAC LTP */
#define A2D_AAC_IE_OBJ_TYPE_MPEG_4_AAC_SCA     0x10    /* b7:MPEG-4 AAC SCALABLE */

#define A2D_AAC_IE_CHANNELS_MSK                0x0C
#define A2D_AAC_IE_CHANNELS_1                  0x08    /* Channel 1 */
#define A2D_AAC_IE_CHANNELS_2                  0x04    /* Channel 2 */

#define A2D_AAC_IE_VBR_MSK                     0x80
#define A2D_AAC_IE_VBR                         0x80    /* supported */

#define A2DP_DEFAULT_SINK_LATENCY 0

typedef struct {
    uint32_t subband;    /* 4, 8 */
    uint32_t blk_len;    /* 4, 8, 12, 16 */
    uint16_t sampling_rate; /*44.1khz,48khz*/
    uint8_t  channels;      /*0(Mono),1(Dual_mono),2(Stereo),3(JS)*/
    uint8_t  alloc;         /*0(Loudness),1(SNR)*/
    uint8_t  min_bitpool;   /* 2 */
    uint8_t  max_bitpool;   /*53(44.1khz),51 (48khz) */
    uint32_t bitrate;      /* 320kbps to 512kbps */
} audio_sbc_encoder_config_t;


/* Information about BT APTX encoder configuration
 * This data is used between audio HAL module and
 * BT IPC library to configure DSP encoder
 */
typedef struct {
    uint16_t sampling_rate;
    uint8_t  channels;
    uint32_t bitrate;
} audio_aptx_encoder_config_t;


/* Information about BT AAC encoder configuration
 * This data is used between audio HAL module and
 * BT IPC library to configure DSP encoder
 */
typedef struct {
    uint32_t enc_mode; /* LC, SBR, PS */
    uint16_t format_flag; /* RAW, ADTS */
    uint16_t channels; /* 1-Mono, 2-Stereo */
    uint32_t sampling_rate;
    uint32_t bitrate;
} audio_aac_encoder_config_t;

//HIDL callbacks to invoke callback to BT stack
typedef void (*bt_ipc_start_stream_req_cb)(void);
typedef void (*bt_ipc_suspend_stream_req_cb)(void);
typedef void (*bt_ipc_stop_stream_req_cb)(void);
typedef void (*bt_ipc_a2dp_check_ready_cb)(void);
typedef void (*bt_ipc_get_codec_config_cb)(void);
typedef void (*bt_ipc_get_multicast_status_cb)(void);
typedef void (*bt_ipc_get_connected_devices_cb)(void);
typedef void (*bt_ipc_get_connection_status_cb)(void);
typedef void (*bt_ipc_get_sink_latency_cb)(void);
typedef struct {
 bt_ipc_start_stream_req_cb start_req_cb;
 bt_ipc_suspend_stream_req_cb suspend_req_cb;
 bt_ipc_stop_stream_req_cb stop_req_cb;
 bt_ipc_a2dp_check_ready_cb a2dp_check_ready_cb;
 bt_ipc_get_multicast_status_cb get_mcast_status_cb;
 bt_ipc_get_connected_devices_cb get_connected_device_cb;
 bt_ipc_get_connection_status_cb get_connection_status_cb;
 bt_ipc_get_codec_config_cb get_codec_cfg_cb;
 bt_ipc_get_sink_latency_cb get_sink_latency_cb;
}bt_lib_callback_t;

void bt_stack_init(bt_lib_callback_t *lib_cb);
void bt_stack_deinit(tA2DP_CTRL_ACK status);
void bt_stack_on_stream_started(tA2DP_CTRL_ACK status);
void bt_stack_on_stream_suspended(tA2DP_CTRL_ACK status);
void bt_stack_on_stream_stopped(tA2DP_CTRL_ACK status);
void bt_stack_on_get_codec_cfg(tA2DP_CTRL_ACK status, const char *config, size_t len);
void bt_stack_on_get_mcast_status(uint8_t status);
void bt_stack_on_get_num_connected_devices(uint8_t num);
void bt_stack_on_get_connection_status(tA2DP_CTRL_ACK status);
void bt_stack_on_check_a2dp_ready(tA2DP_CTRL_ACK status);
void bt_stack_on_get_sink_latency(uint16_t latency);
int audio_stream_open(void);
int audio_stream_close(void);
int audio_start_stream(void);
int audio_stop_stream(void);
int audio_suspend_stream(void);
void* audio_get_codec_config(uint8_t *mcast, uint8_t *num_dev, audio_format_t *codec_type);
void audio_handoff_triggered(void);
void clear_a2dpsuspend_flag(void);
void* audio_get_next_codec_config(uint8_t idx, audio_format_t *codec_type);
int audio_check_a2dp_ready(void);
uint16_t audio_get_a2dp_sink_latency();
int wait_for_stack_response(uint8_t duration);
#endif

