# OpenX32 DSP Architectural Findings and Bugs

This document summarizes core bugs and architectural issues identified in the DSP codebase during host test suite development.

## 1. SDRAM FX Slot Calculation and Memory Overflow (DSP2)

* **Location**: `dsp2/src/defines.h` (lines 54-55) and `dsp2/src/fxBase.cpp` (line 34)
* **Issue**:
  * `app.ldf` allocates `mem_sdram_audio` from `0x04080000` to `0x043FFFFF` (3.5M 32-bit words, 14 MB total).
  * `defines.h` sets `SDRAM_AUDIO_START` to `0x04200000` (an offset of 1.5M words / 6 MB into audio memory).
  * `fxBase.cpp` calculates slot addresses as `SDRAM_AUDIO_START + slot * (SDRAM_AUDIO_SIZE_BYTE / 8)`.
  * SHARC DM addresses are 32-bit word addresses, not byte addresses. Using `14 * 1024 * 1024 / 8 = 1,835,008` as a word stride is a 4x error.
  * Slot 1 already exceeds the hardware limit (`0x0457FFFF > 0x043FFFFF`), and slot 7 reaches `0x04FFFFFF`.
* **Fix**:
  * Set `SDRAM_AUDIO_START` to `0x04080000`.
  * Divide by 4 bytes per word before slicing into 8 slots: `WORDS_PER_SLOT = (14 * 1024 * 1024 / 4) / 8 = 0x70000` (458,752 words).
  * Sliced correctly, slot 0 starts at `0x04080000` and slot 7 ends at exactly `0x043FFFFF`.

## 2. Moving RMS Off-by-One Window Truncation (DSP2)

* **Location**: `dsp2/src/buffer.cpp` (lines 38-52)
* **Issue**:
  * `bufferMovingRms` writes `ringBuffer->buffer[head] = value`, increments `head`, and then subtracts `ringBuffer->buffer[head]`.
  * Overwriting before reading means sample `n - (bufferLen - 1)` is subtracted early instead of sample `n - bufferLen`.
  * The effective moving window is `bufferLen - 1` samples instead of `bufferLen`, while the divisor remains `bufferLen`. For DC input 2.0 (amplitude squared = 4.0) with bufferLen 8, the steady-state output is 3.5 instead of 4.0.
* **Fix**:
  * Read and store `oldestValue = ringBuffer->buffer[head]` before overwriting `buffer[head]`.
  * Subtract `oldestValue` when updating `ringBuffer->output`.

## 3. Double Negation of Biquad Pole Coefficients (DSP1)

* **Location**: `dsp2/src/helperFcn.cpp` (lines 59, 127) vs `dsp1/src/fx.c` (lines 89, 91)
* **Issue**:
  * `helperFcn_calcBiquadCoeffs` already negates the pole coefficients (`peqCoeffs[3] = -peqCoeffs[1]`).
  * `fxSetPeqCoeffs` negates them a second time when interleaving (`-coeffs[3]`).
  * Inverting the feedback pole signs turns a stable filter into an unstable divergent system.
* **Fix**:
  * Unify the coefficient sign convention between `helperFcn` and `fxSetPeqCoeffs`.

## 4. Word Size and Strict Aliasing Assumptions

* **Location**: `dsp2/src/fastApproxMath.cpp` (line 45) and `dsp1/src/audio.c` (lines 317, 1149)
* **Issue**:
  * `fastApproxMath_invsqrtf` assumes `sizeof(long) == 4` and uses `*(long*)&y`. On 64-bit platforms this reads 8 bytes from a 4-byte stack float, causing buffer overflow and undefined behavior.
  * `audio.c` casts `int*` to `float*` (`*(float*)&audioRxBuf[...]`), which breaks ISO C strict aliasing.
* **Fix**:
  * Use explicit `int32_t` or union-based bit punning instead of `*(long*)`.

## 5. Monolithic DSP1 Audio Processing and Missing Seams

* **Location**: `dsp1/src/audio.c` (line 192) and `dsp1/src/dsp1.h` (lines 69-156)
* **Issue**:
  * `audioProcessData` is an inlined 1000-line function coupling DSP math to hardware registers and hardcoded addresses (`0x04200000`).
  * Gate, compressor, and mixbus loops have misplaced `#pragma optimize_for_speed` inside loops (lines 508, 544, 762).
  * `dsp1.h` declares an anonymous tentative struct `dsp` at global scope without `extern`, preventing clean inclusion in modular C++ code.
* **Fix**:
  * Extract gate, compressor, and mixbus block processing into discrete functions.
  * Give `dsp` a named type in `dsp1.h` and declare it `extern`.
