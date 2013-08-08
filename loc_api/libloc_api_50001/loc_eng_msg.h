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
#ifndef LOC_ENG_MSG_H
#define LOC_ENG_MSG_H


#include <hardware/gps.h>
#include <gps_extended.h>
#include <stdlib.h>
#include <string.h>
#include <log_util.h>
#include <loc_eng_log.h>
#include <loc_eng.h>
#include <loc_eng_msg_id.h>
#include <MsgTask.h>
#include <LocEngAdapter.h>

#ifndef SSID_BUF_SIZE
    #define SSID_BUF_SIZE (32+1)
#endif
#ifdef USE_GLIB

#include <glib.h>

#endif /* USE_GLIB */
#include "platform_lib_includes.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

using namespace loc_core;

struct loc_eng_msg {
    const void* owner;
    const int msgid;
    inline loc_eng_msg(void* instance, int id) :
        owner(instance), msgid(id)
    {
        LOC_LOGV("creating msg %s", loc_get_msg_name(msgid));
        LOC_LOGV("creating msg ox%x", msgid);
    }
    virtual ~loc_eng_msg()
    {
        LOC_LOGV("deleting msg %s (0x%x)", loc_get_msg_name(msgid), msgid);
    }
};

struct loc_eng_msg_position_mode : public loc_eng_msg {
    const LocPosMode pMode;
    inline loc_eng_msg_position_mode(void* instance,
                                     LocPosMode &mode) :
        loc_eng_msg(instance, LOC_ENG_MSG_SET_POSITION_MODE),
        pMode(mode)
    {
        pMode.logv();
    }
};

struct loc_eng_msg_report_position : public loc_eng_msg {
    const UlpLocation location;
    const GpsLocationExtended locationExtended;
    const void* locationExt;
    const enum loc_sess_status status;
    const LocPosTechMask technology_mask;
    inline loc_eng_msg_report_position(void* instance, UlpLocation &loc, GpsLocationExtended &locExtended, void* locExt,
                                       enum loc_sess_status st) :
        loc_eng_msg(instance, LOC_ENG_MSG_REPORT_POSITION),
        location(loc), locationExtended(locExtended), locationExt(locExt), status(st), technology_mask(LOC_POS_TECH_MASK_DEFAULT)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n  altitude: %f\n  speed: %f\n  bearing: %f\n  accuracy: %f\n  timestamp: %lld\n  rawDataSize: %d\n  rawData: %p\n  Session status: %d\n Technology mask: %u",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, location.rawDataSize,
                 location.rawData,status,technology_mask);
    }
    inline loc_eng_msg_report_position(void* instance, UlpLocation &loc, GpsLocationExtended &locExtended, void* locExt,
                                       enum loc_sess_status st, LocPosTechMask technology) :
        loc_eng_msg(instance, LOC_ENG_MSG_REPORT_POSITION),
        location(loc), locationExtended(locExtended), locationExt(locExt), status(st), technology_mask(technology)
    {
        LOC_LOGV("flags: %d\n  source: %d\n  latitude: %f\n  longitude: %f\n  altitude: %f\n  speed: %f\n  bearing: %f\n  accuracy: %f\n  timestamp: %lld\n  rawDataSize: %d\n  rawData: %p\n  Session status: %d\n Technology mask: %u",
                 location.gpsLocation.flags, location.position_source,
                 location.gpsLocation.latitude, location.gpsLocation.longitude,
                 location.gpsLocation.altitude, location.gpsLocation.speed,
                 location.gpsLocation.bearing, location.gpsLocation.accuracy,
                 location.gpsLocation.timestamp, location.rawDataSize,
                 location.rawData,status,technology_mask);
    }
};

struct loc_eng_msg_report_sv : public loc_eng_msg {
    const GpsSvStatus svStatus;
    const GpsLocationExtended locationExtended;
    const void* svExt;
    inline loc_eng_msg_report_sv(void* instance, GpsSvStatus &sv, GpsLocationExtended &locExtended, void* ext) :
        loc_eng_msg(instance, LOC_ENG_MSG_REPORT_SV), svStatus(sv), locationExtended(locExtended), svExt(ext)
    {
        LOC_LOGV("num sv: %d\n  ephemeris mask: %dxn  almanac mask: %x\n  used in fix mask: %x\n      sv: prn         snr       elevation      azimuth",
                 svStatus.num_svs, svStatus.ephemeris_mask, svStatus.almanac_mask, svStatus.used_in_fix_mask);
        for (int i = 0; i < svStatus.num_svs && i < GPS_MAX_SVS; i++) {
            LOC_LOGV("   %d:   %d    %f    %f    %f\n  ",
                     i,
                     svStatus.sv_list[i].prn,
                     svStatus.sv_list[i].snr,
                     svStatus.sv_list[i].elevation,
                     svStatus.sv_list[i].azimuth);
        }
    }
};


