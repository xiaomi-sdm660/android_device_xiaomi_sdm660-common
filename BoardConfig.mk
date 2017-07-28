# config.mk
#
# Product-specific compile-time definitions.
#

TARGET_BOARD_PLATFORM := sdm660
TARGET_BOARD_SUFFIX := _64
TARGET_BOOTLOADER_BOARD_NAME :=sdm660

TARGET_ARCH := arm64
TARGET_ARCH_VARIANT := armv8-a
TARGET_CPU_ABI := arm64-v8a
TARGET_CPU_ABI2 :=
TARGET_CPU_VARIANT := generic

TARGET_2ND_ARCH := arm
TARGET_2ND_ARCH_VARIANT := armv7-a-neon
TARGET_2ND_CPU_ABI := armeabi-v7a
TARGET_2ND_CPU_ABI2 := armeabi
TARGET_2ND_CPU_VARIANT := cortex-a53

TARGET_NO_BOOTLOADER := false
TARGET_USES_UEFI := true
TARGET_NO_KERNEL := false

-include $(QCPATH)/common/sdm660_64/BoardConfigVendor.mk

# Some framework code requires this to enable BT
BOARD_HAVE_BLUETOOTH := true
BOARD_USES_WIPOWER := true
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := device/qcom/common

USE_OPENGL_RENDERER := true
BOARD_USE_LEGACY_UI := true

TARGET_USERIMAGES_USE_EXT4 := true
BOARD_BOOTIMAGE_PARTITION_SIZE := 0x04000000

#A/B related defines
AB_OTA_UPDATER := true
# Full A/B partiton update set
# AB_OTA_PARTITIONS := xbl rpm tz hyp pmic modem abl boot keymaster cmnlib cmnlib64 system bluetooth
# Subset A/B partitions for Android-only image update
AB_OTA_PARTITIONS ?= boot system
BOARD_BUILD_SYSTEM_ROOT_IMAGE := true
TARGET_NO_RECOVERY := true
BOARD_USES_RECOVERY_AS_BOOT := true
ifeq ($(ENABLE_VENDOR_IMAGE), true)
TARGET_RECOVERY_FSTAB := device/qcom/sdm660_64/recovery_vendor_variant.fstab
else
TARGET_RECOVERY_FSTAB := device/qcom/sdm660_64/recovery.fstab
endif

ifneq ($(AB_OTA_UPDATER),true)
   TARGET_RECOVERY_UPDATER_LIBS += librecovery_updater_msm
endif

BOARD_SYSTEMIMAGE_PARTITION_SIZE := 3221225472
BOARD_USERDATAIMAGE_PARTITION_SIZE := 48318382080
BOARD_PERSISTIMAGE_PARTITION_SIZE := 33554432
BOARD_PERSISTIMAGE_FILE_SYSTEM_TYPE := ext4
BOARD_FLASH_BLOCK_SIZE := 131072 # (BOARD_KERNEL_PAGESIZE * 64)

ifeq ($(ENABLE_VENDOR_IMAGE), true)
BOARD_VENDORIMAGE_PARTITION_SIZE := 838860800
BOARD_VENDORIMAGE_FILE_SYSTEM_TYPE := ext4
TARGET_COPY_OUT_VENDOR := vendor
BOARD_PROPERTY_OVERRIDES_SPLIT_ENABLED := true
endif

# Enable suspend during charger mode
BOARD_CHARGER_ENABLE_SUSPEND := true

TARGET_USES_ION := true
TARGET_USES_NEW_ION_API :=true
TARGET_USES_QCOM_DISPLAY_BSP := true

#Gralloc h/w specif flags
TARGET_USES_HWC2 := true
TARGET_USES_GRALLOC1 := true

ifeq ($(BOARD_KERNEL_CMDLINE),)
ifeq ($(TARGET_KERNEL_VERSION),4.4)
     BOARD_KERNEL_CMDLINE += console=ttyMSM0,115200,n8 androidboot.console=ttyMSM0 earlycon=msm_serial_dm,0xc170000
else
     BOARD_KERNEL_CMDLINE += console=ttyHSL0,115200,n8 androidboot.console=ttyHSL0 earlycon=msm_hsl_uart,0xc1b0000
endif
BOARD_KERNEL_CMDLINE += androidboot.hardware=qcom user_debug=31 msm_rtb.filter=0x37 ehci-hcd.park=3 lpm_levels.sleep_disabled=1 sched_enable_hmp=1 sched_enable_power_aware=1 service_locator.enable=1 swiotlb=1 androidboot.configfs=true androidboot.usbcontroller=a800000.dwc3
endif

BOARD_EGL_CFG := device/qcom/sdm660_64/egl.cfg
BOARD_SECCOMP_POLICY := device/qcom/sdm660_32/seccomp

BOARD_KERNEL_BASE        := 0x00000000
BOARD_KERNEL_PAGESIZE    := 4096
BOARD_KERNEL_TAGS_OFFSET := 0x01E00000
BOARD_RAMDISK_OFFSET     := 0x02000000

TARGET_KERNEL_ARCH := arm64
TARGET_KERNEL_HEADER_ARCH := arm64
TARGET_KERNEL_CROSS_COMPILE_PREFIX := aarch64-linux-android-
TARGET_USES_UNCOMPRESSED_KERNEL := false

MAX_EGL_CACHE_KEY_SIZE := 12*1024
MAX_EGL_CACHE_SIZE := 2048*1024

TARGET_FORCE_HWC_FOR_VIRTUAL_DISPLAYS := true
MAX_VIRTUAL_DISPLAY_DIMENSION := 4096

BOARD_USES_GENERIC_AUDIO := true
USE_CAMERA_STUB := false
BOARD_QTI_CAMERA_32BIT_ONLY := true
TARGET_NO_RPC := true

TARGET_PLATFORM_DEVICE_BASE := /devices/soc.0/
TARGET_INIT_VENDOR_LIB := libinit_msm

NUM_FRAMEBUFFER_SURFACE_BUFFERS := 3
TARGET_KERNEL_APPEND_DTB := true
TARGET_COMPILE_WITH_MSM_KERNEL := true

#Enable PD locater/notifier
TARGET_PD_SERVICE_ENABLED := true

#Enable HW based full disk encryption
TARGET_HW_DISK_ENCRYPTION := true

TARGET_CRYPTFS_HW_PATH := device/qcom/common/cryptfs_hw

# Enable dex pre-opt to speed up initial boot
ifeq ($(HOST_OS),linux)
    ifeq ($(WITH_DEXPREOPT),)
      WITH_DEXPREOPT := true
      WITH_DEXPREOPT_PIC := true
      ifneq ($(TARGET_BUILD_VARIANT),user)
        # Retain classes.dex in APK's for non-user builds
        DEX_PREOPT_DEFAULT := nostripping
      endif
    endif
endif

#Enable peripheral manager
TARGET_PER_MGR_ENABLED := true

#Enable SSC Feature
TARGET_USES_SSC := true

# Enable sensor multi HAL
USE_SENSOR_MULTI_HAL := true

#Enable CPUSets
ENABLE_CPUSETS := true
ENABLE_SCHEDBOOST := true

#Enabling IMS Feature
TARGET_USES_IMS := true

#Add NON-HLOS files for ota upgrade
ADD_RADIO_FILES := true
TARGET_RECOVERY_UI_LIB := librecovery_ui_msm


