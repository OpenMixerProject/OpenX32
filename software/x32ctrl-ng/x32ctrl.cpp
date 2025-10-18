/*
							   =#%@@@@@###%@@@@%-
						   =*###+               :#@*
						+****.                      :%-
					  #++++             ############  :%-          @@@@@@@@@@@@
					.+===             ###======++==*##  *+       @@@*#%%#****#@@@
				   -=-+               #+======+=.*===*#         @@#**@.*@******%@
				  -=-+                ##======*  #====+#.      @@****@  @******@@
				  +:-                  ##+====#  #***==+#+   =@@**@@@@  @****%@@
				 =*=*                   -#*===#  ## #===+## @@@***@ @@  @@#*@@@
	  @@@@@@       ..                     ##+*#- ## #***==#= @@@@@@ @@   +@@@   @@@@@@@  @@@@@
	@@@    @@@                             ##= + ## ## #+==#- @@ @@ @@ = @@@    @@  @@  @@   @@
	@@      .@@#@@@@@@@  @@@ @@@ @@@@@@@@   .# # ## .= #-#++#= @ @  @@ * @*       @@*        @@
	@@       @@ @@    @@ @@@@@@@  @@   @@      # =#  = + *::=#   @  @+ *           -@@@   @@@=
	@@@    @@@  @@:   @@ @@       @@   @@      #  : .- : *::-#   @  +  #             @@ @@    @@
	  @@@@@@    @@@@@@@   @@@@@@  @@   @@@  =# # ## :+ #-#++#+ @ @  @@.* @@     @@@@@@  @@@@@@@@
				@@                         ##+ * ## ## #+==#+ @@@@@ @@ = @@@
				@@                        ##+= = ## #***==#+ @@***@ @@   #@@@
				   :                    :#*==+#: ## #===+## @@@***@ @@  @@#*@@#
				  .%+                  ##+====#  #***==+#=   +@@**@@@@  @****%@@
					%.                ##======*  #====*#  .*-  @@****@  @******@@
					 %=               #*======+: *===*#   +-=+  @@#**@ -@******@@
					  -@-             +##+=====+++=*##  ==-=-    @@@#%@@#****%@@@
						*@*             =###########  -===*        @@@@@@@@@@@@
						   @@%.                   .::=++*
							 .#@@%%*-.    .:=+**##***+.
								  .-+%%%%%%#***=-.

  Control software for Behringer X32 using OpenX32
  https://github.com/OpenMixerProject/OpenX32
  v0.1.5, 30.09.2025

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

  Parts of this software with kind permission of Music Tribe. Thank you!
*/

#include "x32ctrl.h"

X32Ctrl* ctrl;

// ###########################################################################
// #
// #      The Main Function - of course
// #
// ###########################################################################
int main(int argc, char* argv[]) {
	srand(time(NULL));

	Config* config = new Config();
	State* state = new State();
	config->SetDebug(true);
	parseParams(argc, argv, state);
	ctrl = new X32Ctrl(config, state);
	ctrl->Run();

	return 0;
}


// ###########################################################################
// #
// #      Timer
// #
// ###########################################################################

// timer-raw-functions to be called either by linux-timer (X32core) or LVGL-timer (all other models)
void timer100msCallbackLvgl(_lv_timer_t* lv_timer) {ctrl->Tick100ms();}
void timer100msCallbackLinux(int timer) {ctrl->Tick100ms();}
void timer10msCallbackLvgl(_lv_timer_t* lv_timer) {ui_tick(); ctrl->Tick10ms();}
void timer10msCallbackLinux(int timer) {ctrl->Tick10ms();}

timer_t timerid;
struct sigevent sev;
struct itimerspec trigger;

// initialize 100ms timer (only for Non-GUI systems)
int init100msTimer(void) {
  // Set up the signal handler
  struct sigaction sa;
  sa.sa_handler = timer100msCallbackLinux;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
	perror("sigaction");
	return 1;
  }

  // Set up the sigevent structure for the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;

  // Create the timer
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
	perror("timer_create");
	return 1;
  }

  // Set the timer to trigger every 1 second (1,000,000,000 nanoseconds)
  trigger.it_value.tv_sec = 0;
  trigger.it_value.tv_nsec = 50000000; // 50ms = 50000us = 50000000ns
  trigger.it_interval.tv_sec = 0;
  trigger.it_interval.tv_nsec = 50000000;

  // Arm the timer
  if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
	perror("timer_settime");
	return 1;
  }
}

// ###########################################################################
// #
// #      X32Ctrl Class
// #
// ###########################################################################

X32Ctrl::X32Ctrl(Config* config, State* state) : X32Base(config, state) {}

