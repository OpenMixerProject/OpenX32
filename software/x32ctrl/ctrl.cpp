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

#include "ctrl.h"

#define X32_CTRL_VERSION "v0.2.5"
#define X32_CTRL_URL "https://github.com/OpenMixerProject/OpenX32"
#define X32_CTRL_CONFIGFILE "x32ctrl.cfg"


// these need to be globaly defined here, to be used by various functions
X32Ctrl* ctrl;
Config* config = new Config();
State* state = new State();
CLI::App* app = new CLI::App();
vector<string> debug_parameters;

// handle STRG-C and write config file
void my_handler(int s){
    printf("Caught signal %d\n",s);

	// write config to file
	if (app){
		ofstream myfile;
		myfile.open(X32_CTRL_CONFIGFILE);
		myfile << app->config_to_str(true,true);
		myfile.close();
	}
    exit(0); 
}

int main(int argc, char* argv[]) {
	srand(time(NULL));

	struct sigaction sigIntHandler;
	sigIntHandler.sa_handler = my_handler;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;
	sigaction(SIGINT, &sigIntHandler, NULL);

	Helper* helper = new Helper();

	app->description("OpenX32 Main Control");
	argv = app->ensure_utf8(argv);


	// Command line options

	app->set_config("--config", X32_CTRL_CONFIGFILE)->configurable(false)->description("Load and Store configuration from/to alternate file");;
	app->add_flag("--version", "Get the version number, builddate and a nice logo")->configurable(false);
	app->add_flag("-p,--print", "Print configuration and exit")->configurable(false);

	app->add_option("--L", "Bitstream file for the Lattice FPGA")
		->option_text("FILE")
		//->default_str("lattice.bit")
		->configurable(false);
	app->add_option("--X", "Bitstream file for the Xilinx FPGA")
		->option_text("FILE")
		//->default_str("xilinx.bit")
		->configurable(false);
	app->add_option("--D1", "Bitstream file for the DSP1")
		->option_text("FILE")
		//->default_str("dsp1.ldr")
		->configurable(false);
	app->add_option("--D2", "Bitstream file for the DSP2")
		->option_text("FILE")
		//->default_str("dsp2.ldr")
		->configurable(false);

	app->add_option("--samplerate", "Set Samplerate to 44100 or 48000 kHz")
		->default_val<uint32_t>(48000)
		->check(CLI::IsMember(new set<uint32_t>{41000, 48000}));

	// DEBUG
	app->add_option("-d,--debug", debug_parameters, "Prints debugging information to stdout. You can specify one or multiple of the following Flags: ADDA DSP1 DSP2 FPGA GUI MIXER SPI SURFACE UART VCHANNEL X32CTRL XREMOTE")
			->configurable(false)
			->expected(1,-1)
			->option_text("FLAG FLAG ...");
	
	app->get_config_formatter_base()->quoteCharacter('"', '"');

	CLI11_PARSE(*app, argc, argv);

	if(app->get_option("--print")->as<bool>()) {
        std::cout << app->config_to_str(true, false);
        return 0;
    }

	if (app->count("--version") > 0) {
		printf("  ____                  __   ______ ___ \n");
		printf(" / __ \\                 \\ \\ / /___ \\__ \\ \n");
		printf("| |  | |_ __   ___ _ __  \\ V /  __) | ) |\n");
		printf("| |  | | '_ \\ / _ \\ '_ \\  > <  |__ < / / \n");
		printf("| |__| | |_) |  __/ | | |/ . \\ ___) / /_ \n");
		printf(" \\____/| .__/ \\___|_| |_/_/ \\_\\____/____|\n");
		printf("       | |                               \n");
		printf("       |_|                               \n");
		printf("\n");
		printf("%s build on %s %s\n\n%s\n", X32_CTRL_VERSION, __DATE__, __TIME__, X32_CTRL_URL);
		return 0;
	}

	config->SetSamplerate(app->get_option("--samplerate")->as<uint32_t>());

	for(uint8_t i=0; i<debug_parameters.size(); i++) {
		if (debug_parameters[i] == "ADDA") { helper->DEBUG_ADDA(true); }
		if (debug_parameters[i] == "DSP1") { helper->DEBUG_DSP1(true); }
		if (debug_parameters[i] == "DSP2") { helper->DEBUG_DSP2(true); }
		if (debug_parameters[i] == "FPGA") { helper->DEBUG_FPGA(true); }
		if (debug_parameters[i] == "GUI") { helper->DEBUG_GUI(true); }
		if (debug_parameters[i] == "MIXER") { helper->DEBUG_MIXER(true); }
		if (debug_parameters[i] == "SPI") { helper->DEBUG_SPI(true); }
		if (debug_parameters[i] == "SURFACE") { helper->DEBUG_SURFACE(true); }
		if (debug_parameters[i] == "UART") { helper->DEBUG_UART(true); }
		if (debug_parameters[i] == "VCHANNEL") { helper->DEBUG_VCHANNEL(true); }
		if (debug_parameters[i] == "X32CTRL") { helper->DEBUG_X32CTRL(true); }
		if (debug_parameters[i] == "XREMOTE") { helper->DEBUG_XREMOTE(true); }	
		
		if (debug_parameters[i] == "X32SERVER") {  
			X32 x;
			x.x32_startup();
			exit(0);
		}
	}

	X32BaseParameter* basepar = new X32BaseParameter(app, config, state, helper);
	ctrl = new X32Ctrl(basepar);
	ctrl->Run();

    exit(0);
}


