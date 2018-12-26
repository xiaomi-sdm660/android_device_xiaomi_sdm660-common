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

# Audio
PRODUCT_PROPERTY_OVERRIDES += \
	af.fast_track_multiplier=1 \
	audio.offload.min.duration.secs=30 \
	audio.offload.video=true \
	audio.deep_buffer.media=true \
	persist.vendor.audio.hifi.int_codec=true \
	persist.vendor.audio.hw.binder.size_kbyte=1024 \
	persist.vendor.audio.ras.enabled=false \
	persist.dirac.acs.controller=qem \
	persist.dirac.acs.storeSettings=1 \
	persist.dirac.acs.ignore_error=1 \
	ro.af.client_heap_size_kbyte=7168 \
	ro.audio.flinger_standbytime_ms=300 \
	ro.audio.soundfx.dirac=true \
	ro.vendor.audio.sdk.fluencetype=fluence \
	ro.vendor.audio.sdk.ssr=false \
	vendor.audio.dolby.ds2.enabled=false \
	vendor.audio.dolby.ds2.hardbypass=false \
	vendor.audio.flac.sw.decoder.24bit=true \
	vendor.audio_hal.period_size=192 \
	vendor.audio.hw.aac.encoder=true \
	vendor.audio.noisy.broadcast.delay=600 \
	vendor.audio.offload.buffer.size.kb=64 \
	vendor.audio.offload.gapless.enabled=true \
	vendor.audio.offload.multiaac.enable=true \
	vendor.audio.offload.multiple.enabled=false \
	vendor.audio.offload.passthrough=false \
	vendor.audio.offload.pstimeout.secs=3 \
	vendor.audio.parser.ip.buffer.size=262144 \
	vendor.audio.safx.pbe.enabled=true \
	vendor.audio.tunnel.encode=false \
	vendor.audio.use.sw.alac.decoder=true \
	vendor.audio.use.sw.ape.decoder=true \
	vendor.fm.a2dp.conc.disabled=true \
	vendor.voice.path.for.pcm.voip=true


# Bluetooth
PRODUCT_PROPERTY_OVERRIDES += \
	bt.max.hfpclient.connections=1 \
	persist.bt.a2dp.aac_disable=true \
	persist.vendor.bt.a2dp_offload_cap=sbc-aptx-aptxhd-aac \
	vendor.qcom.bluetooth.soc=cherokee

# Brightness
PRODUCT_PROPERTY_OVERRIDES += \
	sys.autobrightness_optimize=true

# Camera
PRODUCT_PROPERTY_OVERRIDES += \
	persist.camera.preview.ubwc=0 \
	persist.vendor.qti.telephony.vt_cam_interface=1 \
	vendor.camera.aux.packagelist=org.codeaurora.snapcam,com.android.camera,com.qualcomm.qti.qmmi,org.cyanogenmod.snap \
	camera.hal1.packagelist=com.whatsapp,com.android.camera,com.android.camera2

# Charger
PRODUCT_PROPERTY_OVERRIDES += \
	persist.chg.max_volt_mv=9000

# CNE
PRODUCT_PROPERTY_OVERRIDES += \
	persist.vendor.cne.feature=1

# Dalvik
PRODUCT_PROPERTY_OVERRIDES += \
	dalvik.vm.heapgrowthlimit=256m \
	dalvik.vm.heapstartsize=8m \
	dalvik.vm.heapsize=512m \
	dalvik.vm.heaptargetutilization=0.75 \
	dalvik.vm.heapminfree=512k \
	dalvik.vm.heapmaxfree=8m

# Data
PRODUCT_PROPERTY_OVERRIDES += \
	persist.fuse_sdcard=true

# Dirac (Xiaomi Headset Effect)
PRODUCT_PROPERTY_OVERRIDES += \
	persist.dirac.acs.controller=qem \
	persist.dirac.acs.storeSettings=1 \
	persist.dirac.acs.ignore_error=1 \
	ro.audio.soundfx.dirac=true

# Display
PRODUCT_PROPERTY_OVERRIDES += \
	vendor.gralloc.enable_fb_ubwc=1 \
	debug.sf.enable_hwc_vds=1 \
	debug.sf.hw=1 \
	debug.sf.latch_unsignaled=1 \
	persist.hwc.enable_vds=1 \
	ro.opengles.version=196610 \
	sdm.debug.disable_skip_validate=1 \
	vendor.display.enable_default_color_mode=0

# DPM
PRODUCT_PROPERTY_OVERRIDES += \
	persist.vendor.dpm.feature=1

# DRM
PRODUCT_PROPERTY_OVERRIDES += \
	drm.service.enabled=true

# Factory reset partition
PRODUCT_PROPERTY_OVERRIDES += \
	ro.frp.pst=/dev/block/bootdevice/by-name/frp

# GPS
PRODUCT_PROPERTY_OVERRIDES += \
	persist.backup.ntpServer="0.pool.ntp.org" \
	persist.vendor.overlay.izat.optin=rro