void X32Ctrl::Run(){

	helper->Log("  ____                  __   ______ ___  \n");
	helper->Log(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
	helper->Log("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
	helper->Log("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
	helper->Log("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
	helper->Log(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
	helper->Log("       | |                               \n");
	helper->Log("       |_|                               \n");
	helper->Log("OpenX32 Main Control\n");
	helper->Log("v0.2.0, 12.10.2025\n");
	helper->Log("https://github.com/OpenMixerProject/OpenX32\n");

	// first try to find what we are: Fullsize, Compact, Producer, Rack or Core
	helper->Debug("Reading config...\n");
	char model[12];
	char serial[15];
	char date[16];
	helper->ReadConfig("/etc/x32.conf", "MDL=", model, 12);
	helper->ReadConfig("/etc/x32.conf", "SN=", serial, 15);
	helper->ReadConfig("/etc/x32.conf", "DATE=", date, 16);
	helper->Log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

	config->SetModel(model);
	config->SetSamplerate(48000);
	config->SetBankMode(X32_SURFACE_MODE_BANKING_X32);

	mixer = new Mixer(config, state);
	
	surface = new Surface(config, state);
	surface->Init();

	xremote = new XRemote(config, state);
	xremote->Init();

	Init();
	InitPages();
	SetSelect(0, true);
	DebugPrintvChannels();

	if (config->IsModelX32Core()){
		// only necessary if LVGL is not used
		helper->Log("Starting Timer...\n");
		init100msTimer(); // start 100ms-timer only for Non-GUI systems

		state->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

		helper->Log("Wait for incoming data on /dev/ttymxc1...\n");
		helper->Log("Press Ctrl+C to terminate program.\n");

		while (1) {
			// run service-tasks
			Tick10ms();

			// sleep for 1ms to lower CPU-load
			usleep(1000);
		}

	} else {
		helper->Log("Initializing GUI...\n");
		guiInit(); // initializes LVGL, FBDEV and starts endless loop
	}
}


void X32Ctrl::Init(){
	// empty all banks
	// "Input Channels"
	for (uint8_t b = 0; b < 8; b++){
		for (uint8_t sCh = 0; sCh < 16; sCh++){
			modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
			modes[X32_SURFACE_MODE_BANKING_USER].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}
	// "Busses"
	for (uint8_t b = 0; b < 4; b++){
		for (uint8_t sCh = 0; sCh < 8; sCh++){
			modes[X32_SURFACE_MODE_BANKING_X32].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
			modes[X32_SURFACE_MODE_BANKING_USER].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}

	//##################################################################################
	//#
	//#   assign channels to input fader bank - X32 Default Layout
	//#
	//##################################################################################

	if (config->IsModelX32Full()){
		// bank is 16 channels wide
		for (uint8_t bank=0;bank<4;bank++){
			for (int i = 0; i <=15; i++) {
				modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 16);
				helper->Debug("Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 16));
			}
		}
	}

	if (config->IsModelX32CompactOrProducer()){
		// bank is 8 channels wide
		for (uint8_t bank=0;bank<8;bank++){
			for (int i = 0; i <=7; i++) {
				modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 8);
				helper->Debug("Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 8));
			}
		}
	}

	//##################################################################################
	//#
	//#   assign channels to bus fader bank - X32 Default Layout
	//#
	//##################################################################################

	// DCA - starts at channel 72
	for (int i = 0; i <=7; i++) {
		modes[X32_SURFACE_MODE_BANKING_X32].busBanks[0].surfaceChannel2VChannel[i] = i + 72;
	}

	// Bus 1-8 - starts at channel 48
	for (int i = 0; i <=7; i++) {
		modes[X32_SURFACE_MODE_BANKING_X32].busBanks[1].surfaceChannel2VChannel[i] = i + 48;
	}

	// Bus 9-16 - starts at channel 56
	for (int i = 0; i <=7; i++) {
		modes[X32_SURFACE_MODE_BANKING_X32].busBanks[2].surfaceChannel2VChannel[i] = i + 56;
	}

	// Matrix / SPECIAL / SUB - starts at channel 64
	for (int i = 0; i <=9; i++) {
		modes[X32_SURFACE_MODE_BANKING_X32].busBanks[3].surfaceChannel2VChannel[i] = i + 64;
	}

	activeBank_inputFader = 0;
	activeBank_busFader = 0;
	activeEQ = 0;
	activeBusSend = 0;
}

void parseParams(int argc, char* argv[], State* state) {
	for (int8_t i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-fpga") == 0) {
			if (i + 1 < argc) {
				state->switchFpga = i+1;
				state->switchFpgaPath = String(argv[state->switchFpga]);
				i++;
			} else {
				state->switchFpga = -1;
			}
		}
		else if (strcmp(argv[i], "-dsp1") == 0) {
			if (i + 1 < argc) {
				state->switchDsp1 = i+1;
				state->switchDsp1Path = String(argv[state->switchDsp1]);
				i++;
			} else {
				state->switchDsp1 = -1;
			}
		}
		else if (strcmp(argv[i], "-dsp2") == 0) {
			if (i + 1 < argc) {
				state->switchDsp2 = i+1;
				state->switchDsp2Path = String(argv[state->switchDsp2]);
				i++;
			} else {
				state->switchDsp2 = -1;
			}
		}
		else if (strcmp(argv[i], "-noinit") == 0) {
			if (i + 1 < argc) {
				state->switchNoinit = i+1;
				i++;
			} else {
				state->switchNoinit = -1;
			}
		}
		// handle unknown parameters
		else {
			printf("Unknown parameter: %s\n", argv[i]);
		}
	}
}

//#####################################################################################################################
//#####################################################################################################################
//
// 			T I M E R
//
//#####################################################################################################################
//#####################################################################################################################


void X32Ctrl::Tick10ms(void){
	surface->ProcessUartData();
	mixer->ProcessUartData();

	ProcessEvents();

	// TODO
	// // continuously read data from both DSPs if we expect data
	// spiSendDspParameterArray(0, '?', 0, 0, dataToRead[0], NULL); // dummy-command just for reading without adding data to TxBuffer
	// spiSendDspParameterArray(1, '?', 0, 0, dataToRead[1], NULL); // dummy-command just for reading without adding data to TxBuffer

	// communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
	UdpHandleCommunication();

	syncAll();
}

void X32Ctrl::Tick100ms(void){
	// TODO
	// // surface wants to know the current state of all LED's and Meters
	// surfaceKeepalive();
	
	touchcontrolTick();
	surfaceUpdateMeter();

	// update meters on XRemote-clients
	xremote->UpdateMeter(mixer);

	// TODO
	// // toggle the LED on DSP1 and DSP2 to show some activity
	// spiSendDspParameter_uint32(0, 'a', 42, 0, 2);
	// spiSendDspParameter_uint32(1, 'a', 42, 0, 2);

	// read meter- and dynamics-information from DSP
	if (config->IsModelX32FullOrCompactOrProducer()) {
		// TODO
		//spiSendDspParameter_uint32(0, '?', 'm', 0, 0); // non-blocking request of meter-data
		// //spiSendDspParameter_uint32(0, '?', 'd', 0, 0); // non-blocking request of gate- and compression
	}

	// read the current DSP load
	if (!config->IsModelX32Core()) {
		// TODO
		// spiSendDspParameter_uint32(0, '?', 'c', 0, 0); // non-blocking request of DSP-Load-parameter
		// spiSendDspParameter_uint32(1, '?', 'c', 0, 0); // non-blocking request of DSP-Load-parameter
		// lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% [v%.2f]", dsp.dspLoad[0], dsp.dspVersion[0], dsp.dspLoad[1], dsp.dspVersion[1]); // show the received value (could be a bit older than the request)
	}
}


//#####################################################################################################################
//#####################################################################################################################
//
// 			E V E N T S
//
//#####################################################################################################################
//#####################################################################################################################

void X32Ctrl::ProcessEvents(void){

  // ############################################
  // #
  // #      Surface-Events
  // #
  // ############################################

  while(surface->HasNextEvent()){
	SurfaceEvent* event = surface->GetNextEvent();
	helper->Debug("Event: %s\n", (event->ToString()).c_str());

	switch(event->classId){
	  case 'f':
		FaderMoved(event);
		break;
	  case 'b':
		ButtonPressed(event);
		break;
	  case 'e':
		EncoderTurned(event);
		break;
	  default:
		helper->Debug("unknown message: %s\n", event->ToString().c_str());
	}

	delete(event);
  }

  // ############################################
  // #
  // #      ADDA-Events
  // #
  // ############################################


  // ############################################
  // #
  // #      DSP-Events
  // #
  // ############################################

  // TODO DSP 1 + 2

// void callbackDsp1(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
//     float* floatValues = (float*)values;
//     uint32_t* intValues = (uint32_t*)values;

//     switch (classId) {
//         case 's': // status-feedback
//             switch (channel) {
//                 case 'v': // DSP-Version
//                     if (valueCount == 1) {
//                         dsp->dspVersion[0] = floatValues[0];
//                     }
//                     break;
//                 case 'c': // DSP-Load in dspClockCycles
//                     if (valueCount == 1) {
//                         dsp->dspLoad[0] = (((float)intValues[0]/264.0f) / (16.0f/0.048f)) * 100.0f;
//                     }
//                     break;
//             }
//             break;
//         case 'm': // meter information
//             // copy meter-info to individual channels
//             // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
//             if (valueCount == 43) {
//                 for (int i = 0; i < 40; i++) {
//                     dsp->dspChannel[i].meterPu = abs(floatValues[i])/2147483648.0f; // convert 32-bit value to p.u.
//                     uint32_t data = (uint32_t)abs(floatValues[i]); // convert received float-value to unsigned integer
//                     // data contains a 32-bit sample-value
//                     // lets check the threshold and set meterInfo
//                     dsp->dspChannel[i].meterInfo = 0;
//                     if (data >= vuThresholds[0])  { dsp->dspChannel[i].meterInfo |= 0b00100000; } // CLIP
//                     if (data >= vuThresholds[5])  { dsp->dspChannel[i].meterInfo |= 0b00010000; } // -6dBfs
//                     if (data >= vuThresholds[8])  { dsp->dspChannel[i].meterInfo |= 0b00001000; } // -12dBfs
//                     if (data >= vuThresholds[10]) { dsp->dspChannel[i].meterInfo |= 0b00000100; } // -18dBfs
//                     if (data >= vuThresholds[14]) { dsp->dspChannel[i].meterInfo |= 0b00000010; } // -30dBfs
//                     if (data >= vuThresholds[24]) { dsp->dspChannel[i].meterInfo |= 0b00000001; } // -60dBfs

//                     // the dynamic-information is received with the 'd' information, but we will store them here
//                     if (dsp->dspChannel[i].gate.gain < 1.0f) { dsp->dspChannel[i].meterInfo |= 0b01000000; }
//                     if (dsp->dspChannel[i].compressor.gain < 1.0f) { dsp->dspChannel[i].meterInfo |= 0b10000000; }
//                 }
//                 dsp->mainChannelLR.meterPu[0] = abs(floatValues[40])/2147483648.0f; // convert 32-bit value to p.u.
//                 dsp->mainChannelLR.meterPu[1] = abs(floatValues[41])/2147483648.0f; // convert 32-bit value to p.u.
//                 dsp->mainChannelSub.meterPu[0] = abs(floatValues[42])/2147483648.0f; // convert 32-bit value to p.u.

//                 // leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
//                 // leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
//                 dsp->mainChannelLR.meterInfo[0] = 0;
//                 dsp->mainChannelLR.meterInfo[1] = 0;
//                 dsp->mainChannelSub.meterInfo[0] = 0;
//                 uint32_t data[3];
//                 data[0] = abs(floatValues[40]);
//                 data[1] = abs(floatValues[41]);
//                 data[2] = abs(floatValues[42]);
//                 for (int i = 0; i < 24; i++) {
//                     if (data[0] >= vuThresholds[i]) { dsp->mainChannelLR.meterInfo[0]  |= (1U << (23 - i)); }
//                     if (data[1] >= vuThresholds[i]) { dsp->mainChannelLR.meterInfo[1]  |= (1U << (23 - i)); }
//                     if (data[2] >= vuThresholds[i]) { dsp->mainChannelSub.meterInfo[0] |= (1U << (23 - i)); }
//                 }
//             }
//             break;
//         case 'd': // dynamics-information
//             if (valueCount == 80) {
//                 // first copy the compression-information
//                 for (int i = 0; i < 40; i++) {
//                     dsp->dspChannel[i].compressor.gain = floatValues[i];
//                     dsp->dspChannel[i].gate.gain = floatValues[40 + i];
//                 }
//             }
//         default:
//             break;
//     }
// }

// void callbackDsp2(uint8_t classId, uint8_t channel, uint8_t index, uint8_t valueCount, void* values) {
//     float* floatValues = (float*)values;
//     uint32_t* intValues = (uint32_t*)values;

//     switch (classId) {
//         case 's': // status-feedback
//             switch (channel) {
//                 case 'v': // DSP-Version
//                     if (valueCount == 1) {
//                         dsp->dspVersion[1] = floatValues[0];
//                     }
//                     break;
//                 case 'c': // DSP-Load in dspClockCycles
//                     if (valueCount == 1) {
//                         dsp->dspLoad[1] = (((float)intValues[0]/264.0f) / (16.0f/0.048f)) * 100.0f;
//                     }
//                     break;
//             }
//             break;
//         default:
//             break;
//     }
// }

  // ############################################
  // #
  // #      XRemote-Events
  // #
  // ############################################

  UdpHandleCommunication();

}

// receive data from XRemote client
void X32Ctrl::UdpHandleCommunication(void) {
    char rxData[500];
    int bytes_available;
    uint8_t channel;
    data_32b value32bit;
    
    // check for bytes in UDP-buffer
    int result = ioctl(xremote->UdpHandle, FIONREAD, &bytes_available);
    if (bytes_available > 0) {
        socklen_t xremoteClientAddrLen = sizeof(xremote->ClientAddr);
        uint8_t len = recvfrom(xremote->UdpHandle, rxData, bytes_available, MSG_WAITALL, (struct sockaddr *) &xremote->ClientAddr, &xremoteClientAddrLen);
        
        if (len > 0) {
            if (String(rxData) != "/renew") {
                //fprintf(stdout, "Received command: %s\n", rxData);
            }
            
            if (memcmp(rxData, "/inf", 4) == 0) {
                // info
                xremote->AnswerInfo();
            }else if (memcmp(rxData, "/xinf", 4) == 0) {
                // xinfo
                xremote->AnswerXInfo();
            }else if (memcmp(rxData, "/sta", 4) == 0) {
                // status
                xremote->AnswerStatus();
            }else if (memcmp(rxData, "/xre", 4) == 0) {
                // xremote
                // Optional: read and store IP-Address of client
                // send routing, names and colors
                for (uint8_t i=0; i<32; i++) {
                    xremote->SetName(i+1, String("Ch") + String(i+1)); // TODO: implement own data
                    xremote->SetColor(i+1, 0); // TODO: implement own data
                    xremote->SetSource(i+1, i+1);
                }
                xremote->SetCard(10); // X-LIVE
            }else if (memcmp(rxData, "/uns", 4) == 0) {
                // unsubscribe
                // Optional: remove xremote client
            }else if (memcmp(rxData, "/ch/", 4) == 0) {
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
                            mixer->SetVolume(channel, newVolume);
                            helper->Debug("Ch %u: Volume set to %f\n",  channel+1, newVolume);
                        }else if ((rxData[11] == 'p') && (rxData[12] == 'a') && (rxData[13] == 'n')) {
                            // get pan-value
                            value32bit.u8[0] = rxData[23];
                            value32bit.u8[1] = rxData[22];
                            value32bit.u8[2] = rxData[21];
                            value32bit.u8[3] = rxData[20];
                            
                            //encoderValue = value32bit.f * 255.0f;
                            mixer->SetBalance(channel,  value32bit.f * 100.0f);
                            helper->Debug("Ch %u: Balance set to %f\n",  channel+1, value32bit.f * 100.0f);
                        }else if ((rxData[11] == 'o') && (rxData[12] == 'n')) {
                            // get mute-state (caution: here it is "mixer-on"-state)
                            mixer->SetMute(channel, (rxData[20+3] == 0));
                            helper->Debug("Ch %u: Mute set to %u\n",  channel+1, (rxData[20+3] == 0));
                        }
                    }else if ((rxData[7] == 'c') && (rxData[8] == 'o') && (rxData[9] == 'n')) {
                        // config
                        if  ((rxData[14] == 'c') && (rxData[15] == 'o') && (rxData[16] == 'l')) {
                            // color
                            value32bit.u8[0] = rxData[27];
                            value32bit.u8[1] = rxData[26];
                            value32bit.u8[2] = rxData[25];
                            value32bit.u8[3] = rxData[24];
                            
                            if (value32bit.u32 < 8) {
                                //fprintf(stdout, "Ch %u: Set color to %u\n",  channel+1, value32bit.u32);
                            }else{
                                //fprintf(stdout, "Ch %u: Set inverted color to %u\n",  channel+1, value32bit.u32 - 8 +64);
                            }
                        }else if  ((rxData[14] == 'n') && (rxData[15] == 'a') && (rxData[16] == 'm')) {
                            // name
                            String name = String(&rxData[24]);
                            //fprintf(stdout, "Ch %u: Set name to %s\n",  channel+1, name.c_str());
                        }else if  ((rxData[14] == 'i') && (rxData[15] == 'c') && (rxData[16] == 'o')) {
                            // icon
                            value32bit.u8[0] = rxData[27];
                            value32bit.u8[1] = rxData[26];
                            value32bit.u8[2] = rxData[25];
                            value32bit.u8[3] = rxData[24];
                            
                            // do something with channel and value32bit.f
                            //Serial.println("/ch/" + String(channel) + "/config/icon " + String(value32bit.u32));
                            //fprintf(stdout, "Ch %u: Set icon to %u\n",  channel+1, value32bit.u32);
                        }
                    }
                }
            }else if (memcmp(rxData, "/mai", 4) == 0) {
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
            }else if (memcmp(rxData, "/-st", 4) == 0) {
                // stat
                
                if ((rxData[7] == 's') && (rxData[8] == 'o') && (rxData[9] == 'l') && (rxData[10] == 'o') && (rxData[11] == 's') && (rxData[12] == 'w')) {
                    // /-stat/solosw/xx~~~~,i~~[integer]
                    channel = ((rxData[14]-48)*10 + (rxData[15]-48)) - 1;
                    value32bit.u8[0] = rxData[27];
                    value32bit.u8[1] = rxData[26];
                    value32bit.u8[2] = rxData[25];
                    value32bit.u8[3] = rxData[24];
                    
                    // we receive solo-values for 80 channels
/*
                    if (channel < 32) {
                        mixerSetSolo(channel, (value32bit.u32 == 1));
                    }
*/
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
                
                //fprintf(stdout, "Received command: %s\n", rxData);
            }else if (memcmp(rxData, "/bat", 4) == 0) {
            }else if (memcmp(rxData, "/ren", 4) == 0) {
            }else if (memcmp(rxData, "/for", 4) == 0) {
            }else{
                // ignore unused commands for now
                //fprintf(stdout, "Received unsupported command: %s\n", rxData);
            }
        }else{
            //fprintf(stdout, "Caution: len <= 0");
        }
    }
}

// ####################################################################
// #
// #
// #        GUI
// #
// #
// ###################################################################

void X32Ctrl::guiInit(void) {
  lv_init();

  driver_backends_register();
  driver_backends_init_backend("FBDEV");

  lv_timer_create(timer10msCallbackLvgl, 10, NULL);
  lv_timer_create(timer100msCallbackLvgl, 100, NULL); // surface/gui sync

  // initialize GUI created by EEZ
  ui_init();

  ShowPage((X32_PAGE)-1);   // shows the welcome page
  state->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface

  // setup EQ-graph
  lv_chart_set_type(objects.current_channel_eq, LV_CHART_TYPE_LINE);
  lv_obj_set_style_size(objects.current_channel_eq, 0, 0, LV_PART_INDICATOR);
  lv_obj_set_style_line_width(objects.current_channel_eq, 5, LV_PART_ITEMS);
  chartSeriesEQ = lv_chart_add_series(objects.current_channel_eq, lv_palette_main(LV_PALETTE_AMBER), LV_CHART_AXIS_PRIMARY_Y);
  lv_chart_set_div_line_count(objects.current_channel_eq, 5, 7);
  lv_chart_set_range(objects.current_channel_eq, LV_CHART_AXIS_PRIMARY_Y, -15000, 15000);
  lv_chart_set_point_count(objects.current_channel_eq, 200);
  lv_chart_set_series_color(objects.current_channel_eq, chartSeriesEQ, lv_color_hex(0xef7900));
  //chart-shadow: 0x7e4000

  // start endless loop
  driver_backends_run_loop();
}

void X32Ctrl::guiSetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
	sprintf(&displayEncoderText[0][0], "%s", enc1.c_str());
	sprintf(&displayEncoderText[1][0], "%s", enc2.c_str());
	sprintf(&displayEncoderText[2][0], "%s", enc3.c_str());
	sprintf(&displayEncoderText[3][0], "%s", enc4.c_str());
	sprintf(&displayEncoderText[4][0], "%s", enc5.c_str());
	sprintf(&displayEncoderText[5][0], "%s", enc6.c_str());
	lv_btnmatrix_set_map(objects.display_encoders, displayEncoderButtonMap);
}

void X32Ctrl::DrawEq(uint8_t selectedChannelIndex) {
	if (selectedChannelIndex >= 40) {
		return;
	}

	// calculate the filter-response between 20 Hz and 20 kHz for all 4 PEQs
	sPEQ* peq;
	float eqValue[200];
	float freq;

	memset(&eqValue[0], 0, sizeof(eqValue));

	for (uint16_t pixel = 0; pixel < 200; pixel++) {
		freq = 20.0f * powf(1000.0f, ((float)pixel/199.0f));

		// LowCut
		eqValue[pixel] += mixer->dsp->fx->CalcFrequencyResponse_LC(freq, mixer->dsp->Channel[selectedChannelIndex].lowCutFrequency, config->GetSamplerate());

		// PEQ
		for (uint8_t i_peq = 0; i_peq < MAX_CHAN_EQS; i_peq++) {
			peq = &mixer->dsp->Channel[GetSelectedvChannelIndex()].peq[i_peq];
			eqValue[pixel] += mixer->dsp->fx->CalcFrequencyResponse_PEQ(peq->a[0], peq->a[1], peq->a[2], peq->b[1], peq->b[2], freq, config->GetSamplerate());
		}
	}

	int32_t* chartSeriesEqPoints = lv_chart_get_series_y_array(objects.current_channel_eq, chartSeriesEQ);
	for (uint16_t i = 0; i < 200; i++) {
	   chartSeriesEqPoints[i] = eqValue[i] * 1000.0f; // draw diagonal line
	}
	lv_chart_refresh(objects.current_channel_eq);
}

void X32Ctrl::InitPages(void){
	// Init Pages
	pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
	pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

	pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
	pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_EQ;

	pages[X32_PAGE_EQ].prevPage = X32_PAGE_CONFIG;
	pages[X32_PAGE_EQ].nextPage = X32_PAGE_NONE;
}

void X32Ctrl::ShowNextPage(void){
	if (pages[activePage].nextPage != X32_PAGE_NONE){
		ShowPage(pages[activePage].nextPage);
	}
}

void X32Ctrl::ShowPrevPage(void){
	if (pages[activePage].prevPage != X32_PAGE_NONE){
		ShowPage(pages[activePage].prevPage);
	}
}


void X32Ctrl::ShowPage(X32_PAGE p_page) {  // TODO: move to GUI Update section

	// first turn all page LEDs off
	surface->SetLedByEnum(X32_BTN_HOME, 0);
	surface->SetLedByEnum(X32_BTN_METERS, 0);
	surface->SetLedByEnum(X32_BTN_ROUTING, 0);
	surface->SetLedByEnum(X32_BTN_SETUP, 0);
	surface->SetLedByEnum(X32_BTN_LIBRARY, 0);
	surface->SetLedByEnum(X32_BTN_EFFECTS, 0);
	surface->SetLedByEnum(X32_BTN_MUTE_GRP, 0);
	surface->SetLedByEnum(X32_BTN_UTILITY, 0);
	// turn all view LEDs of
	surface->SetLedByEnum(X32_BTN_VIEW_CONFIG, 0);
	surface->SetLedByEnum(X32_BTN_VIEW_EQ, 0);

	activePage = p_page;

	switch (activePage)
	{
		case X32_PAGE_HOME:
			lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 0, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_HOME, 1);
			break;
		case X32_PAGE_CONFIG:
			lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 1, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_VIEW_CONFIG, 1);
			break;

		case X32_PAGE_EQ:
			lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 4, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_VIEW_EQ, 1);
			break;

		case X32_PAGE_METERS:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_METERS, 1);
			break;
		case X32_PAGE_ROUTING:
			lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_ROUTING, 1);
			break;
		case X32_PAGE_SETUP:
			lv_tabview_set_active(objects.maintab, 4, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_SETUP, 1);
			break;
		case X32_PAGE_LIBRARY:
			lv_tabview_set_active(objects.maintab, 5, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_LIBRARY, 1);
			break;
		case X32_PAGE_EFFECTS:
			lv_tabview_set_active(objects.maintab, 6, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_EFFECTS, 1);
			break;
		case X32_PAGE_MUTE_GRP:
			lv_tabview_set_active(objects.maintab, 7, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_MUTE_GRP, 1);
			break;
		case X32_PAGE_UTILITY:
			lv_tabview_set_active(objects.maintab, 8, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_UTILITY, 1);
			break;
		default:
			activePage = X32_PAGE_NONE;
			helper->Debug("ERROR: Page not found! Show welcome page instead.\n");
	}

	state->SetChangeFlags(X32_MIXER_CHANGED_PAGE);
}

