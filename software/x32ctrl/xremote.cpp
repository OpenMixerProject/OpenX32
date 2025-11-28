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
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
  

  This file offers a server for the XRemote-protocol

  The XRemote-protocol is used to communicate with the XEdit software.
  The communication is based on OSC-protocol.

  Parts of this file are based on the "Unofficial X32/M32 OSC Remote Protocol" v4.02 by Patrick-Gilles Maillot.
  Thank you very much for sharing your work!

  Linux UDP examples:
  - https://gist.github.com/saxbophone/f770e86ceff9d488396c0c32d47b757e
  - https://openbook.rheinwerk-verlag.de/linux_unix_programmierung/Kap11-016.htm
*/

#include "xremote.h"

XRemote::XRemote(X32BaseParameter* basepar) : X32Base(basepar) {}


int8_t XRemote::Init() {
    if ((UdpHandle = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        //fprintf(stderr, "Error on creating UDP-socket!");
        return -1;
    }
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = INADDR_ANY;
    ServerAddr.sin_port = htons(10023);
    
    if (bind(UdpHandle, (const struct sockaddr *)&ServerAddr, sizeof(ServerAddr)) < 0) {
        //fprintf(stderr, "Error on binding UDP-socket!");
        close(UdpHandle);
        return -1;
    }
    
    return 0;
}

void XRemote::AnswerInfo() {
    uint16_t len = sprint(TxMessage, 0, 's', "/info");
    len = sprint(TxMessage, len, 's', ",ssss");
    len = sprint(TxMessage, len, 's', X32CTRL_VERSION);
    len = sprint(TxMessage, len, 's', "OpenX32");
    len = sprint(TxMessage, len, 's', "X32"); // must be a known device by X-Edit
    len = sprint(TxMessage, len, 's', "4.13"); // must be a supported firmware-version by X-Edit

    SendUdpPacket(TxMessage, len);
}

void XRemote::AnswerXInfo() {
    uint16_t len = sprint(TxMessage, 0, 's', "/xinfo");
    len = sprint(TxMessage, len, 's', ",ssss");
    len = sprint(TxMessage, len, 's', helper->getIpAddress().c_str());
    len = sprint(TxMessage, len, 's', "OpenX32");
    len = sprint(TxMessage, len, 's', "X32"); // must be a known device by X-Edit
    len = sprint(TxMessage, len, 's', "4.13"); // must be a supported firmware-version by X-Edit

    SendUdpPacket(TxMessage, len);
}

void XRemote::AnswerStatus() {
    uint16_t len = sprint(TxMessage, 0, 's', "/status");
    len = sprint(TxMessage, len, 's', ",sss");
    len = sprint(TxMessage, len, 's', "active");
    len = sprint(TxMessage, len, 's', helper->getIpAddress().c_str());
    len = sprint(TxMessage, len, 's', "OpenX32");

    SendUdpPacket(TxMessage, len);
}

void XRemote::SetFader(String type, uint8_t ch, float value) {
    char cmd[32] = {0};
    sprintf(cmd, "/%s/%02i/mix/fader", type.c_str(), ch+1);
    
    //SendBasicMessage((String("/") + type + String("/") + String() + String("/mix/fader")).c_str(), 'f', 'b', (char*)&value_pu);
    SendBasicMessage(cmd, 'f', 'b', (char*)&value);
}

void XRemote::SetPan(uint8_t ch, float value_pu) {
    char cmd[32] = {0};
    sprintf(cmd, "/ch/%02i/mix/pan", ch);
    SendBasicMessage(cmd, 'f', 'b', (char*)&value_pu);
}

void XRemote::SetMainFader(float value_pu) {
    SendBasicMessage("/main/st/mix/fader", 'f', 'b', (char*)&value_pu);
}

void XRemote::SetMainPan(float value_pu) {
    SendBasicMessage("/main/st/mix/pan", 'f', 'b', (char*)&value_pu);
}

void XRemote::SetName(uint8_t vchannelIndex, String name) {
    char nameArray[12] = {0};
    char cmd[50] = {0};
    name.toCharArray(nameArray, 12);
    sprintf(cmd, "/ch/%02i/config/name", vchannelIndex+1);
    SendBasicMessage(cmd, 's', 's', nameArray);
}

// 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE (add 64 to invert)
void XRemote::SetColor(uint8_t ch, int32_t color) {
    char cmd[32] = {0};
    sprintf(cmd, "/ch/%02i/config/color", ch);
    SendBasicMessage(cmd, 'i', 'b', (char*)&color);
}

void XRemote::SetSource(uint8_t ch, int32_t source) {
    char cmd[32] = {0};
    sprintf(cmd, "/ch/%02i/config/source", ch);
    SendBasicMessage(cmd, 'i', 'b', (char*)&source);
}

void XRemote::SetIcon(uint8_t ch, int32_t icon) {
    char cmd[32] = {0};
    sprintf(cmd, "/ch/%02i/config/icon", ch);
    SendBasicMessage(cmd, 'i', 'b', (char*)&icon);
}

void XRemote::SetCard(uint8_t card) {
    char cmd[32] = {0};
    String scmd = String("-stat/xcardtype ") + String(card);
    scmd.toCharArray(cmd, scmd.length() + 1);
    cmd[scmd.length()] = 0x10; // add linefeed
    SendBasicMessage("node", 's', 's', cmd);
}

void XRemote::SetMute(uint8_t ch, uint8_t muted) {
    char cmd[32] = {0};
    int32_t online;
    if (muted == 0) {
      online = 1;
    }else{
      online = 0;
    }
    sprintf(cmd, "/ch/%02i/mix/on", ch);
    SendBasicMessage(cmd, 'i', 'b', (char*)&online);
}

void XRemote::SetSolo(uint8_t ch, uint8_t solo) {
    char cmd[20] = {0};
    String channel;
    String state;
    if (ch < 10) {
      channel = "0" + String(ch);
    }else{
      channel = String(ch);
    }
    if (solo > 0) {
      state = String("ON");
    }else{
      state = String("OFF");
    }
    String scmd = String("-stat/solosw/") + channel + String(" ") + state;
    scmd.toCharArray(cmd, 20);
    cmd[scmd.length()] = 0x10; // add linefeed
    SendBasicMessage("node", 's', 's', cmd);
}

void XRemote::UpdateMeter(Mixer* mixer) {
    int32_t value;

    uint16_t len = sprint(TxMessage, 0, 's', "meters/0");
    len = sprint(TxMessage, len, 's', ",b"); // 4 chars
    value = (70 + 1)*4; // number of bytes
    len = sprint(TxMessage, len, 'b', (char*)&value); // big-endian
    value = 70; // number of floats
    len = sprint(TxMessage, len, 'l', (char*)&value); // little endian

    float f;
    for (uint16_t i=0; i<40; i++) {
      // TODO use right type
      len = sprint(TxMessage, len, 'l', String(mixer->dsp->Channel[i].meterPu).c_str()); // little endian
    }
    for (uint16_t i=40; i<70; i++) {
      f = (float)rand()/(float)RAND_MAX; // 32 channels, 8 aux, 8 FX returns, 16 busse, 6 matrix
      len = sprint(TxMessage, len, 'l', (char*)&f); // little endian
    }

    SendUdpPacket(TxMessage, len);
}

////////////////////////////////////
// low-level communication functions
////////////////////////////////////

void XRemote::SendUdpPacket(char* buffer, uint16_t size) {
    sendto(
        UdpHandle,
        buffer,
        size,
        0,
        (struct sockaddr *) &ClientAddr,
        sizeof(ClientAddr)
    );
}

void XRemote::SendBasicMessage(const char* cmd, char type, char format, char* value) {
    char tmp[3];
    tmp[0] = ',';
    tmp[1] = type;
    tmp[2] = 0;
    
    uint16_t len = sprint(TxMessage, 0, 's', cmd);
    len = sprint(TxMessage, len, 's', tmp);
    len = sprint(TxMessage, len, format, value);
    //helper->Debug(DEBUG_XREMOTE, (String(TxMessage) + String("\n")).c_str(), );
    helper->DebugPrintMessageWithNullBytes(DEBUG_XREMOTE, TxMessage, len);
    SendUdpPacket(TxMessage, len);
}

uint16_t XRemote::sprint(char* bd, uint16_t index, char format, const char* bs) {
    /*
      Based on the work of Patrick-Gilles Maillot
      https://github.com/pmaillot/X32-Behringer/blob/master/X32.c
    */
    
    int i;
    // check format
    switch (format) {
        case 's': // string : copy characters one at a time until a 0 is found
            if (bs) {
                strcpy(bd+index, bs);
                index += (int)strlen(bs) + 1;
            } else {
                bd[index++] = 0;
            }
            // align to 4 bytes boundary if needed
            while (index & 3) bd[index++] = 0;
            break;
        case 'b': // float or int : copy the 4 bytes of float or int in big-endian order
            i = 4;
            while (i > 0)
                bd[index++] = (char)(bs[--i]);
            break;
        case 'l': // float or int : copy the 4 bytes of float or int in little-endian order
            i = 0;
            while (i < 4)
                bd[index++] = (char)(bs[i++]);
            break;
        default:
            // don't copy anything
            break;
    }
    return index;
}

uint16_t XRemote::fprint(char* bd, uint16_t index, char* text, char format, char* bs) {
    /*
      Based on the work of Patrick-Gilles Maillot
      https://github.com/pmaillot/X32-Behringer/blob/master/X32.c
    */
    
    // first copy text
    strcpy (bd+index, text);
    index += (int)strlen(text) + 1;
    // align to 4 bytes boundary if needed
    while (index & 3) bd[index++] = 0;
    // then set format, keeping #4 alignment
    bd[index++] = ',';
    bd[index++] = format;
    bd[index++] = 0;
    bd[index++] = 0;
    // based on format, set value
    return sprint(bd, index, format, bs);
}
