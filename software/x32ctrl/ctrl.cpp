#include "ctrl.h"

X32Ctrl::X32Ctrl(X32BaseParameter* basepar) : X32Base(basepar) {
	mixer = new Mixer(basepar);
	surface = new Surface(basepar);
	xremote = new XRemote(basepar);
	lcdmenu = new LcdMenu(basepar, mixer, surface); // only used for X32Core (at the moment, maybe later for assing-section?)
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

void X32Ctrl::Init(){
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
	helper->ReadConfig("/etc/x32.conf", "MDL=", model, 12);
	helper->ReadConfig("/etc/x32.conf", "SN=", serial, 15);
	helper->ReadConfig("/etc/x32.conf", "DATE=", date, 16);
	helper->Log("Detected model: %s with Serial %s built on %s\n", model, serial, date);

	if (state->bodyless) {
		config->SetModel("X32RACK");
	} else {
		config->SetModel(model);
	}
	config->SetBankMode(X32_SURFACE_MODE_BANKING_X32);

	//##################################################################################
	//#
	//# 	Initialize system
	//#
	//##################################################################################

	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->Init()");
	mixer->Init();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "surface->Init()");
	surface->Init();

	helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "xremote->Init()");
	xremote->Init();

	if (config->IsModelX32Core()) {
		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "lcdmenu->Init()");
		lcdmenu->OnInit();
	}

	//############################################################################
	//#                                                                          #
	//#     Default Config                                                       #
	//#                                                                          #
	//############################################################################
	//#                                                                          #
	//#     With GUI                                                             #
	//#                                                                          #
	if(config->IsModelX32FullOrCompactOrProducer()) {

			ResetFaderBankLayout();
			LoadFaderBankLayout(FADER_BANK_LAYOUT_X32);
	}
	//############################################################################
	//#                                                                          #
	//#     Without GUI                                                          #
	//#                                                                          #
	if(config->IsModelX32Core()) {
			// Placeholder
	}
	//############################################################################
	//#                                                                          #
	//#     Common to all                                                        #
	//#                                                                          #

			// Just load a default set of FXes
			// TODO: Save and Load
			mixer->dsp->DSP2_SetFx(0, FX_TYPE::REVERB, 2);
            mixer->dsp->DSP2_SetFx(1, FX_TYPE::CHORUS, 2);
            mixer->dsp->DSP2_SetFx(2, FX_TYPE::TRANSIENTSHAPER, 2);
            mixer->dsp->DSP2_SetFx(3, FX_TYPE::DELAY, 2);
            mixer->dsp->DSP2_SetFx(4, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(5, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(6, FX_TYPE::NONE, 2);
            mixer->dsp->DSP2_SetFx(7, FX_TYPE::NONE, 2);
	//#                                                                          #
	//############################################################################
    //#                                                                          #
	//#     EoDC - Default of Default Config :-)                                 #
	//#                                                                          #
	//############################################################################
	

	// Config
	if(helper->GetFileSize(X32_MIXER_CONFIGFILE) == -1)	{
		// create new ini file
		helper->DEBUG_INI(DEBUGLEVEL_NORMAL, "no %s found, creating one", X32_MIXER_CONFIGFILE);
		ofstream outfile (X32_MIXER_CONFIGFILE);
		outfile << endl;
		outfile.close();

		SaveConfig();		
	} else {
		LoadConfig();
	}

	// set brightness and contrast
	helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Set LED Brightness to %d", state->ledbrightness);
    surface->SetBrightness(0, state->ledbrightness); // brightness of LEDs
    surface->SetBrightness(1, state->ledbrightness);
    surface->SetBrightness(4, state->ledbrightness);
    surface->SetBrightness(5, state->ledbrightness);
    surface->SetBrightness(8, state->ledbrightness);

	Mixerparameter* parameter = config->GetParameter(MP_ID::LCD_CONTRAST);
    helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Set LCD Contrast to %d", parameter->GetUint());
    surface->SetContrast(0, parameter->GetUint()); // contrast of LCDs
    surface->SetContrast(4, parameter->GetUint());
    surface->SetContrast(5, parameter->GetUint());
    surface->SetContrast(8, parameter->GetUint());
}

// Load a Fader bank layout, e.g. LAYOUT_X32 or LAYOUT_USER
void X32Ctrl::LoadFaderBankLayout(int layout) {

	if (layout == FADER_BANK_LAYOUT_X32) {

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
					modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 16);
					helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 16));
				}
			}
		}
		if (config->IsModelX32CompactOrProducer()){
			// bank is 8 channels wide
			for (uint8_t bank=0;bank<8;bank++){
				for (int i = 0; i <=7; i++) {
					modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i] = i + (bank * 8);
					helper->DEBUG_X32CTRL(DEBUGLEVEL_VERBOSE, "Assing bank%d: surfaceChannel%d <-> vchannel%d\n", bank, i, i + (bank * 8));
				}
			}
		}

		// assign channels to bus fader bank

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
	}	
}

void X32Ctrl::ResetFaderBankLayout() {
	// reset channel section
	for (uint8_t b = 0; b < 8; b++){
		for (uint8_t sCh = 0; sCh < 16; sCh++){
			modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
			modes[X32_SURFACE_MODE_BANKING_USER].inputBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}
	// reset bus section
	for (uint8_t b = 0; b < 4; b++){
		for (uint8_t sCh = 0; sCh < 8; sCh++){
			modes[X32_SURFACE_MODE_BANKING_X32].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
			modes[X32_SURFACE_MODE_BANKING_USER].busBanks[b].surfaceChannel2VChannel[sCh] = VCHANNEL_NOT_SET;
		}
	}
}

//#####################################################################################################################
//
// ##        #######     ###    ########  
// ##       ##     ##   ## ##   ##     ## 
// ##       ##     ##  ##   ##  ##     ## 
// ##       ##     ## ##     ## ##     ## 
// ##       ##     ## ######### ##     ## 
// ##       ##     ## ##     ## ##     ## 
// ########  #######  ##     ## ########  
//
//#####################################################################################################################

void X32Ctrl::LoadConfig() {
	helper->DEBUG_INI(DEBUGLEVEL_NORMAL, "Load config from %s", X32_MIXER_CONFIGFILE);
	mixer_ini.load(X32_MIXER_CONFIGFILE);

	// go over all known Mixerparameters and try to load them
	for (const auto& [parameter_id, parameter] : *config->GetParameterList()) {

		if (parameter->GetNoConfigfile() || parameter_id == MP_ID::NONE)
		{
			// this Mixerparameter should not be written to config file
			continue;
		}

		for (uint index = 0; index < parameter->GetInstances(); index++)
		{
			String section = parameter->GetConfigGroup() + String(index);			
			String entry = parameter->GetConfigName();
			
			using enum MP_VALUE_TYPE;

			switch(parameter->GetType())
			{
				case FLOAT:
					parameter->Set(mixer_ini[section.c_str()][entry.c_str()].as<float>(), index);
					break;
				case UINT:
					parameter->Set(mixer_ini[section.c_str()][entry.c_str()].as<uint>(), index);
					break;
    			case INT:
					parameter->Set(mixer_ini[section.c_str()][entry.c_str()].as<int>(), index);
					break;
    			case BOOL:
					parameter->Set(mixer_ini[section.c_str()][entry.c_str()].as<bool>(), index);
					break;
    			case STRING:
					parameter->Set(String(mixer_ini[section.c_str()][entry.c_str()].as<string>().c_str()), index);
					break;
				default:
					__throw_out_of_range("SaveConfig() -> MP_VALUE_TYPE is not handled!");
			}
		}
	}
}

//#####################################################################################################################
//
//  ######     ###    ##     ## ######## 
// ##    ##   ## ##   ##     ## ##       
// ##        ##   ##  ##     ## ##       
//  ######  ##     ## ##     ## ######   
//       ## #########  ##   ##  ##       
// ##    ## ##     ##   ## ##   ##       
//  ######  ##     ##    ###    ######## 
//
//#####################################################################################################################

void X32Ctrl::SaveConfig() {

	// go over all known Mixerparameter an store them
	for (const auto& [parameter_id, parameter] : *config->GetParameterList()) {

		if (parameter->GetNoConfigfile() || parameter_id == MP_ID::NONE)
		{
			// this Mixerparameter should not be written to config file
			continue;
		}

		for (uint index = 0; index < parameter->GetInstances(); index++)
		{
			String section = parameter->GetConfigGroup() + String(index);			
			String entry = parameter->GetConfigName();
			
			using enum MP_VALUE_TYPE;

			switch(parameter->GetType())
			{
				case FLOAT:
					mixer_ini[section.c_str()][entry.c_str()] = (parameter->GetFloat(index));
					break;
				case UINT:
					mixer_ini[section.c_str()][entry.c_str()] = (parameter->GetUint(index));
					break;
    			case INT:
					mixer_ini[section.c_str()][entry.c_str()] = parameter->GetInt(index);
					break;
    			case BOOL:
					mixer_ini[section.c_str()][entry.c_str()] = parameter->GetBool(index);
					break;
    			case STRING:
					mixer_ini[section.c_str()][entry.c_str()] = parameter->GetString(index).c_str();
					break;
				default:
					__throw_out_of_range("SaveConfig() -> MP_VALUE_TYPE is not handled!");
			}
		}
	}

	helper->DEBUG_INI(DEBUGLEVEL_NORMAL, "Save config to %s", X32_MIXER_CONFIGFILE);
	mixer_ini.save(X32_MIXER_CONFIGFILE);
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

void X32Ctrl::Tick10ms(void){
	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "10ms");

	if (config->IsModelX32FullOrCompactOrProducer()) {
		surface->Touchcontrol();	
	}
	mixer->dsp->ReadAndUpdateVUMeterData();

	ProcessUartData();

	// communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
	//UdpHandleCommunication();

	// sync if any Mixerparameter has changed
	if (config->HasAnyParameterChanged())
	{
		syncGuiOrLcd();
		syncSurface();
		syncXRemote(false);

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->Sync()");
		mixer->Sync();

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "mixer->card->Sync()");
		mixer->card->Sync();

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "Reset list of changed Mixerparameter.");
		config->ResetChangedParameterList();
	}
}

