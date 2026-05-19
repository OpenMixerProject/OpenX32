#include "ctrl.h"

X32Ctrl::X32Ctrl(X32BaseParameter* basepar) : X32Base(basepar)
{
	mixer = new Mixer(basepar);
	surface = new Surface(basepar);
	xremote = new XRemote(basepar);
	wsm = new WSM(basepar);
	lcdmenu = new LcdMenu(basepar, mixer, surface); // only used for X32Core (at the moment, maybe later for assing-section?)

	#if ENABLE_ARTNET
	artnet = new Artnet(basepar);
	#endif
}

// ###########################################################################
// #
// #      #### ##    ## #### ######## 
// #       ##  ###   ##  ##     ##    
// #       ##  ####  ##  ##     ##    
// #       ##  ## ## ##  ##     ##    
// #       ##  ##  ####  ##     ##    
// #       ##  ##   ###  ##     ##    
// #      #### ##    ## ####    ## 
// #
// ###########################################################################

void X32Ctrl::Init()
{
	//##################################################################################
	//#
	//# 	Read x32ctrl configuration
	//#
	//##################################################################################

	// first try to find what we are: Fullsize, Compact, Producer, Rack or Core
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "Reading config...");
	char model[12];
	char serial[15];
	char date[16];
	char cfg[5];
	helper->ReadConfig("/etc/x32.conf", "MDL=", model, 12);
	helper->ReadConfig("/etc/x32.conf", "SN=", serial, 15);
	helper->ReadConfig("/etc/x32.conf", "DATE=", date, 16);
	helper->ReadConfig("/etc/x32.conf", "CFG", cfg, 5);
	helper->Log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

	if (state->bodyless) {
		config->SetModel("X32C");
	} else if (state->raspi) {
		config->SetModel("X32RACK");
	} else {
		config->SetModel(model);
	}

	//##################################################################################
	//#
	//# 	Initialize system
	//#
	//##################################################################################

	InitBanks();
	LoadDefaultSurfaceBinding();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->Init()");
	mixer->Init();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "surface->Init()");
	surface->Init();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "xremote->Init()");
	xremote->Init();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "wsm->Init()");
	wsm->Init();


	if (config->IsModelX32Core()) {
		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "lcdmenu->Init()");
		lcdmenu->OnInit();
	}

	#if ENABLE_ARTNET
	helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "artnet->Init()");
	artnet->Init();
	#endif
	

	//############################################################################
	//#                                                                          #
	//#     Default X32Config                                                       #
	//#                                                                          #
	//############################################################################
	//#                                                                          #
	//#     With GUI                                                             #
	//#                                                                          #
	if(config->IsModelX32FullOrCompactOrProducer())
	{
			ResetFaderBankLayout();
			LoadFaderBankLayout();
	}
	//############################################################################
	//#                                                                          #
	//#     Without GUI                                                          #
	//#                                                                          #
	if(config->IsModelX32Core())
	{
			// Placeholder
	}
	//############################################################################
	//#                                                                          #
	//#     Common to all                                                        #
	//#                                                                          #

			// Just load a default set of FXes
			// TODO: Save and Load
			/*
			// available FX types:
			========================
			NONE = -1,
			REVERB = 0,
			CHORUS = 1,
			TRANSIENTSHAPER = 2,
			OVERDRIVE = 3,
			DELAY = 4,
			MULTIBANDCOMPRESOR = 5,
			DYNAMICEQ = 6
			*/

			mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2); // this effect takes lot of DSP-ressources
            mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2);
            mixer->dsp->DSP2_SetFx(2, FX_TYPE::DELAY, 2);
            mixer->dsp->DSP2_SetFx(3, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(4, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(5, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(6, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2);
	//#                                                                          #
	//############################################################################
    //#                                                                          #
	//#     EoDC - Default of Default X32Config :-)                                 #
	//#                                                                          #
	//############################################################################

	// DEBUG
	if (state->raspi)
	{
		// load X32 Core button definitions for DEBUG with RASPI
		surface->LoadX32CoreDefinitions();
	}

	// X32Config
	if(!mixer->LoadConfig(0))
	{
		// create new ini file
		helper->DEBUG_INI(DEBUGLEVEL_NORMAL, "no default configfile found, creating one");
		
		mixer->SaveConfig(0);
	}
}

// Load a Fader bank layout, e.g. LAYOUT_X32 or LAYOUT_USER
void X32Ctrl::LoadFaderBankLayout()
{
	// ##################################################
	// #
	// #   	Fader bank layout like X32
	// #
	// #	Channel section		|	Bus section
	// #	--------------------|--------------------
	// #	32 Channels			|   DCA 1-8
	// #	8 Aux				|	Bus 1-8
	// #	Fx Return			|	Bus 9-16
	// #	Busses 				|	Matrix + Sub
	// #
	// ##################################################


	if (config->IsModelX32Full()){
		// bank is 16 channels wide
		for (uint8_t bank=0;bank<4;bank++){
			for (int i = 0; i <=15; i++) {
				inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 16);
				helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 16));
			}
		}
	}
	if (config->IsModelX32CompactOrProducer()){
		// bank is 8 channels wide
		for (uint8_t bank=0;bank<8;bank++){
			for (int i = 0; i <=7; i++) {
				inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 8);
				helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 8));
			}
		}
	}

	// assign channels to bus fader bank

	// DCA - starts at channel 72
	for (int i = 0; i <=7; i++) {
		busBanks[0].surfaceChannel2VChannel[i] = i + 72;
	}
	// Bus 1-8 - starts at channel 48
	for (int i = 0; i <=7; i++) {
		busBanks[1].surfaceChannel2VChannel[i] = i + 48;
	}
	// Bus 9-16 - starts at channel 56
	for (int i = 0; i <=7; i++) {
		busBanks[2].surfaceChannel2VChannel[i] = i + 56;
	}
	// Matrix / SPECIAL / SUB - starts at channel 64
	for (int i = 0; i <=9; i++) {
		busBanks[3].surfaceChannel2VChannel[i] = i + 64;
	}
}

void X32Ctrl::ResetFaderBankLayout()
{
	// reset channel section
	for (uint8_t b = 0; b < 8; b++)
	{
		for (uint8_t sCh = 0; sCh < 16; sCh++)
		{
			inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}
	// reset bus section
	for (uint8_t b = 0; b < 4; b++)
	{
		for (uint8_t sCh = 0; sCh < 8; sCh++)
		{
			busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}
}



//#####################################################################################################################
//
// ######## #### ##     ## ######## ########  
//    ##     ##  ###   ### ##       ##     ## 
//    ##     ##  #### #### ##       ##     ## 
//    ##     ##  ## ### ## ######   ########  
//    ##     ##  ##     ## ##       ##   ##   
//    ##     ##  ##     ## ##       ##    ##  
//    ##    #### ##     ## ######## ##     ## 
//
//#####################################################################################################################

void X32Ctrl::Tick10ms(void)
{
	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "10ms");

	//#####################################
	//
	//   Freeze changed parameter list
	//
	config->FreezeParameterList();
	//
	//#####################################

	if (config->IsModelX32FullOrCompactOrProducer()) 
	{
		surface->Touchcontrol();	
	}

	// this stateMachine handles the read and write to and from the two DSPs
	mixer->dsp->CallbackStateMachine();

	// read incoming data from surface and handle it
	ProcessUartDataSurface();

	// read incoming data from adda-boards and expansion-card
	ProcessUartDataAdda();

	// read incoming data from AES50 devices
	ProcessUartDataAES50();

	// communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
	//UdpHandleCommunication();

	// communication with Sennheiser Media Control Protocol
	UdpHandleCommunication_WSM();


	// sync if any Mixerparameter has changed
	if (config->HasAnyParameterChanged())
	{
		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->Sync()");
		mixer->Sync();

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->card->Sync()");
		mixer->card->Sync();

		#if ENABLE_ARTNET
		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "artnet->Sync()");
		artnet->Sync();
		#endif

		syncGuiOrLcd();
		//syncXRemote(false);
	}

	if (config->HasAnyParameterChanged() || config->HasAnySurfaceBindingChanged())
	{
		// sync GUI(s) last, to get visual response after the hardware is synced!
		syncSurface(false);
	}

	//#####################################
	//
	//   Unfreeze changed parameter list
	//
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "Reset list of changed Mixerparameter.");
	config->ResetAndUnfreezeChangedParameterList();
	//
	//#####################################
}

void X32Ctrl::Tick50ms(void)
{
	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "50ms");

	// Update VU-Meters
	UpdateMeters();

#if ENABLE_ARTNET
	// update Dimmerkernel
	artnet->Tick();
#endif
}

void X32Ctrl::Tick100ms(void)
{
	static float dspLoadHistory[2][20];
	static uint8_t dspLoadHistoryPointer = 0;
	static uint8_t startupCounter = 0;

	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "100ms");

	// Led Blinking
	surface->Blink();

	// request data from all known clients
	wsm->RequestDataFromClients();

	// DSP-Activity Light
    if (!(state->dsp_disable_activity_light)) {
   	    // toggle the LED on DSP1 and DSP2 to show some activity
        uint32_t value = 2;
		mixer->dsp->spi->QueueDspData(0, 'a', 42, 0, 1, (float*)&value);
        mixer->dsp->spi->QueueDspData(1, 'a', 42, 0, 1, (float*)&value);
    }

	// DEBUG Row in GUI-Header
	if (config->GetBool(DEBUG_HEADER) && !config->IsModelX32Core())
	{
		// calculate mean-value and show the current DSP-load
		dspLoadHistory[0][dspLoadHistoryPointer] = state->dspLoad[0];
		dspLoadHistory[1][dspLoadHistoryPointer] = state->dspLoad[1];
		dspLoadHistoryPointer++;
		if (dspLoadHistoryPointer >= 20) {
			dspLoadHistoryPointer = 0;
		}

		float dspLoadMean[2] = {0, 0};
		for (uint8_t i = 0; i < 20; i++) {
			dspLoadMean[0] += dspLoadHistory[0][i];
			dspLoadMean[1] += dspLoadHistory[1][i];
		}
		dspLoadMean[0] /= 20.0f;
		dspLoadMean[1] /= 20.0f;

		// show DSP debug infos
		lv_label_set_text_fmt(objects.header_debugtext, "DSP1 L: %.1f %% V: v%.2f G: %.0f TxQ: %d DSP2 L: %.1f %% V: v%.2f G: %.0f H: %.0f free TxQ: %d", 
			(double)dspLoadMean[0], (double)state->dspVersion[0], (double)state->dspAudioGlitchCounter[0], mixer->dsp->spi->GetDspTxQueueLength(0),
			(double)dspLoadMean[1], (double)state->dspVersion[1], (double)state->dspAudioGlitchCounter[1], (double)state->dspFreeHeapWords[1], mixer->dsp->spi->GetDspTxQueueLength(1)
		);
	}

	// send AES50-data to FPGA
	// DeviceTypeAndProperty every 2 seconds, Headamp-Message every 2 seconds (Names every 10 seconds)
	mixer->fpga->AES50Tick();

	if (startupCounter < 100)
	{
		startupCounter++;

		if (startupCounter == 1)
		{
			// set IP-Address in GUI
			lv_label_set_text_fmt(objects.header_ip, "IP: %s", helper->getIpAddress().c_str());
		}

		if (startupCounter == 10)
		{
			// the gate, the dynamics and the EQ-settings are not loaded correctly on first load, so load it again after a short time
			mixer->LoadConfig(0);

			// in the following lines the default configuration is set so that the users of the beta-version
			// can start with a working system

			// route channel 1-4 to effects using post-fader tapping
			for (uint8_t i = 0; i < 8; i++)
			{
				config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_DSPCHANNEL + (i / 2), 40 + i);
				config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), 40 + i);
			}

			// set AUX7/8 to MONITOR L/R
			config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MONLEFT, 38);
			config->Set(ROUTING_DSP_OUTPUT, DSP_BUF_IDX_MONRIGHT, 39);
			config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), 38);
			config->Set(ROUTING_DSP_OUTPUT_TAPPOINT, to_underlying(DSP_TAP::POST_FADER), 39);

			// set volume of FX-return to 0dBfs
			for (int i = 0; i < 8; i++)
			{
				config->Set(CHANNEL_VOLUME, 0, 40 + i);
			}

			// set default FXes in FX slots
			mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2); // on first load this effect has a bug, so we have to disable it a bit later
            mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2);
            mixer->dsp->DSP2_SetFx(2, FX_TYPE::DELAY, 2);
            mixer->dsp->DSP2_SetFx(3, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(4, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(5, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(6, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2);

			// set FX-settings to wet on slot 1-4
			config->Set(FX_REVERB_DRY, 0, 0); // fx-slot 1
			config->Set(FX_REVERB_WET, 1, 0); // fx-slot 1
			config->Set(FX_CHORUS_MIX, 1, 1); // fx-slot 2		
		}

		if (startupCounter == 40) {
			// disable effect as on first start of the effect some parts in
			// the external memory gets corrupted. This needs more debugging
			// for now stop-restart is fine
			mixer->dsp->DSP2_SetFx(0, FX_TYPE::NONE, 2);
			mixer->dsp->DSP2_SetFx(2, FX_TYPE::NONE, 2);
		}

		if (startupCounter == 50) {
			// renable effect
			mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2);
			mixer->dsp->DSP2_SetFx(2, FX_TYPE::DELAY, 2);
		}

		if (startupCounter == 60) {
			// unmute ADDA-boards
			mixer->adda->SetMuteAll(false);
		}
	}
}