# HW Keys
PRODUCT_PROPERTY_OVERRIDES += \
	qemu.hw.mainkeys=0

# IMS
PRODUCT_PROPERTY_OVERRIDES += \
	persist.dbg.ims_volte_enable=1 \
	persist.radio.videopause.mode=1 \
	persist.data.iwlan.enable=true \
	persist.dbg.volte_avail_ovr=1 \
	persist.dbg.vt_avail_ovr=1 \
	persist.radio.VT_HYBRID_ENABLE=1

# Keyguard
PRODUCT_PROPERTY_OVERRIDES += \
	keyguard.no_require_sim=true

# Media
PRODUCT_PROPERTY_OVERRIDES += \
	media.aac_51_output_enabled=true \
	media.stagefright.enable-player=true \
	media.stagefright.enable-http=true \
	media.stagefright.enable-aac=true \
	media.stagefright.enable-qcp=true \
	media.stagefright.enable-scan=true \
	mm.enable.smoothstreaming=true \
	mmp.enable.3g2=true \
  	persist.mm.enable.prefetch=true \
	video.disable.ubwc=1

# Netflix custom property
PRODUCT_PROPERTY_OVERRIDES += \
	ro.netflix.bsp_rev=Q660-13149-1

# Perf
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.perf.debug=false \
	ro.vendor.extension_library=libqti-perfd-client.so \
	ro.vendor.qti.sys.fw.bg_apps_limit=60 \
	vendor.display.disable_skip_validate=1 \
	sdm.perf_hint_window=50

# Qcom System Daemon
PRODUCT_PROPERTY_OVERRIDES += \
	persist.vendor.qcomsysd.enabled=1

# Radio
PRODUCT_PROPERTY_OVERRIDES += \
	persist.data.df.dl_mode=5 \
	persist.data.df.ul_mode=5 \
	persist.data.df.agg.dl_pkt=10 \
	persist.data.df.agg.dl_size=4096 \
	persist.data.df.mux_count=8 \
	persist.data.df.iwlan_mux=9 \
	persist.data.df.dev_name=rmnet_usb0 \
	persist.vendor.data.mode=concurrent \
	persist.data.netmgrd.qos.enable=true \
	persist.data.wda.enable=true \
	persist.vendor.radio.apm_sim_not_pwdn=1 \
	persist.radio.atfwd.start=true \
	persist.radio.data_con_rprt=1 \
	persist.rmnet.data.enable=true \
	persist.vendor.radio.sib16_support=1 \
	persist.vendor.radio.custom_ecc=1 \
	persist.vendor.radio.rat_on=combine \
	persist.radio.schd.cache=3500 \
	rild.libpath=/system/vendor/lib64/libril-qc-qmi-1.so \
	ro.com.android.dataroaming=true \
	ro.telephony.default_network=22,20 \
	ro.vendor.use_data_netmgrd=true \
	telephony.lteOnCdmaDevice=1

# Sensors
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.sensors.facing=false \
	ro.vendor.sensors.cmc=false \
	ro.vendor.sensors.dev_ori=false \
	ro.vendor.sensors.pmd=true \
	ro.vendor.sensors.sta_detect=true \
	ro.vendor.sensors.mot_detect=true \
	ro.vendor.sdk.sensors.gestures=false \
	vendor.display.disable_rotator_downscale=1

# Shutdown and Restart
PRODUCT_PROPERTY_OVERRIDES += \
	ro.build.shutdown_timeout=0 \
	sys.vendor.shutdown.waittime=500

# Stm-events
PRODUCT_PROPERTY_OVERRIDES += \
	persist.debug.coresight.config=stm-events

# Suspend Time
PRODUCT_PROPERTY_OVERRIDES += \
	sys.autosuspend.timeout=500000

# System delay
PRODUCT_PROPERTY_OVERRIDES += \
	persist.sys.job_delay=true

# Time daemon
PRODUCT_PROPERTY_OVERRIDES += \
	persist.timed.enable=true \
	persist.delta_time.enable=true

# Memory optimizations
PRODUCT_PROPERTY_OVERRIDES += \
    ro.vendor.qti.sys.fw.bservice_enable=true

# Vendor specific library
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.at_library=libqti-at.so

# Video encoding
PRODUCT_PROPERTY_OVERRIDES += \
	vendor.vidc.dec.enable.downscalar=1 \
	vendor.vidc.enc.disable_bframes=1 \
	vendor.vidc.enc.disable.pq=false \
	vidc.enc.dcvs.extra-buff-count=2 \
	vidc.enc.target_support_bframe=1

# WFD
PRODUCT_PROPERTY_OVERRIDES += \
	persist.debug.wfd.enable=1 \
	persist.sys.wfd.virtual=0

# Wipower
PRODUCT_PROPERTY_OVERRIDES += \
	ro.bluetooth.emb_wp_mode=true \
	ro.bluetooth.wipower=true

# ZRAM
PRODUCT_PROPERTY_OVERRIDES += \
	ro.vendor.qti.config.zram=true
