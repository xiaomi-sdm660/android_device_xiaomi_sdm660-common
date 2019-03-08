LOCAL_PATH := $(call my-dir)
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
ifeq ($(GNSS_HIDL_VERSION),2.0)
include $(LOCAL_PATH)/2.0/Android.mk
else
ifeq ($(GNSS_HIDL_VERSION),1.1)
include $(LOCAL_PATH)/1.1/Android.mk
else
include $(LOCAL_PATH)/1.0/Android.mk
endif #GNSS HIDL 1.1
endif #GNSS HIDL 2.0
else #QMAA flag set, build dummy android.hardware.gnss@1.0-impl-qti
include $(LOCAL_PATH)/dummy/Android.mk
endif #BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE
