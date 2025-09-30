#!/bin/bash

# This script will copy the binaries into the folder "USB". You can copy the content of this folder
# into the root-directory of an empty USB-Thumbdrive to use it as a harddrive in Linux.
#
# 1. After booting the kernel, mount the drive:
# mount /dev/sda1 /mnt/usb
# 2. Then all the startup-script
# /mnt/usb/scripts/startall.sh
#
# This will startup some helpful tools like SSHD
#
# If OpenX32 might get bigger and we can boot from a USB-Thumbdrive, this might not be necessary any more
#
#

# first create the necessary folders
mkdir usb/usr
mkdir usb/usr/local
mkdir usb/usr/local/libexec
mkdir usb/bin
mkdir usb/sbin
mkdir usb/lib

# copy important libraries (they are placed in initramfs at the moment)
#cp /usr/lib/arm-linux-gnueabi/libc.so.6 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libgcc_s.so.1 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libstdc++.so.6 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libm.so.6 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/ld-linux.so.3 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libgpiod.so.2 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libasound.so.2 usb/lib/
#cp /usr/lib/arm-linux-gnueabi/libz.so.1 usb/lib/

# for QEMU and SDL
#cp /usr/lib/arm-linux-gnueabi/libSDL2.so usb/lib/

#TODO: create symlinks to the correct versions during compilation

# sshd
cp software/bin/sshd usb/sbin/
cp software/bin/ssh-keygen usb/bin/
cp software/bin/libexec/sshd-auth usb/usr/local/libexec/
cp software/bin/libexec/sshd-session usb/usr/local/libexec/

# X32 user applications
cp software/bin/x32ctrl usb/bin/
