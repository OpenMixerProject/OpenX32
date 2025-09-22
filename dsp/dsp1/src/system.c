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

void systemPllInit()
{
	int i, pmctlsetting;

	pmctlsetting = *pPMCTL;
	pmctlsetting &= ~(0xFF); // clear pmctlsetting

	// Core clock = 16MHz * (33/2) = 264MHz. Maximum is 266MHz
	pmctlsetting = (PLLM33 | PLLD2 | DIVEN | SDCKR2); // multiplier=33 | divider=2 | divider enabled | SD-Clock-Ratio=2=132MHz
	*pPMCTL = pmctlsetting;

    // bypass mode must be used if any runtime VCO clock change is required
	pmctlsetting |= PLLBP;
    pmctlsetting ^= DIVEN; // DIVEN can be cleared: DIVEN=1 -> Load PLLD, DIVEN=0 -> Do not load PLLD
	*pPMCTL = pmctlsetting;

    /*Wait for around 4096 cycles for the pll to lock.*/
    for (i=0; i<4096; i++) {
         NOP();
    }

    // disable bypass mode
    *pPMCTL ^= PLLBP;
}

void systemExternalMemoryInit()
{
/*
	// SDCL3   = set CAS Latency to 3 (from datasheet)
	// X16DE   = Data-Bits 0..15 are connected to SD-ram
	// SDCAW10 = set Address Width to 10 bit
	// SDRAW11 = set Row address Width to 11 bit
	// SDTRAS  = SDRAM tRAS Specification. Active Command delay = x cycles
	// SDTRP   = SDRAM tRP Specification. Precharge delay = x cycles.
	// SDTWR   = SDRAM tWR Specification. tWR = x cycles.
	// SDTRCD  = SDRAM tRCD Specification. tRCD = x cycles.
	// SDPSS   = start SDRAM power-up on next cycle
	*pSDCTL = SDCL3 | X16DE | SDCAW10 | SDRAW11 | SDTRAS8 | SDTRP4 | SDTWR3 | SDTRCD4 | SDPSS;

	// Mapping Bank 1 to SDRAM
	*pEPCTL |= B1SD;

	// configure the AMI Control Register of Bank1 for the K4S281632E
	// AMIEN  = enables AMI Controller
	// BW16   = set DataBusWidth to 16bit
	// WS32   = 23 WaitStates
	*pAMICTL1 = AMIEN | BW16 | WS23;

	*pSDRRC = 0x406; // RDIV from datasheet: RDIV = (f_SDCLK * t_REF/NRA) - (t_RAW * t_RP)

	// enable SDRAM
	*pSYSCTL |= MSEN;
*/
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
	sysreg_bit_clr(sysreg_FLAGS, FLG7); // turn on LED



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
	*pSPCTL4  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	//*pSPCTL4 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;
	//*pSPCTL6  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A | SPTRAN;
	//*pSPCTL6 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B | SPTRAN;

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
	*pSPCTL5  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	//*pSPCTL5 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;
	//*pSPCTL7  = SLEN32 | CKRE | FSR | SDEN_A | SCHEN_A;
	//*pSPCTL7 |= SLEN32 | CKRE | FSR | SDEN_B | SCHEN_B;

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

	// enable SPORT
	*pSPCTL0 |= SPEN_A | SPEN_B;
	*pSPCTL1 |= SPEN_A | SPEN_B;
	*pSPCTL2 |= SPEN_A | SPEN_B;
	*pSPCTL3 |= SPEN_A | SPEN_B;
	*pSPCTL4 |= SPEN_A;// | SPEN_B;
	*pSPCTL5 |= SPEN_A;// | SPEN_B;
	//*pSPCTL6 |= SPEN_A | SPEN_B;
	//*pSPCTL7 |= SPEN_A | SPEN_B;
}

// endless loop for the case, that CPU-load is above 100% so that
// new audio-data cannot be processed
void systemCrash(void) {
	sysreg_bit_clr(sysreg_FLAGS, FLG7);
    while(1) {
    	NOP();
    }
}