void X32Ctrl::ProcessUartDataAdda() {
	// read incoming data from adda-boards and expansion-card
	String newCommand = mixer->adda->Receive();
	
	if (newCommand.length() > 0) {
		helper->DEBUG_ADDA(DEBUGLEVEL_TRACE, "Received ADDA command: %s", newCommand.c_str());

		if (newCommand.indexOf("*9") > -1) {
			// we are receiving an answer from the expansion-card

			// only add to debug-text when not sample-index-update
			if (newCommand.indexOf("*9N22") == -1) {
				mixer->debugText += mixer->debugText + "\n" + newCommand;
				if (mixer->debugText.length() > 1000) {
					mixer->debugText = "";
				}
				lv_label_set_text(objects.setup_debug_label, mixer->debugText.c_str());
			}

			mixer->card->ProcessCommand(newCommand);
		}
	}
}

void X32Ctrl::ProcessUartDataAES50() {
	mixer->fpga->AES50Receive();
}

//#####################################################################################################################
//#####################################################################################################################
//
// 			E V E N T S
//
//#####################################################################################################################
//#####################################################################################################################


// receive data from XRemote client
void X32Ctrl::UdpHandleCommunication(void) {
    char rxData[500];
    int bytes_available = 0;
    uint8_t channel;
    data_32b value32bit;
    
    // check for bytes in UDP-buffer
    int result = ioctl(xremote->UdpHandle, FIONREAD, &bytes_available);
    if (bytes_available > 0) {
        socklen_t xremoteClientAddrLen = sizeof(xremote->ClientAddr);
        uint8_t len = recvfrom(xremote->UdpHandle, rxData, bytes_available, MSG_WAITALL, (struct sockaddr *) &xremote->ClientAddr, &xremoteClientAddrLen);

		tosc_message osc;

		if (!tosc_parseMessage(&osc, rxData, len)) {
			string adrPath = string(tosc_getAddress(&osc));
    		vector<string> address = helper->split(adrPath, "/");
			address.erase(address.begin()); // delete empty element
			string format = string(tosc_getFormat(&osc));

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
						//mixer->SetVolumeOscvalue(channel-1, newVolume);
						helper->DEBUG_XREMOTE(DEBUGLEVEL_VERBOSE, "Ch %u: Volume set to %f\n", channel, (double)newVolume);
					}else if (address[3] == "pan") {
						// get pan-value
						value32bit.u8[0] = rxData[23];
						value32bit.u8[1] = rxData[22];
						value32bit.u8[2] = rxData[21];
						value32bit.u8[3] = rxData[20];
						
						//encoderValue = value32bit.f * 255.0f;
						//mixer->SetBalance(channel,  value32bit.f * 100.0f);
						helper->DEBUG_XREMOTE(DEBUGLEVEL_VERBOSE, "Ch %u: Balance set to %f\n",  channel+1, (double)(value32bit.f * 100.0f));
					}else if (address[3] == "on") {
						// get mute-state (caution: here it is "mixer-on"-state)
						//mixer->SetMute(channel, (rxData[20+3] == 0));
						helper->DEBUG_XREMOTE(DEBUGLEVEL_VERBOSE, "Ch %u: Mute set to %u\n",  channel+1, (rxData[20+3] == 0));
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
				//xremote->AnswerAny();
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

// receive data from WSM client
void X32Ctrl::UdpHandleCommunication_WSM()
{
    char rxData[500];
    int bytes_available = 0;
    uint8_t channel;
    data_32b value32bit;
	struct sockaddr_in ClientAddr;
    
    // check for bytes in UDP-buffer
    int result = ioctl(wsm->UdpHandle, FIONREAD, &bytes_available);
    if (bytes_available > 0) {
        socklen_t wsmClientAddrLen = sizeof(ClientAddr);
        uint8_t len = recvfrom(wsm->UdpHandle, rxData, bytes_available, MSG_WAITALL, (struct sockaddr *) &ClientAddr, &wsmClientAddrLen);

		String clientIp = inet_ntoa(ClientAddr.sin_addr);
		String message = String(rxData);
		message.replace("\r", "\r\n");

		helper->DEBUG_XREMOTE(DEBUGLEVEL_NORMAL, "Sennheiser Media Control Protocoll (%s): %s", clientIp.c_str(), message.c_str());
	}

	
}


//#####################################################################################################################
//
// ########     ###     ######   ########  ######  
// ##     ##   ## ##   ##    ##  ##       ##    ## 
// ##     ##  ##   ##  ##        ##       ##       
// ########  ##     ## ##   #### ######    ######  
// ##        ######### ##    ##  ##             ## 
// ##        ##     ## ##    ##  ##       ##    ## 
// ##        ##     ##  ######   ########  ######  
//
//#####################################################################################################################

void X32Ctrl::InitPagesAndGUI()
{
	PageBaseParameter* pagebasepar = new PageBaseParameter(app, config, state, helper, mixer, surface);
	
	pages[X32_PAGE::HOME] = new PageHome(pagebasepar);
	pages[X32_PAGE::CONFIG] = new PageConfig(pagebasepar);
	pages[X32_PAGE::GATE] = new PageGate(pagebasepar);
	pages[X32_PAGE::COMPRESSOR] = new PageDynamics(pagebasepar);
	pages[X32_PAGE::EQ] = new PageEq(pagebasepar);
	pages[X32_PAGE::SENDS] = new PageSends(pagebasepar);
	pages[X32_PAGE::MAIN] = new PageMain(pagebasepar);
	pages[X32_PAGE::METERS] = new PageMeters(pagebasepar);
	pages[X32_PAGE::RTA] = new PageRta(pagebasepar);
	pages[X32_PAGE::ROUTING] = new PageRouting(pagebasepar);
	pages[X32_PAGE::ROUTING_FPGA] = new PageRoutingFpga(pagebasepar);
	pages[X32_PAGE::ROUTING_DSP1] = new PageRoutingChannels(pagebasepar);
	pages[X32_PAGE::ROUTING_DSP2] = new PageRoutingDsp(pagebasepar);
	pages[X32_PAGE::SETUP] = new PageSetup(pagebasepar);
	pages[X32_PAGE::SETUP_CARD] = new PageSetupCard(pagebasepar);
	pages[X32_PAGE::SETUP_SURFACE] = new PageSetupSurface(pagebasepar);
	pages[X32_PAGE::ABOUT] = new PageAbout(pagebasepar);
	pages[X32_PAGE::DEBUG] = new PageDebug(pagebasepar);
	pages[X32_PAGE::PROTOTYPEGUI] = new PagePrototypeGui(pagebasepar);
	pages[X32_PAGE::LIBRARY] = new PageLibrary(pagebasepar);
	pages[X32_PAGE::EFFECTS] = new PageEffects(pagebasepar);
	pages[X32_PAGE::SCENES] = new PageScenes(pagebasepar);
	
	for (const auto& [key, value] : pages)
	{
		value->Init();
	}	
}

bool X32Ctrl::ShowNextPage()
{
	X32_PAGE activePage = (X32_PAGE)config->GetUint(ACTIVE_PAGE);

	X32_PAGE nextPage = pages[activePage]->GetNextPage();
	if (nextPage != X32_PAGE::NONE)
	{
		config->Set(ACTIVE_PAGE, (uint)nextPage);
		return true;
	} 
	
	return false;
}

bool X32Ctrl::ShowPrevPage()
{
	X32_PAGE activePage = (X32_PAGE)config->GetUint(ACTIVE_PAGE);

	X32_PAGE prevPage = pages[activePage]->GetPrevPage();
	if (prevPage != X32_PAGE::NONE)
	{
		config->Set(ACTIVE_PAGE, (uint)prevPage);
		return true;
	}

	return false;
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


// sync mixer state to GUI
void X32Ctrl::syncGuiOrLcd() {

	//####################################
	//#     X32 Core - Sync Lcd
	//####################################

	if (config->IsModelX32Core()){
		if (state->x32core_lcdmode_setup) {
			lcdmenu->OnChange(false);
		}
		
		// return, because X32Core has no GUI
		return;
	}
	
	//####################################
	//#     Update Active Page
	//####################################

	Page* activePage = pages.at((X32_PAGE)(config->GetUint(ACTIVE_PAGE)));
	activePage->Change();

	//####################################
	//#     Show Active Page 
	//#
	//#  Has to be done after updating the current active page, so we get the call.
	//#  If done in other order, the changed Mixerparemter array is emptied and we never change the page!
	//####################################

	if (config->HasParameterChanged(ACTIVE_PAGE)){
		Page* newPage = pages.at((X32_PAGE)config->GetUint(ACTIVE_PAGE));
		newPage->Show();
	}

	//####################################
	//#     Update General Header
	//####################################

	uint chanIndex = config->GetUint(SELECTED_CHANNEL);
	if (config->HasParameterChanged(SELECTED_CHANNEL) ||
		config->HasParametersChanged({
			SELECTED_CHANNEL,
			CHANNEL_NAME,
			CHANNEL_COLOR}, chanIndex)
		)
	{
		lv_color_t color;

		switch (config->GetUint(CHANNEL_COLOR, chanIndex))
		{
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

		lv_label_set_text(objects.current_channel_number, config->GetString(CHANNEL_NAME, chanIndex).c_str());
		lv_label_set_text(objects.current_channel_name, config->GetString(CHANNEL_NAME_INTERN, chanIndex).c_str());
		lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);
	}
}

// sync mixer state to Surface
void X32Ctrl::syncSurface(bool fullSync)
{
	// ######################################
	//
	// Check, if banking has changed
	//
	// ######################################

	if (config->IsModelX32FullOrCompactOrProducer())
	{
		if (config->HasParameterChanged(BANKING_INPUT))
		{
			if (config->IsModelX32Full())
			{
				X32BankId bank1;
				X32BankId bank2;

				switch((X32BankId)(config->GetUint(BANKING_INPUT)))
				{
					case X32BankId::CH1_16:
						bank1 = X32BankId::CH1_8;
						bank2 = X32BankId::CH9_16;
						break;
					case X32BankId::CH17_32:
						bank1 = X32BankId::CH17_24;
						bank2 = X32BankId::CH25_32;
						break;
					case X32BankId::AUX_USB_FX_RET:
						bank1 = X32BankId::AUX_USB;
						bank2 = X32BankId::FX_RET;
						break;
					case X32BankId::BUS1_16:
						bank1 = X32BankId::BUS1_8;
						bank2 = X32BankId::BUS9_16;
						break;
					case X32BankId::REMOTE1:
						bank1 = X32BankId::REMOTE1;
						bank2 = X32BankId::REMOTE2;
						break;
					case X32BankId::FLEX1:
						bank1 = X32BankId::FLEX1;
						bank2 = X32BankId::FLEX2;
						break;
				}

				LoadBank(X32BankTarget::InputSection, bank1);
				LoadBank(X32BankTarget::InputSection2, bank2);
			}
			else
			{
				LoadBank(X32BankTarget::InputSection, (X32BankId)(config->GetUint(BANKING_INPUT)));
			}
		}

		if (config->HasParameterChanged(BANKING_BUS))
		{
			X32BankId bank = (X32BankId)(config->GetUint(BANKING_BUS));

			// special translations for X32 Full: banking over input AND bus section
			switch(bank)
			{
				case X32BankId::CH17_32:
					bank = X32BankId::CH17_24;
					break;
				case X32BankId::AUX_USB_FX_RET:
					bank = X32BankId::AUX_USB;
					break;
				default:
					break;
			}

			LoadBank(X32BankTarget::BusSection, bank);
		}

		// ######################################
		//
		//   DCA Spill
		//
		// ######################################

		vector<MP_ID> filter;
		for (uint i = 0; i < DCA_GROUPS; i++)
    	{
       		filter.push_back(config->MpCalcId(DCA_GROUP_1_MASTER, i));
		}
		if (config->HasParametersChanged(filter))
		{
			// loop through all DCA groups
			for (uint i = 0; i < DCA_GROUPS; i++)
			{
				MP_ID dcaGroupId = config->MpCalcId(DCA_GROUP_1_MASTER, i);
		
				if (config->HasParameterChanged(dcaGroupId))
				{
					if (config->GetBool(dcaGroupId))
					{
						// DCA Spill

						helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "DCA Spill");

						uint nextSurfaceChannelStrip = 0;

						// loop through all channels
						for (uint chanIndex = 0; chanIndex < MAX_VCHANNELS; chanIndex++)
						{
							X32Bank* banktoUse = (nextSurfaceChannelStrip < 8) ? banks[(uint)X32BankId::FLEX1] : banks[(uint)X32BankId::FLEX2];

							if (config->GetBool(config->MpCalcId(DCA_GROUP_1, i), chanIndex))
							{
								// channel is part of the spilled DCA Group -> bind it to the next free channel strip
							 	SurfaceBindingParameter* binding =  banktoUse->channelstrip[nextSurfaceChannelStrip % 8]->lcd;

								binding->mp_action = MixerparameterAction::LCD_Channel;
								binding->mp_index = chanIndex;

								nextSurfaceChannelStrip++;
							}	
							
							if (config->IsModelX32Full() && nextSurfaceChannelStrip > 16)
							{
								break;
							}
							if (config->IsModelX32CompactOrProducer() && nextSurfaceChannelStrip > 8)
							{
								break;
							}
						}

						preSpillLoadedBank = (X32BankId)config->GetUint(BANKING_INPUT);
						config->Set(BANKING_INPUT, (uint)X32BankId::FLEX1);

						break;
					}
					else
					{
						// DCA "Unspill"

						if (config->GetUint(BANKING_INPUT) == (uint)X32BankId::FLEX1)
						{
							helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "DCA Unspill");

							config->Set(BANKING_INPUT, (uint)preSpillLoadedBank);
							preSpillLoadedBank = X32BankId::None;
						}
					}
				}
			}
		}
	}

	// ######################################
	//
	//   LCD Contrast and Brightness
	//
	// ######################################


	if (config->IsModelX32FullOrCompact())
	{
		if (config->HasParameterChanged(LCD_CONTRAST))
		{
			uint contrast = config->GetUint(LCD_CONTRAST);
			helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Set LCD Contrast to %d", contrast);
			
			surface->SetContrast(X32_BOARD_EXTRA, contrast);
			surface->SetContrast(X32_BOARD_L, contrast);
			surface->SetContrast(X32_BOARD_M, contrast);
			surface->SetContrast(X32_BOARD_R, contrast);
		}

		if (config->HasParameterChanged(LED_BRIGHTNESS))
		{
			uint brightness = config->GetUint(LED_BRIGHTNESS);
			helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Set LED Brightness to %d", brightness);
			
			surface->SetBrightness(X32_BOARD_EXTRA, brightness);
			surface->SetBrightness(X32_BOARD_L, brightness);
			surface->SetBrightness(X32_BOARD_M, brightness);
			surface->SetBrightness(X32_BOARD_R, brightness);
		}
	}

	// ######################################
	//
	//   X32 Rack Channel Indicator
	//
	// ######################################

	if (config->IsModelX32Rack())
	{
		if (config->HasParameterChanged(SELECTED_CHANNEL))
		{
			setLedChannelIndicator_Rack();
		}
	}

	// ###########################################
	//
	// Sync all the bound Surfaceelements
	//
	// ###########################################

	for (auto const& [key, value] : *(config->GetSurfaceBinding()))
    {
		// ignore nullptr
		if (value == 0)
		{
			continue;
		}

		SurfaceElementId element_id = key;
		SurfaceBindingParameter* binding_parameter = value;

		// Filter Surfacelements with no visual/physical feedback
		switch(element_id)
		{
			case SurfaceElementId::DISPLAY_ENCODER_1:
			case SurfaceElementId::DISPLAY_ENCODER_2:
			case SurfaceElementId::DISPLAY_ENCODER_3:
			case SurfaceElementId::DISPLAY_ENCODER_4:
			case SurfaceElementId::DISPLAY_ENCODER_5:
			case SurfaceElementId::DISPLAY_ENCODER_6:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_1:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_2:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_3:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_4:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_5:
			case SurfaceElementId::DISPLAY_ENCODER_BUTTON_6:
			case SurfaceElementId::UP:
			case SurfaceElementId::DOWN:
			case SurfaceElementId::LEFT:
			case SurfaceElementId::RIGHT:
				config->RemoveSurfaceBindingChanged(element_id);
				continue;
				break;
			default:
				break;
		}

		// calcluate Mixerparameter ID and Index
		MP_ID parameter_id = config->ParameterCalcId(binding_parameter);
		uint parameter_index = config->ParameterCalcIndex(binding_parameter);

		// full sync
		bool hasChanged = fullSync;

		// Check if the bound Mixerparameter has changed
		if (!hasChanged)
		{
			switch (binding_parameter->mp_action)
			{
				case MixerparameterAction::CLEAR_SOLO:
					hasChanged = config->HasParameterChanged(CHANNEL_SOLO);
					break;
				case MixerparameterAction::LCD_Channel:
					switch(config->GetUint(CHANNEL_LCD_MODE))
					{
						case 0:
							if (config->HasParametersChanged({CHANNEL_PANORAMA, CHANNEL_NAME, CHANNEL_COLOR, CHANNEL_COLOR_INVERTED	}, binding_parameter->mp_index) ||
								config->HasParameterChanged(CHANNEL_LCD_MODE)
							)
							{
								hasChanged = true;
							}
							break;
						case 1:
							if (config->HasParametersChanged({CHANNEL_PHASE_INVERT, CHANNEL_VOLUME, CHANNEL_PANORAMA, CHANNEL_GAIN,	CHANNEL_GATE_TRESHOLD,
									CHANNEL_DYNAMICS_TRESHOLD, CHANNEL_PHANTOM, CHANNEL_NAME, CHANNEL_COLOR, CHANNEL_COLOR_INVERTED }, binding_parameter->mp_index) ||
								config->HasParametersChanged({MP_CAT::CHANNEL_EQ}, binding_parameter->mp_index) || 
								config->HasParameterChanged(CHANNEL_LCD_MODE)
							)
							{
								hasChanged = true;
							}
							break;
					}
					break;
				case MixerparameterAction::LCD_Artnet:
					hasChanged = config->HasParameterChanged(DMX_ARTNET_VALUE, binding_parameter->mp_index);
					break;
				case MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL:
				case MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL:
					hasChanged |= config->HasParameterChanged((MP_ID)binding_parameter->mp_index);
					// intentionally no break!
				case MixerparameterAction::SET_SELECTED_CHANNEL:
				case MixerparameterAction::TOGGLE_SELECTED_CHANNEL:
				case MixerparameterAction::CHANGE_SELECTED_CHANNEL:
					hasChanged |= config->HasParameterChanged(SELECTED_CHANNEL);
					// intentionally no break!
				default:
					hasChanged |= config->HasParameterChanged(parameter_id, parameter_index);
					break;
			}
		}
		
		/*
		 Check if the binding has changed		
		*/

		if(config->HasSurfaceBindingChanged(element_id))
		{
			hasChanged = true;
			config->RemoveSurfaceBindingChanged(element_id);
		}

		/*
		 Now sync the Surfaceelements
		*/

		if (hasChanged && config->HasSurfaceElement(element_id))
		{
			SurfaceElement* element = config->GetSurfaceElement(element_id);

			helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Caluclated Mixerparamter: %s Index: %d", config->GetParameter(parameter_id)->GetName().c_str(), parameter_index);

			// ####################################################
			//
			//                      FADER
			//
			// ####################################################
			if (element->element_type == SurfaceElementType::Fader)
			{
				if (parameter_id == NONE)
				{
					continue;
				}

				u_int16_t faderPosition = 0;

				if (parameter_id == MP_ID::DMX_ARTNET_VALUE)
				{
					faderPosition = helper->DMX2Fadervalue(config->GetUint(parameter_id, parameter_index));
				}
				else 
				{
					faderPosition = helper->Dbfs2Fader(config->GetFloat(parameter_id, parameter_index));
				}

				surface->SetFader(element->GetBoard(), element->GetIndex(), faderPosition);
			}
			// ####################################################
			//
			//                   BUTTON / LED
			//
			// ####################################################
			else if (element->element_type == SurfaceElementType::Button || element->element_type == SurfaceElementType::Led)
			{
				if (parameter_id == NONE)
				{
					continue;
				}

				bool ledOn = false;
				bool ledBlink = false;

				if (binding_parameter->mp_action == MixerparameterAction::CLEAR_SOLO)
				{
					ledBlink = mixer->IsSoloActivated();
				}
				if (binding_parameter->mp_action == MixerparameterAction::SET_TO_INDEX)
				{
					ledOn = config->GetInt(parameter_id, parameter_index) == binding_parameter->mp_index;
				}
				else if (binding_parameter->mp_action == MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL)
				{
					ledOn = config->GetInt(parameter_id, parameter_index) == binding_parameter->extra_value;
				}
				else
				{
					ledOn = config->GetBool(parameter_id, parameter_index);
				}

				// let the LED blink (if the Mixerparamter says it to do)
				if (ledOn && config->GetParameter(parameter_id)->GetBlink())
				{
					ledBlink = ledOn;
				}				

				surface->SetLed(element_id, ledOn, ledBlink);
			}
			// ####################################################
			//
			//                     ENCODER
			//
			// ####################################################
			else if (element->element_type == SurfaceElementType::Encoder)
			{
				if (parameter_id == NONE)
				{
					continue;
				}

				switch(config->GetParameter(parameter_id)->GetUOM())
				{
					case MP_UOM::HZ:
						{
							float max = config->GetParameter(parameter_id)->GetMax();
							float min = config->GetParameter(parameter_id)->GetMin();
							float max_zerobased = (max-min);

							float wert = config->GetFloat(parameter_id, parameter_index);

				            uint position = (uint)((13+1)-std::pow(13, (1.0 - (wert / max_zerobased))));
                    
                			surface->SetEncoderRing(element->GetBoard(), element->GetIndex(), 6, position, 1);
						}
						break;
					case MP_UOM::PANORAMA:
						surface->SetEncoderRing(element->GetBoard(), element->GetIndex(), 2, (config->GetFloat(parameter_id, parameter_index) + 100.0f)/2.0f, 1);
						break;
					default:
						surface->SetEncoderRing(element->GetBoard(), element->GetIndex(), 0, config->GetPercent(parameter_id, parameter_index), 1);
				}
			}
			// ####################################################
			//
			//                       LCD
			//
			// ####################################################
			else if (element->element_type == SurfaceElementType::Lcd)
			{
				if(!state->surface_disable_lcd_update)
				{
					if (binding_parameter->mp_action == MixerparameterAction::LCD_Channel)
					{
						SetLcdFromChannel(element->GetBoard(), element->GetIndex(), parameter_index);
					}
					#if ENABLE_ARTNET
					else if (binding_parameter->mp_action == MixerparameterAction::LCD_Artnet)
					{
						SetLcdFromArtnet(element->GetBoard(), element->GetIndex(), parameter_index);
					}
					#endif
				}
			}
		}
	}
}


	


	// if (config->IsModelX32Core()) {

	// 	if (config->HasParameterChanged(SELECTED_CHANNEL)){
	// 		setLedChannelIndicator_Core();
	// 	}

	// 	if (!state->x32core_lcdmode_setup && (
	// 			config->HasParametersChanged({CHANNEL_SOLO,CHANNEL_MUTE,CHANNEL_COLOR,CHANNEL_NAME}, chanIndex) ||
	// 			config->HasParameterChanged(SELECTED_CHANNEL)
	// 		)
	// 	)
	// 	{
	// 		SetLcdFromChannel(X32_BOARD_MAIN, 0, chanIndex);
	// 	}

	// 	// Volume
	// 	if (config->HasParametersChanged({CHANNEL_VOLUME, CHANNEL_MUTE}, chanIndex))
	// 	{
	// 		surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 0, config->GetFloat(CHANNEL_VOLUME, chanIndex), config->GetBool(CHANNEL_MUTE, chanIndex), 1);
	// 	}

	// 	// Main Channel
	// 	if (config->HasParametersChanged({CHANNEL_VOLUME, CHANNEL_MUTE}, (uint)X32_VCHANNEL_BLOCK::MAIN))
	// 	{
	// 		surface->SetEncoderRingDbfs(1, 1, config->GetFloat(CHANNEL_VOLUME, (uint)X32_VCHANNEL_BLOCK::MAIN), config->GetBool(CHANNEL_MUTE, (uint)X32_VCHANNEL_BLOCK::MAIN), 1);
	// 	}
	// }

#if ENABLE_ARTNET

void X32Ctrl::SetLcdFromArtnet(uint8_t p_boardId, uint8_t lcdIndex, uint8_t artnetIndex)
{
	using enum MP_ID;

    LcdData* data = new LcdData();
	uint textcount = 0;
	uint textIndex = 0;

	data->boardId = p_boardId;
	data->color = (uint)X32_COLOR::YELLOW;
	data->lcdIndex = lcdIndex;
	data->icon.icon = 0;
	data->icon.x = 0;
	data->icon.y = 0;

	// Channel Name
	data->texts[textIndex].text = "DMX" + String(artnetIndex + 1);
	data->texts[textIndex].size = 0x20;
	data->texts[textIndex].x = 0;
	data->texts[textIndex].y = 0;

	textIndex++;

	// Value
	//float value = artnet->GetValue(artnetIndex);
	float value = config->GetFloat(DMX_ARTNET_VALUE, artnetIndex);
	data->texts[textIndex].text = String(value, 0) + " / " + String(value/2.55f, 1) + "%";
	data->texts[textIndex].size = 0;
	data->texts[textIndex].x = 0;
	data->texts[textIndex].y = 20;

	textIndex++;

	// Value
	if (value < 32){
		data->texts[textIndex].text = "";
	} else if (value < 64){
		data->texts[textIndex].text = "O";
	} else if (value < 96){
		data->texts[textIndex].text = "OO";
	} else if (value < 128){
		data->texts[textIndex].text = "OOO";
	} else if (value < 160){
		data->texts[textIndex].text = "OOOO";
	} else if (value < 192){
		data->texts[textIndex].text = "OOOOO";
	} else if (value < 224){
		data->texts[textIndex].text = "OOOOOO";
	} else if (value < 240){
		data->texts[textIndex].text = "OOOOOOO";
	} else {
		data->texts[textIndex].text = "OOOOOOOO";
	}
	data->texts[textIndex].size = 0;
	data->texts[textIndex].x = 0;
	data->texts[textIndex].y = 51;

	textcount = textIndex + 1;

	surface->SetLcdX(data, textcount);
	delete data;
}

#endif

void X32Ctrl::SetLcdFromChannel(uint8_t p_boardId, uint8_t lcdIndex, uint8_t channelIndex)
{
	using enum MP_ID;

    LcdData* data = new LcdData();
	uint textcount = 0;
	
	switch(config->GetUint(CHANNEL_LCD_MODE))
	{
		case 0:
			{
				uint textIndex = 0;

				data->boardId = p_boardId;
				data->color = config->GetUint(CHANNEL_COLOR, channelIndex) | (config->GetUint(CHANNEL_COLOR_INVERTED, channelIndex) * SURFACE_COLOR_INVERTED);
				data->lcdIndex = lcdIndex;
				data->icon.icon = 0;
				data->icon.x = 0;
				data->icon.y = 0;

				// Volume / Panorama

				float balance = config->GetFloat(CHANNEL_PANORAMA, channelIndex);
				
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
				data->texts[textIndex].text = balanceText;    
				data->texts[textIndex].size = 0;
				data->texts[textIndex].x = 0;
				data->texts[textIndex].y = 0;

				textIndex++;

				// Channel Name
				data->texts[textIndex].text = config->GetString(CHANNEL_NAME, channelIndex);
				data->texts[textIndex].size = 0x20;
				data->texts[textIndex].x = 0;
				data->texts[textIndex].y = 20;

				textIndex++;

				// Channel Internal Name
				data->texts[textIndex].text = config->GetString(CHANNEL_NAME_INTERN, channelIndex);
				data->texts[textIndex].size = 0;
				data->texts[textIndex].x = 35;
				data->texts[textIndex].y = 51;

				textcount = textIndex + 1;
			}
			break;
		case 1:
			{
				data->boardId = p_boardId;
				data->color = config->GetUint(CHANNEL_COLOR, channelIndex) | (config->GetUint(CHANNEL_COLOR_INVERTED, channelIndex) * SURFACE_COLOR_INVERTED );
				data->lcdIndex = lcdIndex;
				data->icon.icon = 0;
				data->icon.x = 0;
				data->icon.y = 0;

				// Gain / Lowcut
				data->texts[0].text = String(config->GetFloat(CHANNEL_GAIN, channelIndex), 1) + String("dB ") + String(config->GetFloat(CHANNEL_LOWCUT_FREQ, channelIndex), 0) + String("Hz");
				data->texts[0].size = 0;
				data->texts[0].x = 3;
				data->texts[0].y = 0;

				// Phanton / Invert / Gate / Dynamics / EQ active
				data->texts[1].text =
					String(config->GetBool(CHANNEL_PHANTOM, channelIndex) ? "48V " : "    ") +
					String(config->GetBool(CHANNEL_PHASE_INVERT, channelIndex) ? "@ " : "  ") +
					String(config->GetFloat(CHANNEL_GATE_TRESHOLD, channelIndex) > -80.0f ? "G " : "  ") +
					String(config->GetFloat(CHANNEL_DYNAMICS_TRESHOLD, channelIndex) < 0.0f ? "D " : "  ") +
					// TODO String(mixer->GetEq(channelIndex) ? "E " : "  ");
					String(true ? "E" : " ");
				data->texts[1].size = 0;
				data->texts[1].x = 10;
				data->texts[1].y = 15;

				// Volume / Panorama

				float balance = config->GetFloat(CHANNEL_PANORAMA, channelIndex);
				
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

				float volume = config->GetFloat(CHANNEL_VOLUME, channelIndex);
				if (volume > -100) {
					data->texts[2].text = String(volume, 1) + String("dB ") + String(balanceText);
				}else{
					data->texts[2].text = String("-oodB ") + String(balanceText);
				}
				data->texts[2].size = 0;
				data->texts[2].x = 8;
				data->texts[2].y = 30;

				// vChannel Name
				data->texts[3].text = config->GetString(CHANNEL_NAME, channelIndex);
				data->texts[3].size = 0;
				data->texts[3].x = 0;
				data->texts[3].y = 48;

				textcount = 4;
			}
			break;
	}

	surface->SetLcdX(data, textcount);
	delete data;
}

// Update all meters (Gui, Surface, xremote)
void X32Ctrl::UpdateMeters(void) {

	if (state->surface_disable_meter_update)
	{
		return;
	}

	//xremote->UpdateMeter(mixer);

	// ########################################
	//
	//		GUI Meters
	//
	// ########################################

	if (config->IsModelX32FullOrCompactOrProducerOrRack())
	{
		pages[(X32_PAGE)config->GetUint(ACTIVE_PAGE)]->UpdateMeters();
	}

	// ########################################
	//
	//		Surface Meters
	//
	// ########################################

	uint8_t chanIdx = config->GetUint(SELECTED_CHANNEL); //config->selectedVChannel;

	if (config->IsModelX32Core()) {
		// selected channel
		surface->SetMeterLed(X32_BOARD_MAIN, 0, mixer->dsp->rChannel[chanIdx].meter8Info);
	}

	if (config->IsModelX32Rack()) {
		surface->SetMeterLedMain_Rack(
			mixer->dsp->rChannel[chanIdx].meter8Info, 	// selected channel
			mixer->dsp->MainChannelLR.meterInfo[0],
		 	mixer->dsp->MainChannelLR.meterInfo[1],
		 	mixer->dsp->MainChannelSub.meterInfo[0]
		);
	}

	if (config->IsModelX32Producer()) {
		surface->SetMeterLedMain_Producer(
			mixer->dsp->rChannel[chanIdx].meter8Info, 	// selected channel
			surfaceCalcDynamicMeter(chanIdx),			// selected channel
			mixer->dsp->MainChannelLR.meterInfo[0],
			mixer->dsp->MainChannelLR.meterInfo[1],
			mixer->dsp->MainChannelSub.meterInfo[0]
		);
	}

	if (config->IsModelX32FullOrCompact()) {
		surface->SetMeterLedMain_FullOrCompact(
			mixer->dsp->rChannel[chanIdx].meter8Info,	// selected channel
			surfaceCalcDynamicMeter(chanIdx),			// selected channel
			mixer->dsp->MainChannelLR.meterInfo[0],
			mixer->dsp->MainChannelLR.meterInfo[1],
			mixer->dsp->MainChannelSub.meterInfo[0]
		);
	}

	
	// ########################################
	//
	//		Channels
	//
	// ########################################

	if (config->IsModelX32FullOrCompactOrProducer())
	{
		for (uint8_t i = 0; i < 8; i++)
		{
			SurfaceBindingParameter* binding_board_l = config->GetSurfaceBinding((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_VUMETER_1 + i));
			if (binding_board_l)
			{
				surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[binding_board_l->mp_index].meter6Info);
			}

			if (config->IsModelX32Full())
			{
				SurfaceBindingParameter* binding_board_m = config->GetSurfaceBinding((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_VUMETER_1 + i));
				if (binding_board_m)
				{
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->rChannel[binding_board_m->mp_index].meter6Info);
				}
			}

			SurfaceBindingParameter* binding_board_r = config->GetSurfaceBinding((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_VUMETER_1 + i));
			if (binding_board_r)
			{
				surface->SetMeterLed(X32_BOARD_R, i, mixer->dsp->rChannel[binding_board_r->mp_index].meter6Info);
			}
		}
	}
}


