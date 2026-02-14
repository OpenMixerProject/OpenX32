/*
    ____                  __   ______ ___
   / __ \                 \ \ / /___ \__ \
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / /
  | |__| | |_) |  __/ | | |/ . \ ___) / /_
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |
         |_|

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "system.h"

#define SDMODIFY 1

#pragma section("seg_int_code")
void systemPllSetBypass(bool bypass) {
	int i;

	if (bypass) {
		// enable bypass-mode
		*pPMCTL |= PLLBP;
	}else{
		// wait at least 4096 CLKIN cycles
		for (i=0; i<4096; i++) { asm("nop;"); }
		// disable bypass-mode
		*pPMCTL &= ~PLLBP;
	}

	// wait 16 CCLK cycles
	for (i=0; i<16; i++) { asm("nop;"); }
}

#pragma section("seg_int_code")
void systemPllInit() {
	/*
	PLL is special on the SHARC...

	The general clock-path looks like this:
	CLKIN -> INDIV -> PLLM -> PLLD -> CCLK

	BOOTCFG[1:0] is set to 00 on the X32, so the console boots at 6*16MHz = 96MHz

	Here some important things for the ADSP-21371:
	- CCLK_max = 266MHz
	- f_VCO = 200MHz ... 800MHz
	- do _not_ set DIVEN and PLLBP in same CCLK
	- CLKIN x PLLM < f_VCO_max/2 if INDIV==0
	- CLKIN x PLLM < f_VCO_max   if INDIV==1
	- setting INDIV bit requires the PLL to be set into bypass mode first
	- all changes to the VCO-frequency requires the PLL to be set in bypass mode first

	Calculation of the best fitting settings:
	f_CCLK = (2 * PLLM * f_input)     / (2 * PLLD)
	       = (2 * PLLM * CLKIN/INDIV) / (2 * PLLD)
	       = (2 * 33   * 16MHz/2)     / (2 * 1)
	       = 264MHz

	check f_VCO:
	f_VCO = 2 * PLLM * f_input
	      = 2 * PLLM * (CLKIN/DIVIN)
	      = 2 * 33   * 16MHz/2
	      = 528MHz < 800MHz <- this is valid
	*/

	// Step 1: enable input divider to match VCO-requirements lateron
	// ================================================================
	systemPllSetBypass(true);
	*pPMCTL |= INDIV;
	systemPllSetBypass(false);

	// Step 2: set PLL-configuration to the desired values
	// ================================================================
	systemPllSetBypass(true);
	*pPMCTL = (INDIV | PLLM33 | PLLD1 | SDCKR2 | PLLBP | DIVEN);
	*pPMCTL = (INDIV | PLLM33 | PLLD1 | SDCKR2 | PLLBP); // disable DIVEN (DIVEN is WriteOnly, so set the whole setting without DIVEN)
	systemPllSetBypass(false);
}

#pragma section("seg_int_code")
void systemExternalMemoryInit() {
	// in X32 the RAM of each DSP is a MT48LC8M16A2-6A SDRAM (2 Meg x 16 x 4 Banks)
	// from datasheet page 1
	// t_WR = 2 CLK
	// t_RCD = 18ns (3 CLK)
	// t_RP = 18ns (3 CLK)
	// t_CL = 18ns (3 CLK)
	// t_RAS = 42..120 ns = 6 CLK ... 16 CLK
	// t_REF = max. 64 ns = 8 CLK
	// Auto-Refresh: 64ms for 4096 cycles
	//
	// From datasheet page 2
	// - col addressing 9bit
	// - row addressing 12 bit
	// - refresh count 4k
	// - bank addressing 2 bit
	//
	// it is connected as 16-bit Row and Column Address Mapping (2k Words) but without A12 connected
	// see table 3-10 in Hardware Reference of ADSP-21371

	// set SDRRC and enable SDRAM read optimization
	// RDIV from datasheet: RDIV = (f_SDCLK * t_REF/NRA) - (t_RAS + t_RP)
	// RDIV from datasheet: RDIV = (131.072MHz * 64ms/4096) - (7 + 3ns) = 2039 = 2039 = 0x7F7
	*pSDRRC = 0x7F7 | (1<<17) | SDROPT; // SDMODIFY is set to 0001 (default) and SDROPT is enabled

	//  SDCL3     = SDRAM CAS Latency = 3 cycles
	//  X16DE     = SDRAM external bus width = 16 bits wide
	//  SDNOBSTOP = SDRAM No Burst Stop
	//  SDCAW9    = SDRAM Bank Column Address Width = 9 bits
	//  SDRAW12   = SDRAM Row Address Width = 12 bits
	//  SDTRAS7   = SDRAM tRAS Specification = 7 cycles
	//  SDTRP3    = SDRAM tRP Specification = 3 cycles.
	//  SDTWR2    = SDRAM tWR Specification = 2 cycles.
	//  SDTRCD3   = SDRAM tRCD Specification = 3 cycles.
	//  DSDCLK1   = Disable SDRAM Clock 1
	//  SDPSS     = Start SDRAM Power up Sequence
	*pSDCTL = SDCL3 | X16DE | SDNOBSTOP | SDCAW9 | SDRAW12 | SDTRAS7 | SDTRP3 | SDTWR2 | SDTRCD3 | DSDCLK1 | SDPSS;
	*pSDCTL &= ~DSDCTL; // enable controller

	// enable SDRAM
	*pSYSCTL |= MSEN;

	// Mapping Bank 1 to SDRAM and disable all other banks
	*pEPCTL |= B1SD;
	*pEPCTL &= ~(B0SD|B2SD|B3SD);

    // wait at least 8 PCLK cycles before next activity
    for (int i = 0; i < 20; i++) { asm("nop;"); }

	// configure the AMI Control Register of Bank1 for the K4S281632E
	// 1:1 mapping for 8 bits per byte
	// AMIEN  = enables AMI Controller
	// BW8    = set DataBusWidth to 8bit
	// WS23   = 23 WaitStates (we need two cycles to read/write 32-bit values due to 16-bit databus)
	*pAMICTL1 = AMIEN | BW16 | WS23; // minimum WaitState without ACK is WS2

	// dummy access to initialize the controller
	int dummy = *(int*)SDRAM_START;
	NOP();
	NOP();
	NOP();
	NOP();
	NOP();

/*
	// clear audio-memory that is used by fxRack
    float* ptr = (float*)SDRAM_AUDIO_START;
    for (int i = 0; i < (SDRAM_AUDIO_SIZE_BYTE / sizeof(float)); i++) {
        ptr[i] = 0.0f;
    }
*/
}

