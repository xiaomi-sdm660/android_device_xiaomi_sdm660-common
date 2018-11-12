LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

# files that live under device/qcom/common/rootdir/etc/

include $(CLEAR_VARS)
LOCAL_MODULE		:= fstab.qcom
LOCAL_MODULE_TAGS	:= optional eng
LOCAL_MODULE_CLASS	:= ETC
ifeq ($(AB_OTA_UPDATER), true)
LOCAL_SRC_FILES		:= etc/fstab_AB.qcom
else
LOCAL_SRC_FILES		:= etc/fstab.qcom
endif
LOCAL_MODULE_PATH	:= $(TARGET_OUT_VENDOR_ETC)
include $(BUILD_PREBUILT)

# Offmode charging
include $(CLEAR_VARS)
LOCAL_MODULE          := chargeonlymode
LOCAL_MODULE_OWNER    := xiaomi
LOCAL_MODULE_PATH     := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
LOCAL_SRC_FILES       := sbin/chargeonlymode
LOCAL_MODULE_TAGS     := optional
LOCAL_MODULE_CLASS    := EXECUTABLES
include $(BUILD_PREBUILT)