// only X32 Rack
void X32Ctrl::setLedChannelIndicator_Rack(void)
{
	uint chanIdx = config->GetUint(SELECTED_CHANNEL);
	surface->SetLed(SurfaceElementId::LED_IN, (chanIdx <= 31));
	surface->SetLed(SurfaceElementId::LED_AUX_FX, (chanIdx >= 32)&&(chanIdx <= 47));
	surface->SetLed(SurfaceElementId::LED_BUS, (chanIdx >= 48)&&(chanIdx <= 63));
	surface->SetLed(SurfaceElementId::LED_MATRIX, (chanIdx >= 64)&&(chanIdx <= 69));
	surface->SetLed(SurfaceElementId::LED_MAIN, (chanIdx >= 70)&&(chanIdx <= 71));
	surface->SetLed(SurfaceElementId::LED_DCA, (chanIdx >= 72)&&(chanIdx <= 79));
	surface->SetLed(SurfaceElementId::LED_MAIN, (chanIdx == 80));

	// set 7-Segment Display
	surface->SetX32RackDisplay(chanIdx);        
}

// only X32 Core
void X32Ctrl::setLedChannelIndicator_Core(void){
		// uint8_t chanIdx = config->GetUint(SELECTED_CHANNEL);
		// surface->SetLedByEnum(X32_LED_IN, (chanIdx <= 31));
		// surface->SetLedByEnum(X32_LED_AUX, (chanIdx >= 32)&&(chanIdx <= 47));
		// surface->SetLedByEnum(X32_LED_BUS, (chanIdx >= 48)&&(chanIdx <= 63));
		// surface->SetLedByEnum(X32_LED_DCA, (chanIdx >= 64)&&(chanIdx <= 69));
		// surface->SetLedByEnum(X32_LED_MTX, (chanIdx >= 70)&&(chanIdx <= 79));
}

