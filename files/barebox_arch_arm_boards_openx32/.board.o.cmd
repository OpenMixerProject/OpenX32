savedcmd_arch/arm/boards/openx32/board.o := /usr/bin/arm-none-eabi-gcc -Wp,-MD,arch/arm/boards/openx32/.board.o.d -nostdinc -isystem /usr/lib/gcc/arm-none-eabi/13.2.1/include -D__KERNEL__ -D__BAREBOX__ -Iinclude  -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/dts/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/generated/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/generated/uapi -include /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/linux/kconfig.h -fno-builtin -ffreestanding -Ulinux -Uunix -D__ARM__ -fno-strict-aliasing -marm -msoft-float -mlittle-endian -mabi=aapcs-linux -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -fdata-sections -ffunction-sections -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -funsigned-char -Werror=implicit-function-declaration -Werror=implicit-int -Werror=int-conversion -Wno-format-zero-length -Os -pipe -Wmissing-prototypes -std=gnu11 -mno-unaligned-access -ggdb3 -ftrivial-auto-var-init=zero -Wno-unused-but-set-variable -Wno-trampolines  -fno-delete-null-pointer-checks -fno-allow-store-data-races -fno-strict-overflow -fno-stack-check -fcf-protection=none -mbranch-protection=none -Wno-address-of-packed-member -Wtype-limits -Wno-pointer-sign -fmacro-prefix-map=/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/=   -fno-stack-protector  -fPIE    -D"KBUILD_STR(s)=$(pound)s" -D"KBUILD_BASENAME=KBUILD_STR(board)"  -D"KBUILD_MODNAME=KBUILD_STR(board)" -c -o arch/arm/boards/openx32/board.o arch/arm/boards/openx32/board.c

source_arch/arm/boards/openx32/board.o := arch/arm/boards/openx32/board.c

