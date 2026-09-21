#ifndef DEFINES_H_
#define DEFINES_H_

#include <limits.h>

#define FX_USE_REVERB				1	// this effect takes a lot of ressource so that Multiband-Compressor and DynamicEQ cannot be used at the moment
#define FX_USE_DEFEEDBACK			1
#define FX_USE_MULTIBANDCOMPRESSOR	0	// this effect takes a lot of ressources and cannot be used together with reverb
#define FX_USE_DYNAMICEQ			0	// this effect takes a lot of ressources and cannot be used together with reverb

#define FX_USE_UPMIXER				0	// enables a stereo-decompositing and 5.1-surround-upmixing effect (all other effects will be disabled)

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
#define DSP_BUF_IDX_UPMIX_CENTER	18  // Audio from Surround-Upmixer
#define DSP_BUF_IDX_UPMIX_BACKLEFT	19  // Audio from Surround-Upmixer
#define DSP_BUF_IDX_UPMIX_BACKRIGHT	20  // Audio from Surround-Upmixer
#define DSP_BUF_IDX_UPMIX_LFE		21  // Audio from Surround-Upmixer
#define DSP_BUF_IDX_OSC_LEFT		22  // Audio from Oscillator Left
#define DSP_BUF_IDX_OSC_RIGHT		23  // Audio from Oscillator Right

#define USE_SPI_TXD_MODE		2 // 0 = CoreWrite, 1 = DMA Single, 2 = DMA-Chaining

#define SDRAM_START_WORD_ADDRESS			0x04000000U	// first 32-bit SHARC word in SDRAM Bank 1 (nMS1)
#define SDRAM_AUDIO_START_WORD_ADDRESS	0x04080000U	// first 32-bit SHARC word reserved for FX audio
#define SDRAM_AUDIO_END_WORD_ADDRESS		0x043FFFFFU	// final 32-bit SHARC word reserved for FX audio (inclusive)
#define SDRAM_AUDIO_CAPACITY_WORDS		(SDRAM_AUDIO_END_WORD_ADDRESS - SDRAM_AUDIO_START_WORD_ADDRESS + 1U)
#define SDRAM_AUDIO_SLOT_COUNT			8U
#define SDRAM_AUDIO_SLOT_CAPACITY_WORDS	(SDRAM_AUDIO_CAPACITY_WORDS / SDRAM_AUDIO_SLOT_COUNT)

#if SDRAM_AUDIO_CAPACITY_WORDS <= 0
	#error "DSP2 audio SDRAM capacity must be positive"
#endif
#if (SDRAM_AUDIO_CAPACITY_WORDS % SDRAM_AUDIO_SLOT_COUNT) != 0
	#error "DSP2 audio SDRAM must divide exactly into eight FX slots"
#endif
#if (SDRAM_AUDIO_START_WORD_ADDRESS + (SDRAM_AUDIO_SLOT_COUNT * SDRAM_AUDIO_SLOT_CAPACITY_WORDS) - 1U) != SDRAM_AUDIO_END_WORD_ADDRESS
	#error "DSP2 final FX slot must end at the audio SDRAM boundary"
#endif

typedef char dsp2_float_must_be_32_bits[((sizeof(float) * CHAR_BIT) == 32) ? 1 : -1];
#if CHAR_BIT == 32
typedef char dsp2_float_must_be_one_sharc_word[(sizeof(float) == 1) ? 1 : -1];
#endif

#define MAX_CHAN				24
#define MAX_CHAN_EQS			4
#define CHANNELS_PER_TDM		8
#define TDM_INPUTS				3
#define SAMPLES_IN_BUFFER		16
#define BUFFER_COUNT			2	// single-, double-, triple- or multi-buffering (e.g. for delay or other things)
#define BUFFER_SIZE				SAMPLES_IN_BUFFER * CHANNELS_PER_TDM
#define M_PI					3.1415926535897932384626433832795f
#define INT32_TO_FLOAT_NORM		(1.0f / 2147483647.0f)
#define SRUDEBUG  					// Check SRU Routings for errors. Can be removed on final design
#define PCI						(1 << 19)	//0x00080000
#define OFFSET_MASK				0x7FFFF
#define SPI_MAX_RX_PAYLOAD_SIZE	45  // 41 int-values + * + # + parameter
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
// External SDRAM is partitioned into ordinary data and audio data. External
// execution is intentionally not used until the X32 boot path is proven safe.
#define em						section("seg_ext_data")		// pm = ProgramMemory, dm = DataMemory, em = ExternalMemory, am = AudioMemory
#define am						section("seg_ext_audio")	// pm = ProgramMemory, dm = DataMemory, em = ExternalMemory, am = AudioMemory

#endif /* DEFINES_H_ */
