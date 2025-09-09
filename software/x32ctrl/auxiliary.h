#ifndef AUXILIARY_H_
#define AUXILIARY_H_

#include "x32ctrl.h"

#define VOLUME_MIN -100.0f

int readConfig(const char *filename, const char *key, char *value_buffer, size_t buffer_size);
uint8_t limitMin(uint8_t value, uint8_t min);
uint8_t limitMax(uint8_t value, uint8_t max);
float saturateMin_f(float value, float min);
float saturateMax_f(float value, float max);
uint8_t saturate(uint8_t value, uint8_t min, uint8_t max);
float saturate_f(float value, float min, float max);
float fader2dBfs(uint16_t faderValue);
uint16_t dBfs2fader(float dbfsValue);
String getIpAddress();
//int valueToBinaryString(uint8_t value, char *output);
int init100msTimer(); // only for Non-GUI systems
long getFileSize(const char *filename);
uint32_t reverseBitOrder_uint32(uint32_t n);
void reverseBitOrderArray(uint8_t* data, uint32_t len);

#endif
