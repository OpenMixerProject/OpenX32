#!/bin/bash
echo "Preparing checkout of u-boot v2020.10 without history..."
cd u-boot
git init
git remote add origin https://github.com/u-boot/u-boot.git
git fetch --depth 1 origin tag v2020.10
git checkout v2020.10
cd ..

echo "Preparing checkout of linux v6.18 LTS without history..."
rm -rf linux
mkdir -p linux
cd linux
git init
git remote add origin https://git.kernel.org/pub/scm/linux/kernel/git/stable/linux.git
git fetch --depth 1 origin tag v6.18.22
git checkout v6.18.22
patch -p1 < ../files/kernel_patches/901-debloat_sock_diag.patch
patch -p1 < ../files/kernel_patches/902-debloat_proc.patch
patch -p1 < ../files/kernel_patches/904-debloat_dma_buf.patch
cd ..

echo "Preparing checkout of LVGL-Linux-Port and LVGL without history..."
cd software/x32ctrl/lv_port_linux
git init
git remote add origin https://github.com/lvgl/lv_port_linux.git
git fetch --depth 1 origin master
git checkout master
cd lvgl
git init
git remote add origin https://github.com/lvgl/lvgl.git
git fetch --depth 1 origin master
git checkout master
cd ../../../..

echo "pyATK, busybox, dropbear and other submodules will be cloned with full history..."
echo "Checking out all remaining submodules..."
git submodule update --init --recursive

echo "Done."
