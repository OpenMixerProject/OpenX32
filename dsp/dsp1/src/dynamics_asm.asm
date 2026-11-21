/*
  OpenX32 - ADSP-21371 SHARC Assembly Dynamics Kernels (CCES easm21k)
  Copyright 2025-2026 OpenMixerProject (GPLv3)

  Optimized for ADSP-21371 SHARC:
  - Zero-overhead hardware loops (LCNTR=16)
  - 0ms attack instant block-peak lookahead (ABS + MAX) with zero transient overshoot
  - Hardware LOGB / SCALB fast power-to-dBFS and dB-to-linear (< 0.03 dB error, zero sqrtf/log/pow)
  - 6-stage quadratic soft-knee gain computer (0 dB hard .. 36 dB soft 5)
  - Hard Gate (zero-transcendental power compare) + Downward Expander (2:1 / 3:1 / 4:1)
  - 40-bit extended-precision ALU accumulation throughout
*/

#include <asm_sprt.h>

// Struct field offsets (must match dynamics_asm.h)
#define COMP_OFF_THRESH_DB    0
#define COMP_OFF_SLOPE        1
#define COMP_OFF_MAKEUP       2
#define COMP_OFF_ATTACK       3
#define COMP_OFF_HOLD_TICKS   4
#define COMP_OFF_RELEASE      5
#define COMP_OFF_KNEE_HALF    6
#define COMP_OFF_KNEE_COEFF   7
#define COMP_OFF_ENV          8
#define COMP_OFF_HOLD_TIMER   9

#define GATE_OFF_THRESH_SQ16  0
#define GATE_OFF_GAIN_MIN     1
#define GATE_OFF_ATTACK       2
#define GATE_OFF_HOLD_TICKS   3
#define GATE_OFF_RELEASE      4
#define GATE_OFF_THRESH_DB    5
#define GATE_OFF_NEG_RANGE    6
#define GATE_OFF_EXP_SLOPE    7
#define GATE_OFF_ENV          8
#define GATE_OFF_HOLD_TIMER   9

#define SAMPLES_PER_BLOCK     16

.SECTION/DM seg_dmda;
.VAR _dyn_c_noise_floor   = 737870.0;       // -140 dBFS power floor (16 * (2^31-1)^2 * 10^-14)
.VAR _dyn_c_neg140        = -140.0;
.VAR _dyn_c_pos24         = 24.0;
.VAR _dyn_c_0p999         = 0.999;
.VAR _dyn_c_16p0          = 16.0;
.VAR _dyn_c_1p0           = 1.0;
.VAR _dyn_c_log2_a        = 1.6797000;      // quadratic log2(m) coeff a
.VAR _dyn_c_log2_b        = 0.3398500;      // quadratic log2(m) coeff b
.VAR _dyn_c_10log10_2     = 3.01029995664;  // 10 * log10(2)
.VAR _dyn_c_pwr_offset    = 198.67979412;   // 10 * log10(16 * (2^31-1)^2)
.VAR _dyn_c_inv_20log10_2 = 0.16609640474;  // 1 / (20 * log10(2))
.VAR _dyn_c_exp2_c1       = 0.65685425;     // quadratic 2^xf coeff c1
.VAR _dyn_c_exp2_c2       = 0.34314575;     // quadratic 2^xf coeff c2

.SECTION/PM seg_pmco;

// ============================================================================
// Internal helper: _asm_powerToDb_fast
//   Input : F2 = sumOfSquares (16-sample energy)
//   Output: F0 = levelDb in dBFS (clamped at -140.0 dBFS)
//   Clobbers: R0, R1, F0, F1, F2, F4
// ============================================================================
_asm_powerToDb_fast:
	F4 = DM(_dyn_c_noise_floor);
	COMP(F2, F4);
	IF LE JUMP _p2db_floor;

	// Hardware LOGB extracts unbiased IEEE-754 exponent in 1 cycle
	R0 = LOGB F2;
	R1 = -R0;
	// Hardware SCALB shifts mantissa m into [1.0, 2.0) in 1 cycle
	F0 = SCALB F2 BY R1;
	F1 = DM(_dyn_c_1p0);
	F2 = F0 - F1;                       // F2 = m - 1.0
	F4 = DM(_dyn_c_log2_b);
	F4 = F4 * F0;                       // 0.33985 * m
	F1 = DM(_dyn_c_log2_a);
	F1 = F1 - F4;                       // 1.6797 - 0.33985 * m
	F2 = F2 * F1;                       // log2_m
	F1 = FLOAT R0;                      // (float)exp
	F1 = F1 + F2;                       // exp + log2_m
	F4 = DM(_dyn_c_10log10_2);
	F0 = F1 * F4;                       // 3.0103 * log2(sumSq)
	F4 = DM(_dyn_c_pwr_offset);
	F0 = F0 - F4;                       // - 198.6798
	RTS;