// ###########################################################################
// #
// #      ###### #  #    # ##### #####
// #        #    #  ##  ## #     #   #      
// #        #    #  # ## # ##### ####       
// #        #    #  #    # #     #   #      
// #        #    #  #    # ##### #    #      
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
void init100msTimer(void) {
  // Set up the signal handler
  struct sigaction sa;
  sa.sa_handler = timer100msCallbackLinux;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  if (sigaction(SIGRTMIN, &sa, NULL) == -1) {
	perror("sigaction");
	//return 1;
  }

  // Set up the sigevent structure for the timer
  sev.sigev_notify = SIGEV_SIGNAL;
  sev.sigev_signo = SIGRTMIN;
  sev.sigev_value.sival_ptr = &timerid;

  // Create the timer
  if (timer_create(CLOCK_REALTIME, &sev, &timerid) == -1) {
	perror("timer_create");
	//return 1;
  }
  
  // Set the timer to trigger every 1 second (1,000,000,000 nanoseconds)
  trigger.it_value.tv_sec = 0;
  trigger.it_value.tv_nsec = 50000000; // 50ms = 50000us = 50000000ns
  trigger.it_interval.tv_sec = 0;
  trigger.it_interval.tv_nsec = 50000000;

  // Arm the timer
  if (timer_settime(timerid, 0, &trigger, NULL) == -1) {
	perror("timer_settime");
	//return 1;
  }
}

void X32Ctrl::Tick10ms(void){
	surface->ProcessUartData();
	mixer->Tick10ms();

	ProcessEvents();

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

	mixer->Tick100ms();

	if (!config->IsModelX32Core()) {
		// read the current DSP load
		lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% [v%.2f]", (double)state->dspLoad[0], (double)state->dspVersion[0], (double)state->dspLoad[1], (double)state->dspVersion[1]); // show the received value (could be a bit older than the request)
	}
}

// ###########################################################################
// #
// #      X32Ctrl Class
// #
// ###########################################################################

X32Ctrl::X32Ctrl(X32BaseParameter* basepar) : X32Base(basepar) {
	mixer = new Mixer(basepar);
	surface = new Surface(basepar);
	xremote = new XRemote(basepar);
}

void X32Ctrl::Run(){

	// first try to find what we are: Fullsize, Compact, Producer, Rack or Core
	helper->DEBUG_X32CTRL("Reading config...");

	char model[12];
	char serial[15];
	char date[16];
	helper->ReadConfig("/etc/x32.conf", "MDL=", model, 12);
	helper->ReadConfig("/etc/x32.conf", "SN=", serial, 15);
	helper->ReadConfig("/etc/x32.conf", "DATE=", date, 16);
	helper->Log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

	config->SetModel(model);
	config->SetBankMode(X32_SURFACE_MODE_BANKING_X32);
	
	surface->Init();
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
				helper->DEBUG_X32CTRL("Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 16));
			}
		}
	}

	if (config->IsModelX32CompactOrProducer()){
		// bank is 8 channels wide
		for (uint8_t bank=0;bank<8;bank++){
			for (int i = 0; i <=7; i++) {
				modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 8);
				helper->DEBUG_X32CTRL("Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 8));
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
	helper->DEBUG_X32CTRL("Event: %s\n", (event->ToString()).c_str());

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
		helper->DEBUG_X32CTRL("unknown message: %s\n", event->ToString().c_str());
	}

	delete(event);
  }
}

