LOCAL_PATH := $(call my-dir)
include $(LOCAL_PATH)/build/target_specific_features.mk
ifneq ($(BOARD_VENDOR_QCOM_GPS_LOC_API_HARDWARE),)
include $(call all-makefiles-under,$(LOCAL_PATH))
else
include $(LOCAL_PATH)/android/Android.mk
endif