void X32Ctrl::Tick50ms(void) {
	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "50ms");

	UpdateMeters();
}

void X32Ctrl::Tick100ms(void) {
	helper->DEBUG_TIMER(DEBUGLEVEL_TRACE, "100ms");

	surface->Blink();
	mixer->dsp->spi->ActivityLight();

	if (!config->IsModelX32Core() && config->GetUint(MP_ID::ACTIVE_PAGE) == (uint)X32_PAGE::UTILITY) {
		// read the current DSP load
		// show the received value (could be a bit older than the request)
	 	lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% / Heap: %.0f Words free [v%.2f]", (double)state->dspLoad[0], (double)state->dspVersion[0], (double)state->dspLoad[1], (double)state->dspFreeHeapWords[1], (double)state->dspVersion[1]);
	}
}


//#####################################################################################################################
//#####################################################################################################################
//
// 			E V E N T S
//
//#####################################################################################################################
//#####################################################################################################################

void X32Ctrl::ProcessEventsRaw(SurfaceEvent* event){
	switch(event->classId){
	  case 'f':
		FaderMoved(event);
		break;
	  case 'b':
		ButtonPressedOrReleased(event);
		break;
	  case 'e':
		EncoderTurned(event);
		break;
	  default:
		helper->DEBUG_X32CTRL(DEBUGLEVEL_TRACE, "unknown message: %s\n", event->ToString().c_str());
	}
	delete(event);
}

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

void X32Ctrl::InitPagesAndGUI(){
	PageBaseParameter* pagebasepar = new PageBaseParameter(app, config, state, helper, mixer, surface);
	
	pages[X32_PAGE::HOME] = new PageHome(pagebasepar);
	pages[X32_PAGE::CONFIG] = new PageConfig(pagebasepar);
	pages[X32_PAGE::GATE] = new PageGate(pagebasepar);
	pages[X32_PAGE::COMPRESSOR] = new PageDynamics(pagebasepar);
	pages[X32_PAGE::EQ] = new PageEq(pagebasepar);
	pages[X32_PAGE::METERS] = new PageMeters(pagebasepar);
	pages[X32_PAGE::ROUTING] = new PageRouting(pagebasepar);
	pages[X32_PAGE::ROUTING_FPGA] = new PageRoutingFpga(pagebasepar);
	pages[X32_PAGE::ROUTING_DSP1] = new PageRoutingDsp1(pagebasepar);
	pages[X32_PAGE::ROUTING_DSP2] = new PageRoutingDsp2(pagebasepar);
	pages[X32_PAGE::SETUP] = new PageSetup(pagebasepar);
	pages[X32_PAGE::SETUP_CARD] = new PageSetupCard(pagebasepar);
	pages[X32_PAGE::LIBRARY] = new PageLibrary(pagebasepar);
	pages[X32_PAGE::EFFECTS] = new PageEffects(pagebasepar);
	pages[X32_PAGE::MUTE_GRP] = new PageMutegroup(pagebasepar);
	pages[X32_PAGE::UTILITY] = new PageUtility(pagebasepar);
	for (const auto& [key, value] : pages) {
		value->Init();
	}

	// trigger first update of header
	config->Refresh(MP_ID::SELECTED_CHANNEL);

	// trigger first update on shown page
	config->Refresh(MP_ID::ACTIVE_PAGE);	
}

void X32Ctrl::ShowNextPage(void){

	X32_PAGE activePage = (X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE);

	X32_PAGE nextPage = pages[activePage]->GetNextPage();
	if (nextPage != X32_PAGE::NONE){
	 	ShowPage(nextPage);
	} else {
		// if theres is no next page, send button press to page
		pages[activePage]->DisplayButton(X32_BTN_RIGHT, true);
	}
}

void X32Ctrl::ShowPrevPage(void){

	X32_PAGE activePage = (X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE);

	X32_PAGE prevPage = pages[activePage]->GetPrevPage();
	if (prevPage != X32_PAGE::NONE){
	 	ShowPage(prevPage);
	} else {
		// if theres is no prev page, send button press to page
		pages[activePage]->DisplayButton(X32_BTN_LEFT, true);
	}
}

