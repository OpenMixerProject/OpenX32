#!/bin/bash

echo "0/8 Copying configuration- and patched files for OpenX32 to U-Boot- and Linux-Sources..."
# configuration-files
cp files/config_uboot u-boot/.config
cp files/config_linux linux/.config
cp files/config_busybox busybox/.config
cp files/meminit.txt pyatk/bin/
# patched source-files
cp files/imximage.cfg u-boot/board/freescale/mx25pdk/imximage.cfg
cp files/mx25pdk.c u-boot/board/freescale/mx25pdk/mx25pdk.c
cp files/mx25pdk.h u-boot/include/configs/mx25pdk.h
cp files/imx25-pdk.dts linux/arch/arm/boot/dts/nxp/imx/imx25-pdk.dts

# prepare Barebox
cp -rP files/barebox_arch_arm_boards_openx32/*  barebox/arch/arm/boards/openx23/
cp files/barebox_arch_arm_boards_Makefile       barebox/arch/arm/boards/Makefile
cp files/barebox_arc_arm_mach-imx_Kconfig       barebox/arch/arm/mach-imx/
cp files/imx25-pdk.dts                          barebox/arch/arm/dts/imx25-openx32.dts
cp files/barebox_arc_arm_dts_Makefile           barebox/arch/arm/dts/Makefile
cp files/barebox_images_Makefile.imx            barebox/images/Makefile.imx
cp files/config_barebox                         barebox/.config
#cp files/imximage.cfg                           

# =================== Loader =======================

echo "1/8 Compiling Miniloader..."
cd miniloader
make
echo "2/8 Compiling u-boot..."
cd ../u-boot
ARCH=arm CROSS_COMPILE=/usr/bin/arm-none-eabi- make
echo "2/8 Compiling barebox..."
cd ../barebox
ARCH=arm CROSS_COMPILE=/usr/bin/arm-none-eabi- make

# =================== Linux =======================

#echo "3/8 Compiling linux..."
#cd ../linux
#ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make zImage > /dev/null
#ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make dtbs
#echo "4/8 Creating U-Boot-Image..."
#mkimage -A ARM -O linux -T kernel -C none -a 0x80060000 -e 0x80060000 -n "Linux kernel (OpenX32)" -d arch/arm/boot/zImage /tmp/uImage

# =================== Programs =======================

#echo "5/8 Compiling busybox..."
#cd ../busybox
#ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make -j$(nproc) > /dev/null
#ARCH=arm make install > /dev/null
#cd ..

#echo "6/8 Creating initramfs..."
#cp -rP /tmp/busybox_install/bin initramfs_root/
#cp -rP /tmp/busybox_install/sbin initramfs_root/
#cp -rP /tmp/busybox_install/linuxrc initramfs_root/

#cd software/x32sdconfig
#./compile.sh
#cd ../../
#cp software/bin/x32sdconfig initramfs_root/bin/

#cd initramfs_root
#mkdir -p dev proc sys etc mnt home usr
#rm /tmp/initramfs.cpio.gz
#rm /tmp/uramdisk.bin
#find . -print0 | cpio --null -ov --format=newc > /tmp/initramfs.cpio
#gzip -9 /tmp/initramfs.cpio
#echo "7/8 Creating U-Boot-Image..."
#mkimage -A ARM -O linux -T ramdisk -C gzip -a 0 -e 0 -n "Ramdisk Image" -d /tmp/initramfs.cpio.gz /tmp/uramdisk.bin

# =================== Binary-Blob =======================

echo "8/8 Merging Miniloader, U-Boot, Linux kernel and DeviceTreeBlob..."
cd ..
rm /tmp/openx32.bin
# Miniloader at offset 0x000000: will be started by i.MX Serial Download Program
echo "     0% Copying Miniloader..."
dd if=miniloader/miniloader.bin of=/tmp/openx32.bin bs=1 conv=notrunc > /dev/null 2>&1
# U-Boot at offset 0x0000C0: will be started by Miniloader
echo "    20% Copying U-Boot..."
dd if=u-boot/u-boot.bin of=/tmp/openx32.bin bs=1 seek=$((0xC0)) conv=notrunc > /dev/null 2>&1
# Linux-Kernel at offset 0x060000 (384 kiB for Miniloader + U-Boot): will be started by U-Boot
#echo "    40% Copying Linux-Kernel...."
echo "    41% copy barebox..."
#dd if=/tmp/uImage of=/tmp/openx32.bin bs=1 seek=$((0x60000)) conv=notrunc > /dev/null 2>&1
dd if=barebox/images/barebox-dt-2nd.img of=/tmp/openx32.bin bs=1 seek=$((0x60000)) conv=notrunc
dd if=barebox/arch/arm/dts/versatile-pb.dtb of=/tmp/openx32.bin bs=1 seek=$((0x100000)) conv=notrunc
# DeviceTreeBlob at offset 0x800000 (~8 MiB for Kernel)
#echo "    60% Copying DeviceTreeBlob..."
#dd if=linux/arch/arm/boot/dts/nxp/imx/imx25-pdk.dtb of=/tmp/openx32.bin bs=1 seek=$((0x800000)) conv=notrunc > /dev/null 2>&1
# InitramFS at offset 0x810000 (~64kiB for DeviceTreeBlob)
#echo "    80% Copying initramfs..."
#dd if=/tmp/uramdisk.bin of=/tmp/openx32.bin bs=1 seek=$((0x810000)) conv=notrunc > /dev/null 2>&1
echo "   100% Add some zeros at the end of the binary-file..."
dd if=/dev/zero of=/tmp/openx32.bin bs=1 count=100 oflag=append conv=notrunc > /dev/null 2>&1

echo "Done. System-Image with Miniloader, u-Boot, Linux Kernel, Ramdisk and DeviceTreeBlob is stored as /tmp/openx32.bin"
