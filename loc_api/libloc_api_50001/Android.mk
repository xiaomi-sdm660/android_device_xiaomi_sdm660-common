ifneq ($(BUILD_TINY_ANDROID),true)
#Compile this library only for builds with the latest modem image

BIT_ENABLED_BOARD_PLATFORM_LIST := msm7630_fusion
BIT_ENABLED_BOARD_PLATFORM_LIST += msm8660
ifeq ($(call is-board-platform-in-list,$(BIT_ENABLED_BOARD_PLATFORM_LIST)),true)
FEATURE_GNSS_BIT_API := true
endif # is-board-platform-in-list

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := libloc_adapter

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libgps.utils \
    libdl

LOCAL_SRC_FILES += \
    loc_eng_log.cpp \
    LocApiAdapter.cpp

LOCAL_CFLAGS += \
     -fno-short-enums \
     -D_ANDROID_

LOCAL_C_INCLUDES:= \
    $(TARGET_OUT_HEADERS)/gps.utils

LOCAL_COPY_HEADERS_TO:= libloc_eng/
LOCAL_COPY_HEADERS:= \
   LocApiAdapter.h \
   loc.h \
   loc_eng.h \
   loc_eng_xtra.h \
   loc_eng_ni.h \
   loc_eng_agps.h \
   loc_eng_msg.h \
   loc_eng_msg_id.h \
   loc_eng_log.h

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := libloc_eng

LOCAL_MODULE_TAGS := optional

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libloc_adapter \
    libgps.utils \
    libdl

LOCAL_SRC_FILES += \
    loc_eng.cpp \
    loc_eng_agps.cpp \
    loc_eng_xtra.cpp \
    loc_eng_ni.cpp \
    loc_eng_log.cpp

ifeq ($(FEATURE_GNSS_BIT_API), true)
LOCAL_CFLAGS += -DFEATURE_GNSS_BIT_API
endif # FEATURE_GNSS_BIT_API

LOCAL_SRC_FILES += \
    loc_eng_dmn_conn.cpp \
    loc_eng_dmn_conn_handler.cpp \
    loc_eng_dmn_conn_thread_helper.c \
    loc_eng_dmn_conn_glue_msg.c \
    loc_eng_dmn_conn_glue_pipe.c

LOCAL_CFLAGS += \
     -fno-short-enums \
     -D_ANDROID_

LOCAL_C_INCLUDES:= \
    $(TARGET_OUT_HEADERS)/gps.utils \
    hardware/qcom/gps/loc_api/ulp/inc

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := gps.$(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE)

LOCAL_MODULE_TAGS := optional

## Libs

LOCAL_SHARED_LIBRARIES := \
    libutils \
    libcutils \
    libloc_eng \
    libgps.utils

LOCAL_SRC_FILES += \
    loc.cpp \
    gps.c

LOCAL_CFLAGS += \
    -fno-short-enums \
    -D_ANDROID_ \

## Includes
LOCAL_C_INCLUDES:= \
    $(TARGET_OUT_HEADERS)/gps.utils

LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

include $(BUILD_SHARED_LIBRARY)

endif # not BUILD_TINY_ANDROID
