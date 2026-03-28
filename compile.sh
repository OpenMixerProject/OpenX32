#!/bin/bash
echo "                               =#%@@@@@###%@@@@%-                                                "
echo "                           =*###+               :#@*                                             "
echo "                        +****.                      :%-                                          "
echo "                      #++++             ############  :%-          @@@@@@@@@@@@                  "
echo "                    .+===             ###======++==*##  *+       @@@*#%%#****#@@@                "
echo "                   -=-+               #+======+=.*===*#         @@#**@.*@******%@                "
echo "                  -=-+                ##======*  #====+#.      @@****@  @******@@                "
echo "                  +:-                  ##+====#  #***==+#+   =@@**@@@@  @****%@@                 "
echo "                 =*=*                   -#*===#  ## #===+## @@@***@ @@  @@#*@@@                  "
echo "      @@@@@@       ..                     ##+*#- ## #***==#= @@@@@@ @@   +@@@   @@@@@@@  @@@@@   "
echo "    @@@    @@@             @@@@            ##= + ## ## #+==#- @@ @@ @@ = @@@    @@  @@  @@   @@  "
echo "    @@      .@@#@@@@@@@  @@@ @@@ @@@@@@@@   .# # ## .= #-#++#= @ @  @@ * @*       @@*        @@  "
echo "    @@       @@ @@    @@ @@@@@@@  @@   @@      # =#  = + *::=#   @  @+ *           -@@@   @@@=   "
echo "    @@@    @@@  @@:   @@ @@       @@   @@      #  : .- : *::-#   @  +  #             @@ @@    @@ "
echo "      @@@@@@    @@@@@@@   @@@@@@  @@   @@@  =# # ## :+ #-#++#+ @ @  @@.* @@     @@@@@@  @@@@@@@@ "
echo "                @@                         ##+ * ## ## #+==#+ @@@@@ @@ = @@@                     "
echo "                @@                        ##+= = ## #***==#+ @@***@ @@   #@@@                    "
echo "                   :                    :#*==+#: ## #===+## @@@***@ @@  @@#*@@#                  "
echo "                  .%+                  ##+====#  #***==+#=   +@@**@@@@  @****%@@                 "
echo "                    %.                ##======*  #====*#  .*-  @@****@  @******@@                "
echo "                     %=               #*======+: *===*#   +-=+  @@#**@ -@******@@                "
echo "                      -@-             +##+=====+++=*##  ==-=-    @@@#%@@#****%@@@                "
echo "                        *@*             =###########  -===*        @@@@@@@@@@@@                  "
echo "                           @@%.                   .::=++*                                        "
echo "                             .#@@%%*-.    .:=+**##***+.                                          "
echo "                                  .-+%%%%%%#***=-.                                               "
echo ""
echo "Compiling OpenX32 Operating System for the Behringer X32 Audio-Mixing Console"

cleanup() {
    tput csr 0 $(($(tput lines) - 1)) # reset scroll-region
    tput rc                          # restore cursor
    echo -e "\nScript beendet."
    exit
}
trap cleanup SIGINT SIGTERM

# get terminal-infos
LINES=$(tput lines)
COLUMNS=$(tput cols)
DATA_LINES=$((LINES - 2)) # leave two lines for status-information

# define scroll-Region (row 0 to DATA_LINES)
tput csr 0 $DATA_LINES
clear

update_progress() {
    local Percent=$1
    local Text=$2
    # save cursor-Position
    tput sc
    # jump to row = rowcount-2 to display separator
    tput cup $((LINES - 1)) 0
    printf '%*s' "$COLUMNS" '' | tr ' ' '-' 
    # jump to last row
    tput cup $LINES 0
    # display progressbar in green
    echo -ne "\e[32m[COMPILING OPENX32]: $Percent% - Current: $Text\e[0m"
    # move cursor back to old position for next script-output
    tput rc
}

# ================ CHECK SCRIPT-PARAMETERS ================
# Standardmäßig werden alle Schritte ausgeführt
COMPILE_UBOOT=true
COMPILE_LINUX=true
COMPILE_BUSYBOX=true
COMPILE_SOFTWARE=true

