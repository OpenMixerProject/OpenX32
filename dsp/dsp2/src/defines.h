#ifndef DEFINES_H_
#define DEFINES_H_

#define DSP_VERSION				0.05

// DSP2 receives 24 Audio-channels
// Channel 1-8		-> FX Sends 1-8
// Channel 9-16		-> FX Sends 9-16
// Channel 17-24	-> Aux 1-8
//    Aux Ch 1-2	-> USB L/R
//    Aux Ch 3-4	-> SPDIF L/R
//    Aux Ch 5-6	-> i.MX25 L/R
//    Aux Ch 7-8	-> Unused
#define DSP_BUF_IDX_FXA			1	// FX-Sends 1-8
#define DSP_BUF_IDX_FXB			9	// FX-Sends 9-16
#define DSP_BUF_IDX_USB_LEFT	17  // USB-Audio Left
#define DSP_BUF_IDX_USB_RIGHT	18  // USB-Audio Right
#define DSP_BUF_IDX_SPDIF_LEFT	19  // SPDIF-Audio Left
#define DSP_BUF_IDX_SPDIF_RIGHT	20  // SPDIF-Audio Right
#define DSP_BUF_IDX_IMX_LEFT	19  // iMX25-Audio Left
#define DSP_BUF_IDX_IMX_RIGHT	20  // iMX25-Audio Right

#define USE_SPI_TXD_MODE		0 // 0 = CoreWrite, 1 = DMA

#define SDRAM_START  			0x00200000	// start address of SDRAM
#define SDRAM_SIZE	 			0x00400000	// size of SDRAM in 32-bit words (16 MiB)

#define MAX_CHAN				24
#define MAX_CHAN_EQS			4
#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				3
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define M_PI					3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_MAX_RX_PAYLOAD_SIZE	30  // 27 int-values + * + # + parameter
#define SPI_RX_BUFFER_SIZE		(SPI_MAX_RX_PAYLOAD_SIZE * 3)  // store up to 3 payload-sets
#define SPI_TX_BUFFER_SIZE		200 // transmit up to 200 values

// SPDIF configuration
#define CLKA_DIVIDER			8	// provides SCLK serial clock to S/PDIF TX and SPORT0
#define FSA_DIVIDER				512	// provides Frame Sync to S/PDIF TX and SPORT0
#define CLKB_DIVIDER			2 	// provides HFCLK to S/PDIF TX

#define DO_CYCLE_COUNTS				// enable cycle counter

#endif /* DEFINES_H_ */
