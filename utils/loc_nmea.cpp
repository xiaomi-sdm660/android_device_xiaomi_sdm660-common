/* Copyright (c) 2012-2017, The Linux Foundation. All rights reserved.
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
 *     * Neither the name of The Linux Foundation nor the names of its
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

#define LOG_NDEBUG 0
#define LOG_TAG "LocSvc_nmea"
#include <loc_nmea.h>
#include <math.h>
#include <log_util.h>
#include <loc_pla.h>

#define GLONASS_SV_ID_OFFSET 64
#define MAX_SATELLITES_IN_USE 12
#define MSEC_IN_ONE_WEEK      604800000ULL
#define UTC_GPS_OFFSET_MSECS  315964800000ULL

// GNSS system id according to NMEA spec
#define SYSTEM_ID_GPS          1
#define SYSTEM_ID_GLONASS      2
#define SYSTEM_ID_GALILEO      3
// Extended systems
#define SYSTEM_ID_BEIDOU       4
#define SYSTEM_ID_QZSS         5

//GNSS signal id according to NMEA spec
#define SIGNAL_ID_ALL_SIGNALS  0
#define SIGNAL_ID_GPS_L1CA     1
#define SIGNAL_ID_GPS_L1P      2
#define SIGNAL_ID_GPS_L1M      3
#define SIGNAL_ID_GPS_L2P      4
#define SIGNAL_ID_GPS_L2CM     5
#define SIGNAL_ID_GPS_L2CL     6
#define SIGNAL_ID_GPS_L5I      7
#define SIGNAL_ID_GPS_L5Q      8


#define SIGNAL_ID_GLO_G1CA     1
#define SIGNAL_ID_GLO_G1P      2
#define SIGNAL_ID_GLO_G2CA     3
#define SIGNAL_ID_GLO_G2P      4


#define SIGNAL_ID_GAL_E5A      1
#define SIGNAL_ID_GAL_E5B      2
#define SIGNAL_ID_GAL_E5AB     3
#define SIGNAL_ID_GAL_E6A      4
#define SIGNAL_ID_GAL_E6BC     5
#define SIGNAL_ID_GAL_L1A      6
#define SIGNAL_ID_GAL_L1BC     7

//Extended signal id
#define SIGNAL_ID_BDS_B1I      1
#define SIGNAL_ID_BDS_B1C      2
#define SIGNAL_ID_BDS_B2I      3
#define SIGNAL_ID_BDS_B2AI     4


typedef struct loc_nmea_sv_meta_s
{
    char talker[3];
    LocGnssConstellationType svType;
    uint32_t mask;
    uint32_t svCount;
    uint32_t svIdOffset;
    uint32_t signalId;
    uint32_t systemId;
} loc_nmea_sv_meta;

typedef struct loc_sv_cache_info_s
{
    uint32_t gps_used_mask;
    uint32_t glo_used_mask;
    uint32_t gal_used_mask;
    uint32_t qzss_used_mask;
    uint32_t bds_used_mask;
    uint32_t gps_l1_count;
    uint32_t gps_l5_count;
    uint32_t glo_g1_count;
    uint32_t glo_g2_count;
    uint32_t gal_e1_count;
    uint32_t gal_e5_count;
    uint32_t qzss_l1_count;
    uint32_t qzss_l5_count;
    uint32_t bds_b1_count;
    uint32_t bds_b2_count;
    float hdop;
    float pdop;
    float vdop;
} loc_sv_cache_info;

/*===========================================================================
FUNCTION    convert_signalType_to_signalId

DESCRIPTION
   convert signalType to signal ID

DEPENDENCIES
   NONE

RETURN VALUE
   value of signal ID

SIDE EFFECTS
   N/A

===========================================================================*/
static uint32_t convert_signalType_to_signalId(GnssSignalTypeMask signalType)
{
    uint32_t signalId = SIGNAL_ID_ALL_SIGNALS;

    switch (signalType) {
        case GNSS_SIGNAL_GPS_L1CA:
            signalId = SIGNAL_ID_GPS_L1CA;
            break;
        case GNSS_SIGNAL_GPS_L2:
            signalId = SIGNAL_ID_GPS_L2CL;
            break;
        case GNSS_SIGNAL_GPS_L5:
            signalId = SIGNAL_ID_GPS_L5Q;
            break;
        case GNSS_SIGNAL_GLONASS_G1:
            signalId = SIGNAL_ID_GLO_G1CA;
            break;
        case GNSS_SIGNAL_GLONASS_G2:
            signalId = SIGNAL_ID_GLO_G2CA;
            break;
        case GNSS_SIGNAL_GALILEO_E1:
            signalId = SIGNAL_ID_GAL_L1BC;
            break;
        case GNSS_SIGNAL_GALILEO_E5A:
            signalId = SIGNAL_ID_GAL_E5A;
            break;
        case GNSS_SIGNAL_GALILEO_E5B:
            signalId = SIGNAL_ID_GAL_E5B;
            break;
        case GNSS_SIGNAL_QZSS_L1CA:
            signalId = SIGNAL_ID_GPS_L1CA;
            break;
        case GNSS_SIGNAL_QZSS_L2:
            signalId = SIGNAL_ID_GPS_L2CL;
            break;
        case GNSS_SIGNAL_QZSS_L5:
            signalId = SIGNAL_ID_GPS_L5Q;
            break;
        case GNSS_SIGNAL_BEIDOU_B1I:
            signalId = SIGNAL_ID_BDS_B1I;
            break;
        case GNSS_SIGNAL_BEIDOU_B1C:
            signalId = SIGNAL_ID_BDS_B1C;
            break;
        case GNSS_SIGNAL_BEIDOU_B2I:
            signalId = SIGNAL_ID_BDS_B2I;
            break;
        case GNSS_SIGNAL_BEIDOU_B2AI:
            signalId = SIGNAL_ID_BDS_B2AI;
            break;
        default:
            signalId = SIGNAL_ID_ALL_SIGNALS;
    }

    return signalId;

}