uint8_t X32Ctrl::surfaceCalcDynamicMeter(uint8_t channel) {
	// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
	if (channel < 40) {
		uint32_t meterdata = 0;

		//if (!!RECEIVED_CHANNEL_COMPRESSOR!! < 1.0f) { meterdata |= 0b10000000; };

/*
		float gateValue = (1.0f - !!RECEIVED_CHANNEL_GAIN!!) * 80.0f;
		if (gateValue >= 2.0f)  { meterdata |= 0b00100000; }        
		if (gateValue >= 4.0f)  { meterdata |= 0b00010000; }        
		if (gateValue >= 6.0f)  { meterdata |= 0b00001000; }        
		if (gateValue >= 10.0f) { meterdata |= 0b00000100; }        
		if (gateValue >= 18.0f) { meterdata |= 0b00000010; }        
		if (gateValue >= 30.0f) { meterdata |= 0b00000001; }        

		if (!!RECEIVED_CHANNEL_GAIN!! < 1.0f) { meterdata |= 0b01000000; };
*/
		return meterdata;
	}else{
		return 0; // no dynamic-data for other channels at the moment
	}
}

// sync mixer state to GUI
void X32Ctrl::syncXRemote(bool syncAll) {
	bool fullSync = false;

	if (syncAll || config->HasParameterChanged(SELECTED_CHANNEL)){ 
		// channel selection has changed - do a full sync
		fullSync=true; 
	}
	
	// DEBUG
	xremote->SetCard(10); // X-LIVE

	for(uint8_t i=0; i<(uint)X32_VCHANNELTYPE::NORMAL; i++) {
		//uint8_t chanindex = i;
		//VChannel* chan = mixer->GetVChannel(i);
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
		// 	xremote->SetFader(String("ch"), chanindex, mixer->GetVolumeOscvalue(chanindex));
		// }
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

	for(uint8_t i=(uint)X32_VCHANNEL_BLOCK::AUX; i<(uint)X32_VCHANNELTYPE::AUX; i++) {
		//uint8_t chanindex = i;
		//VChannel* chan = mixer->GetVChannel(i);
		// if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
		// 	xremote->SetFader(String("auxin"), chanindex, mixer->GetVolumeOscvalue(chanindex));
		// }
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
}

// ####################################################################
// #
// #
// #        channel Assigment
// #
// #
// ####################################################################

// direction - positive or negative integer value
void X32Ctrl::ChangeSelect(int8_t direction)
{
	int16_t newSelectedVChannel = config->GetUint(SELECTED_CHANNEL) + direction;
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "ChangeSelect(): selected channel index: %d, direction: %d, new channel index: %d", config->GetUint(SELECTED_CHANNEL), direction, newSelectedVChannel);
	if (newSelectedVChannel < 0) {
		newSelectedVChannel = MAX_VCHANNELS -1;
	} else if (newSelectedVChannel >= MAX_VCHANNELS){
		newSelectedVChannel = 0;
	}

	config->Set(SELECTED_CHANNEL, newSelectedVChannel);
}


//##############################################################################################################################
//
//  ######  ##     ## ########  ########    ###     ######  ########      ######## ##     ## ######## ##    ## ########  ######  
// ##    ## ##     ## ##     ## ##         ## ##   ##    ## ##            ##       ##     ## ##       ###   ##    ##    ##    ## 
// ##       ##     ## ##     ## ##        ##   ##  ##       ##            ##       ##     ## ##       ####  ##    ##    ##       
//  ######  ##     ## ########  ######   ##     ## ##       ######        ######   ##     ## ######   ## ## ##    ##     ######  
//       ## ##     ## ##   ##   ##       ######### ##       ##            ##        ##   ##  ##       ##  ####    ##          ## 
// ##    ## ##     ## ##    ##  ##       ##     ## ##    ## ##            ##         ## ##   ##       ##   ###    ##    ##    ## 
//  ######   #######  ##     ## ##       ##     ##  ######  ########      ########    ###    ######## ##    ##    ##     ###### 
//
//############################################################################################################################## 

void X32Ctrl::ProcessUartDataSurface() {
    uint8_t receivedClass = 0;
    uint8_t receivedIndex = 0;
    uint16_t receivedValue = 0;
    bool lastPackageIncomplete = false;

    int bytesToProcess = surface->uart->Rx(&surfaceBufferUart[0], sizeof(surfaceBufferUart));

    if (bytesToProcess <= 0) {
        return;
	}

    // first init package buffer with 0x00s
    for (uint8_t package=0; package<SURFACE_MAX_PACKET_LENGTH;package++){
        // start at surfacePacketCurrentIndex to not overwrite saved data from last incomplete package
        for (int i = surfacePacketCurrentIndex; i < 6; i++) {
            surfacePacketBuffer[package][i]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }



    if (helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE)) {
        printf("DEBUG_SURFACE: ");

        // print received values on one row
        bool divide_after_next_dbg = false;
        for (int i = 0; i < bytesToProcess; i++) {
            if (divide_after_next_dbg && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
                printf("| ");
                divide_after_next_dbg = false;
            }
            printf("%02X ", (uint8_t)surfaceBufferUart[i]); // empfangene Bytes als HEX-Wert ausgeben
            if (divide_after_next_dbg){
                printf("| ");
                divide_after_next_dbg = false;
            } 
            if ((uint8_t)surfaceBufferUart[i] == 0xFE) {
                divide_after_next_dbg=true;
            }
        }
        printf("\n");
    }

    // break up received data into packages
    bool divide_after_next = false;
    for (int i = 0; i < bytesToProcess; i++) {

        if (divide_after_next && ((uint8_t)surfaceBufferUart[i] == 0xFE)) {
            // previous package had no checksum
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++] = (uint8_t)surfaceBufferUart[i];

        if (divide_after_next) {
            surfacePacketCurrent++;
            surfacePacketCurrentIndex=0;
            divide_after_next = false;
        }

        // use 0xFE as package divider
        if (((uint8_t)surfaceBufferUart[i] == 0xFE))
        {
            divide_after_next = true;
        }
    }

    if (divide_after_next){
        // divide_after_next got no usage, because the uartBuffer was emptied out -> reason: no checksum was send
        // clean up this situation
        surfacePacketCurrent++;
        while (surfacePacketCurrentIndex < 6){  
            // fill with zero - maybe not needed
            surfacePacketBuffer[surfacePacketCurrent][surfacePacketCurrentIndex++]=0x00;
        }
        surfacePacketCurrentIndex=0;
    }

    if (
        (surfacePacketCurrentIndex!=0) &&
        !((surfacePacketBuffer[surfacePacketCurrent][3]==0xFE) | (surfacePacketBuffer[surfacePacketCurrent][4]==0xFE))
    ){
        // last package was incomplete, save it for next run
        /*
            Example1:                                  _____ incomplete, has no 0xFE (and is too short)
                                                      /  
            this run         66 01 FB 00 FE 12 | 66 02

            next run         46 02 FE 44 | 66 03 D6 02 FE 33 | 66 04 73 02 FE 15 | 66 05 4E 03 FE 38 | 66 06 21 02 FE 65 |
                             \
                              \____ take the bytes from the last incomplete package and glue it together


            Example2:                                        _____ incomplete, has no 0xFE
                                                            / 
            this run         66 05 EF 0E FE 0C | 66 06 52 0D

            next run         FE 29 | 66 07 C2 0C FE 39
                             \
                              \____ take the bytes from the last incomplete package and glue it together
            
        */

        helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "surfacePacketCurrent=%d seems incomplete? surfacePacketCurrentIndex=%d", surfacePacketCurrent, surfacePacketCurrentIndex);
        lastPackageIncomplete = true;
    }


    if (helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE)) {
        printf("DEBUG_SURFACE: ");
        
        // print packages, one in a row    
        uint8_t packagesToPrint = surfacePacketCurrent;
        if (lastPackageIncomplete){
            packagesToPrint++;
        }
        printf("surfacePacketCurrent=%d\n", surfacePacketCurrent);

        for (int package=0; package < packagesToPrint; package++) {
            printf("surfaceProcessUartData(): Package %d: ", package);
            for (uint8_t i = 0; i<6; i++){
                printf("%02X ", (uint8_t)surfacePacketBuffer[package][i]);
            }
            if (surfacePacketBuffer[package][0] == 0xFE){
                printf("  <--- Board %d", surfacePacketBuffer[package][1] & 0x7F);
            } else if (lastPackageIncomplete){
                printf("  <--- incomplete, saved for next run");
            }
            printf("\n");
        } 
    }   


    for (int8_t package=0; package < surfacePacketCurrent;package++){

        if (surfacePacketBuffer[package][0] == 0xFE){
            // received BoardId
            uint8_t receivedBoardIdtemp = surfacePacketBuffer[package][1] & 0x7F;
            switch(receivedBoardIdtemp){
                case 0:
                case 1:
                case 4:
                case 5:
                case 8:
                    receivedBoardId = receivedBoardIdtemp;
                    break;
            }
        } else
		{   
			receivedClass = surfacePacketBuffer[package][0];
            receivedIndex = surfacePacketBuffer[package][1];
            
			if ((uint8_t)(surfacePacketBuffer[package][3]) == 0xFE)
			{
                // short package - uint8_t !!

                receivedValue = (uint16_t)surfacePacketBuffer[package][2];
                
				// TODO: Check checksum
				//receivedChecksum = surfacePacketBuffer[package][4];
            }
			else if ((uint8_t)(surfacePacketBuffer[package][4]) == 0xFE)
			{
                // long package - uint16_t !!
				// for example: fader value

                receivedValue = ((uint16_t)surfacePacketBuffer[package][3] << 8) | (uint16_t)surfacePacketBuffer[package][2];
                
				// TODO: Check checksum
				//receivedChecksum = surfacePacketBuffer[package][5];
            }
        

            // only process valid packages
            bool valid = true;

            switch (receivedClass){
                case 'f':
                case 'b':
                case 'e':
                    break;
                default:
                    valid = false;
                    break;
            }       

            if (valid)
			{
				helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "Callback: BoardId 0x%02X, Class 0x%02X, Index 0x%02X, Value 0x%04X", receivedBoardId, receivedClass, receivedIndex, receivedValue);
				
				// "SurfaceManager"
				ProcessSurface((X32_BOARD)receivedBoardId, receivedClass, receivedIndex, receivedValue);

				// old
				//ProcessSurfaceEventsRaw(new SurfaceEvent((X32_BOARD)receivedBoardId, receivedClass, receivedIndex, receivedValue));
            } 
        }
    }

    // all packages are processed
    // now clean up for next run

    if (lastPackageIncomplete){
        // copy last incomplete package to package0 for next run
        for (uint8_t i=0; i < surfacePacketCurrentIndex; i++){
            surfacePacketBuffer[0][i] = surfacePacketBuffer[surfacePacketCurrent][i];
        }

        // reset index for next run
        lastPackageIncomplete=false;
        surfacePacketCurrent=0;
        // do NOT touch surfacePacketCurrentIndex!
    }else {
        // reset index for next run
        surfacePacketCurrent=0;
        surfacePacketCurrentIndex=0;
    }
}