// receive data from XRemote client
void X32Ctrl::UdpHandleCommunication(void) {
    char rxData[500];
    int bytes_available = 0;
    uint8_t channel;
    data_32b value32bit;
    
    // check for bytes in UDP-buffer
    //int result = ioctl(xremote->UdpHandle, FIONREAD, &bytes_available);
    if (bytes_available > 0) {
        socklen_t xremoteClientAddrLen = sizeof(xremote->ClientAddr);
        uint8_t len = recvfrom(xremote->UdpHandle, rxData, bytes_available, MSG_WAITALL, (struct sockaddr *) &xremote->ClientAddr, &xremoteClientAddrLen);

		tosc_message osc;

		if (!tosc_parseMessage(&osc, rxData, len)) {
			string adrPath = string(tosc_getAddress(&osc));
    		vector<string> address = helper->split(adrPath, "/");
			address.erase(address.begin()); // delete empty element
			string format = string(tosc_getFormat(&osc));

			// TODO
			// DEBUG_XREMOTE("XRemote data received: ");
			// helper->DebugPrintMessageWithNullBytes(DEBUG_XREMOTE, rxData, len);
		
			if (address[0] == "renew") {
                //fprintf(stdout, "Received command: %s\n", rxData);
            } else if (address[0] == "info") {
                xremote->AnswerInfo();
            } else if (address[0] == "xinfo") {
                xremote->AnswerXInfo();
            } else if (address[0] == "status") {
                xremote->AnswerStatus();
            } else if (address[0] == "xremote") {
                // Optional: read and store IP-Address of client
				//xremoteSync(true);
            } else if (address[0] == "unsubscribe") {
                // Optional: remove xremote client
            } else if (address[0] == "ch") {
                // /ch/xx/mix/fader~~~~,f~~[float]
                // /ch/xx/mix/pan~~,f~~[float]
                // /ch/xx/mix/on~~~,i~~[int]

                //channel = ((rxData[4]-48)*10 + (rxData[5]-48)) - 1;
				channel = stoi(address[1]);

				if (address[2] == "mix") {
					if (address[3] == "fader") {
						float newVolume = tosc_getNextFloat(&osc);
						mixer->SetVolumeOscvalue(channel-1, newVolume);
						helper->DEBUG_XREMOTE( "Ch %u: Volume set to %f\n", channel, (double)newVolume);
					}else if (address[3] == "pan") {
						// get pan-value
						value32bit.u8[0] = rxData[23];
						value32bit.u8[1] = rxData[22];
						value32bit.u8[2] = rxData[21];
						value32bit.u8[3] = rxData[20];
						
						//encoderValue = value32bit.f * 255.0f;
						mixer->SetBalance(channel,  value32bit.f * 100.0f);
						helper->DEBUG_XREMOTE( "Ch %u: Balance set to %f\n",  channel+1, (double)(value32bit.f * 100.0f));
					}else if (address[3] == "on") {
						// get mute-state (caution: here it is "mixer-on"-state)
						mixer->SetMute(channel, (rxData[20+3] == 0));
						helper->DEBUG_XREMOTE( "Ch %u: Mute set to %u\n",  channel+1, (rxData[20+3] == 0));
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
            } else if (address[0] == "main") {
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
                            
                            //float newVolume = (value32bit.f * 54.0f) - 48.0f;
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



			// tosc_getFormat(&osc)); // the OSC format string, e.g. "f"
			// 	for (int i = 0; osc.format[i] != '\0'; i++) {
			// 		switch (osc.format[i]) {
			// 			case 'f': printf("%g ", tosc_getNextFloat(&osc)); break;
			// 			case 'i': printf("%i ", tosc_getNextInt32(&osc)); break;
			// 			// returns NULL if the buffer length is exceeded
			// 			case 's': printf("%s ", tosc_getNextString(&osc)); break;
			// 			default: continue;
			// 	}
		    // }
			// printf("\n");
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
  char dev[] = "FBDEV";
  driver_backends_init_backend(dev);

  lv_timer_create(timer10msCallbackLvgl, 10, NULL);
  lv_timer_create(timer100msCallbackLvgl, 100, NULL); // surface/gui sync

  // initialize GUI created by EEZ
  ui_init();

  ShowPage(X32_PAGE_HOME);   // shows the HOME Page
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
	// Home
	pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
	pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

	pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
	pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_EQ;

	pages[X32_PAGE_EQ].prevPage = X32_PAGE_CONFIG;
	pages[X32_PAGE_EQ].nextPage = X32_PAGE_NONE;

	// Routing
	pages[X32_PAGE_ROUTING].prevPage = X32_PAGE_NONE;
	pages[X32_PAGE_ROUTING].nextPage = X32_PAGE_ROUTING_FPGA;

	pages[X32_PAGE_ROUTING_FPGA].prevPage = X32_PAGE_ROUTING;
	pages[X32_PAGE_ROUTING_FPGA].nextPage = X32_PAGE_ROUTING_DSP1;

	pages[X32_PAGE_ROUTING_DSP1].prevPage = X32_PAGE_ROUTING_FPGA;
	pages[X32_PAGE_ROUTING_DSP1].nextPage = X32_PAGE_ROUTING_DSP2;

	pages[X32_PAGE_ROUTING_DSP2].prevPage = X32_PAGE_ROUTING_DSP1;
	pages[X32_PAGE_ROUTING_DSP2].nextPage = X32_PAGE_NONE;
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
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 0, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_HOME, 1);
			break;
		case X32_PAGE_CONFIG:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 1, LV_ANIM_OFF);
			if (config->IsModelX32FullOrCompactOrProducer()) {
				surface->SetLedByEnum(X32_BTN_VIEW_CONFIG, 1);
			}
			break;

		case X32_PAGE_EQ:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 4, LV_ANIM_OFF);
			if (config->IsModelX32FullOrCompactOrProducer()) {
				surface->SetLedByEnum(X32_BTN_VIEW_EQ, 1);
			}
			break;

		case X32_PAGE_METERS:
			lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_METERS, 1);
			break;
		case X32_PAGE_ROUTING:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 0, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_ROUTING, 1);
			break;
		case X32_PAGE_ROUTING_FPGA:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 1, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_ROUTING, 1);
			break;
		case X32_PAGE_ROUTING_DSP1:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 2, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_ROUTING, 1);
			break;
		case X32_PAGE_ROUTING_DSP2:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 3, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_ROUTING, 1);
			break;
		case X32_PAGE_SETUP:
			lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_SETUP, 1);
			break;
		case X32_PAGE_LIBRARY:
			lv_tabview_set_active(objects.maintab, 4, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_LIBRARY, 1);
			break;
		case X32_PAGE_EFFECTS:
			lv_tabview_set_active(objects.maintab, 5, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_EFFECTS, 1);
			break;
		case X32_PAGE_MUTE_GRP:
			lv_tabview_set_active(objects.maintab, 6, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_MUTE_GRP, 1);
			break;
		case X32_PAGE_UTILITY:
			lv_tabview_set_active(objects.maintab, 7, LV_ANIM_OFF);
			surface->SetLedByEnum(X32_BTN_UTILITY, 1);
			break;
		default:
			activePage = X32_PAGE_NONE;
			helper->Error("Page not found!\n");
	}

	state->SetChangeFlags(X32_MIXER_CHANGED_PAGE);
}

