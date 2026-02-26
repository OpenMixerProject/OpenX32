#ifndef DEFINES_H_
#define DEFINES_H_

#define DSP_VERSION				0.2
#define FX_USE_UPMIXER			0	// enables a full-featured stereo-decompositing and 5.1-surround-upmixing effect (all other effects will be disabled)
#define FX_USE_MATRIXUPMIXER	0	// enables a nice but simple stereo-to-5.1-surround matrix-upmixer (all other effects will be disabled)

// DSP2 receives 24 Audio-channels from DSP1
// Channel 1-8		-> FX Sends 1-8
// Channel 9-16		-> FX Sends 9-16
// Channel 17-24	-> Aux 1-8
//    Aux Ch 1-2	-> Output to i.MX25 L/R
//    Aux Ch 3-4	-> Output to AES/EBU XLR L/R
//    Aux Ch 5-6	-> Unused
//    Aux Ch 7-8	-> Channel 7 unused / Channel 8 = RTA-Source

// DSP2 sends 24 Audio-channels back to DSP1
// Channel 1-8		-> FX-Return 1-8 of FX 1-4
// Channel 9-16		-> FX-Return 9-16 of FX 5-8
// Channel 17-24	-> Aux 1-8
//	  Aux Ch 1-2	->
//	  Aux Ch 3-4	->
//	  Aux Ch 5-6	->
//	  Aux Ch 7-8	->

// input buffer indices
#define DSP_BUF_IDX_FXA				0	// FX-Sends 1-8
#define DSP_BUF_IDX_FXB				8	// FX-Sends 9-16
#define DSP_BUF_IDX_IMXOUT_LEFT		16  // Audio to iMX25 Left
#define DSP_BUF_IDX_IMXOUT_RIGHT	17  // Audio to iMX25 Right
#define DSP_BUF_IDX_SPDIF_LEFT		18  // Audio to SPDIF-XLR Left
#define DSP_BUF_IDX_SPDIF_RIGHT		19  // Audio to SPDIF-XLR Right
//#define DSP_BUF_IDX_UNUSED		20	// unused input-channel
//#define DSP_BUF_IDX_UNUSED		21	// unused input-channel
//#define DSP_BUF_IDX_UNUSED		22	// unused input-channel
#define DSP_BUF_IDX_RTA_SOURCE		23  // Audio to RTA-Analyzer

// output buffer indices
#define DSP_BUF_IDX_IMXIN_LEFT		16  // Audio from iMX25 Left
#define DSP_BUF_IDX_IMXIN_RIGHT		17  // Audio from iMX25 Right
#define DSP_BUF_IDX_OSC_LEFT		18  // Audio from Oscillator Left
#define DSP_BUF_IDX_OSC_RIGHT		19  // Audio from Oscillator Right
//#define DSP_BUF_IDX_UNUSED		20  // unused output-channel
//#define DSP_BUF_IDX_UNUSED		21  // unused output-channel
//#define DSP_BUF_IDX_UNUSED		22  // unused output-channel
//#define DSP_BUF_IDX_UNUSED		23  // unused output-channel

#define USE_SPI_TXD_MODE		2 // 0 = CoreWrite, 1 = DMA Single, 2 = DMA-Chaining

#define SDRAM_START  			0x04000000	// start address of SDRAM on Bank1 (nMS1)
#define SDRAM_AUDIO_START  		0x04200000	// start address of audio-data in SDRAM on Bank1 (nMS1)
#define SDRAM_AUDIO_SIZE_BYTE	(14 * 1024 * 1024)	// we are using 1MB for external program-code, 1MB for system-data and remaining 14MB for audio-data
// we are slicing the external 16MB SDRAM into parts for each effect-slot
//#define SDRAM_FX_x			(SDRAM_START + (x * (SDRAM_SIZE_BYTE / 8)))

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

#define SAMPLERATE_MAX			48000	// this is not the current samplerate, but the maximum supported to allocate correct amount of memory

// SPDIF configuration			Input-Clock is 24.576 MHz on DAI4
#define CLKA_DIVIDER			8	// provides SCLK (3.072 MHz) serial clock to S/PDIF RX/TX and SPORTs
#define FSA_DIVIDER				512	// provides FrameSync (48kHz) to S/PDIF RX/TX and SPORTs
#define CLKB_DIVIDER			2 	// provides HFCLK (12.288 MHz) to S/PDIF RX/TX

// some system-defines
#define DO_CYCLE_COUNTS				// enable cycle counter

// defines for memory-access
// our external RAM is sliced into multiple regions:
// 1) external program-code
// 2) external data-memory for variables  -> defined as "em" afterwards
// 3) external data-memory for audio-data -> defined as "am" afterwards
#define em						section("seg_ext_data")		// pm = ProgramMemory, dm = DataMemory, em = ExternalMemory, am = AudioMemory
#define am						section("seg_ext_audio")	// pm = ProgramMemory, dm = DataMemory, em = ExternalMemory, am = AudioMemory

#endif /* DEFINES_H_ */
