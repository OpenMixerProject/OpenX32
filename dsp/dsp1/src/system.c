#include "system.h"

#define SDMODIFY 1

void systemPllInit()
{
	int i, pmctlsetting;

	pmctlsetting = *pPMCTL;
	pmctlsetting &= ~(0xFF); // clear pmctlsetting

	// Core clock = 16MHz * (16/1) = 256 MHz
	pmctlsetting = (PLLM16 | PLLD1 | DIVEN | SDCKR2); // multiplier=16 | divider=1 | divider enabled | SD-Clock-Ratio=2=128MHz
	*pPMCTL = pmctlsetting;
}

void systemExternalMemoryInit()
{
	// Programming SDRAM control registers and enabling SDRAM read optimization
	// RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
	// (131.072*(10^6)*64*(10^-3)/4096) - (6+3) = 2039 = 0x7F7
	*pSDRRC = (0x7F7) | (SDMODIFY << 17) | SDROPT;

	/* Configure SDRAM Control Register (SDCTL) for PART MT48LC4M32B2
	  SDCL3  : SDRAM CAS Latency= 3 cycles
	  DSDCLK1: Disable SDRAM Clock 1
	  SDPSS  : Start SDRAM Power up Sequence
	  SDCAW8 : SDRAM Bank Column Address Width= 8 bits
	  SDRAW12: SDRAM Row Address Width= 12 bits
	  SDTRAS7: SDRAM tRAS Specification. Active Command delay = 6 cycles
	  SDTRP3 : SDRAM tRP Specification. Precharge delay = 3 cycles.
	  SDTWR2 : SDRAM tWR Specification. tWR = 2 cycles.
	  SDTRCD3: SDRAM tRCD Specification. tRCD = 3 cycles.
	 */

	*pSDCTL = SDCL3 | DSDCLK1 | SDPSS | SDCAW9 | SDRAW12 | SDTRAS6 | SDTRP3 | SDTWR2 | SDTRCD3 | X16DE;

	// Note that MS2 & MS3 pin multiplexed with flag2 & flag3
	// MSEN bit must be enabled to access SDRAM
	*pSYSCTL |= MSEN;

	// Mapping Bank 0 to SDRAM
	// MS2 is connected to chip select of 16-bit SDRAM device
	*pEPCTL |= B0SD;
	*pEPCTL &= ~(B1SD | B2SD | B3SD);

	/*  Configure AMI Control Register (AMICTL) Bank 1 for the AMD AM29LV08
		WS23 : Wait States= 23 cycles
		AMIEN: Enable AMI
		BW8  : External Data Bus Width= 8 bits*/
	*pAMICTL1 = WS23 | AMIEN | BW8; // Flash Settings
}