//####################################################################
//
//  ######  ##     ## ########  ########    ###     ######  ######## 
// ##    ## ##     ## ##     ## ##         ## ##   ##    ## ##       
// ##       ##     ## ##     ## ##        ##   ##  ##       ##       
//  ######  ##     ## ########  ######   ##     ## ##       ######   
//       ## ##     ## ##   ##   ##       ######### ##       ##       
// ##    ## ##     ## ##    ##  ##       ##     ## ##    ## ##       
//  ######   #######  ##     ## ##       ##     ##  ######  ######## 
//
//
// ########  #### ##    ## ########  #### ##    ##  ######   
// ##     ##  ##  ###   ## ##     ##  ##  ###   ## ##    ##  
// ##     ##  ##  ####  ## ##     ##  ##  ####  ## ##        
// ########   ##  ## ## ## ##     ##  ##  ## ## ## ##   #### 
// ##     ##  ##  ##  #### ##     ##  ##  ##  #### ##    ##  
// ##     ##  ##  ##   ### ##     ##  ##  ##   ### ##    ##  
// ########  #### ##    ## ########  #### ##    ##  ######   
//
//####################################################################



// Bind Surfaceelements to Mixerparameter or special functions
void X32Ctrl::LoadDefaultSurfaceBinding()
{
	// Display
	config->SurfaceBind(SurfaceElementId::HOME, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::HOME));
	config->SurfaceBind(SurfaceElementId::METERS, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::METERS));
	config->SurfaceBind(SurfaceElementId::ROUTING, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::ROUTING));
	config->SurfaceBind(SurfaceElementId::SETUP, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::SETUP));
	config->SurfaceBind(SurfaceElementId::LIBRARY, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::LIBRARY));
	config->SurfaceBind(SurfaceElementId::EFFECTS, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::EFFECTS));
	config->SurfaceBind(SurfaceElementId::MUTE_GRP, MixerparameterAction::TOGGLE, DISPLAY_MUTE_GROUP);
	config->SurfaceBind(SurfaceElementId::UTILITY, MixerparameterAction::TOGGLE, DISPLAY_UTILITY);

	config->SurfaceBind(SurfaceElementId::LEFT, MixerparameterAction::TOGGLE, DISPLAY_LEFT);
	config->SurfaceBind(SurfaceElementId::RIGHT, MixerparameterAction::TOGGLE, DISPLAY_RIGHT);
	config->SurfaceBind(SurfaceElementId::UP, MixerparameterAction::TOGGLE, DISPLAY_UP);
	config->SurfaceBind(SurfaceElementId::DOWN, MixerparameterAction::TOGGLE, DISPLAY_DOWN);

	if (config->IsModelX32FullOrCompactOrProducer())
	{
		// Config / Preamp
		config->SurfaceBind(SurfaceElementId::GAIN_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_GAIN);
		config->SurfaceBind(SurfaceElementId::PHANTOM_48V, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_PHANTOM);
		config->SurfaceBind(SurfaceElementId::PHASE_INVERT, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_PHASE_INVERT);
		config->SurfaceBind(SurfaceElementId::LOW_CUT_FREQ_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_LOWCUT_FREQ);
		config->SurfaceBind(SurfaceElementId::LOW_CUT, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_LOWCUT_ENABLE);
		config->SurfaceBind(SurfaceElementId::VIEW_CONFIG, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::CONFIG));

		// Gate
		config->SurfaceBind(SurfaceElementId::GATE_THRESHOLD_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_GATE_TRESHOLD);
		config->SurfaceBind(SurfaceElementId::GATE, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_GATE_ENABLE);
		config->SurfaceBind(SurfaceElementId::VIEW_GATE, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::GATE));

		// Dynamics
		config->SurfaceBind(SurfaceElementId::DYNAMICS_THRESHOLD_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_DYNAMICS_TRESHOLD);
		config->SurfaceBind(SurfaceElementId::COMP_EXP, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_COMPRESSOR_ENABLE);
		config->SurfaceBind(SurfaceElementId::VIEW_DYNAMICS, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::COMPRESSOR));

		// EQ
		config->SurfaceBind(SurfaceElementId::EQ_HCUT_LED, MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::HICUT);
		config->SurfaceBind(SurfaceElementId::EQ_HSHV_LED, MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::HIGHSHELV);
		//config->SurfaceBind(SurfaceElementId::EQ_VEQ_LED, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::V);
		config->SurfaceBind(SurfaceElementId::EQ_PEQ_LED, MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::PEQ);
		config->SurfaceBind(SurfaceElementId::EQ_LSHV_LED, MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::LOWSHELV);
		config->SurfaceBind(SurfaceElementId::EQ_LCUT_LED, MixerparameterAction::SET__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ, (uint)EQ_TYPE::LOWCUT);
		config->SurfaceBind(SurfaceElementId::EQ_MODE, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_TYPE1, (uint)BANKING_EQ);

		config->SurfaceBind(SurfaceElementId::EQ_Q_ENCODER, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_Q1, (uint)BANKING_EQ);
		config->SurfaceBind(SurfaceElementId::EQ_FREQ_ENCODER, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_FREQ1, (uint)BANKING_EQ);
		config->SurfaceBind(SurfaceElementId::EQ_GAIN_ENCODER, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_EQ_GAIN1, (uint)BANKING_EQ);

		config->SurfaceBind(SurfaceElementId::EQ_LOW, MixerparameterAction::SET_TO_INDEX, BANKING_EQ, 0);
		config->SurfaceBind(SurfaceElementId::EQ_LOW_MID, MixerparameterAction::SET_TO_INDEX, BANKING_EQ, 1);
		config->SurfaceBind(SurfaceElementId::EQ_HIGH_MID, MixerparameterAction::SET_TO_INDEX, BANKING_EQ, 2);
		config->SurfaceBind(SurfaceElementId::EQ_HIGH, MixerparameterAction::SET_TO_INDEX, BANKING_EQ, 3);
		config->SurfaceBind(SurfaceElementId::EQ, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_EQ_ENABLE);
		config->SurfaceBind(SurfaceElementId::VIEW_EQ, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::EQ));

		// Bus Sends
		config->SurfaceBind(SurfaceElementId::VIEW_MIX_BUS_SENDS, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::SENDS));
		if (config->IsModelX32Full())
		{
			config->SurfaceBind(SurfaceElementId::BUS_SEND_ENCODER_1, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_BUS_SEND01, (uint)BANKING_BUS_SENDS, 4);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_ENCODER_2, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_BUS_SEND02, (uint)BANKING_BUS_SENDS, 4);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_ENCODER_3, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_BUS_SEND03, (uint)BANKING_BUS_SENDS, 4);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_ENCODER_4, MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL, CHANNEL_BUS_SEND04, (uint)BANKING_BUS_SENDS, 4);

			config->SurfaceBind(SurfaceElementId::BUS_SEND_1_4, MixerparameterAction::SET_TO_INDEX, BANKING_BUS_SENDS, 0);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_5_8, MixerparameterAction::SET_TO_INDEX, BANKING_BUS_SENDS, 1);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_9_12, MixerparameterAction::SET_TO_INDEX, BANKING_BUS_SENDS, 2);
			config->SurfaceBind(SurfaceElementId::BUS_SEND_13_16, MixerparameterAction::SET_TO_INDEX, BANKING_BUS_SENDS, 3);
		}
		
		// Bus Mixes
		config->SurfaceBind(SurfaceElementId::MAIN_BUS_LEVEL_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_VOLUME_SUB);
		config->SurfaceBind(SurfaceElementId::MONO_BUS, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SEND_SUB);
		config->SurfaceBind(SurfaceElementId::PAN_BAL_ENCODER, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_PANORAMA);
		config->SurfaceBind(SurfaceElementId::MAIN_LR_BUS, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SEND_LR);
		config->SurfaceBind(SurfaceElementId::VIEW_MAIN, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::MAIN));
		
		// Scenes
		config->SurfaceBind(SurfaceElementId::VIEW_SCENES, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::SCENES));
		//SurfaceBind_MixerParameter(SurfaceElementId::VIEW_ASSIGN, SurfaceBindingAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::CONFIG));

		// Assign
		config->SurfaceBind(SurfaceElementId::ASSIGN_A, MixerparameterAction::SET_TO_INDEX, BANKING_ASSIGN, 0);
		config->SurfaceBind(SurfaceElementId::ASSIGN_B, MixerparameterAction::SET_TO_INDEX, BANKING_ASSIGN, 1);
		config->SurfaceBind(SurfaceElementId::ASSIGN_C, MixerparameterAction::SET_TO_INDEX, BANKING_ASSIGN, 2);
		config->SurfaceBind(SurfaceElementId::VIEW_ASSIGN, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::SETUP_SURFACE));

		// Remote
		config->SurfaceBind(SurfaceElementId::DAW_REMOTE, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::REMOTE1));

		// Banking of Input Section
		if (config->IsModelX32Full())
		{
			config->SurfaceBind(SurfaceElementId::CH1_16, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH1_16));
			config->SurfaceBind(SurfaceElementId::CH17_32, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH17_32));
			config->SurfaceBind(SurfaceElementId::AUX_USB_RX_RET, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::AUX_USB_FX_RET));
			config->SurfaceBind(SurfaceElementId::BUS_MASTER, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::BUS1_16));
		}
		else if (config->IsModelX32CompactOrProducer())
		{
			config->SurfaceBind(SurfaceElementId::CH1_8, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH1_8));
			config->SurfaceBind(SurfaceElementId::CH9_16, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH9_16));
			config->SurfaceBind(SurfaceElementId::CH17_24, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH17_24));
			config->SurfaceBind(SurfaceElementId::CH25_32, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::CH25_32));
			config->SurfaceBind(SurfaceElementId::AUX_USB, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::AUX_USB));
			config->SurfaceBind(SurfaceElementId::FX_RET, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::FX_RET));
			config->SurfaceBind(SurfaceElementId::BUS1_8_MASTER, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::BUS1_8));
			config->SurfaceBind(SurfaceElementId::BUS9_16_MASTER, MixerparameterAction::SET_TO_INDEX, BANKING_INPUT, (uint)(X32BankId::BUS9_16));
		}

		// Banking of Bus Section
		config->SurfaceBind(SurfaceElementId::DCA, MixerparameterAction::SET_TO_INDEX, BANKING_BUS, (uint)(X32BankId::DCA));
		config->SurfaceBind(SurfaceElementId::BUS1_8, MixerparameterAction::SET_TO_INDEX, BANKING_BUS, (uint)(X32BankId::BUS1_8));
		config->SurfaceBind(SurfaceElementId::BUS9_16, MixerparameterAction::SET_TO_INDEX, BANKING_BUS, (uint)(X32BankId::BUS9_16));
		config->SurfaceBind(SurfaceElementId::MATRIX_MAIN, MixerparameterAction::SET_TO_INDEX, BANKING_BUS, (uint)(X32BankId::MATRIX_MAIN));

        LoadMainFaderSurfaceBinding();

        // Mute Groups
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_1, MixerparameterAction::TOGGLE, MUTE_GROUP_1_MUTE);
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_2, MixerparameterAction::TOGGLE, MUTE_GROUP_2_MUTE);
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_3, MixerparameterAction::TOGGLE, MUTE_GROUP_3_MUTE);
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_4, MixerparameterAction::TOGGLE, MUTE_GROUP_4_MUTE);
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_5, MixerparameterAction::TOGGLE, MUTE_GROUP_5_MUTE);
		config->SurfaceBind(SurfaceElementId::MUTE_GROUP_6, MixerparameterAction::TOGGLE, MUTE_GROUP_6_MUTE);

	}

	if (config->IsModelX32FullOrCompactOrProducerOrRack())
	{
		config->SurfaceBind(SurfaceElementId::CLEAR_SOLO, MixerparameterAction::CLEAR_SOLO, NONE);
	}

	if (config->IsModelX32Rack())
	{
		config->SurfaceBind(SurfaceElementId::VIEW_SCENES, MixerparameterAction::SET_TO_INDEX, ACTIVE_PAGE, (uint)(X32_PAGE::SCENES));

		config->SurfaceBind(SurfaceElementId::CHANNEL_ENCODER, MixerparameterAction::CHANGE, SELECTED_CHANNEL);

		config->SurfaceBind(SurfaceElementId::CHANNEL_SOLO, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_SOLO);
		config->SurfaceBind(SurfaceElementId::CHANNEL_MUTE, MixerparameterAction::TOGGLE_SELECTED_CHANNEL, CHANNEL_MUTE);
		config->SurfaceBind(SurfaceElementId::CHANNEL_LEVEL, MixerparameterAction::CHANGE_SELECTED_CHANNEL, CHANNEL_VOLUME);
		config->SurfaceBind(SurfaceElementId::MAIN_LEVEL, MixerparameterAction::CHANGE, CHANNEL_VOLUME, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
	}
}