deps_arch/arm/boards/openx32/board.o := \
    $(wildcard include/config/DRIVER_VIDEO_IMX) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/linux/kconfig.h \
    $(wildcard include/config/FOO) \
  include/linux/is_defined.h \
    $(wildcard include/config/BOOGER) \
  include/common.h \
  include/stdio.h \
    $(wildcard include/config/PBL_CONSOLE) \
    $(wildcard include/config/ARCH_HAS_CTRLC) \
    $(wildcard include/config/CONSOLE_NONE) \
  /usr/lib/gcc/arm-none-eabi/13.2.1/include/stdarg.h \
  include/console.h \
    $(wildcard include/config/CBSIZE) \
    $(wildcard include/config/PROMPT) \
    $(wildcard include/config/CONSOLE_FULL) \
    $(wildcard include/config/DEBUG_LL) \
  include/param.h \
    $(wildcard include/config/PARAMETER) \
  include/linux/err.h \
  include/linux/compiler.h \
    $(wildcard include/config/TRACE_BRANCH_PROFILING) \
    $(wildcard include/config/PROFILE_ALL_BRANCHES) \
    $(wildcard include/config/STACK_VALIDATION) \
  include/linux/compiler_types.h \
    $(wildcard include/config/HAVE_ARCH_COMPILER_H) \
    $(wildcard include/config/STACKPROTECTOR) \
    $(wildcard include/config/ENABLE_MUST_CHECK) \
    $(wildcard include/config/ARCH_SUPPORTS_OPTIMIZED_INLINING) \
    $(wildcard include/config/OPTIMIZE_INLINING) \
  include/linux/compiler-gcc.h \
    $(wildcard include/config/ARCH_USE_BUILTIN_BSWAP) \
  include/linux/types.h \
    $(wildcard include/config/UID16) \
    $(wildcard include/config/ARCH_DMA_ADDR_T_64BIT) \
    $(wildcard include/config/PHYS_ADDR_T_64BIT) \
  include/linux/posix_types.h \
  include/linux/stddef.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/posix_types.h \
  include/asm-generic/posix_types.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/bitsperlong.h \
  include/asm-generic/bitsperlong.h \
    $(wildcard include/config/64BIT) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/types.h \
  include/asm-generic/int-ll64.h \
  include/linux/errno.h \
  include/asm-generic/errno.h \
  include/linux/list.h \
    $(wildcard include/config/DEBUG_LIST) \
  include/linux/container_of.h \
  include/linux/build_bug.h \
  include/linux/poison.h \
    $(wildcard include/config/ILLEGAL_POINTER_VALUE) \
  include/linux/const.h \
  include/uapi/linux/const.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/barrier.h \
  include/asm-generic/barrier.h \
  include/asm-generic/rwonce.h \
  include/linux/clk.h \
    $(wildcard include/config/HAVE_CLK) \
    $(wildcard include/config/COMMON_CLK) \
    $(wildcard include/config/COMMON_CLK_OF_PROVIDER) \
  include/linux/spinlock.h \
  include/linux/cleanup.h \
  include/linux/stringify.h \
  include/linux/string.h \
    $(wildcard include/config/FORTIFY_SOURCE) \
  include/linux/overflow.h \
  include/linux/limits.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/string.h \
    $(wildcard include/config/ARM_OPTIMZED_STRING_FUNCTIONS) \
  include/deep-probe.h \
    $(wildcard include/config/OFDEVICE) \
  include/xfuncs.h \
  include/linux/bug.h \
    $(wildcard include/config/BUG_ON_DATA_CORRUPTION) \
  include/asm-generic/bug.h \
  include/printf.h \
    $(wildcard include/config/ARCH_HAS_STACK_DUMP) \
  include/wchar.h \
  include/malloc.h \
    $(wildcard include/config/MALLOC_TLSF) \
    $(wildcard include/config/INIT_ON_ALLOC_DEFAULT_ON) \
    $(wildcard include/config/INIT_ON_FREE_DEFAULT_ON) \
  include/types.h \
  include/driver.h \
    $(wildcard include/config/OFTREE) \
    $(wildcard include/config/DRIVER_NET_DM9K) \
    $(wildcard include/config/USB_EHCI) \
    $(wildcard include/config/DRIVER_NET_KS8851_MLL) \
    $(wildcard include/config/CDEV_ALIAS) \
    $(wildcard include/config/FS_AUTOMOUNT) \
  include/linux/ioport.h \
  include/linux/uuid.h \
  include/uapi/linux/uuid.h \
  include/linux/printk.h \
    $(wildcard include/config/COMPILE_LOGLEVEL) \
  include/linux/module.h \
  include/linux/export.h \
    $(wildcard include/config/MODULES) \
  include/device.h \
    $(wildcard include/config/CMD_DEVINFO) \
    $(wildcard include/config/ARCH_DMA_DEFAULT_COHERENT) \
  include/init.h \
  include/of.h \
    $(wildcard include/config/BOOTM_OFTREE_FALLBACK) \
    $(wildcard include/config/OF_OVERLAY) \
  include/fdt.h \
  include/linux/libfdt.h \
  include/linux/libfdt_env.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/byteorder.h \
  include/linux/byteorder/little_endian.h \
  include/linux/swab.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/swab.h \
  include/linux/byteorder/generic.h \
  include/linux/../../scripts/dtc/libfdt/libfdt.h \
  include/linux/../../scripts/dtc/libfdt/libfdt_env.h \
  include/linux/../../scripts/dtc/libfdt/fdt.h \
  include/errno.h \
  include/filetype.h \
    $(wildcard include/config/ARM) \
    $(wildcard include/config/FUZZ) \
    $(wildcard include/config/MIPS) \
  include/serdev.h \
  include/poller.h \
    $(wildcard include/config/POLLER) \
  include/kfifo.h \
  include/clock.h \
  include/linux/time.h \
  include/linux/bitops.h \
  include/linux/bits.h \
  include/uapi/linux/kernel.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/bitops.h \
    $(wildcard include/config/CPU_V8) \
    $(wildcard include/config/CPU_32) \
  include/asm-generic/bitops/__fls.h \
  include/asm-generic/bitops/fls64.h \
  include/asm-generic/bitops/hweight.h \
  include/barebox.h \
  include/module.h \
  include/elf.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/elf.h \
    $(wildcard include/config/CPU_64) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/ptrace.h \
    $(wildcard include/config/ARM_THUMB) \
  include/config.h \
  include/linux/kernel.h \
  include/linux/array_size.h \
  include/linux/barebox-wrapper.h \
  include/linux/slab.h \
  include/dma.h \
    $(wildcard include/config/HAS_DMA) \
  include/linux/align.h \
  include/dma-dir.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/dma.h \
    $(wildcard include/config/MMU) \
    $(wildcard include/config/ARMV7R_MPU) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/io.h \
    $(wildcard include/config/ARM32) \
    $(wildcard include/config/ARM64) \
  include/asm-generic/io.h \
  include/linux/instruction_pointer.h \
  include/asm-generic/io-typeconfused.h \
  include/asm-generic/bitio.h \
  include/linux/gfp.h \
  include/linux/math.h \
  include/linux/math64.h \
    $(wildcard include/config/ARCH_SUPPORTS_INT128) \
  include/asm-generic/div64.h \
  include/linux/log2.h \
    $(wildcard include/config/ARCH_HAS_ILOG2_U32) \
    $(wildcard include/config/ARCH_HAS_ILOG2_U64) \
  include/linux/wordpart.h \
  include/linux/minmax.h \
  include/linux/kstrtox.h \
  include/linux/hex.h \
  include/linux/pagemap.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/common.h \
  include/barebox-info.h \
    $(wildcard include/config/BANNER) \
  include/linux/sizes.h \
  include/gpio.h \
    $(wildcard include/config/GENERIC_GPIO) \
    $(wildcard include/config/GPIOLIB) \
    $(wildcard include/config/ARCH_NR_GPIO) \
    $(wildcard include/config/OF_GPIO) \
  include/slice.h \
  include/bthread.h \
    $(wildcard include/config/BTHREAD) \
  include/linux/iopoll.h \
  include/io.h \
  include/pbl.h \
  include/environment.h \
    $(wildcard include/config/ENVIRONMENT_VARIABLES) \
  include/mach/imx/imx25-regs.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/armlinux.h \
    $(wildcard include/config/BOOT_ATAGS) \
    $(wildcard include/config/ARM_BOARD_APPEND_ATAG) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/memory.h \
    $(wildcard include/config/EFI_PAYLOAD) \
  include/memory.h \
    $(wildcard include/config/MEMORY_ATTRIBUTES) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/setup.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/secure.h \
    $(wildcard include/config/ARM_SECURE_MONITOR) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/sections.h \
  include/asm-generic/sections.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/barebox-arm.h \
    $(wildcard include/config/ARM_EXCEPTIONS) \
    $(wildcard include/config/OPTEE_SIZE) \
    $(wildcard include/config/STACK_GUARD_PAGE) \
    $(wildcard include/config/FS_PSTORE_RAMOOPS_SIZE) \
    $(wildcard include/config/FS_PSTORE_RAMOOPS) \
  include/asm-generic/memory_layout.h \
    $(wildcard include/config/MEMORY_LAYOUT_DEFAULT) \
    $(wildcard include/config/MALLOC_SIZE) \
    $(wildcard include/config/STACK_SIZE) \
    $(wildcard include/config/MEMORY_LAYOUT_FIXED) \
    $(wildcard include/config/STACK_BASE) \
    $(wildcard include/config/MALLOC_BASE) \
    $(wildcard include/config/OPTEE_SHM_SIZE) \
    $(wildcard include/config/SCRATCH_SIZE) \
  include/pbl/handoff-data.h \
    $(wildcard include/config/PBL_IMAGE) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/barebox-arm-head.h \
    $(wildcard include/config/THUMB2_BAREBOX) \
    $(wildcard include/config/PBL_BREAK) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/system.h \
    $(wildcard include/config/CPU_XSC3) \
    $(wildcard include/config/CPU_FA526) \
    $(wildcard include/config/CPU_64v8) \
    $(wildcard include/config/CPU_32v7) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/reloc.h \
  include/asm-generic/reloc.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/mach-types.h \
    $(wildcard include/config/ARCH_VERSATILE_PB) \
    $(wildcard include/config/MACH_CSB337) \
    $(wildcard include/config/MACH_SCB9328) \
    $(wildcard include/config/MACH_AT91RM9200EK) \
    $(wildcard include/config/MACH_AT91SAM9261EK) \
    $(wildcard include/config/MACH_AT91SAM9260EK) \
    $(wildcard include/config/MACH_PM9261) \
    $(wildcard include/config/MACH_AT91SAM9263EK) \
    $(wildcard include/config/MACH_PM9263) \
    $(wildcard include/config/MACH_OMAP3_BEAGLE) \
    $(wildcard include/config/MACH_AT91SAM9G20EK) \
    $(wildcard include/config/MACH_USB_A9260) \
    $(wildcard include/config/MACH_USB_A9263) \
    $(wildcard include/config/MACH_QIL_A9260) \
    $(wildcard include/config/MACH_PICOCOM1) \
    $(wildcard include/config/MACH_AT91SAM9M10G45EK) \
    $(wildcard include/config/MACH_USB_A9G20) \
    $(wildcard include/config/MACH_QIL_A9G20) \
    $(wildcard include/config/MACH_CHUMBY) \
    $(wildcard include/config/MACH_TNY_A9260) \
    $(wildcard include/config/MACH_TNY_A9G20) \
    $(wildcard include/config/MACH_MX51_BABBAGE) \
    $(wildcard include/config/MACH_TNY_A9263) \
    $(wildcard include/config/MACH_AT91SAM9G10EK) \
    $(wildcard include/config/MACH_TX25) \
    $(wildcard include/config/MACH_MX23EVK) \
    $(wildcard include/config/MACH_PM9G45) \
    $(wildcard include/config/MACH_OMAP4_PANDA) \
    $(wildcard include/config/MACH_ARMADA_XP_DB) \
    $(wildcard include/config/MACH_TX28) \
    $(wildcard include/config/MACH_BCM2708) \
    $(wildcard include/config/MACH_MX53_LOCO) \
    $(wildcard include/config/MACH_TX53) \
    $(wildcard include/config/MACH_CCMX53) \
    $(wildcard include/config/MACH_CCWMX53) \
    $(wildcard include/config/MACH_VMX53) \
    $(wildcard include/config/MACH_DSS11) \
    $(wildcard include/config/MACH_BEAGLEBONE) \
    $(wildcard include/config/MACH_PCAAXL2) \
    $(wildcard include/config/MACH_MX6Q_SABRESD) \
    $(wildcard include/config/MACH_TQMA53) \
    $(wildcard include/config/MACH_IMX233_OLINUXINO) \
    $(wildcard include/config/MACH_CFA10036) \
    $(wildcard include/config/MACH_PCM051) \
    $(wildcard include/config/MACH_HABA_KNX_LITE) \
    $(wildcard include/config/MACH_VAR_SOM_MX6) \
    $(wildcard include/config/MACH_PCAAXS1) \
    $(wildcard include/config/MACH_PFLA03) \
  include/mach/imx/imx-nand.h \
  include/linux/mtd/mtd.h \
  include/linux/mtd/mtd-abi.h \
  include/mach/imx/iomux-mx25.h \
  include/mach/imx/iomux-v3.h \
  include/linux/bitfield.h \
  include/mach/imx/generic.h \
    $(wildcard include/config/ARCH_IMX1) \
    $(wildcard include/config/ARCH_IMX21) \
    $(wildcard include/config/ARCH_IMX25) \
    $(wildcard include/config/ARCH_IMX27) \
    $(wildcard include/config/ARCH_IMX31) \
    $(wildcard include/config/ARCH_IMX35) \
    $(wildcard include/config/ARCH_IMX50) \
    $(wildcard include/config/ARCH_IMX51) \
    $(wildcard include/config/ARCH_IMX53) \
    $(wildcard include/config/ARCH_IMX6) \
    $(wildcard include/config/ARCH_IMX7) \
    $(wildcard include/config/ARCH_IMX8MM) \
    $(wildcard include/config/ARCH_IMX8MN) \
    $(wildcard include/config/ARCH_IMX8MP) \
    $(wildcard include/config/ARCH_IMX8MQ) \
    $(wildcard include/config/ARCH_IMX93) \
    $(wildcard include/config/ARCH_VF610) \
    $(wildcard include/config/ARCH_MULTIARCH) \
    $(wildcard include/config/BOARD_ARM_GENERIC_DT) \
  include/bootsource.h \
  include/mach/imx/imx_cpu_types.h \
  include/mach/imx/iim.h \
    $(wildcard include/config/IMX_IIM) \
  include/net.h \
    $(wildcard include/config/NET) \
    $(wildcard include/config/NET_RESOLV) \
  include/stdlib.h \
  include/led.h \
    $(wildcard include/config/LED_TRIGGERS) \
    $(wildcard include/config/LED_GPIO) \
    $(wildcard include/config/LED_GPIO_BICOLOR) \
    $(wildcard include/config/LED_GPIO_RGB) \
  include/linux/phy.h \
    $(wildcard include/config/PHYLIB) \
  include/linux/ethtool.h \
  include/linux/mii.h \
  include/platform_data/imxfb.h \
  include/fb.h \
  include/ioctl.h \
  include/asm-generic/ioctl.h \
  include/mach/imx/bbu.h \
    $(wildcard include/config/BAREBOX_UPDATE) \
    $(wildcard include/config/BAREBOX_UPDATE_IMX_EXTERNAL_NAND) \
  include/bbu.h \
    $(wildcard include/config/BAREBOX_UPDATE_IMX_NAND_FCB) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/mmu.h \
    $(wildcard include/config/CACHE_L2X0) \

arch/arm/boards/openx32/board.o: $(deps_arch/arm/boards/openx32/board.o)

$(deps_arch/arm/boards/openx32/board.o):