void X32Ctrl::ShowPage(X32_PAGE newPage) {

	// only work's on devices with display
	if (!(config->IsModelX32FullOrCompactOrProducerOrRack()))
	{
		return;
	}

	X32_PAGE activePage = (X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE);
	
	if (newPage == activePage) {
		// operator has pressed the button of the current active page,
		// so go back to previous page
		newPage = lastPage;
	}

	if (pages[newPage] != nullptr) {

		lastPage = activePage;
		activePage = newPage;
		
		Page* p = pages[activePage];
		Page* l = pages[lastPage];

		// turn off led of prev page
		if (l->GetLed() != X32_BTN_NONE) {
			surface->SetLedByEnum(l->GetLed(), false);
		}

		// turn on led of new active page
		if (p->GetLed() != X32_BTN_NONE) {
			surface->SetLedByEnum(p->GetLed(), true);
		}

		config->Set(MP_ID::ACTIVE_PAGE, (uint)activePage);
		
	} else {
		helper->Error("Page 0x%02X is not registered -> look at X32Ctrl::InitPages() to do so!\n", newPage);
	}
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
			lcdmenu->OnChange();
		}
		
		// return, because X32Core has no GUI
		return;
	}
	
	//####################################
	//#     Show Active Page
	//####################################

	if (config->HasParameterChanged(MP_ID::ACTIVE_PAGE)){
		pages[(X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE)]->Show();
	}

	//####################################
	//#     Update Active Page
	//####################################

	pages[(X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE)]->Change();

	//####################################
	//#     Update General Header
	//####################################

	if (config->HasParameterChanged(MP_ID::SELECTED_CHANNEL)) {

		uint chanIndex = config->GetUint(MP_ID::SELECTED_CHANNEL);

		lv_color_t color;
		switch (config->GetUint(MP_ID::CHANNEL_COLOR, chanIndex)){
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

		lv_label_set_text(objects.current_channel_number, config->GetString(MP_ID::CHANNEL_NAME_INTERN, chanIndex).c_str());
		lv_label_set_text(objects.current_channel_name, config->GetString(MP_ID::CHANNEL_NAME, chanIndex).c_str());
		lv_obj_set_style_bg_color(objects.current_channel_color, color, 0);
	}
}

// sync mixer state to Surface
void X32Ctrl::syncSurface(void) {
	if ((config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) || (config->GetBankMode() == X32_SURFACE_MODE_BANKING_USER))
	{
		surfaceSyncBoardMain();

		if (config->IsModelX32FullOrCompactOrProducer()){   
			surfaceSyncBankIndicator();

			surfaceSyncBoard(X32_BOARD_L);
			if (config->IsModelX32Full()){
				surfaceSyncBoard(X32_BOARD_M);
			}
			surfaceSyncBoard(X32_BOARD_R);
		
			surfaceSyncBoardExtra();
		}
	}
}

void X32Ctrl::surfaceSyncBoardMain() {

	using enum MP_ID;

	bool fullSync = false;
	uint8_t chanIndex = config->GetUint(SELECTED_CHANNEL);

	if (config->HasParameterChanged(SELECTED_CHANNEL)){ 
		// channel selection has changed - do a full sync
		fullSync=true; 
	}

	if (config->IsModelX32FullOrCompactOrProducer()) {
		
		// Phantom
		if (config->HasParameterChanged(CHANNEL_PHANTOM) || fullSync)
		{
			surface->SetLedByEnum(X32_BTN_PHANTOM_48V, config->GetBool(CHANNEL_PHANTOM, chanIndex)); 
		}
		// Phase Invert
		if (config->HasParameterChanged(CHANNEL_PHASE_INVERT) || fullSync)
		{
			surface->SetLedByEnum(X32_BTN_PHASE_INVERT, config->GetBool(CHANNEL_PHASE_INVERT, chanIndex));
		}
		// Gain
		if (config->HasParameterChanged(CHANNEL_GAIN) || fullSync)
		{
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_GAIN] >> 8, surface->Enum2Encoder[X32_ENC_GAIN] & 0xFF, 0, config->GetPercent(CHANNEL_GAIN, chanIndex), 1);
		}
		// Balance/Panorama
		if (config->HasParameterChanged(CHANNEL_PANORAMA) || fullSync)
		{
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_PAN] >> 8, surface->Enum2Encoder[X32_ENC_PAN] & 0xFF, 2, (config->GetFloat(CHANNEL_PANORAMA, chanIndex) + 100.0f)/2.0f, 1);
		}
		// Bus sends
		if (config->HasParametersChanged(MP_CAT::CHANNEL_SENDS) || fullSync)
		{
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_1] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_1] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, config->GetUint(BANKING_BUS_SENDS) * 4 + 0)/20.0f) * 100.0f, 1);
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_2] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_2] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, config->GetUint(BANKING_BUS_SENDS) * 4 + 1)/20.0f) * 100.0f, 1);
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_3] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_3] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, config->GetUint(BANKING_BUS_SENDS) * 4 + 2)/20.0f) * 100.0f, 1);
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_4] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_4] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, config->GetUint(BANKING_BUS_SENDS) * 4 + 3)/20.0f) * 100.0f, 1);
		}
		// Gate
		if (config->HasParameterChanged(CHANNEL_GATE_TRESHOLD) || fullSync)
		{
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_GATE] >> 8, surface->Enum2Encoder[X32_ENC_GATE] & 0xFF, 4, 100.0f - ((config->GetFloat(CHANNEL_GATE_TRESHOLD, chanIndex) + 80.0f)/0.8f), 1);
		}
		// Dynamics
		if (config->HasParameterChanged(CHANNEL_DYNAMICS_TRESHOLD) || fullSync)
		{
			surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_DYNAMICS] >> 8, surface->Enum2Encoder[X32_ENC_DYNAMICS] & 0xFF, 4, 100.0f - ((config->GetFloat(CHANNEL_DYNAMICS_TRESHOLD, chanIndex) + 60.0f)/0.6f), 1);
		}
		// EQ
		if (config->HasParametersChanged(MP_CAT::CHANNEL_EQ) || fullSync)
		{
			if (chanIndex < 40) {
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_LOWCUT] >> 8, surface->Enum2Encoder[X32_ENC_LOWCUT] & 0xFF, 1, (config->GetFloat(CHANNEL_LOWCUT, chanIndex) - 20.0f)/3.8f, 1);
				
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_FREQ] >> 8, surface->Enum2Encoder[X32_ENC_EQ_FREQ] & 0xFF, 1, (config->GetFloat((MP_ID)((uint)CHANNEL_EQ_FREQ1 + config->GetUint(BANKING_EQ, chanIndex))) - 20.0f)/199.8f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_GAIN] >> 8, surface->Enum2Encoder[X32_ENC_EQ_GAIN] & 0xFF, 2, (config->GetFloat((MP_ID)((uint)CHANNEL_EQ_GAIN1 + config->GetUint(BANKING_EQ, chanIndex))) + 15.0f)/0.3f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_Q] >> 8, surface->Enum2Encoder[X32_ENC_EQ_Q] & 0xFF, 3, ((10.0f - config->GetFloat((MP_ID)((uint)CHANNEL_EQ_Q1 + config->GetUint(BANKING_EQ, chanIndex)))) + 0.3f)/0.097f, 1);
				
				// EQ-LEDS
				uint eq_type = config->GetUint((MP_ID)((uint)CHANNEL_EQ_TYPE1 + config->GetUint(BANKING_EQ, chanIndex)), chanIndex);

				surface->SetLedByEnum(X32_LED_EQ_HCUT, eq_type == 6); // LowPass
				surface->SetLedByEnum(X32_LED_EQ_HSHV, eq_type == 3); // HighShelf
				surface->SetLedByEnum(X32_LED_EQ_LCUT, eq_type == 7); // HighPass
				surface->SetLedByEnum(X32_LED_EQ_LSHV, eq_type == 2); // LowShelf
				surface->SetLedByEnum(X32_LED_EQ_PEQ, eq_type == 1); // PEQ
				surface->SetLedByEnum(X32_LED_EQ_VEQ, false);

				uint activeEQ = config->GetUint(BANKING_EQ);
				surface->SetLedByEnum(X32_BTN_EQ_LOW, activeEQ == 0);
				surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, activeEQ == 1);
				surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, activeEQ == 2);
				surface->SetLedByEnum(X32_BTN_EQ_HIGH, activeEQ == 3);
			}
		}
	}

	if (config->IsModelX32Rack()) {			
		if (config->HasParameterChanged(SELECTED_CHANNEL)){
			setLedChannelIndicator_Rack();
		}
		// Solo
		if (config->HasParameterChanged(CHANNEL_SOLO) || fullSync){
			surface->SetLedByEnum(X32_BTN_CHANNEL_SOLO, config->GetBool(CHANNEL_SOLO, chanIndex)); 
		}
		// Mute
		if (config->HasParameterChanged(CHANNEL_MUTE) || fullSync){
			surface->SetLedByEnum(X32_BTN_CHANNEL_MUTE, config->GetBool(CHANNEL_MUTE, chanIndex)); 
		}
		// Volume
		if (config->HasParametersChanged({CHANNEL_VOLUME, CHANNEL_MUTE}) ||fullSync){
			surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 0, config->GetFloat(CHANNEL_VOLUME, chanIndex), config->GetBool(CHANNEL_MUTE, chanIndex), 1);
		}
	}

	if (config->IsModelX32Core()) {

		if (config->HasParameterChanged(SELECTED_CHANNEL)){
			setLedChannelIndicator_Core();
		}

		if (!state->x32core_lcdmode_setup && (
			config->HasParametersChanged({
				SELECTED_CHANNEL,
				CHANNEL_SOLO,
				CHANNEL_MUTE,
				CHANNEL_COLOR,
				CHANNEL_NAME
			})))
		{
			SetLcdFromVChannel(X32_BOARD_MAIN, 0, chanIndex);
		}

		// Volume
		if (config->HasParametersChanged({CHANNEL_VOLUME, CHANNEL_MUTE}, chanIndex))
		{
			surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 0, config->GetFloat(CHANNEL_VOLUME, chanIndex), config->GetBool(CHANNEL_MUTE, chanIndex), 1);
		}

		// Main Channel
		if (config->HasParametersChanged({CHANNEL_VOLUME, CHANNEL_MUTE}, (uint)X32_VCHANNEL_BLOCK::MAIN))
		{
			surface->SetEncoderRingDbfs(1, 1, config->GetFloat(MP_ID::CHANNEL_VOLUME, (uint)X32_VCHANNEL_BLOCK::MAIN), config->GetBool(MP_ID::CHANNEL_MUTE, (uint)X32_VCHANNEL_BLOCK::MAIN), 1);
		}
	}


	if (config->IsModelX32Rack()){
		// Clear Solo
		if (config->HasParameterChanged(MP_ID::CHANNEL_SOLO)){
			bool soloActive = mixer->IsSoloActivated();
			surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, soloActive, soloActive);
		}
		// Main Channel
		uint mainchanIndex = (uint)X32_VCHANNEL_BLOCK::MAIN;
		if (config->HasParameterChanged(MP_ID::CHANNEL_VOLUME, mainchanIndex) || config->HasParameterChanged(MP_ID::CHANNEL_MUTE, mainchanIndex)){
			surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 1, config->GetFloat(MP_ID::CHANNEL_VOLUME, mainchanIndex), config->GetBool(MP_ID::CHANNEL_MUTE, mainchanIndex), 0);
		}
	}
}

