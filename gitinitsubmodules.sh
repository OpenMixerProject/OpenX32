#!/bin/bash
echo "Preparing checkout of u-boot v2020.10 without history..."
cd u-boot
git init
git remote add origin https://github.com/u-boot/u-boot.git
git fetch --depth 1 origin tag v2020.10
git checkout v2020.10
cd ..

echo "Preparing checkout of linux v6.12 LTS without history..."
cd linux
git init
git remote add origin https://github.com/torvalds/linux.git
git fetch --depth 1 origin tag v6.12
git checkout v6.12
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

echo "pyATK, busybox, openssh and other submodules will be cloned with full history..."
echo "Checking out all remaining submodules..."
git submodule update --init --recursive

echo "Done."