_p2db_floor:
	F0 = DM(_dyn_c_neg140);
	RTS;

// ============================================================================
// Internal helper: _asm_dbToLinear_fast
//   Input : F0 = dB
//   Output: F0 = linear gain = 10^(dB/20)
//   Clobbers: R0, R1, F0, F1, F2, F4
// ============================================================================
_asm_dbToLinear_fast:
	F4 = DM(_dyn_c_neg140);
	COMP(F0, F4);
	IF LE JUMP _db2lin_zero;

	F4 = DM(_dyn_c_pos24);
	F0 = MIN(F0, F4);

	F4 = DM(_dyn_c_inv_20log10_2);
	F1 = F0 * F4;                       // x = dB / (20 * log10(2))
	R0 = TRUNC F1;                      // integer part (towards zero)
	F2 = FLOAT R0;
	COMP(F2, F1);
	IF GT R0 = R0 - 1;                  // floor(x) for negative values
	F2 = FLOAT R0;
	F2 = F1 - F2;                       // xf in [0.0, 1.0)

	// mant = 1.0 + xf * (0.65685425 + 0.34314575 * xf)
	F4 = DM(_dyn_c_exp2_c2);
	F4 = F4 * F2;
	F1 = DM(_dyn_c_exp2_c1);
	F1 = F1 + F4;
	F1 = F1 * F2;
	F4 = DM(_dyn_c_1p0);
	F1 = F1 + F4;

	// Hardware SCALB multiplies by 2^R0 in 1 cycle
	F0 = SCALB F1 BY R0;
	RTS;

_db2lin_zero:
	F0 = F0 - F0;                       // 0.0f
	RTS;

// ============================================================================
// void fxCompressorBlock_asm(const float* src, float* dst, sCompAsmParams* params)
//   R4  = src
//   R8  = dst
//   R12 = params
// ============================================================================
.GLOBAL _fxCompressorBlock_asm;
_fxCompressorBlock_asm:
	leaf_entry;

	// Save callee-saved registers we use (R3, R5, R6, R7, R9, R10, R11, I2, I3)
	DM(I7, M7) = R3;
	DM(I7, M7) = R5;
	DM(I7, M7) = R6;
	DM(I7, M7) = R7;
	DM(I7, M7) = R9;
	DM(I7, M7) = R10;
	DM(I7, M7) = R11;
	DM(I7, M7) = I2;
	DM(I7, M7) = I3;

	I2 = R4;                            // src base (preserved for apply loop)
	I3 = R12;                           // params base
	I12 = R8;                           // dst base

	// Check 0ms attack (coeffAttack >= 0.999f)
	F0 = DM(COMP_OFF_ATTACK, I3);
	F1 = DM(_dyn_c_0p999);
	COMP(F0, F1);
	I4 = I2;                            // scan pointer = src
	F2 = F2 - F2;                       // accumulator = 0.0f
	IF LT JUMP _comp_rms_detect;

	// --- 0ms Attack: Instant Block-Peak Lookahead (ABS + MAX) ---
	LCNTR = SAMPLES_PER_BLOCK, DO _comp_peak_loop UNTIL LCE;
		F0 = DM(I4, M6);
		F0 = ABS F0;
_comp_peak_loop:
		F2 = MAX(F0, F2);

	F2 = F2 * F2;                       // maxAbs^2
	F4 = DM(_dyn_c_16p0);
	F2 = F2 * F4;                       // 16 * maxAbs^2 -> exact peak dBFS
	JUMP _comp_gain_calc;

_comp_rms_detect:
	// --- >0ms Attack: Zero-overhead 16-sample Sum-of-Squares ---
	LCNTR = SAMPLES_PER_BLOCK, DO _comp_rms_loop UNTIL LCE;
		F0 = DM(I4, M6);
		F1 = F0 * F0;
_comp_rms_loop:
		F2 = F2 + F1;

