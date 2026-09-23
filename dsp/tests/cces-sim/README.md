# CCES simulator test status

No ADSP-21371 instruction-set simulator is installed with the available CCES
2.12.1 installation. The installation contains `adi_securebootsim.exe`, which
simulates secure-boot packaging rather than SHARC instruction execution; it
cannot run DSP1 or DSP2 DXE files. Consequently, no CCES simulator cycle or
runtime result is available.

The DSP2 Release build does compile the memory-layout assertions in
`dsp2/src/defines.h` with SHARC compiler 8.16.1.0. Runtime address coverage,
including the old failing calculation and all eight corrected slot ranges, is
provided by `../host/src/test_dsp2_memory_layout.cpp` on the host. This is not
claimed as simulator or hardware execution.
