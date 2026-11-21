/* hw_spi.h - OpenX32 Linux spidev backend for OpenInferno inferno-portd
 *
 * Bridges inferno-portd's 32-bit register accesses over the X32 i.MX25 <-> FPGA
 * SPI bus (fpga/spi_reg_bridge.vhd):
 *   Write (7 bytes): [0xA5] [ADDR_HI] [ADDR_LO] [D31..24] [D23..16] [D15..8] [D7..0]
 *   Read  (8 bytes): [0x5A] [ADDR_HI] [ADDR_LO] [0x00]    [R31..24] [R23..16] [R15..8] [R7..0]
 *
 * OpenInferno window sits at FPGA register base 0x1000..0x1FFF:
 *   0x1000..0x10FF : aoip_regs
 *   0x1100..0x11FF : inferno_regs (MAXLAT=1 / 1 ms, MAX_CH=48)
 *   0x1200..0x12FF : cpu_pktq
 *   0x1300..0x13FF : fs_phase
 *
 * License: GPL-3.0-or-later
 */
#ifndef OPENX32_HW_SPI_H
#define OPENX32_HW_SPI_H

#include <stdint.h>
#include "hw.h"

/* Open an OpenX32 FPGA SPI register backend (e.g. dev="/dev/spidev0.0", speed_hz=8000000).
 * Configures port_base=0x1000, clock_base=0x1000, dclk_base=0x1300, cs2k_base=0xFFFFFFFF. */
hw_t *hw_spi_open(const char *spidev_path, uint32_t speed_hz);

/* Convenience helpers for OpenX32 system / Dual-AES50 SPI registers (0x0000..0x00FF) */
uint32_t x32_spi_rd_sys(hw_t *h, uint16_t addr);
void     x32_spi_wr_sys(hw_t *h, uint16_t addr, uint32_t val);

#endif