//#####################################################################################################################
//
//  ######  ##    ## ##    ##  ######  
// ##    ##  ##  ##  ###   ## ##    ## 
// ##         ####   ####  ## ##       
//  ######     ##    ## ## ## ##       
//       ##    ##    ##  #### ##       
// ##    ##    ##    ##   ### ##    ## 
//  ######     ##    ##    ##  ######
//
//#####################################################################################################################

void X32Ctrl::syncAll(void) {
	if (state->HasAnyChanged()){
		guiSync();
		surfaceSync();
		xremoteSync();

		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
			// TODO Maybe?: do not sync if just selection has changed
			mixer->SyncVChannelsToHardware();
		}

		state->ResetChangeFlags();
		for(uint8_t index = 0; index < MAX_VCHANNELS; index++){
			mixer->GetVChannel(index)->ResetVChannelChangeFlags();
		}
	}
}

// sync mixer state to GUI
void X32Ctrl::guiSync(void) {
	if (config->IsModelX32Core()){
		return;
	}

	if (state->HasChanged(X32_MIXER_CHANGED_PAGE)    ||
		state->HasChanged(X32_MIXER_CHANGED_BANKING) ||
		state->HasChanged(X32_MIXER_CHANGED_SELECT)  ||
		state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)||
		state->HasChanged(X32_MIXER_CHANGED_ROUTING)||
		state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)||
		state->HasChanged(X32_MIXER_CHANGED_GUI)) {

		if (state->HasChanged(X32_MIXER_CHANGED_PAGE)){
			helper->DEBUG_GUI("Page changed to: %d\n", activePage);
		}

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

		lv_label_set_text_fmt(objects.current_channel_number, "%s", chan->nameIntern.c_str());
		lv_label_set_text_fmt(objects.current_channel_name, "%s", chan->name.c_str());
		lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);

		// //set Encoders to default state
		// const char*  encoderTextMap[] = {"Input", " ", " "," "," ","Output", NULL};
		// lv_btnmatrix_set_map(objects.display_encoders, encoderTextMap);

		//####################################
		//#         Page Home
		//####################################

		bool phantomPower = mixer->GetPhantomPower(GetSelectedvChannelIndex());
		
		if (activePage == X32_PAGE_CONFIG){
		//####################################
		//#         Page Config
		//####################################
			char dspSourceName[5] = "";
			mixer->dsp->GetSourceName(&dspSourceName[0], GetSelectedvChannelIndex(), mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[GetSelectedvChannelIndex()].inputSource - 1]);
			lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

			lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)mixer->GetGain(chanIndex));
			lv_label_set_text_fmt(objects.current_channel_phantom, "%d", phantomPower);
			lv_label_set_text_fmt(objects.current_channel_invert, "%d", mixer->GetPhaseInvert(chanIndex));
			lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)mixer->GetBalance(chanIndex));
			lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)mixer->GetVolumeDbfs(chanIndex));


			//char outputDestinationName[10] = "";
			//routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
			//lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

			guiSetEncoderText("Source\n[Invert]", "Gain\n[48V]", "PAN/BAL\n[Center]", "Volume\n[Mute]", "-", "-");
		}else if (activePage == X32_PAGE_ROUTING_FPGA) {
		//####################################
		//#         Page Routing (FPGA)
		//####################################
			char outputDestinationName[10] = "";
			char inputSourceName[10] = "";
			uint8_t routingIndex = 0;

			// Table

			// output-taps
			// 1-16 = XLR-outputs
			// 17-32 = UltraNet/P16-outputs
			// 33-64 = Card-outputs
			// 65-72 = AUX-outputs
			// 73-112 = DSP-inputs
			// 113-160 = AES50A-outputs
			// 161-208 = AES50B-outputs

			// Inital Table Draw
			if (!state->page_routing_fpga_table_drawn){
				lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
				lv_table_set_column_count(objects.table_routing_fpga, 4);
				for (uint8_t i=0; i < NUM_OUTPUT_CHANNEL; i++){
					mixer->fpga->RoutingGetOutputNameByIndex(&outputDestinationName[0], i+1);
					routingIndex = mixer->fpga->RoutingGetOutputSourceByIndex(i+1);
					mixer->fpga->RoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex);
					lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 0, "%s", outputDestinationName);
					lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 2, "%s", inputSourceName);
				}
				lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, "#");
				lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 3, "#");
				state->page_routing_fpga_table_drawn = true;
			}

			// Update Table

			if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {

				if (state->gui_selected_item >= NUM_OUTPUT_CHANNEL) {
					state->gui_selected_item = NUM_OUTPUT_CHANNEL-1;
				}

				if (state->gui_selected_item != state->gui_old_selected_item ) {
					// remove old indicator
					lv_table_set_cell_value(objects.table_routing_fpga, state->gui_old_selected_item, 1, " ");
					lv_table_set_cell_value(objects.table_routing_fpga, state->gui_old_selected_item, 3, " ");
					
					// display new indicator
					lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, "#");
					lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 3, "#");
					
					// set select to scroll table
					lv_table_set_selected_cell(objects.table_routing_fpga, state->gui_selected_item, 2);
					
					state->gui_old_selected_item = state->gui_selected_item;
				}
			} 
			
			if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
				routingIndex = mixer->fpga->RoutingGetOutputSourceByIndex(state->gui_selected_item+1);
				mixer->fpga->RoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex);
				lv_table_set_cell_value_fmt(objects.table_routing_fpga, state->gui_selected_item, 2, "%s", inputSourceName);
			}

			guiSetEncoderText("Select", "Change", "-", "-", String(routingIndex), String(state->gui_selected_item));
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
				chan = GetVChannel(i);
				chanIndex = i;

				if (mixer->GetPhantomPower(i)){
					lv_buttonmatrix_set_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
				} else {
					lv_buttonmatrix_clear_button_ctrl(objects.phantomindicators, i, LV_BUTTONMATRIX_CTRL_CHECKED);
				}

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
		}else if (activePage == X32_PAGE_UTILITY) {
		//####################################
		//#         Page Meters
		//####################################
			guiSetEncoderText("Reload DSP1", "-", "-", "-", "-", String(state->debugvalue).c_str());
		}else{
		//####################################
		//#         All other pages
		//####################################
			guiSetEncoderText("-", "-", "-", "-", "-", "-");
		}
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
				surface->SetLedByEnum(X32_BTN_PHANTOM_48V, mixer->GetPhantomPower(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT)){
				surface->SetLedByEnum(X32_BTN_PHASE_INVERT, mixer->GetPhaseInvert(chanIndex));
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
				// update gain-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_GAIN) >> 8, surface->Enum2Encoder(X32_ENC_GAIN) & 0xFF, 0, (mixer->GetGain(chanIndex) + 12.0f)/0.72f, 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)){
				// update pan-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_PAN) >> 8, surface->Enum2Encoder(X32_ENC_PAN) & 0xFF, 2, (mixer->GetBalance(chanIndex) + 100.0f)/2.0f, 1);
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)){
				// update pan-encoder
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_1) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_1) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 0)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_2) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_2) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 1)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_3) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_3) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 2)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_4) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_4) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 3)/20.0f) * 100.0f, 1);
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
					
					// EQ-LEDS
					surface->SetLedByEnum(X32_LED_EQ_HCUT, false);
					surface->SetLedByEnum(X32_LED_EQ_HSHV, false);
					surface->SetLedByEnum(X32_LED_EQ_LCUT, false);
					surface->SetLedByEnum(X32_LED_EQ_LSHV, false);
					surface->SetLedByEnum(X32_LED_EQ_PEQ, false);
					surface->SetLedByEnum(X32_LED_EQ_VEQ, false);

					switch (mixer->dsp->Channel[chanIndex].peq[activeEQ].type) {
						case 0: // Allpass
							break;
						case 1: // Peak
							surface->SetLedByEnum(X32_LED_EQ_PEQ, true);
							break;
						case 2: // LowShelf
							surface->SetLedByEnum(X32_LED_EQ_LSHV, true);
							break;
						case 3: // HighShelf
							surface->SetLedByEnum(X32_LED_EQ_HSHV, true);
							break;
						case 4: // Bandpass
							break;
						case 5: // Notch
							break;
						case 6: // LowPass
							surface->SetLedByEnum(X32_LED_EQ_HCUT, true);
							break;
						case 7: // HighPass
							surface->SetLedByEnum(X32_LED_EQ_LCUT, true);
							break;
					}

					surface->SetLedByEnum(X32_BTN_EQ_LOW, activeEQ == 0);
					surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, activeEQ == 1);
					surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, activeEQ == 2);
					surface->SetLedByEnum(X32_BTN_EQ_HIGH, activeEQ == 3);
				}
			}
			
		}

		if (config->IsModelX32Rack()){
			// Channel section
			setLedChannelIndicator();
			chan = GetSelectedvChannel();

			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
				surface->SetLedByEnum(X32_BTN_CHANNEL_SOLO, mixer->GetSolo(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
				surface->SetLedByEnum(X32_BTN_CHANNEL_MUTE, mixer->GetMute(chanIndex)); 
			}
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
				surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 0, mixer->GetVolumeDbfs(chanIndex),  mixer->GetMute(chanIndex), 0);
			}
		}
	}

	if (config->IsModelX32Rack()){
		// Clear Solo
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)){ surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, mixer->IsSoloActivated()); }

		// Main Channel
		VChannel* mainchan = GetVChannel(X32_VCHANNEL_BLOCK_MAIN);
		if (mainchan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || mainchan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
			surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 1, mixer->GetVolumeDbfs(X32_VCHANNEL_BLOCK_MAIN),  mixer->GetMute(X32_VCHANNEL_BLOCK_MAIN), 0);
		}
	}
}

