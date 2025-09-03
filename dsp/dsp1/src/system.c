#include "system.h"

#define SDMODIFY 1

unsigned int tdmData0[8][NUM_SAMPLES];
int TCB_tdmData0[4] = { 0, sizeof(tdmData0), 1, 0};

void systemPllInit()
{
	int i, pmctlsetting;

	pmctlsetting= *pPMCTL;
	pmctlsetting &= ~(0xFF); // clear pmctlsetting

	// Core clock = 16MHz * (16/1) = 256 MHz
	pmctlsetting= (PLLM16|PLLD1|DIVEN|SDCKR2); // multiplier=16 | divider=1 | divider enabled | SD-Clock-Ratio=2=128MHz
	*pPMCTL= pmctlsetting;
	pmctlsetting|= PLLBP;
	pmctlsetting^= DIVEN;
	*pPMCTL= pmctlsetting;

	// Wait for around 4096 cycles for the PLL to lock.
	for (i=0; i<4096; i++)
		NOP();

	*pPMCTL ^= PLLBP;       // clear Bypass Mode
	*pPMCTL |= (CLKOUTEN);  // and start clk-out
}

void systemExternalMemoryInit()
{
	// Programming SDRAM control registers and enabling SDRAM read optimization
	// RDIV = ((f SDCLK X t REF )/NRA) - (tRAS + tRP )
	// (131.072*(10^6)*64*(10^-3)/4096) - (6+3) = 2039 = 0x7F7
	*pSDRRC= (0x7F7)|(SDMODIFY<<17)|SDROPT;

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

	*pSDCTL= SDCL3|DSDCLK1|SDPSS|SDCAW9|SDRAW12|SDTRAS6|SDTRP3|SDTWR2|SDTRCD3|X16DE;

	// Note that MS2 & MS3 pin multiplexed with flag2 & flag3
	// MSEN bit must be enabled to access SDRAM
	*pSYSCTL |= MSEN;

	// Mapping Bank 0 to SDRAM
	// MS2 is connected to chip select of 16-bit SDRAM device
	*pEPCTL |= B0SD;
	*pEPCTL &= ~(B1SD|B2SD|B3SD);

	/*  Configure AMI Control Register (AMICTL) Bank 1 for the AMD AM29LV08
		WS23 : Wait States= 23 cycles
		AMIEN: Enable AMI
		BW8  : External Data Bus Width= 8 bits*/
	*pAMICTL1 = WS23|AMIEN|BW8; // Flash Settings
}

