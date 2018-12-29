#
# Copyright (C) 2018 The Xiaomi-SDM660 Project
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

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)
$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_o_mr1.mk)
$(call inherit-product, build/target/product/embedded.mk)

# Overlays
DEVICE_PACKAGE_OVERLAYS := device/xiaomi/sdm660-common/overlay

# Platform properties
$(call inherit-product, $(PLATFORM_PATH)/platform_prop.mk)

# Android_filesystem_config
PRODUCT_PACKAGES += \
	fs_config_files

# ANT+
PRODUCT_PACKAGES += \
	AntHalService \
	antradio_app \
	com.dsi.ant.antradio_library \
	libantradio

PRODUCT_COPY_FILES += \
	external/ant-wireless/antradio-library/com.dsi.ant.antradio_library.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.dsi.ant.antradio_library.xml

# Audio
PRODUCT_PACKAGES += \
	android.hardware.audio@4.0-impl:32 \
	android.hardware.audio@2.0-service \
	android.hardware.audio.effect@4.0-impl:32 \
	android.hardware.audio.effect@2.0-service \
	android.hardware.soundtrigger@2.1-impl:32 \
	android.hardware.soundtrigger@2.1-service \
	android.hardware.audio@4.0 \
	android.hardware.audio.common@4.0 \
	android.hardware.audio.common@4.0-util \
	audio.a2dp.default \
	audio.primary.sdm660 \
	audio.r_submix.default \
	audio.usb.default \
	libaudio-resampler \
	libqcompostprocbundle \
	libqcomvisualizer \
	libqcomvoiceprocessing \
	libvolumelistener \
	tinymix

PRODUCT_COPY_FILES += \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/default_volume_tables.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
	$(TOPDIR)frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml

PRODUCT_PROPERTY_OVERRIDES += \
	ro.af.client_heap_size_kbyte=7168 \
	ro.config.media_vol_steps=25 \
	ro.config.vc_call_vol_steps=7

# Bluetooth
PRODUCT_PACKAGES += \
	libbthost_if

# Camera
PRODUCT_PACKAGES += \
	android.frameworks.displayservice@1.0_32 \
	android.hardware.camera.provider@2.4-impl:32 \
	android.hardware.camera.provider@2.4-service \
	camera.device@3.2-impl \
	libxml2 \
	Snap \
	vendor.qti.hardware.camera.device@1.0 \
	vendor.qti.hardware.camera.device@1.0_vendor

# CNE
PRODUCT_PACKAGES += \
	cneapiclient \
	com.quicinc.cne

# Display
PRODUCT_PACKAGES += \
	android.hardware.graphics.allocator@2.0-impl:64 \
	android.hardware.graphics.allocator@2.0-service \
	android.hardware.graphics.composer@2.1-impl:64 \
	android.hardware.graphics.composer@2.1-service \
	android.hardware.graphics.mapper@2.0-impl \
	android.hardware.memtrack@1.0-impl \
	android.hardware.memtrack@1.0-service \
	copybit.sdm660 \
	gralloc.sdm660 \
	hwcomposer.sdm660 \
	libdisplayconfig \
	liboverlay \
	libqdMetaData.system \
	libtinyxml \
	memtrack.sdm660

PRODUCT_PACKAGES += \
	android.hardware.configstore@1.0-service \
	android.hardware.broadcastradio@1.0-impl

# Doze
PRODUCT_PACKAGES += \
	XiaomiDoze

# DRM
PRODUCT_PACKAGES += \
	android.hardware.drm@1.0-impl:32 \
	android.hardware.drm@1.0-service \
	android.hardware.drm@1.1-service.clearkey

# Exclude TOF sensor from InputManager
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml

# FBE support
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/rootdir/bin/init.qti.qseecomd.sh:$(TARGET_COPY_OUT_VENDOR)/bin/init.qti.qseecomd.sh

