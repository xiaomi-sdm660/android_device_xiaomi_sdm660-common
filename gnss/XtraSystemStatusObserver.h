/* Copyright (c) 2017, The Linux Foundation. All rights reserved.
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
#ifndef XTRA_SYSTEM_STATUS_OBS_H
#define XTRA_SYSTEM_STATUS_OBS_H

#include <cinttypes>
#include <MsgTask.h>
#ifdef USE_GLIB
#include <LocThread.h>
#endif
#include <LocIpc.h>

using namespace std;
using loc_core::IOsObserver;
using loc_core::IDataItemObserver;
using loc_core::IDataItemCore;
using loc_util::LocIpc;

#ifdef USE_GLIB
// XtraHalListenerSocket class
// listener socket for getting msgs from xtra-daemon in LE for invoking
// LocNetIface functions
// TBD - this will be removed once bidirectional socket changes in
// xtra-daemon will be implemented
class XtraHalListenerSocket: public LocRunnable {
public :
    // constructor & destructor
    XtraHalListenerSocket(IOsObserver* sysStatObs) :
            mThread(NULL),
            mRunning(false) {
        mSystemStatusObsrvr = sysStatObs;
        mRunning = true;
        // create listener socket in a thread
        startListenerThread();
    }
    XtraHalListenerSocket() {}
    inline virtual ~XtraHalListenerSocket() {
        if (mThread) {
            stopListenXtraDaemon();
            delete mThread;
            mThread = NULL;
        }
    }

    // Overrides of LocRunnable methods
    // This method will be repeated called until it returns false; or
    // until thread is stopped.
    virtual bool run();

    // The method to be run before thread loop (conditionally repeatedly)
    // calls run()
    inline virtual void prerun() {}

    // The method to be run after thread loop (conditionally repeatedly)
    // calls run()
    inline virtual void postrun() {}

private:
    IOsObserver*    mSystemStatusObsrvr;
    int mSocketFd;
    LocThread* mThread;
    bool mRunning;
    void startListenerThread();
    void stopListenXtraDaemon();
    void receiveData(const int socketFd);
};
#endif

class XtraSystemStatusObserver : public IDataItemObserver, public LocIpc{
public :
    // constructor & destructor
    inline XtraSystemStatusObserver(IOsObserver* sysStatObs, const MsgTask* msgTask):
#ifdef USE_GLIB
            mHalListenerSocket(sysStatObs),
#endif
            mSystemStatusObsrvr(sysStatObs), mMsgTask(msgTask) {
        subscribe(true);
        startListeningNonBlocking(LOC_IPC_HAL);
    }
    inline XtraSystemStatusObserver() {
        startListeningNonBlocking(LOC_IPC_HAL);
    };
    inline virtual ~XtraSystemStatusObserver() {
        subscribe(false);
        stopListening();
    }

    // IDataItemObserver overrides
    inline virtual void getName(string& name);
    virtual void notify(const list<IDataItemCore*>& dlist);

#ifdef USE_GLIB
    // IFrameworkActionReq functions reqd
    virtual bool connectBackhaul();
    virtual bool disconnectBackhaul();
#endif

    bool updateLockStatus(uint32_t lock);
    bool updateConnectionStatus(bool connected, int32_t type);
    bool updateTac(const string& tac);
    bool updateMccMnc(const string& mccmnc);
    bool updateXtraThrottle(const bool enabled);
    inline const MsgTask* getMsgTask() { return mMsgTask; }
    void subscribe(bool yes);

    void onReceive(const std::string& data) override;

private:
    IOsObserver*    mSystemStatusObsrvr;
    const MsgTask* mMsgTask;
#ifdef USE_GLIB
    // XtraHalListenerSocket class
    // TBD - this will be removed once bidirectional socket changes in
    // xtra-daemon will be implemented
    XtraHalListenerSocket mHalListenerSocket;
#endif

};

#endif
