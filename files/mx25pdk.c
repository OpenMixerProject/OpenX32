// SPDX-License-Identifier: GPL-2.0+
/*
 * (C) Copyright 2011 Freescale Semiconductor, Inc.
 *
 * Author: Fabio Estevam <fabio.estevam@freescale.com>
 */

#include <common.h>
#include <init.h>
#include <asm/io.h>
#include <asm/gpio.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/iomux-mx25.h>
#include <asm/arch/clock.h>
#include <mmc.h>
#include <fsl_esdhc_imx.h>
#include <i2c.h>
#include <linux/delay.h>
#include <mc34704.h>

#define FEC_RESET_B		IMX_GPIO_NR(1, 1)
#define LAMP_PWM		IMX_GPIO_NR(1, 26)
#define USB_POWER		IMX_GPIO_NR(4, 11)
#define MCU_BUSY		IMX_GPIO_NR(4, 24)
#define SURFACE_RESET		IMX_GPIO_NR(4, 28)

DECLARE_GLOBAL_DATA_PTR;

#ifdef CONFIG_FSL_ESDHC_IMX
struct fsl_esdhc_cfg esdhc_cfg[1] = {
	{IMX_MMC_SDHC1_BASE},
};
#endif

/*
 * FIXME: need to revisit this
 * The original code enabled PUE and 100-k pull-down without PKE, so the right
 * value here is likely:
 *	0 for no pull
 * or:
 *	PAD_CTL_PUS_100K_DOWN for 100-k pull-down
 */
#define FEC_OUT_PAD_CTRL	0

#define I2C_PAD_CTRL		(PAD_CTL_HYS | PAD_CTL_PUS_100K_UP | \
				 PAD_CTL_ODE)

static void mx25pdk_fec_init(void)
{
	static const iomux_v3_cfg_t fec_pads[] = {
		MX25_PAD_FEC_TX_CLK__FEC_TX_CLK,
		MX25_PAD_FEC_RX_DV__FEC_RX_DV,
		MX25_PAD_FEC_RDATA0__FEC_RDATA0,
		NEW_PAD_CTRL(MX25_PAD_FEC_TDATA0__FEC_TDATA0, FEC_OUT_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_FEC_TX_EN__FEC_TX_EN, FEC_OUT_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_FEC_MDC__FEC_MDC, FEC_OUT_PAD_CTRL),
		MX25_PAD_FEC_MDIO__FEC_MDIO,
		MX25_PAD_FEC_RDATA1__FEC_RDATA1,
		NEW_PAD_CTRL(MX25_PAD_FEC_TDATA1__FEC_TDATA1, FEC_OUT_PAD_CTRL),

		NEW_PAD_CTRL(MX25_PAD_GPIO_B__GPIO_B, 0), /* FEC_RESET_B */
	};

	static const iomux_v3_cfg_t i2c_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_I2C1_CLK__I2C1_CLK, I2C_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_I2C1_DAT__I2C1_DAT, I2C_PAD_CTRL),
	};

	imx_iomux_v3_setup_multiple_pads(fec_pads, ARRAY_SIZE(fec_pads));

	/* configure FEC_RESET as output */
	gpio_request(FEC_RESET_B, "FEC_RESET");
	gpio_direction_output(FEC_RESET_B, 1);

	/* Assert RESET */
	gpio_set_value(FEC_RESET_B, 0);

	udelay(10);

	/* Deassert RESET */
	gpio_set_value(FEC_RESET_B, 1);

	/* Setup I2C pins */
	imx_iomux_v3_setup_multiple_pads(i2c_pads, ARRAY_SIZE(i2c_pads));
}

int dram_init(void)
{
	/* dram_init must store complete ramsize in gd->ram_size */
	gd->ram_size = get_ram_size((void *)CONFIG_SYS_SDRAM_BASE,
				PHYS_SDRAM_1_SIZE);
	return 0;
}

/*
 * Set up input pins with hysteresis and 100-k pull-ups
 */
#define UART5_IN_PAD_CTRL       (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP)
/*
 * FIXME: need to revisit this
 * The original code enabled PUE and 100-k pull-down without PKE, so the right
 * value here is likely:
 *	0 for no pull
 * or:
 *	PAD_CTL_PUS_100K_DOWN for 100-k pull-down
 */
#define UART5_OUT_PAD_CTRL      0

static void mx25pdk_uart_init(void)
{
	static const iomux_v3_cfg_t uart_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_LBA__UART5_RXD_MUX, UART5_IN_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_ECB__UART5_TXD_MUX, UART5_OUT_PAD_CTRL),
	};

	imx_iomux_v3_setup_multiple_pads(uart_pads, ARRAY_SIZE(uart_pads));
}

int board_early_init_f(void)
{
	return 0;
}

