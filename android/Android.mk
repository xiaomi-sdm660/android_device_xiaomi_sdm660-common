LOCAL_PATH := $(call my-dir)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifeq ($(GNSS_HIDL_VERSION),1.1)
include $(LOCAL_PATH)/1.1/Android.mk
else
include $(LOCAL_PATH)/1.0/Android.mk
endif
else #QMAA flag set, build dummy android.hardware.gnss@1.0-impl-qti
include $(LOCAL_PATH)/dummy/Android.mk
endif #BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
