/*
  This software offers a server for the XRemote-protocol

  The XRemote-protocol is used to communicate with the XEdit software.
  The communication is based on OSC-protocol.

  This file uses some parts of the MackieMCU-struct to keep names and meters in sync. The X32Edit displays
  all 32 channels at once. At the moment only volume-fader, pan, mute and solo is supported.

  Parts of this file are based on the "Unofficial X32/M32 OSC Remote Protocol" v4.02 by Patrick-Gilles Maillot.
  Thank you very much for sharing your work!

  Linux UDP examples:
  - https://gist.github.com/saxbophone/f770e86ceff9d488396c0c32d47b757e
  - https://openbook.rheinwerk-verlag.de/linux_unix_programmierung/Kap11-016.htm
*/

#include "xremote.h"

int8_t xremoteInit() {
	if ((xremoteUdpHandle = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		fprintf(stderr, "Error on creating UDP-socket!");
		return -1;
	}
    xremoteServerAddr.sin_family = AF_INET;
	xremoteServerAddr.sin_addr.s_addr = INADDR_ANY;
	xremoteServerAddr.sin_port = htons(10023);
    
	if (bind(xremoteUdpHandle, (const struct sockaddr *)&xremoteServerAddr, sizeof(xremoteServerAddr)) < 0) {
		fprintf(stderr, "Error on binding UDP-socket!");
		close(xremoteUdpHandle);
		return -1;
	}
	
    return 0;
}

// send data to XRemote client
void xremoteUpdateAll() {
    // TODO: insert our real data from X32 here
    for(uint8_t i=0; i<32; i++) {
        xremoteSetFader(i+1, 8000.0f/16383.0f); // TODO: implement real data
        xremoteSetPan(i+1, 128.0f/255.0f); // TODO: implement real data
        xremoteSetMute(i+1, 0); // TODO: implement real data
        xremoteSetSolo(i+1, 0); // TODO: implement real data
        xremoteSetColor(i+1, 0); // TODO: implement real data // 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE (add 64 to invert)
        xremoteSetName(i+1, String("Ch ") + String(i+1)); // TODO: implement real data
    }
    xremoteUpdateMeter();
}

// receive data from XRemote client
void xremoteUdpHandleCommunication() {
    char rxData[500];
	int bytes_available;
    uint8_t channel;
    data_32b value32bit;
	
	// check for bytes in UDP-buffer
	int result = ioctl(xremoteUdpHandle, FIONREAD, &bytes_available);
	if (bytes_available > 0) {
		socklen_t xremoteClientAddrLen = sizeof(xremoteClientAddr);
	    uint8_t len = recvfrom(xremoteUdpHandle, rxData, bytes_available, MSG_WAITALL, (struct sockaddr *) &xremoteClientAddr, &xremoteClientAddrLen);
        
        if (len > 0) {
            // TODO: implement data-interpreter
			fprintf(stdout, "%c %c %c %c\n", (uint8_t)rxData[0], (uint8_t)rxData[1], (uint8_t)rxData[2], (uint8_t)rxData[3]);

			if (memcmp(rxData, xremote_cmd_info, 4) == 0) {
			  // info
			  xremoteAnswerInfo();
			}else if (memcmp(rxData, xremote_cmd_xinfo, 4) == 0) {
			  // xinfo
			  xremoteAnswerXInfo();
			}else if (memcmp(rxData, xremote_cmd_status, 4) == 0) {
			  // status
			  xremoteAnswerStatus();
fprintf(stdout, "status");
			}else if (memcmp(rxData, xremote_cmd_xremote, 4) == 0) {
			  // xremote
			  // Optional: read and store IP-Address of client
			  // send routing, names and colors
			  for (uint8_t i=0; i<32; i++) {
				xremoteSetName(i+1, String("Ch") + String(i+1)); // TODO: implement own data
				xremoteSetColor(i+1, 0); // TODO: implement own data
				xremoteSetSource(i+1, i+1);
			  }
			  xremoteSetCard(10); // X-LIVE
fprintf(stdout, "xremote");
			}else if (memcmp(rxData, xremote_cmd_unsubscribe, 4) == 0) {
			  // unsubscribe
			  // Optional: remove xremote client
			}else if (memcmp(rxData, xremote_cmd_channel, 4) == 0) {
			  // channel

			  // /ch/xx/mix/fader~~~~,f~~[float]
			  // /ch/xx/mix/pan~~,f~~[float]
			  // /ch/xx/mix/on~~~,i~~[int]
			  channel = ((rxData[4]-48)*10 + (rxData[5]-48)) - 1;
			  if (len > 13) {
				if ((rxData[7] == 'm') && (rxData[8] == 'i') && (rxData[9] == 'x')) {
				  if ((rxData[11] == 'f') && (rxData[12] == 'a') && (rxData[13] == 'd')) {
					// get fader-value
					value32bit.u8[0] = rxData[27];
					value32bit.u8[1] = rxData[26];
					value32bit.u8[2] = rxData[25];
					value32bit.u8[3] = rxData[24];

					float newVolume = (value32bit.f * 54.0f) - 48.0f;
					//mixerSetVolume(channel, newVolume);
				  }else if ((rxData[11] == 'p') && (rxData[12] == 'a') && (rxData[13] == 'n')) {
					// get pan-value
					value32bit.u8[0] = rxData[23];
					value32bit.u8[1] = rxData[22];
					value32bit.u8[2] = rxData[21];
					value32bit.u8[3] = rxData[20];

/*
					MackieMCU.channel[channel].encoderValue = value32bit.f * 255.0f;
					mixerSetBalance(channel,  value32bit.f * 100.0f);
*/
				  }else if ((rxData[11] == 'o') && (rxData[12] == 'n')) {
					// get mute-state (caution: here it is "mixer-on"-state)
					//mixerSetMute(channel, (rxData[20+3] == 0));
				  }
				}else if ((rxData[7] == 'c') && (rxData[8] == 'o') && (rxData[9] == 'n')) {
				  // config
				  if  ((rxData[14] == 'c') && (rxData[15] == 'o') && (rxData[16] == 'l')) {
					// color
					value32bit.u8[0] = rxData[27];
					value32bit.u8[1] = rxData[26];
					value32bit.u8[2] = rxData[25];
					value32bit.u8[3] = rxData[24];

/*
					if (value32bit.u32 < 8) {
					  MackieMCU.channel[channel].color = value32bit.u32;
					}else{
					  MackieMCU.channel[channel].color = value32bit.u32 - 8 + 64;
					}
*/
				  }else if  ((rxData[14] == 'n') && (rxData[15] == 'a') && (rxData[16] == 'm')) {
					// name
					String name;
/*
					charToString(&rxData[24], name);
					MackieMCU.channel[channel].name = name;
*/
				  }else if  ((rxData[14] == 'i') && (rxData[15] == 'c') && (rxData[16] == 'o')) {
					// icon
					value32bit.u8[0] = rxData[27];
					value32bit.u8[1] = rxData[26];
					value32bit.u8[2] = rxData[25];
					value32bit.u8[3] = rxData[24];

					// do something with channel and value32bit.f
					//Serial.println("/ch/" + String(channel) + "/config/icon " + String(value32bit.u32));
				  }
				}
			  }
fprintf(stdout, "channel");
			}else if (memcmp(rxData, xremote_cmd_main, 4) == 0) {
			  // main

			  // /main/st/mix/fader~~,f~~[float]
			  // /main/st/mix/pan~~~~,f~~[float]
			  // /main/st/mix/on~,i~~[int]
			  if (len > 12) {
				if ((rxData[6] == 's') && (rxData[7] == 't') && (rxData[9] == 'm') && (rxData[10] == 'i') && (rxData[11] == 'x')) {
				  if ((rxData[13] == 'f') && (rxData[14] == 'a') && (rxData[15] == 'd')) {
					// get fader-value
					value32bit.u8[0] = rxData[27];
					value32bit.u8[1] = rxData[26];
					value32bit.u8[2] = rxData[25];
					value32bit.u8[3] = rxData[24];

					float newVolume = (value32bit.f * 54.0f) - 48.0f;
					//mixerSetMainVolume(newVolume);
				  }else if ((rxData[13] == 'p') && (rxData[14] == 'a') && (rxData[15] == 'n')) {
					// get pan-value
					value32bit.u8[0] = rxData[27];
					value32bit.u8[1] = rxData[26];
					value32bit.u8[2] = rxData[25];
					value32bit.u8[3] = rxData[24];

					//mixerSetMainBalance(value32bit.f * 100);
				  }else if ((rxData[13] == 'o') && (rxData[14] == 'n')) {
					// get mute-state
					// /main/st/mix/on~,i~~~
					// do something with channel and (rxData[20+3]) // 0 = mute off, 31 = mute on
				  }
				}
			  }
fprintf(stdout, "main");
			}else if (memcmp(rxData, xremote_cmd_stat, 4) == 0) {
			  // stat

			  if ((rxData[7] == 's') && (rxData[8] == 'o') && (rxData[9] == 'l') && (rxData[10] == 'o') && (rxData[11] == 's') && (rxData[12] == 'w')) {
				// /-stat/solosw/xx~~~~,i~~[integer]
				channel = ((rxData[14]-48)*10 + (rxData[15]-48)) - 1;
				value32bit.u8[0] = rxData[27];
				value32bit.u8[1] = rxData[26];
				value32bit.u8[2] = rxData[25];
				value32bit.u8[3] = rxData[24];

				// we receive solo-values for 80 channels
				if ((channel>=0) && (channel<32)) {
				  //mixerSetSolo(channel, (value32bit.u32 == 1));
				}
			  }else if ((rxData[7] == 'u') && (rxData[8] == 'r') && (rxData[9] == 'e') && (rxData[10] == 'c')) {
				value32bit.u8[0] = rxData[27];
				value32bit.u8[1] = rxData[26];
				value32bit.u8[2] = rxData[25];
				value32bit.u8[3] = rxData[24];

				// /-stat/urec/state~~~,i~~[integer]
				if (value32bit.u32 == 0) {
				  // stop
				}else if (value32bit.u32 == 1) {
				  // pause
				}else if (value32bit.u32 == 2) {
				  // play
				}else if (value32bit.u32 == 3) {
				  // record
				}
			  }
fprintf(stdout, "stat");
			}else{
			  // ignore unused commands for now
fprintf(stdout, "unused command");
			}
        }else{
fprintf(stdout, "len <= 0");
		}
	}
}

void xremoteAnswerInfo() {
    uint16_t len = xremotesprint(xremote_TxMessage, 0, 's', "/info");
    len = xremotesprint(xremote_TxMessage, len, 's', ",ssss");
    len = xremotesprint(xremote_TxMessage, len, 's', "V2.07");
    len = xremotesprint(xremote_TxMessage, len, 's', "OpenX32");
    len = xremotesprint(xremote_TxMessage, len, 's', "X32"); // must be a known device by X-Edit
    len = xremotesprint(xremote_TxMessage, len, 's', "4.13"); // must be a supported firmware-version by X-Edit

    xremoteSendUdpPacket(xremote_TxMessage, len);
}

void xremoteAnswerXInfo() {
    uint16_t len = xremotesprint(xremote_TxMessage, 0, 's', "/xinfo");
    len = xremotesprint(xremote_TxMessage, len, 's', ",ssss");
    len = xremotesprint(xremote_TxMessage, len, 's', "141.51.116.133"); // TODO: send our own IP
    len = xremotesprint(xremote_TxMessage, len, 's', "OpenX32");
    len = xremotesprint(xremote_TxMessage, len, 's', "X32"); // must be a known device by X-Edit
    len = xremotesprint(xremote_TxMessage, len, 's', "4.13"); // must be a supported firmware-version by X-Edit

    xremoteSendUdpPacket(xremote_TxMessage, len);
}

void xremoteAnswerStatus() {
    uint16_t len = xremotesprint(xremote_TxMessage, 0, 's', "/status");
    len = xremotesprint(xremote_TxMessage, len, 's', ",sss");
    len = xremotesprint(xremote_TxMessage, len, 's', "active");
    len = xremotesprint(xremote_TxMessage, len, 's', "141.51.116.133"); // TODO: send our own IP
    len = xremotesprint(xremote_TxMessage, len, 's', "OpenX32");

    xremoteSendUdpPacket(xremote_TxMessage, len);
}

void xremoteSetFader(uint8_t ch, float value_pu) {
    char cmd[32];
    sprintf(cmd, "/ch/%02i/mix/fader", ch);
    xremoteSendBasicMessage(cmd, 'f', 'b', (char*)&value_pu);
}

void xremoteSetPan(uint8_t ch, float value_pu) {
    char cmd[32];
    sprintf(cmd, "/ch/%02i/mix/pan", ch);
    xremoteSendBasicMessage(cmd, 'f', 'b', (char*)&value_pu);
}

void xremoteSetMainFader(float value_pu) {
    xremoteSendBasicMessage("/main/st/mix/fader", 'f', 'b', (char*)&value_pu);
}

void xremoteSetMainPan(float value_pu) {
    xremoteSendBasicMessage("/main/st/mix/pan", 'f', 'b', (char*)&value_pu);
}

void xremoteSetName(uint8_t ch, String name) {
    char nameArray[12];
    char cmd[50];
    name.toCharArray(nameArray, 12);
    sprintf(cmd, "/ch/%02i/config/name", ch);
    xremoteSendBasicMessage(cmd, 's', 's', nameArray);
}

// 0=BLACK, 1=RED, 2=GREEN, 3=YELLOW, 4=BLUE, 5=PINK, 6=CYAN, 7=WHITE (add 64 to invert)
void xremoteSetColor(uint8_t ch, int32_t color) {
    char cmd[32];
    sprintf(cmd, "/ch/%02i/config/color", ch);
    xremoteSendBasicMessage(cmd, 'i', 'b', (char*)&color);
}

void xremoteSetSource(uint8_t ch, int32_t source) {
    char cmd[32];
    sprintf(cmd, "/ch/%02i/config/source", ch);
    xremoteSendBasicMessage(cmd, 'i', 'b', (char*)&source);
}

void xremoteSetIcon(uint8_t ch, int32_t icon) {
    char cmd[32];
    sprintf(cmd, "/ch/%02i/config/icon", ch);
    xremoteSendBasicMessage(cmd, 'i', 'b', (char*)&icon);
}

void xremoteSetCard(uint8_t card) {
    char cmd[32];
    String scmd = "-stat/xcardtype " + String(card);
    scmd.toCharArray(cmd, scmd.length() + 1);
    cmd[scmd.length()] = 0x10; // add linefeed
    xremoteSendBasicMessage("node", 's', 's', cmd);
}

void xremoteSetMute(uint8_t ch, uint8_t muted) {
    char cmd[32];
    int32_t online;
    if (muted == 0) {
      online = 1;
    }else{
      online = 0;
    }
    sprintf(cmd, "/ch/%02i/mix/on", ch);
    xremoteSendBasicMessage(cmd, 'i', 'b', (char*)&online);
}

void xremoteSetSolo(uint8_t ch, uint8_t solo) {
    char cmd[20];
    String channel;
    String state;
    if (ch < 10) {
      channel = "0" + String(ch);
    }else{
      channel = String(ch);
    }
    if (solo > 0) {
      state = "ON";
    }else{
      state = "OFF";
    }
    String scmd = "-stat/solosw/" + channel + " " + state;
    scmd.toCharArray(cmd, 20);
    cmd[scmd.length()] = 0x10; // add linefeed
    xremoteSendBasicMessage("node", 's', 's', cmd);
}

void xremoteUpdateMeter() {
    int32_t value;

    uint16_t len = xremotesprint(xremote_TxMessage, 0, 's', "meters/0");
    len = xremotesprint(xremote_TxMessage, len, 's', ",b"); // 4 chars
    value = (70 + 1)*4; // number of bytes
    len = xremotesprint(xremote_TxMessage, len, 'b', (char*)&value); // big-endian
    value = 70; // number of floats
    len = xremotesprint(xremote_TxMessage, len, 'l', (char*)&value); // little endian

    float f;
    for (uint16_t i=0; i<70; i++) {
      f = 128.0f/255.0f; // 32 channels, 8 aux, 8 FX returns, 16 busse, 6 matrix
      len = xremotesprint(xremote_TxMessage, len, 'l', (char*)&f); // little endian
    }

    xremoteSendUdpPacket(xremote_TxMessage, len);
}

////////////////////////////////////
// low-level communication functions
////////////////////////////////////

void xremoteSendUdpPacket(char *buffer, uint16_t size) {
    sendto(
        xremoteUdpHandle,
        buffer,
        size,
        0,
        (struct sockaddr *) &xremoteClientAddr,
        sizeof(xremoteClientAddr)
    );
}

void xremoteSendBasicMessage(char *cmd, char type, char format, char *value) {
    char tmp[2];
    tmp[0] = ',';
    tmp[1] = type;
    
    uint16_t len = xremotesprint(xremote_TxMessage, 0, 's', cmd);
    len = xremotesprint(xremote_TxMessage, len, 's', tmp);
    len = xremotesprint(xremote_TxMessage, len, format, value);
    xremoteSendUdpPacket(xremote_TxMessage, len);
}

uint16_t xremotesprint(char *bd, uint16_t index, char format, const char *bs) {
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

uint16_t xremotefprint(char *bd, uint16_t index, char* text, char format, char *bs) {
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
    return xremotesprint(bd, index, format, bs);
}

int main(int argc, char *argv[]) {
    fprintf(stdout, "XRemote server test\n");

    fprintf(stdout, "  Binding UDP-Server to port 10023...\n");
    if (xremoteInit() != 0) {
        return -1;
    }
    fprintf(stdout, "  OK, waiting for incoming data...\n");

    while(1) {
        // check for incoming data
        xremoteUdpHandleCommunication();
        
        // sleep for 1ms to lower CPU-load
		counter++;
		if (counter >= 100) {
			//xremoteUpdateMeter();
			counter = 0;
		}
        usleep(1000);
    }

    return 0;
}