int board_init(void)
{
	// configure peripheral enables for CGCR0
	// configure peripheral enables for CGCR1
	// configure peripheral enables for CGCR2
	// Set Peripheral Clock Divider Registers (CCM_PDR0...CCM_PDR3)
	asm volatile(
	"ldr r0, =0x53F8000C \n\t"
	"ldr r1, =0x1FFFFFFF \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80010 \n\t"
	"ldr r1, =0xFFFFFFFF \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80014 \n\t"
	"ldr r1, =0x000FDFFF \n\t"
	"str r1, [r0] \n\t"

	"ldr r0, =0x53F80018 \n\t"
	"ldr r1, =0x23C83403 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F8001C \n\t"
	"ldr r1, =0x03030303 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80020 \n\t"
	"ldr r1, =0x01010103 \n\t"
	"str r1, [r0] \n\t"
	"ldr r0, =0x53F80024 \n\t"
	"ldr r1, =0x01010101 \n\t"
	"str r1, [r0]"
	);

	// set GPIOs within IOMUXC
	static const iomux_v3_cfg_t gpio_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_CONTRAST__PWM4_PWMO, 0),
		NEW_PAD_CTRL(MX25_PAD_PWM__GPIO_1_26, 0),
		NEW_PAD_CTRL(MX25_PAD_D9__GPIO_4_11, 0),
		NEW_PAD_CTRL(MX25_PAD_UART1_RTS__GPIO_4_24, 0),
		NEW_PAD_CTRL(MX25_PAD_UART2_RTS__GPIO_4_28, 0),
	};
	imx_iomux_v3_setup_multiple_pads(gpio_pads, ARRAY_SIZE(gpio_pads));

	// enable display-backlight
        // configure Prescaler but disable PWM (PWM_PWMC)
        asm volatile(
        "ldr r0, =0x53FC8000\n\t"
        "ldr r1, =0x000100A0\n\t"
        "str r1, [r0]"
        );
        // set periode (PWM_PWMPR) and dutycycle (PWM_PWMSAR)
        asm volatile(
        "ldr r0, =0x53FC8010\n\t"
        "ldr r1, =0x00000190\n\t"
        "str r1, [r0]\n\t"
        "ldr r0, =0x53FC800C\n\t"
        "ldr r1, =0x000000C8\n\t"
        "str r1, [r0]"
        );
        // configure Prescaler and enable PWM (PWM_PWMC)
        asm volatile(
        "ldr r0, =0x53FC8000\n\t"
        "ldr r1, =0x000100A1\n\t"
        "str r1, [r0]"
        );

        // set SPI-pin-strength to high
        asm volatile(
        "ldr r0, =0x43FAC450\n\t"
        "ldr r1, =0x00000002\n\t"
        "str r1, [r0]"
        );

	// enable LAMP (asserted when high)
	gpio_request(LAMP_PWM, "LAMP_PWM");
	gpio_direction_output(LAMP_PWM, 1);
	gpio_set_value(LAMP_PWM, 1);

	// enable USB_POWER (asserted when high)
	gpio_request(USB_POWER, "USB_POWER");
	gpio_direction_output(USB_POWER, 1);
	gpio_set_value(USB_POWER, 1);

	// enable MCU_BUSY LED (asserted when high)
	gpio_request(MCU_BUSY, "MCU_BUSY");
	gpio_direction_output(MCU_BUSY, 1);
	gpio_set_value(MCU_BUSY, 1);

	// enable SURFACE_RESET pin (asserted when zero)
	gpio_request(SURFACE_RESET, "SURFACE_RESET");
	gpio_direction_output(SURFACE_RESET, 1);
	gpio_set_value(SURFACE_RESET, 1);

	mx25pdk_uart_init();

	/* address of boot parameters */
	gd->bd->bi_boot_params = PHYS_SDRAM_1 + 0x100;

	return 0;
}

int board_late_init(void)
{
	mx25pdk_fec_init();

	return 0;
}

#ifdef CONFIG_FSL_ESDHC_IMX
int board_mmc_getcd(struct mmc *mmc)
{
	return 1; /* X32 does not support card-detect */
}

int board_mmc_init(struct bd_info *bis)
{
	static const iomux_v3_cfg_t sdhc1_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_SD1_CMD__SD1_CMD, NO_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_SD1_CLK__SD1_CLK, NO_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_SD1_DATA0__SD1_DATA0, NO_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_SD1_DATA1__SD1_DATA1, NO_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_SD1_DATA2__SD1_DATA2, NO_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_SD1_DATA3__SD1_DATA3, NO_PAD_CTRL),
	};

	imx_iomux_v3_setup_multiple_pads(sdhc1_pads, ARRAY_SIZE(sdhc1_pads));

	/*
	 * Set the eSDHC1 PER clock to the maximum frequency lower than or equal
	 * to 50 MHz that can be obtained, which requires to use UPLL as the
	 * clock source. This actually gives 48 MHz.
	 */
	imx_set_perclk(MXC_ESDHC1_CLK, true, 50000000);
	esdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC1_CLK);
	return fsl_esdhc_initialize(bis, &esdhc_cfg[0]);
}
#endif

int checkboard(void)
{
	puts("Board: MX25PDK\n");

	return 0;
}

/* Lowlevel init isn't used on mx25pdk, so just provide a dummy one here */
void lowlevel_init(void) {}
