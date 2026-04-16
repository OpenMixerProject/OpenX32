#!/bin/bash
/usr/bin/arm-linux-gnueabi-gcc -mcpu=arm926ej-s -flto -fwhole-program -flto-partition=none -fno-caller-saves -fno-plt  -I . *.c -o ../bin/x32sdconfig
