The DSP is using a 16MB external SDRAM memory. When changing something within the system.svc, the
memory mapping sometimes gets overwritten. In this case the DSP wont start any more.

Here are the correct memory-settings:

   mem_sdram_data          { TYPE(DM RAM SYNCHRONOUS) START(0x04000000) END(0x043FFFFF) WIDTH(32) } // 16MB SDRAM = 4.2M Words for system-data

Add them to the file app.ldf