#!/bin/bash

echo "Preparing checkout of u-boot v2020.10 without history..."
git clone --depth 1 --single-branch --branch v2020.10 https://github.com/u-boot/u-boot.git

echo "Preparing checkout of linux v6.18 LTS without history..."
rm -rf linux
mkdir -p linux
git clone --depth 1 --single-branch --branch v6.18.25 https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
cd linux
patch -p1 < ../files/kernel_patches/901-debloat_sock_diag.patch
patch -p1 < ../files/kernel_patches/902-debloat_proc.patch
patch -p1 < ../files/kernel_patches/904-debloat_dma_buf.patch
cd ..

git clone --depth 1 --single-branch https://git.busybox.net/busybox software/busybox
git clone --depth 1 --single-branch https://github.com/mkj/dropbear.git software/dropbear
git clone --depth 1 --single-branch https://github.com/OpenMixerProject/AES50.git fpga/AES50
git clone --depth 1 --single-branch https://github.com/xn--nding-jua/ethernet_mac fpga/ethernet_mac
git clone --depth 1 --single-branch https://github.com/ponty/framebuffer-vncserver.git software/framebuffer-vncserver
git clone --depth 1 --single-branch https://github.com/LibVNC/libvncserver.git software/libvncserver
git clone --depth 1 --single-branch https://github.com/OpenLightingProject/libartnet.git software/libartnet

git clone --depth 1 --single-branch https://github.com/OpenMixerProject/OpenMixerControl.git software/omc

echo "Done."
