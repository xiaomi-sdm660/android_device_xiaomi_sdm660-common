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
GUARDED_DEVICES_COMMON="tulip jasmine_sprout wayne clover lavender platina jason whyred"
VENDOR=xiaomi

INITIAL_COPYRIGHT_YEAR=2018

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

# Initialize the common helper
setup_vendor "$DEVICE_COMMON" "$VENDOR" "$ROOT" true

if ([[ "$ONLY_DEVICE" = "false" ]] || [[ -z "$ONLY_DEVICE" ]]) && [[ -s "${COMMON_DIR}"/proprietary-files.txt ]]; then
    # Copyright headers and guards
    write_headers "$GUARDED_DEVICES_COMMON"
    # The common blobs
    write_makefiles "$COMMON_DIR"/proprietary-files.txt true
    # Finish
    write_footers
fi
if ([[ "$ONLY_COMMON" = "false" ]] || [[ -z "$ONLY_COMMON" ]]) && [[ -s "${DEVICE_DIR}"/proprietary-files.txt ]]; then
    # Reinitialize the helper for device and write copyright headers and guards
    DEVICE_COMMON="$DEVICE"
    if [[ ! "$IS_COMMON" = "true" ]]; then
        IS_COMMON=false
        GUARDED_DEVICES=
    fi
    # Reinitialize the helper for device
    INITIAL_COPYRIGHT_YEAR="$DEVICE_BRINGUP_YEAR"
    setup_vendor "$DEVICE" "$VENDOR" "$ROOT" "$IS_COMMON" "$CLEAN_VENDOR"
    # Copyright headers and guards
    write_headers "$GUARDED_DEVICES"
    # The standard device blobs
    write_makefiles "${DEVICE_DIR}"/proprietary-files.txt true
    # We are done!
    write_footers
fi
