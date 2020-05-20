#! /vendor/bin/sh

# Copyright (c) 2012-2013, 2016-2019, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of The Linux Foundation nor
#       the names of its contributors may be used to endorse or promote
#       products derived from this software without specific prior written
#       permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
# OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
# ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

# Post boot configuration script targeted at sdm660/sdm636

function configure_zram_parameters() {
    MemTotalStr=`cat /proc/meminfo | grep MemTotal`
    MemTotal=${MemTotalStr:16:8}
    
    # Zram disk - 75% for Go devices.
    # For 512MB Go device, size = 384MB, set same for Non-Go.
    # For 1GB Go device, size = 768MB, set same for Non-Go.
    # For 2GB Go device, size = 1536MB, set same for Non-Go.
    # For >2GB Non-Go devices, size = 50% of RAM size. Limit the size to 4GB.
    # And enable lz4 zram compression for Go targets.

    RamSizeGB=`echo "($MemTotal / 1048576 ) + 1" | bc`
    if [ $RamSizeGB -le 2 ]; then
        zRamSizeBytes=`echo "$RamSizeGB * 1024 * 1024 * 1024 * 3 / 4" | bc`
        zRamSizeMB=`echo "$RamSizeGB * 1024 * 3 / 4" | bc`
    else
        zRamSizeBytes=`echo "$RamSizeGB * 1024 * 1024 * 1024 / 2" | bc`
        zRamSizeMB=`echo "$RamSizeGB * 1024 / 2" | bc`
    fi

    # use MB avoid 32 bit overflow
    if [ $zRamSizeMB -gt 4096 ]; then
        zRamSizeBytes=4294967296
    fi

    echo lz4 > /sys/block/zram0/comp_algorithm

    if [ -f /sys/block/zram0/disksize ]; then
        if [ -f /sys/block/zram0/use_dedup ]; then
            echo 1 > /sys/block/zram0/use_dedup
        fi
        echo $zRamSizeBytes > /sys/block/zram0/disksize

        # ZRAM may use more memory than it saves if SLAB_STORE_USER
        # debug option is enabled.
        if [ -e /sys/kernel/slab/zs_handle ]; then
            echo 0 > /sys/kernel/slab/zs_handle/store_user
        fi
        if [ -e /sys/kernel/slab/zspage ]; then
            echo 0 > /sys/kernel/slab/zspage/store_user
        fi

        mkswap /dev/block/zram0
        swapon /dev/block/zram0 -p 32758
    fi
}

configure_zram_parameters

if [ -f /sys/devices/soc0/soc_id ]; then
        soc_id=`cat /sys/devices/soc0/soc_id`
else
        soc_id=`cat /sys/devices/system/soc/soc0/id`
fi

if [ -f /sys/devices/soc0/hw_platform ]; then
        hw_platform=`cat /sys/devices/soc0/hw_platform`
else
        hw_platform=`cat /sys/devices/system/soc/soc0/hw_platform`
fi

panel=`cat /sys/class/graphics/fb0/modes`
if [ "${panel:5:1}" == "x" ]; then
    panel=${panel:2:3}
else
    panel=${panel:2:4}
fi

if [ $panel -gt 1080 ]; then
    echo 2 > /proc/sys/kernel/sched_window_stats_policy
    echo 5 > /proc/sys/kernel/sched_ravg_hist_size
else
    echo 3 > /proc/sys/kernel/sched_window_stats_policy
    echo 3 > /proc/sys/kernel/sched_ravg_hist_size
fi


# Disable wsf for all targets beacause we are using efk.
# wsf Range : 1..1000 So set to bare minimum value 1.
echo 1 > /proc/sys/vm/watermark_scale_factor


# Start Host based Touch processing
case "$hw_platform" in
        "MTP" | "Surf" | "RCM" | "QRD" )
        # Start the Host based Touch processing but not in the power off mode.
        bootmode=`getprop ro.bootmode`
        if [ "charger" != $bootmode ]; then
            start vendor.hbtp
        fi
        ;;
esac

#Apply settings for sdm660 only
case "$soc_id" in
        "317" | "324" | "325" | "326"  )
        # Start cdsprpcd only for sdm660 and disable for sdm630 and sdm636
        start vendor.cdsprpcd
        ;;
esac

# Post-setup services
setprop vendor.post_boot.parsed 1

# Let kernel know our image version/variant/crm_version
if [ -f /sys/devices/soc0/select_image ]; then
    image_version="10:"
    image_version+=`getprop ro.build.id`
    image_version+=":"
    image_version+=`getprop ro.build.version.incremental`
    image_variant=`getprop ro.product.name`
    image_variant+="-"
    image_variant+=`getprop ro.build.type`
    oem_version=`getprop ro.build.version.codename`
    echo 10 > /sys/devices/soc0/select_image
    echo $image_version > /sys/devices/soc0/image_version
    echo $image_variant > /sys/devices/soc0/image_variant
    echo $oem_version > /sys/devices/soc0/image_crm_version
fi

# Change console log level as per console config property
console_config=`getprop persist.console.silent.config`
case "$console_config" in
    "1")
        echo "Enable console config to $console_config"
        echo 0 > /proc/sys/kernel/printk
        ;;
    *)
        echo "Enable console config to $console_config"
        ;;
esac

# Parse misc partition path and set property
misc_link=$(ls -l /dev/block/bootdevice/by-name/misc)
real_path=${misc_link##*>}
setprop persist.vendor.mmi.misc_dev_path $real_path

# set sys.use_fifo_ui prop if eas exist
	available_governors=$(cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_available_governors)

	if echo "$available_governors" | grep schedutil; then
	  setprop sys.use_fifo_ui 1
	fi
