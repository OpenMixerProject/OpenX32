#!/bin/bash
arm-linux-gnueabi-gcc -mcpu=arm926ej-s -Os -fno-caller-saves -pipe -funit-at-a-time -msoft-float -fno-plt -fno-unwind-tables -fno-asynchronous-unwind-tables -I . *.c -o ../../initramfs_root/openx32/x32sdconfig