//#####################################################################################################################
//#####################################################################################################################
//
// 			S Y N C
//
//#####################################################################################################################
//#####################################################################################################################

// sync mixer state to GUI
void X32Ctrl::guiSync(void) {
	if (config->IsModelX32Core()){
		return;
	}

	helper->Debug("Active Page: %d\n", activePage);

	VChannel* chan = GetSelectedvChannel();
	uint8_t chanIndex = GetSelectedvChannelIndex();

	//####################################
	//#         General
	//####################################

	lv_color_t color;
	switch (chan->color){
		case SURFACE_COLOR_BLACK:
			color = lv_color_make(0, 0, 0);
			break;
		case SURFACE_COLOR_RED:
			color = lv_color_make(255, 0, 0);
			break;
		case SURFACE_COLOR_GREEN:
			color = lv_color_make(0, 255, 0);
			break;
		case SURFACE_COLOR_YELLOW:
			color = lv_color_make(255, 255, 0);
			break;
		case SURFACE_COLOR_BLUE:
			color = lv_color_make(0, 0, 255);
			break;
		case SURFACE_COLOR_PINK:
			color = lv_color_make(255, 0, 255);
			break;
		case SURFACE_COLOR_CYAN:
			color = lv_color_make(0, 255, 255);
			break;
		case SURFACE_COLOR_WHITE:
			color = lv_color_make(255, 255, 255);
			break;
	}

	// TODO
	// lv_label_set_text_fmt(objects.current_channel_number, "vCh%d", pChannelSelected.index);
	lv_label_set_text_fmt(objects.current_channel_name, "%s", chan->name.c_str());
	lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);

	// //set Encoders to default state
	// const char*  encoderTextMap[] = {"Input", " ", " "," "," ","Output", NULL};
	// lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);

	//####################################
	//#         Page Home
	//####################################


	// TODO
	//bool phantomPower = mixer->halGetPhantomPower(pChannelSelected->index);
	
	if (activePage == X32_PAGE_CONFIG){
	//####################################
	//#         Page Config
	//####################################
		char dspSourceName[5] = "";
		char inputSourceName[10] = "";
		// TODO dspGetSourceName(&dspSourceName[0], pChannelSelected.index);
		sprintf(&inputSourceName[0], "%02d: %s", (chanIndex + 1), dspSourceName);
		lv_label_set_text_fmt(objects.current_channel_source, inputSourceName);

		lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)mixer->halGetGain(chanIndex));
		// TODO
		//lv_label_set_text_fmt(objects.current_channel_phantom, "%d", phantomPower);
		lv_label_set_text_fmt(objects.current_channel_invert, "%d", mixer->halGetPhaseInvert(chanIndex));
		lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)mixer->halGetBalance(chanIndex));
		lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)mixer->halGetVolume(chanIndex));


		//char outputDestinationName[10] = "";
		//routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
		//lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

		guiSetEncoderText("Source", "Gain", "-", "-", "-", "-");
	}else if (activePage == X32_PAGE_ROUTING) {
	//####################################
	//#         Page Routing
	//####################################
		// char outputDestinationName[10] = "";
		// char inputSourceName[10] = "";
		// uint8_t routingIndex = 0;

		// // read name of selected output-routing channel
		// fpgaRoutingGetOutputNameByIndex(&outputDestinationName[0], selectedOutputChannelIndex); // selectedOutputChannelIndex = 1..112
		// lv_label_set_text_fmt(objects.hardware_channel_output, outputDestinationName);

		// // find name of currently set input-source
		// routingIndex = fpgaRoutingGetOutputSourceByIndex(selectedOutputChannelIndex); // selectedOutputChannelIndex = 1..112
		// fpgaRoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex); // routingIndex = 0..112
		// lv_label_set_text_fmt(objects.hardware_channel_source, inputSourceName);

		// guiSetEncoderText("Output", "Source", "-", "-", "-", "-");
	}else if (activePage == X32_PAGE_EQ) {
	//####################################
	//#         Page EQ
	//####################################
		// draw EQ-plot
		DrawEq(GetSelectedvChannelIndex());

		if (chanIndex < 40) {
			// support EQ-channel
			guiSetEncoderText("LC: " + helper->freq2String(mixer->dsp->Channel[chanIndex].lowCutFrequency),
				"F: " + helper->freq2String(mixer->dsp->Channel[chanIndex].peq[activeEQ].fc),
				"G: " + String(mixer->dsp->Channel[chanIndex].peq[activeEQ].gain, 1) + " dB",
				"Q: " + String(mixer->dsp->Channel[chanIndex].peq[activeEQ].Q, 1),
				"M: " + helper->eqType2String(mixer->dsp->Channel[chanIndex].peq[activeEQ].type),
				"PEQ: " + String(activeEQ + 1)
			);
		}else{
			// unsupported at the moment
			guiSetEncoderText("-", "-", "-", "-", "-", "-");
		}
	}else if (activePage == X32_PAGE_METERS) {
	//####################################
	//#         Page Meters
	//####################################

		// TODO

		for(int i=0; i<=15; i++){
			VChannel* chan = GetVChannel(i);
			uint8_t chanIndex = i;

			// TODO
			// if (phantomPower){
			//     lv_buttonmatrix_set_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
			// } else {
			//     lv_buttonmatrix_clear_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
			// }

			switch (i){
				case 0:
					lv_slider_set_value(objects.slider01, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 1:
					lv_slider_set_value(objects.slider02, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 2:
					lv_slider_set_value(objects.slider03, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 3:
					lv_slider_set_value(objects.slider04, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 4:
					lv_slider_set_value(objects.slider05, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 5:
					lv_slider_set_value(objects.slider06, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 6:
					lv_slider_set_value(objects.slider07, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 7:
					lv_slider_set_value(objects.slider08, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 8:
					lv_slider_set_value(objects.slider09, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 9:
					lv_slider_set_value(objects.slider10, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 10:
					lv_slider_set_value(objects.slider11, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 11:
					lv_slider_set_value(objects.slider12, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 12:
					lv_slider_set_value(objects.slider13, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 13:
					lv_slider_set_value(objects.slider14, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 14:
					lv_slider_set_value(objects.slider15, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
				case 15:
					lv_slider_set_value(objects.slider16, helper->Dbfs2Fader(mixer->dsp->Channel[chanIndex].volumeLR), LV_ANIM_OFF);
					break;
			}
		}

		lv_label_set_text_fmt(objects.volumes, "%2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB %2.1fdB", 
			(double)mixer->dsp->Channel[0].volumeLR,
			(double)mixer->dsp->Channel[1].volumeLR,
			(double)mixer->dsp->Channel[2].volumeLR,
			(double)mixer->dsp->Channel[3].volumeLR,
			(double)mixer->dsp->Channel[4].volumeLR,
			(double)mixer->dsp->Channel[5].volumeLR,
			(double)mixer->dsp->Channel[6].volumeLR,
			(double)mixer->dsp->Channel[7].volumeLR
		);
	}else if (activePage == X32_PAGE_SETUP) {
	//####################################
	//#         Page Setup
	//####################################

		// pChannelSelected.solo ?
		//     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
		//     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

		// pChannelSelected.mute ?
		//     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
		//     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);
	}else{
	//####################################
	//#         All other pages
	//####################################
		guiSetEncoderText("-", "-", "-", "-", "-", "-");
	}
}

void X32Ctrl::syncAll(void) {
	if (state->HasAnyChanged()){
		if (
			state->HasChanged(X32_MIXER_CHANGED_PAGE)    ||
			state->HasChanged(X32_MIXER_CHANGED_BANKING) ||
			state->HasChanged(X32_MIXER_CHANGED_SELECT)  ||
			state->HasChanged(X32_MIXER_CHANGED_VCHANNEL) ||
			state->HasChanged(X32_MIXER_CHANGED_GUI)
		   ) {
			guiSync();
			surfaceSync();
		}
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
			// TODO Maybe?: do not sync if just selection has changed
			mixer->SyncVChannelsToHardware();
		}

		state->ResetChangeFlags();
		// TODO Reset all Change Flags of vchannels
	}
}


// sync mixer state to Surface
void X32Ctrl::surfaceSync(void) {
	if ((config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) || (config->GetBankMode() == X32_SURFACE_MODE_BANKING_USER))
	{
		surfaceSyncBoardMain();

		if (config->IsModelX32FullOrCompactOrProducer()){   
			surfaceSyncBoard(X32_BOARD_L);
			if (config->IsModelX32Full()){
				surfaceSyncBoard(X32_BOARD_M);
			}
			surfaceSyncBoard(X32_BOARD_R);
			surfaceSyncBankIndicator();
		}
	}
}

void X32Ctrl::surfaceSyncBoardMain() {
	bool needForSync = false;
	bool fullSync = false;
	VChannel* chan = GetSelectedvChannel();
	uint8_t chanIndex = GetSelectedvChannelIndex();

	if (state->HasChanged(X32_MIXER_CHANGED_SELECT)){ 
		// channel selection has changed - do a full sync
		needForSync=true;
		fullSync=true; 
	}

	if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL) && chan->HasAnyChanged()) {
		// the data in the currently selected channel has changed
		needForSync=true;
	}

	if (needForSync){
		if (config->IsModelX32FullOrCompactOrProducer()){
			// Channel section
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
				surface->SetLedByEnum(X32_BTN_PHANTOM_48V, mixer->halGetPhantomPower(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT)){
				surface->SetLedByEnum(X32_BTN_PHASE_INVERT, mixer->halGetPhaseInvert(chanIndex));
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
				// update gain-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_GAIN) >> 8, surface->Enum2Encoder(X32_ENC_GAIN) & 0xFF, 0, (mixer->halGetGain(chanIndex) + 12.0f)/0.72f, 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
				// update pan-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_PAN) >> 8, surface->Enum2Encoder(X32_ENC_PAN) & 0xFF, 2, (mixer->halGetBalance(chanIndex) + 100.0f)/2.0f, 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)){
				// update pan-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_1) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_1) & 0xFF, 0, pow(10.0f, mixer->halGetBusSend(chanIndex, activeBusSend * 4 + 0)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_2) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_2) & 0xFF, 0, pow(10.0f, mixer->halGetBusSend(chanIndex, activeBusSend * 4 + 1)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_3) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_3) & 0xFF, 0, pow(10.0f, mixer->halGetBusSend(chanIndex, activeBusSend * 4 + 2)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_4) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_4) & 0xFF, 0, pow(10.0f, mixer->halGetBusSend(chanIndex, activeBusSend * 4 + 3)/20.0f) * 100.0f, 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_GATE) >> 8, surface->Enum2Encoder(X32_ENC_GATE) & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].gate.threshold + 80.0f)/0.8f), 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_DYNAMICS) >> 8, surface->Enum2Encoder(X32_ENC_DYNAMICS) & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].compressor.threshold + 60.0f)/0.6f), 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)){
				// update EQ-encoder
				if (chanIndex < 40) {
					surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_LOWCUT) >> 8, surface->Enum2Encoder(X32_ENC_LOWCUT) & 0xFF, 1, (mixer->dsp->Channel[chanIndex].lowCutFrequency - 20.0f)/3.8f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_EQ_FREQ) >> 8, surface->Enum2Encoder(X32_ENC_EQ_FREQ) & 0xFF, 1, (mixer->dsp->Channel[chanIndex].peq[activeEQ].fc - 20.0f)/199.8f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_EQ_GAIN) >> 8, surface->Enum2Encoder(X32_ENC_EQ_GAIN) & 0xFF, 2, (mixer->dsp->Channel[chanIndex].peq[activeEQ].gain + 15.0f)/0.3f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_EQ_Q) >> 8, surface->Enum2Encoder(X32_ENC_EQ_Q) & 0xFF, 3, ((10.0f - mixer->dsp->Channel[chanIndex].peq[activeEQ].Q) + 0.3f)/0.097f, 1);
					switch (mixer->dsp->Channel[chanIndex].peq[activeEQ].type) {
						case 0: // Allpass
							break;
						case 1: // Peak
							break;
						case 2: // LowShelf
							break;
						case 3: // HighShelf
							break;
						case 4: // Bandpass
							break;
						case 5: // Notch
							break;
						case 6: // LowPass
							break;
						case 7: // HighPass
							break;
					}
				}
			}
			
		}

		if (config->IsModelX32Rack()){
			// Channel section
			setLedChannelIndicator();
			chan = GetSelectedvChannel();

			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
				helper->Debug(" Solo");
				surface->SetLedByEnum(X32_BTN_CHANNEL_SOLO, mixer->halGetSolo(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
				helper->Debug(" Mute");
				surface->SetLedByEnum(X32_BTN_CHANNEL_MUTE, mixer->halGetMute(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
				// u_int16_t faderVolume = helper->Dbfs2Fader(halGetVolume(chanIndex));
				// uint8_t pct = (faderVolume/VOLUME_MIN
				// setEncoderRing(X32_BOARD_MAIN, 0, 0, , 1);
			}
		}
	}

	// Clear Solo
	if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)){ surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, mixer->IsSoloActivated()); }
}

void X32Ctrl::surfaceSyncBoard(X32_BOARD p_board) {
	bool fullSync = false;

	if (state->HasChanged(X32_MIXER_CHANGED_BANKING)) {
		fullSync=true;
	}

	uint8_t offset = 0;
	if (config->IsModelX32Full()){
		if (p_board == X32_BOARD_M){ offset=8; }
		if (p_board == X32_BOARD_R){ offset=16; }
	} else if (config->IsModelX32CompactOrProducer()) {
		if (p_board == X32_BOARD_R){ offset=8; }
	}

	// update main-channel

	uint8_t maxChannel = 7;
	if ((p_board == X32_BOARD_R) && config->IsModelX32FullOrCompactOrProducer()) {
		maxChannel = 8; // include main-channel
	}
	for(int i=0; i<=maxChannel; i++){
		uint8_t channelIndex = SurfaceChannel2vChannel(i+offset);

		if (channelIndex == VCHANNEL_NOT_SET) {

			// TODO: do only, wenn channel got unassigned

			surface->SetLed(p_board, 0x20+i, 0);
			surface->SetLed(p_board, 0x30+i, 0);
			surface->SetLed(p_board, 0x40+i, 0);
			surface->SetFader(p_board, i, 0);
			//  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
			surface->SetLcd(p_board,     i, 0,     0,    0,    0,  0x00,  0,  0,          "",  0x00,  0, 0, "");

		} else {
			VChannel* chan = GetVChannel(channelIndex);

			if (fullSync || chan->HasAnyChanged()){
				helper->Debug("syncronize channel%d: %s -", channelIndex, chan->name.c_str());

				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SELECT)){ 
					helper->Debug(" Select");
					surface->SetLed(p_board, 0x20+i, chan->selected);
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
					helper->Debug(" Solo");
					surface->SetLed(p_board, 0x30+i, mixer->halGetSolo(channelIndex)); 
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
					helper->Debug(" Mute");
					surface->SetLed(p_board, 0x40+i, mixer->halGetMute(channelIndex)); 
				}

				if ((fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)) && touchcontrolCanSetFader(p_board, i)){
					helper->Debug(" Fader");
					//u_int16_t faderVolume = helper->Dbfs2Fader(mixer->halGetVolume(channelIndex));
					u_int16_t faderVolume = helper->Dbfs2Fader(mixer->vchannel[channelIndex]->dspChannel->volumeLR);
					surface->SetFader(p_board, i, faderVolume);
				}

				if (
					fullSync                                                         ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT )  ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME )        ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)        ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_COLOR)          ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_NAME)
				   )
				{
					helper->Debug(" LCD");
					surface->SetLcdFromVChannel(p_board, i, chan);

					// char lcdText[20];
					// sprintf(lcdText, "%2.1FdB %s", (double)halGetVolume(chanIndex), (halGetPhantomPower(chanIndex) ? "(48V)" : ""));
					// //  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
					// setLcd(p_board,     i, chan->color,     0,    12,    chan->icon,  0x00,  1,  1,          lcdText,  0x00,  1, 47, chan->name);
				}

				helper->Debug("\n");
			}
		}
	}

	if (p_board == X32_BOARD_R){
		// Clear Solo
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) { surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, mixer->IsSoloActivated()); }
	}
}