_comp_gain_calc:
	// F0 = levelDb = powerToDb_fast(F2)
	CALL _asm_powerToDb_fast;

	// overshoot = levelDb - thresholdDb
	F1 = DM(COMP_OFF_THRESH_DB, I3);
	F5 = F0 - F1;                       // F5 = overshoot
	F6 = DM(COMP_OFF_KNEE_HALF, I3);    // F6 = kneeHalf
	F0 = F0 - F0;                       // F0 = targetGainDb = 0.0f

	// if (overshoot >= kneeHalf) -> above knee (also handles hard knee)
	COMP(F5, F6);
	IF LT JUMP _comp_check_knee;
	F1 = F1 - F1;
	COMP(F5, F1);                       // overshoot > 0.0f?
	IF LE JUMP _comp_to_lin;
	F4 = DM(COMP_OFF_SLOPE, I3);
	F0 = F5 * F4;
	F0 = -F0;                           // targetGainDb = -overshoot * slope
	JUMP _comp_to_lin;

_comp_check_knee:
	F7 = -F6;                           // -kneeHalf
	COMP(F5, F7);
	IF LE JUMP _comp_to_lin;            // below knee -> targetGainDb = 0.0f
	// Inside quadratic soft-knee: u = overshoot + kneeHalf; targetGainDb = kneeCoeff * u * u
	F1 = F5 + F6;
	F1 = F1 * F1;
	F4 = DM(COMP_OFF_KNEE_COEFF, I3);
	F0 = F1 * F4;

_comp_to_lin:
	// targetGainLinear in F5 (1.0f if targetGainDb >= 0)
	F1 = F1 - F1;
	COMP(F0, F1);
	IF GE JUMP _comp_unity_target;
	CALL _asm_dbToLinear_fast;
	F5 = F0;
	JUMP _comp_ahr;

_comp_unity_target:
	F5 = DM(_dyn_c_1p0);

_comp_ahr:
	// Attack / Hold / Release state machine
	F6 = DM(COMP_OFF_ENV, I3);          // F6 = env
	R7 = DM(COMP_OFF_HOLD_TIMER, I3);   // R7 = holdTimer
	F9 = F9 - F9;                       // F9 = coeff = 0.0f

	COMP(F5, F6);                       // targetGainLinear < env?
	IF GE JUMP _comp_hold_rel;
	// Attack
	F9 = DM(COMP_OFF_ATTACK, I3);
	F1 = DM(COMP_OFF_HOLD_TICKS, I3);
	R7 = TRUNC F1;
	JUMP _comp_apply_setup;

_comp_hold_rel:
	R7 = PASS R7;
	IF LE JUMP _comp_release;
	R7 = R7 - 1;
	JUMP _comp_apply_setup;

_comp_release:
	F9 = DM(COMP_OFF_RELEASE, I3);

_comp_apply_setup:
	DM(COMP_OFF_HOLD_TIMER, I3) = R7;

	// a1 = 1.0 - coeff (F10), b0 = coeff * targetGainLinear (F11), makeup (F3)
	F1 = DM(_dyn_c_1p0);
	F10 = F1 - F9;
	F11 = F9 * F5;
	F3 = DM(COMP_OFF_MAKEUP, I3);

	// Apply envelope + makeup across 16 samples in hardware loop
	I4 = I2;                            // reload src base
	LCNTR = SAMPLES_PER_BLOCK, DO _comp_apply_loop UNTIL LCE;
		F6 = F6 * F10;
		F0 = DM(I4, M6), F6 = F6 + F11; // parallel load src[s] + env = env*a1 + b0
		F0 = F0 * F3;                   // src[s] * makeup
		F0 = F0 * F6;                   // * env
_comp_apply_loop:
		DM(I12, M6) = F0;

	// Write back updated envelope state
	DM(COMP_OFF_ENV, I3) = F6;

	// Restore callee-saved registers
	MODIFY(I7, 1);
	I3  = DM(I7, M6);
	I2  = DM(I7, M6);
	R11 = DM(I7, M6);
	R10 = DM(I7, M6);
	R9  = DM(I7, M6);
	R7  = DM(I7, M6);
	R6  = DM(I7, M6);
	R5  = DM(I7, M6);
	R3  = DM(I7, M5);

	leaf_exit;
_fxCompressorBlock_asm.end:

