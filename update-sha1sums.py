#!/usr/bin/env python
#
# Copyright (C) 2016 The CyanogenMod Project
# Copyright (C) 2017-2020 The LineageOS Project
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

import os
import sys
from hashlib import sha1

device = 'sdm660-common'
vendor = 'xiaomi'

with open('proprietary-files.txt', 'r') as f:
    lines = f.read().splitlines()
vendorPath = '../../../vendor/' + vendor + '/' + device + '/proprietary'
needSHA1 = False


class Section:
    def __init__(self, name, group):
        self.name = name
        self.group = group

    def __repr__(self):
        return repr(self.name)

    def apply(self):
        if self.group:
            lines.append(self.name)
            # Ignore leading '-' while sorting
            self.group = sorted(self.group, key=lambda line: line[1:] if line.startswith('-') else line)
            lines.extend(self.group)
            lines.append('')


def sort_file(ignore_section=""):
    grp = []
    sections = []
    name = ""

    for line in lines:
        if len(line) != 0:
            if line[0] == '#':
                if name and grp:
                    sections.append(Section(name, grp))
                    grp = []
                if ignore_section and ignore_section in line:
                    name = ""
                else:
                    name = line
            elif name:
                grp.append(line)

    if name and grp:
        sections.append(Section(name, grp))

    lines.clear()

    sections = sorted(sections, key=lambda sec: str.lower(sec.name))
    for section in sections:
        section.apply()


def cleanup():
    for index, line in enumerate(lines):
        # Skip empty or commented lines
        if len(line) == 0 or line[0] == '#' or '|' not in line:
            continue

        # Drop SHA1 hash, if existing
        lines[index] = line.split('|')[0]
    lines.append('')


def update():
    for index, line in enumerate(lines):
        # Skip empty lines
        if len(line) == 0:
            continue

        # Check if we need to set SHA1 hash for the next files
        if line[0] == '#':
            needSHA1 = (' - from' in line)
            continue

        if needSHA1:
            # Remove existing SHA1 hash
            line = line.split('|')[0]

            filePath = line.split(';')[0].split(':')[-1]
            if filePath[0] == '-':
                filePath = filePath[1:]

            with open(os.path.join(vendorPath, filePath), 'rb') as f:
                hash = sha1(f.read()).hexdigest()

            lines[index] = '%s|%s' % (line, hash)
    lines.append('')


if len(sys.argv) >= 2:
    if sys.argv[1] == '-c':
        cleanup()
    elif sys.argv[1] == '-s':
        if len(sys.argv) == 3:
            sort_file(sys.argv[2])
        else:
            sort_file()
    else:
        update()
else:
    update()

with open('proprietary-files.txt', 'w') as file:
    file.write('\n'.join(lines))
