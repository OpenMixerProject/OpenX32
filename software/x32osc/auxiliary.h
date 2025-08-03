#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include "x32osc-reaper.h"

int readConfig(const char *filename, const char *key, char *value_buffer, size_t buffer_size);
//int valueToBinaryString(uint8_t value, char *output);
//int initTimer();

#endif