void X32Ctrl::surfaceUpdateMeter(void) {
	if (config->IsModelX32Core()) {
		// no led-meters at all
		return;
	}

	// update preamp, dynamics, meterL, meterR, meterSolo
	// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
	// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
	//setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);
	uint8_t chanIdx = GetSelectedvChannelIndex();
	surface->SetMeterLedMain(
		surfaceCalcPreampMeter(chanIdx),
		surfaceCalcDynamicMeter(chanIdx),
		mixer->dsp->MainChannelLR.meterInfo[0],
		mixer->dsp->MainChannelLR.meterInfo[1],
		mixer->dsp->MainChannelSub.meterInfo[0]
	);

	if (config->IsModelX32Rack()) {
		// no led-meters per channel
		return;
	}

	// update channel-meters
	if (config->IsModelX32Full()) {
		// update meters on board L and M
		switch (activeBank_inputFader) {
			case 0: // Input 1-16
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[i].meterInfo);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->Channel[i + 8].meterInfo);
				}
				break;
			case 1: // Input 17-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[16 + i].meterInfo);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->Channel[16 + i + 8].meterInfo);
				}
				break;
			case 2: // Aux 1-8 / FX-Return
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[32 + i].meterInfo);
					//setMeterLed(X32_BOARD_M, i, 0);
				}
				break;
			case 3: // Bus 1-16
				break;
		}
	}else{
		// update meters on board L
		switch (activeBank_inputFader) {
			case 0: // Input 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[i].meterInfo);
				}
				break;
			case 1: // Input 9-16
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[8 + i].meterInfo);
				}
				break;
			case 2: // Input 17-24
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[16 + i].meterInfo);
				}
				break;
			case 3: // Input 25-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[24 + i].meterInfo);
				}
				break;
			case 4: // Aux 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[32 + i].meterInfo);
				}
				break;
			case 5: // FX-Return
				break;
			case 6: // Bus 1-8
				break;
			case 7: // Bus 9-16
				break;
		}
	}

	// update meters on board R
	switch (activeBank_busFader) {
		case 0: // DCA1-8
			// no meter here
			break;
		case 1: // BUS 1-8
			break;
		case 2: // BUS 1-16
			break;
		case 3: // Matrix 1-6, Special, MainSub
			break;
	}
}

