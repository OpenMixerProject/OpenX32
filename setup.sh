#!/bin/bash

# general tools to compile linux, busybox, u-Boot and the x32ctrl software
# ========================================================================
echo "This script will setup a Debian-based environment to allow compilation of the necessary components..."
echo "Installing packages..."
sudo apt update
sudo apt install libncurses-dev gawk flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf llvm fakeroot build-essential devscripts gcc-arm-none-eabi binutils-arm-none-eabi gcc-arm-linux-gnueabi g++-arm-linux-gnueabi binutils-arm-linux-gnueabi u-boot-tools bc cpio dropbear-bin

# extract toolchain to directory /opt/cross
sudo tar -xf toolchains/cross-arm-arm926ej.tar.xz -C /

# download, compile and install libz into /opt/cross
cd /tmp
wget https://zlib.net/zlib-1.3.2.tar.gz
tar -xf zlib-1.3.2.tar.gz
rm zlib-1.3.2.tar.gz
cd zlib-1.3.2
export PATH=/opt/cross/bin:$PATH
export CHOST=arm-openwrt-linux-muslgnueabi
export CC=${CHOST}-gcc
export AR=${CHOST}-ar
export RANLIB=${CHOST}-ranlib
# install in the toolchain-folder
./configure --prefix=/opt/cross/arm-openwrt-linux-muslgnueabi --shared
make
sudo make install




# install individual libraries for some tools
# ========================================================================
#echo "Installing libraries for ARM..."
sudo dpkg --add-architecture armel
sudo apt update

# libz required for vnc-server
# libudev required for automount and usb-DMX512-interfaces
sudo apt install zlib1g-dev:armel libz-dev:armel libudev-dev:armel

# libasound2 required for audio-output
#sudo apt install libasound2:armel



# install Python 3 and pyATK to upload code to the i.MX25 via the USB connection
# ==============================================================================
#echo "Install packages for pyATK"
#sudo apt install python3 python3-pip python3.11-venv
#echo "Configuring pyATK..."
#cp files/usbdev.py pyatk/pyatk/channel/
#cp files/boot.py pyatk/pyatk/
#cd software/pyatk
#python3 -m venv pyatk_venv
#source pyatk_venv/bin/activate
#pip install pyserial pyusb==1.0.0
#pip install setuptools
#sudo pyatk_venv/bin/python3 setup.py install
#deactivate
#cd ..