#pragma section("seg_int_code")
void systemPcgInit(void) {
	// configure precision clock generator (PCG)
	// FS_DIVIDER = fixed clock divider (here 512)
	// ENFSA  = Enable FS A
	// ENCLKA = Enable CLK A
	// ENCLKB = Enable CLK B
	*pPCG_CTLA1 = CLKA_DIVIDER | 0xC00000; // align FS rising with CLK falling from PCG for I2S mode
	*pPCG_CTLA0 = FSA_DIVIDER | ENFSA | ENCLKA ;
	*pPCG_CTLB1 = CLKB_DIVIDER;
	*pPCG_CTLB0 = ENCLKB;
	*pPCG_PW = 0;
	*pPCG_SYNC = 0;
}

#pragma section("seg_int_code")
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

	// Enable pull-up resistors on unused DAI pins
	* (volatile int *)DAI_PIN_PULLUP = 0xdfff0;

	// setting up SRU for LED-control via Flag-signals
	// =======================================
	// flags 4 to 15 are supported for DPI. Flags 0 to 3 not available on the DPI
	SRU(FLAG7_O, DPI_PB07_I); // connect output of Flag7 to input of DPI-PinBuffer 7 (LED on DPI7)
	SRU(HIGH, DPI_PBEN07_I); // set Pin-Buffer to output (HIGH=Output, LOW=Input)
	sysreg_bit_set(sysreg_FLAGS, FLG7O); // set flag-pins
	sysreg_bit_clr(sysreg_FLAGS, FLG7); // turn on LED





	// setup SRU for TDM
	// setting up DAI
	// =======================================
	// route Framesync to SPORT
	SRU(DAI_PB01_O, SPORT0_FS_I);  // TDM FS    -> SPORT0 FS
	SRU(DAI_PB01_O, SPORT1_FS_I);  // TDM FS    -> SPORT1 FS
	SRU(DAI_PB01_O, SPORT2_FS_I);  // TDM FS    -> SPORT2 FS
	SRU(DAI_PB01_O, SPORT3_FS_I);  // TDM FS    -> SPORT3 FS
	SRU(DAI_PB01_O, DAI_INT_28_I); // TDM FS    -> DAI_INT_28 (MISCA0_INT)

	// route Bitclock to SPORT
	SRU(DAI_PB02_O, SPORT0_CLK_I); // TDM BCLK  -> SPORT0 CLK
	SRU(DAI_PB02_O, SPORT1_CLK_I); // TDM BCLK  -> SPORT1 CLK
	SRU(DAI_PB02_O, SPORT2_CLK_I); // TDM BCLK  -> SPORT2 CLK
	SRU(DAI_PB02_O, SPORT3_CLK_I); // TDM BCLK  -> SPORT3 CLK

	// route data-inputs to SPORT
	SRU(DAI_PB05_O, SPORT1_DA_I);  // TDM FX IN0 -> SPORT1 DATA IN0
	SRU(DAI_PB07_O, SPORT1_DB_I);  // TDM FX IN1 -> SPORT1 DATA IN1
	SRU(DAI_PB09_O, SPORT3_DA_I);  // TDM REC IN -> SPORT3 DATA IN0

	// tie the pin buffer inputs LOW as these pins are input-pins and buffer is not used
	SRU(LOW, DAI_PB01_I); // TDM FS
	SRU(LOW, DAI_PB02_I); // TDM BCLK
	SRU(LOW, DAI_PB05_I); // TDM FX IN0
	SRU(LOW, DAI_PB07_I); // TDM FX IN1
	SRU(LOW, DAI_PB09_I); // TDM REC IN

	// tie the pin buffer enable inputs LOW to make DAI pins 1,2, 11-14 and 19 inputs
	SRU(LOW, PBEN01_I); // TDM FS
	SRU(LOW, PBEN02_I); // TDM BCLK
	SRU(LOW, PBEN05_I); // TDM FX IN0
	SRU(LOW, PBEN07_I); // TDM FX IN1
	SRU(LOW, PBEN09_I); // TDM REC IN

	// route SPORT to data-outputs
	SRU(SPORT0_DA_O, DAI_PB06_I); // SPORT0 DATA OUT0 -> TDM FX OUT0
	SRU(SPORT0_DB_O, DAI_PB08_I); // SPORT0 DATA OUT1 -> TDM FX OUT1
	SRU(SPORT2_DA_O, DAI_PB10_I); // SPORT2 DATA OUT0 -> TDM PLAY OUT

	// tie the pin buffer enable inputs HIGH to make DAI pins 15-18 and 20 outputs
	SRU(HIGH, PBEN06_I); // TDM FX OUT0
	SRU(HIGH, PBEN08_I); // TDM FX OUT1
	SRU(HIGH, PBEN10_I); // TDM PLAY OUT





	// setup SRU for SPDIF (see Figure 13 on page 12 in EE-266)
	SRU (PCG_CLKA_O, SPORT6_CLK_I); // connecting PCG_CLKA to SPORT6_CLK
	SRU (PCG_CLKA_O, DIT_CLK_I); // connecting PCG_CLKA to DIT_CLK

	SRU (PCG_CLKB_O, DIT_HFCLK_I); // connecting PCG_CLKB to DIT_HFCLK
	SRU (PCG_FSA_O, SPORT6_FS_I); // connecting PCG_FSA to SPORT6_FS
	SRU (PCG_FSA_O, DIT_FS_I); // connecting PCG_FSA to DIT_FS

	SRU (SPORT6_DA_O, DIT_DAT_I); // connecting SPORT6_DA to DIT_DAT

	// connecting DIT to DAI_PIN13
	SRU (HIGH, PBEN13_I);
	SRU (DIT_O, DAI_PB13_I);






	// setup SPI interface to i.MX25
	// ========================================
	// route SPI signals to i.MX25
	SRU(DPI_PB01_O, SPI_MOSI_I);	// Connect DPI PB1 to MOSI
	SRU(DPI_PB03_O, SPI_CLK_I); 	// Connect DPI PB3 to SPI CLK
	SRU(DPI_PB04_O, SPI_DS_I);  	// Connect DPI PB4 to SPI ChipSelect
	SRU(SPI_MISO_O, DPI_PB02_I);	// Connect MISO to DPI PB2

	SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);	// control PinBuffer for MISO by SPI-Hardware
	//SRU(LOW, DPI_PBEN02_I); // disable MISO for Debugging

	// set all other pins to input
	SRU(LOW, DPI_PBEN01_I); 		// set to input
	SRU(LOW, DPI_PBEN03_I); 		// set to input
	SRU(LOW, DPI_PBEN04_I); 		// set to input
}

