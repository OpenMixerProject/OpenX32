When changing something within the system.svc, the memory mapping sometimes gets overwritten.
Here are the important changes to the app.ldf file to use external SDRAM and to split the
L1 RAM into 48-bit ProgramMemory (PM) and 32-bit DataMemory (DM) 


   mem_block2_pm48         { TYPE(PM RAM) START(0x000C0000) END(0x000C0AA9) WIDTH(48) } // 50% of block2 for ProgramMemory
   mem_block2_dm32         { TYPE(DM RAM) START(0x000C1000) END(0x000C1FFF) WIDTH(32) } // 50% of block2 for DataMemory

   mem_sdram_data          { TYPE(DM RAM SYNCHRONOUS) START(0x04040000) END(0x0407FFFF) WIDTH(32) } // 1MB SDRAM = 0.25k Words for system-data
   mem_sdram_audio         { TYPE(DM RAM SYNCHRONOUS) START(0x04080000) END(0x043FFFFF) WIDTH(32) } // 14MB SDRAM = 3M Words for audio-data (this memory is hidden for the CCES-linker)

   mem_sdram_code          { TYPE(PM RAM SYNCHRONOUS) START(0x00400000) END(0x0042AAAA) WIDTH(32) } // allow first 1MB of external SDRAM for Code: 0x40000 (DM-Words) * 2/3 = 0x2AAAA (PM-Words Offset)
   mem_flash               { TYPE(DM RAM ASYNCHRONOUS) START(0x08000000) END(0x083FFFFF) WIDTH(8) }



When these memories are defined, we have to define the individual sections:
	Block 0 is used for 100% 48-bit ProgramMemory (PM) by default
	Block 1 is used for 32-bit DataMemory (DM) and the Stack-Memory by default
	Block 2 is split into 50% 48-bit ProgramMemory (PM) and 50% 32-bit DataMemory (DM)
	Block 3 is used for 32-bit DataMemory (DM) and the Heap-Memory by default
	1MB of the external SD-RAM is used as ProgramMemory (PM) for slow code
	1MB of the external SD-RAM is used as DataMemory (EM) for slow data
	14MB of the external SD-RAM is used for the individual effect-racks 

      dxe_block2_pm_code
      {
         INPUT_SECTIONS( $OBJS_LIBS(seg_block2_code) )
         INPUT_SECTIONS( $OBJS_LIBS_INTERNAL(seg_pmco) )
         INPUT_SECTIONS( $OBJS_LIBS_NOT_EXTERNAL(seg_pmco) )
         INPUT_SECTIONS( $OBJS_LIBS(seg_pmco) )
      } > mem_block2_pm48
      
      dxe_block2_pm_data
      {
         INPUT_SECTIONS( $OBJS_LIBS(seg_pmda) )
      } > mem_block2_dm32

      dxe_sdram_audio
      {
         INPUT_SECTIONS( $OBJS_LIBS(seg_ext_audio) )
      } > mem_sdram_audio      
