/*
    ____                  __   ______ ___
   / __ \                 \ \ / /___ \__ \
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / /
  | |__| | |_) |  __/ | | |/ . \ ___) / /_
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |
         |_|

  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025-2026 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  This file contains some helper-functions that will be used from multiple other files
*/

#include "fastApproxMath.h"

// credits to Martin Leitner-Ankerl
float fastApproxMath_powf(float a, float b) {
    union {
        float f;
        int i;
    } u = { a };
    u.i = (int)(b * (u.i - 1064866816) + 1064866816);
    return u.f;
}

// credits to John Carmack
float fastApproxMath_invsqrtf(float number) {
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;                       // evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration

	return y;
}

float fastApproxMath_expf(float val) {
    union {
        int i;
        float f;
    } converter;

    // Die Konstanten für float (32-bit) Annäherungen weichen leicht ab.
    // Ein gängiger Wert für die Annäherung ist:
    // i = L * val + B
    // wobei L = 2^23 / ln(2) und B der Exponenten-Bias ist.
    
    // 12102203 ist eine optimierte Konstante für float-Approximationen
    int tmp = (int)(12102203 * val + 1064866805);
    
    converter.i = tmp;
    return converter.f;
}

float fastApproxMath_lnf(float val) {
    union {
        int i;
        float f;
    } converter;

    converter.f = val;

    // Wir nehmen den gesamten int-Wert (32 Bit)
    // Die Konstanten müssen für das float-Format (IEEE 754 32-bit) angepasst sein
    return ((float)converter.i - 1064866805.0f) / 12102203.0f;
}

float fastApproxMath_sinf(float x) {
    const float PI = 3.14159265f;
    return (16.0f * x * (PI - x)) / (5.0f * PI * PI - 4.0f * x * (PI - x));
}

float fastApproxMath_log2f(float val) {
    union { float f; int i; } conv;
    conv.f = val;
    // Extrahiert den Exponenten aus dem IEEE 754 Float
    // (Bits 23-30), zieht den Bias (127) ab
    return (float)((conv.i >> 23) & 0xFF) - 127.0f;
}

float fastApproxMath_absf(float f) {
    union { float f; unsigned int i; } conv;
    conv.f = f;
    conv.i &= 0x7fffffff; // Setzt das vorderste Bit (Vorzeichen) auf 0
    return conv.f;
}

float fastApproxMath_tanh(float x) {
    float x2 = x * x;
    // Eine Ann�herung, die die S-Kurve von tanh sehr gut nachahmt
    return x * (27.0f + x2) / (27.0f + 9.0f * x2);
}


// functions for double

// credits to Martin Leitner-Ankerl
double fastApproxMath_pow(double a, double b) {
    union {
        double d;
        int x[2];
    } u = { a };
    u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
    u.x[0] = 0;
    return u.d;
}

// credits to Martin Leitner-Ankerl
double fastApproxMath_exp(double val) {
    union {
        long long i;
        double d;
    } converter;

    // Berechnung des ganzzahligen Repräsentanten
    // 1512775 * val + (1072693248 - 60801)
    long long tmp = (long long)(1512775.0 * val + 1072632447.0);

    // Entspricht tmp << 32 in Java (Verschiebung in den Exponenten-Bereich)
    converter.i = tmp << 32;

    return converter.d;
}

double fastApproxMath_ln(double val) {
    union {
        long long i;
        double d;
    } converter;

    converter.d = val;

    // Entspricht Double.doubleToLongBits(val) >> 32
    // Extrahiert die oberen 32 Bit des 64-Bit Doubles
    double x = (double)(converter.i >> 32);

    // Die Umkehrrechnung der exp-Konstanten
    return (x - 1072632447.0) / 1512775.0;
}

