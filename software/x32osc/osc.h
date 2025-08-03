#ifndef OSC_H_
#define OSC_H_

// tinyOSC
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>


#include "tinyosc/tinyosc.h"
#include "surface.h"
#include "x32osc-reaper.h"


void oscInit(char* destIp);
void oscRun();
void oscSend(char* address, char* datatype, float data);

#endif