void X32Ctrl::LoadMainFaderSurfaceBinding()
{
    // Main Fader
    config->SurfaceBind(SurfaceElementId::BOARD_R_SELECT_MAIN, MixerparameterAction::SET_TO_INDEX, SELECTED_CHANNEL, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
    config->SurfaceBind(SurfaceElementId::BOARD_R_SOLO_MAIN, MixerparameterAction::TOGGLE, CHANNEL_SOLO, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
    config->SurfaceBind(SurfaceElementId::BOARD_R_LCD_MAIN, MixerparameterAction::LCD_Channel, NONE, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
    config->SurfaceBind(SurfaceElementId::BOARD_R_MUTE_MAIN, MixerparameterAction::TOGGLE, CHANNEL_MUTE, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
    config->SurfaceBind(SurfaceElementId::BOARD_R_FADER_MAIN, MixerparameterAction::SET, CHANNEL_VOLUME, to_underlying(X32_VCHANNEL_BLOCK::MAIN));
}

//#####################################################################################################################
//
// ########     ###    ##    ## ##     ## #### ##    ##  ######   
// ##     ##   ## ##   ###   ## ##    ##   ##  ###   ## ##    ##  
// ##     ##  ##   ##  ####  ## ##   ##    ##  ####  ## ##        
// ########  ##     ## ## ## ## #####      ##  ## ## ## ##   #### 
// ##     ## ######### ##  #### ##   ##    ##  ##  #### ##    ##  
// ##     ## ##     ## ##   ### ##    ##   ##  ##   ### ##    ##  
// ########  ##     ## ##    ## ##     ## #### ##    ##  ######   
//
//#####################################################################################################################

void X32Ctrl::InitBanks()
{
	if (config->IsModelX32FullOrCompactOrProducer())
	{
		InitBank_Channelstrip(new X32Bank(X32BankId::CH1_8, "Channel 1-8"), 0);
		InitBank_Channelstrip(new X32Bank(X32BankId::CH9_16, "Channel 9-16"), 8);
		InitBank_Channelstrip(new X32Bank(X32BankId::CH17_24, "Channel 17-24"), 16);
		InitBank_Channelstrip(new X32Bank(X32BankId::CH25_32, "Channel 25-32"), 24);
		InitBank_Channelstrip(new X32Bank(X32BankId::AUX_USB, "AUX/USB"), (uint)(X32_VCHANNEL_BLOCK::AUX));
		InitBank_Channelstrip(new X32Bank(X32BankId::FX_RET, "FX Return"), (uint)(X32_VCHANNEL_BLOCK::FXRET));
		InitBank_Channelstrip(new X32Bank(X32BankId::BUS1_8, "Bus 1-8"), (uint)(X32_VCHANNEL_BLOCK::BUS));
		InitBank_Channelstrip(new X32Bank(X32BankId::BUS9_16, "Bus 9-16"), ((uint)(X32_VCHANNEL_BLOCK::BUS)) + 8);
		InitBank_Channelstrip_DCA(new X32Bank(X32BankId::DCA, "DCA"), (uint)(X32_VCHANNEL_BLOCK::DCA));
		InitBank_Channelstrip(new X32Bank(X32BankId::MATRIX_MAIN, "Matrix/Main"), (uint)(X32_VCHANNEL_BLOCK::MATRIX));
		InitBank_DMX(new X32Bank(X32BankId::REMOTE1, "Remote1"), 0);
		InitBank_DMX(new X32Bank(X32BankId::REMOTE2, "Remote2"), 8);
		InitBank_Flex(new X32Bank(X32BankId::FLEX1, "Flex1"));
		InitBank_Flex(new X32Bank(X32BankId::FLEX2, "Flex2"));
		InitBank_Flex(new X32Bank(X32BankId::FLEX3, "Flex3"));
	}
}

void X32Ctrl::InitBank_Channelstrip(X32Bank* bank, uint offset)
{
    for (uint i = 0; i < 8; i++)
    {
		bank->channelstrip[i] = new X32BankParameter();

        bank->channelstrip[i]->select = new SurfaceBindingParameter(MixerparameterAction::SET_TO_INDEX, SELECTED_CHANNEL, i + offset);
		bank->channelstrip[i]->vumeter = new SurfaceBindingParameter(MixerparameterAction::VUMETER, NONE, i + offset);
        bank->channelstrip[i]->solo = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_SOLO, i + offset);
		bank->channelstrip[i]->lcd = new SurfaceBindingParameter(MixerparameterAction::LCD_Channel, NONE, i + offset);
        bank->channelstrip[i]->mute = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_MUTE, i + offset);
        bank->channelstrip[i]->fader = new SurfaceBindingParameter(MixerparameterAction::SET, CHANNEL_VOLUME, i + offset);
    }

	banks[(uint)(bank->GetID())] = bank;
}

void X32Ctrl::InitBank_Channelstrip_DCA(X32Bank* bank, uint offset)
{
    for (uint i = 0; i < 8; i++)
    {
		bank->channelstrip[i] = new X32BankParameter();

        bank->channelstrip[i]->select = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, config->MpCalcId(DCA_GROUP_1_MASTER, i), 0);
		//bank->channelstrip[i]->vumeter = new SurfaceBindingParameter(MixerparameterAction::VUMETER, NONE, i + offset);
        bank->channelstrip[i]->solo = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_SOLO, i + offset);
		bank->channelstrip[i]->lcd = new SurfaceBindingParameter(MixerparameterAction::LCD_Channel, NONE, i + offset);
        bank->channelstrip[i]->mute = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_MUTE, i + offset);
        bank->channelstrip[i]->fader = new SurfaceBindingParameter(MixerparameterAction::SET, CHANNEL_VOLUME, i + offset);
    }

	banks[(uint)(bank->GetID())] = bank;
}

