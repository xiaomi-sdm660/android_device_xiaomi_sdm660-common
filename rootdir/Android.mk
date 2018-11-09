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
include $(BUILD_PREBUILT)