void X32Ctrl::surfaceSyncBoard(X32_BOARD p_board) {
	bool fullSync = false;

	if (state->HasChanged(X32_MIXER_CHANGED_SELECT) || state->HasChanged(X32_MIXER_CHANGED_BANKING)){ 
		// channel selection has changed - do a full sync
		fullSync=true; 
	}

	uint8_t offset = 0;
	if (config->IsModelX32Full()){
		if (p_board == X32_BOARD_M){ offset=8; }
		if (p_board == X32_BOARD_R){ offset=16; }
	} else if (config->IsModelX32CompactOrProducer()) {
		if (p_board == X32_BOARD_R){ offset=8; }
	}

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
				helper->DEBUG_SURFACE("syncronize vChannel%d: %s -", channelIndex, chan->name.c_str());

				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SELECT)){ 
					helper->DEBUG_SURFACE(" Select");
					surface->SetLed(p_board, 0x20+i, chan->selected);
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
					helper->DEBUG_SURFACE(" Solo");
					surface->SetLed(p_board, 0x30+i, mixer->GetSolo(channelIndex)); 
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
					helper->DEBUG_SURFACE(" Mute");
					surface->SetLed(p_board, 0x40+i, mixer->GetMute(channelIndex)); 
				}

				if ((fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)) && touchcontrolCanSetFader(p_board, i)){
					helper->DEBUG_SURFACE(" Fader");
					u_int16_t faderVolume = mixer->GetVolumeFadervalue(channelIndex);
					surface->SetFader(p_board, i, faderVolume);
				}

				if (
					fullSync                                              ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT)   ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)         ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)           ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)             ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)           ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)        ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)        ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_COLOR)          ||
					chan->HasChanged(X32_VCHANNEL_CHANGED_NAME)
				)
				{
					helper->DEBUG_SURFACE(" LCD");
					SetLcdFromVChannel(p_board, i, channelIndex);
				}

				helper->DEBUG_SURFACE("\n");
			}
		}
	}

	if (p_board == X32_BOARD_R){
		// Clear Solo
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) { surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, mixer->IsSoloActivated()); }
	}
}