void systemSruInit(void) {
	// name-convention: output/input is relative to signals peripheral
	// _O of pinbuffer is used for internal connections of an external input pin
	// _I of pinbuffer is used for internal connections of an external output pin
	//
	// SPORT0_CLK_O
	//    |    |  |_______ direction relative to signals peripherals
	//    |    |__________ signal function
	//    |_______________ peripheral
	//
	//
	// o--- DAI_PBxx_O ---------------\
	//                                |
	//                    ___         |
	//                   |   ___      |
	//                   |      ___   |
	// o--- DAI_PBxx_I --|      ___---o---o External DAI pin buffer
	//                   |   ___
	//                   |___ |
	//                        |
	// o--- PBENxx_I ---------|
	//

	// used NOP() after each SRU(...) is suggested in Processor Hardware Reference v2.2 on page 6-21

	// setting up pins for LED-control via Flag-signals
	// =======================================
	// flags 4 to 15 are supported for DPI. Flags 0 to 3 not available on the DPI
	SRU(FLAG7_O, DPI_PB07_I); // connect output of Flag7 to input of DPI-PinBuffer 7 (LED on DPI7)
	NOP();
	SRU(HIGH, DPI_PBEN07_I); // set Pin-Buffer to output (HIGH=Output, LOW=Input)
	NOP();
	sysreg_bit_set(sysreg_FLAGS, FLG7O); // set flag-pins
	sysreg_bit_clr(sysreg_FLAGS, FLG7); // clear flag-pins



	// setting up DAI
	// =======================================
	// route Framesync to SPORT
	SRU(DAI_PB01_O, SPORT0_FS_I);  // TDM FS    -> SPORT0 FS
	NOP();
	SRU(DAI_PB01_O, SPORT1_FS_I);  // TDM FS    -> SPORT1 FS
	NOP();
	SRU(DAI_PB01_O, SPORT2_FS_I);  // TDM FS    -> SPORT2 FS
	NOP();
	SRU(DAI_PB01_O, SPORT3_FS_I);  // TDM FS    -> SPORT3 FS
	NOP();
	SRU(DAI_PB01_O, SPORT4_FS_I);  // TDM FS    -> SPORT4 FS
	NOP();
	SRU(DAI_PB01_O, SPORT5_FS_I);  // TDM FS    -> SPORT5 FS
	NOP();
	SRU(DAI_PB01_O, SPORT6_FS_I);  // TDM FS    -> SPORT6 FS
	NOP();
	SRU(DAI_PB01_O, SPORT7_FS_I);  // TDM FS    -> SPORT7 FS
	NOP();

	// route Bitclock to SPORT
	SRU(DAI_PB02_O, SPORT0_CLK_I); // TDM BCLK  -> SPORT0 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT1_CLK_I); // TDM BCLK  -> SPORT1 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT2_CLK_I); // TDM BCLK  -> SPORT2 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT3_CLK_I); // TDM BCLK  -> SPORT3 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT4_CLK_I); // TDM BCLK  -> SPORT4 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT5_CLK_I); // TDM BCLK  -> SPORT5 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT6_CLK_I); // TDM BCLK  -> SPORT6 CLK
	NOP();
	SRU(DAI_PB02_O, SPORT7_CLK_I); // TDM BCLK  -> SPORT7 CLK
	NOP();

	// route data-inputs to SPORT
	SRU(DAI_PB11_O, SPORT1_DA_I);  // TDM IN0   -> SPORT1 DATA IN0
	NOP();
	SRU(DAI_PB12_O, SPORT1_DB_I);  // TDM IN1   -> SPORT1 DATA IN1
	NOP();
	SRU(DAI_PB13_O, SPORT3_DA_I);  // TDM IN2   -> SPORT3 DATA IN0
	NOP();
	SRU(DAI_PB14_O, SPORT3_DB_I);  // TDM IN3   -> SPORT3 DATA IN1
	NOP();
	SRU(DAI_PB19_O, SPORT5_DA_I);  // TDM INAUX -> SPORT5 DATA IN0
	NOP();
	//SRU(..., SPORT5_DB_I);  // unused for now
	//SRU(..., SPORT7_DA_I);  // unused for now
	//SRU(..., SPORT7_DB_I);  // unused for now

	// tie the pin buffer inputs LOW as these pins are input-pins and buffer is not used
	SRU(LOW, DAI_PB01_I); // TDM FS
	NOP();
	SRU(LOW, DAI_PB02_I); // TDM BCLK
	NOP();
	SRU(LOW, DAI_PB11_I); // TDM IN0
	NOP();
	SRU(LOW, DAI_PB12_I); // TDM IN1
	NOP();
	SRU(LOW, DAI_PB13_I); // TDM IN2
	NOP();
	SRU(LOW, DAI_PB14_I); // TDM IN3
	NOP();
	SRU(LOW, DAI_PB19_I); // TDM INAUX
	NOP();

	// tie the pin buffer enable inputs LOW to make DAI pins 1,2, 11-14 and 19 inputs
	SRU(LOW, PBEN01_I); // TDM FS
	NOP();
	SRU(LOW, PBEN02_I); // TDM BCLK
	NOP();
	SRU(LOW, PBEN11_I); // TDM IN0
	NOP();
	SRU(LOW, PBEN12_I); // TDM IN1
	NOP();
	SRU(LOW, PBEN13_I); // TDM IN2
	NOP();
	SRU(LOW, PBEN14_I); // TDM IN3
	NOP();
	SRU(LOW, PBEN19_I); // TDM INAUX
	NOP();

	// route SPORT to data-outputs
	SRU(SPORT0_DA_O, DAI_PB15_I); // SPORT0 DATA OUT0 -> TDM OUT0
	NOP();
	SRU(SPORT0_DB_O, DAI_PB16_I); // SPORT0 DATA OUT1 -> TDM OUT1
	NOP();
	SRU(SPORT2_DA_O, DAI_PB17_I); // SPORT2 DATA OUT0 -> TDM OUT2
	NOP();
	SRU(SPORT2_DB_O, DAI_PB18_I); // SPORT2 DATA OUT2 -> TDM OUT3
	NOP();
	SRU(SPORT4_DA_O, DAI_PB20_I); // SPORT4 DATA OUT0 -> TDM OUTAUX
	NOP();
	//SRU(SPORT4_DB_O, ...); // unused for now
	//SRU(SPORT6_DA_O, ...); // unused for now
	//SRU(SPORT6_DB_O, ...); // unused for now

	// tie the pin buffer enable inputs HIGH to make DAI pins 15-18 and 20 outputs
	SRU(HIGH, PBEN15_I); // TDM OUT0
	NOP();
	SRU(HIGH, PBEN16_I); // TDM OUT1
	NOP();
	SRU(HIGH, PBEN17_I); // TDM OUT2
	NOP();
	SRU(HIGH, PBEN18_I); // TDM OUT3
	NOP();
	SRU(HIGH, PBEN20_I); // TDM OUTAUX
	NOP();



	// setup SPI interface to i.MX25
	// ========================================
	// route SPI signals to i.MX25
	SRU(DPI_PB01_O, SPI_MOSI_I);	// Connect DPI PB1 to MOSI
	NOP();
	SRU(SPI_MISO_O, DPI_PB02_I);	// Connect MISO to DPI PB2
	NOP();
	SRU(DPI_PB03_O, SPI_CLK_I); 	// Connect DPI PB3 to SPI CLK
	NOP();
	SRU(DPI_PB04_O, SPI_DS_I);  	// Connect DPI PB4 to SPI ChipSelect
	NOP();

	SRU (LOW, DPI_PBEN01_I); 		// set to input
	NOP();
	SRU (HIGH, DPI_PBEN02_I); 		// set to output
	NOP();
	SRU (LOW, DPI_PBEN03_I); 		// set to input
	NOP();
	SRU (LOW, DPI_PBEN04_I); 		// set to input
	NOP();
}

