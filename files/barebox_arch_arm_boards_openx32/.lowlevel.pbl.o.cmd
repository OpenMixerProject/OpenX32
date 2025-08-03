savedcmd_arch/arm/boards/openx32/lowlevel.pbl.o := /usr/bin/arm-none-eabi-gcc -Wp,-MD,arch/arm/boards/openx32/.lowlevel.pbl.o.d -nostdinc -isystem /usr/lib/gcc/arm-none-eabi/13.2.1/include -D__KERNEL__ -D__BAREBOX__ -Iinclude  -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/dts/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/generated/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/uapi -I/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/generated/uapi -include /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/include/linux/kconfig.h -fno-builtin -ffreestanding -Ulinux -Uunix -D__ARM__ -fno-strict-aliasing -marm -msoft-float -mlittle-endian -mabi=aapcs-linux -D__LINUX_ARM_ARCH__=5 -march=armv5te -mtune=arm9tdmi -fdata-sections -ffunction-sections -Wall -Wundef -Werror=strict-prototypes -Wno-trigraphs -fno-strict-aliasing -fno-common -fshort-wchar -funsigned-char -Werror=implicit-function-declaration -Werror=implicit-int -Werror=int-conversion -Wno-format-zero-length -Os -pipe -Wmissing-prototypes -std=gnu11 -mno-unaligned-access -ggdb3 -ftrivial-auto-var-init=zero -Wno-unused-but-set-variable -Wno-trampolines  -fno-delete-null-pointer-checks -fno-allow-store-data-races -fno-strict-overflow -fno-stack-check -fcf-protection=none -mbranch-protection=none -Wno-address-of-packed-member -Wtype-limits -Wno-pointer-sign -fmacro-prefix-map=/home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/=   -fno-stack-protector  -fPIE    -D__PBL__  -D"KBUILD_STR(s)=$(pound)s" -D"KBUILD_BASENAME=KBUILD_STR(lowlevel.pbl)"  -D"KBUILD_MODNAME=KBUILD_STR(lowlevel.pbl)" -c -o arch/arm/boards/openx32/lowlevel.pbl.o arch/arm/boards/openx32/lowlevel.c

source_arch/arm/boards/openx32/lowlevel.pbl.o := arch/arm/boards/openx32/lowlevel.c

deps_arch/arm/boards/openx32/lowlevel.pbl.o := \
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
  include/mach/imx/imx25-regs.h \
  include/mach/imx/esdctl.h \
  include/io.h \
  include/linux/sizes.h \
  include/mach/imx/imx-nand.h \
  include/linux/mtd/mtd.h \
  include/linux/mtd/mtd-abi.h \
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
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/sections.h \
  include/asm-generic/sections.h \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/reloc.h \
  include/asm-generic/reloc.h \
  include/debug_ll.h \
    $(wildcard include/config/HAS_DEBUG_LL) \
  /home/alex/Entwicklung/X32/barebox/barebox-2025.07.0/arch/arm/include/asm/debug_ll.h \
    $(wildcard include/config/DEBUG_IMX_UART) \
    $(wildcard include/config/DEBUG_ROCKCHIP_UART) \
    $(wildcard include/config/DEBUG_OMAP_UART) \
    $(wildcard include/config/DEBUG_ZYNQMP_UART) \
    $(wildcard include/config/DEBUG_STM32MP_UART) \
    $(wildcard include/config/DEBUG_VEXPRESS_UART) \
    $(wildcard include/config/DEBUG_BCM283X_UART) \
    $(wildcard include/config/DEBUG_LAYERSCAPE_UART) \
    $(wildcard include/config/DEBUG_SEMIHOSTING) \
    $(wildcard include/config/DEBUG_QEMU_ARM64_VIRT) \
    $(wildcard include/config/ARCH_MVEBU) \
    $(wildcard include/config/ARCH_ZYNQ) \
    $(wildcard include/config/ARCH_VERSATILE) \
    $(wildcard include/config/ARCH_TEGRA) \
    $(wildcard include/config/ARCH_SOCFPGA) \
    $(wildcard include/config/ARCH_PXA) \
    $(wildcard include/config/ARCH_MXS) \
    $(wildcard include/config/ARCH_CLPS711X) \
    $(wildcard include/config/ARCH_AT91) \
    $(wildcard include/config/ARCH_K3) \
  include/mach/versatile/debug_ll.h \
  include/debug_ll/pl011.h \
  include/linux/amba/serial.h \

arch/arm/boards/openx32/lowlevel.pbl.o: $(deps_arch/arm/boards/openx32/lowlevel.pbl.o)

$(deps_arch/arm/boards/openx32/lowlevel.pbl.o):