void X32Ctrl::surfaceSyncBankIndicator(void) {
	if (state->HasChanged(X32_MIXER_CHANGED_BANKING)) {
		if (config->IsModelX32Full()){
			surface->SetLedByEnum(X32_BTN_CH_1_16, 0);
			surface->SetLedByEnum(X32_BTN_CH_17_32, 0);
			surface->SetLedByEnum(X32_BTN_AUX_IN_EFFECTS, 0);
			surface->SetLedByEnum(X32_BTN_BUS_MASTER, 0);
			if (activeBank_inputFader == 0) { surface->SetLedByEnum(X32_BTN_CH_1_16, 1); }
			if (activeBank_inputFader == 1) { surface->SetLedByEnum(X32_BTN_CH_17_32, 1); }
			if (activeBank_inputFader == 2) { surface->SetLedByEnum(X32_BTN_AUX_IN_EFFECTS, 1); }
			if (activeBank_inputFader == 3) { surface->SetLedByEnum(X32_BTN_BUS_MASTER, 1); }
		}
		if (config->IsModelX32CompactOrProducer()) {
			surface->SetLedByEnum(X32_BTN_CH_1_8, 0);
			surface->SetLedByEnum(X32_BTN_CH_9_16, 0);
			surface->SetLedByEnum(X32_BTN_CH_17_24, 0);
			surface->SetLedByEnum(X32_BTN_CH_25_32, 0);
			surface->SetLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 0);
			surface->SetLedByEnum(X32_BTN_EFFECTS_RETURNS, 0);
			surface->SetLedByEnum(X32_BTN_BUS_1_8_MASTER, 0);
			surface->SetLedByEnum(X32_BTN_BUS_9_16_MASTER, 0);
			if (activeBank_inputFader == 0) { surface->SetLedByEnum(X32_BTN_CH_1_8, 1); }
			if (activeBank_inputFader == 1) { surface->SetLedByEnum(X32_BTN_CH_9_16, 1); }
			if (activeBank_inputFader == 2) { surface->SetLedByEnum(X32_BTN_CH_17_24, 1); }
			if (activeBank_inputFader == 3) { surface->SetLedByEnum(X32_BTN_CH_25_32, 1); }
			if (activeBank_inputFader == 4) { surface->SetLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, 1); }
			if (activeBank_inputFader == 5) { surface->SetLedByEnum(X32_BTN_EFFECTS_RETURNS, 1); }
			if (activeBank_inputFader == 6) { surface->SetLedByEnum(X32_BTN_BUS_1_8_MASTER, 1); }
			if (activeBank_inputFader == 7) { surface->SetLedByEnum(X32_BTN_BUS_9_16_MASTER, 1); }
		}
		surface->SetLedByEnum(X32_BTN_GROUP_DCA_1_8, 0);
		surface->SetLedByEnum(X32_BTN_BUS_1_8, 0);
		surface->SetLedByEnum(X32_BTN_BUS_9_16, 0);
		surface->SetLedByEnum(X32_BTN_MATRIX_MAIN_C, 0);
		if (activeBank_busFader == 0) { surface->SetLedByEnum(X32_BTN_GROUP_DCA_1_8, 1); }
		if (activeBank_busFader == 1) { surface->SetLedByEnum(X32_BTN_BUS_1_8, 1); }
		if (activeBank_busFader == 2) { surface->SetLedByEnum(X32_BTN_BUS_9_16, 1); }
		if (activeBank_busFader == 3) { surface->SetLedByEnum(X32_BTN_MATRIX_MAIN_C, 1); }
	}
}

