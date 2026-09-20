#pragma once

/* SHARC memory space qualifiers */
#ifndef pm
#define pm
#endif

#ifndef dm
#define dm
#endif

/* SHARC section attribute macro */
#ifndef section
#define section(x)
#endif

/* SHARC compiler pragma placeholder */
#ifndef OP
#define OP
#endif

/* C++ restrict keyword */
#if defined(__cplusplus) && !defined(restrict)
#define restrict __restrict
#endif
