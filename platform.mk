#
# Copyright (C) 2018 The nubia-SDM660 Project
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
PLATFORM_PATH := device/nubia/sdm660-common

$(warning "device sdm660 common: src_target_dir: $(SRC_TARGET_DIR)")

# Inherit from those products. Most specific first.
$(call inherit-product, $(SRC_TARGET_DIR)/product/core_64_bit.mk)

$(call inherit-product, $(SRC_TARGET_DIR)/product/full_base_telephony.mk)

$(call inherit-product, $(SRC_TARGET_DIR)/product/product_launched_with_o_mr1.mk)

$(call inherit-product-if-exists, build/target/product/embedded.mk)


# Overlays
DEVICE_PACKAGE_OVERLAYS := device/nubia/sdm660-common/overlay

# Build Fingerprint
PRODUCT_BUILD_PROP_OVERRIDES += \
	PRIVATE_BUILD_DESC="OP3A.190814.001 5565753 release-keys"

BUILD_FINGERPRINT := nubia/nx611j/nx611j:8/OP3A.190814.001/5565753:user/release-keys

# Platform properties
$(call inherit-product, $(PLATFORM_PATH)/platform_prop.mk)

# Alipay
#PRODUCT_PACKAGES += \
#  IFAAService \
#  org.ifaa.android.manager

#PRODUCT_BOOT_JARS += \
#  org.ifaa.android.manager

# Android_filesystem_config
PRODUCT_PACKAGES += \
	fs_config_files

# ANT+
PRODUCT_PACKAGES += \
	com.dsi.ant.antradio_library \
	libantradio

PRODUCT_COPY_FILES += \
	external/ant-wireless/antradio-library/com.dsi.ant.antradio_library.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.dsi.ant.antradio_library.xml

# Audio
PRODUCT_PACKAGES += \
	android.hardware.audio@2.0 \
	android.hardware.audio.effect@2.0 \
	android.hardware.audio.common@2.0 \
	android.hardware.audio.common@2.0-util \
	com.qualcom.qti.bluetooth_audio@1.0 \
	libaudio-resampler \
	libaudio-hal \
	libqcompostprocbundle \
	libqcomvisualizer \
	libqcomvoiceprocessing \
	libvolumelistener \
	nubia_mmi_audio 

PRODUCT_COPY_FILES += \
	frameworks/av/services/audiopolicy/config/a2dp_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/a2dp_audio_policy_configuration.xml \
	frameworks/av/services/audiopolicy/config/audio_policy_volumes.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/audio_policy_volumes.xml \
	frameworks/av/services/audiopolicy/config/default_volume_tables.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/default_volume_tables.xml \
	frameworks/av/services/audiopolicy/config/r_submix_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/r_submix_audio_policy_configuration.xml \
	frameworks/av/services/audiopolicy/config/usb_audio_policy_configuration.xml:/$(TARGET_COPY_OUT_VENDOR)/etc/usb_audio_policy_configuration.xml

# Bluetooth
BOARD_BLUETOOTH_BDROID_BUILDCFG_INCLUDE_DIR := $(PLATFORM_PATH)/bluetooth

PRODUCT_PACKAGES += \
	libbthost_if

# Camera
PRODUCT_PACKAGES += \
	android.frameworks.displayservice@1.0 \
	vendor.display.color@1.0_vendor \
	vendor.display.config@1.0_vendor \
	vendor.display.config@1.1_vendor \
	vendor.display.postproc@1.0_vendor \
	android.hardware.camera.provider@2.4 \
	camera.device@1.0-impl \
	camera.device@3.2-impl \
	camera.device@3.3-impl \
	libxml2_1 \
	vendor.qti.hardware.camera.device@1.0 \
	vendor.qti.hardware.camera.device@1.0_vendor

