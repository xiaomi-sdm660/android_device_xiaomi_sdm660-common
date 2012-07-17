#Compile the old library version from Google push for Passion builds with old modem image
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION),50001)
LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE := gps.$(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE)

LOCAL_MODULE_TAGS := optional

LOCAL_STATIC_LIBRARIES:= \
    libloc_api-rpc 

LOCAL_SHARED_LIBRARIES := \
    librpc \
    libutils \
    libcutils

LOCAL_SRC_FILES += \
    loc_eng.cpp \
    loc_eng_ioctl.cpp \
    loc_eng_xtra.cpp \
    loc_eng_ni.cpp \
    gps.c

LOCAL_CFLAGS += \
    -fno-short-enums \
    -DAMSS_VERSION=$(BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION)

LOCAL_C_INCLUDES:= \
	$(TARGET_OUT_HEADERS)/libloc_api-rpc \
	$(TARGET_OUT_HEADERS)/libloc_api-rpc/inc \
	$(TARGET_OUT_HEADERS)/libcommondefs-rpc/inc \
	$(TARGET_OUT_HEADERS)/librpc


LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

include $(BUILD_SHARED_LIBRARY)
endif #BOARD_VENDOR_QCOM_GPS_LOC_API_AMSS_VERSION = 50001