void systemSportInit() {
	// see Processor Hardware Reference v2.2 page 7-57

	// clear multi-channel control-registers
	*pSPCTL0 = 0; // SPORT 0 control register
	*pSPCTL1 = 0;
	*pSPCTL2 = 0;
	*pSPCTL3 = 0;
	*pSPCTL4 = 0;
	*pSPCTL5 = 0;
	*pSPCTL6 = 0;
	*pSPCTL7 = 0;
	*pSPMCTL0 = 0; // SPORT 0 Multichannel Control Register
	*pSPMCTL1 = 0;
	*pSPMCTL2 = 0;
	*pSPMCTL3 = 0;
	*pSPMCTL4 = 0;
	*pSPMCTL5 = 0;
	*pSPMCTL6 = 0;
	*pSPMCTL7 = 0;
	*pDIV0 = 0x00000000;
	*pDIV1 = 0x00000000;
	*pDIV2 = 0x00000000;
	*pDIV3 = 0x00000000;
	*pDIV4 = 0x00000000;
	*pDIV5 = 0x00000000;
	*pDIV6 = 0x00000000;
	*pDIV7 = 0x00000000;

	// enable TDM8 transmitter
	// SPEN_A	Channel A enabled
	// SLEN32	Serial data length is 32 (only 24 bits are used here)
	// CKRE		Rising Edge of Clock
	// FSR		Frame sync required
	// SDEN_A	Enable serial port channel A DMA
	// SCHEN_A	Enable serial port channel A DMA chaining
	// SPTRAN	SPORT Channel A as transmitter
	*pSPCTL0  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	*pSPCTL0 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;
	*pSPCTL2  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	*pSPCTL2 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;
	*pSPCTL4  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	//*pSPCTL4 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;
	//*pSPCTL6  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	//*pSPCTL6 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;

	// enable TDM8 receiver
	// SPEN_A 	Channel A enabled
	// SLEN32 	Serial data length is 32 (only 24 bits are used here)
	// CKRE 	Rising Edge of Clock
	// FSR 		Frame sync required
	// SDEN_A 	Enable serial port channel A DMA
	// SCHEN_A	Enable serial port channel A DMA chaining
	*pSPCTL1  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	*pSPCTL1 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;
	*pSPCTL3  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	*pSPCTL3 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;
	*pSPCTL5  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	//*pSPCTL5 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;
	//*pSPCTL7  = SPEN_A | SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	//*pSPCTL7 |= SPEN_B | SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;

	// configure DMA memory
	// chained DMA is used to autoinitialize next DMA in line
	// IOP automatically loads new index, modify, and count values from a memory location
	// chain-pointer is 20-bit while the lower 19-bit are the memory address field (therefore 0x7FFFF)
	//
	// *pCPSP0A requires the address of tx0a_buf and settings 19th bit -> enabling the interrupt after the current TCB
	*pCPSP0A = (((unsigned int)&audioTx_tcb[0][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP0B = (((unsigned int)&audioTx_tcb[1][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP2A = (((unsigned int)&audioTx_tcb[2][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP2B = (((unsigned int)&audioTx_tcb[3][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP4A = (((unsigned int)&audioTx_tcb[4][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP4B = (((unsigned int)&audioTx_tcb[5][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP6A = (((unsigned int)&audioTx_tcb[6][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP6B = (((unsigned int)&audioTx_tcb[7][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory

	*pCPSP1A = (((unsigned int)&audioRx_tcb[0][0][0] + 3) & OFFSET_MASK) | PCI; // pointing DMA to desired memory and enable interrupt after every processed TCB (set PCI-bit)
	*pCPSP1B = (((unsigned int)&audioRx_tcb[1][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP3A = (((unsigned int)&audioRx_tcb[2][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP3B = (((unsigned int)&audioRx_tcb[3][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP5A = (((unsigned int)&audioRx_tcb[4][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP5B = (((unsigned int)&audioRx_tcb[5][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP7A = (((unsigned int)&audioRx_tcb[6][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	//*pCPSP7B = (((unsigned int)&audioRx_tcb[7][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory





	// set multichannel mode to 8 channels with 1 clock delay of framesync
	*pSPMCTL0 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL1 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL2 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL3 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL4 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL5 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL6 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL7 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay

	// enable transmit channels 0-39 (see Processor Hardware Reference v2.2 page 7-38)
	*pMT0CS0 = 0x0000FFFF; // SPORT 0 multichannel tx select, select channels 15-0 of 31 - 0
	*pMT2CS0 = 0x0000FFFF; // SPORT 2 multichannel tx select, channels 15-0 of 31 - 0
	*pMT4CS0 = 0x000000FF; // SPORT 4 multichannel tx select, channels 7-0 of 31 - 0
	//*pMT6CS0 = 0x00000000; // SPORT 6 multichannel tx select, channels 31 - 0
	// enable receive channels 0-39
	*pMR1CS0 = 0x0000FFFF; // SPORT 1 multichannel rx select, channels 15-0 of 31 - 0
	*pMR3CS0 = 0x0000FFFF; // SPORT 3 multichannel rx select, channels 15-0 of 31 - 0
	*pMR5CS0 = 0x000000FF; // SPORT 5 multichannel rx select, channels 7-0 of 31 - 0
	//*pMR7CS0 = 0x00000000; // SPORT 7 multichannel rx select, channels 31 - 0

	// no companding for the 8 active timeslots for transmitter (would be available on channel A anyway)
	*pMT0CCS0 = 0;
	*pMT2CCS0 = 0;
	*pMT4CCS0 = 0;
	*pMT6CCS0 = 0;
	// no companding for the 8 active timeslots for receiver (would be available on channel A anyway)
	*pMR1CCS0 = 0;
	*pMR3CCS0 = 0;
	*pMR5CCS0 = 0;
	*pMR7CCS0 = 0;

	// enable multichannel operation (SPORT mode and DMA in standby and ready)
	*pSPMCTL0 |= MCEA | MCEB;
	*pSPMCTL1 |= MCEA | MCEB;
	*pSPMCTL2 |= MCEA | MCEB;
	*pSPMCTL3 |= MCEA | MCEB;
	*pSPMCTL4 |= MCEA | MCEB;
	*pSPMCTL5 |= MCEA | MCEB;
	*pSPMCTL6 |= MCEA | MCEB;
	*pSPMCTL7 |= MCEA | MCEB;
}

// endless loop for the case, that CPU-load is above 100% so that
// new audio-data cannot be processed
void systemCrash(void) {
    while(1) {
    	NOP();
    }
}