void X32Ctrl::surfaceSyncBoard(X32_BOARD p_board) {

	using enum MP_ID;

	bool fullSync = false;

	if (config->IsModelX32Full()){
		if (config->HasParameterChanged(BANKING_INPUT) && ((p_board == X32_BOARD_L) || (p_board == X32_BOARD_M))){ fullSync=true; }
		if (config->HasParameterChanged(BANKING_BUS) && p_board == X32_BOARD_R){ fullSync=true; }
	}

	if (config->IsModelX32CompactOrProducer()){
		if (config->HasParameterChanged(BANKING_INPUT) && p_board == X32_BOARD_L){ fullSync=true; }
		if (config->HasParameterChanged(BANKING_BUS) && p_board == X32_BOARD_R){ fullSync=true; }
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

		if (channelIndex == VCHANNEL_NOT_SET)
		{
			// TODO: do only, wenn channel got unassigned
			surface->SetLed(p_board, 0x20+i, 0);
			surface->SetLed(p_board, 0x30+i, 0);
			surface->SetLed(p_board, 0x40+i, 0);
			surface->SetFader(p_board, i, 0);
			//  setLcd(boardId, index, color, xicon, yicon, icon, sizeA, xA, yA, const char* strA, sizeB, xB, yB, const char* strB)
			surface->SetLcd(p_board,     i, 0,     0,    0,    0,  0x00,  0,  0,          "",  0x00,  0, 0, "");

		} else {
			
			if (config->HasParameterChanged(CHANNEL_SELECTED) || fullSync)
			{ 
				surface->SetLed(p_board, 0x20+i, config->GetBool(CHANNEL_SELECTED, channelIndex));
			}
			if (config->HasParameterChanged(CHANNEL_SOLO) || fullSync)
			{
				surface->SetLed(p_board, 0x30+i, config->GetBool(CHANNEL_SOLO, channelIndex)); 
			}
			if (config->HasParameterChanged(CHANNEL_MUTE) || fullSync)
			{
				surface->SetLed(p_board, 0x40+i, config->GetBool(CHANNEL_MUTE, channelIndex)); 
			}

			if (config->HasParameterChanged(CHANNEL_VOLUME, channelIndex) || fullSync)
			{
				u_int16_t faderPosition = helper->Dbfs2Fader(config->GetFloat(CHANNEL_VOLUME, channelIndex));
				surface->SetFader(p_board, i, faderPosition);
			}

			if(!state->surface_disable_lcd_update)
			{
				if (config->HasParametersChanged({
						CHANNEL_PHASE_INVERT,
						CHANNEL_VOLUME,
						CHANNEL_GAIN,
						CHANNEL_GATE_TRESHOLD,
						CHANNEL_DYNAMICS_TRESHOLD,
						CHANNEL_PHANTOM,
						CHANNEL_COLOR,
						CHANNEL_NAME
					}) || config->HasParametersChanged({
						MP_CAT::CHANNEL_EQ
					}) ||
						fullSync
				)
				{
					helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "LCD");

					SetLcdFromVChannel(p_board, i, channelIndex);
				}
			}
		}
	}

	if (p_board == X32_BOARD_R){
		// Clear Solo
		if (config->HasParameterChanged(CHANNEL_SOLO)) { 
			bool soloActivated = mixer->IsSoloActivated();
			surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, soloActivated, soloActivated); 
		}
	}
}

void X32Ctrl::surfaceSyncBoardExtra() {
	if (config->IsModelX32Full()) {
		// TODO
		surface->SetLcd(0, 0, 7, 0, 0, 0xA0, 0x20, 5, 5, "OpenX32", 0, 0, 0, "");
		surface->SetLcd(0, 1, 7, 0, 0, 0xA0, 0x20, 5, 5, "is a", 0, 0, 0, "");
		surface->SetLcd(0, 2, 7, 0, 0, 0xA0, 0x20, 5, 5, "cool", 0, 0, 0, "");
		surface->SetLcd(0, 3, 7, 0, 0, 0xA0, 0x20, 5, 5, "Thing!", 0, 0, 0, "");
	}
}


void X32Ctrl::SetLcdFromVChannel(uint8_t p_boardId, uint8_t lcdIndex, uint8_t channelIndex)
{
	using enum MP_ID;

    LcdData* data = new LcdData();

    data->boardId = p_boardId;
    data->color = config->GetUint(CHANNEL_COLOR, channelIndex) | config->GetUint(CHANNEL_COLOR_INVERTED, channelIndex);
    data->lcdIndex = lcdIndex;
    data->icon.icon = 0;
    data->icon.x = 0;
    data->icon.y = 0;

    // Gain / Lowcut
    data->texts[0].text = String(config->GetFloat(CHANNEL_GAIN, channelIndex), 1) + String("dB ") + String(config->GetFloat(CHANNEL_LOWCUT, channelIndex), 0) + String("Hz");
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
    data->texts[3].text = config->GetFloat(CHANNEL_NAME, channelIndex);
    data->texts[3].size = 0;
    data->texts[3].x = 0;
    data->texts[3].y = 48;

    surface->SetLcdX(data, 4);

	delete data;
}


