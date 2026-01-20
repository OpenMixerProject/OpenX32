The DSP is using a 16MB external SDRAM memory. When changing something within the system.svc, the
memory mapping sometimes gets overwritten. In this case the DSP wont start any more.

Here are the correct memory-settings:

   mem_sdram_data          { TYPE(DM RAM SYNCHRONOUS) START(0x04000000) END(0x0407FFFF) WIDTH(32) } // 2MB SDRAM for system-data
   mem_sdram_audio         { TYPE(DM RAM SYNCHRONOUS) START(0x04080000) END(0x04FFFFFF) WIDTH(32) } // 14MB SDRAM for audio-data


Add them to the file app.ldf