// only X32 Rack and X32 Core
void X32Ctrl::setLedChannelIndicator(void){
	if (config->IsModelX32Core() || config->IsModelX32Rack()){
		// Turn off all LEDS
		surface->SetLedByEnum(X32_LED_IN, 0);
		surface->SetLedByEnum(X32_LED_AUX, 0);
		surface->SetLedByEnum(X32_LED_BUS, 0);
		surface->SetLedByEnum(X32_LED_DCA, 0);
		surface->SetLedByEnum(X32_LED_MAIN, 0);
		surface->SetLedByEnum(X32_LED_MATRIX, 0);

		uint8_t chanIdx = GetSelectedvChannelIndex();

		if ((chanIdx >= 0)&&(chanIdx <= 31)){
			surface->SetLedByEnum(X32_LED_IN, 1);
		}
		if ((chanIdx >= 32)&&(chanIdx <= 47)){
			surface->SetLedByEnum(X32_LED_AUX, 1);
		}
		if ((chanIdx >= 48)&&(chanIdx <= 63)){
			surface->SetLedByEnum(X32_LED_BUS, 1);
		}
		if ((chanIdx >= 64)&&(chanIdx <= 69)){
			surface->SetLedByEnum(X32_LED_MATRIX, 1);
		}
		if ((chanIdx >= 70)&&(chanIdx <= 71)){
			surface->SetLedByEnum(X32_LED_MAIN, 1);
		}
		if ((chanIdx >= 72)&&(chanIdx <= 79)){
			surface->SetLedByEnum(X32_LED_DCA, 1);
		}

		// set 7-Segment Display
		surface->SetX32RackDisplay(chanIdx);        
	}
}

uint8_t X32Ctrl::surfaceCalcPreampMeter(uint8_t channel) {
	if (channel >= 40) {
		return 0; // no preamps outside the 40 dsp-channels
	}

	float audiodata = mixer->dsp->Channel[channel].meterPu*2147483648.0f;
	uint32_t meterdata = 0;
	if (audiodata >= vuThresholds[0])  { meterdata |= 0b10000000; } // CLIP
	if (audiodata >= vuThresholds[3])  { meterdata |= 0b01000000; } // -3dBfs
	if (audiodata >= vuThresholds[5])  { meterdata |= 0b00100000; } // -6dBfs
	if (audiodata >= vuThresholds[7])  { meterdata |= 0b00010000; } // -9dBfs
	if (audiodata >= vuThresholds[8])  { meterdata |= 0b00001000; } // -12dBfs
	if (audiodata >= vuThresholds[10]) { meterdata |= 0b00000100; } // -18dBfs
	if (audiodata >= vuThresholds[14]) { meterdata |= 0b00000010; } // -30dBfs
	if (audiodata >= vuThresholds[24]) { meterdata |= 0b00000001; } // SIG

	return meterdata;
}

uint8_t X32Ctrl::surfaceCalcDynamicMeter(uint8_t channel) {
	// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
	if (channel < 40) {
		uint32_t meterdata = 0;

		if (mixer->dsp->Channel[channel].compressor.gain < 1.0f) { meterdata |= 0b10000000; };

		float gateValue = (1.0f - mixer->dsp->Channel[channel].gate.gain) * 80.0f;
		if (gateValue >= 2.0f)  { meterdata |= 0b00100000; }        
		if (gateValue >= 4.0f)  { meterdata |= 0b00010000; }        
		if (gateValue >= 6.0f)  { meterdata |= 0b00001000; }        
		if (gateValue >= 10.0f) { meterdata |= 0b00000100; }        
		if (gateValue >= 18.0f) { meterdata |= 0b00000010; }        
		if (gateValue >= 30.0f) { meterdata |= 0b00000001; }        

		if (mixer->dsp->Channel[channel].gate.gain < 1.0f) { meterdata |= 0b01000000; };

		return meterdata;
	}else{
		return 0; // no dynamic-data for other channels at the moment
	}
}

// ####################################################################
// #
// #
// #        channel Assigment
// #
// #
// ####################################################################

// direction - positive or negative integer value
void X32Ctrl::ChangeSelect(uint8_t direction){
	int16_t newSelectedVChannel = selectedVChannel += direction;
	if (newSelectedVChannel < 0) {
		newSelectedVChannel = 0;
	} else if (newSelectedVChannel >= MAX_VCHANNELS){
		newSelectedVChannel = MAX_VCHANNELS - 1;
	}

	SetSelect(newSelectedVChannel, true);
}

void X32Ctrl::SetSelect(uint8_t vChannelIndex, bool select){
	for(uint8_t i=0; i<MAX_VCHANNELS;i++){
		mixer->vchannel[i]->selected = false;
		mixer->vchannel[i]->SetChanged(X32_VCHANNEL_CHANGED_SELECT);
	}
	mixer->vchannel[vChannelIndex]->selected = select;
	mixer->vchannel[vChannelIndex]->SetChanged(X32_VCHANNEL_CHANGED_SELECT);

	selectedVChannel = vChannelIndex;
	state->SetChangeFlags(X32_MIXER_CHANGED_SELECT);
}

void X32Ctrl::ToggleSelect(uint8_t vChannelIndex){
	SetSelect(vChannelIndex, !mixer->vchannel[vChannelIndex]->selected);
}

uint8_t X32Ctrl::GetSelectedvChannelIndex(void){
	return selectedVChannel;
}

VChannel* X32Ctrl::GetSelectedvChannel(void){
	return mixer->vchannel[GetSelectedvChannelIndex()];
}

VChannel* X32Ctrl::GetVChannel(uint8_t VChannelIndex){
	return mixer->vchannel[VChannelIndex];
}

uint8_t X32Ctrl::SurfaceChannel2vChannel(uint8_t surfaceChannel){
	if ((config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) || (config->GetBankMode() == X32_SURFACE_MODE_BANKING_USER)){
		if (config->IsModelX32Full()){
			if (surfaceChannel <= 15){
				// input-section
				return modes[config->GetBankMode()].inputBanks[activeBank_inputFader].surfaceChannel2VChannel[surfaceChannel];
			} else if (surfaceChannel == 24) {
				// main-channel
				return 80;
			} else {
				// bus-section and mainfader
				return modes[config->GetBankMode()].busBanks[activeBank_busFader].surfaceChannel2VChannel[surfaceChannel-16];
			}
		}
		if (config->IsModelX32CompactOrProducer()){
			if (surfaceChannel <= 7){
				// input-section
				return modes[config->GetBankMode()].inputBanks[activeBank_inputFader].surfaceChannel2VChannel[surfaceChannel];
			} else if (surfaceChannel == 16) {
				// main-channel
				return 80;
			} else {
				// bus-section and mainfader
				return modes[config->GetBankMode()].busBanks[activeBank_busFader].surfaceChannel2VChannel[surfaceChannel-8];
			}
			return 0;
		}
		if (config->IsModelX32Core()){
			// TODO
		}
	}

	return 0;
}

uint8_t X32Ctrl::GetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex) {
	uint8_t offset = 0;
	if (p_board == X32_BOARD_M) { offset=8; }
	if (config->IsModelX32Full()){
		if (p_board == X32_BOARD_R) { offset=16; }
	}
	if (config->IsModelX32CompactOrProducer()){
		if (p_board == X32_BOARD_R) { offset=8; }
	}
	return SurfaceChannel2vChannel(p_buttonIndex + offset);
}