// Update all meters (Gui, Surface, xremote)
void X32Ctrl::UpdateMeters(void) {

	if (config->IsModelX32FullOrCompactOrProducerOrRack())
	{
		pages[(X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE)]->UpdateMeters();
	}


	xremote->UpdateMeter(mixer);

	// ########################################
	//
	//		Surface Meters
	//
	// ########################################

	uint8_t chanIdx = config->GetUint(MP_ID::SELECTED_CHANNEL); //config->selectedVChannel;

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

	if (config->IsModelX32CompactOrProducer()){
		switch (config->GetUint(BANKING_INPUT)) {
			case 0: // Input 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[i].meter6Info);
				}
				break;
			case 1: // Input 9-16
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[8 + i].meter6Info);
				}
				break;
			case 2: // Input 17-24
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[16 + i].meter6Info);
				}
				break;
			case 3: // Input 25-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[24 + i].meter6Info);
				}
				break;
			case 4: // Aux 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[32 + i].meter6Info);
				}
				break;
			case 5: // FX-Return
				break;
			case 6: // Bus 1-8
				break;
			case 7: // Bus 9-16
				break;
		}

		// update meters on board R
		switch (config->GetUint(BANKING_BUS)) {
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

		// update channel-meters
	if (config->IsModelX32Full()) {

		// update meters on board L and M
		switch (config->GetUint(BANKING_INPUT)) {
			case 0: // Input 1-16
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[i].meter6Info);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->rChannel[i + 8].meter6Info);
				}
				break;
			case 1: // Input 17-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[16 + i].meter6Info);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->rChannel[16 + i + 8].meter6Info);
				}
				break;
			case 2: // Aux 1-8 / FX-Return
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[32 + i].meter6Info);
					//setMeterLed(X32_BOARD_M, i, 0);
				}
				break;
			case 3: // Bus 1-16
				break;
		}
	}
}

void X32Ctrl::surfaceSyncBankIndicator(void) {
	if (config->HasParameterChanged(BANKING_INPUT)) {
		if (config->IsModelX32Full()){
			surface->SetLedByEnum(X32_BTN_CH_1_16, config->GetUint(BANKING_INPUT) == 0);
			surface->SetLedByEnum(X32_BTN_CH_17_32, config->GetUint(BANKING_INPUT) == 1);
			surface->SetLedByEnum(X32_BTN_AUX_IN_EFFECTS, config->GetUint(BANKING_INPUT) == 2);
			surface->SetLedByEnum(X32_BTN_BUS_MASTER, config->GetUint(BANKING_INPUT) == 3);
		}
		if (config->IsModelX32CompactOrProducer()) {
			surface->SetLedByEnum(X32_BTN_CH_1_8, config->GetUint(BANKING_INPUT) == 0);
			surface->SetLedByEnum(X32_BTN_CH_9_16, config->GetUint(BANKING_INPUT) == 1);
			surface->SetLedByEnum(X32_BTN_CH_17_24, config->GetUint(BANKING_INPUT) == 2);
			surface->SetLedByEnum(X32_BTN_CH_25_32, config->GetUint(BANKING_INPUT) == 3);
			surface->SetLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, config->GetUint(BANKING_INPUT) == 4);
			surface->SetLedByEnum(X32_BTN_EFFECTS_RETURNS, config->GetUint(BANKING_INPUT) == 5);
			surface->SetLedByEnum(X32_BTN_BUS_1_8_MASTER, config->GetUint(BANKING_INPUT) == 6);
			surface->SetLedByEnum(X32_BTN_BUS_9_16_MASTER, config->GetUint(BANKING_INPUT) == 7);
		}
	}
	if (config->HasParameterChanged(MP_ID::BANKING_BUS)) {
		surface->SetLedByEnum(X32_BTN_GROUP_DCA_1_8, config->GetUint(BANKING_BUS) == 0);
		surface->SetLedByEnum(X32_BTN_BUS_1_8, config->GetUint(BANKING_BUS) == 1);
		surface->SetLedByEnum(X32_BTN_BUS_9_16, config->GetUint(BANKING_BUS) == 2);
		surface->SetLedByEnum(X32_BTN_MATRIX_MAIN_C, config->GetUint(BANKING_BUS) == 3);
	}
}

// only X32 Rack
void X32Ctrl::setLedChannelIndicator_Rack(void){
		uint8_t chanIdx = config->GetUint(MP_ID::SELECTED_CHANNEL);
		surface->SetLedByEnum(X32_LED_IN, (chanIdx <= 31));
		surface->SetLedByEnum(X32_LED_AUX, (chanIdx >= 32)&&(chanIdx <= 47));
		surface->SetLedByEnum(X32_LED_BUS, (chanIdx >= 48)&&(chanIdx <= 63));
		surface->SetLedByEnum(X32_LED_DCA, (chanIdx >= 64)&&(chanIdx <= 69));
		surface->SetLedByEnum(X32_LED_MAIN, (chanIdx >= 70)&&(chanIdx <= 71));
		surface->SetLedByEnum(X32_LED_MATRIX, (chanIdx >= 72)&&(chanIdx <= 79));

		// set 7-Segment Display
		surface->SetX32RackDisplay(chanIdx);        
}

// only X32 Core
void X32Ctrl::setLedChannelIndicator_Core(void){
		uint8_t chanIdx = config->GetUint(MP_ID::SELECTED_CHANNEL);
		surface->SetLedByEnum(X32_LED_IN, (chanIdx <= 31));
		surface->SetLedByEnum(X32_LED_AUX, (chanIdx >= 32)&&(chanIdx <= 47));
		surface->SetLedByEnum(X32_LED_BUS, (chanIdx >= 48)&&(chanIdx <= 63));
		surface->SetLedByEnum(X32_LED_DCA, (chanIdx >= 64)&&(chanIdx <= 69));
		surface->SetLedByEnum(X32_LED_MTX, (chanIdx >= 70)&&(chanIdx <= 79));
}

uint8_t X32Ctrl::surfaceCalcDynamicMeter(uint8_t channel) {
	// leds = 8-bit bitwise (bit 0=-60dB ... 4=-6dB, 5=Clip, 6=Gate, 7=Comp)
	if (channel < 40) {
		uint32_t meterdata = 0;

		if (mixer->dsp->Channel[channel].compressor.gain < 1.0f) { meterdata |= 0b10000000; };

		float gateValue = (1.0f - config->GetFloat(MP_ID::CHANNEL_GATE_GAIN, channel)) * 80.0f;
		if (gateValue >= 2.0f)  { meterdata |= 0b00100000; }        
		if (gateValue >= 4.0f)  { meterdata |= 0b00010000; }        
		if (gateValue >= 6.0f)  { meterdata |= 0b00001000; }        
		if (gateValue >= 10.0f) { meterdata |= 0b00000100; }        
		if (gateValue >= 18.0f) { meterdata |= 0b00000010; }        
		if (gateValue >= 30.0f) { meterdata |= 0b00000001; }        

		if (config->GetFloat(MP_ID::CHANNEL_GATE_GAIN, channel) < 1.0f) { meterdata |= 0b01000000; };

		return meterdata;
	}else{
		return 0; // no dynamic-data for other channels at the moment
	}
}