/*===========================================================================
FUNCTION    loc_nmea_sv_meta_init

DESCRIPTION
   Init loc_nmea_sv_meta passed in

DEPENDENCIES
   NONE

RETURN VALUE
   Pointer to loc_nmea_sv_meta

SIDE EFFECTS
   N/A

===========================================================================*/
static loc_nmea_sv_meta* loc_nmea_sv_meta_init(loc_nmea_sv_meta& sv_meta,
                                               loc_sv_cache_info& sv_cache_info,
                                               GnssSvType svType,
                                               GnssSignalTypeMask signalType,
                                               bool needCombine)
{
    memset(&sv_meta, 0, sizeof(sv_meta));
    sv_meta.svType = svType;

    switch (svType)
    {
        case GNSS_SV_TYPE_GPS:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'P';
            sv_meta.mask = sv_cache_info.gps_used_mask;
            sv_meta.systemId = SYSTEM_ID_GPS;
            if (GNSS_SIGNAL_GPS_L1CA == signalType) {
                sv_meta.svCount = sv_cache_info.gps_l1_count;
            } else if (GNSS_SIGNAL_GPS_L5 == signalType) {
                sv_meta.svCount = sv_cache_info.gps_l5_count;
            }
            break;
        case GNSS_SV_TYPE_GLONASS:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'L';
            sv_meta.mask = sv_cache_info.glo_used_mask;
            // GLONASS SV ids are from 65-96
            sv_meta.svIdOffset = GLONASS_SV_ID_OFFSET;
            sv_meta.systemId = SYSTEM_ID_GLONASS;
            if (GNSS_SIGNAL_GLONASS_G1 == signalType) {
                sv_meta.svCount = sv_cache_info.glo_g1_count;
            } else if (GNSS_SIGNAL_GLONASS_G2 == signalType) {
                sv_meta.svCount = sv_cache_info.glo_g2_count;
            }
            break;
        case GNSS_SV_TYPE_GALILEO:
            sv_meta.talker[0] = 'G';
            sv_meta.talker[1] = 'A';
            sv_meta.mask = sv_cache_info.gal_used_mask;
            sv_meta.systemId = SYSTEM_ID_GALILEO;
            if (GNSS_SIGNAL_GALILEO_E1 == signalType) {
                sv_meta.svCount = sv_cache_info.gal_e1_count;
            } else if (GNSS_SIGNAL_GALILEO_E5A == signalType) {
                sv_meta.svCount = sv_cache_info.gal_e5_count;
            }
            break;
        case GNSS_SV_TYPE_QZSS:
            sv_meta.talker[0] = 'P';
            sv_meta.talker[1] = 'Q';
            sv_meta.mask = sv_cache_info.qzss_used_mask;
            // QZSS SV ids are from 193-197. So keep svIdOffset 0
            sv_meta.systemId = SYSTEM_ID_QZSS;
            if (GNSS_SIGNAL_QZSS_L1CA == signalType) {
                sv_meta.svCount = sv_cache_info.qzss_l1_count;
            } else if (GNSS_SIGNAL_QZSS_L5 == signalType) {
                sv_meta.svCount = sv_cache_info.qzss_l5_count;
            }
            break;
        case GNSS_SV_TYPE_BEIDOU:
            sv_meta.talker[0] = 'P';
            sv_meta.talker[1] = 'Q';
            sv_meta.mask = sv_cache_info.bds_used_mask;
            // BDS SV ids are from 201-235. So keep svIdOffset 0
            sv_meta.systemId = SYSTEM_ID_BEIDOU;
            if (GNSS_SIGNAL_BEIDOU_B1I == signalType) {
                sv_meta.svCount = sv_cache_info.bds_b1_count;
            } else if (GNSS_SIGNAL_BEIDOU_B2AI == signalType) {
                sv_meta.svCount = sv_cache_info.bds_b2_count;
            }
            break;
        default:
            LOC_LOGE("NMEA Error unknow constellation type: %d", svType);
            return NULL;
    }
    sv_meta.signalId = convert_signalType_to_signalId(signalType);
    if (needCombine &&
                (sv_cache_info.gps_used_mask ? 1 : 0) +
                (sv_cache_info.glo_used_mask ? 1 : 0) +
                (sv_cache_info.gal_used_mask ? 1 : 0) +
                (sv_cache_info.qzss_used_mask ? 1 : 0) +
                (sv_cache_info.bds_used_mask ? 1 : 0) > 1)
    {
        // If GPS, GLONASS, Galileo, QZSS, BDS etc. are combined
        // to obtain the reported position solution,
        // talker shall be set to GN, to indicate that
        // the satellites are used in a combined solution
        sv_meta.talker[0] = 'G';
        sv_meta.talker[1] = 'N';
    }
    return &sv_meta;
}

