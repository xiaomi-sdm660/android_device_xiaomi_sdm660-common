>#
# Copyright (C) 2018 The Xiaomi-SDM660 Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

VNDK_SP_LIBRARIES := \
    android.hardware.graphics.allocator@2.0 \
    android.hardware.graphics.mapper@2.0 \
    android.hardware.graphics.mapper@2.1 \
    android.hardware.graphics.common@1.0 \
    android.hardware.graphics.common@1.1 \
    android.hardware.renderscript@1.0 \
    android.hidl.base@1.0 \
    android.hidl.memory@1.0 \
    libRSCpuRef \
    libRSDriver \
    libRS_internal \
    libbacktrace \
    libbase \
    libbcinfo \
    libblas \
    libc++ \
    libcompiler_rt \
    libcutils \
    libft2 \
    libhardware \
    libhidlbase \
    libhidlmemory \
    libhidltransport \
    libhwbinder \
    libion \
    liblzma \
    libpng \
    libunwind \
    libunwindstack\
    libutils \
    libutilscallstack \
    libdexfile

EXTRA_VENDOR_LIBRARIES := \
    android.hidl.base@1.0 \
    vendor.display.config@1.0 \
    vendor.display.config@1.1 \
    vendor.qti.hardware.camera.device@1.0