// sync mixer state to GUI
void X32Ctrl::syncXRemote(bool syncAll) {
	bool fullSync = false;

	if (syncAll || config->HasParameterChanged(MP_ID::SELECTED_CHANNEL)){ 
		// channel selection has changed - do a full sync
		fullSync=true; 
	}
	
	// DEBUG
	xremote->SetCard(10); // X-LIVE

	for(uint8_t i=0; i<(uint)X32_VCHANNELTYPE::NORMAL; i++) {
		uint8_t chanindex = i;
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
		uint8_t chanindex = i;
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
void X32Ctrl::ChangeSelect(int8_t direction){
	int16_t newSelectedVChannel = config->GetUint(MP_ID::SELECTED_CHANNEL) + direction;
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "ChangeSelect(): selected channel index: %d, direction: %d, new channel index: %d", config->GetUint(MP_ID::SELECTED_CHANNEL), direction, newSelectedVChannel);
	if (newSelectedVChannel < 0) {
		newSelectedVChannel = MAX_VCHANNELS -1;
	} else if (newSelectedVChannel >= MAX_VCHANNELS){
		newSelectedVChannel = 0;
	}

	config->Set(MP_ID::SELECTED_CHANNEL, newSelectedVChannel);
}


uint8_t X32Ctrl::SurfaceChannel2vChannel(uint8_t surfaceChannel){
	if ((config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) || (config->GetBankMode() == X32_SURFACE_MODE_BANKING_USER)){
		if (config->IsModelX32Full()){
			if (surfaceChannel <= 15){
				// input-section
				return modes[config->GetBankMode()].inputBanks[config->GetUint(BANKING_INPUT)].surfaceChannel2VChannel[surfaceChannel];
			} else if (surfaceChannel == 24) {
				// main-channel
				return 80;
			} else {
				// bus-section and mainfader
				return modes[config->GetBankMode()].busBanks[config->GetUint(BANKING_BUS)].surfaceChannel2VChannel[surfaceChannel-16];
			}
		}
		if (config->IsModelX32CompactOrProducer()){
			if (surfaceChannel <= 7){
				// input-section
				return modes[config->GetBankMode()].inputBanks[config->GetUint(BANKING_INPUT)].surfaceChannel2VChannel[surfaceChannel];
			} else if (surfaceChannel == 16) {
				// main-channel
				return 80;
			} else {
				// bus-section and mainfader
				return modes[config->GetBankMode()].busBanks[config->GetUint(BANKING_BUS)].surfaceChannel2VChannel[surfaceChannel-8];
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

void X32Ctrl::ProcessUartData() {
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

            if (valid){
                helper->DEBUG_SURFACE(DEBUGLEVEL_TRACE, "Callback: BoardId 0x%02X, Class 0x%02X, Index 0x%02X, Value 0x%04X", receivedBoardId, receivedClass, receivedIndex, receivedValue);
				ProcessEventsRaw(new SurfaceEvent((X32_BOARD)receivedBoardId, receivedClass, receivedIndex, receivedValue));
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
		config->Set(MP_ID::CHANNEL_VOLUME, helper->Fadervalue2dBfs(event->value), vchannelIndex);
		surface->FaderMoved(event);
	}
}

void X32Ctrl::ButtonPressedOrReleased(SurfaceEvent* event)
{
	using enum MP_ID;

	X32_BTN button = surface->Button2Enum[((uint16_t)event->boardId << 8) + (uint16_t)(event->value & 0x7F)];
	bool isButtonPressed = (event->value >> 7) == 1;

	helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "X32_BTN:%d", button);

	// Logic for double button press
	if (isButtonPressed) {
		if (buttonPressed == X32_BTN_NONE) {
			buttonPressed = button;
		} else {
			helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "DoubleButtonPress: Button1 %d, Button2 %d", buttonPressed, secondbuttonPressed);
			secondbuttonPressed = button;
		}
	} else {
		if (buttonPressed == button) {
			buttonPressed = X32_BTN_NONE;
		}
		if (secondbuttonPressed == button) {
			secondbuttonPressed = X32_BTN_NONE;
		}
	}

	// Standard button assingments
	if (isButtonPressed) {
		switch (button) {
			// Note: X32_BTN_UP and X32_BTN_DOWN are handled in the page classes!
			case X32_BTN_LEFT:
				ShowPrevPage();
				break;
			case X32_BTN_RIGHT:
				ShowNextPage();
				break;
			case X32_BTN_HOME:
				ShowPage(X32_PAGE::HOME);
				break;
			case X32_BTN_VIEW_CONFIG:
				ShowPage(X32_PAGE::CONFIG);
				break;
			case X32_BTN_VIEW_GATE:
				ShowPage(X32_PAGE::GATE);
				break;
			case X32_BTN_VIEW_COMPRESSOR:
				ShowPage(X32_PAGE::COMPRESSOR);
				break;
			case X32_BTN_VIEW_EQ:
				ShowPage(X32_PAGE::EQ);
				break;
			case X32_BTN_METERS:
				ShowPage(X32_PAGE::METERS);
				break;
			case X32_BTN_ROUTING:
				ShowPage(X32_PAGE::ROUTING);
				break;
			case X32_BTN_SETUP:
				ShowPage(X32_PAGE::SETUP);
				break;
			case X32_BTN_LIBRARY:
				ShowPage(X32_PAGE::LIBRARY);
				break;
			case X32_BTN_EFFECTS:
				ShowPage(X32_PAGE::EFFECTS);
				break;
			case X32_BTN_MUTE_GRP:
				ShowPage(X32_PAGE::MUTE_GRP);
				break;
			case X32_BTN_UTILITY:
				ShowPage(X32_PAGE::UTILITY);
				break;
			case X32_BTN_EQ_MODE:
				config->Change(((MP_ID)((uint)CHANNEL_EQ_TYPE1 + config->GetUint(BANKING_EQ))), +1, config->GetUint(SELECTED_CHANNEL));
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
			default:
				// just here to avoid compiler warnings
				break;
		}
	}


	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		if (isButtonPressed){
			switch (button) {
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
					config->Toggle(CHANNEL_SELECTED, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_SELECT));
					break;
				case X32_BTN_BOARD_M_CH_1_SELECT:
				case X32_BTN_BOARD_M_CH_2_SELECT:
				case X32_BTN_BOARD_M_CH_3_SELECT:
				case X32_BTN_BOARD_M_CH_4_SELECT:
				case X32_BTN_BOARD_M_CH_5_SELECT:
				case X32_BTN_BOARD_M_CH_6_SELECT:
				case X32_BTN_BOARD_M_CH_7_SELECT:
				case X32_BTN_BOARD_M_CH_8_SELECT:
					config->Toggle(CHANNEL_SELECTED, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_SELECT));
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
					config->Toggle(CHANNEL_SELECTED, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_SELECT));
					break;
				case X32_BTN_BOARD_L_CH_1_SOLO:
				case X32_BTN_BOARD_L_CH_2_SOLO:
				case X32_BTN_BOARD_L_CH_3_SOLO:
				case X32_BTN_BOARD_L_CH_4_SOLO:
				case X32_BTN_BOARD_L_CH_5_SOLO:
				case X32_BTN_BOARD_L_CH_6_SOLO:
				case X32_BTN_BOARD_L_CH_7_SOLO:
				case X32_BTN_BOARD_L_CH_8_SOLO:
					config->Toggle(CHANNEL_SOLO, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_SOLO));
					break;
				case X32_BTN_BOARD_M_CH_1_SOLO:
				case X32_BTN_BOARD_M_CH_2_SOLO:
				case X32_BTN_BOARD_M_CH_3_SOLO:
				case X32_BTN_BOARD_M_CH_4_SOLO:
				case X32_BTN_BOARD_M_CH_5_SOLO:
				case X32_BTN_BOARD_M_CH_6_SOLO:
				case X32_BTN_BOARD_M_CH_7_SOLO:
				case X32_BTN_BOARD_M_CH_8_SOLO:
					config->Toggle(CHANNEL_SOLO, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_SOLO));
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
					config->Toggle(CHANNEL_SOLO, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_SOLO));
					break;
				case X32_BTN_BOARD_L_CH_1_MUTE:
				case X32_BTN_BOARD_L_CH_2_MUTE:
				case X32_BTN_BOARD_L_CH_3_MUTE:
				case X32_BTN_BOARD_L_CH_4_MUTE:
				case X32_BTN_BOARD_L_CH_5_MUTE:
				case X32_BTN_BOARD_L_CH_6_MUTE:
				case X32_BTN_BOARD_L_CH_7_MUTE:
				case X32_BTN_BOARD_L_CH_8_MUTE:
					config->Toggle(CHANNEL_MUTE, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_L_CH_1_MUTE));
					break;
				case X32_BTN_BOARD_M_CH_1_MUTE:
				case X32_BTN_BOARD_M_CH_2_MUTE:
				case X32_BTN_BOARD_M_CH_3_MUTE:
				case X32_BTN_BOARD_M_CH_4_MUTE:
				case X32_BTN_BOARD_M_CH_5_MUTE:
				case X32_BTN_BOARD_M_CH_6_MUTE:
				case X32_BTN_BOARD_M_CH_7_MUTE:
				case X32_BTN_BOARD_M_CH_8_MUTE:
					config->Toggle(CHANNEL_MUTE, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_M_CH_1_MUTE));
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
					config->Toggle(CHANNEL_MUTE, GetvChannelIndexFromButtonOrFaderIndex(event->boardId, button - X32_BTN_BOARD_R_CH_1_MUTE));
					break;
				case X32_BTN_CLEAR_SOLO:
					mixer->ClearSolo();
					break;
				case X32_BTN_PHANTOM_48V:
					config->Toggle(CHANNEL_PHANTOM, config->GetUint(SELECTED_CHANNEL));
					break;
				case X32_BTN_PHASE_INVERT:
					config->Toggle(CHANNEL_PHASE_INVERT, config->GetUint(SELECTED_CHANNEL));
					break;
				case X32_BTN_CHANNEL_SOLO: // only X32 Rack
					config->Toggle(CHANNEL_SOLO, config->GetUint(SELECTED_CHANNEL));
					break;
				case X32_BTN_CHANNEL_MUTE: // only X32 Rack
					config->Toggle(CHANNEL_MUTE, config->GetUint(SELECTED_CHANNEL));
					break;
				case X32_BTN_SCENE_SETUP:
					if (config->IsModelX32Core()) {
						state->x32core_lcdmode_setup = !state->x32core_lcdmode_setup;
						if (state->x32core_lcdmode_setup) {
							surface->SetLedByEnum(X32_BTN_SCENE_SETUP, 1);
							lcdmenu->OnShow();
						} else {
							surface->SetLedByEnum(X32_BTN_SCENE_SETUP, 0);
							// trigger switch to channel lcd
							//state->SetChangeFlags(X32_MIXER_CHANGED_SELECT);
						}
					}
					break;
				case X32_BTN_ASSIGN_3:
					break;
				case X32_BTN_ASSIGN_5:
					break;
				case X32_BTN_ASSIGN_6:
					if (config->IsModelX32Core()) {
						config->Set(CARD_NUMBER_OF_CHANNELS, CARD_CHANNELMODE_32IN_32OUT);
						mixer->fpga->RoutingCardAs32CHInput();
					}
					break;
				default:
					helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Unhandled button detected.\n");
					break;
			}
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()){
		switch (button){
			case X32_BTN_UP:
			case X32_BTN_DOWN:
			case X32_BTN_ENCODER1:
			case X32_BTN_ENCODER2:
			case X32_BTN_ENCODER3:
			case X32_BTN_ENCODER4:		
			case X32_BTN_ENCODER5:				
			case X32_BTN_ENCODER6:
				pages[(X32_PAGE)config->GetUint(ACTIVE_PAGE)]->DisplayButton(button, isButtonPressed);
				break;
			default:
				break;
		}
	}
}

