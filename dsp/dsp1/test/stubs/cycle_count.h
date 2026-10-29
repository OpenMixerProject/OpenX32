/* host-test stub for the CCES header <cycle_count.h> */
#pragma once
typedef unsigned long long cycle_t;
#define START_CYCLE_COUNT(c) ((c) = 0)
#define STOP_CYCLE_COUNT(dst, c) ((dst) = (c))