# CNE
PRODUCT_PACKAGES += \
	libcneapiclient \
	com.quicinc.cne.api@1.0 \
	com.quicinc.cne.constants@1.0 \
	com.quicinc.cne.constants@2.0 \
	com.quicinc.cne.server@1.0 \
	com.quicinc.cne.server@2.0 \
	libservices

# Display
PRODUCT_PACKAGES += \
	android.hardware.graphics.allocator@2.0 \
	android.hardware.graphics.composer@2.1 \
	android.hardware.graphics.mapper@2.0 \
	android.hardware.memtrack@1.0 \
	libmemtrack \
	libgrallocutils \
	libdisplayconfig \
	libsd_sdk_display \
	vendor.display.color@1.0  \
	libqdMetaData.system \
	libgpu_tonemapper \
	vendor.display.config@1.0 \
	vendor.display.config@1.1 \
	vendor.display.postproc@1.0 \
	libsdmcore

PRODUCT_PACKAGES += \
	android.hardware.configstore@1.0 \
	android.hardware.broadcastradio@1.0

# DRM
PRODUCT_PACKAGES += \
	android.hardware.drm@1.0 \
	libdrmframework_jni \
	libdrmframework \
	libdrminputstream \
	libmediadrm


# Exclude TOF sensor from InputManager
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/excluded-input-devices.xml:system/etc/excluded-input-devices.xml

# Fingerprint feature
PRODUCT_PACKAGES += \
	vendor.qti.hardware.fingerprint@1.0 \
	vendor.sw.swfingerprint@1.0 \
	nubia_mmi_fingerprint \
	android.hardware.biometrics.fingerprint@2.1

PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.fingerprint.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.fingerprint.xml

# Framework RRO
#PRODUCT_ENFORCE_RRO_TARGETS := \
	#framework-res

# Freeform Multiwindow
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.software.freeform_window_management.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.software.freeform_window_management.xml

# Fstab
PRODUCT_PACKAGES += \
	fstab.qcom

# GMS
PRODUCT_GMS_CLIENTID_BASE := android-nubia

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
	$(PLATFORM_PATH)/configs/gps/xtwifi.conf:$(TARGET_COPY_OUT_VENDOR)/etc/xtwifi.conf \
	$(PLATFORM_PATH)/configs/com.qualcomm.location.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/com.qualcomm.location.xml

PRODUCT_PACKAGES += \
	android.hardware.gnss@1.0 \
	vendor.qti.gnss@1.0_vendor \
	vendor.qti.gnss@1.0 \
	libgnss \
	libgnsspps \
	android.frameworks.sensorservice@1.0  \
	libsensorservice \
	libsensor1 \
	libsensor_reg \
	nubia_mmi_sensor \
	sensor_calibrate \
	android.hardware.neuralnetworks@1.0 \
	libneuralnetworks

# Health
PRODUCT_PACKAGES += \
	android.hardware.health@1.0 \
	libsystem_health_mon

# HIDL
PRODUCT_PACKAGES += \
	android.hidl.base@1.0 \
	libbt-hidlclient \
	libkeystore-engine-wifi-hidl \
	libkeystore-wifi-hidl \
	android.hidl.allocator@1.0 \
	android.hidl.base@1.0 \
	android.hidl.memory@1.0 \
	android.hidl.token@1.0 \
	android.hidl.token@1.0-utils \
	libdisplayservicehidl \
	libhidlbase \
	libhidl-gen-utils \
	libhidlmemory \
	libhidltransport \
	libschedulerservicehidl \
	libsensorservicehidl


# IMS
PRODUCT_PACKAGES += \
	com.qualcomm.qti.imscmservice@1.0_vendor \
	com.qualcomm.qti.imscmservice@1.1_vendor \
	com.qualcomm.qti.imscmservice@1.0 \
	com.qualcomm.qti.imscmservice@1.1 \
	vendor.qti.hardware.radio.ims@1.0_vendor \
	vendor.qti.imsrtpservice@1.0-service-Impl \
	vendor.qti.imsrtpservice@1.0_vendor \
	vendor.qti.imsrtpservice@1.0 \
	lib-imscmservice \
	lib-imsdpl \
	lib-imsqimf \
	lib-imsrcs-v2 \
	lib-imsxml \
	libimscamera_jni \
	lib-imscamera \
	libimsmedia_jni \
	lib-imsvideocodec \
	lib-imsvtextutils \
	lib-imsvt \
	lib-imsvtutils 

