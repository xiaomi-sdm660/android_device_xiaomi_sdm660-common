ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
LOCAL_PATH := $(call my-dir)
ifeq ($(GNSS_HIDL_VERSION),1.1)
include $(LOCAL_PATH)/1.1/Android.mk
else
include $(LOCAL_PATH)/1.0/Android.mk
endif

endif #BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
