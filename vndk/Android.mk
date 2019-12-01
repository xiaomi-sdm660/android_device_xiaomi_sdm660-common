#
# Copyright (C) 2019 The LineageOS Project
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

LOCAL_PATH := $(call my-dir)

include $(LOCAL_PATH)/vndk-ext-libs.mk

define define-vndk-lib
include $$(CLEAR_VARS)
LOCAL_MODULE := $1.vndk-ext-gen
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$1,,,$3,)/$1.so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := $2
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $$(basename $1).so
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := true
include $$(BUILD_PREBUILT)
endef

$(foreach lib,$(EXTRA_VENDOR_LIBRARIES_32),\
    $(eval $(call define-vndk-lib,$(lib),32,$(TARGET_2ND_ARCH_VAR_PREFIX))))

$(foreach lib,$(EXTRA_VENDOR_LIBRARIES_64),\
    $(eval $(call define-vndk-lib,$(lib),first,)))

include $(CLEAR_VARS)
LOCAL_MODULE := vndk-ext
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := $(addsuffix .vndk-ext-gen,$(EXTRA_VENDOR_LIBRARIES_32) $(EXTRA_VENDOR_LIBRARIES_64))
include $(BUILD_PHONY_PACKAGE)