void X32Ctrl::InitBank_DMX(X32Bank* bank, uint offset)
{
    for (uint i = 0; i < 8; i++)
    {
		bank->channelstrip[i] = new X32BankParameter();

        //bank->channelstrip[i]->select = new SurfaceBindingParameter(MixerparameterAction::SET_TO_INDEX, SELECTED_CHANNEL, i + offset);
		//bank->channelstrip[i]->vumeter = new SurfaceBindingParameter(MixerparameterAction::VUMETER, NONE, i + offset);
        //bank->channelstrip[i]->solo = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_SOLO, i + offset);
		bank->channelstrip[i]->lcd = new SurfaceBindingParameter(MixerparameterAction::LCD_Artnet, DMX_ARTNET_VALUE, i + offset);
        //bank->channelstrip[i]->mute = new SurfaceBindingParameter(MixerparameterAction::TOGGLE, CHANNEL_MUTE, i + offset);
        bank->channelstrip[i]->fader = new SurfaceBindingParameter(MixerparameterAction::DMX, DMX_ARTNET_VALUE, i + offset);
    }

	banks[(uint)(bank->GetID())] = bank;
}

// create a empty bank for flexible use
void X32Ctrl::InitBank_Flex(X32Bank* bank)
{
    for (uint i = 0; i < 8; i++)
    {
		bank->channelstrip[i] = new X32BankParameter();

        bank->channelstrip[i]->select = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
		bank->channelstrip[i]->vumeter = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
        bank->channelstrip[i]->solo = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
		bank->channelstrip[i]->lcd = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
        bank->channelstrip[i]->mute = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
        bank->channelstrip[i]->fader = new SurfaceBindingParameter(MixerparameterAction::NONE, NONE, 0);
    }

	banks[(uint)(bank->GetID())] = bank;
}

void X32Ctrl::LoadBank(X32BankTarget target, X32BankId id)
{
	if (id == X32BankId::None)
	{
		return;
	}

	helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Load Bank %d to section %d", id, target);

	X32Bank* bank_to_load = banks[(uint)id];

	if (target == X32BankTarget::InputSection)
	{
		for (uint i = 0; i < 8; i++)
		{
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_SELECT_1 + i), bank_to_load->channelstrip[i]->select);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_VUMETER_1 + i), bank_to_load->channelstrip[i]->vumeter);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_SOLO_1 + i), bank_to_load->channelstrip[i]->solo);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_LCD_1 + i), bank_to_load->channelstrip[i]->lcd);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_MUTE_1 + i), bank_to_load->channelstrip[i]->mute);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_L_FADER_1 + i), bank_to_load->channelstrip[i]->fader);
		}

		bankLoadedInputsection = bank_to_load;
	}

	if (target == X32BankTarget::InputSection2)
	{
		for (uint i = 0; i < 8; i++)
		{
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_SELECT_1 + i), bank_to_load->channelstrip[i]->select);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_VUMETER_1 + i), bank_to_load->channelstrip[i]->vumeter);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_SOLO_1 + i), bank_to_load->channelstrip[i]->solo);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_LCD_1 + i), bank_to_load->channelstrip[i]->lcd);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_MUTE_1 + i), bank_to_load->channelstrip[i]->mute);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_M_FADER_1 + i), bank_to_load->channelstrip[i]->fader);
		}

		bankLoadedInputsection2 = bank_to_load;
	}

	if (target == X32BankTarget::BusSection)
	{
		for (uint i = 0; i < 8; i++)
		{
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_SELECT_1 + i), bank_to_load->channelstrip[i]->select);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_VUMETER_1 + i), bank_to_load->channelstrip[i]->vumeter);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_SOLO_1 + i), bank_to_load->channelstrip[i]->solo);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_LCD_1 + i), bank_to_load->channelstrip[i]->lcd);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_MUTE_1 + i), bank_to_load->channelstrip[i]->mute);
			config->SurfaceBindParameter((SurfaceElementId)((uint)SurfaceElementId::BOARD_R_FADER_1 + i), bank_to_load->channelstrip[i]->fader);
		}

		bankLoadedBussection = bank_to_load;
	}
}

//#####################################################################################################################
//
//  ######  ##     ## ########  ########    ###     ######  ########      #### ##    ## ########  ##     ## ######## 
// ##    ## ##     ## ##     ## ##         ## ##   ##    ## ##             ##  ###   ## ##     ## ##     ##    ##    
// ##       ##     ## ##     ## ##        ##   ##  ##       ##             ##  ####  ## ##     ## ##     ##    ##    
//  ######  ##     ## ########  ######   ##     ## ##       ######         ##  ## ## ## ########  ##     ##    ##    
//       ## ##     ## ##   ##   ##       ######### ##       ##             ##  ##  #### ##        ##     ##    ##    
// ##    ## ##     ## ##    ##  ##       ##     ## ##    ## ##             ##  ##   ### ##        ##     ##    ##    
//  ######   #######  ##     ## ##       ##     ##  ######  ########      #### ##    ## ##         #######     ##    
//
//#####################################################################################################################