// ============================================================================
// void fxGateBlock_asm(float* src_dst, sGateAsmParams* params)
//   R4 = src_dst (in-place 16-sample buffer)
//   R8 = params
// ============================================================================
.GLOBAL _fxGateBlock_asm;
_fxGateBlock_asm:
	leaf_entry;

	DM(I7, M7) = R5;
	DM(I7, M7) = R6;
	DM(I7, M7) = R7;
	DM(I7, M7) = R9;
	DM(I7, M7) = R10;
	DM(I7, M7) = R11;
	DM(I7, M7) = I2;
	DM(I7, M7) = I3;

	I2 = R4;                            // src_dst base
	I3 = R8;                            // params base
	I4 = I2;

	// 16-sample sum-of-squares (zero sqrtf)
	F2 = F2 - F2;
	LCNTR = SAMPLES_PER_BLOCK, DO _gate_rms_loop UNTIL LCE;
		F0 = DM(I4, M6);
		F1 = F0 * F0;
_gate_rms_loop:
		F2 = F2 + F1;

	// Check Gate vs Expander mode (expanderSlope == 0.0f -> Hard Gate)
	F5 = DM(_dyn_c_1p0);                // default targetGainLinear = 1.0f
	F4 = DM(GATE_OFF_EXP_SLOPE, I3);
	F1 = F1 - F1;
	COMP(F4, F1);
	IF NE JUMP _gate_expander;

	// Hard Gate: zero-transcendental power compare
	F0 = DM(GATE_OFF_THRESH_SQ16, I3);
	COMP(F2, F0);
	IF GE JUMP _gate_ahr;
	F5 = DM(GATE_OFF_GAIN_MIN, I3);
	JUMP _gate_ahr;

_gate_expander:
	// Downward Expander (2:1 / 3:1 / 4:1) clamped to Range
	CALL _asm_powerToDb_fast;           // F0 = levelDb
	F1 = DM(GATE_OFF_THRESH_DB, I3);
	F0 = F0 - F1;                       // diffDb
	F1 = F1 - F1;
	COMP(F0, F1);
	IF GE JUMP _gate_ahr;               // above threshold -> targetGainLinear = 1.0f
	F4 = DM(GATE_OFF_EXP_SLOPE, I3);
	F0 = F0 * F4;                       // attenDb (negative)
	F1 = DM(GATE_OFF_NEG_RANGE, I3);
	F0 = MAX(F0, F1);                   // clamp to -rangeDb
	CALL _asm_dbToLinear_fast;
	F5 = F0;

_gate_ahr:
	F6 = DM(GATE_OFF_ENV, I3);          // F6 = env
	R7 = DM(GATE_OFF_HOLD_TIMER, I3);   // R7 = holdTimer
	F9 = F9 - F9;                       // coeff = 0.0f

	COMP(F5, F6);                       // targetGainLinear > env?
	IF LE JUMP _gate_hold_rel;
	// Attack (opening)
	F9 = DM(GATE_OFF_ATTACK, I3);
	F1 = DM(GATE_OFF_HOLD_TICKS, I3);
	R7 = TRUNC F1;
	JUMP _gate_apply_setup;

_gate_hold_rel:
	R7 = PASS R7;
	IF LE JUMP _gate_release;
	R7 = R7 - 1;
	JUMP _gate_apply_setup;

_gate_release:
	F9 = DM(GATE_OFF_RELEASE, I3);

_gate_apply_setup:
	DM(GATE_OFF_HOLD_TIMER, I3) = R7;

	F1 = DM(_dyn_c_1p0);
	F10 = F1 - F9;                      // a1 = 1 - coeff
	F11 = F9 * F5;                      // b0 = coeff * targetGainLinear

	I4 = I2;                            // read ptr
	I12 = I2;                           // write ptr (in-place)
	LCNTR = SAMPLES_PER_BLOCK, DO _gate_apply_loop UNTIL LCE;
		F6 = F6 * F10;
		F0 = DM(I4, M6), F6 = F6 + F11;
		F0 = F0 * F6;
_gate_apply_loop:
		DM(I12, M6) = F0;

	DM(GATE_OFF_ENV, I3) = F6;

	MODIFY(I7, 1);
	I3  = DM(I7, M6);
	I2  = DM(I7, M6);
	R11 = DM(I7, M6);
	R10 = DM(I7, M6);
	R9  = DM(I7, M6);
	R7  = DM(I7, M6);
	R6  = DM(I7, M6);
	R5  = DM(I7, M5);

	leaf_exit;
_fxGateBlock_asm.end:
