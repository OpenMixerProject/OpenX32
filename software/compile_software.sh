#!/bin/bash
echo "Compiling softwares for X32..."

# create binary-directory
mkdir bin
mkdir bin/libexec

echo "Compiling x32sdconfig..."
cd x32sdconfig
./compile.sh
cd ..

echo "Compiling x32ctrl..."
cd x32ctrl
make
cd ..

echo "Compiling fpgaconfig..."
cd fpgaconfig
./compile.sh
cd ..

echo "Compiling openssh-portable..."
cd openssh-portable
autoreconf
CC=/usr/bin/arm-linux-gnueabi-gcc ./configure --without-openssl --host=armel
make
mv sshd ../bin/
mv ssh-keygen ../bin/
mv sshd-auth ../bin/libexec/
mv sshd-session ../bin/libexec/
cd ..

echo "Done."