#pragma section("seg_int_code")
void systemSportInit() {
	// see Processor Hardware Reference v2.2 page 7-57

	// clear multi-channel control-registers
	*pSPCTL0 = 0; // SPORT 0 control register
	*pSPCTL1 = 0;
	*pSPCTL2 = 0;
	*pSPCTL3 = 0;
	*pSPMCTL0 = 0; // SPORT 0 Multichannel Control Register
	*pSPMCTL1 = 0;
	*pSPMCTL2 = 0;
	*pSPMCTL3 = 0;
	*pDIV0 = 0x00000000;
	*pDIV1 = 0x00000000;
	*pDIV2 = 0x00000000;
	*pDIV3 = 0x00000000;

	// configure TDM8 transmitter
	// SPEN_A	Channel A enabled
	// SLEN32	Serial data length is 32 (only 24 bits are used here)
	// CKRE		Rising Edge of Clock
	// FSR		Frame sync required
	// SDEN_A	Enable serial port channel A DMA
	// SCHEN_A	Enable serial port channel A DMA chaining
	// SPTRAN	SPORT Channel A as transmitter
	*pSPCTL0  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	*pSPCTL0 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;
	*pSPCTL2  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	*pSPCTL2 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;

	// configure TDM8 receiver
	// SPEN_A 	Channel A enabled
	// SLEN32 	Serial data length is 32 (only 24 bits are used here)
	// CKRE 	Rising Edge of Clock
	// FSR 		Frame sync required
	// SDEN_A 	Enable serial port channel A DMA
	// SCHEN_A	Enable serial port channel A DMA chaining
	*pSPCTL1  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	*pSPCTL1 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;
	*pSPCTL3  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	*pSPCTL3 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;

	// configure DMA memory
	// chained DMA is used to autoinitialize next DMA in line
	// IOP automatically loads new index, modify, and count values from a memory location
	// chain-pointer is 20-bit while the lower 19-bit are the memory address field (therefore 0x7FFFF)
	//
	// *pCPSP0A requires the address of tx0a_buf and settings 19th bit -> enabling the interrupt after the current TCB
	*pCPSP0A = (((unsigned int)&audioTx_tcb[0][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP0B = (((unsigned int)&audioTx_tcb[1][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP2A = (((unsigned int)&audioTx_tcb[2][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP2B = (((unsigned int)&audioTx_tcb[3][0][0] + 3) & OFFSET_MASK); // unused channel

	*pCPSP1A = (((unsigned int)&audioRx_tcb[0][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP1B = (((unsigned int)&audioRx_tcb[1][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP3A = (((unsigned int)&audioRx_tcb[2][0][0] + 3) & OFFSET_MASK); // pointing DMA to desired memory
	*pCPSP3B = (((unsigned int)&audioRx_tcb[3][0][0] + 3) & OFFSET_MASK) | PCI; // unused channel and enable interrupt after every processed TCB (set PCI-bit)




	// set multichannel mode to 8 channels with 1 clock delay of framesync
	*pSPMCTL0 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL1 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL2 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay
	*pSPMCTL3 = NCH7 | MFD1; // set to 8 channels | multichannel mode 1 cycle frame sync delay

	// enable transmit channels 0-39 (see Processor Hardware Reference v2.2 page 7-38)
	*pMT0CS0 = 0x0000FFFF; // SPORT 0 multichannel tx select, select channels 15-0 of 31 - 0
	*pMT2CS0 = 0x0000FFFF; // SPORT 2 multichannel tx select, channels 15-0 of 31 - 0
	// enable receive channels 0-39
	*pMR1CS0 = 0x0000FFFF; // SPORT 1 multichannel rx select, channels 15-0 of 31 - 0
	*pMR3CS0 = 0x0000FFFF; // SPORT 3 multichannel rx select, channels 15-0 of 31 - 0

	// no companding for the 8 active timeslots for transmitter (would be available on channel A anyway)
	*pMT0CCS0 = 0;
	*pMT2CCS0 = 0;
	// no companding for the 8 active timeslots for receiver (would be available on channel A anyway)
	*pMR1CCS0 = 0;
	*pMR3CCS0 = 0;

	// enable multichannel operation (SPORT mode and DMA in standby and ready)
	*pSPMCTL0 |= MCEA | MCEB;	// Sender A/B for FX OUT0/1
	*pSPMCTL2 |= MCEA;			// Sender A for FX PLAY OUT
	*pSPMCTL1 |= MCEA | MCEB;	// Receiver A/B for FX IN0/1
	*pSPMCTL3 |= MCEA;			// Receiver A for FX REC IN

	// enable SPORT
	*pSPCTL0 |= SPEN_A | SPEN_B;
	*pSPCTL2 |= SPEN_A;
	*pSPCTL1 |= SPEN_A | SPEN_B;
	*pSPCTL3 |= SPEN_A;




	// setup SPORT for SPDIF
	// SPEN_A = enable primary channel
	// SLEN32 = WordLength = 32 bit
	// OPMODE = I2S mode enable
	// SPTRAN = Transmitter
	*pSPCTL6 |= SPEN_A | SLEN32 | OPMODE | SPTRAN;
}

#pragma section("seg_int_code")
void systemSpdifTxInit(void) {
	// AES/EBU supports only 24-bit, so we have to set the 8 LSB to zero
	// otherwise the channel-status-byte will contain data

	// DIT_EN       = Enables SPDIF transmitter
	// DIT_SMODEIN0 = Serial Input Format: Left justified (000)
	*pDITCTL = DIT_EN | DIT_SMODEIN0; //configuration of SPDIF TX
}

// endless loop for the case, that CPU-load is above 100% so that
// new audio-data cannot be processed
#pragma section("seg_int_code")
void systemCrash(void) {
	//sysreg_bit_clr(sysreg_FLAGS, FLG7);
    //while(1) {
    //	NOP();
    //}
}
