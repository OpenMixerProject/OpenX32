# DSP memory improvements

The DSP projects were close to exhausting their internal memory, especially DSP2's program memory. This change keeps the existing feature set, fixes unsafe SDRAM layouts, and applies a small code-size optimization to control-path functions.

## Measured result

Both measurements are clean CCES 2.12.1 Release builds of the same configuration.

| DSP2 metric | Before | After | Improvement |
| --- | ---: | ---: | ---: |
| Internal PM used | 7,870 words | 7,769 words | 101 words freed |
| Internal PM free | 153 words | 254 words | 66% more headroom |
| DXE size | 338,596 bytes | 337,112 bytes | 1,484 bytes smaller |
| LDR size | 56,740 bytes | 56,128 bytes | 612 bytes smaller |

No DSP1 memory saving is claimed. Its linker layout was tightened so that the fixed delay lines cannot collide with future linker-managed SDRAM data.

## What changed

- DSP2's 14 MiB audio SDRAM is now divided in SHARC word units into eight non-overlapping 1.75 MiB FX slots. The previous calculation treated byte counts as word offsets and addressed memory beyond the physical SDRAM.
- Ordinary DSP2 code can no longer spill silently into external SDRAM. Builds with too many enabled effects now fail at link time instead of producing firmware that would execute code from an unsafe address.
- The external PM address range now matches the ADSP-21371 mapping and exposes the intended 174,763 PM words for explicitly placed external code.
- Selected non-audio-hot-path DSP2 functions are optimized for size. Global size optimization was intentionally not enabled because its cycle cost has not been measured on hardware.
- DSP1's upper 8 MiB SDRAM range is reserved for its manually addressed delay lines and hidden from normal linker allocation.

## Verification and remaining limits

- DSP1 and DSP2 Release builds complete with the Wine-hosted CCES toolchain.
- DSP2 builds with zero warnings; DSP1 retains its existing warnings.
- The Linux host suite passes 34 tests and 1,160 assertions in normal and ASan/UBSan builds.
- There is no ADSP-21371 instruction simulator in the installed CCES package. Audio behavior, cycle load, and all eight FX slots still need validation on X32 hardware before release.
