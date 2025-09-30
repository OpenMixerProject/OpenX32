#!/bin/bash
echo "Deleting temporary files..."
rm /tmp/initramfs.cpio.gz
rm /tmp/uramdisk.bin
rm /tmp/openx32.bin

echo "Copy initramfs-folder into single file..."
cd initramfs_root
find . -print0 | cpio --null -ov --format=newc > /tmp/initramfs.cpio
gzip -9 /tmp/initramfs.cpio
echo "Creating U-Boot-Image..."
mkimage -A ARM -O linux -T ramdisk -C gzip -a 0 -e 0 -n "Ramdisk Image" -d /tmp/initramfs.cpio.gz /tmp/uramdisk.bin

# =================== Binary-Blob =======================

echo "Merging Miniloader, U-Boot, Linux kernel and DeviceTreeBlob..."
cd ..
# Miniloader at offset 0x000000: will be started by i.MX Serial Download Program
echo "     0% Copying Miniloader..."
dd if=miniloader/miniloader.bin of=/tmp/openx32.bin bs=1 conv=notrunc > /dev/null 2>&1
# U-Boot at offset 0x0000C0: will be started by Miniloader
echo "    20% Copying U-Boot..."
dd if=u-boot/u-boot.bin of=/tmp/openx32.bin bs=1 seek=$((0xC0)) conv=notrunc > /dev/null 2>&1
# Linux-Kernel at offset 0x060000 (384 kiB for Miniloader + U-Boot): will be started by U-Boot
echo "    40% Copying Linux-Kernel...."
dd if=/tmp/uImage of=/tmp/openx32.bin bs=1 seek=$((0x60000)) conv=notrunc > /dev/null 2>&1
# DeviceTreeBlob at offset 0x800000 (~8 MiB for Kernel)
echo "    60% Copying DeviceTreeBlob..."
dd if=linux/arch/arm/boot/dts/nxp/imx/imx25-pdk.dtb of=/tmp/openx32.bin bs=1 seek=$((0x800000)) conv=notrunc > /dev/null 2>&1
# InitramFS at offset 0x810000 (~64kiB for DeviceTreeBlob)
echo "    80% Copying initramfs..."
dd if=/tmp/uramdisk.bin of=/tmp/openx32.bin bs=1 seek=$((0x810000)) conv=notrunc > /dev/null 2>&1
echo "   100% Add some zeros at the end of the binary-file..."
dd if=/dev/zero of=/tmp/openx32.bin bs=1 count=100 oflag=append conv=notrunc > /dev/null 2>&1

# =================== DCP-Loader-File =======================

echo "9/9 Creating DCP-Updater-File..."
perl software/dcpapp/dcp_compiler.pl /tmp/openx32.bin:binary/dcpapp.bin /tmp/dcp_corefs_openx32.run

echo "Done. System-Image with Miniloader, u-Boot, Linux Kernel, Ramdisk and DeviceTreeBlob is stored as /tmp/openx32.bin"


