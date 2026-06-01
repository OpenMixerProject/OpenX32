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

echo "Checking out all submodules..."
git submodule update --init --recursive --progress --remote
echo "Done."
