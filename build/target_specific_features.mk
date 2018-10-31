GNSS_CFLAGS := \
    -Werror \
    -Wno-error=unused-parameter \
    -Wno-error=format \
    -Wno-error=macro-redefined \
    -Wno-error=reorder \
    -Wno-error=missing-braces \
    -Wno-error=self-assign \
    -Wno-error=enum-conversion \
    -Wno-error=logical-op-parentheses \
    -Wno-error=null-arithmetic \
    -Wno-error=null-conversion \
    -Wno-error=parentheses-equality \
    -Wno-error=undefined-bool-conversion \
    -Wno-error=tautological-compare \
    -Wno-error=switch \
    -Wno-error=date-time

# GPS-HIDL
GNSS_HIDL_1_0_TARGET_LIST := msm8960
GNSS_HIDL_1_0_TARGET_LIST += msm8974
GNSS_HIDL_1_0_TARGET_LIST += msm8226
GNSS_HIDL_1_0_TARGET_LIST += msm8610
GNSS_HIDL_1_0_TARGET_LIST += apq8084
GNSS_HIDL_1_0_TARGET_LIST += msm8916
GNSS_HIDL_1_0_TARGET_LIST += msm8994
GNSS_HIDL_1_0_TARGET_LIST += msm8909
GNSS_HIDL_1_0_TARGET_LIST += msm8952
GNSS_HIDL_1_0_TARGET_LIST += msm8992
GNSS_HIDL_1_0_TARGET_LIST += msm8996
GNSS_HIDL_1_0_TARGET_LIST += msm8937
GNSS_HIDL_1_0_TARGET_LIST += msm8953
GNSS_HIDL_1_0_TARGET_LIST += msm8998
GNSS_HIDL_1_0_TARGET_LIST += apq8098_latv
GNSS_HIDL_1_0_TARGET_LIST += sdm710
GNSS_HIDL_1_0_TARGET_LIST += qcs605
GNSS_HIDL_1_0_TARGET_LIST += sdm845
GNSS_HIDL_1_0_TARGET_LIST += sdm660
GNSS_HIDL_1_1_TARGET_LIST := msmnile
GNSS_HIDL_1_1_TARGET_LIST += $(MSMSTEPPE)
GNSS_HIDL_1_1_TARGET_LIST += $(TRINKET)


ifneq (,$(filter $(GNSS_HIDL_1_0_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
GNSS_HIDL_VERSION = 1.0
endif

ifneq (,$(filter $(GNSS_HIDL_1_1_TARGET_LIST),$(TARGET_BOARD_PLATFORM)))
GNSS_HIDL_VERSION = 1.1
endif