# Fingerprint feature
PRODUCT_PACKAGES += \
	fingerprintd \
	android.hardware.biometrics.fingerprint@2.1-service.xiaomi_sdm660

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.fingerprint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.fingerprint.xml

# FM packages
PRODUCT_PACKAGES += \
	libqcomfm_jni \
	android.hardware.broadcastradio@1.0-impl \
	FM2 \
	qcom.fmradio \
	qcom.fmradio.xml

# Framework RRO
PRODUCT_ENFORCE_RRO_TARGETS := \
	framework-res

# Freeform Multiwindow
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.software.freeform_window_management.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.freeform_window_management.xml

# Fstab
PRODUCT_PACKAGES += \
	fstab.qcom

# GMS
PRODUCT_GMS_CLIENTID_BASE := android-xiaomi

# Google Media Codec
PRODUCT_COPY_FILES += \
	frameworks/av/media/libstagefright/data/media_codecs_google_audio.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_audio.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_telephony.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_telephony.xml \
	frameworks/av/media/libstagefright/data/media_codecs_google_video.xml:$(TARGET_COPY_OUT_VENDOR)/etc/media_codecs_google_video.xml

# GPS
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/gps/apdr.conf:$(TARGET_COPY_OUT_VENDOR)/etc/apdr.conf \
	$(PLATFORM_PATH)/configs/gps/flp.conf:$(TARGET_COPY_OUT_VENDOR)/etc/flp.conf \
	$(PLATFORM_PATH)/configs/gps/gps.conf:$(TARGET_COPY_OUT_VENDOR)/etc/gps.conf \
	$(PLATFORM_PATH)/configs/gps/izat.conf:$(TARGET_COPY_OUT_VENDOR)/etc/izat.conf \
	$(PLATFORM_PATH)/configs/gps/lowi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/lowi.conf \
	$(PLATFORM_PATH)/configs/gps/sap.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sap.conf \
	$(PLATFORM_PATH)/configs/gps/xtwifi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/xtwifi.conf

PRODUCT_PACKAGES += \
	android.hardware.gnss@1.1-impl-qti \
	android.hardware.gnss@1.1-service-qti \
	libgnss \
	libgnsspps \
	libsensorndkbridge \
	libvehiclenetwork-native

# Health
PRODUCT_PACKAGES += \
	android.hardware.health@2.0-service

# HIDL
PRODUCT_PACKAGES += \
	android.hidl.base@1.0

# IMS
PRODUCT_PACKAGES += \
	ims-ext-common

# Input
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/idc/uinput-fpc.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/uinput-fpc.idc \
	$(PLATFORM_PATH)/configs/idc/uinput-goodix.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/uinput-goodix.idc

# IPACM
PRODUCT_PACKAGES += \
	ipacm \
	IPACM_cfg.xml \
	libipanat \
	liboffloadhal

# IPv6
PRODUCT_PACKAGES += \
	ebtables \
	ethertypes

# IRSC
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Keylayouts
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/keylayout/sdm660-snd-card_Button_Jack.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/sdm660-snd-card_Button_Jack.kl \
	$(PLATFORM_PATH)/keylayout/uinput-goodix.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/uinput-goodix.kl

# MSM IRQ Balancer configuration file for SDM660
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/msm_irqbalance.conf:$(TARGET_COPY_OUT_VENDOR)/etc/msm_irqbalance.conf

# Net
PRODUCT_PACKAGES += \
	android.system.net.netd@1.0 \
	libandroid_net \
	netutils-wrapper-1.0

# OMX
PRODUCT_PACKAGES += \
	libc2dcolorconvert \
	libaacwrapper \
	libmm-omxcore \
	libOmxAacEnc \
	libOmxAmrEnc \
	libOmxCore \
	libOmxEvrcEnc \
	libOmxG711Enc \
	libOmxQcelp13Enc \
	libOmxVdec \
	libOmxVenc \
	libstagefrighthw

