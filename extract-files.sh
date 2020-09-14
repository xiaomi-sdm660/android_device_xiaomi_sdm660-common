#!/bin/bash
#
# Copyright (C) 2018 The LineageOS Project
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

HELPER="$ROOT"/vendor/carbon/build/tools/extract_utils.sh
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

    vendor/bin/mlipayd@1.1)
        patchelf --remove-needed vendor.xiaomi.hardware.mtdservice@1.0.so "${2}"
        ;;

    vendor/lib64/libmlipay.so | vendor/lib64/libmlipay@1.1.so)
        patchelf --remove-needed vendor.xiaomi.hardware.mtdservice@1.0.so "${2}"
        sed -i "s|/system/etc/firmware|/vendor/firmware\x0\x0\x0\x0|g" "${2}"
        ;;

    vendor/lib/hw/camera.sdm660.so)
        patchelf --add-needed camera.sdm660_shim.so "${2}"
        ;;

    vendor/lib64/libril-qc-hal-qmi.so)
        patchelf --replace-needed "libprotobuf-cpp-full.so" "libprotobuf-cpp-full-v29.so" "${2}"
        ;;

    vendor/lib64/libwvhidl.so)
        patchelf --replace-needed "libprotobuf-cpp-lite.so" "libprotobuf-cpp-lite-v29.so" "${2}"
        ;;
    product/etc/permissions/vendor.qti.hardware.data.connection-V1.{0,1}-java.xml)
        sed -i 's/xml version="2.0"/xml version="1.0"/' "${2}"

    esac
}

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
