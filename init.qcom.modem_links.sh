#!/system/bin/sh
# Copyright (c) 2013, The Linux Foundation. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above
#       copyright notice, this list of conditions and the following
#       disclaimer in the documentation and/or other materials provided
#       with the distribution.
#     * Neither the name of The Linux Foundation nor the names of its
#       contributors may be used to endorse or promote products derived
#       from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
# ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
# BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
# WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
# IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#

# No path is set up at this point so we have to do it here.
PATH=/sbin:/system/sbin:/system/bin:/system/xbin
export PATH

# Check for images and set up symlinks
cd /firmware/image

# Get the list of files in /firmware/image
# for which sym links have to be created

fwfiles=`ls modem* adsp* wcnss* mba*`

# Check if the links with similar names
# have been created in /system/etc/firmware

cd /system/etc/firmware
linksNeeded=0

# For everyfile in fwfiles check if
# the corresponding file exists
for fwfile in $fwfiles; do

   # if (condition) does not seem to work
   # with the android shell. Therefore
   # make do with case statements instead.
   # if a file named $fwfile is present
   # no need to create links. If the file
   # with the name $fwfile is not present
   # need to create links.

   case `ls $fwfile` in
      $fwfile)
         continue;;
      *)
         # file with $fwfile does not exist
         # need to create links
         linksNeeded=1
         break;;
   esac

done

# symlinks created for qca6714 firmware
ln -s /firmware/image/athwlan.bin /system/etc/firmware/athwlan.bin
ln -s /firmware/image/fakeboar.bin /system/etc/firmware/fakeBoardData_AR6004.bin
ln -s /firmware/image/otp.bin /system/etc/firmware/otp.bin
ln -s /firmware/image/utf.bin /system/etc/firmware/utf.bin

case $linksNeeded in
   1)
      cd /firmware/image

      case `ls modem.mdt 2>/dev/null` in
         modem.mdt)
            for imgfile in modem*; do
               ln -s /firmware/image/$imgfile /system/etc/firmware/$imgfile 2>/dev/null
            done
            ;;
        *)
            # trying to log here but nothing will be logged since it is
            # early in the boot process. Is there a way to log this message?
            log -p w -t PIL no modem image found;;
      esac

      case `ls adsp.mdt 2>/dev/null` in
         adsp.mdt)
            for imgfile in adsp*; do
               ln -s /firmware/image/$imgfile /system/etc/firmware/$imgfile 2>/dev/null
            done
            ;;
         *)
            log -p w -t PIL no adsp image found;;
      esac

      case `ls wcnss.mdt 2>/dev/null` in
         wcnss.mdt)
            for imgfile in wcnss*; do
               ln -s /firmware/image/$imgfile /system/etc/firmware/$imgfile 2>/dev/null
            done
            ;;
         *)
            log -p w -t PIL no wcnss image found;;
      esac

      case `ls mba.mdt 2>/dev/null` in
         mba.mdt)
            for imgfile in mba*; do
               ln -s /firmware/image/$imgfile /system/etc/firmware/$imgfile 2>/dev/null
            done
            ;;
         *)
            log -p w -t PIL no mba image found;;
      esac

      ;;

   *)
      # Nothing to do. No links needed
      ;;
esac

cd /