# Argumente verarbeiten
while [[ $# -gt 0 ]]; do
  case $1 in
    --skip-uboot)
      COMPILE_UBOOT=false
      shift # Nächstes Argument prüfen
      ;;
    --skip-linux)
      COMPILE_LINUX=false
      shift
      ;;
    --skip-busybox)
      COMPILE_BUSYBOX=false
      shift
      ;;
    --skip-software)
      COMPILE_SOFTWARE=false
      shift
      ;;
    *)
      echo "Unknown Parameter: $1"
      exit 1
      ;;
  esac
done



update_progress 0 "Prepare compilation..."
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
# custom boot logo - fullscreen -> console has only 1 line!
# cp files/linux-boot-logo_final.ppm linux/drivers/video/logo/logo_linux_clut224.ppm

# =================== Loader =======================

update_progress 5 "Compile Miniloader..."
cd miniloader
make

if [ "$COMPILE_UBOOT" = true ]; then
	update_progress 10 "Compile U-Boot..."
	cd ../u-boot
	ARCH=arm CROSS_COMPILE=/usr/bin/arm-none-eabi- make -j$(nproc)
fi

# =================== Linux =======================

if [ "$COMPILE_LINUX" = true ]; then
	update_progress 25 "Compile Linux..."
	cd ../linux
	ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make -j$(nproc) zImage
	ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make -j$(nproc) dtbs
	update_progress 50 "Create U-Boot-image..."
	mkimage -A ARM -O linux -T kernel -C none -a 0x80060000 -e 0x80060000 -n "Linux kernel (OpenX32)" -d arch/arm/boot/zImage /tmp/uImage
fi

# =================== Busybox =======================

if [ "$COMPILE_BUSYBOX" = true ]; then
	update_progress 55 "Compile busybox..."
	cd ../busybox
	ARCH=arm CROSS_COMPILE=/usr/bin/arm-linux-gnueabi- make -j$(nproc)
	ARCH=arm make install
	cd ..
	cp -rP /tmp/busybox_install/bin initramfs_root/
	cp -rP /tmp/busybox_install/sbin initramfs_root/
	cp -rP /tmp/busybox_install/linuxrc initramfs_root/
fi

# =============== Software-Tools ===================

if [ "$COMPILE_SOFTWARE" = true ]; then
	cd software

	update_progress 60 "Compile x32sdconfig..."
	cd x32sdconfig
	./compile.sh
	cd ..

	update_progress 65 "Compile x32ctrl..."
	cd x32ctrl
	make -j$(nproc)
	cd ..

	update_progress 70 "Compile dropbear..."
	cd dropbear
	./configure --host=arm-linux-gnueabi --enable-static --disable-zlib --disable-harden --disable-syslog
	make PROGRAMS="dropbear dropbearkey"
	cp dropbear ../bin/
	cp dropbearkey ../bin/
	cd ..

	update_progress 75 "Compile fb-vnc-server..."
	cd libvncserver
	rm -r build
	mkdir build && cd build
	cmake .. -DCMAKE_TOOLCHAIN_FILE=../../../files/libvncserver_toolchain.cmake \
        -DCMAKE_INSTALL_PREFIX=/tmp/armv5_libs \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
	    -DZLIB_INCLUDE_DIR=/usr/include/ \
	    -DZLIB_LIBRARY=/usr/lib/arm-linux-gnueabi/libz.a \
        -DCMAKE_C_FLAGS="-march=armv5t" \
	    -DCMAKE_EXE_LINKER_FLAGS="-static -L/usr/lib/arm-linux-gnueabi"
	cmake --build .
	make -j$(nproc) install
	cd ../..	
	
	cd framebuffer-vncserver
	rm -r build
	mkdir -p build && cd build
	VNC_LIB_ROOT=/tmp/armv5_libs
	ZLIB_LIB_PATH=/usr/lib/arm-linux-gnueabi
	cmake .. \
	    -DCMAKE_TOOLCHAIN_FILE=../../files/framebuffer-vncserver.cmake \
	    -DCMAKE_INSTALL_PREFIX={$VNC_LIB_ROOT} \
	    -DCMAKE_BUILD_TYPE=Release \
	    -DCMAKE_C_FLAGS="-march=armv5t -O2 -I${VNC_LIB_ROOT}/include -I/usr/include" \
	    -DCMAKE_PREFIX_PATH="${VNC_LIB_ROOT}" \
	    -DCMAKE_FIND_ROOT_PATH="${VNC_LIB_ROOT}" \
	    -DCMAKE_EXE_LINKER_FLAGS="-static -L${VNC_LIB_ROOT}/lib -L${ZLIB_LIB_PATH} -lvncserver -lpthread -ldl"
	make -j$(nproc)
	cd ../..
	
	cd ..
