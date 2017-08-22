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
#define LOG_TAG "LocSvc_XtraSystemStatusObs"

#include <sys/stat.h>
#include <sys/un.h>
#include <errno.h>
#include <ctype.h>
#include <cutils/properties.h>
#include <math.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <loc_log.h>
#include <loc_nmea.h>
#include <SystemStatus.h>
#include <vector>
#include <sstream>
#include <XtraSystemStatusObserver.h>
#include <LocAdapterBase.h>
#include <DataItemId.h>
#include <DataItemsFactoryProxy.h>
#include <DataItemConcreteTypesBase.h>

using namespace loc_core;

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "LocSvc_XSSO"

bool XtraSystemStatusObserver::updateLockStatus(uint32_t lock) {
    stringstream ss;
    ss <<  "gpslock";
    ss << " " << lock;
    return ( send(LOC_IPC_XTRA, ss.str()) );
}

bool XtraSystemStatusObserver::updateConnectionStatus(bool connected, int32_t type) {
    stringstream ss;
    ss <<  "connection";
    ss << " " << (connected ? "1" : "0");
    ss << " " << type;
    return ( send(LOC_IPC_XTRA, ss.str()) );
}
bool XtraSystemStatusObserver::updateTac(const string& tac) {
    stringstream ss;
    ss <<  "tac";
    ss << " " << tac.c_str();
    return ( send(LOC_IPC_XTRA, ss.str()) );
}

bool XtraSystemStatusObserver::updateMccMnc(const string& mccmnc) {
    stringstream ss;
    ss <<  "mncmcc";
    ss << " " << mccmnc.c_str();
    return ( send(LOC_IPC_XTRA, ss.str()) );
}

bool XtraSystemStatusObserver::updateXtraThrottle(const bool enabled) {
    stringstream ss;
    ss <<  "xtrathrottle";
    ss << " " << (enabled ? 1 : 0);
    return ( send(LOC_IPC_XTRA, ss.str()) );
}

void XtraSystemStatusObserver::onReceive(const std::string& data) {
    if (!strncmp(data.c_str(), "ping", sizeof("ping") - 1)) {
        LOC_LOGd("ping received");

    } else {
        LOC_LOGw("unknown event: %s", data.c_str());
    }
}

void XtraSystemStatusObserver::subscribe(bool yes)
{
    // Subscription data list
    list<DataItemId> subItemIdList;
    subItemIdList.push_back(NETWORKINFO_DATA_ITEM_ID);
    subItemIdList.push_back(MCCMNC_DATA_ITEM_ID);

    if (yes) {
        mSystemStatusObsrvr->subscribe(subItemIdList, this);

        list<DataItemId> reqItemIdList;
        reqItemIdList.push_back(TAC_DATA_ITEM_ID);

        mSystemStatusObsrvr->requestData(reqItemIdList, this);

    } else {
        mSystemStatusObsrvr->unsubscribe(subItemIdList, this);
    }
}

// IDataItemObserver overrides
void XtraSystemStatusObserver::getName(string& name)
{
    name = "XtraSystemStatusObserver";
}

void XtraSystemStatusObserver::notify(const list<IDataItemCore*>& dlist)
{
    struct HandleOsObserverUpdateMsg : public LocMsg {
        XtraSystemStatusObserver* mXtraSysStatObj;
        list <IDataItemCore*> mDataItemList;

        inline HandleOsObserverUpdateMsg(XtraSystemStatusObserver* xtraSysStatObs,
                const list<IDataItemCore*>& dataItemList) :
                mXtraSysStatObj(xtraSysStatObs) {
            for (auto eachItem : dataItemList) {
                IDataItemCore* dataitem = DataItemsFactoryProxy::createNewDataItem(
                        eachItem->getId());
                if (NULL == dataitem) {
                    break;
                }
                // Copy the contents of the data item
                dataitem->copy(eachItem);

                mDataItemList.push_back(dataitem);
            }
        }

        inline ~HandleOsObserverUpdateMsg() {
            for (auto each : mDataItemList) {
                delete each;
            }
        }

        inline void proc() const {
            for (auto each : mDataItemList) {
                switch (each->getId())
                {
                    case NETWORKINFO_DATA_ITEM_ID:
                    {
                        NetworkInfoDataItemBase* networkInfo =
                                static_cast<NetworkInfoDataItemBase*>(each);
                        mXtraSysStatObj->updateConnectionStatus(networkInfo->mConnected,
                                networkInfo->mType);
                    }
                    break;

                    case TAC_DATA_ITEM_ID:
                    {
                        TacDataItemBase* tac =
                                 static_cast<TacDataItemBase*>(each);
                        mXtraSysStatObj->updateTac(tac->mValue);
                    }
                    break;

                    case MCCMNC_DATA_ITEM_ID:
                    {
                        MccmncDataItemBase* mccmnc =
                                static_cast<MccmncDataItemBase*>(each);
                        mXtraSysStatObj->updateMccMnc(mccmnc->mValue);
                    }
                    break;

                    default:
                    break;
                }
            }
        }
    };
    mMsgTask->sendMsg(new (nothrow) HandleOsObserverUpdateMsg(this, dlist));
}

