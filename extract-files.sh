#!/bin/bash
#
# Copyright (C) 2020 Paranoid Android
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

set -e

DEVICE_COMMON=sdm660-common
VENDOR=xiaomi

# Load extract_utils and do some sanity checks
COMMON_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "$COMMON_DIR" ]]; then COMMON_DIR="$PWD"; fi

if [[ -z "$DEVICE_DIR" ]]; then
    DEVICE_DIR="${COMMON_DIR}/../${DEVICE}"
fi

ROOT="$COMMON_DIR"/../../..

HELPER="$ROOT"/tools/extract-utils/extract_utils.sh
if [ ! -f "$HELPER" ]; then
    echo "Unable to find helper script at $HELPER"
    exit 1
fi
. "$HELPER"

# Default to sanitizing the vendor folder before extraction
CLEAN_VENDOR=true
ONLY_COMMON=false
ONLY_DEVICE=false

while [ "${#}" -gt 0 ]; do
    case "${1}" in
        -o | --only-common )
                ONLY_COMMON=true
                ;;
        -d | --only-device )
                ONLY_DEVICE=true
                ;;
        -n | --no-cleanup )
                CLEAN_VENDOR=false
                ;;
        -k | --kang )
                KANG="--kang"
                ;;
        -s | --section )
                SECTION="${2}"; shift
                CLEAN_VENDOR=false
                ;;
        * )
                SRC="${1}"
                ;;
    esac
    shift
done

if [ -z "$SRC" ]; then
    SRC=adb
fi

function blob_fixup() {
    case "${1}" in

    system_ext/etc/init/dpmd.rc)
        sed -i "s|/system/product/bin/|/system/system_ext/bin/|g" "${2}"
        ;;
    
    system_ext/etc/permissions/com.qualcomm.qti.imscmservice-V2.0-java.xml | system_ext/etc/permissions/com.qualcomm.qti.imscmservice-V2.1-java.xml | system_ext/etc/permissions/com.qualcomm.qti.imscmservice-V2.2-java.xml)
        sed -i 's|product|system_ext|g' "${2}"
        ;;
        
    system_ext/etc/permissions/com.qti.dpmframework.xml | system_ext/etc/permissions/com.qualcomm.qti.imscmservice.xml | system_ext/etc/permissions/dpmapi.xml | system_ext/etc/permissions/telephonyservice.xml)
        sed -i "s|/system/product/framework/|/system/system_ext/framework/|g" "${2}"
        ;;
        
    system_ext/etc/permissions/qcrilhook.xml)
        sed -i "s|/product/framework/qcrilhook.jar|/system_ext/framework/qcrilhook.jar|g" "${2}"
        ;;
        
    system_ext/etc/permissions/vendor.qti.hardware.data.connection-V1.0-java.xml | system_ext/etc/permissions/vendor.qti.hardware.data.connection-V1.1-java.xml)
        sed -i 's/xml version="2.0"/xml version="1.0"/' "${2}"
        sed -i "s|product|system_ext|g" "${2}"
        ;;
        
    system_ext/lib64/libdpmframework.so)
        "${PATCHELF}" --add-needed libcutils_shim.so "${2}"
        ;;

    vendor/bin/mlipayd@1.1)
        "${PATCHELF}" --remove-needed vendor.xiaomi.hardware.mtdservice@1.0.so "${2}"
        ;;
    
    vendor/lib64/libmlipay.so | vendor/lib64/libmlipay@1.1.so)
        "${PATCHELF}" --remove-needed vendor.xiaomi.hardware.mtdservice@1.0.so "${2}"
        sed -i "s|/system/etc/firmware|/vendor/firmware\x0\x0\x0\x0|g" "${2}"
        ;;

    vendor/lib/hw/camera.sdm660.so)
        "${PATCHELF}" --add-needed camera.sdm660_shim.so "${2}"
        ;;

    vendor/lib64/libril-qc-hal-qmi.so)
        "${PATCHELF}" --replace-needed "libprotobuf-cpp-full.so" "libprotobuf-cpp-full-v29.so" "${2}"
        ;;

    vendor/lib64/libwvhidl.so)
        "${PATCHELF}" --replace-needed "libprotobuf-cpp-lite.so" "libprotobuf-cpp-lite-v29.so" "${2}"
        ;;

    esac

    device_blob_fixup "$@"
}

if ! typeset -f device_blob_fixup > /dev/null; then
    device_blob_fixup() {
        :
    }
fi

# Initialize the common helper
setup_vendor "$DEVICE_COMMON" "$VENDOR" "$ROOT" true $CLEAN_VENDOR

if [[ "$ONLY_DEVICE" = "false" ]] && [[ -s "${COMMON_DIR}"/proprietary-files.txt ]]; then
    extract "$COMMON_DIR"/proprietary-files.txt "$SRC" "${KANG}" --section "${SECTION}"
fi
if [[ "$ONLY_COMMON" = "false" ]] && [[ -s "${DEVICE_DIR}"/proprietary-files.txt ]]; then
    if [[ ! "$IS_COMMON" = "true" ]]; then
        IS_COMMON=false
    fi
    # Reinitialize the helper for device
    setup_vendor "$DEVICE" "$VENDOR" "$ROOT" "$IS_COMMON" "$CLEAN_VENDOR"
    extract "${DEVICE_DIR}"/proprietary-files.txt "$SRC" "${KANG}" --section "${SECTION}"
fi

"$COMMON_DIR"/setup-makefiles.sh
