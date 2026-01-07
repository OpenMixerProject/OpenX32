#!/bin/bash

# general tools to compile linux, busybox, u-Boot and the x32ctrl software
# ========================================================================
echo "This script will setup a Debian-based environment to allow compilation of the necessary components..."
echo "Installing packages..."
sudo apt update
sudo apt install libncurses-dev gawk flex bison openssl libssl-dev dkms libelf-dev libudev-dev libpci-dev libiberty-dev autoconf llvm fakeroot build-essential devscripts gcc-arm-none-eabi binutils-arm-none-eabi gcc-arm-linux-gnueabi g++-arm-linux-gnueabi binutils-arm-linux-gnueabi u-boot-tools bc cpio


# install individual libraries for some tools (not used at the moment)
# ========================================================================
#echo "Installing libraries for ARM..."
#sudo dpkg --add-architecture armel
#sudo apt install libasound2:armel



# install Python 3 and pyATK to upload code to the i.MX25 via the USB connection
# ==============================================================================
#echo "Install packages for pyATK"
#sudo apt install python3 python3-pip python3.11-venv
#echo "Configuring pyATK..."
#cp files/usbdev.py pyatk/pyatk/channel/
#cp files/boot.py pyatk/pyatk/
#cd pyatk
#python3 -m venv pyatk_venv
#source pyatk_venv/bin/activate
#pip install pyserial pyusb==1.0.0
#pip install setuptools
#sudo pyatk_venv/bin/python3 setup.py install
#deactivate
#cd ..
