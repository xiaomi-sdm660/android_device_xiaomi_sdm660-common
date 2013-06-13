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


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif //__LOC_ULP_H__
