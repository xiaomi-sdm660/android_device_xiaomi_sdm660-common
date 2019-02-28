#! /vendor/bin/sh

# Copyright (c) 2013-2014, The Linux Foundation. All rights reserved.
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

#
# start ril-daemon only for targets on which radio is present
#
baseband=`getprop ro.baseband`
sgltecsfb=`getprop persist.vendor.radio.sglte_csfb`
datamode=`getprop persist.vendor.data.mode`
rild_status=`getprop init.svc.ril-daemon`
vendor_rild_status=`getprop init.svc.vendor.ril-daemon`

case "$baseband" in
    "apq" | "sda" | "qcs" )
    setprop ro.vendor.radio.noril yes
    setprop ro.radio.noril yes
    setprop hw.nophone yes
    start vendor.ipacm
    if [ -n "$rild_status" ] || [ -n "$vendor_rild_status" ]; then
    stop ril-daemon
    stop vendor.ril-daemon
    stop vendor.qcrild
    stop vendor.qcrild2
    fi
esac

case "$baseband" in
    "msm" | "csfb" | "svlte2a" | "mdm" | "mdm2" | "sglte" | "sglte2" | "dsda2" | "unknown" | "dsda3" | "sdm" | "sdx" | "sm6")
    # Get ril-daemon status again to ensure that we have latest info
    rild_status=`getprop init.svc.ril-daemon`
    vendor_rild_status=`getprop init.svc.vendor.ril-daemon`

    if [[ -z "$rild_status" || "$rild_status" = "stopped" ]] && [[ -z "$vendor_rild_status" || "$vendor_rild_status" = "stopped" ]]; then
    start vendor.qcrild
    fi
    start vendor.ipacm

    multisim=`getprop persist.radio.multisim.config`

    if [ "$multisim" = "dsds" ] || [ "$multisim" = "dsda" ]; then
        if [[ -z "$rild_status" || "$rild_status" = "stopped" ]] && [[ -z "$vendor_rild_status" || "$vendor_rild_status" = "stopped" ]]; then
        start vendor.qcrild2
        else
        start vendor.ril-daemon2
        fi
    elif [ "$multisim" = "tsts" ]; then
        if [[ -z "$rild_status" || "$rild_status" = "stopped" ]] && [[ -z "$vendor_rild_status" || "$vendor_rild_status" = "stopped" ]]; then
        start vendor.qcrild2
        start vendor.qcrild3
        else
        start vendor.ril-daemon2
        fi
    fi

    case "$datamode" in
        "tethered")
        start vendor.dataqti
        start vendor.port-bridge
            ;;
        "concurrent")
        start vendor.dataqti
        start vendor.netmgrd
        start vendor.port-bridge
            ;;
        *)
        start vendor.netmgrd
            ;;
    esac
esac

#
# Allow persistent faking of bms
# User needs to set fake bms charge in persist.bms.fake_batt_capacity
#
fake_batt_capacity=`getprop persist.bms.fake_batt_capacity`
case "$fake_batt_capacity" in
    "") ;; #Do nothing here
    * )
    echo "$fake_batt_capacity" > /sys/class/power_supply/battery/capacity
    ;;
esac
