// SPDX-License-Identifier: GPL-2.0-or-later
// SPDX-FileCopyrightText: 2011 Sascha Hauer <s.hauer@pengutronix.de>, Pengutronix

#include <common.h>
#include <init.h>
#include <mach/imx/imx25-regs.h>
#include <mach/imx/esdctl.h>
#include <io.h>
#include <linux/sizes.h>
#include <mach/imx/imx-nand.h>
#include <asm/barebox-arm.h>
#include <asm/barebox-arm-head.h>
#include <asm/system.h>
#include <asm/sections.h>
#include <asm-generic/memory_layout.h>
#include <debug_ll.h>

static inline void setup_uart5(void)
{
	void __iomem *uartbase = (void *)MX25_UART5_BASE_ADDR;
	void __iomem *iomuxbase = (void *)MX25_IOMUXC_BASE_ADDR;

	writel(0x0, iomuxbase + 0x174);

	writel(0x00000000, uartbase + 0x80);
	writel(0x00004027, uartbase + 0x84);
	writel(0x00000704, uartbase + 0x88);
	writel(0x00000a81, uartbase + 0x90);
	writel(0x0000002b, uartbase + 0x9c);
	writel(0x00013880, uartbase + 0xb0);
	writel(0x0000047f, uartbase + 0xa4);
	writel(0x0000a259, uartbase + 0xa8);
	writel(0x00000001, uartbase + 0x80);

	putc_ll('>');
}

static inline void __bare_init  setup_sdram(uint32_t base, uint32_t esdctl,
		uint32_t esdcfg)
{
	uint32_t esdctlreg = MX25_ESDCTL_BASE_ADDR + IMX_ESDCTL0;
	uint32_t esdcfgreg = MX25_ESDCTL_BASE_ADDR + IMX_ESDCFG0;

	if (base == 0x90000000) {
		esdctlreg += 8;
		esdcfgreg += 8;
	}

	esdctl |= ESDCTL0_SDE;

	writel(esdcfg, esdcfgreg);
	writel(esdctl | ESDCTL0_SMODE_PRECHARGE, esdctlreg);
	writel(0, base + 1024);
	writel(esdctl | ESDCTL0_SMODE_AUTO_REFRESH, esdctlreg);
	readb(base);
	readb(base);
	writel(esdctl | ESDCTL0_SMODE_LOAD_MODE, esdctlreg);
	writeb(0, base + 0x33);
	writel(esdctl, esdctlreg);
}

extern char __dtb_imx25_openx32_start[];

static void __noreturn openx32_start(void)
{
	void *fdt;

	fdt = __dtb_imx25_openx32_start + get_runtime_offset();

	imx25_barebox_entry(fdt);
}

static void __noreturn openx32_load_nand(void)
{
	imx25_nand_load_image();
	openx32_start();
}

static void __bare_init openx32_common_init(void)
{
	uint32_t r;

	arm_cpu_lowlevel_init();

	/* AIPS setup - Only setup MPROTx registers. The PACR default values are good.
	 * Set all MPROTx to be non-bufferable, trusted for R/W,
	 * not forced to user-mode.
	 */
	writel(0x77777777, 0x43f00000);
	writel(0x77777777, 0x43f00004);
	writel(0x77777777, 0x53f00000);
	writel(0x77777777, 0x53f00004);

	/* MAX (Multi-Layer AHB Crossbar Switch) setup
	 * MPR - priority for MX25 is (SDHC2/SDMA)>USBOTG>RTIC>IAHB>DAHB
	 */
	writel(0x00043210, 0x43f04000);
	writel(0x00043210, 0x43f04100);
	writel(0x00043210, 0x43f04200);
	writel(0x00043210, 0x43f04300);
	writel(0x00043210, 0x43f04400);
	/* SGPCR - always park on last master */
	writel(0x10, 0x43f04010);
	writel(0x10, 0x43f04110);
	writel(0x10, 0x43f04210);
	writel(0x10, 0x43f04310);
	writel(0x10, 0x43f04410);
	/* MGPCR - restore default values */
	writel(0x0, 0x43f04800);
	writel(0x0, 0x43f04900);
	writel(0x0, 0x43f04a00);
	writel(0x0, 0x43f04b00);
	writel(0x0, 0x43f04c00);

	/* Configure M3IF registers
	 * M3IF Control Register (M3IFCTL) for MX25
	 * MRRP[0] = LCDC           on priority list (1 << 0)  = 0x00000001
	 * MRRP[1] = MAX1       not on priority list (0 << 1)  = 0x00000000
	 * MRRP[2] = MAX0       not on priority list (0 << 2)  = 0x00000000
	 * MRRP[3] = USB HOST   not on priority list (0 << 3)  = 0x00000000
	 * MRRP[4] = SDMA       not on priority list (0 << 4)  = 0x00000000
	 * MRRP[5] = SD/ATA/FEC not on priority list (0 << 5)  = 0x00000000
	 * MRRP[6] = SCMFBC     not on priority list (0 << 6)  = 0x00000000
	 * MRRP[7] = CSI        not on priority list (0 << 7)  = 0x00000000
	 *                                                       ----------
	 *                                                       0x00000001
	 */
	writel(0x1, 0xb8003000);

	/* configure ARM clk */
	writel(0x20034000, MX25_CCM_BASE_ADDR + MX25_CCM_CCTL);

	/* enable all the clocks */
	writel(0x1fffffff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR0);
	writel(0xffffffff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR1);
	writel(0x000fdfff, MX25_CCM_BASE_ADDR + MX25_CCM_CGCR2);

	setup_uart5();

	/* Skip SDRAM initialization if we run from RAM */
	r = get_pc();
	if (r > 0x80000000 && r < 0xa0000000)
		openx32_start();

	/* set to 3.3v SDRAM */
	writel(0x800, MX25_IOMUXC_BASE_ADDR + 0x454);

	writel(ESDMISC_RST, MX25_ESDCTL_BASE_ADDR + IMX_ESDMISC);

	while (!(readl(MX25_ESDCTL_BASE_ADDR + IMX_ESDMISC) & (1 << 31)));

#define ESDCTLVAL	(ESDCTL0_ROW13 | ESDCTL0_COL9 |	ESDCTL0_DSIZ_15_0 | \
			 ESDCTL0_REF4 | ESDCTL0_PWDT_PRECHARGE_PWDN | ESDCTL0_BL)
#define ESDCFGVAL	(ESDCFGx_tRP_3 | ESDCFGx_tMRD_2 | ESDCFGx_tRAS_6 | \
			 ESDCFGx_tRRD_2 | ESDCFGx_tCAS_3 | ESDCFGx_tRCD_3 | \
			 ESDCFGx_tRC_9)

	setup_sdram(0x80000000, ESDCTLVAL, ESDCFGVAL);
	setup_sdram(0x90000000, ESDCTLVAL, ESDCFGVAL);

	imx25_nand_relocate_to_sdram(openx32_load_nand);
}

ENTRY_FUNCTION(start_imx25_openx32, r0, r1, r2)
{
	arm_setup_stack(MX25_IRAM_BASE_ADDR + MX25_IRAM_SIZE);

	openx32_common_init();
}