void X32Ctrl::SetLcdFromVChannel(uint8_t p_boardId, uint8_t lcdIndex, uint8_t channelIndex){
    LcdData* data = new LcdData();
	VChannel* chan = GetVChannel(channelIndex);

    data->boardId = p_boardId;
    data->color = chan->color;
    data->lcdIndex = lcdIndex;
    data->icon.icon = 0;
    data->icon.x = 0;
    data->icon.y = 0;

    // Gain / Lowcut
    data->texts[0].text = String(mixer->GetGain(channelIndex), 1) + String("dB ") + String(mixer->GetLowcut(channelIndex), 0) + String("Hz");
    data->texts[0].size = 0;
    data->texts[0].x = 3;
    data->texts[0].y = 0;

    // Phanton / Invert / Gate / Dynamics / EQ active
    data->texts[1].text =
		String(mixer->GetPhantomPower(channelIndex) ? "48V " : "    ") +
		String(mixer->GetPhaseInvert(channelIndex) ? "@ " : "  ") +
		String(mixer->GetGate(channelIndex) > -80 ? "G " : "  ") +
		String(mixer->GetDynamics(channelIndex) < 0 ? "D " : "  ") +
		// TODO String(mixer->GetEq(channelIndex) ? "E " : "  ");
		String(true ? "E" : " ");
    data->texts[1].size = 0;
    data->texts[1].x = 10;
    data->texts[1].y = 15;

    // Volume / Panorama

    float balance = mixer->GetBalance(channelIndex);
    
    char balanceText[8] = "-------";
    if (balance < -70){
        balanceText[0] = '|';
    } else if (balance < -40){
        balanceText[1] = '|';
    } else if (balance < -10){
        balanceText[2] = '|';
    } else if (balance > 70){
        balanceText[6] = '|';
    } else if (balance > 40){
        balanceText[5] = '|';
    } else if (balance > 10){
        balanceText[4] = '|';
    } else {
        balanceText[3] = '|';
    }

	float volume = mixer->GetVolumeDbfs(channelIndex);
    if (volume > -100) {
        data->texts[2].text = String(volume, 1) + String("dB ") + String(balanceText);
    }else{
		data->texts[2].text = String("-oodB ") + String(balanceText);
    }
    data->texts[2].size = 0;
    data->texts[2].x = 8;
    data->texts[2].y = 30;

    // vChannel Name
    data->texts[3].text = String(chan->name);
    data->texts[3].size = 0;
    data->texts[3].x = 0;
    data->texts[3].y = 48;

    surface->SetLcdX(data, 4);

	delete data;
}

