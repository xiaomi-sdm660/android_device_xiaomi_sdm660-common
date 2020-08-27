# Android fstab file.
# The filesystem that contains the filesystem checker binary (typically /system) cannot
# specify MF_CHECK, and must come before any filesystems that do specify MF_CHECK

# NOTE: /system and /vendor partitions are now early-mounted and the fstab entry is specified in device tree (duplicated below for recovery image purposes only):
#       /proc/device-tree/firmware/android/fstab/system
#       /proc/device-tree/firmware/android/fstab/vendor

#TODO: Add 'check' as fs_mgr_flags with data partition.
# Currently we dont have e2fsck compiled. So fs check would failed.

# Non-A/B fstab.qcom variant
#<src>                                                      <mnt_point>             <type>  <mnt_flags and options>                                                                             <fs_mgr_flags>
/dev/block/bootdevice/by-name/userdata                      /data                   ext4    nodev,noatime,nosuid,barrier=1,noauto_da_alloc,discard,lazytime                                     wait,check,fileencryption=ice,quota,reservedsize=128M
/dev/block/bootdevice/by-name/userdata                      /data                   f2fs    nodev,noatime,nosuid,nodiratime,fsync_mode=nobarrier,background_gc=off                              wait,check,fileencryption=ice,quota,reservedsize=128M
/devices/soc/c084000.sdhci/mmc_host*                        /storage/sdcard1        auto    nodev,nosuid                                                                                        wait,voldmanaged=sdcard1:auto,encryptable=footer
/dev/block/bootdevice/by-name/misc                          /misc                   emmc    defaults                                                                                            defaults
/dev/block/bootdevice/by-name/modem                         /vendor/firmware_mnt    vfat    ro,shortname=lower,uid=1000,gid=1000,dmask=227,fmask=337,context=u:object_r:firmware_file:s0        wait
/dev/block/bootdevice/by-name/bluetooth                     /vendor/bt_firmware     vfat    ro,shortname=lower,uid=1002,gid=3002,dmask=227,fmask=337,context=u:object_r:bt_firmware_file:s0     wait
/devices/soc/a800000.ssusb/a800000.dwc3/xhci-hcd.0.auto*    /storage/usbotg         auto    nodev,nosuid                                                                                        wait,voldmanaged=usbotg:auto
/dev/block/bootdevice/by-name/dsp                           /vendor/dsp             ext4    ro,nodev,nosuid,barrier=1                                                                           wait
/dev/block/bootdevice/by-name/cache                         /cache                  ext4    nodev,noatime,nosuid,barrier=1                                                                      wait
/dev/block/bootdevice/by-name/cache                         /cache                  f2fs    nodev,noatime,nosuid,nodiratime,discard,fsync_mode=nobarrier,inline_xattr,inline_data,data_flush    wait
/dev/block/bootdevice/by-name/persist                       /mnt/vendor/persist     ext4    nodev,noatime,nosuid,barrier=1                                                                      wait,check

/dev/block/bootdevice/by-name/system                        /system                 ext4    ro,barrier=1                                                                                        wait,recoveryonly
/dev/block/bootdevice/by-name/vendor                        /vendor                 ext4    ro,barrier=1                                                                                        wait,recoveryonly
/dev/block/bootdevice/by-name/boot                          /boot                   emmc    defaults                                                                                            defaults