fi

# copy tools to initramFS
cp software/bin/x32sdconfig initramfs_root/openx32/
cp software/bin/x32ctrl initramfs_root/openx32/
cp software/dropbear/dropbear initramfs_root/openx32/
cp software/dropbear/dropbearkey initramfs_root/openx32/
cp software/framebuffer-vncserver/build/framebuffer-vncserver initramfs_root/openx32/

# =================== Create InitramFS =======================

update_progress 80 "Create initramFS..."
cd initramfs_root
mkdir -p dev proc sys etc mnt home usr
rm /tmp/initramfs.cpio.gz
rm /tmp/uramdisk.bin
find . -print0 | cpio --null -ov --format=newc > /tmp/initramfs.cpio
gzip -9 /tmp/initramfs.cpio
mkimage -A ARM -O linux -T ramdisk -C gzip -a 0 -e 0 -n "Ramdisk Image" -d /tmp/initramfs.cpio.gz /tmp/uramdisk.bin
cd ..

# =================== Binary-Blob =======================

rm /tmp/openx32.bin
# Miniloader at offset 0x000000: will be started by i.MX Serial Download Program
update_progress 85 "Merge binary-files...Miniloader -> openx32.bin"
dd if=miniloader/miniloader.bin of=/tmp/openx32.bin conv=notrunc
# U-Boot at offset 0x0000C0: will be started by Miniloader
update_progress 86 "Merge binary-files...U-Boot -> openx32.bin"
dd if=u-boot/u-boot.bin of=/tmp/openx32.bin bs=8 seek=$((0x18)) conv=notrunc
# Linux-Kernel at offset 0x060000 (384 kiB for Miniloader + U-Boot): will be started by U-Boot
update_progress 87 "Merge binary-files...Linux-Kernel -> openx32.bin"
dd if=/tmp/uImage of=/tmp/openx32.bin bs=512 seek=$((0x300)) conv=notrunc
# DeviceTreeBlob at offset 0x600000 (~6 MiB for Kernel)
update_progress 88 "Merge binary-files...DeviceTreeBlob -> openx32.bin"
dd if=linux/arch/arm/boot/dts/nxp/imx/imx25-pdk.dtb of=/tmp/openx32.bin bs=512 seek=$((0x3000)) conv=notrunc
# InitramFS at offset 0x810000 (~64kiB for DeviceTreeBlob)
update_progress 89 "Merge binary-files...InitramFS -> openx32.bin"
dd if=/tmp/uramdisk.bin of=/tmp/openx32.bin bs=512 seek=$((0x3080)) conv=notrunc
update_progress 90 "Merge binary-files...Finalize openx32.bin"
dd if=/dev/zero of=/tmp/openx32.bin bs=1 count=100 oflag=append conv=notrunc

# =================== DCP-Loader-File =======================

update_progress 95 "Creating final DCP-Loader-File..."

# creating unencrypted test-application
perl software/dcpapp/dcp_compiler.pl /tmp/openx32.bin:binary/dcpapp.bin /tmp/dcp_corefs_openx32.run

# creating encrypted OpenX32 DCP-Image
#mkdir -p /tmp/openx32/binary
#cp /tmp/openx32.bin /tmp/openx32/binary/dcpapp.bin
#./dcp-tool -c /tmp/dcp_corefs_openx32-alpha4.run "OpenX32 Alpha 4 - https://github.com/OpenMixerProject" /tmp/openx32/


update_progress 100 "Done."
echo "  ____                  __   ______ ____"
echo " / __ \\                 \\ \\ / /___ \\__  \\"
echo "| |  | |_ __   ___ _ __  \\ V /  __) | ) |"
echo "| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / /"
echo "| |__| | |_) |  __/ | | |/ . \\ ___) / /_ "
echo " \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|"
echo "       | |    https://www.openx32.com"
echo "       |_|    "
echo "Your version of OpenX32 is ready and it smells like fresh bread rolls. Yummie."
echo "The image with Miniloader, u-Boot, Linux Kernel, Ramdisk and DeviceTreeBlob is stored as /tmp/dcp_corefs_openx32.run"


# Am Ende die Scroll-Region wieder freigeben
tput csr 0 $((LINES - 1))
tput cup $LINES 0
echo ""
