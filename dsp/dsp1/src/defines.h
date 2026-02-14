#ifndef DEFINES_H_
#define DEFINES_H_

#define DSP_VERSION				0.43

#define DEBUG_DISABLE_LOWCUT	0
#define DEBUG_DISABLE_GATE		0
#define DEBUG_DISABLE_EQ		0
#define DEBUG_DISABLE_DYNAMICS	0
#define DEBUG_DISABLE_MIXBUS	0
#define DEBUG_DISABLE_EQMAIN	1
#define DEBUG_DISABLE_DELAYLINE	1

#define TEST_MATRIXMULTIPLICATION_MIXBUS 0

#define ACTIVE_MIX_BUSSES		8

#define DSP_BUF_IDX_OFF			0	// no audio
#define DSP_BUF_IDX_DSPCHANNEL	1	// DSP-Channel 1-32
#define DSP_BUF_IDX_AUX			33	// Aux-Channel 1-8
#define DSP_BUF_IDX_DSP2_FX		41  // FXDSP2 FX-Channel 1-16
#define DSP_BUF_IDX_MIXBUS		57	// Mixbus 1-16
#define DSP_BUF_IDX_MAINLEFT	73	// main left
#define DSP_BUF_IDX_MAINRIGHT	74	// main right
#define DSP_BUF_IDX_MAINSUB		75	// main sub
#define DSP_BUF_IDX_MATRIX		76	// Matrix 1-6
#define DSP_BUF_IDX_DSP2_AUX	82	// FXDSP2 AUX-Channel 1-8
#define DSP_BUF_IDX_MONLEFT		90	// Monitor Left
#define DSP_BUF_IDX_MONRIGHT	91	// Monitor Right
#define DSP_BUF_IDX_TALKBACK	92	// Talkback

#define USE_SPI_TXD_MODE		2 // 0 = CoreWrite, 1 = DMA Single, 2 = DMA-Chaining

#define SDRAM_START  			0x04000000	// start address of SDRAM
#define SDRAM_SIZE	 			0x00400000	// size of SDRAM in 32-bit words (16 MiB)

#define MAX_CHAN_FPGA			40	// total channels that are received from FPGA
#define MAX_CHAN_DSP2			24	// total channels that are received from DSP2
#define MAX_DSP2_FXRETURN		8	// FX Return-Channels from DSP2
#define MAX_DSP2_FXINSERT		8	// FX Insert-Channels from DSP2
#define MAX_DSP2_AUX			8	// AUX-Return-Channels from DSP2
#define MAX_MIXBUS				16	// internal Mixbus-Channels
#define MAX_MATRIX				6	// internal Matrix-Channels
#define MAX_MAIN				3	// internal Mainbus-Channels
#define MAX_MONITOR				3	// internal Monitor-Channels

#define MAX_CHAN_EQS			4
#define CHANNELS_WITH_4BD_EQ	(MAX_CHAN_FPGA + MAX_DSP2_FXRETURN + MAX_MAIN)	// consecutive channels that are using a 4-band PEQ
#define CHANNELS_WITH_6BD_EQ	(MAX_MIXBUS)	// consecutive channels that are using a 6-band PEQ
#define MAX_CHAN_FULLFEATURED	32	// depending on the overall load not all channels can be full-featured (gate + dynamics)
#define MAX_CHAN_REDUCED		(MAX_CHAN_FPGA + MAX_DSP2_FXRETURN - MAX_CHAN_FULLFEATURED)	// depending on the overall load not all channels can be full-featured (gate + dynamics)

#define CHANNELS_PER_TDM		8
#define TDM_INPUTS_FPGA			(MAX_CHAN_FPGA / CHANNELS_PER_TDM)
#define TDM_INPUTS_DSP2			3
#define TDM_INPUTS				(TDM_INPUTS_FPGA + TDM_INPUTS_DSP2)
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define M_PI					3.1415926535897932384626433832795f
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_MAX_RX_PAYLOAD_SIZE	30  // 27 int-values + * + # + parameter
#define SPI_RX_BUFFER_SIZE		(SPI_MAX_RX_PAYLOAD_SIZE * 2)  // store up to 2 payload-sets
#define SPI_TX_BUFFER_SIZE		200 // transmit up to 200 values - must be dividable by 2!

#define SAMPLERATE_MAX			48000
#define	DELAYLINE_LENGTH_MS		500 // 500ms
#define SAMPLES_IN_DELAYLINE	((SAMPLERATE_MAX * DELAYLINE_LENGTH_MS) / 1000)

#define audioVolumeSmootherCoeff 0.01f // (30.0f / (48kHz/16 Samples)) = 0.01f

#define em						section("seg_ext_data")		// pm = ProgramMemory, dm = DataMemory, em = ExternalMemory
#define DO_CYCLE_COUNTS				// enable cycle counter

#endif /* DEFINES_H_ */