# Permissions
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.audio.low_latency.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.low_latency.xml \
	frameworks/native/data/etc/android.hardware.audio.pro.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.audio.pro.xml \
	frameworks/native/data/etc/android.hardware.bluetooth_le.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth_le.xml \
	frameworks/native/data/etc/android.hardware.bluetooth.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.bluetooth.xml \
	frameworks/native/data/etc/android.hardware.camera.autofocus.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/native/data/etc/android.hardware.camera.front.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.front.xml \
	frameworks/native/data/etc/android.hardware.camera.full.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.full.xml \
	frameworks/native/data/etc/android.hardware.camera.raw.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.camera.raw.xml \
	frameworks/native/data/etc/android.hardware.location.gps.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.location.gps.xml \
	frameworks/native/data/etc/android.hardware.opengles.aep.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.opengles.aep.xml \
	frameworks/native/data/etc/android.hardware.sensor.accelerometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.accelerometer.xml \
	frameworks/native/data/etc/android.hardware.sensor.ambient_temperature.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.ambient_temperature.xml \
	frameworks/native/data/etc/android.hardware.sensor.barometer.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.barometer.xml \
	frameworks/native/data/etc/android.hardware.sensor.compass.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.compass.xml \
	frameworks/native/data/etc/android.hardware.sensor.gyroscope.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.gyroscope.xml \
	frameworks/native/data/etc/android.hardware.sensor.hifi_sensors.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.hifi_sensors.xml \
	frameworks/native/data/etc/android.hardware.sensor.light.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.light.xml \
	frameworks/native/data/etc/android.hardware.sensor.proximity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.proximity.xml \
	frameworks/native/data/etc/android.hardware.sensor.relative_humidity.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.relative_humidity.xml \
	frameworks/native/data/etc/android.hardware.sensor.stepcounter.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepcounter.xml \
	frameworks/native/data/etc/android.hardware.sensor.stepdetector.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.sensor.stepdetector.xml \
	frameworks/native/data/etc/android.hardware.telephony.cdma.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.cdma.xml \
	frameworks/native/data/etc/android.hardware.telephony.gsm.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/native/data/etc/android.hardware.touchscreen.multitouch.jazzhand.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.touchscreen.multitouch.jazzhand.xml \
	frameworks/native/data/etc/android.hardware.usb.accessory.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.accessory.xml \
	frameworks/native/data/etc/android.hardware.usb.host.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.usb.host.xml \
	frameworks/native/data/etc/android.hardware.vulkan.level-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level.xml \
	frameworks/native/data/etc/android.hardware.vulkan.version-1_1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.version.xml \
	frameworks/native/data/etc/android.hardware.vulkan.compute-0.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.compute.xml \
	frameworks/native/data/etc/android.hardware.wifi.direct.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.direct.xml \
	frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
	frameworks/native/data/etc/android.hardware.wifi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.xml \
	frameworks/native/data/etc/android.hardware.wifi.passpoint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.wifi.passpoint.xml \
	frameworks/native/data/etc/android.software.midi.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.midi.xml \
	frameworks/native/data/etc/android.software.sip.voip.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.sip.voip.xml \
	frameworks/native/data/etc/handheld_core_hardware.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/handheld_core_hardware.xml

# Power
PRODUCT_PACKAGES += \
	android.hardware.power@1.1-service-qti

# Public Libraries
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/public.libraries.txt:$(TARGET_COPY_OUT_VENDOR)/etc/public.libraries.txt

# QCOM
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/privapp-permissions-qti.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/privapp-permissions-qti.xml \
	$(PLATFORM_PATH)/configs/qti_whitelist.xml:$(TARGET_COPY_OUT_VENDOR)/etc/sysconfig/qti_whitelist.xml

# QMI
PRODUCT_PACKAGES += \
	libjson