void X32Ctrl::FaderMoved(SurfaceEvent* event){
	uint8_t vchannelIndex = VCHANNEL_NOT_SET;

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		uint8_t offset = 0;
		if (event->boardId == X32_BOARD_M) { 
			offset=8;
		}
		if (config->IsModelX32Full()){
			if (event->boardId == X32_BOARD_R) { 
				offset=16;
			}
		}
		if (config->IsModelX32CompactOrProducer()){
			if (event->boardId == X32_BOARD_R) { 
				offset=8;
			}
		}

		vchannelIndex = SurfaceChannel2vChannel(event->index + offset);
		mixer->SetVolume(vchannelIndex, helper->Fader2dBfs(event->value));

		touchcontrol.board = event->boardId;
		touchcontrol.faderIndex = event->index;
		touchcontrol.value = 5;

		helper->Debug("FaderMoved(%s): vChannel%d TouchControl=%d\n", event->ToString().c_str(), vchannelIndex, touchcontrol.value);
	}
}

void X32Ctrl::ButtonPressed(SurfaceEvent* event) {
	X32_BTN button = surface->Button2Enum(((uint16_t)event->boardId << 8) + (uint16_t)(event->value & 0x7F));
	bool buttonPressed = (event->value >> 7) == 1;

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		if (buttonPressed){
			switch (button) {
				case X32_BTN_LEFT:
					ShowPrevPage();
					break;
				case X32_BTN_RIGHT:
					ShowNextPage();
					break;
				case X32_BTN_HOME:
					ShowPage(X32_PAGE_HOME);
					break;
				case X32_BTN_VIEW_CONFIG:
					ShowPage(X32_PAGE_CONFIG);
					break;
				case X32_BTN_VIEW_EQ:
					ShowPage(X32_PAGE_EQ);
					break;
				case X32_BTN_METERS:
					ShowPage(X32_PAGE_METERS);
					break;
				case X32_BTN_ROUTING:
					ShowPage(X32_PAGE_ROUTING);
					break;
				case X32_BTN_SETUP:
					ShowPage(X32_PAGE_SETUP);
					break;
				case X32_BTN_LIBRARY:
					ShowPage(X32_PAGE_LIBRARY);
					break;
				case X32_BTN_EFFECTS:
					ShowPage(X32_PAGE_EFFECTS);
					break;
				case X32_BTN_MUTE_GRP:
					ShowPage(X32_PAGE_MUTE_GRP);
					break;
				case X32_BTN_UTILITY:
					ShowPage(X32_PAGE_UTILITY);
					break;
				case X32_BTN_EQ_LOW:
				case X32_BTN_EQ_LOW_MID:
				case X32_BTN_EQ_HIGH_MID:
				case X32_BTN_EQ_HIGH:
					BankingEQ(button);
					break;
				case X32_BTN_BUS_SEND_1_4:
				case X32_BTN_BUS_SEND_5_8:
				case X32_BTN_BUS_SEND_9_12:
				case X32_BTN_BUS_SEND_13_16:
					BankingSends(button);
					break;
				case X32_BTN_CH_1_16:
				case X32_BTN_CH_17_32:
				case X32_BTN_AUX_IN_EFFECTS:
				case X32_BTN_BUS_MASTER:
				case X32_BTN_EFFECTS_RETURNS:
				case X32_BTN_CH_1_8:
				case X32_BTN_CH_9_16:
				case X32_BTN_CH_17_24:
				case X32_BTN_CH_25_32:
				case X32_BTN_AUX_IN_1_6_USB_REC:
				case X32_BTN_BUS_1_8_MASTER:
				case X32_BTN_BUS_9_16_MASTER:
				case X32_BTN_GROUP_DCA_1_8:
				case X32_BTN_BUS_1_8:
				case X32_BTN_BUS_9_16:
				case X32_BTN_MATRIX_MAIN_C:
					Banking(button);
					break;
				case X32_BTN_BOARD_L_CH_1_SELECT:
				case X32_BTN_BOARD_L_CH_2_SELECT:
				case X32_BTN_BOARD_L_CH_3_SELECT:
				case X32_BTN_BOARD_L_CH_4_SELECT:
				case X32_BTN_BOARD_L_CH_5_SELECT:
				case X32_BTN_BOARD_L_CH_6_SELECT:
				case X32_BTN_BOARD_L_CH_7_SELECT:
				case X32_BTN_BOARD_L_CH_8_SELECT:
					ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_SELECT));
					break;
				case X32_BTN_BOARD_M_CH_1_SELECT:
				case X32_BTN_BOARD_M_CH_2_SELECT:
				case X32_BTN_BOARD_M_CH_3_SELECT:
				case X32_BTN_BOARD_M_CH_4_SELECT:
				case X32_BTN_BOARD_M_CH_5_SELECT:
				case X32_BTN_BOARD_M_CH_6_SELECT:
				case X32_BTN_BOARD_M_CH_7_SELECT:
				case X32_BTN_BOARD_M_CH_8_SELECT:
					ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_SELECT));
					break;
				case X32_BTN_BOARD_R_CH_1_SELECT:
				case X32_BTN_BOARD_R_CH_2_SELECT:
				case X32_BTN_BOARD_R_CH_3_SELECT:
				case X32_BTN_BOARD_R_CH_4_SELECT:
				case X32_BTN_BOARD_R_CH_5_SELECT:
				case X32_BTN_BOARD_R_CH_6_SELECT:
				case X32_BTN_BOARD_R_CH_7_SELECT:
				case X32_BTN_BOARD_R_CH_8_SELECT:
				case X32_BTN_MAIN_SELECT:
					ToggleSelect(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_SELECT));
					break;
				case X32_BTN_BOARD_L_CH_1_SOLO:
				case X32_BTN_BOARD_L_CH_2_SOLO:
				case X32_BTN_BOARD_L_CH_3_SOLO:
				case X32_BTN_BOARD_L_CH_4_SOLO:
				case X32_BTN_BOARD_L_CH_5_SOLO:
				case X32_BTN_BOARD_L_CH_6_SOLO:
				case X32_BTN_BOARD_L_CH_7_SOLO:
				case X32_BTN_BOARD_L_CH_8_SOLO:
					mixer->ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_SOLO));
					break;
				case X32_BTN_BOARD_M_CH_1_SOLO:
				case X32_BTN_BOARD_M_CH_2_SOLO:
				case X32_BTN_BOARD_M_CH_3_SOLO:
				case X32_BTN_BOARD_M_CH_4_SOLO:
				case X32_BTN_BOARD_M_CH_5_SOLO:
				case X32_BTN_BOARD_M_CH_6_SOLO:
				case X32_BTN_BOARD_M_CH_7_SOLO:
				case X32_BTN_BOARD_M_CH_8_SOLO:
					mixer->ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_SOLO));
					break;
				case X32_BTN_BOARD_R_CH_1_SOLO:
				case X32_BTN_BOARD_R_CH_2_SOLO:
				case X32_BTN_BOARD_R_CH_3_SOLO:
				case X32_BTN_BOARD_R_CH_4_SOLO:
				case X32_BTN_BOARD_R_CH_5_SOLO:
				case X32_BTN_BOARD_R_CH_6_SOLO:
				case X32_BTN_BOARD_R_CH_7_SOLO:
				case X32_BTN_BOARD_R_CH_8_SOLO:
				case X32_BTN_MAIN_SOLO:
					mixer->ToggleSolo(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_SOLO));
					break;
				case X32_BTN_BOARD_L_CH_1_MUTE:
				case X32_BTN_BOARD_L_CH_2_MUTE:
				case X32_BTN_BOARD_L_CH_3_MUTE:
				case X32_BTN_BOARD_L_CH_4_MUTE:
				case X32_BTN_BOARD_L_CH_5_MUTE:
				case X32_BTN_BOARD_L_CH_6_MUTE:
				case X32_BTN_BOARD_L_CH_7_MUTE:
				case X32_BTN_BOARD_L_CH_8_MUTE:
					mixer->ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_MUTE));
					break;
				case X32_BTN_BOARD_M_CH_1_MUTE:
				case X32_BTN_BOARD_M_CH_2_MUTE:
				case X32_BTN_BOARD_M_CH_3_MUTE:
				case X32_BTN_BOARD_M_CH_4_MUTE:
				case X32_BTN_BOARD_M_CH_5_MUTE:
				case X32_BTN_BOARD_M_CH_6_MUTE:
				case X32_BTN_BOARD_M_CH_7_MUTE:
				case X32_BTN_BOARD_M_CH_8_MUTE:
					mixer->ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_MUTE));
					break;
				case X32_BTN_BOARD_R_CH_1_MUTE:
				case X32_BTN_BOARD_R_CH_2_MUTE:
				case X32_BTN_BOARD_R_CH_3_MUTE:
				case X32_BTN_BOARD_R_CH_4_MUTE:
				case X32_BTN_BOARD_R_CH_5_MUTE:
				case X32_BTN_BOARD_R_CH_6_MUTE:
				case X32_BTN_BOARD_R_CH_7_MUTE:
				case X32_BTN_BOARD_R_CH_8_MUTE:
				case X32_BTN_MAIN_MUTE:
					mixer->ToggleMute(GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_MUTE));
					break;
				case X32_BTN_CLEAR_SOLO:
					mixer->ClearSolo();
					break;
				case X32_BTN_PHANTOM_48V:
					mixer->TogglePhantom(GetSelectedvChannelIndex());
					break;
				case X32_BTN_PHASE_INVERT:
					mixer->TogglePhaseInvert(GetSelectedvChannelIndex());
					break;
				case X32_BTN_CHANNEL_SOLO: // only X32 Rack
					mixer->ToggleSolo(GetSelectedvChannelIndex());
					break;
				case X32_BTN_CHANNEL_MUTE: // only X32 Rack
					mixer->ToggleMute(GetSelectedvChannelIndex());
					break;
				default:
					// TODO: Callback to x32ctrl if needed
					helper->Debug("Unhandled button detected.\n");
					break;
			}
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()){
		if (activePage == X32_PAGE_CONFIG){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						break;
					case X32_BTN_ENCODER2:
						mixer->TogglePhantom(GetSelectedvChannelIndex());
						break;
					case X32_BTN_ENCODER3:
						break;
					case X32_BTN_ENCODER4:
						break;
					case X32_BTN_ENCODER5:
						break;
					case X32_BTN_ENCODER6:
						break;
					default:
						break;
				}
			}
		}else if (activePage == X32_PAGE_EQ){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->ChangeLowcut(GetSelectedvChannelIndex(), -10000); // will be limited to 20 Hz
						break;
					case X32_BTN_ENCODER2:
						mixer->SetPeq(GetSelectedvChannelIndex(), activeEQ, 'F', 3000);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetPeq(GetSelectedvChannelIndex(), activeEQ, 'G', 0);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetPeq(GetSelectedvChannelIndex(), activeEQ, 'Q', 2);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetPeq(GetSelectedvChannelIndex(), activeEQ, 'T', 1);
						break;
					case X32_BTN_ENCODER6:
						mixer->dsp->ResetEq(GetSelectedvChannelIndex());
						break;
					default:
						break;
				}
			}
		}
	}
}