void systemSruInit(void) {
	// setting up pins for LED-control via Flag-signals
	// =======================================
	// flags 4 to 15 are supported for DPI. Flags 0 to 3 not available on the DPI
	SRU(FLAG7_O, DPI_PB07_I); // connect output of Flag7 to input of DPI-PinBuffer 7 (LED on DPI7)
	SRU(HIGH, DPI_PBEN07_I); // set Pin-Buffer to output (HIGH=Output, LOW=Input)
	sysreg_bit_set(sysreg_FLAGS, FLG7O); // set flag-pins
	sysreg_bit_clr(sysreg_FLAGS, FLG7); // clear flag-pins



	// setting up DAI
	// =======================================
	// route Framesync to SPORT
	SRU(DAI_PB01_O, SPORT0_FS_I);  // TDM FS    -> SPORT0 FS
	SRU(DAI_PB01_O, SPORT1_FS_I);  // TDM FS    -> SPORT1 FS
	SRU(DAI_PB01_O, SPORT2_FS_I);  // TDM FS    -> SPORT2 FS
	SRU(DAI_PB01_O, SPORT3_FS_I);  // TDM FS    -> SPORT3 FS
	SRU(DAI_PB01_O, SPORT4_FS_I);  // TDM FS    -> SPORT4 FS
	SRU(DAI_PB01_O, SPORT5_FS_I);  // TDM FS    -> SPORT5 FS
	SRU(DAI_PB01_O, SPORT6_FS_I);  // TDM FS    -> SPORT6 FS
	SRU(DAI_PB01_O, SPORT7_FS_I);  // TDM FS    -> SPORT7 FS

	// route Bitclock to SPORT
	SRU(DAI_PB02_O, SPORT0_CLK_I); // TDM BCLK  -> SPORT0 CLK
	SRU(DAI_PB02_O, SPORT1_CLK_I); // TDM BCLK  -> SPORT1 CLK
	SRU(DAI_PB02_O, SPORT2_CLK_I); // TDM BCLK  -> SPORT2 CLK
	SRU(DAI_PB02_O, SPORT3_CLK_I); // TDM BCLK  -> SPORT3 CLK
	SRU(DAI_PB02_O, SPORT4_CLK_I); // TDM BCLK  -> SPORT4 CLK
	SRU(DAI_PB02_O, SPORT5_CLK_I); // TDM BCLK  -> SPORT5 CLK
	SRU(DAI_PB02_O, SPORT6_CLK_I); // TDM BCLK  -> SPORT6 CLK
	SRU(DAI_PB02_O, SPORT7_CLK_I); // TDM BCLK  -> SPORT7 CLK

	// route data-inputs to SPORT
	SRU(DAI_PB11_O, SPORT0_DA_I);  // TDM IN0   -> SPORT0 DATA IN0
	SRU(DAI_PB12_O, SPORT0_DB_I);  // TDM IN1   -> SPORT0 DATA IN1
	SRU(DAI_PB13_O, SPORT1_DA_I);  // TDM IN2   -> SPORT1 DATA IN0
	SRU(DAI_PB14_O, SPORT1_DB_I);  // TDM IN3   -> SPORT1 DATA IN1
	SRU(DAI_PB19_O, SPORT2_DA_I);  // TDM INAUX -> SPORT2 DATA IN0

	// tie the pin buffer inputs LOW as these pins are input-pins and buffer is not used
	SRU(LOW, DAI_PB01_I); // TDM FS
	SRU(LOW, DAI_PB02_I); // TDM BCLK
	SRU(LOW, DAI_PB11_I); // TDM IN0
	SRU(LOW, DAI_PB12_I); // TDM IN1
	SRU(LOW, DAI_PB13_I); // TDM IN2
	SRU(LOW, DAI_PB14_I); // TDM IN3
	SRU(LOW, DAI_PB19_I); // TDM INAUX

	// tie the pin buffer enable inputs LOW to make DAI pins 1,2, 11-14 and 19 inputs
	SRU(LOW, PBEN01_I); // TDM FS
	SRU(LOW, PBEN02_I); // TDM BCLK
	SRU(LOW, PBEN11_I); // TDM IN0
	SRU(LOW, PBEN12_I); // TDM IN1
	SRU(LOW, PBEN13_I); // TDM IN2
	SRU(LOW, PBEN14_I); // TDM IN3
	SRU(LOW, PBEN19_I); // TDM INAUX

	// route SPORT to data-outputs
	SRU(SPORT2_DB_O, DAI_PB15_I); // SPORT2 DATA OUT1 -> TDM OUT0
	SRU(SPORT3_DA_O, DAI_PB16_I); // SPORT3 DATA OUT0 -> TDM OUT1
	SRU(SPORT3_DB_O, DAI_PB17_I); // SPORT3 DATA OUT1 -> TDM OUT2
	SRU(SPORT4_DA_O, DAI_PB18_I); // SPORT4 DATA OUT0 -> TDM OUT3
	SRU(SPORT4_DB_O, DAI_PB20_I); // SPORT4 DATA OUT1 -> TDM OUTAUX

	// tie the pin buffer enable inputs HIGH to make DAI pins 15-18 and 20 outputs
	SRU(HIGH, PBEN15_I); // TDM OUT0
	SRU(HIGH, PBEN16_I); // TDM OUT1
	SRU(HIGH, PBEN17_I); // TDM OUT2
	SRU(HIGH, PBEN18_I); // TDM OUT3
	SRU(HIGH, PBEN20_I); // TDM OUTAUX



	// setup SPI interface to FPGA
	// ========================================
	// route SPI signals to FPGA
	SRU(SPI_MOSI_O, DPI_PB01_I)     // Connect MOSI to DPI PB1
	SRU(DPI_PB02_O, SPI_MISO_I)     // Connect DPI PB2 to MISO
	SRU(SPI_CLK_O,  DPI_PB03_I)     // Connect SPI CLK to DPI PB3
	SRU(SPI_FLG3_O, DPI_PB04_I)     // Connect SPI FLAG3 to DPI PB4

	// tie pin buffer enable from SPI peripheral to determine whether they are inputs or outputs
	SRU(SPI_MOSI_PBEN_O, DPI_PBEN01_I);
	SRU(SPI_MISO_PBEN_O, DPI_PBEN02_I);
	SRU(SPI_CLK_PBEN_O,  DPI_PBEN03_I);
	SRU(SPI_FLG3_PBEN_O, DPI_PBEN04_I);
}

void systemSportInit() {
	// clear multi-channel control-registers
	*pSPMCTL0 = 0;
	*pSPMCTL1 = 0;
	*pSPMCTL2 = 0;
	*pSPCTL0 = 0;
	*pSPCTL1 = 0;
	*pSPCTL2 = 0;

	// read data from SPORT0A
	*pSPCTL0 = (SLEN32 | SPEN_A | SCHEN_A | SDEN_A | FSR); // sample-length=32bit | SPORT-channel-A enabled | chaining A enabled | channel A enabled | receive FrameSync
	*pCPSP0A = ((unsigned int) TCB_tdmData0 + 3) & OFFSET_MASK; // Points to next DMA parameters

	// write data to SPORT2B
	*pSPCTL2 = (SPTRAN | SLEN32 | SPEN_B | SCHEN_B | SDEN_B | FSR); // transmitter | sample-length=32bit | SPORT-channel-B enabled | chaining B enabled | channel B enabled | receive FrameSync
	*pCPSP2B = ((unsigned int) TCB_tdmData0 + 3) & OFFSET_MASK; // Points to next DMA parameters
}

// endless loop for the case, that CPU-load is above 100% so that
// new audio-data cannot be processed
void systemCrash(void) {
    while(1) {
    	NOP();
    }
}