void X32Ctrl::EncoderTurned(SurfaceEvent* event)
{
	using enum MP_ID;

	X32_ENC encoder = surface->Encoder2Enum[((uint16_t)event->boardId << 8) + (uint16_t)(event->index)];
	int8_t amount = 0;
	int8_t newValue;

	if (event->value > 0 && event->value < 128){
		amount = (uint8_t)event->value;
	} else if (event->value > 128 && event->value < 256) {
		amount = -(256 - (uint8_t)(event->value));
	}
	helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Encoder amount: %d", amount);

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		switch (encoder){
			case X32_ENC_ASSIGN_1: // use Assing 1 temporarly on X32 Core for Channel Volume
				config->Change(CHANNEL_VOLUME, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_ASSIGN_2: // use Assing 2 temporarly on X32 Core for Main Volume
				config->Change(CHANNEL_VOLUME, amount, (uint)X32_VCHANNEL_BLOCK::MAIN);
				break;
			case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core
				if (config->IsModelX32Core() && state->x32core_lcdmode_setup) {
					lcdmenu->OnLcdEncoderTurned(amount);
				} else {
					ChangeSelect(amount);
				}
				break;
			case X32_ENC_MAIN_LEVEL:  // only X32 Rack
				config->Change(CHANNEL_VOLUME, amount, (uint)X32_VCHANNEL_BLOCK::MAIN);
				break;
			case X32_ENC_CHANNEL_LEVEL:
				config->Change(CHANNEL_VOLUME, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_GAIN:
				config->Change(CHANNEL_GAIN, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_GATE:
				config->Change(CHANNEL_GATE_TRESHOLD, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_LOWCUT:
				config->Change(CHANNEL_LOWCUT, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_DYNAMICS:
				config->Change(CHANNEL_DYNAMICS_TRESHOLD, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_EQ_Q:
				config->Change(((MP_ID)((uint)CHANNEL_EQ_Q1 + config->GetUint(BANKING_EQ))), amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_EQ_FREQ:
				config->Change(((MP_ID)((uint)CHANNEL_EQ_FREQ1 + config->GetUint(BANKING_EQ))), amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_EQ_GAIN:
				config->Change(((MP_ID)((uint)CHANNEL_EQ_GAIN1 + config->GetUint(BANKING_EQ))), amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_PAN:
				config->Change(CHANNEL_PANORAMA, amount, config->GetUint(SELECTED_CHANNEL));
				break;
			case X32_ENC_BUS_SEND_1:
				mixer->ChangeBusSend(config->GetUint(SELECTED_CHANNEL), 0, amount, config->GetUint(BANKING_BUS_SENDS));
				break;
			case X32_ENC_BUS_SEND_2:
				mixer->ChangeBusSend(config->GetUint(SELECTED_CHANNEL), 1, amount, config->GetUint(BANKING_BUS_SENDS));
				break;
			case X32_ENC_BUS_SEND_3:
				mixer->ChangeBusSend(config->GetUint(SELECTED_CHANNEL), 2, amount, config->GetUint(BANKING_BUS_SENDS));
				break;
			case X32_ENC_BUS_SEND_4:
				mixer->ChangeBusSend(config->GetUint(SELECTED_CHANNEL), 3, amount, config->GetUint(BANKING_BUS_SENDS));
				break;
			default:
				// just here to avoid compiler warnings                  
				break;
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()) {
		switch (encoder){
			case X32_ENC_ENCODER1:
			case X32_ENC_ENCODER2:
			case X32_ENC_ENCODER3:
			case X32_ENC_ENCODER4:
			case X32_ENC_ENCODER5:
			case X32_ENC_ENCODER6:
				pages[(X32_PAGE)config->GetUint(ACTIVE_PAGE)]->DisplayEncoderTurned(encoder, amount);
				break;
			default:  
				// just here to avoid compiler warnings                  
				break;
		}
	}
}

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
void X32Ctrl::SimulatorButton(uint key)
{
	if (key != state->simulator_last_key)
	{
		printf("Simulatorbutton: %d\n", key);

		X32_PAGE activePage = (X32_PAGE)config->GetUint(MP_ID::ACTIVE_PAGE);

		switch (key)
		{
			using enum X32_PAGE;

			case 49:
				// HOME
				ShowPage(HOME);
				break;
			case 50:
				ShowPage(METERS);
				break;
			case 51:
				ShowPage(ROUTING);
				break;
			case 52:
				ShowPage(LIBRARY);
				break;
			case 53:
				ShowPage(EFFECTS);
				break;
			case 54:
				ShowPage(SETUP);
				break;
			case 55:
				//ShowPage(MONITOR);
				break;
			case 56:
				//ShowPage(SCENES);
				break;
			case 57:
				ShowPage(MUTE_GRP);
				break;
			case 48:
				ShowPage(UTILITY);
				break;
			case 17:
				pages[activePage]->DisplayButton(X32_BTN_UP, true);
				pages[activePage]->DisplayButton(X32_BTN_UP, false);
				break;
			case 18:
				pages[activePage]->DisplayButton(X32_BTN_DOWN, true);
				pages[activePage]->DisplayButton(X32_BTN_DOWN, false);
				break;
			case 20:
				ShowPrevPage();
				break;
			case 19:
				ShowNextPage();
				break;
			case 113: // Q
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER1, 1);
				break;
			case 119: // W
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER2, 1);
				break;
			case 101: // E
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER3, 1);
				break;
			case 114: // R
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER4, 1);
				break;
			case 116: // T
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER5, 1);
				break;
			case 122: // Z
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER6, 1);
				break;
			case 97: // A
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER1, -1);
				break;
			case 115: // S
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER2, -1);
				break;
			case 100: // D
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER3, -1);
				break;
			case 102: // F
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER4, -1);
				break;
			case 103: // G
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER5, -1);
				break;
			case 104: // H
				pages[activePage]->DisplayEncoderTurned(X32_ENC_ENCODER6, -1);
				break;
			case 121: // Y
				pages[activePage]->DisplayButton(X32_BTN_ENCODER1, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER1, false);
				break;
			case 120: // X
				pages[activePage]->DisplayButton(X32_BTN_ENCODER2, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER2, false);
				break;
			case 99: // C
				pages[activePage]->DisplayButton(X32_BTN_ENCODER3, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER3, false);
				break;
			case 118: // V
				pages[activePage]->DisplayButton(X32_BTN_ENCODER4, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER4, false);
				break;
			case 98: // B
				pages[activePage]->DisplayButton(X32_BTN_ENCODER5, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER5, false);
				break;
			case 110: // N
				pages[activePage]->DisplayButton(X32_BTN_ENCODER6, true);
				pages[activePage]->DisplayButton(X32_BTN_ENCODER6, false);
				break;
		}
		
		state->simulator_last_key = key;
	}	
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


