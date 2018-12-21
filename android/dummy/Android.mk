LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

#build dummy android.hardware.gnss@1.0-impl-qti
include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.gnss@1.0-impl-qti
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_SRC_FILES := GnssDummy.cpp

LOCAL_SHARED_LIBRARIES := \
    liblog \
    libhidlbase \
    libhidltransport \
    libhwbinder \
    libcutils \
    libutils \
    android.hardware.gnss@1.0 \

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_SHARED_LIBRARY)

BUILD_GNSS_HIDL_SERVICE := true
ifneq ($(BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET), true)
ifneq ($(LW_FEATURE_SET),true)
#BUILD_GNSS_HIDL_SERVICE := false
endif # LW_FEATURE_SET
endif # BOARD_VENDOR_QCOM_LOC_PDK_FEATURE_SET

ifeq ($(BUILD_GNSS_HIDL_SERVICE), true)
include $(CLEAR_VARS)
LOCAL_MODULE := android.hardware.gnss@1.0-service-qti
LOCAL_VENDOR_MODULE := true
LOCAL_MODULE_RELATIVE_PATH := hw
LOCAL_INIT_RC := android.hardware.gnss@1.0-service-qti.rc
LOCAL_SRC_FILES := \
    serviceDummy.cpp \

#LOCAL_C_INCLUDES:= \
    $(LOCAL_PATH)/location_api
#LOCAL_HEADER_LIBRARIES := \
    libgps.utils_headers \
    libloc_core_headers \
    libloc_pla_headers \
    liblocation_api_headers


LOCAL_SHARED_LIBRARIES := \
    liblog \
    libcutils \
    libdl \
    libbase \
    libutils \

LOCAL_SHARED_LIBRARIES += \
    libhwbinder \
    libhidlbase \
    libhidltransport \
    android.hardware.gnss@1.0 \

LOCAL_CFLAGS += $(GNSS_CFLAGS)
include $(BUILD_EXECUTABLE)
endif # BUILD_GNSS_HIDL_SERVICE