# Ramdisk
PRODUCT_PACKAGES += \
	init.class_main.sh \
	init.mdm.sh \
	init.qcom.early_boot.sh \
	init.qcom.post_boot.sh \
	init.qcom.sh \
	init.qcom.usb.sh \
	init.qti.fm.sh \
	init.qti.qseecomd.sh

PRODUCT_PACKAGES += \
	init.msm.usb.configfs.rc \
	init.qcom.rc \
	init.qcom.usb.rc \
	init.target.rc \
	ueventd.qcom.rc

# RCS
PRODUCT_PACKAGES += \
	rcs_service_aidl \
	rcs_service_aidl.xml \
	rcs_service_api \
	rcs_service_api.xml

# RenderScript HAL
PRODUCT_PACKAGES += \
	android.hardware.renderscript@1.0-impl

# RIL
PRODUCT_PACKAGES += \
	android.hardware.radio@1.2 \
	android.hardware.radio.config@1.0 \
	android.hardware.secure_element@1.0 \
	librmnetctl \
	libxml2 \
	libprotobuf-cpp-full

PRODUCT_PACKAGES += \
	rild \
	CarrierConfig

# Sensors
PRODUCT_PACKAGES += \
	android.hardware.sensors@1.0-impl:64 \
	android.hardware.sensors@1.0-service

# Sensor HAL conf file
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/sensors/hals.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf

# Telephony
PRODUCT_PACKAGES += \
	telephony-ext

PRODUCT_BOOT_JARS += \
	telephony-ext

# TextClassifier
PRODUCT_PACKAGES += \
	textclassifier.bundle1

# Thermal
PRODUCT_PACKAGES += \
	android.hardware.thermal@1.0-impl \
	android.hardware.thermal@1.0-service \
	thermal.sdm660

# Touchscreen
PRODUCT_PACKAGES += \
	libtinyxml2

# USB
PRODUCT_PACKAGES += \
	android.hardware.usb@1.0-service.xiaomi_sdm660

# Video seccomp policy files
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/seccomp/mediacodec-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy \
	$(PLATFORM_PATH)/seccomp/mediaextractor-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaextractor.policy

# VNDK
PRODUCT_PACKAGES += vndk-sp

PRODUCT_COPY_FILES += \
	prebuilts/vndk/v27/arm/arch-arm-armv7-a-neon/shared/vndk-core/android.frameworks.sensorservice@1.0.so:$(TARGET_COPY_OUT_VENDOR)/lib/android.frameworks.sensorservice@1.0-v27.so \
	prebuilts/vndk/v27/arm64/arch-arm64-armv8-a/shared/vndk-core/android.frameworks.sensorservice@1.0.so:$(TARGET_COPY_OUT_VENDOR)/lib64/android.frameworks.sensorservice@1.0-v27.so

# VR feature
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.vr.high_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vr.high_performance.xml

# Wifi
PRODUCT_PACKAGES += \
	android.hardware.wifi@1.0-service \
	hostapd \
	hostapd_cli \
	libqsap_sdk \
	libQWiFiSoftApCfg \
	libwifi-hal-qcom \
	libwpa_client \
	wificond \
	wifilogd \
	wpa_supplicant \
	wpa_supplicant.conf

PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/wifi/hostapd.accept:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd.accept \
	$(PLATFORM_PATH)/wifi/hostapd.conf:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd_default.conf \
	$(PLATFORM_PATH)/wifi/hostapd.deny:$(TARGET_COPY_OUT_VENDOR)/etc/hostapd/hostapd.deny \
	$(PLATFORM_PATH)/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf \
	$(PLATFORM_PATH)/wifi/wifi_concurrency_cfg.txt:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wifi_concurrency_cfg.txt \
	$(PLATFORM_PATH)/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
	$(PLATFORM_PATH)/wifi/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini

# XiaomiParts
PRODUCT_PACKAGES += \
	XiaomiParts

# KCal
PRODUCT_PACKAGES += \
	KCal

# Wi-Fi Display
PRODUCT_BOOT_JARS += \
    WfdCommon