# Input
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/idc/uinput-fpc.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/uinput-fpc.idc \
	$(PLATFORM_PATH)/configs/idc/uinput-goodix.idc:$(TARGET_COPY_OUT_VENDOR)/usr/idc/uinput-goodix.idc

# IPACM
PRODUCT_PACKAGES += \
	libipanat \
	liboffloadhal

# IPv6
PRODUCT_PACKAGES += \
	libebtable_broute \
	libebtable_filter \
	libebtable_nat \

# IRSC
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/sec_config:$(TARGET_COPY_OUT_VENDOR)/etc/sec_config

# Keylayouts
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/keylayout/sdm660-snd-card_Button_Jack.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/sdm660-snd-card_Button_Jack.kl \
	$(PLATFORM_PATH)/configs/keylayout/gpio-keys.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/gpio-keys.kl \
	$(PLATFORM_PATH)/configs/keylayout/uinput-goodix.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/uinput-goodix.kl \
	$(PLATFORM_PATH)/configs/keylayout/uinput-fpc.kl:$(TARGET_COPY_OUT_VENDOR)/usr/keylayout/uinput-fpc.kl

# Lights
PRODUCT_PACKAGES += \
	android.hardware.light@2.0

# LiveDisplay native
#PRODUCT_PACKAGES += \
#	vendor.lineage.livedisplay@2.0-service-sdm

# MSM IRQ Balancer configuration file for SDM660
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/configs/msm_irqbalance.conf:$(TARGET_COPY_OUT_VENDOR)/etc/msm_irqbalance.conf

# Net
PRODUCT_PACKAGES += \
	android.system.net.netd@1.0 \
	libandroid_net \
	libnetdaidl \
	libnetd_client \
	libnetdutils \
	libnetlink \
	libnetutils \
	libnetmgr


# OMX
PRODUCT_PACKAGES += \
	libOmxMux \
	android.hardware.media.omx@1.0 \
	libmedia_omx \
	libstagefright_omx \
	libstagefright_omx_utils \
	libmm-omxcore \
	libOmxAacDec \
	libOmxAacEnc \
	libOmxAlacDec \
	libOmxAlacDecSw \
	libOmxAmrDec \
	libOmxAmrEnc \
	libOmxAmrwbplusDec \
	libOmxApeDec \
	libOmxApeDecSw \
	libOmxCore \
	libOmxEvrcDec \
	libOmxEvrcEnc \
	libOmxG711Dec \
	libOmxG711Enc \
	libOmxQcelp13Dec \
	libOmxQcelp13Enc \
	libOmxVdec \
	libOmxVenc \
	libOmxVpp \
	libOmxWmaDec \
	libc2dcolorconvert \
	libI420colorconvert \
	libmm-color-convertor \
	libsdm-color \
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
	frameworks/native/data/etc/android.hardware.vulkan.level-1.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vulkan.level-1.xml \
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
	vendor.qti.esepowermanager@1.0_vendor \
	vendor.qti.hardware.wipower@1.0_vendor \
	android.hardware.power@1.0 \
	android.hardware.power@1.1 \
	libpowermanager \
	libpower \
	vendor.qti.esepowermanager@1.0 

PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/power-libperfmgr/powerhint.json:$(TARGET_COPY_OUT_VENDOR)/etc/powerhint.json

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
	init.qcom.power.sh \
	init.qti.fm.sh \
	init.qti.qseecomd.sh \
	move_goodix_data.sh

