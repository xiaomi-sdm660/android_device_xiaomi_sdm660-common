ifneq ($(BUILD_TINY_ANDROID),true)

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libloc_api_v02

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libqmi_cci \
    libqmi_csi \
    libqmi_common_so \
    libloc_adapter \
    libgps.utils

LOCAL_SRC_FILES += \
    LocApiV02Adapter.cpp \
    loc_api_v02_log.c \
    loc_api_v02_client.c \
    loc_api_sync_req.c \
    location_service_v02.c

LOCAL_CFLAGS += \
    -fno-short-enums \
    -D_ANDROID_

ifeq ($(FEATURE_IPV6), true)
LOCAL_CFLAGS += -DFEATURE_IPV6
endif #FEATURE_IPV6

ifeq ($(FEATURE_DELEXT), true)
LOCAL_CFLAGS += -DFEATURE_DELEXT
endif #FEATURE_DELEXT

## Includes
LOCAL_C_INCLUDES := \
    $(TARGET_OUT_HEADERS)/libloc_eng \
    $(TARGET_OUT_HEADERS)/qmi-framework/inc \
    $(TARGET_OUT_HEADERS)/qmi/inc \
    $(TARGET_OUT_HEADERS)/gps.utils

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID
