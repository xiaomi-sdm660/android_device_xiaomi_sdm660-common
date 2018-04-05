# 
# Copyright (C) 2018 The Mokee Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#
# This file sets variables that control the way modules are built
# thorughout the system. It should not be used to conditionally
# disable makefiles (the proper mechanism to control what gets
# included in a build is to use PRODUCT_PACKAGES in a product
# definition file).
#

# Platform Path
PLATFORM_PATH := device/xiaomi/sdm660-common

# Overlays
DEVICE_PACKAGE_OVERLAYS := device/xiaomi/sdm660-common/overlay

# Default vendor configuration.
ifeq ($(ENABLE_VENDOR_IMAGE),)
ENABLE_VENDOR_IMAGE := true
endif

# Disable QTIC until it's brought up in split system/vendor
# configuration to avoid compilation breakage.
ifeq ($(ENABLE_VENDOR_IMAGE), true)
#TARGET_USES_QTIC := false
endif

# Add soft home, back and multitask keys
PRODUCT_PROPERTY_OVERRIDES += \
    qemu.hw.mainkeys=0

# Video codec configuration files
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/configs/media_profiles.xml:system/etc/media_profiles.xml \
    $(PLATFORM_PATH)/configs/media_profiles.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_profiles_vendor.xml \
    $(PLATFORM_PATH)/configs/media_codecs.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs.xml \
    $(PLATFORM_PATH)/configs/media_codecs_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_performance.xml \
    $(PLATFORM_PATH)/configs/media_codecs_vendor_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_vendor_audio.xml

# video seccomp policy files
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/seccomp/mediacodec-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy \
    $(PLATFORM_PATH)/seccomp/mediaextractor-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaextractor.policy

# Whitelists and Appboost 
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/whitelistedapps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/whitelistedapps.xml \
    $(PLATFORM_PATH)/configs/gamedwhitelist.xml:$(TARGET_COPY_OUT_VENDOR)/etc/gamedwhitelist.xml \
    $(PLATFORM_PATH)/configs/appboosts.xml:$(TARGET_COPY_OUT_VENDOR)/etc/appboosts.xml

PRODUCT_PROPERTY_OVERRIDES += \
    video.disable.ubwc=1

# Power
PRODUCT_PACKAGES += \
    android.hardware.power@1.0-service \
    android.hardware.power@1.0-impl

# Override heap growth limit due to high display density on device
PRODUCT_PROPERTY_OVERRIDES += \
    dalvik.vm.heapgrowthlimit=256m

# DAVLIK-HEAP Configration    
$(call inherit-product, frameworks/native/build/phone-xhdpi-2048-dalvik-heap.mk)

# default is nosdcard, S/W button enabled in resource
PRODUCT_CHARACTERISTICS := nosdcard

#
# system prop for opengles version
#
# 196610 is decimal for 0x30002 to report major/minor versions as 3/2
PRODUCT_PROPERTY_OVERRIDES += \
    ro.opengles.version=196610

PRODUCT_BOOT_JARS += tcmiface
PRODUCT_BOOT_JARS += telephony-ext

PRODUCT_PACKAGES += telephony-ext

# system prop for Bluetooth SOC type
PRODUCT_PROPERTY_OVERRIDES += \
    qcom.bluetooth.soc=cherokee

# HIDL Manifest
DEVICE_MANIFEST_FILE := $(PLATFORM_PATH)/manifest.xml
DEVICE_MATRIX_FILE   := device/qcom/common/compatibility_matrix.xml

PRODUCT_PACKAGES += android.hardware.media.omx@1.0-impl

# Sensor HAL conf file
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/sensors/hals.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf
    
# Exclude TOF sensor from InputManager
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml

# WLAN driver configuration file
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini \
    $(PLATFORM_PATH)/wifi_concurrency_cfg.txt:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wifi_concurrency_cfg.txt

PRODUCT_PACKAGES += \
    wpa_supplicant_overlay.conf \
    p2p_supplicant_overlay.conf

#ANT+ stack
PRODUCT_PACKAGES += \
    AntHalService \
    libantradio \
    antradio_app \
    libvolumelistener

#Display/Graphics
PRODUCT_PACKAGES += \
    android.hardware.graphics.allocator@2.0-impl \
    android.hardware.graphics.allocator@2.0-service \
    android.hardware.graphics.mapper@2.0-impl \
    android.hardware.graphics.composer@2.1-impl \
    android.hardware.graphics.composer@2.1-service \
    android.hardware.memtrack@1.0-impl \
    android.hardware.memtrack@1.0-service \
    android.hardware.light@2.0-impl \
    android.hardware.light@2.0-service \
    android.hardware.configstore@1.0-service

PRODUCT_PACKAGES += \
    vendor.display.color@1.0-service \
    vendor.display.color@1.0-impl

# Vibrator
PRODUCT_PACKAGES += \
    android.hardware.vibrator@1.0-impl \
    android.hardware.vibrator@1.0-service \

# Camera configuration file. Shared by passthrough/binderized camera HAL
PRODUCT_PACKAGES += camera.device@3.2-impl
PRODUCT_PACKAGES += camera.device@1.0-impl
PRODUCT_PACKAGES += android.hardware.camera.provider@2.4-impl
# Enable binderized camera HAL
PRODUCT_PACKAGES += android.hardware.camera.provider@2.4-service

PRODUCT_PACKAGES += \
	android.hardware.usb@1.0-service

# Ramdisk
PRODUCT_PACKAGES += \
    init.target.rc \
    fstab.qcom

# Sensor features
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
    frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
    frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
    frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
    frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
    frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
    frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
    frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
    frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml

#Facing, CMC and Gesture
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.sensors.facing=false \
    ro.vendor.sensors.cmc=false \
    ro.vendor.sdk.sensors.gestures=false
    
# Wifi
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/wifi/hostapd.accept:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd.accept \
    $(PLATFORM_PATH)/wifi/hostapd.conf:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd_default.conf \
    $(PLATFORM_PATH)/wifi/hostapd.deny:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd.deny \
    $(PLATFORM_PATH)/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf \
    $(PLATFORM_PATH)/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf

PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/wifi/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini \
    $(PLATFORM_PATH)/wifi/wifi_concurrency_cfg.txt:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wifi_concurrency_cfg.txt

# High performance VR feature
PRODUCT_COPY_FILES += \
    frameworks/native/data/etc/android.hardware.vr.high_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vr.high_performance.xml
    
# FBE support
PRODUCT_COPY_FILES += \
    $(PLATFORM_PATH)/rootdir/bin/init.qti.qseecomd.sh:$(TARGET_COPY_OUT_VENDOR)/bin/init.qti.qseecomd.sh

# MIDI feature
PRODUCT_COPY_FILES += frameworks/native/data/etc/android.software.midi.xml:system/etc/permissions/android.software.midi.xml

# MSM IRQ Balancer configuration file for SDM660
PRODUCT_COPY_FILES += $(PLATFORM_PATH)/configs/msm_irqbalance.conf:$(TARGET_COPY_OUT_VENDOR)/etc/msm_irqbalance.conf

PRODUCT_FULL_TREBLE_OVERRIDE := true

PRODUCT_VENDOR_MOVE_ENABLED := true

#for android_filesystem_config.h
PRODUCT_PACKAGES += \
    fs_config_files

#for wlan
PRODUCT_PACKAGES += \
        wificond \
        wifilogd

#Healthd packages
PRODUCT_PACKAGES += android.hardware.health@1.0-impl \
                    android.hardware.health@1.0-convert \
                    android.hardware.health@1.0-service \
                    libhealthd.msm

#FEATURE_OPENGLES_EXTENSION_PACK support string config file
PRODUCT_COPY_FILES += \
        frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml

TARGET_SUPPORT_SOTER := true

#Enable QTI KEYMASTER and GATEKEEPER HIDLs
ifeq ($(ENABLE_VENDOR_IMAGE), true)
KMGK_USE_QTI_SERVICE := true
endif

#Enable AOSP KEYMASTER and GATEKEEPER HIDLs
ifneq ($(KMGK_USE_QTI_SERVICE), true)
PRODUCT_PACKAGES += android.hardware.gatekeeper@1.0-impl \
                    android.hardware.gatekeeper@1.0-service \
                    android.hardware.keymaster@3.0-impl \
                    android.hardware.keymaster@3.0-service
endif

PRODUCT_PROPERTY_OVERRIDES += rild.libpath=/system/vendor/lib64/libril-qc-qmi-1.so

#Thermal
PRODUCT_PACKAGES += android.hardware.thermal@1.0-impl \
                    android.hardware.thermal@1.0-service

#Property for setting the max timeout of autosuspend
PRODUCT_PROPERTY_OVERRIDES += sys.autosuspend.timeout=500000
