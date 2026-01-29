The DSP is using a 16MB external SDRAM memory. When changing something within the system.svc, the
memory mapping sometimes gets overwritten. In this case the DSP wont start any more.

Here are the correct memory-settings:

   mem_sdram_data          { TYPE(DM RAM SYNCHRONOUS) START(0x04000000) END(0x0407FFFF) WIDTH(32) } // 2MB SDRAM = 0.5k Words for system-data
   mem_sdram_audio         { TYPE(DM RAM SYNCHRONOUS) START(0x04080000) END(0x043FFFFF) WIDTH(32) } // 14MB SDRAM = 3M Words for audio-data (this memory is hidden for the CCES-linker)

Add them to the file app.ldf




If Code should be placed in external SDRAM, we can use the following setup:

   /*
    Segment	Start (Logisch)	Ende (Logisch)	Breite	Physischer Bereich (DM-Sicht)	Größe
	Code (PM)	0x00400000	0x0042AAAA	32-bit	0x04000000 - 0x0403FFFF	1 MB
	Data (DM)	0x04040000	0x0407FFFF	32-bit	0x04040000 - 0x0407FFFF	1 MB
	Audio (DM)	0x04080000	0x043FFFFF	32-bit	0x04080000 - 0x043FFFFF	14 MB
   */


   /*
   ** Physical address (P)          0x20_0000 ... 0x3F_FFFF
   ** DM 32-bit Logical address (L) 0x20_0000 ... 0x3F_FFFF
   ** L=P
   */
   mem_sdram_data          { TYPE(DM RAM SYNCHRONOUS) START(0x04040000) END(0x0407FFFF) WIDTH(32) } // 1MB SDRAM = 0.25k Words for system-data
   mem_sdram_audio         { TYPE(DM RAM SYNCHRONOUS) START(0x04080000) END(0x043FFFFF) WIDTH(32) } // 14MB SDRAM = 3M Words for audio-data (this memory is hidden for the CCES-linker)

   /*
   ** Physical address (P)          0x40_0000 ... 0x5F_FFFF
   ** PM 48-bit Logical address (L) 0x2A_AAAB ... 0x3F_FFFF
   ** L=P*(2/3)
   */
   mem_sdram_code          { TYPE(PM RAM SYNCHRONOUS) START(0x00400000) END(0x0042AAAA) WIDTH(32) } // allow first 1MB of external SDRAM for Code: 0x40000 (DM-Words) * 2/3 = 0x2AAAA (PM-Words Offset)
   mem_flash               { TYPE(DM RAM ASYNCHRONOUS) START(0x08000000) END(0x083FFFFF) WIDTH(8) }