/*===========================================================================
FUNCTION    loc_nmea_put_checksum

DESCRIPTION
   Generate NMEA sentences generated based on position report

DEPENDENCIES
   NONE

RETURN VALUE
   Total length of the nmea sentence

SIDE EFFECTS
   N/A

===========================================================================*/
static int loc_nmea_put_checksum(char *pNmea, int maxSize)
{
    uint8_t checksum = 0;
    int length = 0;
    if(NULL == pNmea)
        return 0;

    pNmea++; //skip the $
    while (*pNmea != '\0')
    {
        checksum ^= *pNmea++;
        length++;
    }

    // length now contains nmea sentence string length not including $ sign.
    int checksumLength = snprintf(pNmea,(maxSize-length-1),"*%02X\r\n", checksum);

    // total length of nmea sentence is length of nmea sentence inc $ sign plus
    // length of checksum (+1 is to cover the $ character in the length).
    return (length + checksumLength + 1);
}

/*===========================================================================
FUNCTION    loc_nmea_generate_GSA

DESCRIPTION
   Generate NMEA GSA sentences generated based on position report
   Currently below sentences are generated:
   - $GPGSA : GPS DOP and active SVs
   - $GLGSA : GLONASS DOP and active SVs
   - $GAGSA : GALILEO DOP and active SVs
   - $GNGSA : GNSS DOP and active SVs

DEPENDENCIES
   NONE

RETURN VALUE
   Number of SVs used

SIDE EFFECTS
   N/A

===========================================================================*/
static uint32_t loc_nmea_generate_GSA(const GpsLocationExtended &locationExtended,
                              char* sentence,
                              int bufSize,
                              loc_nmea_sv_meta* sv_meta_p,
                              std::vector<std::string> &nmeaArraystr)
{
    if (!sentence || bufSize <= 0 || !sv_meta_p)
    {
        LOC_LOGE("NMEA Error invalid arguments.");
        return 0;
    }

    char* pMarker = sentence;
    int lengthRemaining = bufSize;
    int length = 0;

    uint32_t svUsedCount = 0;
    uint32_t svUsedList[32] = {0};

    char fixType = '\0';

    const char* talker = sv_meta_p->talker;
    uint32_t svIdOffset = sv_meta_p->svIdOffset;
    uint32_t mask = sv_meta_p->mask;

    for (uint8_t i = 1; mask > 0 && svUsedCount < 32; i++)
    {
        if (mask & 1)
            svUsedList[svUsedCount++] = i + svIdOffset;
        mask = mask >> 1;
    }

    if (svUsedCount == 0 && GNSS_SV_TYPE_GPS != sv_meta_p->svType)
        return 0;

    if (svUsedCount == 0)
        fixType = '1'; // no fix
    else if (svUsedCount <= 3)
        fixType = '2'; // 2D fix
    else
        fixType = '3'; // 3D fix

    // Start printing the sentence
    // Format: $--GSA,a,x,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,xx,p.p,h.h,v.v,s*cc
    // a : Mode  : A : Automatic, allowed to automatically switch 2D/3D
    // x : Fixtype : 1 (no fix), 2 (2D fix), 3 (3D fix)
    // xx : 12 SV ID
    // p.p : Position DOP (Dilution of Precision)
    // h.h : Horizontal DOP
    // v.v : Vertical DOP
    // s : GNSS System Id
    // cc : Checksum value
    length = snprintf(pMarker, lengthRemaining, "$%sGSA,A,%c,", talker, fixType);

    if (length < 0 || length >= lengthRemaining)
    {
        LOC_LOGE("NMEA Error in string formatting");
        return 0;
    }
    pMarker += length;
    lengthRemaining -= length;

    // Add first 12 satellite IDs
    for (uint8_t i = 0; i < 12; i++)
    {
        if (i < svUsedCount)
            length = snprintf(pMarker, lengthRemaining, "%02d,", svUsedList[i]);
        else
            length = snprintf(pMarker, lengthRemaining, ",");

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return 0;
        }
        pMarker += length;
        lengthRemaining -= length;
    }

    // Add the position/horizontal/vertical DOP values
    if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DOP)
    {
        length = snprintf(pMarker, lengthRemaining, "%.1f,%.1f,%.1f,",
                locationExtended.pdop,
                locationExtended.hdop,
                locationExtended.vdop);
    }
    else
    {   // no dop
        length = snprintf(pMarker, lengthRemaining, ",,,");
    }
    pMarker += length;
    lengthRemaining -= length;

    // system id
    length = snprintf(pMarker, lengthRemaining, "%d", sv_meta_p->systemId);
    pMarker += length;
    lengthRemaining -= length;

    /* Sentence is ready, add checksum and broadcast */
    length = loc_nmea_put_checksum(sentence, bufSize);
    nmeaArraystr.push_back(sentence);

    return svUsedCount;
}

