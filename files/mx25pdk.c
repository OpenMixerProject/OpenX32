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
#define SURFACE_RESET	IMX_GPIO_NR(4, 28)

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
#define UART_IN_PAD_CTRL       (PAD_CTL_HYS | PAD_CTL_PUS_100K_UP)
/*
 * FIXME: need to revisit this
 * The original code enabled PUE and 100-k pull-down without PKE, so the right
 * value here is likely:
 *	0 for no pull
 * or:
 *	PAD_CTL_PUS_100K_DOWN for 100-k pull-down
 */
#define UART_OUT_PAD_CTRL      0

static void mx25pdk_uart_init(void)
{
	static const iomux_v3_cfg_t uart_pads[] = {
		NEW_PAD_CTRL(MX25_PAD_UART2_RXD__UART2_RXD, UART_IN_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_UART2_TXD__UART2_TXD, UART_OUT_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_LBA__UART5_RXD_MUX, UART_IN_PAD_CTRL),
		NEW_PAD_CTRL(MX25_PAD_ECB__UART5_TXD_MUX, UART_OUT_PAD_CTRL),
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

/////////////////////////////////////////////////
// functions for mxc-uart from serial_mxc.c
/////////////////////////////////////////////////

#include <watchdog.h>

struct mxc_uart {
	u32 rxd;
	u32 spare0[15];

	u32 txd;
	u32 spare1[15];

	u32 cr1;
	u32 cr2;
	u32 cr3;
	u32 cr4;

	u32 fcr;
	u32 sr1;
	u32 sr2;
	u32 esc;

	u32 tim;
	u32 bir;
	u32 bmr;
	u32 brc;

	u32 onems;
	u32 ts;
};

#define mxc_uart_base_surface	((struct mxc_uart *)UART2_BASE)

#define URXD_CHARRDY	(1<<15)
#define URXD_ERR	(1<<14)
#define URXD_OVRRUN	(1<<13)
#define URXD_FRMERR	(1<<12)
#define URXD_BRK	(1<<11)
#define URXD_PRERR	(1<<10)
#define URXD_RX_DATA	(0xFF)
#define UCR1_ADEN	(1<<15) // Auto dectect interrupt
#define UCR1_ADBR	(1<<14) // Auto detect baud rate
#define UCR1_TRDYEN	(1<<13) // Transmitter ready interrupt enable
#define UCR1_IDEN	(1<<12) // Idle condition interrupt
#define UCR1_RRDYEN	(1<<9)	// Recv ready interrupt enable
#define UCR1_RDMAEN	(1<<8)	// Recv ready DMA enable
#define UCR1_IREN	(1<<7)	// Infrared interface enable
#define UCR1_TXMPTYEN	(1<<6)	// Transimitter empty interrupt enable
#define UCR1_RTSDEN	(1<<5)	// RTS delta interrupt enable
#define UCR1_SNDBRK	(1<<4)	// Send break
#define UCR1_TDMAEN	(1<<3)	// Transmitter ready DMA enable
#define UCR1_UARTCLKEN	(1<<2)	// UART clock enabled
#define UCR1_DOZE	(1<<1)	// Doze
#define UCR1_UARTEN	(1<<0)	// UART enabled
#define UCR2_ESCI	(1<<15) // Escape seq interrupt enable
#define UCR2_IRTS	(1<<14) // Ignore RTS pin
#define UCR2_CTSC	(1<<13) // CTS pin control
#define UCR2_CTS	(1<<12) // Clear to send
#define UCR2_ESCEN	(1<<11) // Escape enable
#define UCR2_PREN	(1<<8)  // Parity enable
#define UCR2_PROE	(1<<7)  // Parity odd/even
#define UCR2_STPB	(1<<6)	// Stop
#define UCR2_WS		(1<<5)	// Word size
#define UCR2_RTSEN	(1<<4)	// Request to send interrupt enable
#define UCR2_TXEN	(1<<2)	// Transmitter enabled
#define UCR2_RXEN	(1<<1)	// Receiver enabled
#define UCR2_SRST	(1<<0)	// SW reset
#define UCR3_DTREN	(1<<13) // DTR interrupt enable
#define UCR3_PARERREN	(1<<12) // Parity enable
#define UCR3_FRAERREN	(1<<11) // Frame error interrupt enable
#define UCR3_DSR	(1<<10) // Data set ready
#define UCR3_DCD	(1<<9)  // Data carrier detect
#define UCR3_RI		(1<<8)  // Ring indicator
#define UCR3_ADNIMP	(1<<7)  // Autobaud Detection Not Improved
#define UCR3_RXDSEN	(1<<6)  // Receive status interrupt enable
#define UCR3_AIRINTEN	(1<<5)  // Async IR wake interrupt enable
#define UCR3_AWAKEN	(1<<4)  // Async wake interrupt enable
#define UCR3_REF25	(1<<3)  // Ref freq 25 MHz
#define UCR3_REF30	(1<<2)  // Ref Freq 30 MHz
#define UCR3_INVT	(1<<1)  // Inverted Infrared transmission
#define UCR3_BPEN	(1<<0)  // Preset registers enable
#define UCR4_CTSTL_32	(32<<10) // CTS trigger level (32 chars)
#define UCR4_INVR	(1<<9)  // Inverted infrared reception
#define UCR4_ENIRI	(1<<8)  // Serial infrared interrupt enable
#define UCR4_WKEN	(1<<7)  // Wake interrupt enable
#define UCR4_REF16	(1<<6)  // Ref freq 16 MHz
#define UCR4_IRSC	(1<<5)  // IR special case
#define UCR4_TCEN	(1<<3)  // Transmit complete interrupt enable
#define UCR4_BKEN	(1<<2)  // Break condition interrupt enable
#define UCR4_OREN	(1<<1)  // Receiver overrun interrupt enable
#define UCR4_DREN	(1<<0)  // Recv data ready interrupt enable
#define UFCR_RXTL_SHF	0       // Receiver trigger level shift
#define UFCR_RFDIV	(7<<7)  // Reference freq divider mask
#define UFCR_RFDIV_SHF	7	// Reference freq divider shift
#define RFDIV		4	// divide input clock by 2
#define UFCR_DCEDTE	(1<<6)  // DTE mode select
#define UFCR_TXTL_SHF	10      // Transmitter trigger level shift
#define USR1_PARITYERR	(1<<15) // Parity error interrupt flag
#define USR1_RTSS	(1<<14) // RTS pin status
#define USR1_TRDY	(1<<13) // Transmitter ready interrupt/dma flag
#define USR1_RTSD	(1<<12) // RTS delta
#define USR1_ESCF	(1<<11) // Escape seq interrupt flag
#define USR1_FRAMERR	(1<<10) // Frame error interrupt flag
#define USR1_RRDY	(1<<9)	// Receiver ready interrupt/dma flag
#define USR1_TIMEOUT	(1<<7)	// Receive timeout interrupt status
#define USR1_RXDS	(1<<6)	// Receiver idle interrupt flag
#define USR1_AIRINT	(1<<5)	// Async IR wake interrupt flag
#define USR1_AWAKE	(1<<4)	// Aysnc wake interrupt flag
#define USR2_ADET	(1<<15) // Auto baud rate detect complete
#define USR2_TXFE	(1<<14) // Transmit buffer FIFO empty
#define USR2_DTRF	(1<<13) // DTR edge interrupt flag
#define USR2_IDLE	(1<<12) // Idle condition
#define USR2_IRINT	(1<<8)	// Serial infrared interrupt flag
#define USR2_WAKE	(1<<7)	// Wake
#define USR2_RTSF	(1<<4)	// RTS edge interrupt flag
#define USR2_TXDC	(1<<3)	// Transmitter complete
#define USR2_BRCD	(1<<2)	// Break condition
#define USR2_ORE	(1<<1)	// Overrun error
#define USR2_RDR	(1<<0)	// Recv data ready
#define UTS_FRCPERR	(1<<13) // Force parity error
#define UTS_LOOP	(1<<12) // Loop tx and rx
#define UTS_TXEMPTY	(1<<6)	// TxFIFO empty
#define UTS_RXEMPTY	(1<<5)	// RxFIFO empty
#define UTS_TXFULL	(1<<4)	// TxFIFO full
#define UTS_RXFULL	(1<<3)	// RxFIFO full
#define UTS_SOFTRS	(1<<0)	// Software reset
#define TXTL		2  // reset default
#define RXTL		1  // reset default

static void surface_serial_init(void)
{
	writel(0, &mxc_uart_base_surface->cr1);
	writel(0, &mxc_uart_base_surface->cr2);

	while (!(readl(&mxc_uart_base_surface->cr2) & UCR2_SRST));

	writel(0x704 | UCR3_ADNIMP, &mxc_uart_base_surface->cr3);

	writel(0x704 | UCR3_ADNIMP, &mxc_uart_base_surface->cr3);
	writel(0x8000, &mxc_uart_base_surface->cr4);
	writel(0x2b, &mxc_uart_base_surface->esc);
	writel(0, &mxc_uart_base_surface->tim);

	writel(0, &mxc_uart_base_surface->ts);
}

static void surface_serial_setbaudrate(unsigned long clk, unsigned long baudrate)
{
	u32 tmp;

	tmp = RFDIV << UFCR_RFDIV_SHF;
	tmp |= (TXTL << UFCR_TXTL_SHF) | (RXTL << UFCR_RXTL_SHF);
	writel(tmp, &mxc_uart_base_surface->fcr);

	writel(0xf, &mxc_uart_base_surface->bir);
	writel(clk / (2 * baudrate), &mxc_uart_base_surface->bmr);

	writel(UCR2_WS | UCR2_IRTS | UCR2_RXEN | UCR2_TXEN | UCR2_SRST, &mxc_uart_base_surface->cr2);
	writel(UCR1_UARTEN, &mxc_uart_base_surface->cr1);
}

void surface_serial_putc(const char c)
{
	writel(c, &mxc_uart_base_surface->txd);

	// wait for transmitter to be ready
	while (!(readl(&mxc_uart_base_surface->ts) & UTS_TXEMPTY))
		WATCHDOG_RESET();
}

uint8_t surface_calculate_checksum(char* data, uint16_t len) {
	// a single message can contain up to max. 64 chars
	int32_t sum = 0xFE;
	for (uint8_t i = 0; i < (len-1); i++) {
	sum -= data[i];
	}
	sum -= (len - 3); // remove 2-byte HEADER (0xFE 0x8i) and 1-byte end (0xFE)

	// write the calculated sum to the last element of the array
	return (sum & 0x7F);
}

void surface_set_led(uint8_t boardId, uint8_t index, bool ledOn) {
	char buf[7];
	uint8_t len = 6;
	
	buf[0] = 0xFE; // startbyte
	buf[1] = 0x80 + boardId;
	buf[2] = 'L'; // LED
	buf[3] = 0x80; // index fixed at 0x80 for LEDs
	if (ledOn) {
		buf[4] = index + 0x80;
	}else{
		buf[4] = index;
	}
	
	buf[5] = 0xFE;
	buf[6] = surface_calculate_checksum(&buf[0], len);
	
	// send data over ttymxc1 (UART2)
	for (uint8_t i = 0; i < (len + 1); i++) {
		surface_serial_putc(buf[i]);
	}
}

void surface_set_contrast(uint8_t boardId, uint8_t contrast) {
	char buf[7];
	uint8_t len = 6;
	
	buf[0] = 0xFE; // startbyte
	buf[1] = 0x80 + boardId;
	buf[2] = 'C'; // ControlMessage
	buf[3] = 'C'; // index
	buf[4] = contrast & 0x3F;
	
	buf[5] = 0xFE; // endbyte
	buf[6] = surface_calculate_checksum(&buf[0], len);
	
	// send data over ttymxc1 (UART2)
	for (uint8_t i = 0; i < (len + 1); i++) {
		surface_serial_putc(buf[i]);
	}
}

void surface_set_brightness(uint8_t boardId, uint8_t brightness) {
	char buf[7];
	uint8_t len = 6;
	
	buf[0] = 0xFE; // startbyte
	buf[1] = 0x80 + boardId;
	buf[2] = 'C'; // ControlMessage
	buf[3] = 'B'; // index
	buf[4] = brightness;
	
	buf[5] = 0xFE; // endbyte
	buf[6] = surface_calculate_checksum(&buf[0], len);
	
	// send data over ttymxc1 (UART2)
	for (uint8_t i = 0; i < (len + 1); i++) {
		surface_serial_putc(buf[i]);
	}
}

void surface_set_lcd(uint8_t boardId, uint8_t index, uint8_t color) {
	// boardId = 0, 4, 5, 8
	// index = 0 ... 8
	// color = 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE

	char buf[32];
	uint8_t len = 31;
	
	buf[0] = 0xFE; // startbyte
	buf[1] = 0x80 + boardId;
	buf[2] = 'D'; // display
	buf[3] = index;
	buf[4] = color & 0x0F; // use only 4 bits (bit 0=R, 1=G, 2=B, 3=Inverted)
	
	// begin of script
	buf[5] = 0xA0; // icon: 0xA0 -> no icon
	buf[6] = 0; // icon X
	buf[7] = 0; // icon Y
	
	// transmit first text
	buf[8] = 0x20 + 8; // small text = 0x00, large text = 0x20
	buf[9] = 0; // text X
	buf[10] = 0; // text Y
	buf[11] = 'O';
	buf[12] = 'p';
	buf[13] = 'e';
	buf[14] = 'n';
	buf[15] = 'X';
	buf[16] = '3';
	buf[17] = '2';
	buf[18] = ' ';

	// transmit second text
	buf[19] = 0x00 + 8; // small text = 0x00, large text = 0x20
	buf[20] = 0; // text X
	buf[21] = 30; // text Y
	buf[22] = 'B';
	buf[23] = 'o';
	buf[24] = 'o';
	buf[25] = 't';
	buf[26] = 'i';
	buf[27] = 'n';
	buf[28] = 'g';
	buf[29] = ' ';
	
	buf[30] = 0xFE; // endbyte
	buf[31] = surface_calculate_checksum(&buf[0], len);
	
	// send data over ttymxc1 (UART2)
	for (uint8_t i = 0; i < (len + 1); i++) {
		surface_serial_putc(buf[i]);
	}
}

/////////////////////////////////////////////////
// end of functions for mxc-uart from serial_mxc.c
/////////////////////////////////////////////////

int board_late_init(void)
{
	mx25pdk_fec_init();
	
	// init ttymxc1 (UART2) for surface-elements like LCDs, LEDs, faders and encoders
	surface_serial_init();
	u32 clk = imx_get_uartclk();
	surface_serial_setbaudrate(clk, 115200);

	// enable X32_LED_IN = 0x10
	surface_set_led(0x00, 0x10, true); // boardId, LED-index, state

	// set first LCD on board 0 and display text "OpenX32! Booting"
	surface_set_brightness(8, 0xC0); // default brightness
	surface_set_contrast(8, 40); // default
	surface_set_lcd(8, 8, 0b00000111); // boardId=0=R, index=8=Mainfader, color (bit 0=R, 1=G, 2=B, 3=Inverted)

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