void X32Ctrl::BankingSends(X32_BTN p_button) {
	surface->SetLedByEnum(X32_BTN_BUS_SEND_1_4, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_5_8, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_9_12, 0);
	surface->SetLedByEnum(X32_BTN_BUS_SEND_13_16, 0);

	switch (p_button){
		case X32_BTN_BUS_SEND_1_4:
			config->Set(BANKING_BUS_SENDS, 0);
			surface->SetLedByEnum(X32_BTN_BUS_SEND_1_4, 1);
			break;
		case X32_BTN_BUS_SEND_5_8:
			config->Set(BANKING_BUS_SENDS, 1);
			surface->SetLedByEnum(X32_BTN_BUS_SEND_5_8, 1);
			break;
		case X32_BTN_BUS_SEND_9_12:
			config->Set(BANKING_BUS_SENDS, 2);
			surface->SetLedByEnum(X32_BTN_BUS_SEND_9_12, 1);
			break;
		case X32_BTN_BUS_SEND_13_16:
			config->Set(BANKING_BUS_SENDS, 3);
			surface->SetLedByEnum(X32_BTN_BUS_SEND_13_16, 1);
			break;
		default:
			break;
	}
}

void X32Ctrl::BankingEQ(X32_BTN p_button){
	surface->SetLedByEnum(X32_BTN_EQ_LOW, 0);
	surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, 0);
	surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, 0);
	surface->SetLedByEnum(X32_BTN_EQ_HIGH, 0);

	uint newBankingEq;

	switch (p_button){
		case X32_BTN_EQ_LOW:
			newBankingEq = 0;
			surface->SetLedByEnum(X32_BTN_EQ_LOW, 1);
			break;
		case X32_BTN_EQ_LOW_MID:
			newBankingEq = 1;
			surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, 1);
			break;
		case X32_BTN_EQ_HIGH_MID:
			newBankingEq = 2;
			surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
			break;
		case X32_BTN_EQ_HIGH:
			newBankingEq = 3;
			surface->SetLedByEnum(X32_BTN_EQ_HIGH, 1);
			break;
		// TODO: implement LOW2 and HIGH2
		default:
			break;
	}

	config->Set(MP_ID::BANKING_EQ, newBankingEq);
}

void X32Ctrl::Banking(X32_BTN p_button)
{
	if (config->IsModelX32Full()){
		switch (p_button){
			case X32_BTN_CH_1_16:
				config->Set(BANKING_INPUT, 0);
				break;
			case X32_BTN_CH_17_32:
				config->Set(BANKING_INPUT, 1);
				break;
			case X32_BTN_AUX_IN_EFFECTS:
				config->Set(BANKING_INPUT, 2);
				break;
			case X32_BTN_BUS_MASTER:
				config->Set(BANKING_INPUT, 3);
				break;
			case X32_BTN_GROUP_DCA_1_8:
				config->Set(BANKING_BUS, 0);
				break;
			case X32_BTN_BUS_1_8:
				config->Set(BANKING_BUS, 1);
				break;
			case X32_BTN_BUS_9_16:
				config->Set(BANKING_BUS, 2);
				break;
			case X32_BTN_MATRIX_MAIN_C:
				config->Set(BANKING_BUS, 3);
				break;
			default:
				break;
		}
	}
	if (config->IsModelX32CompactOrProducer()){
		switch (p_button){
			case X32_BTN_CH_1_8:
				config->Set(BANKING_INPUT, 0);
				break;
			case X32_BTN_CH_9_16:
				config->Set(BANKING_INPUT, 1);
				break;
			case X32_BTN_CH_17_24:
				config->Set(BANKING_INPUT, 2);
				break;
			case X32_BTN_CH_25_32:
				config->Set(BANKING_INPUT, 3);
				break;
			case X32_BTN_AUX_IN_1_6_USB_REC:
				config->Set(BANKING_INPUT, 4);
				break;
			case X32_BTN_EFFECTS_RETURNS:
				config->Set(BANKING_INPUT, 5);
				break;
			case X32_BTN_BUS_1_8_MASTER:
				config->Set(BANKING_INPUT, 6);
				break;
			case X32_BTN_BUS_9_16_MASTER:
				config->Set(BANKING_INPUT, 7);
				break;
			case X32_BTN_GROUP_DCA_1_8:
				config->Set(BANKING_BUS, 0);
				break;
			case X32_BTN_BUS_1_8:
				config->Set(BANKING_BUS, 1);
				break;
			case X32_BTN_BUS_9_16:
				config->Set(BANKING_BUS, 2);
				break;
			case X32_BTN_MATRIX_MAIN_C:
				config->Set(BANKING_BUS, 3);
				break;
			default:
				break;
		}
	}
}
