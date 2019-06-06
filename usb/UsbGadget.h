/*
 * Copyright (C) 2018 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_HARDWARE_USB_GADGET_V1_0_USBGADGET_H
#define ANDROID_HARDWARE_USB_GADGET_V1_0_USBGADGET_H

#include <android-base/file.h>
#include <android-base/properties.h>
#include <android-base/unique_fd.h>
#include <android/hardware/usb/gadget/1.0/IUsbGadget.h>
#include <hidl/MQDescriptor.h>
#include <hidl/Status.h>
#include <string>
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <thread>
#include <utils/Log.h>
#include <chrono>
#include <condition_variable>
#include <mutex>

namespace android {
namespace hardware {
namespace usb {
namespace gadget {
namespace V1_0 {
namespace implementation {

using ::android::sp;
using ::android::base::GetProperty;
using ::android::base::SetProperty;
using ::android::base::unique_fd;
using ::android::base::WriteStringToFile;
using ::android::hardware::hidl_array;
using ::android::hardware::hidl_memory;
using ::android::hardware::hidl_string;
using ::android::hardware::hidl_vec;
using ::android::hardware::Return;
using ::android::hardware::Void;
using ::std::chrono::steady_clock;
using ::std::lock_guard;
using ::std::move;
using ::std::mutex;
using ::std::string;
using ::std::thread;
using ::std::unique_ptr;
using ::std::vector;
using namespace std::chrono;
using namespace std::chrono_literals;

struct UsbGadget : public IUsbGadget {
  UsbGadget();
  unique_fd mInotifyFd;
  unique_fd mEventFd;
  unique_fd mEpollFd;

  unique_ptr<thread> mMonitor;
  volatile bool mMonitorCreated;
  vector<string> mEndpointList;
  // protects the CV.
  std::mutex mLock;
  std::condition_variable mCv;

  // Makes sure that only one request is processed at a time.
  std::mutex mLockSetCurrentFunction;
  uint64_t mCurrentUsbFunctions;
  bool mCurrentUsbFunctionsApplied;

  Return<void> setCurrentUsbFunctions(uint64_t functions,
                                      const sp<IUsbGadgetCallback>& callback,
                                      uint64_t timeout) override;

  Return<void> getCurrentUsbFunctions(
      const sp<IUsbGadgetCallback>& callback) override;

  private:
  Status tearDownGadget();
  Status setupFunctions(uint64_t functions,
                        const sp<IUsbGadgetCallback>& callback,
                        uint64_t timeout);
};

}  // namespace implementation
}  // namespace V1_0
}  // namespace gadget
}  // namespace usb
}  // namespace hardware
}  // namespace android

#endif  // ANDROID_HARDWARE_USB_V1_2_USBGADGET_H