PRODUCT_PACKAGES += \
	init.msm.usb.configfs.rc \
	init.qcom.rc \
	init.qcom.usb.rc \
	init.target.rc \
	init.spectrum.rc \
	init.performance.sdm660.rc \
	ueventd.qcom.rc
	#init.environ.rc \	           
	#init.nb.device.rc \	            
	#init.nubia.display.Hrc \	           
	#init.nubia.display.rc \	           
	#init.nubia.touch.rc \	           
	#init.rc \	          
	#init.recovery.qcom.rc \	           
	#init.rom.rc \	            
	#init.usb.configfs.rc \	          
	#init.usb.rc \	           
	#init.zygote32.rc \	            
	#init.zygote64_32.rc \	            

# RCS
PRODUCT_PACKAGES += \
	rcs_service_aidl \
	rcs_service_aidl.xml \
	rcs_service_api \
	rcs_service_api.xml

# RenderScript HAL
PRODUCT_PACKAGES += \
	android.hardware.renderscript@1.0

# RIL
PRODUCT_PACKAGES += \
	android.hardware.broadcastradio@1.0 \
	android.hardware.broadcastradio@1.1 \
	android.hardware.radio@1.0 \
	android.hardware.radio@1.1 \
	android.hardware.radio.deprecated@1.0 \
	libantradio \
	libradio_metadata \
	libril-qc-radioconfig \
	vendor.qti.hardware.radio.am@1.0_vendor \
	vendor.qti.hardware.radio.atcmdfwd@1.0_vendor \
	vendor.qti.hardware.radio.lpa@1.0_vendor \
	vendor.qti.hardware.radio.qcrilhook@1.0_vendor \
	vendor.qti.hardware.radio.qtiradio@1.0_vendor \
	vendor.qti.hardware.radio.uim@1.0_vendor \
	vendor.qti.hardware.radio.uim_remote_client@1.0_vendor \
	vendor.qti.hardware.radio.uim_remote_server@1.0_vendor \
	librmnetctl \
	libxml \
	libprotobuf-cpp-full
	#android.hardware.radio.config@1.0 \
	#android.hardware.secure_element@1.0 \

#PRODUCT_PACKAGES += \
	#rild \
	#CarrierConfig

# Sensors
PRODUCT_PACKAGES += \
	android.hardware.sensors@1.0 \
	sensors.hal.tof \
	sensors.oem \
	sensors.ssc 

# Sensor HAL conf file
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/sensors/hals.conf:$(TARGET_COPY_OUT_VENDOR)/etc/sensors/hals.conf

# Telephony
#PRODUCT_PACKAGES += \
.	telephony-ext

#PRODUCT_BOOT_JARS += \
#	telephony-ext

# TextClassifier
#PRODUCT_PACKAGES += \
	#textclassifier.bundle1

# Thermal
PRODUCT_PACKAGES += \
	android.hardware.thermal@1.0 \
	android.hardware.thermal@1.1 \
	libthermalclient \
	libthermalioctl \
	libthermalcallback \
	libthermalservice
	#thermal.sdm660

# ThermalController app
#PRODUCT_PACKAGES += \
	#ThermalController

# Touchscreen
PRODUCT_PACKAGES += \
	libtinyxml \
	libtinyxml2_1

# USB
PRODUCT_PACKAGES += \
	android.hardware.usb@1.0 \
	libusbhost

# Video seccomp policy files
PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/seccomp/mediacodec-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediacodec.policy \
	$(PLATFORM_PATH)/seccomp/mediaextractor-seccomp.policy:$(TARGET_COPY_OUT_VENDOR)/etc/seccomp_policy/mediaextractor.policy

# VNDK
PRODUCT_PACKAGES += \
	libdng_sdk \
	libstdc++ \
	vndk-sp \
	libvndksupport