struct LocEngPositionMode : public LocMsg {
    LocEngAdapter* mAdapter;
    const LocPosMode mPosMode;
    LocEngPositionMode(LocEngAdapter* adapter,
                       LocPosMode &mode);
    virtual void proc() const;
    virtual void log() const;
    void send() const;
};


struct LocEngStartFix : public LocMsg {
    loc_eng_data_s_type* mLocEng;
    LocEngStartFix(loc_eng_data_s_type* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngStopFix : public LocMsg {
    loc_eng_data_s_type* mLocEng;
    LocEngStopFix(loc_eng_data_s_type* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngReportPosition : public LocMsg {
    void* mLocEng;
    const UlpLocation mLocation;
    const GpsLocationExtended mLocationExtended;
    const void* mLocationExt;
    const enum loc_sess_status mStatus;
    const LocPosTechMask mTechMask;
    LocEngReportPosition(void* locEng,
                         UlpLocation &loc,
                         GpsLocationExtended &locExtended,
                         void* locExt,
                         enum loc_sess_status st,
                         LocPosTechMask technology);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngReportSv : public LocMsg {
    void* mLocEng;
    const GpsSvStatus mSvStatus;
    const GpsLocationExtended mLocationExtended;
    const void* mSvExt;
    LocEngReportSv(void* locEng,
                   GpsSvStatus &sv,
                   GpsLocationExtended &locExtended,
                   void* svExtended);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngReportStatus : public LocMsg {
    void* mLocEng;
    const GpsStatusValue mStatus;
    LocEngReportStatus(void* locEng,
                       GpsStatusValue engineStatus);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngReportNmea : public LocMsg {
    void* mLocEng;
    char* const mNmea;
    const int mLen;
    LocEngReportNmea(void* locEng,
                     const char* data, int len);
    inline virtual ~LocEngReportNmea()
    {
        delete[] mNmea;
    }
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngReportXtraServer : public LocMsg {
    void* mLocEng;
    int mMaxLen;
    char *mServers;
    LocEngReportXtraServer(void* locEng,
                           const char *url1, const char *url2,
                           const char *url3, const int maxlength);
    inline virtual ~LocEngReportXtraServer()
    {
        delete[] mServers;
    }
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngSuplEsOpened : public LocMsg {
    void* mLocEng;
    LocEngSuplEsOpened(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngSuplEsClosed : public LocMsg {
    void* mLocEng;
    LocEngSuplEsClosed(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngRequestSuplEs : public LocMsg {
    void* mLocEng;
    const int mID;
    LocEngRequestSuplEs(void* locEng, int id);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngRequestATL : public LocMsg {
    void* mLocEng;
    const int mID;
    const AGpsExtType mType;
    LocEngRequestATL(void* locEng, int id,
                     AGpsExtType agps_type);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngReleaseATL : public LocMsg {
    void* mLocEng;
    const int mID;
    LocEngReleaseATL(void* locEng, int id);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngReqRelBIT : public LocMsg {
    void* mLocEng;
    const AGpsExtType mType;
    const int mIPv4Addr;
    char* const mIPv6Addr;
    const bool mIsReq;
    LocEngReqRelBIT(void* instance, AGpsExtType type,
                    int ipv4, char* ipv6, bool isReq);
    virtual ~LocEngReqRelBIT();
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngReqRelWifi : public LocMsg {
    void* mLocEng;
    const AGpsExtType mType;
    const loc_if_req_sender_id_e_type mSenderId;
    char* const mSSID;
    char* const mPassword;
    const bool mIsReq;
    LocEngReqRelWifi(void* locEng, AGpsExtType type,
                     loc_if_req_sender_id_e_type sender_id,
                     char* s, char* p, bool isReq);
    virtual ~LocEngReqRelWifi();
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
    void send() const;
};

struct LocEngRequestXtra : public LocMsg {
    void* mLocEng;
    LocEngRequestXtra(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngRequestTime : public LocMsg {
    void* mLocEng;
    LocEngRequestTime(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngRequestNi : public LocMsg {
    void* mLocEng;
    const GpsNiNotification mNotify;
    const void *mPayload;
    LocEngRequestNi(void* locEng,
                    GpsNiNotification &notif,
                    const void* data);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngDown : public LocMsg {
    void* mLocEng;
    LocEngDown(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};

struct LocEngUp : public LocMsg {
    void* mLocEng;
    LocEngUp(void* locEng);
    virtual void proc() const;
    void locallog() const;
    virtual void log() const;
};


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LOC_ENG_MSG_H */
