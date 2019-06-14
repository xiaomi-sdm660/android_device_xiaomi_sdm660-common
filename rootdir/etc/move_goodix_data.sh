#!/system/bin/sh
#
# Copyright (C) 2019 The LineageOS Project
#
# SPDX-License-Identifier: Apache-2.0
#

DEST_PATH="/data/vendor/goodix"
FILES_MOVED="/data/vendor/goodix/moved"
SRC_PATH="/data/misc/goodix"

if [ ! -f "$FILES_MOVED" ]; then
  for i in "$SRC_PATH/"*; do
    dest_path=$DEST_PATH/"${i#$SRC_PATH/}"
    mv $i "$dest_path"
  done
  restorecon -R "$DEST_PATH"
  echo 1 > "$FILES_MOVED"
fi