#ifdef USE_GLIB
bool XtraSystemStatusObserver::connectBackhaul()
{
    return mSystemStatusObsrvr->connectBackhaul();
}

bool XtraSystemStatusObserver::disconnectBackhaul()
{
    return mSystemStatusObsrvr->disconnectBackhaul();
}

// XtraHalListenerSocket class
// TBD - this will be removed once bidirectional socket changes in
// xtra-daemon will be implemented
void XtraHalListenerSocket::receiveData(const int socketFd) {
    string data;
    array<char, 128> buf;
    const char* bin_msg_conn_backhaul = "connectBackhaul";
    const char* bin_msg_disconn_backhaul = "disconnectBackhaul";

    while (true) {
        ssize_t len = recv(socketFd, buf.data(), buf.size(), 0);
        if (len > 0) {
            LOC_LOGd("received %lu bytes", len);
            data.append(buf.data(), len);

            size_t pos = data.find("\n");
            if (pos == string::npos) {
                continue;
            }

            if (!strncmp(data.c_str(), bin_msg_conn_backhaul,
                                       sizeof(bin_msg_conn_backhaul) - 1)) {
                mSystemStatusObsrvr->connectBackhaul();
            } else if (!strncmp(data.c_str(), bin_msg_disconn_backhaul,
                                            sizeof(bin_msg_disconn_backhaul) - 1)) {
                mSystemStatusObsrvr->disconnectBackhaul();
            }
            else {
                LOC_LOGw("unknown event: %s", data.c_str());
            }
            break;

        } else {
            LOC_LOGd("XtraHalListenerSocket connection broken.");
            break;
        }
    }
}

void XtraHalListenerSocket::startListenerThread() {
    mThread = new (std::nothrow) LocThread();
    if (!mThread) {
        LOC_LOGe("create thread failed");
    }
    mRunning = true;
    if (!mThread->start("XtraHalListenerSocketThread", this, true)) {
        delete mThread;
        mThread = NULL;
    }

    LOC_LOGd("Create listener socket in XtraHalListenerSocket");
    // create socket
    int socketFd;
    if ((socketFd = ::socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        LOC_LOGe("create socket error. reason:%s", strerror(errno));
        return;
    }

    const char* socketPath = "/data/vendor/location/xtra/socket_xtra_locnetiface";
    unlink(socketPath);

    struct sockaddr_un addr = { .sun_family = AF_UNIX };
    snprintf(addr.sun_path, sizeof(addr.sun_path), "%s", socketPath);

    umask(0157);

    if (::bind(socketFd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        LOC_LOGe("bind socket error. reason:%s", strerror(errno));
        return;
    }

    // set up connection
    if (::listen(socketFd, 5/*backlog*/) < 0) {
        LOC_LOGe("cannot bind socket. reason:%s", strerror(errno));
        return;
    }
    mSocketFd = socketFd;

}

bool XtraHalListenerSocket::run() {
    // infinite while loop till mRunning is false when stopListenXtraDaemon
    // is called
    while (mRunning) {
        int clientFd = -1;
        LOC_LOGd("XtraHalListenerSocket - waiting for msg...");
        if ( (clientFd = ::accept(mSocketFd, NULL, NULL)) < 0) {
            LOC_LOGe("connection error. reason:%s", strerror(errno));
        } else {
            LOC_LOGd("XtraHalListenerSocket - receiving data ...");
            receiveData(clientFd);
            if (::close(clientFd)) {
                LOC_LOGe("close connection fail.");
            }
            clientFd = -1;
        }
    }

    // return false once we reach there
    return false;
}

void XtraHalListenerSocket::stopListenXtraDaemon() {
    if (mSocketFd >= 0) {
        if (::close(mSocketFd)) {
            LOC_LOGe("close hal connection fail.");
        }
        mSocketFd = -1;
    }
    mRunning = false;
    mThread->stop();
}

#endif