void X32Ctrl::EncoderTurned(SurfaceEvent* event) {
	X32_ENC encoder = surface->Encoder2Enum(((uint16_t)event->boardId << 8) + (uint16_t)(event->index));
	int8_t amount = 0;
	int8_t newValue;

	if (event->value > 0 && event->value < 128){
		amount = (uint8_t)event->value;
	} else if (event->value > 128 && event->value < 256) {
		amount = -(256 - (uint8_t)(event->value));
	}
	helper->Debug("Encoder: %d\n", amount);

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		switch (encoder){
			case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
				ChangeSelect(amount);
				break;
			case X32_ENC_CHANNEL_LEVEL:
				mixer->ChangeVolume(GetSelectedvChannelIndex(), amount);
				break;
			case X32_BTN_EQ_MODE:
				mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', amount);
				break;
			case X32_ENC_GAIN:
				mixer->ChangeGain(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_GATE:
				mixer->ChangeGate(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_LOWCUT:
				mixer->ChangeLowcut(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_DYNAMICS:
				mixer->ChangeDynamics(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_EQ_Q:
				mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'Q', amount);
				break;
			case X32_ENC_EQ_FREQ:
				mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'F', amount);
				break;
			case X32_ENC_EQ_GAIN:
				mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'G', amount);
				break;
			case X32_ENC_PAN:
				mixer->ChangeBalance(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_BUS_SEND_1:
				mixer->ChangeBusSend(GetSelectedvChannelIndex(), 0, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_2:
				mixer->ChangeBusSend(GetSelectedvChannelIndex(), 1, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_3:
				mixer->ChangeBusSend(GetSelectedvChannelIndex(), 2, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_4:
				mixer->ChangeBusSend(GetSelectedvChannelIndex(), 3, amount, activeBusSend);
				break;
			default:
				// TODO: Callback to x32ctrl if needed
				helper->Debug("Unhandled encoder detected.\n");
				break;
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()) {
		if (activePage == X32_PAGE_CONFIG){
			switch (encoder){
				case X32_ENC_ENCODER1:
					// TODO ?
					//ChangeDspInput(amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeGain(GetSelectedvChannelIndex(), amount);
					break;
				case X32_ENC_ENCODER3:                   
					break;
				case X32_ENC_ENCODER4:
					break;
				case X32_ENC_ENCODER5:
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
		}else if (activePage == X32_PAGE_EQ) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeLowcut(GetSelectedvChannelIndex(), amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'F', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'G', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'Q', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', amount);
					break;
				case X32_ENC_ENCODER6:
					newValue = activeEQ + amount;
					if (newValue < 0) {
						activeEQ = 0;
					}else if(newValue >= MAX_CHAN_EQS) {
						activeEQ = MAX_CHAN_EQS - 1;
					}else{
						activeEQ = newValue;
					}
					mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', 0);
					break;
				default:
					break;
			}
		}else if (activePage == X32_PAGE_ROUTING) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeHardwareOutput(amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeHardwareInput(amount);
					break;
				case X32_ENC_ENCODER3:
					break;
				case X32_ENC_ENCODER4:
					break;
				case X32_ENC_ENCODER5:
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
		}
	}
}

void X32Ctrl::BankingSends(X32_BTN p_button) {
	surface->SetLedByEnum(X32_BTN_BUS_SEND_1_4, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_5_8, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_9_12, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_13_16, 0);

	switch (p_button){
		case X32_BTN_BUS_SEND_1_4:
			activeBusSend = 0;
			surface->SetLedByEnum(X32_BTN_BUS_SEND_1_4, 1);
			break;
		case X32_BTN_BUS_SEND_5_8:
			activeBusSend = 1;
			surface->SetLedByEnum(X32_BTN_BUS_SEND_5_8, 1);
			break;
		case X32_BTN_BUS_SEND_9_12:
			activeBusSend = 2;
			surface->SetLedByEnum(X32_BTN_BUS_SEND_9_12, 1);
			break;
		case X32_BTN_BUS_SEND_13_16:
			activeBusSend = 3;
			surface->SetLedByEnum(X32_BTN_BUS_SEND_13_16, 1);
			break;
		default:
			break;
	}

	state->SetChangeFlags(X32_VCHANNEL_CHANGED_SENDS);
}

void X32Ctrl::BankingEQ(X32_BTN p_button){
	surface->SetLedByEnum(X32_BTN_EQ_LOW, 0);
	surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, 0);
	surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, 0);
	surface->SetLedByEnum(X32_BTN_EQ_HIGH, 0);

	switch (p_button){
		case X32_BTN_EQ_LOW:
			activeEQ = 0;
			surface->SetLedByEnum(X32_BTN_EQ_LOW, 1);
			break;
		case X32_BTN_EQ_LOW_MID:
			activeEQ = 1;
			surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, 1);
			break;
		case X32_BTN_EQ_HIGH_MID:
			activeEQ = 2;
			surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
			break;
		case X32_BTN_EQ_HIGH:
			activeEQ = 3;
			surface->SetLedByEnum(X32_BTN_EQ_HIGH, 1);
			break;
		// TODO: implement LOW2 and HIGH2
		default:
			break;
	}

	state->SetChangeFlags(X32_VCHANNEL_CHANGED_EQ);
	state->SetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void X32Ctrl::Banking(X32_BTN p_button){
	if (config->IsModelX32Full()){
		switch (p_button){
			case X32_BTN_CH_1_16:
				activeBank_inputFader = 0;
				break;
			case X32_BTN_CH_17_32:
				activeBank_inputFader = 1;
				break;
			case X32_BTN_AUX_IN_EFFECTS:
				activeBank_inputFader = 2;
				break;
			case X32_BTN_BUS_MASTER:
				activeBank_inputFader = 3;
				break;
			case X32_BTN_GROUP_DCA_1_8:
				activeBank_busFader = 0;
				break;
			case X32_BTN_BUS_1_8:
				activeBank_busFader = 1;
				break;
			case X32_BTN_BUS_9_16:
				activeBank_busFader = 2;
				break;
			case X32_BTN_MATRIX_MAIN_C:
				activeBank_busFader = 3;
				break;
			default:
				break;
		}
	}
	if (config->IsModelX32CompactOrProducer()){
		switch (p_button){
			case X32_BTN_CH_1_8:
				activeBank_inputFader = 0;
				break;
			case X32_BTN_CH_9_16:
				activeBank_inputFader = 1;
				break;
			case X32_BTN_CH_17_24:
				activeBank_inputFader = 2;
				break;
			case X32_BTN_CH_25_32:
				activeBank_inputFader = 3;
				break;
			case X32_BTN_AUX_IN_1_6_USB_REC:
				activeBank_inputFader = 4;
				break;
			case X32_BTN_EFFECTS_RETURNS:
				activeBank_inputFader = 5;
				break;
			case X32_BTN_BUS_1_8_MASTER:
				activeBank_inputFader = 6;
				break;
			case X32_BTN_BUS_9_16_MASTER:
				activeBank_inputFader = 7;
				break;
			case X32_BTN_GROUP_DCA_1_8:
				activeBank_busFader = 0;
				break;
			case X32_BTN_BUS_1_8:
				activeBank_busFader = 1;
				break;
			case X32_BTN_BUS_9_16:
				activeBank_busFader = 2;
				break;
			case X32_BTN_MATRIX_MAIN_C:
				activeBank_busFader = 3;
				break;
			default:
				break;
		}
	}

	state->SetChangeFlags(X32_MIXER_CHANGED_BANKING);
}

// ####################################################################
// #
// #
// #        Touchcontrol
// #
// #
// ####################################################################

void X32Ctrl::touchcontrolTick(void){
	if (touchcontrol.value > 0) {
		touchcontrol.value--;
		if (touchcontrol.value == 0)
		{
			// trigger sync for this channel
			mixer->SetVChannelChangeFlagsFromIndex(
				GetvChannelIndexFromButtonOrFaderIndex(touchcontrol.board, touchcontrol.faderIndex),
				X32_VCHANNEL_CHANGED_VOLUME
			);
		}
		helper->Debug("TouchControl=%d\n", touchcontrol.value);
	}
}

bool X32Ctrl::touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
	if ((touchcontrol.board != p_board) && (touchcontrol.faderIndex != p_faderIndex)){
		return true;
	} 

	if (touchcontrol.value == 0){
		return true;
	}

	return false;
}

// ####################################################################
// #
// #
// #        Debug
// #
// #
// ###################################################################

void X32Ctrl::DebugPrintBank(uint8_t bank)
{
	helper->Debug("################# BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		helper->Debug("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i]);
	}
	helper->Debug("END ############# BANK%d ############### END\n", bank);
}

void X32Ctrl::DebugPrintBusBank(uint8_t bank)
{
	helper->Debug("################# BUS BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		helper->Debug("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2VChannel[i]);
	}
	helper->Debug("END ############# BUS BANK%d ############### END\n", bank);
}

void X32Ctrl::DebugPrintvChannels(void){
	for (int i = 0; i < MAX_VCHANNELS; i++)
	{
	   helper->Debug("vChannel%-3d %-32s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1f input=dspCh%d\n",
		   i,
	   mixer->vchannel[i]->name.c_str(),
	   mixer->vchannel[i]->color,
	   mixer->vchannel[i]->icon,
	   mixer->vchannel[i]->selected,
	   mixer->vchannel[i]->dspChannel->solo,
	   mixer->vchannel[i]->dspChannel->muted,
	   (double)mixer->vchannel[i]->dspChannel->volumeLR,
	   mixer->vchannel[i]->dspChannel->inputSource);
	}
}
