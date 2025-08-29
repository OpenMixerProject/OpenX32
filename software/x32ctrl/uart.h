#ifndef UART_H_
#define UART_H_

#include "x32ctrl.h"





extern messageBuilder message;
//extern int fdDebug;
extern int fdSurface;
extern int fdAdda;
extern int fdFpga;
//extern int fdMidi;

void messageBuilderInit(messageBuilder *message);
int messageBuilderAddRawByte(messageBuilder *message, char byte);
int messageBuilderAddDataByte(messageBuilder *message, char byte);
int messageBuilderAddString(messageBuilder *message, const char *str);
int messageBuilderAddDataArray(messageBuilder *message, const char *data, uint8_t len);

uint8_t calculateChecksum(const char *data, uint16_t len);
int uartOpen(char *ttydev, uint32_t baudrate, int *fd, bool raw);
int uartTx(int *fd, messageBuilder *builder, bool addChecksum);
int uartRx(int *fd, char *buf, uint16_t bufLen);
int uartTxToFPGA(uint16_t cmd, data_64b *data);

#endif
