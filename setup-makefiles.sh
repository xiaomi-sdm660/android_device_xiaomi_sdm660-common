#!/bin/bash
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

set -e

DEVICE_COMMON=sdm660-common
VENDOR=xiaomi

# Load extract_utils and do some sanity checks
MY_DIR="${BASH_SOURCE%/*}"
if [[ ! -d "${MY_DIR}" ]]; then MY_DIR="${PWD}"; fi

ANDROID_ROOT="${MY_DIR}/../../.."

HELPER="${ANDROID_ROOT}/tools/extract-utils/extract_utils.sh"
if [ ! -f "${HELPER}" ]; then
    echo "Unable to find helper script at ${HELPER}"
    exit 1
fi
source "${HELPER}"

# Initialize the helper for common
setup_vendor "${DEVICE_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true

# Warning headers and guards
write_headers "clover jasmine_sprout jason lavender platina tulip wayne whyred"

# The standard common blobs
write_makefiles "${MY_DIR}/proprietary-files.txt" true

printf "\n%s\n" "ifeq (\$(BOARD_HAVE_QCOM_FM),true)" >> "${PRODUCTMK}"
write_makefiles "${MY_DIR}/proprietary-files-fm.txt" true
echo "endif" >> "${PRODUCTMK}"

# Finish
write_footers

if [ -s "${MY_DIR}/../$DEVICE_SPECIFIED_COMMON/proprietary-files.txt" ]; then
    DEVICE_COMMON="${DEVICE_SPECIFIED_COMMON}"

    # Reinitialize the helper for device specified common
    setup_vendor "${DEVICE_SPECIFIED_COMMON}" "${VENDOR}" "${ANDROID_ROOT}" true

    # Warning headers and guards
    write_headers "${DEVICE_SPECIFIED_COMMON_DEVICE}"

    # The standard device specified common blobs
    write_makefiles "${MY_DIR}/../${DEVICE_SPECIFIED_COMMON}/proprietary-files.txt" true

    # Finish
    write_footers

    DEVICE_COMMON="sdm660-common"
fi

if [ -s "${MY_DIR}/../${DEVICE}/proprietary-files.txt" ]; then
    # Reinitialize the helper for device
    setup_vendor "${DEVICE}" "${VENDOR}" "${ANDROID_ROOT}" false

    # Warning headers and guards
    write_headers

    # The standard device blobs
    write_makefiles "${MY_DIR}/../${DEVICE}/proprietary-files.txt" true

    # Finish
    write_footers
fi