/*===========================================================================
FUNCTION    loc_nmea_generate_GSV

DESCRIPTION
   Generate NMEA GSV sentences generated based on sv report
   Currently below sentences are generated:
   - $GPGSV: GPS Satellites in View
   - $GLGSV: GLONASS Satellites in View
   - $GAGSV: GALILEO Satellites in View

DEPENDENCIES
   NONE

RETURN VALUE
   NONE

SIDE EFFECTS
   N/A

===========================================================================*/
static void loc_nmea_generate_GSV(const GnssSvNotification &svNotify,
                              char* sentence,
                              int bufSize,
                              loc_nmea_sv_meta* sv_meta_p,
                              std::vector<std::string> &nmeaArraystr)
{
    if (!sentence || bufSize <= 0)
    {
        LOC_LOGE("NMEA Error invalid argument.");
        return;
    }

    char* pMarker = sentence;
    int lengthRemaining = bufSize;
    int length = 0;
    int sentenceCount = 0;
    int sentenceNumber = 1;
    size_t svNumber = 1;

    const char* talker = sv_meta_p->talker;
    uint32_t svIdOffset = sv_meta_p->svIdOffset;
    int svCount = sv_meta_p->svCount;
    if (svCount <= 0)
    {
        // no svs in view, so just send a blank $--GSV sentence
        snprintf(sentence, lengthRemaining, "$%sGSV,1,1,0,%d", talker, sv_meta_p->signalId);
        length = loc_nmea_put_checksum(sentence, bufSize);
        nmeaArraystr.push_back(sentence);
        return;
    }

    svNumber = 1;
    sentenceNumber = 1;
    sentenceCount = svCount / 4 + (svCount % 4 != 0);

    while (sentenceNumber <= sentenceCount)
    {
        pMarker = sentence;
        lengthRemaining = bufSize;

        length = snprintf(pMarker, lengthRemaining, "$%sGSV,%d,%d,%02d",
                talker, sentenceCount, sentenceNumber, svCount);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        for (int i=0; (svNumber <= svNotify.count) && (i < 4);  svNumber++)
        {
            GnssSignalTypeMask signalType = svNotify.gnssSvs[svNumber-1].gnssSignalTypeMask;
            if (0 == signalType) {
                // If no signal type in report, it means default L1,G1,E1,B1I
                switch (svNotify.gnssSvs[svNumber - 1].type)
                {
                    case GNSS_SV_TYPE_GPS:
                        signalType = GNSS_SIGNAL_GPS_L1CA;
                        break;
                    case GNSS_SV_TYPE_GLONASS:
                        signalType = GNSS_SIGNAL_GLONASS_G1;
                        break;
                    case GNSS_SV_TYPE_GALILEO:
                        signalType = GNSS_SIGNAL_GALILEO_E1;
                        break;
                    case GNSS_SV_TYPE_QZSS:
                        signalType = GNSS_SIGNAL_QZSS_L1CA;
                        break;
                    case GNSS_SV_TYPE_BEIDOU:
                        signalType = GNSS_SIGNAL_BEIDOU_B1I;
                        break;
                    default:
                        LOC_LOGE("NMEA Error unknow constellation type: %d",
                                svNotify.gnssSvs[svNumber - 1].type);
                        continue;
                }
            }

            if (sv_meta_p->svType == svNotify.gnssSvs[svNumber - 1].type &&
                    sv_meta_p->signalId == convert_signalType_to_signalId(signalType))
            {
                length = snprintf(pMarker, lengthRemaining,",%02d,%02d,%03d,",
                        svNotify.gnssSvs[svNumber - 1].svId + svIdOffset,
                        (int)(0.5 + svNotify.gnssSvs[svNumber - 1].elevation), //float to int
                        (int)(0.5 + svNotify.gnssSvs[svNumber - 1].azimuth)); //float to int

                if (length < 0 || length >= lengthRemaining)
                {
                    LOC_LOGE("NMEA Error in string formatting");
                    return;
                }
                pMarker += length;
                lengthRemaining -= length;

                if (svNotify.gnssSvs[svNumber - 1].cN0Dbhz > 0)
                {
                    length = snprintf(pMarker, lengthRemaining,"%02d",
                            (int)(0.5 + svNotify.gnssSvs[svNumber - 1].cN0Dbhz)); //float to int

                    if (length < 0 || length >= lengthRemaining)
                    {
                        LOC_LOGE("NMEA Error in string formatting");
                        return;
                    }
                    pMarker += length;
                    lengthRemaining -= length;
                }

                i++;
            }

        }

        // append signalId
        length = snprintf(pMarker, lengthRemaining,",%d",sv_meta_p->signalId);
        pMarker += length;
        lengthRemaining -= length;

        length = loc_nmea_put_checksum(sentence, bufSize);
        nmeaArraystr.push_back(sentence);
        sentenceNumber++;

    }  //while
}

