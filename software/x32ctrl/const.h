#pragma once

#include <stdint.h>


const uint32_t vuThresholds_minus60dbfs_25steps[] = {
    VUTRESH_00_DBFS_CLIP, // index  0 = ~0 dBfs // should be 2147483648, but turn-on CLIP-led on before real clipping
    VUTRESH_MINUS_01_DBFS, // index  1 = -1 dBfs
    VUTRESH_MINUS_02_DBFS, // index  2 = -2 dBfs
    VUTRESH_MINUS_03_DBFS, // index  3 = -3 dBfs
    VUTRESH_MINUS_04_DBFS, // index  4 = -4 dBfs
    VUTRESH_MINUS_06_DBFS, // index  5 = -6 dBfs
    VUTRESH_MINUS_08_DBFS,  // index  6 = -8 dBfs
    VUTRESH_MINUS_10_DBFS,  // index  7 = -10 dBfs
    VUTRESH_MINUS_12_DBFS,  // index  8 = -12 dBfs
    VUTRESH_MINUS_15_DBFS,  // index  9 = -15 dBfs
    VUTRESH_MINUS_18_DBFS,  // index 10 = -18 dBfs
    VUTRESH_MINUS_21_DBFS,  // index 11 = -21 dBfs
    VUTRESH_MINUS_24_DBFS,  // index 12 = -24 dBfs
    VUTRESH_MINUS_27_DBFS,   // index 13 = -27 dBfs
    VUTRESH_MINUS_30_DBFS,   // index 14 = -30 dBfs
    VUTRESH_MINUS_33_DBFS,   // index 15 = -33 dBfs
    VUTRESH_MINUS_36_DBFS,   // index 16 = -36 dBfs
    VUTRESH_MINUS_39_DBFS,   // index 17 = -39 dBfs
    VUTRESH_MINUS_42_DBFS,   // index 18 = -42 dBfs
    VUTRESH_MINUS_45_DBFS,   // index 19 = -45 dBfs
    VUTRESH_MINUS_48_DBFS,    // index 20 = -48 dBfs
    VUTRESH_MINUS_51_DBFS,    // index 21 = -51 dBfs
    VUTRESH_MINUS_54_DBFS,    // index 22 = -54 dBfs
    VUTRESH_MINUS_57_DBFS,    // index 23 = -57 dBfs
    VUTRESH_MINUS_60_DBFS     // index 24 = -60 dBfs
};


const uint32_t vuThresholds_minus45dbfs_18steps[] = {
    VUTRESH_00_DBFS_CLIP,
    VUTRESH_MINUS_02_DBFS,
    VUTRESH_MINUS_04_DBFS,
    VUTRESH_MINUS_06_DBFS,
    VUTRESH_MINUS_08_DBFS,
    VUTRESH_MINUS_10_DBFS,
    VUTRESH_MINUS_12_DBFS,
    VUTRESH_MINUS_15_DBFS,
    VUTRESH_MINUS_18_DBFS,
    VUTRESH_MINUS_21_DBFS,
    VUTRESH_MINUS_24_DBFS,
    VUTRESH_MINUS_27_DBFS,
    VUTRESH_MINUS_30_DBFS,
    VUTRESH_MINUS_33_DBFS,
    VUTRESH_MINUS_36_DBFS,
    VUTRESH_MINUS_39_DBFS,
    VUTRESH_MINUS_42_DBFS,
    VUTRESH_MINUS_45_DBFS
};