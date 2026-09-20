#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

static inline void vecvaddf(const float* a, const float* b, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

static inline void vecvsubf(const float* a, const float* b, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] - b[i];
    }
}

static inline void vecvmltf(const float* a, const float* b, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] * b[i];
    }
}

static inline void vecsmltf(const float* a, float s, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] * s;
    }
}

static inline void vecsaddf(const float* a, float s, float* c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + s;
    }
}

#ifdef __cplusplus
}
#endif