void X32Ctrl::ProcessSurface(X32_BOARD board, uint8_t classid, uint8_t index, uint16_t value)
{
	if (classid == 'f') // Fader
	{
		// find surfaceelement
		SurfaceElement* fader = config->GetSurfaceElementFader(board, index);
		if (fader == 0) { return; }

		SurfaceBindingParameter* bindingParameter = config->GetSurfaceBinding(fader->GetId());
		if (bindingParameter == 0) { return; }
				
		switch (bindingParameter->mp_action)
		{
			case MixerparameterAction::SET:
				config->Set(bindingParameter->mp_id, helper->Fadervalue2dBfs(value), bindingParameter->mp_index);
				surface->FaderMoved((uint)board, index, value);
				break;
			case MixerparameterAction::DMX:
				// a fader is moved

				// // currently we are not using mixer-parameters as we have not enough
				// // mixerchannels for all 512 DMX-channels
				// // so we are using a dummy-mixerparameter to get the changes
				// // and use a manual offeset to write the values to the ArtNet-Output

				// uint16_t offset = config->GetUint(DMX_ARTNET_OFFSET);
				// if (board == X32_BOARD_L) {
				// 	offset += 0;
				// }else if (board == X32_BOARD_M) {
				// 	offset += 8; // only on Fullsize
				// }else if (board == X32_BOARD_R) {
				// 	offset += (config->IsModelX32Full() ? 16 : 8);
				// }
				// artnet->SetChannel(index + offset, (float)value * (255.0f/4095.0f), 0);
				config->Set(bindingParameter->mp_id, helper->Fadervalue2DMX(value), bindingParameter->mp_index);
				surface->FaderMoved((uint)board, index, value);
				break;
		}
	}
	else if (classid == 'b') // Button
	{
		// find surfaceelement
		SurfaceElement* button = config->GetSurfaceElementButton(board, value);
		if (button == 0) { return; }

		SurfaceBindingParameter* bindingParameterButton = config->GetSurfaceBinding(button->GetId());

		bool isButtonPressed = (value >> 7) == 1;

		// Logic for double button press
		if (isButtonPressed) {
			if (buttonPressed == 0) {
				buttonPressed = button;
			} else {
				secondbuttonPressed = button;
				helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "DoubleButtonPress: Button1 \"%s\", Button2 \"%s\"", buttonPressed->GetName().c_str(), secondbuttonPressed->GetName().c_str());
			}
		} else {
			if (buttonPressed == button) {
				buttonPressed = 0;				
			}
			if (secondbuttonPressed == button) {
				secondbuttonPressed = 0;
			}
		}

		helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Button \"%s\" %s",
			button->GetName().c_str(),
			isButtonPressed ? "pressed" : "released"
		);

		
		if (bindingParameterButton != 0 && bindingParameterButton->mp_action != MixerparameterAction::NONE)
		{
			MP_ID parameter_id = config->ParameterCalcId(bindingParameterButton);
			uint parameter_index = config->ParameterCalcIndex(bindingParameterButton);

			Mixerparameter* parameter = config->GetParameter(parameter_id);

			// Member Assign Mode (e.g. Mute Groups, DCA Groups)
			bool memberAssingMode = 
					config->IsModelX32FullOrCompactOrProducer() 		&&
					config->GetBool(parameter->GetAssignMembersIf()) 	&&
					config->GetUint(ACTIVE_PAGE) == (uint)X32_PAGE::CONFIG;

			if (isButtonPressed)
			{
				// Member Assign Mode (e.g. Mute Groups, DCA Groups)
				if (memberAssingMode)
				{
					// Bind Select button to destination Mixerparameter
					for (uint i = 0; i < 8; i++)
					{
						// Board L / InputSection
						uint chanIndex_L = bankLoadedInputsection->channelstrip[i]->select->mp_index;
						config->SurfaceBind(config->CalcSurfaceElementId(SurfaceElementId::BOARD_L_SELECT_1, i),
											MixerparameterAction::TOGGLE, parameter->GetAssignMembersTo(), chanIndex_L);

						if (config->IsModelX32Full())
						{
							// Board M / InputSection2
							uint chanIndex_M = bankLoadedInputsection2->channelstrip[i]->select->mp_index;
							config->SurfaceBind(config->CalcSurfaceElementId(SurfaceElementId::BOARD_M_SELECT_1, i),
												MixerparameterAction::TOGGLE, parameter->GetAssignMembersTo(), chanIndex_M);	
						}
						
						if (config->GetUint(BANKING_BUS) != (uint) X32BankId::DCA)
						{
							// Board R / BusSection
							uint chanIndex_R = bankLoadedBussection->channelstrip[i]->select->mp_index;
							config->SurfaceBind(config->CalcSurfaceElementId(SurfaceElementId::BOARD_R_SELECT_1, i),
												MixerparameterAction::TOGGLE, parameter->GetAssignMembersTo(), chanIndex_R);
						}

						// Master Fader
						config->SurfaceBind(SurfaceElementId::BOARD_R_SELECT_MAIN,
											MixerparameterAction::TOGGLE, parameter->GetAssignMembersTo(), to_underlying(X32_VCHANNEL_BLOCK::MAIN));

					}					
				}
				// Normal Mode
				else
				{
					switch (bindingParameterButton->mp_action)
					{
						case MixerparameterAction::REFRESH:
							config->Refresh(parameter_id, parameter_index);
							break;
						case MixerparameterAction::TOGGLE:
							config->Toggle(parameter_id, parameter_index);
							break;
						case MixerparameterAction::TOGGLE_SELECTED_CHANNEL:
							config->Toggle(parameter_id, parameter_index);
							break;
						case MixerparameterAction::PUSH:
						case MixerparameterAction::SET:
						case MixerparameterAction::SET_SELECTED_CHANNEL:
							config->Set(parameter_id, 1, parameter_index);
							break;
						case MixerparameterAction::SET_TO_INDEX:
							{
								// Set value to the bound index value.
								float value_to_set = bindingParameterButton->mp_index;

								if (secondbuttonPressed != 0)
								{
									// Second button was pressed, while holding the first one

									SurfaceBindingParameter* bindingParameterButtonOne = config->GetSurfaceBinding(buttonPressed->GetId());

									if (config->IsModelX32CompactOrProducer())
									{
										// ######################################
										// Banking input section into bus section
										// ######################################
										if (bindingParameterButtonOne->mp_id == BANKING_INPUT && bindingParameterButton->mp_id == BANKING_INPUT)
										{
											// both buttons belong to input banking --> so load the bank into the bus section
											config->Set(BANKING_BUS, value_to_set, parameter_index);
										}
									} 
									else if (config->IsModelX32Full())
									{
										// TODO https://github.com/OpenMixerProject/OpenX32/issues/61

										// #######################################
										// Banking Channels 17-24 into bus section
										// #######################################
										if (bindingParameterButtonOne->mp_id == BANKING_INPUT && bindingParameterButton->mp_id == BANKING_INPUT)
										{
											// both buttons belong to input banking --> so load the bank into the bus section										
											config->Set(BANKING_BUS, value_to_set, parameter_index);
										}

									}
								}
								else
								{
									config->Set(parameter_id, value_to_set, parameter_index);
								}
							}
							break;
						case MixerparameterAction::CHANGE:
						case MixerparameterAction::CHANGE_SELECTED_CHANNEL:
						case MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL:
							config->Change(parameter_id, 1, parameter_index); // every button press does one stepsize "up"
							break;
						case MixerparameterAction::RESET:
						case MixerparameterAction::RESET_SELECTED_CHANNEL:
							config->Reset(parameter_id, parameter_index);
							break;
						case MixerparameterAction::CLEAR_SOLO:
							mixer->ClearSolo();
							break;
						case MixerparameterAction::CUSTOM:
						{
							Page* curent_page = pages[(X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE)];
							curent_page->ChangeCustomButton(button->GetId());
						}
						case MixerparameterAction::DMX:
							// a button is pressed
							break;
						default:
							break;
					}
				}
			}
			else
			{
				// Button was released	

				// Member Assign Mode (e.g. Mute Groups)
				if (memberAssingMode)
				{
					// Reload current banking
					config->Refresh(BANKING_INPUT);
					config->Refresh(BANKING_BUS);
					LoadMainFaderSurfaceBinding();
				}
				// Normal Mode
				else
				{
					switch (bindingParameterButton->mp_action)
					{
						case MixerparameterAction::PUSH:
							config->Set(parameter_id, 0, parameter_index);
							break;
						default:
							break;
					}
				}
			}
		}
		else
		{
			helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Button is not bound.");
		}
	}
	else if (classid == 'e') // Encoder
	{
		// find encoder
		SurfaceElement* encoder = config->GetSurfaceElementEncoder(board, index);
		if (encoder == 0) { return; }
		int8_t amount = 0;

		if (value > 0 && value < 128)
		{
			amount = (uint8_t)value;
		}
		else if (value > 128 && value < 256) 
		{
			amount = -(256 - (uint8_t)(value));
		}

		helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Encoder \"%s\" turned: \"%d\"",
			encoder->GetName().c_str(),
			amount
		);

		SurfaceBindingParameter* bindingParameterEncoder = config->GetSurfaceBinding(encoder->GetId());

		if (bindingParameterEncoder != 0 && bindingParameterEncoder->mp_action != MixerparameterAction::NONE)
		{
			MP_ID parameter_id = config->ParameterCalcId(bindingParameterEncoder);
			uint parameter_index = config->ParameterCalcIndex(bindingParameterEncoder);

			switch (bindingParameterEncoder->mp_action)
			{
				case MixerparameterAction::CHANGE:
				case MixerparameterAction::CHANGE_SELECTED_CHANNEL:
				case MixerparameterAction::CHANGE__MP_INDIRECT__SELECTED_CHANNEL:
					config->Change(parameter_id, amount, parameter_index);
					break;
				case MixerparameterAction::CUSTOM:
					{
						Page* curent_page = pages[(X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE)];
						curent_page->ChangeCustomEncoder(encoder->GetId(), amount);
					}
					break;
				case MixerparameterAction::DMX:
					// an encoder is turned
					break;
			}
		}
		else
		{
			helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Encoder is not bound.");
		}

	}
	else 
	{
		helper->DEBUG_X32CTRL(DEBUGLEVEL_TRACE, "unknown message: %s\n",
			(String("Board: ") + String(board) + " Class: " + String(classid) + " Index: " + String(index) + " Value: " + String(value)).c_str()
		);
	}
}




	// TODO: Implement on X32Core

	// if (isButtonPressed)
	// {
	// 		case X32_BTN_SCENE_SETUP:
	// 			if (config->IsModelX32Core()) {
	// 				state->x32core_lcdmode_setup = !state->x32core_lcdmode_setup;
	// 				if (state->x32core_lcdmode_setup) {
	// 					surface->SetLedByEnum(X32_BTN_SCENE_SETUP, 1);
	// 					lcdmenu->OnShow();
	// 				} else {
	// 					surface->SetLedByEnum(X32_BTN_SCENE_SETUP, 0);
	// 					// trigger switch to channel lcd
	// 					config->Refresh(SELECTED_CHANNEL);
	// 				}
	// 			}
	// 			break;
	// 		case X32_BTN_CHANNEL_ENCODER:
	// 			if (config->IsModelX32Core()) {
	// 				if (state->x32core_lcdmode_setup) {
	// 					lcdmenu->OnLcdEncoderPressed();
	// 				}
	// 			}
	// 			break;
	// }


//################################################################################
//# 
//#  ########   #######  ########  ##    ## ##       ########  ######   ######  
//#  ##     ## ##     ## ##     ##  ##  ##  ##       ##       ##    ## ##    ## 
//#  ##     ## ##     ## ##     ##   ####   ##       ##       ##       ##       
//#  ########  ##     ## ##     ##    ##    ##       ######    ######   ######  
//#  ##     ## ##     ## ##     ##    ##    ##       ##             ##       ## 
//#  ##     ## ##     ## ##     ##    ##    ##       ##       ##    ## ##    ## 
//#  ########   #######  ########     ##    ######## ########  ######   ######  
//# 
//################################################################################


// Key was pressed in the bodyless mode
void X32Ctrl::SimulatorButton(uint32_t key)
{
	printf("Simulatorbutton: %d\n", key);

	X32_PAGE activePage = (X32_PAGE)config->GetUint(ACTIVE_PAGE);

	switch (key)
	{
		using enum X32_PAGE;

		case 49:
			// HOME
			config->Set(ACTIVE_PAGE, (uint)HOME);
			break;
		case 50:
			// METERS
			config->Set(ACTIVE_PAGE, (uint)METERS);
			break;
		case 51:
			config->Set(ACTIVE_PAGE, (uint)ROUTING);
			break;
		case 52:
			config->Set(ACTIVE_PAGE, (uint)LIBRARY);
			break;
		case 53:
			config->Set(ACTIVE_PAGE, (uint)EFFECTS);
			break;
		case 54:
			config->Set(ACTIVE_PAGE, (uint)SETUP);
			break;
		case 55:
			//ShowPage(MONITOR);
			break;
		case 56:
			//ShowPage(SCENES);
			break;
		case 40899:
			config->Toggle(DISPLAY_UTILITY);
			break;
		case 57:
			config->Toggle(DISPLAY_MUTE_GROUP);
			break;
		case 48:
			config->Set(ACTIVE_PAGE, (uint)DEBUG);
			break;
		case 17:
			config->Refresh(DISPLAY_UP);
			break;
		case 18:
			config->Refresh(DISPLAY_DOWN);
			break;
		case 20:
			config->Refresh(DISPLAY_LEFT);
			break;
		case 19:
			config->Refresh(DISPLAY_RIGHT);
			break;
		case 113: // Q
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x09, 1); // Encoder 1 up
			break;
		case 119: // W
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0A, 1); // Encoder 2 up
			break;
		case 101: // E
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0B, 1); // Encoder 3 up
			break;
		case 114: // R
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0C, 1); // Encoder 4 up
			break;
		case 116: // T
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0D, 1); // Encoder 5 up
			break;
		case 122: // Z
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0E, 1); // Encoder 6 up
			break;
		case 97: // A
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x09, 255); // Encoder 1 down
		 	break;
		case 115: // S
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0A, 255); // Encoder 2 down
			break;
		case 100: // D
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0B, 255); // Encoder 3 down
			break;
		case 102: // F
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0C, 255); // Encoder 4 down
			break;
		case 103: // G
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0D, 255); // Encoder 5 down
			break;
		case 104: // H
			ProcessSurface(X32_BOARD_MAIN, 'e', 0x0E, 255); // Encoder 6 down
			break;
		case 121: // Y
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x18 + 0x80); // Encoder Button 1 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x18); // Encoder Button 1 release
			break;
		case 120: // X
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x19 + 0x80); // Encoder Button 2 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x19); // Encoder Button 2 release
			break;
		case 99: // C
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1A + 0x80); // Encoder Button 3 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1A); // Encoder Button 3 release
			break;
		case 118: // V
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1B + 0x80); // Encoder Button 4 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1B); // Encoder Button 4 release
			break;
		case 98: // B
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1C + 0x80); // Encoder Button 5 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1C); // Encoder Button 5 release
			break;
		case 110: // N
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1D + 0x80); // Encoder Button 6 press
			ProcessSurface(X32_BOARD_MAIN, 'b', 0, 0x1D); // Encoder Button 6 release
			break;
		case 0xFFFFFF9F:
			config->Set(ACTIVE_PAGE, (uint)SCENES);
			break;
		case 46274: // Button REMOTE DAW
			ProcessSurface(X32_BOARD_R, 'b', 0, 0x00 + 0x80);
			ProcessSurface(X32_BOARD_R, 'b', 0, 0x00);
			break;
	}
}