#PRODUCT_COPY_FILES += \
	#prebuilts/vndk/v27/arm/arch-arm-armv7-a-neon/shared/vndk-core/android.frameworks.sensorservice@1.0.so:$(TARGET_COPY_OUT_VENDOR)/lib/android.frameworks.sensorservice@1.0-v27.so \
	#prebuilts/vndk/v27/arm64/arch-arm64-armv8-a/shared/vndk-core/android.frameworks.sensorservice@1.0.so:$(TARGET_COPY_OUT_VENDOR)/lib64/android.frameworks.sensorservice@1.0-v27.so

# VR feature
PRODUCT_COPY_FILES += \
	frameworks/native/data/etc/android.hardware.vr.high_performance.xml:$(TARGET_COPY_OUT_VENDOR)/etc/permissions/android.hardware.vr.high_performance.xml

# VR
PRODUCT_PACKAGES += \
	android.frameworks.vr.composer@1.0 \
	android.hardware.vr@1.0 \
	libvr_amb_engine \
	libvraudio \
	libvr_object_engine \
	libvr_sam_wrapper 

    #vr.sdm660

# Wifi
PRODUCT_PACKAGES += \
	android.hardware.wifi@1.0 \
	android.hardware.wifi@1.1 \
	android.hardware.wifi.offload@1.0 \
	android.hardware.wifi.supplicant@1.0 \
	android.system.wifi.keystore@1.0 \
	com.qualcomm.qti.wifidisplayhal@1.0 \
	com.qualcomm.qti.wifidisplayhal@1.0-halimpl \
	com.qualcomm.qti.wifidisplayhal@1.0-impl \
	com.qualcomm.qti.wifidisplayhal@1.0_vendor \
	vendor.qti.hardware.wifi.keystore@1.0 \
	vendor.qti.hardware.wifi.keystore@1.0_vendor \
	vendor.qti.hardware.wifi.supplicant@1.0_vendor \
	libwificond_ipc \
	libwifikeystorehalext \
	libwifikeystorehal \
	libwifi-service \
	libwifi-system-iface \
	libwifi-system \
	libkeystore-engine-wifi-hidl \
	libkeystore-wifi-hidl \
	liblowi_wifihal \
	libwifi-hal-qcom \
	libwifi-hal \
	libxtwifi_ulp_adaptor \
	libcert_parse.wpa_s \
	libwpa_client \
	libqsap_sdk 

	#hostapd \
	#hostapd_cli \
	#libQWiFiSoftApCfg \
	#wificond \
	#wifilogd \
	#wpa_supplicant \
	#wpa_supplicant.conf

PRODUCT_COPY_FILES += \
	$(PLATFORM_PATH)/wifi/p2p_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/p2p_supplicant_overlay.conf \
	$(PLATFORM_PATH)/wifi/wpa_supplicant_overlay.conf:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/wpa_supplicant_overlay.conf \
	$(PLATFORM_PATH)/wifi/WCNSS_qcom_cfg.ini:$(TARGET_COPY_OUT_VENDOR)/etc/wifi/WCNSS_qcom_cfg.ini

# KeyHandler
#PRODUCT_PACKAGES += \
#	org.lineageos.keyhandler

# Wi-Fi Display
PRODUCT_BOOT_JARS += \
	libstagefright_wfd \
	libwfdavenhancements \
	libwfdcodecv4l2 \
	libwfdcommonutils \
	libwfdconfigutils \
	libwfdmminterface \
	libwfdmmsink \
	libwfdmmsrc \
	libwfdnative \
	libwfdrtsp \
	libwfdservice \
	libwfdsm \
	libwfds \
	libwfduibcinterface \
	libwfduibcsinkinterface \
	libwfduibcsink \
	libwfduibcsrcinterface \
	libwfduibcsrc \
	libwfdcommonutils_proprietary \
	libwfdhaldsmanager \
	libwfdhdcpcp \
	libwfdmmservice \
	libwfdmodulehdcpsession


PRODUCT_PACKAGES += \
	libnl 
	#libwfdaac

# Vendor files
#$(call inherit-product, vendor/nubia/sdm660-common/sdm660-common-vendor.mk)