/*===========================================================================
FUNCTION    getUtcTimeWithLeapSecondTransition

DESCRIPTION
   This function returns true if the position report is generated during
   leap second transition period. If not, then the utc timestamp returned
   will be set to the timestamp in the position report. If it is,
   then the utc timestamp returned will need to take into account
   of the leap second transition so that proper calendar year/month/date
   can be calculated from the returned utc timestamp.

DEPENDENCIES
   NONE

RETURN VALUE
   true: position report is generated in leap second transition period.

SIDE EFFECTS
   N/A

===========================================================================*/
bool getUtcTimeWithLeapSecondTransition(const UlpLocation &location,
                                        const GpsLocationExtended &locationExtended,
                                        const LocationSystemInfo &systemInfo,
                                        LocGpsUtcTime &utcPosTimestamp) {
    bool inTransition = false;

    // position report is not generated during leap second transition,
    // we can use the UTC timestamp from position report as is
    utcPosTimestamp = location.gpsLocation.timestamp;

    // Check whether we are in leap second transition.
    // If so, per NMEA spec, we need to display the extra second in format of 23:59:60
    // with year/month/date not getting advanced.
    if ((locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_GPS_TIME) &&
        ((systemInfo.systemInfoMask & LOCATION_SYS_INFO_LEAP_SECOND) &&
         (systemInfo.leapSecondSysInfo.leapSecondInfoMask &
          LEAP_SECOND_SYS_INFO_LEAP_SECOND_CHANGE_BIT))) {

        const LeapSecondChangeInfo  &leapSecondChangeInfo =
            systemInfo.leapSecondSysInfo.leapSecondChangeInfo;
        const GnssSystemTimeStructType &gpsTimestampLsChange =
            leapSecondChangeInfo.gpsTimestampLsChange;

        uint64_t gpsTimeLsChange = gpsTimestampLsChange.systemWeek * MSEC_IN_ONE_WEEK +
                                   gpsTimestampLsChange.systemMsec;
        uint64_t gpsTimePosReport = locationExtended.gpsTime.gpsWeek * MSEC_IN_ONE_WEEK +
                                    locationExtended.gpsTime.gpsTimeOfWeekMs;
        // we are only dealing with positive leap second change, as negative
        // leap second change has never occurred and should not occur in future
        if (leapSecondChangeInfo.leapSecondsAfterChange >
            leapSecondChangeInfo.leapSecondsBeforeChange) {
            // leap second adjustment is always 1 second at a time. It can happen
            // every quarter end and up to four times per year.
            if ((gpsTimePosReport >= gpsTimeLsChange) &&
                (gpsTimePosReport < (gpsTimeLsChange + 1000))) {
                inTransition = true;
                utcPosTimestamp = gpsTimeLsChange + UTC_GPS_OFFSET_MSECS -
                                  leapSecondChangeInfo.leapSecondsBeforeChange * 1000;

                // we substract 1000 milli-seconds from UTC timestmap in order to calculate the
                // proper year, month and date during leap second transtion.
                // Let us give an example, assuming leap second transition is scheduled on 2019,
                // Dec 31st mid night. When leap second transition is happening,
                // instead of outputting the time as 2020, Jan, 1st, 00 hour, 00 min, and 00 sec.
                // The time need to be displayed as 2019, Dec, 31st, 23 hour, 59 min and 60 sec.
                utcPosTimestamp -= 1000;
            }
        }
    }
    return inTransition;
}