void X32Ctrl::surfaceUpdateMeter(void) {
	if (config->IsModelX32Core()) {
		// no led-meters at all
		return;
	}

	uint8_t chanIdx = GetSelectedvChannelIndex();

	if (config->IsModelX32Rack()) {
		surface->SetMeterLedMain_Rack(
			surfaceCalcPreampMeter(chanIdx), // selected channel
			mixer->dsp->MainChannelLR.meterInfo[0],
		 	mixer->dsp->MainChannelLR.meterInfo[1],
		 	mixer->dsp->MainChannelSub.meterInfo[0]
		);
	}

	// update channel-meters
	if (config->IsModelX32Full()) {

		// update preamp, dynamics, meterL, meterR, meterSolo
		// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
		// leds = 32-bit bitwise (bit 0=-57dB ... 22=-2, 23=-1, 24=Clip)
		//setMeterLedMain(0b00000111, 0b11100001, 0x0FFFFF, 0x0FFFFF, 0x000FFF);
		surface->SetMeterLedMain_FullCompactProducer(
			surfaceCalcPreampMeter(chanIdx),
			surfaceCalcDynamicMeter(chanIdx),
			mixer->dsp->MainChannelLR.meterInfo[0],
			mixer->dsp->MainChannelLR.meterInfo[1],
			mixer->dsp->MainChannelSub.meterInfo[0]
		);

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
	}
	
	
	// else{
	// 	// update meters on board L
	// 	switch (activeBank_inputFader) {
	// 		case 0: // Input 1-8
	// 			for (uint8_t i = 0; i < 8; i++) {
	// 				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[i].meterInfo);
	// 			}
	// 			break;
	// 		case 1: // Input 9-16
	// 			for (uint8_t i = 0; i < 8; i++) {
	// 				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[8 + i].meterInfo);
	// 			}
	// 			break;
	// 		case 2: // Input 17-24
	// 			for (uint8_t i = 0; i < 8; i++) {
	// 				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[16 + i].meterInfo);
	// 			}
	// 			break;
	// 		case 3: // Input 25-32
	// 			for (uint8_t i = 0; i < 8; i++) {
	// 				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[24 + i].meterInfo);
	// 			}
	// 			break;
	// 		case 4: // Aux 1-8
	// 			for (uint8_t i = 0; i < 8; i++) {
	// 				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->Channel[32 + i].meterInfo);
	// 			}
	// 			break;
	// 		case 5: // FX-Return
	// 			break;
	// 		case 6: // Bus 1-8
	// 			break;
	// 		case 7: // Bus 9-16
	// 			break;
	// 	}
	// }

	// // update meters on board R
	// switch (activeBank_busFader) {
	// 	case 0: // DCA1-8
	// 		// no meter here
	// 		break;
	// 	case 1: // BUS 1-8
	// 		break;
	// 	case 2: // BUS 1-16
	// 		break;
	// 	case 3: // Matrix 1-6, Special, MainSub
	// 		break;
	// }
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

		if (chanIdx <= 31){
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
	uint8_t meterdata = 0;
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

// sync mixer state to GUI
void X32Ctrl::xremoteSync(bool syncAll) {
	bool fullSync = false;

	if (syncAll || state->HasChanged(X32_MIXER_CHANGED_SELECT)){ 
		// channel selection has changed - do a full sync
		fullSync=true; 
	}
	
	// DEBUG
	xremote->SetCard(10); // X-LIVE

	for(uint8_t i=0; i<X32_VCHANNEL_BLOCKSIZE_NORMAL; i++) {
		uint8_t chanindex = i;
		VChannel* chan = mixer->GetVChannel(i);
		if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
			xremote->SetFader(String("ch"), chanindex, mixer->GetVolumeOscvalue(chanindex));
		}
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)){
		// 	xremote->SetPan(chanindex, mixer->vchannel[chanindex]->dspChannel->balance);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
		// 	xremote->SetMute(chanindex, mixer->vchannel[chanindex]->dspChannel->muted);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
		// 	xremote->SetSolo(chanindex, mixer->vchannel[chanindex]->dspChannel->solo);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_COLOR)){
		// 	xremote->SetColor(chanindex, mixer->vchannel[chanindex]->color);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_NAME)){
		// 	xremote->SetName(chanindex, mixer->vchannel[chanindex]->name);
		// }
	}

	for(uint8_t i=X32_VCHANNEL_BLOCK_AUX; i<X32_VCHANNEL_BLOCKSIZE_AUX; i++) {
		uint8_t chanindex = i;
		VChannel* chan = mixer->GetVChannel(i);
		if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
			xremote->SetFader(String("auxin"), chanindex, mixer->GetVolumeOscvalue(chanindex));
		}
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)){
		// 	xremote->SetPan(chanindex, mixer->vchannel[chanindex]->dspChannel->balance);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
		// 	xremote->SetMute(chanindex, mixer->vchannel[chanindex]->dspChannel->muted);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
		// 	xremote->SetSolo(chanindex, mixer->vchannel[chanindex]->dspChannel->solo);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_COLOR)){
		// 	xremote->SetColor(chanindex, mixer->vchannel[chanindex]->color);
		// }
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_NAME)){
		// 	xremote->SetName(chanindex, mixer->vchannel[chanindex]->name);
		// }
	}
	
	// update meters everytime
    xremote->UpdateMeter(mixer);
}

