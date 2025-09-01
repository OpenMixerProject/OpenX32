#ifndef XREMOTE_H_
#define XREMOTE_H_

// general includes
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "WString.h"

// includes for UDP-communication
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>

// includes for reading IP-Address
#include <ifaddrs.h>
#include <netinet/in.h> 
#include <string.h> 
#include <arpa/inet.h>

int xremoteUdpHandle;
struct sockaddr_in xremoteServerAddr, xremoteClientAddr;
char xremote_TxMessage[450]; // the largest binary blob will take up to 20+(70+1))*4 bytes = 408 bytes

typedef union 
{
  uint32_t u32;
  int32_t s32;
  uint16_t u16[2];
  int16_t s16[2];
  uint8_t u8[4];
  int8_t s8[4];
  float   f;
}data_32b;

int counter=0;

char xremote_cmd_info[4]   = {'/', 'i', 'n', 'f'};
char xremote_cmd_xinfo[4]   = {'/', 'x', 'i', 'n'};
char xremote_cmd_status[4] = {'/', 's', 't', 'a'};
char xremote_cmd_xremote[4]   = {'/', 'x', 'r', 'e'};
char xremote_cmd_unsubscribe[4]   = {'/', 'u', 'n', 's'};
char xremote_cmd_channel[4]   = {'/', 'c', 'h', '/'};
char xremote_cmd_main[4]   = {'/', 'm', 'a', 'i'};
char xremote_cmd_stat[4]   = {'/', '-', 's', 't'};

// function prototypes
int8_t xremoteInit();
void xremoteUpdateAll();
void xremoteUdpHandleCommunication();
void xremoteAnswerInfo();
void xremoteAnswerXInfo();
void xremoteAnswerStatus();
void xremoteSetFader(uint8_t ch, float value_pu);
void xremoteSetPan(uint8_t ch, float value_pu);
void xremoteSetMainFader(float value_pu);
void xremoteSetMainPan(float value_pu);
void xremoteSetName(uint8_t ch, String name);
void xremoteSetColor(uint8_t ch, int32_t color);
void xremoteSetSource(uint8_t ch, int32_t source);
void xremoteSetIcon(uint8_t ch, int32_t icon);
void xremoteSetCard(uint8_t card);
void xremoteSetMute(uint8_t ch, uint8_t muted);
void xremoteSetSolo(uint8_t ch, uint8_t solo);
void xremoteUpdateMeter();
void xremoteSendUdpPacket(char *buffer, uint16_t size);
void xremoteSendBasicMessage(char *cmd, char type, char format, char *value);
uint16_t xremotesprint(char *bd, uint16_t index, char format, const char *bs);
uint16_t xremotefprint(char *bd, uint16_t index, char* text, char format, char *bs);
String xremoteGetIpAddress();
void xremoteCharToString(char *data, String &s);

#endif