/*===========================================================================
FUNCTION    loc_nmea_generate_pos

DESCRIPTION
   Generate NMEA sentences generated based on position report
   Currently below sentences are generated within this function:
   - $GPGSA : GPS DOP and active SVs
   - $GLGSA : GLONASS DOP and active SVs
   - $GAGSA : GALILEO DOP and active SVs
   - $GNGSA : GNSS DOP and active SVs
   - $--VTG : Track made good and ground speed
   - $--RMC : Recommended minimum navigation information
   - $--GGA : Time, position and fix related data

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_nmea_generate_pos(const UlpLocation &location,
                               const GpsLocationExtended &locationExtended,
                               const LocationSystemInfo &systemInfo,
                               unsigned char generate_nmea,
                               std::vector<std::string> &nmeaArraystr)
{
    ENTRY_LOG();

    LocGpsUtcTime utcPosTimestamp = 0;
    bool inLsTransition = false;

    inLsTransition = getUtcTimeWithLeapSecondTransition
                    (location, locationExtended, systemInfo, utcPosTimestamp);

    time_t utcTime(utcPosTimestamp/1000);
    tm * pTm = gmtime(&utcTime);
    if (NULL == pTm) {
        LOC_LOGE("gmtime failed");
        return;
    }

    char sentence[NMEA_SENTENCE_MAX_LENGTH] = {0};
    char* pMarker = sentence;
    int lengthRemaining = sizeof(sentence);
    int length = 0;
    int utcYear = pTm->tm_year % 100; // 2 digit year
    int utcMonth = pTm->tm_mon + 1; // tm_mon starts at zero
    int utcDay = pTm->tm_mday;
    int utcHours = pTm->tm_hour;
    int utcMinutes = pTm->tm_min;
    int utcSeconds = pTm->tm_sec;
    int utcMSeconds = (location.gpsLocation.timestamp)%1000;

    if (inLsTransition) {
        // During leap second transition, we need to display the extra
        // leap second of hour, minute, second as (23:59:60)
        utcHours = 23;
        utcMinutes = 59;
        utcSeconds = 60;
        // As UTC timestamp is freezing during leap second transition,
        // retrieve milli-seconds portion from GPS timestamp.
        utcMSeconds = locationExtended.gpsTime.gpsTimeOfWeekMs % 1000;
    }

   loc_sv_cache_info sv_cache_info = {};

    if (GPS_LOCATION_EXTENDED_HAS_GNSS_SV_USED_DATA & locationExtended.flags) {
        sv_cache_info.gps_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.gps_sv_used_ids_mask;
        sv_cache_info.glo_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.glo_sv_used_ids_mask;
        sv_cache_info.gal_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.gal_sv_used_ids_mask;
        sv_cache_info.qzss_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.bds_sv_used_ids_mask;
        sv_cache_info.bds_used_mask =
                (uint32_t)locationExtended.gnss_sv_used_ids.qzss_sv_used_ids_mask;
    }

    if (generate_nmea) {
        char talker[3] = {'G', 'P', '\0'};
        uint32_t svUsedCount = 0;
        uint32_t count = 0;
        loc_nmea_sv_meta sv_meta;
        // -------------------
        // ---$GPGSA/$GNGSA---
        // -------------------

        count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                        loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GPS,
                        GNSS_SIGNAL_GPS_L1CA, true), nmeaArraystr);
        if (count > 0)
        {
            svUsedCount += count;
            talker[0] = sv_meta.talker[0];
            talker[1] = sv_meta.talker[1];
        }

        // -------------------
        // ---$GLGSA/$GNGSA---
        // -------------------

        count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                        loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GLONASS,
                        GNSS_SIGNAL_GLONASS_G1, true), nmeaArraystr);
        if (count > 0)
        {
            svUsedCount += count;
            talker[0] = sv_meta.talker[0];
            talker[1] = sv_meta.talker[1];
        }

        // -------------------
        // ---$GAGSA/$GNGSA---
        // -------------------

        count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                        loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GALILEO,
                        GNSS_SIGNAL_GALILEO_E1, true), nmeaArraystr);
        if (count > 0)
        {
            svUsedCount += count;
            talker[0] = sv_meta.talker[0];
            talker[1] = sv_meta.talker[1];
        }

        // --------------------------
        // ---$PQGSA/$GNGSA (QZSS)---
        // --------------------------

        count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                        loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_QZSS,
                        GNSS_SIGNAL_QZSS_L1CA, false), nmeaArraystr);
        if (count > 0)
        {
            svUsedCount += count;
            // talker should be default "GP". If GPS, GLO etc is used, it should be "GN"
        }

        // ----------------------------
        // ---$PQGSA/$GNGSA (BEIDOU)---
        // ----------------------------
        count = loc_nmea_generate_GSA(locationExtended, sentence, sizeof(sentence),
                        loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_BEIDOU,
                        GNSS_SIGNAL_BEIDOU_B1I, false), nmeaArraystr);
        if (count > 0)
        {
            svUsedCount += count;
            // talker should be default "GP". If GPS, GLO etc is used, it should be "GN"
        }

        // -------------------
        // ------$--VTG-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_BEARING)
        {
            float magTrack = location.gpsLocation.bearing;
            if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_MAG_DEV)
            {
                float magTrack = location.gpsLocation.bearing - locationExtended.magneticDeviation;
                if (magTrack < 0.0)
                    magTrack += 360.0;
                else if (magTrack > 360.0)
                    magTrack -= 360.0;
            }

            length = snprintf(pMarker, lengthRemaining, "$%sVTG,%.1lf,T,%.1lf,M,", talker, location.gpsLocation.bearing, magTrack);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, "$%sVTG,,T,,M,", talker);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_SPEED)
        {
            float speedKnots = location.gpsLocation.speed * (3600.0/1852.0);
            float speedKmPerHour = location.gpsLocation.speed * 3.6;

            length = snprintf(pMarker, lengthRemaining, "%.1lf,N,%.1lf,K,", speedKnots, speedKmPerHour);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",N,,K,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (!(location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG))
            // N means no fix
            length = snprintf(pMarker, lengthRemaining, "%c", 'N');
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            // D means differential
            length = snprintf(pMarker, lengthRemaining, "%c", 'D');
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            // E means estimated (dead reckoning)
            length = snprintf(pMarker, lengthRemaining, "%c", 'E');
        else // A means autonomous
            length = snprintf(pMarker, lengthRemaining, "%c", 'A');

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        // -------------------
        // ------$--RMC-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        length = snprintf(pMarker, lengthRemaining, "$%sRMC,%02d%02d%02d.%02d,A," ,
                          talker, utcHours, utcMinutes, utcSeconds,utcMSeconds/10);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG)
        {
            double latitude = location.gpsLocation.latitude;
            double longitude = location.gpsLocation.longitude;
            char latHemisphere;
            char lonHemisphere;
            double latMinutes;
            double lonMinutes;

            if (latitude > 0)
            {
                latHemisphere = 'N';
            }
            else
            {
                latHemisphere = 'S';
                latitude *= -1.0;
            }

            if (longitude < 0)
            {
                lonHemisphere = 'W';
                longitude *= -1.0;
            }
            else
            {
                lonHemisphere = 'E';
            }

            latMinutes = fmod(latitude * 60.0 , 60.0);
            lonMinutes = fmod(longitude * 60.0 , 60.0);

            length = snprintf(pMarker, lengthRemaining, "%02d%09.6lf,%c,%03d%09.6lf,%c,",
                              (uint8_t)floor(latitude), latMinutes, latHemisphere,
                              (uint8_t)floor(longitude),lonMinutes, lonHemisphere);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_SPEED)
        {
            float speedKnots = location.gpsLocation.speed * (3600.0/1852.0);
            length = snprintf(pMarker, lengthRemaining, "%.1lf,", speedKnots);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_BEARING)
        {
            length = snprintf(pMarker, lengthRemaining, "%.1lf,", location.gpsLocation.bearing);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        length = snprintf(pMarker, lengthRemaining, "%2.2d%2.2d%2.2d,",
                          utcDay, utcMonth, utcYear);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_MAG_DEV)
        {
            float magneticVariation = locationExtended.magneticDeviation;
            char direction;
            if (magneticVariation < 0.0)
            {
                direction = 'W';
                magneticVariation *= -1.0;
            }
            else
            {
                direction = 'E';
            }

            length = snprintf(pMarker, lengthRemaining, "%.1lf,%c,",
                              magneticVariation, direction);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining, ",,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (!(location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG))
            // N means no fix
            length = snprintf(pMarker, lengthRemaining, "%c", 'N');
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            // D means differential
            length = snprintf(pMarker, lengthRemaining, "%c", 'D');
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            // E means estimated (dead reckoning)
            length = snprintf(pMarker, lengthRemaining, "%c", 'E');
        else  // A means autonomous
            length = snprintf(pMarker, lengthRemaining, "%c", 'A');

        pMarker += length;
        lengthRemaining -= length;

        // hardcode Navigation Status field to 'V'
        length = snprintf(pMarker, lengthRemaining, ",%c", 'V');
        pMarker += length;
        lengthRemaining -= length;

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        // -------------------
        // ------$--GGA-------
        // -------------------

        pMarker = sentence;
        lengthRemaining = sizeof(sentence);

        length = snprintf(pMarker, lengthRemaining, "$%sGGA,%02d%02d%02d.%02d," ,
                          talker, utcHours, utcMinutes, utcSeconds, utcMSeconds/10);

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG)
        {
            double latitude = location.gpsLocation.latitude;
            double longitude = location.gpsLocation.longitude;
            char latHemisphere;
            char lonHemisphere;
            double latMinutes;
            double lonMinutes;

            if (latitude > 0)
            {
                latHemisphere = 'N';
            }
            else
            {
                latHemisphere = 'S';
                latitude *= -1.0;
            }

            if (longitude < 0)
            {
                lonHemisphere = 'W';
                longitude *= -1.0;
            }
            else
            {
                lonHemisphere = 'E';
            }

            latMinutes = fmod(latitude * 60.0 , 60.0);
            lonMinutes = fmod(longitude * 60.0 , 60.0);

            length = snprintf(pMarker, lengthRemaining, "%02d%09.6lf,%c,%03d%09.6lf,%c,",
                              (uint8_t)floor(latitude), latMinutes, latHemisphere,
                              (uint8_t)floor(longitude),lonMinutes, lonHemisphere);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        char gpsQuality;
        if (!(location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_LAT_LONG))
            gpsQuality = '0'; // 0 means no fix
        else if (LOC_NAV_MASK_SBAS_CORRECTION_IONO & locationExtended.navSolutionMask)
            gpsQuality = '2'; // 2 means DGPS fix
        else if (LOC_POS_TECH_MASK_SENSORS == locationExtended.tech_mask)
            gpsQuality = '6'; // 6 means estimated (dead reckoning)
        else
            gpsQuality = '1'; // 1 means GPS fix

        // Number of satellites in use, 00-12
        if (svUsedCount > MAX_SATELLITES_IN_USE)
            svUsedCount = MAX_SATELLITES_IN_USE;
        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_DOP)
        {
            length = snprintf(pMarker, lengthRemaining, "%c,%02d,%.1f,",
                              gpsQuality, svUsedCount, locationExtended.hdop);
        }
        else
        {   // no hdop
            length = snprintf(pMarker, lengthRemaining, "%c,%02d,,",
                              gpsQuality, svUsedCount);
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL)
        {
            length = snprintf(pMarker, lengthRemaining, "%.1lf,M,",
                              locationExtended.altitudeMeanSeaLevel);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,");
        }

        if (length < 0 || length >= lengthRemaining)
        {
            LOC_LOGE("NMEA Error in string formatting");
            return;
        }
        pMarker += length;
        lengthRemaining -= length;

        if ((location.gpsLocation.flags & LOC_GPS_LOCATION_HAS_ALTITUDE) &&
            (locationExtended.flags & GPS_LOCATION_EXTENDED_HAS_ALTITUDE_MEAN_SEA_LEVEL))
        {
            length = snprintf(pMarker, lengthRemaining, "%.1lf,M,,",
                              location.gpsLocation.altitude - locationExtended.altitudeMeanSeaLevel);
        }
        else
        {
            length = snprintf(pMarker, lengthRemaining,",,,");
        }

        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);
    }
    //Send blank NMEA reports for non-final fixes
    else {
        strlcpy(sentence, "$GPGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GNGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$PQGSA,A,1,,,,,,,,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GPVTG,,T,,M,,N,,K,N", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GPRMC,,V,,,,,,,,,,N,V", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);

        strlcpy(sentence, "$GPGGA,,,,,,0,,,,,,,,", sizeof(sentence));
        length = loc_nmea_put_checksum(sentence, sizeof(sentence));
        nmeaArraystr.push_back(sentence);
    }

    EXIT_LOG(%d, 0);
}



/*===========================================================================
FUNCTION    loc_nmea_generate_sv

DESCRIPTION
   Generate NMEA sentences generated based on sv report

DEPENDENCIES
   NONE

RETURN VALUE
   0

SIDE EFFECTS
   N/A

===========================================================================*/
void loc_nmea_generate_sv(const GnssSvNotification &svNotify,
                              std::vector<std::string> &nmeaArraystr)
{
    ENTRY_LOG();

    char sentence[NMEA_SENTENCE_MAX_LENGTH] = {0};
    int svCount = svNotify.count;
    int svNumber = 1;
    loc_sv_cache_info sv_cache_info = {};

    //Count GPS SVs for saparating GPS from GLONASS and throw others
    for(svNumber=1; svNumber <= svCount; svNumber++) {
        if (GNSS_SV_TYPE_GPS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GPGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.gps_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            if (GNSS_SIGNAL_GPS_L5 == svNotify.gnssSvs[svNumber - 1].gnssSignalTypeMask) {
                sv_cache_info.gps_l5_count++;
            } else {
                // GNSS_SIGNAL_GPS_L1CA or default
                // If no signal type in report, it means default L1
                sv_cache_info.gps_l1_count++;
            }
        }
        else if (GNSS_SV_TYPE_GLONASS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GNGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.glo_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            if (GNSS_SIGNAL_GLONASS_G2 == svNotify.gnssSvs[svNumber - 1].gnssSignalTypeMask){
                sv_cache_info.glo_g2_count++;
            } else {
                // GNSS_SIGNAL_GLONASS_G1 or default
                // If no signal type in report, it means default G1
                sv_cache_info.glo_g1_count++;
            }
        }
        else if (GNSS_SV_TYPE_GALILEO == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $GAGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                    (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                      GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.gal_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            if(GNSS_SIGNAL_GALILEO_E5A == svNotify.gnssSvs[svNumber - 1].gnssSignalTypeMask){
                sv_cache_info.gal_e5_count++;
            } else {
                // GNSS_SIGNAL_GALILEO_E1 or default
                // If no signal type in report, it means default E1
                sv_cache_info.gal_e1_count++;
            }
        }
        else if (GNSS_SV_TYPE_QZSS == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $PQGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                  GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.qzss_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            if (GNSS_SIGNAL_QZSS_L5 == svNotify.gnssSvs[svNumber - 1].gnssSignalTypeMask) {
                sv_cache_info.qzss_l5_count++;
            } else {
                // GNSS_SIGNAL_QZSS_L1CA or default
                // If no signal type in report, it means default L1
                sv_cache_info.qzss_l1_count++;
            }
        }
        else if (GNSS_SV_TYPE_BEIDOU == svNotify.gnssSvs[svNumber - 1].type)
        {
            // cache the used in fix mask, as it will be needed to send $PQGSA
            // during the position report
            if (GNSS_SV_OPTIONS_USED_IN_FIX_BIT ==
                (svNotify.gnssSvs[svNumber - 1].gnssSvOptionsMask &
                  GNSS_SV_OPTIONS_USED_IN_FIX_BIT))
            {
                sv_cache_info.bds_used_mask |= (1 << (svNotify.gnssSvs[svNumber - 1].svId - 1));
            }
            if(GNSS_SIGNAL_BEIDOU_B2AI == svNotify.gnssSvs[svNumber - 1].gnssSignalTypeMask){
                sv_cache_info.bds_b2_count++;
            } else {
                // GNSS_SIGNAL_BEIDOU_B1I or default
                // If no signal type in report, it means default B1I
                sv_cache_info.bds_b1_count++;
            }
        }
    }

    loc_nmea_sv_meta sv_meta;
    // ---------------------
    // ------$GPGSV:L1CA----
    // ---------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GPS,
            GNSS_SIGNAL_GPS_L1CA, false), nmeaArraystr);

    // ---------------------
    // ------$GPGSV:L5------
    // ---------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GPS,
            GNSS_SIGNAL_GPS_L5, false), nmeaArraystr);
    // ---------------------
    // ------$GLGSV:G1------
    // ---------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GLONASS,
            GNSS_SIGNAL_GLONASS_G1, false), nmeaArraystr);

    // ---------------------
    // ------$GLGSV:G2------
    // ---------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GLONASS,
            GNSS_SIGNAL_GLONASS_G2, false), nmeaArraystr);

    // ---------------------
    // ------$GAGSV:E1------
    // ---------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GALILEO,
            GNSS_SIGNAL_GALILEO_E1, false), nmeaArraystr);

    // -------------------------
    // ------$GAGSV:E5A---------
    // -------------------------
    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_GALILEO,
            GNSS_SIGNAL_GALILEO_E5A, false), nmeaArraystr);

    // -----------------------------
    // ------$PQGSV (QZSS):L1CA-----
    // -----------------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_QZSS,
            GNSS_SIGNAL_QZSS_L1CA, false), nmeaArraystr);

    // -----------------------------
    // ------$PQGSV (QZSS):L5-------
    // -----------------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_QZSS,
            GNSS_SIGNAL_QZSS_L5, false), nmeaArraystr);
    // -----------------------------
    // ------$PQGSV (BEIDOU:B1I)----
    // -----------------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_BEIDOU,
            GNSS_SIGNAL_BEIDOU_B1I,false), nmeaArraystr);

    // -----------------------------
    // ------$PQGSV (BEIDOU:B2AI)---
    // -----------------------------

    loc_nmea_generate_GSV(svNotify, sentence, sizeof(sentence),
            loc_nmea_sv_meta_init(sv_meta, sv_cache_info, GNSS_SV_TYPE_BEIDOU,
            GNSS_SIGNAL_BEIDOU_B2AI,false), nmeaArraystr);

    EXIT_LOG(%d, 0);
}
