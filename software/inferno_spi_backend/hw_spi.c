/* hw_spi.c - OpenX32 Linux spidev backend for OpenInferno inferno-portd
 *
 * Speaks the 32-bit SPI register protocol implemented in fpga/spi_reg_bridge.vhd.
 * Backward-compatible with legacy 16-bit OpenX32 routing writes on the same bus.
 *
 * License: GPL-3.0-or-later
 */
#include "hw_spi.h"

#include <fcntl.h>
#include <linux/spi/spidev.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define X32_SPI_CMD_WRITE   0xA5u
#define X32_SPI_CMD_READ    0x5Au
#define X32_INFERNO_BASE    0x1000u
#define X32_FSPHASE_BASE    0x1300u

typedef struct {
    int      fd;
    uint32_t speed_hz;
} spi_priv_t;

static int spi_xfer(spi_priv_t *p, const uint8_t *tx, uint8_t *rx, uint32_t len)
{
    struct spi_ioc_transfer tr;
    memset(&tr, 0, sizeof(tr));
    tr.tx_buf        = (unsigned long)tx;
    tr.rx_buf        = (unsigned long)rx;
    tr.len           = len;
    tr.speed_hz      = p->speed_hz;
    tr.bits_per_word = 8;
    return ioctl(p->fd, SPI_IOC_MESSAGE(1), &tr);
}

static uint32_t spi_rd_raw(hw_t *h, uint32_t addr)
{
    spi_priv_t *p = (spi_priv_t *)h->priv;
    uint8_t tx[8] = {
        X32_SPI_CMD_READ,
        (uint8_t)((addr >> 8) & 0xFFu),
        (uint8_t)(addr & 0xFFu),
        0x00u, 0x00u, 0x00u, 0x00u, 0x00u
    };
    uint8_t rx[8] = {0};

    if (addr > 0xFFFFu) return 0;
    if (spi_xfer(p, tx, rx, sizeof(tx)) < 0) return 0;

    return ((uint32_t)rx[4] << 24) |
           ((uint32_t)rx[5] << 16) |
           ((uint32_t)rx[6] <<  8) |
           ((uint32_t)rx[7]);
}

static void spi_wr_raw(hw_t *h, uint32_t addr, uint32_t v)
{
    spi_priv_t *p = (spi_priv_t *)h->priv;
    uint8_t tx[7] = {
        X32_SPI_CMD_WRITE,
        (uint8_t)((addr >> 8) & 0xFFu),
        (uint8_t)(addr & 0xFFu),
        (uint8_t)((v >> 24) & 0xFFu),
        (uint8_t)((v >> 16) & 0xFFu),
        (uint8_t)((v >>  8) & 0xFFu),
        (uint8_t)(v & 0xFFu)
    };
    uint8_t rx[7] = {0};

    if (addr > 0xFFFFu) return;
    (void)spi_xfer(p, tx, rx, sizeof(tx));
}

static int  spi_pollfd(hw_t *h)  { (void)h; return -1; }
static void spi_service(hw_t *h) { (void)h; }

static void spi_close(hw_t *h)
{
    spi_priv_t *p = (spi_priv_t *)h->priv;
    if (p) {
        if (p->fd >= 0) close(p->fd);
        free(p);
    }
    free(h);
}

static const struct hw_ops spi_ops = {
    spi_rd_raw,
    spi_wr_raw,
    spi_pollfd,
    spi_service,
    spi_close
};

hw_t *hw_spi_open(const char *spidev_path, uint32_t speed_hz)
{
    uint8_t mode = SPI_MODE_0;
    uint8_t bits = 8;

    int fd = open(spidev_path, O_RDWR);
    if (fd < 0) return NULL;

    if (ioctl(fd, SPI_IOC_WR_MODE, &mode) < 0 ||
        ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits) < 0 ||
        ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed_hz) < 0) {
        close(fd);
        return NULL;
    }

    spi_priv_t *p = (spi_priv_t *)calloc(1, sizeof(*p));
    hw_t *h       = (hw_t *)calloc(1, sizeof(*h));
    if (!p || !h) {
        free(p);
        free(h);
        close(fd);
        return NULL;
    }

    p->fd         = fd;
    p->speed_hz   = speed_hz;
    h->ops        = &spi_ops;
    h->priv       = p;
    h->port_base  = X32_INFERNO_BASE;
    h->clock_base = X32_INFERNO_BASE;
    h->dclk_base  = X32_FSPHASE_BASE;
    h->cs2k_base  = 0xFFFFFFFFu;
    return h;
}

uint32_t x32_spi_rd_sys(hw_t *h, uint16_t addr)
{
    return spi_rd_raw(h, (uint32_t)addr);
}

void x32_spi_wr_sys(hw_t *h, uint16_t addr, uint32_t val)
{
    spi_wr_raw(h, (uint32_t)addr, val);
}