// ####################################################################
// #
// #
// #        channel Assigment
// #
// #
// ####################################################################

// direction - positive or negative integer value
void X32Ctrl::ChangeSelect(int8_t direction){
	int16_t newSelectedVChannel = GetSelectedvChannelIndex() + direction;
	helper->DEBUG_X32CTRL("ChangeSelect(): selected channel index: %d, direction: %d, new channel index: %d\n", GetSelectedvChannelIndex(), direction, newSelectedVChannel);
	if (newSelectedVChannel < 0) {
		newSelectedVChannel = MAX_VCHANNELS -1;
	} else if (newSelectedVChannel >= MAX_VCHANNELS){
		newSelectedVChannel = 0;
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

		// TODO implement properly
		//if (!surface->IsFaderBlocked(event->boardId, event->index)){
			vchannelIndex = SurfaceChannel2vChannel(event->index + offset);
			mixer->SetVolume(vchannelIndex, helper->Fadervalue2dBfs(event->value));

			touchcontrol.board = event->boardId;
			touchcontrol.faderIndex = event->index;
			touchcontrol.value = 5;

			helper->DEBUG_SURFACE("FaderMoved(%s): vChannel%d TouchControl=%d\n", event->ToString().c_str(), vchannelIndex, touchcontrol.value);
		//}
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
				// case X32_BTN_UP: {
				// 	for (int l =0; l<25; l++){
				// 		surface->SetLed(1, l, true);
				// 	}
				// 	break;
				// }
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
					helper->DEBUG_SURFACE("Unhandled button detected.\n");
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
						mixer->TogglePhaseInvert(GetSelectedvChannelIndex());
						break;
					case X32_BTN_ENCODER2:
						mixer->TogglePhantom(GetSelectedvChannelIndex());
						break;
					case X32_BTN_ENCODER3:
						mixer->SetBalance(GetSelectedvChannelIndex(), 0.0);
						break;
					case X32_BTN_ENCODER4:
						mixer->ToggleMute(GetSelectedvChannelIndex());
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
						mixer->SetLowcut(GetSelectedvChannelIndex(), 20); // set to 20 Hz
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
		}else if (activePage == X32_PAGE_UTILITY){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						// Reload DSP1
						mixer->dsp->spi->CloseDspConnections();
						mixer->dsp->spi->ConfigureDsp();
						mixer->dsp->spi->OpenDspConnections();
						mixer->dsp->dspInit();
						mixer->dsp->SendAll();
						break;
					case X32_BTN_ENCODER2:
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
	helper->DEBUG_SURFACE("Encoder: %d\n", amount);

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		switch (encoder){
			case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
				ChangeSelect(amount);
				break;
			case X32_ENC_MAIN_LEVEL:  // only X32 Rack
				mixer->ChangeVolume(X32_VCHANNEL_BLOCK_MAIN, amount);
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
				// just here to avoid compiler warnings                  
				break;
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()) {
		if (activePage == X32_PAGE_CONFIG){
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeDspInput(GetSelectedvChannelIndex(), amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeGain(GetSelectedvChannelIndex(), amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeBalance(GetSelectedvChannelIndex(), amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeVolume(GetSelectedvChannelIndex(), amount);
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
		}else if (activePage == X32_PAGE_ROUTING_FPGA) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGuiSelection(amount);
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
		else if (activePage == X32_PAGE_UTILITY) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					// reload DSP1 on button-click
					break;
				case X32_ENC_ENCODER2:
					break;
				case X32_ENC_ENCODER3:
					break;
				case X32_ENC_ENCODER4:
					break;
				case X32_ENC_ENCODER5:
					break;
				case X32_ENC_ENCODER6:
					surface->SetLed(1,state->debugvalue++, true);
					state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
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
		helper->DEBUG_SURFACE("TouchControl=%d\n", touchcontrol.value);
	}
}

bool X32Ctrl::touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
	if (touchcontrol.board != p_board){
		return true;
	} 

	if (touchcontrol.faderIndex != p_faderIndex){
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
	helper->DEBUG_X32CTRL("################# BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		helper->DEBUG_X32CTRL("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i]);
	}
	helper->DEBUG_X32CTRL("END ############# BANK%d ############### END\n", bank);
}

void X32Ctrl::DebugPrintBusBank(uint8_t bank)
{
	helper->DEBUG_X32CTRL("################# BUS BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		helper->DEBUG_X32CTRL("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2VChannel[i]);
	}
	helper->DEBUG_X32CTRL("END ############# BUS BANK%d ############### END\n", bank);
}

void X32Ctrl::DebugPrintvChannels(void){
	for (int i = 0; i < MAX_VCHANNELS; i++)
	{
	   helper->DEBUG_X32CTRL(mixer->GetVChannel(i)->ToString().c_str());

	}
}
