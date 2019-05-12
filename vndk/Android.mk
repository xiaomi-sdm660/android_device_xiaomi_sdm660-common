LOCAL_PATH := $(call my-dir)

EXTRA_VENDOR_LIBRARIES_32 := \
    libaudioclient \
    libaudiomanager \
    libbinder \
    libcamera_client \
    libft2 \
    libgui.vendor \
    libharfbuzz_ng \
    libheif \
    libicui18n \
    libicuuc \
    libmedia \
    libmediaextractor \
    libmediametrics \
    libminikin \
    libsonivox

define define-vndk-lib
include $$(CLEAR_VARS)
LOCAL_MODULE := $1.vndk-ext-gen
LOCAL_MODULE_CLASS := SHARED_LIBRARIES
LOCAL_PREBUILT_MODULE_FILE := $$(call intermediates-dir-for,SHARED_LIBRARIES,$1,,,$3,)/$1.so
LOCAL_STRIP_MODULE := false
LOCAL_MULTILIB := $2
LOCAL_MODULE_TAGS := optional
LOCAL_INSTALLED_MODULE_STEM := $$(basename $1).so
LOCAL_MODULE_SUFFIX := .so
LOCAL_VENDOR_MODULE := true
include $$(BUILD_PREBUILT)
endef

$(foreach lib,$(EXTRA_VENDOR_LIBRARIES_32),\
    $(eval $(call define-vndk-lib,$(lib),32,$(TARGET_2ND_ARCH_VAR_PREFIX))))

$(foreach lib,$(EXTRA_VENDOR_LIBRARIES_64),\
    $(eval $(call define-vndk-lib,$(lib),first,)))

include $(CLEAR_VARS)
LOCAL_MODULE := vndk-ext
LOCAL_MODULE_TAGS := optional
LOCAL_REQUIRED_MODULES := $(addsuffix .vndk-ext-gen,$(EXTRA_VENDOR_LIBRARIES_32) $(EXTRA_VENDOR_LIBRARIES_64))
include $(BUILD_PHONY_PACKAGE)