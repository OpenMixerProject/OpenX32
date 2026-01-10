#include "ctrl.h"

X32Ctrl::X32Ctrl(X32BaseParameter* basepar) : X32Base(basepar) {
	mixer = new Mixer(basepar);
	surface = new Surface(basepar);
	xremote = new XRemote(basepar);
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
	
	config->SetModel(model);
	config->SetBankMode(X32_SURFACE_MODE_BANKING_X32);

	//##################################################################################
	//#
	//# 	Initialize system
	//#
	//##################################################################################

	mixer->Init();
	surface->Init();
	xremote->Init();

	if(config->IsModelX32FullOrCompactOrProducer()) {

		ResetFaderBankLayout();
		LoadFaderBankLayout(FADER_BANK_LAYOUT_X32);

		activeBank_inputFader = 0;
		activeBank_busFader = 0;
		state->activeEQ = 0;
		activeBusSend = 0;
	}

	SetSelect(0, true);
	
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

	// trigger first sync to gui/surface
	state->SetChangeFlags(X32_MIXER_CHANGED_ALL);
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

	// mixer config
	{
		string section = string("mixer");
		state->activePage = (X32_PAGE)mixer_ini[section]["activePage"].as<int>();
	}
	// VChannels
	for (uint8_t i = 0; i < MAX_VCHANNELS; i++)	{
		VChannel* chan = GetVChannel(i);
		string section = (String("vchannel") + String(i)).c_str();

		chan->name = mixer_ini[section]["name"].as<string>().c_str();
		chan->color = mixer_ini[section]["color"].as<int>();
	}
	// FPGA -> DSP1 Channels
	for (uint8_t i = 0; i < MAX_FPGA_TO_DSP1_CHANNELS; i++)	{
		string section = string("dspchannel") + to_string(i);
		mixer->dsp->Channel[i].input = mixer_ini[section]["inputSource"].as<int>();
		mixer->dsp->Channel[i].inputTapPoint = mixer_ini[section]["inputTapPoint"].as<int>();
	}
	// DSP1 -> FPGA Channels
	for (uint8_t i = 0; i < MAX_DSP1_TO_FPGA_CHANNELS; i++) {
		string section = string("dspoutchannel") + to_string(i);
		mixer->dsp->Dsp1toFpga[i].input = mixer_ini[section]["outputSource"].as<int>();
		mixer->dsp->Dsp1toFpga[i].tapPoint = mixer_ini[section]["outputTapPoint"].as<int>();
	}
	// DSP1 -> DSP2 Channels
	for (uint8_t i = 0; i < MAX_DSP1_TO_DSP2_CHANNELS; i++) {
		string section = string("dspfxchannel") + to_string(i);
		mixer->dsp->Dsp1toDsp2Routing[i].input = mixer_ini[section]["outputSource"].as<int>();
		mixer->dsp->Dsp1toDsp2Routing[i].tapPoint = mixer_ini[section]["outputTapPoint"].as<int>();
	}
	mixer->dsp->SendAll();

	// FPGA Routing
	{
		string section = "fpga";
		for (uint8_t i = 0; i < 8; i++)	{
			mixer->fpga->fpgaRouting.aux[i] = mixer_ini[section][(String("aux") + i).c_str()].as<int>();
		}
		for (uint8_t i = 0; i < 16; i++) {
			mixer->fpga->fpgaRouting.xlr[i] = mixer_ini[section][(String("xlr") + i).c_str()].as<int>();
			mixer->fpga->fpgaRouting.p16[i] = mixer_ini[section][(String("p16") + i).c_str()].as<int>();
		}
		for (uint8_t i = 0; i < 32; i++) {
			mixer->fpga->fpgaRouting.card[i] = mixer_ini[section][(String("card") + i).c_str()].as<int>();
		}
		for (uint8_t i = 0; i < 40; i++) {
			mixer->fpga->fpgaRouting.dsp[i] = mixer_ini[section][(String("dsp") + i).c_str()].as<int>();
		}
		mixer->fpga->SendRoutingToFpga(-1);
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
	// mixer config
	{
		string section = string("mixer");
		mixer_ini[section]["activePage"] = (int)state->activePage;
	}
	// VChannels
	for (uint8_t i = 0; i < MAX_VCHANNELS; i++) {
		VChannel* chan = GetVChannel(i);
		string section = string("vchannel") + to_string(i);
		mixer_ini[section]["name"] = chan->name.c_str();
		mixer_ini[section]["color"] = (int)chan->color;
	}
	// FPGA -> DSP1 Channels
	for (uint8_t i = 0; i < MAX_FPGA_TO_DSP1_CHANNELS; i++)	{
		string section = string("dspchannel") + to_string(i);
		mixer_ini[section]["inputSource"] = (int)mixer->dsp->Channel[i].input;
		mixer_ini[section]["inputTapPoint"] = (int)mixer->dsp->Channel[i].inputTapPoint;
	}
	// DSP1 -> FPGA Channels
	for (uint8_t i = 0; i < MAX_DSP1_TO_FPGA_CHANNELS; i++) {
		string section = string("dspoutchannel") + to_string(i);
		mixer_ini[section]["outputSource"] = (int)mixer->dsp->Dsp1toFpga[i].input;
		mixer_ini[section]["outputTapPoint"] = (int)mixer->dsp->Dsp1toFpga[i].tapPoint;
	}
	// DSP1 -> DSP2 Channels
	for (uint8_t i = 0; i < MAX_DSP1_TO_DSP2_CHANNELS; i++) {
		string section = string("dspfxchannel") + to_string(i);
		mixer_ini[section]["outputSource"] = (int)mixer->dsp->Dsp1toDsp2Routing[i].input;
		mixer_ini[section]["outputTapPoint"] = (int)mixer->dsp->Dsp1toDsp2Routing[i].tapPoint;
	}
	{
		string section = "fpga";
		for (uint8_t i = 0; i < 8; i++)	{
			mixer_ini[section][(String("aux") + i).c_str()] = (int)mixer->fpga->fpgaRouting.aux[i];
		}
		for (uint8_t i = 0; i < 16; i++) {
			mixer_ini[section][(String("xlr") + i).c_str()] = (int)mixer->fpga->fpgaRouting.xlr[i];
			mixer_ini[section][(String("p16") + i).c_str()] = (int)mixer->fpga->fpgaRouting.p16[i];
		}
		for (uint8_t i = 0; i < 32; i++) {
			mixer_ini[section][(String("card") + i).c_str()] = (int)mixer->fpga->fpgaRouting.card[i];
		}
		for (uint8_t i = 0; i < 40; i++) {
			mixer_ini[section][(String("dsp") + i).c_str()] = (int)mixer->fpga->fpgaRouting.dsp[i];
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

	surface->Touchcontrol();	
	mixer->dsp->ReadAndUpdateVUMeterData();

	ProcessUartData();

	// communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
	UdpHandleCommunication();

	// sync all state-based events
	if (state->HasAnyChanged()){

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "SyncAll ");

		syncGui();
		syncSurface();
		syncXRemote(false);

		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
			mixer->Sync();
		}

		state->ResetChangeFlags();
		for(uint8_t index = 0; index < MAX_VCHANNELS; index++){
			mixer->GetVChannel(index)->ResetVChannelChangeFlags();
		}
	}
}

void X32Ctrl::Tick50ms(void) {

	UpdateMeters();
}

void X32Ctrl::Tick100ms(void) {

	surface->Blink();
	mixer->dsp->spi->ActivityLight();

	if (!config->IsModelX32Core() && state->activePage == X32_PAGE_UTILITY) {
		// read the current DSP load
	 	lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% [v%.2f]", (double)state->dspLoad[0], (double)state->dspVersion[0], (double)state->dspLoad[1], (double)state->dspVersion[1]); // show the received value (could be a bit older than the request)
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
		ButtonPressed(event);
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
						helper->DEBUG_XREMOTE(DEBUGLEVEL_VERBOSE, "Ch %u: Volume set to %f\n", channel, (double)newVolume);
					}else if (address[3] == "pan") {
						// get pan-value
						value32bit.u8[0] = rxData[23];
						value32bit.u8[1] = rxData[22];
						value32bit.u8[2] = rxData[21];
						value32bit.u8[3] = rxData[20];
						
						//encoderValue = value32bit.f * 255.0f;
						mixer->SetBalance(channel,  value32bit.f * 100.0f);
						helper->DEBUG_XREMOTE(DEBUGLEVEL_VERBOSE, "Ch %u: Balance set to %f\n",  channel+1, (double)(value32bit.f * 100.0f));
					}else if (address[3] == "on") {
						// get mute-state (caution: here it is "mixer-on"-state)
						mixer->SetMute(channel, (rxData[20+3] == 0));
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

void X32Ctrl::InitPages(){
	if(!(config->IsModelX32FullOrCompactOrProducerOrRack())) {
		return;
	}
	PageBaseParameter* pagebasepar = new PageBaseParameter(app, config, state, helper, mixer);
	pages[X32_PAGE_HOME] = new PageHome(pagebasepar);
	pages[X32_PAGE_CONFIG] = new PageConfig(pagebasepar);
	pages[X32_PAGE_GATE] = new PageGate(pagebasepar);
	pages[X32_PAGE_COMPRESSOR] = new PageDynamics(pagebasepar);
	pages[X32_PAGE_EQ] = new PageEq(pagebasepar);
	pages[X32_PAGE_METERS] = new PageMeters(pagebasepar);
	pages[X32_PAGE_ROUTING] = new PageRouting(pagebasepar);
	pages[X32_PAGE_ROUTING_FPGA] = new PageRoutingFpga(pagebasepar);
	pages[X32_PAGE_ROUTING_DSP1] = new PageRoutingDsp1(pagebasepar);
	pages[X32_PAGE_ROUTING_DSP2] = new PageRoutingDsp2(pagebasepar);
	pages[X32_PAGE_SETUP] = new PageSetup(pagebasepar);
	pages[X32_PAGE_LIBRARY] = new PageLibrary(pagebasepar);
	pages[X32_PAGE_EFFECTS] = new PageEffects(pagebasepar);
	pages[X32_PAGE_MUTE_GRP] = new PageMutegroup(pagebasepar);
	pages[X32_PAGE_UTILITY] = new PageUtility(pagebasepar);
	for (const auto& [key, value] : pages) {
    	value->Init();
	}
}

void X32Ctrl::ShowNextPage(void){
	X32_PAGE nextPage = pages[state->activePage]->GetNextPage();
	if (nextPage != X32_PAGE_NONE){
	 	ShowPage(nextPage);
	}
}

void X32Ctrl::ShowPrevPage(void){
	X32_PAGE prevPage = pages[state->activePage]->GetPrevPage();
	if (prevPage != X32_PAGE_NONE){
	 	ShowPage(prevPage);
	}
}

void X32Ctrl::ShowPage(X32_PAGE newPage) {

	// only work's on devices with display
	if (!(config->IsModelX32FullOrCompactOrProducerOrRack()))
	{
		return;
	}
	
	if (newPage == state->activePage) {
		// operator has pressed the button of the current active page,
		// so go back to previous page
		newPage = state->lastPage;
	}

	if (pages[newPage] != nullptr) {

		state->lastPage = state->activePage;
		state->activePage = newPage;
		
		Page* p = pages[state->activePage];
		Page* l = pages[state->lastPage];

		// turn off led of prev page
		if (l->GetLed() != X32_BTN_NONE) {
			surface->SetLedByEnum(l->GetLed(), false);
		}

		// turn on led of new active page
		if (p->GetLed() != X32_BTN_NONE) {
			surface->SetLedByEnum(p->GetLed(), true);
		}

		state->SetChangeFlags(X32_MIXER_CHANGED_PAGE);
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

void X32Ctrl::syncAll(void) {
	
}

// sync mixer state to GUI
void X32Ctrl::syncGui() {
	if (config->IsModelX32Core()){
		return;
	}

	// if these have not changed, do nothing else
	if (!(
		state->HasChanged(X32_MIXER_CHANGED_PAGE)  		||
		state->HasChanged(X32_MIXER_CHANGED_SELECT)  	||
		state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)	||
		state->HasChanged(X32_MIXER_CHANGED_ROUTING) 	||
		state->HasChanged(X32_MIXER_CHANGED_METER)		||
		state->HasChanged(X32_MIXER_CHANGED_GUI)
		)) {
		return;
	}
	
	//####################################
	//#       Init / Update Page
	//####################################

	if (state->HasChanged(X32_MIXER_CHANGED_PAGE)){
		helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page changed to: %d\n", state->activePage);
		pages[state->activePage]->Show();
	}

	pages[state->activePage]->Change();

	//####################################
	//#     Update General Header
	//####################################

	if (state->HasChanged(X32_MIXER_CHANGED_SELECT)) {
		VChannel* chan = GetSelectedvChannel();
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
			
		}
		surfaceSyncBoardExtra();
	}
}

void X32Ctrl::surfaceSyncBoardMain() {
	bool needForSync = false;
	bool fullSync = false;
	VChannel* chan = GetSelectedvChannel();
	uint8_t chanIndex = config->selectedVChannel;

	if (state->HasChanged(X32_MIXER_CHANGED_SELECT)){ 
		// channel selection has changed - do a full sync
		needForSync=true;
		fullSync=true; 
	}

	if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL) && chan->HasAnyChanged()) {
		// the data in the currently selected channel has changed
		needForSync=true;
	}

	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "needForSync %d, fullSync %d", needForSync, fullSync);

	if (needForSync){
		if (config->IsModelX32FullOrCompactOrProducer()){
			
			// Phantom
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_PHANTOM)){
				surface->SetLedByEnum(X32_BTN_PHANTOM_48V, mixer->GetPhantomPower(chanIndex)); 
			}
			// Phase Invert
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_PHASE_INVERT)){
				surface->SetLedByEnum(X32_BTN_PHASE_INVERT, mixer->GetPhaseInvert(chanIndex));
			}
			// Gain
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GAIN)){
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_GAIN] >> 8, surface->Enum2Encoder[X32_ENC_GAIN] & 0xFF, 0, (mixer->GetGain(chanIndex) + 12.0f)/0.72f, 1);
			}
			// Balance/Panorama
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)){
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_PAN] >> 8, surface->Enum2Encoder[X32_ENC_PAN] & 0xFF, 2, (mixer->GetBalance(chanIndex) + 100.0f)/2.0f, 1);
			}
			// Bus sends
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)){
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_1] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_1] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 0)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_2] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_2] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 1)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_3] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_3] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 2)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_BUS_SEND_4] >> 8, surface->Enum2Encoder[X32_ENC_BUS_SEND_4] & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 3)/20.0f) * 100.0f, 1);
			}
			// Gate
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_GATE] >> 8, surface->Enum2Encoder[X32_ENC_GATE] & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].gate.threshold + 80.0f)/0.8f), 1);
			}
			// Dynamics
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
				surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_DYNAMICS] >> 8, surface->Enum2Encoder[X32_ENC_DYNAMICS] & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].compressor.threshold + 60.0f)/0.6f), 1);
			}
			// EQ
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)){
				if (chanIndex < 40) {
					surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_LOWCUT] >> 8, surface->Enum2Encoder[X32_ENC_LOWCUT] & 0xFF, 1, (mixer->dsp->Channel[chanIndex].lowCutFrequency - 20.0f)/3.8f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_FREQ] >> 8, surface->Enum2Encoder[X32_ENC_EQ_FREQ] & 0xFF, 1, (mixer->dsp->Channel[chanIndex].peq[state->activeEQ].fc - 20.0f)/199.8f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_GAIN] >> 8, surface->Enum2Encoder[X32_ENC_EQ_GAIN] & 0xFF, 2, (mixer->dsp->Channel[chanIndex].peq[state->activeEQ].gain + 15.0f)/0.3f, 1);
					surface->SetEncoderRing(surface->Enum2Encoder[X32_ENC_EQ_Q] >> 8, surface->Enum2Encoder[X32_ENC_EQ_Q] & 0xFF, 3, ((10.0f - mixer->dsp->Channel[chanIndex].peq[state->activeEQ].Q) + 0.3f)/0.097f, 1);
					
					// EQ-LEDS
					surface->SetLedByEnum(X32_LED_EQ_HCUT, false);
					surface->SetLedByEnum(X32_LED_EQ_HSHV, false);
					surface->SetLedByEnum(X32_LED_EQ_LCUT, false);
					surface->SetLedByEnum(X32_LED_EQ_LSHV, false);
					surface->SetLedByEnum(X32_LED_EQ_PEQ, false);
					surface->SetLedByEnum(X32_LED_EQ_VEQ, false);

					switch (mixer->dsp->Channel[chanIndex].peq[state->activeEQ].type) {
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

					surface->SetLedByEnum(X32_BTN_EQ_LOW, state->activeEQ == 0);
					surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, state->activeEQ == 1);
					surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, state->activeEQ == 2);
					surface->SetLedByEnum(X32_BTN_EQ_HIGH, state->activeEQ == 3);
				}
			}
		}

		if (config->IsModelX32Rack()){			
			if (state->HasChanged(X32_MIXER_CHANGED_SELECT)){
				setLedChannelIndicator();
			}
			// Solo
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
				surface->SetLedByEnum(X32_BTN_CHANNEL_SOLO, mixer->GetSolo(chanIndex)); 
			}
			// Mute
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
				surface->SetLedByEnum(X32_BTN_CHANNEL_MUTE, mixer->GetMute(chanIndex)); 
			}
			// Volume
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
				surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 0, mixer->GetVolumeDbfs(chanIndex),  mixer->GetMute(chanIndex), 0);
			}
		}
	}

	if (config->IsModelX32Rack()){
		// Clear Solo
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)){
			bool soloActive = mixer->IsSoloActivated();
			surface->SetLedByEnum(X32_BTN_CLEAR_SOLO, soloActive, soloActive);
		}
		// Main Channel
		VChannel* mainchan = GetVChannel(X32_VCHANNEL_BLOCK_MAIN);
		if (mainchan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME) || mainchan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
			surface->SetEncoderRingDbfs(X32_BOARD_MAIN, 1, mixer->GetVolumeDbfs(X32_VCHANNEL_BLOCK_MAIN),  mixer->GetMute(X32_VCHANNEL_BLOCK_MAIN), 0);
		}
	}
}

void X32Ctrl::surfaceSyncBoard(X32_BOARD p_board) {
	bool fullSync = false;

	if (config->IsModelX32Full()){
		if (state->HasChanged(X32_MIXER_CHANGED_BANKING_INPUT) && ((p_board == X32_BOARD_L) || (p_board == X32_BOARD_M))){ fullSync=true; }
		if (state->HasChanged(X32_MIXER_CHANGED_BANKING_BUS) && p_board == X32_BOARD_R){ fullSync=true; }
	}

	if (config->IsModelX32CompactOrProducer()){
		if (state->HasChanged(X32_MIXER_CHANGED_BANKING_INPUT) && p_board == X32_BOARD_L){ fullSync=true; }
		if (state->HasChanged(X32_MIXER_CHANGED_BANKING_BUS) && p_board == X32_BOARD_R){ fullSync=true; }
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
				helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "syncronize vChannel%d: %s -", channelIndex, chan->name.c_str());

				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SELECT)){ 
					helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Select");
					surface->SetLed(p_board, 0x20+i, chan->selected);
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SOLO)){
					helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Solo");
					surface->SetLed(p_board, 0x30+i, mixer->GetSolo(channelIndex)); 
				}
				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_MUTE)){
					helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Mute");
					surface->SetLed(p_board, 0x40+i, mixer->GetMute(channelIndex)); 
				}

				if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_VOLUME)){
					helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "Fader");
					u_int16_t faderPosition = mixer->GetVolumeFadervalue(channelIndex);
					surface->SetFader(p_board, i, faderPosition);
				}

				if(!state->surface_disable_lcd_update) {
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
						helper->DEBUG_SURFACE(DEBUGLEVEL_VERBOSE, "LCD");
						SetLcdFromVChannel(p_board, i, channelIndex);
					}
				}
			}
		}
	}

	if (p_board == X32_BOARD_R){
		// Clear Solo
		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) { 
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
		String(mixer->GetGate(channelIndex, 'T') > -80 ? "G " : "  ") +
		String(mixer->GetDynamics(channelIndex, 'T') < 0 ? "D " : "  ") +
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
    data->texts[3].text = chan->name;
    data->texts[3].size = 0;
    data->texts[3].x = 0;
    data->texts[3].y = 48;

    surface->SetLcdX(data, 4);

	delete data;
}

// Update all meters (Gui, Surface, xremote)
void X32Ctrl::UpdateMeters(void) {

	pages[state->activePage]->UpdateMeters();
	xremote->UpdateMeter(mixer);

	// ########################################
	//
	//		Surface Meters
	//
	// ########################################

	uint8_t chanIdx = config->selectedVChannel;

	if (config->IsModelX32Core()) {
		// TODO
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
		switch (activeBank_inputFader) {
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

		// update channel-meters
	if (config->IsModelX32Full()) {

		// update meters on board L and M
		switch (activeBank_inputFader) {
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
	if (state->HasChanged(X32_MIXER_CHANGED_BANKING_INPUT)) {
		if (config->IsModelX32Full()){
			surface->SetLedByEnum(X32_BTN_CH_1_16, activeBank_inputFader == 0);
			surface->SetLedByEnum(X32_BTN_CH_17_32, activeBank_inputFader == 1);
			surface->SetLedByEnum(X32_BTN_AUX_IN_EFFECTS, activeBank_inputFader == 2);
			surface->SetLedByEnum(X32_BTN_BUS_MASTER, activeBank_inputFader == 3);
		}
		if (config->IsModelX32CompactOrProducer()) {
			surface->SetLedByEnum(X32_BTN_CH_1_8, activeBank_inputFader == 0);
			surface->SetLedByEnum(X32_BTN_CH_9_16, activeBank_inputFader == 1);
			surface->SetLedByEnum(X32_BTN_CH_17_24, activeBank_inputFader == 2);
			surface->SetLedByEnum(X32_BTN_CH_25_32, activeBank_inputFader == 3);
			surface->SetLedByEnum(X32_BTN_AUX_IN_1_6_USB_REC, activeBank_inputFader == 4);
			surface->SetLedByEnum(X32_BTN_EFFECTS_RETURNS, activeBank_inputFader == 5);
			surface->SetLedByEnum(X32_BTN_BUS_1_8_MASTER, activeBank_inputFader == 6);
			surface->SetLedByEnum(X32_BTN_BUS_9_16_MASTER, activeBank_inputFader == 7);
		}
	}
	if (state->HasChanged(X32_MIXER_CHANGED_BANKING_BUS)) {
		surface->SetLedByEnum(X32_BTN_GROUP_DCA_1_8, activeBank_busFader == 0);
		surface->SetLedByEnum(X32_BTN_BUS_1_8, activeBank_busFader == 1);
		surface->SetLedByEnum(X32_BTN_BUS_9_16, activeBank_busFader == 2);
		surface->SetLedByEnum(X32_BTN_MATRIX_MAIN_C, activeBank_busFader == 3);
	}
}

// only X32 Rack and X32 Core
void X32Ctrl::setLedChannelIndicator(void){
	if (config->IsModelX32Core() || config->IsModelX32Rack()){
		uint8_t chanIdx = config->selectedVChannel;
		surface->SetLedByEnum(X32_LED_IN, (chanIdx <= 31));
		surface->SetLedByEnum(X32_LED_AUX, (chanIdx >= 32)&&(chanIdx <= 47));
		surface->SetLedByEnum(X32_LED_BUS, (chanIdx >= 48)&&(chanIdx <= 63));
		surface->SetLedByEnum(X32_LED_DCA, (chanIdx >= 64)&&(chanIdx <= 69));
		surface->SetLedByEnum(X32_LED_MAIN, (chanIdx >= 70)&&(chanIdx <= 71));
		surface->SetLedByEnum(X32_LED_MATRIX, (chanIdx >= 72)&&(chanIdx <= 79));

		// set 7-Segment Display
		surface->SetX32RackDisplay(chanIdx);        
	}
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
void X32Ctrl::syncXRemote(bool syncAll) {
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
	int16_t newSelectedVChannel = config->selectedVChannel + direction;
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "ChangeSelect(): selected channel index: %d, direction: %d, new channel index: %d\n", config->selectedVChannel, direction, newSelectedVChannel);
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

	config->selectedVChannel = vChannelIndex;
	state->SetChangeFlags(X32_MIXER_CHANGED_SELECT);
}

void X32Ctrl::ToggleSelect(uint8_t vChannelIndex){
	SetSelect(vChannelIndex, !mixer->vchannel[vChannelIndex]->selected);
}

VChannel* X32Ctrl::GetSelectedvChannel() {
	return mixer->vchannel[config->selectedVChannel];
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
                printf("%02X ", surfacePacketBuffer[package][i]);
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
        } else {
            receivedClass = surfacePacketBuffer[package][0];
            receivedIndex = surfacePacketBuffer[package][1];
            if (surfacePacketBuffer[package][3] == 0xFE){
                // short package
                receivedValue = surfacePacketBuffer[package][2];
                //receivedChecksum = surfacePacketBuffer[package][4];
            } else if (surfacePacketBuffer[package][4] == 0xFE){
                // long package
                receivedValue = ((uint16_t)surfacePacketBuffer[package][3] << 8) | (uint16_t)surfacePacketBuffer[package][2];
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
		mixer->SetVolume(vchannelIndex, helper->Fadervalue2dBfs(event->value));
		surface->FaderMoved(event);
		
		//helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "FaderMoved(%s): vChannel%d TouchControl=%d\n", event->ToString().c_str(), vchannelIndex, touchcontrol.value);
	}
}

void X32Ctrl::ButtonPressed(SurfaceEvent* event) {
	X32_BTN button = surface->Button2Enum[((uint16_t)event->boardId << 8) + (uint16_t)(event->value & 0x7F)];
	bool buttonPressed = (event->value >> 7) == 1;

	helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "X32_BTN:%d", button);

	if (config->GetBankMode() == X32_SURFACE_MODE_BANKING_X32) {
		if (buttonPressed){
			switch (button) {
				// Note: X32_BTN_UP and X32_BTN_DOWN are handled in the page classes!
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
				case X32_BTN_VIEW_GATE:
					ShowPage(X32_PAGE_GATE);
					break;
				case X32_BTN_VIEW_COMPRESSOR:
					ShowPage(X32_PAGE_COMPRESSOR);
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
					mixer->TogglePhantom(config->selectedVChannel);
					break;
				case X32_BTN_PHASE_INVERT:
					mixer->TogglePhaseInvert(config->selectedVChannel);
					break;
				case X32_BTN_CHANNEL_SOLO: // only X32 Rack
					mixer->ToggleSolo(config->selectedVChannel);
					break;
				case X32_BTN_CHANNEL_MUTE: // only X32 Rack
					mixer->ToggleMute(config->selectedVChannel);
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
				pages[state->activePage]->OnDisplayButton(button, buttonPressed);
				break;
			default:
				break;
		}
	}
}

void X32Ctrl::EncoderTurned(SurfaceEvent* event) {
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
			case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
				ChangeSelect(amount);
				break;
			case X32_ENC_MAIN_LEVEL:  // only X32 Rack
				mixer->ChangeVolume(X32_VCHANNEL_BLOCK_MAIN, amount);
				break;
			case X32_ENC_CHANNEL_LEVEL:
				mixer->ChangeVolume(config->selectedVChannel, amount);				
				break;
			case X32_BTN_EQ_MODE:
				mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'T', amount);
				break;
			case X32_ENC_GAIN:
				mixer->ChangeGain(config->selectedVChannel, amount);
				break;
			case X32_ENC_GATE:
				mixer->ChangeGate(config->selectedVChannel, 'T', amount);
				break;
			case X32_ENC_LOWCUT:
				mixer->ChangeLowcut(config->selectedVChannel, amount);
				break;
			case X32_ENC_DYNAMICS:
				mixer->ChangeDynamics(config->selectedVChannel, 'T', amount);
				break;
			case X32_ENC_EQ_Q:
				mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'Q', amount);
				break;
			case X32_ENC_EQ_FREQ:
				mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'F', amount);
				break;
			case X32_ENC_EQ_GAIN:
				mixer->ChangePeq(config->selectedVChannel, state->activeEQ, 'G', amount);
				break;
			case X32_ENC_PAN:
				mixer->ChangeBalance(config->selectedVChannel, amount);
				break;
			case X32_ENC_BUS_SEND_1:
				mixer->ChangeBusSend(config->selectedVChannel, 0, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_2:
				mixer->ChangeBusSend(config->selectedVChannel, 1, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_3:
				mixer->ChangeBusSend(config->selectedVChannel, 2, amount, activeBusSend);
				break;
			case X32_ENC_BUS_SEND_4:
				mixer->ChangeBusSend(config->selectedVChannel, 3, amount, activeBusSend);
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
				pages[state->activePage]->OnDisplayEncoderTurned(encoder, amount);
				break;
			default:  
				// just here to avoid compiler warnings                  
				break;
		}
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
			state->activeEQ = 0;
			surface->SetLedByEnum(X32_BTN_EQ_LOW, 1);
			break;
		case X32_BTN_EQ_LOW_MID:
			state->activeEQ = 1;
			surface->SetLedByEnum(X32_BTN_EQ_LOW_MID, 1);
			break;
		case X32_BTN_EQ_HIGH_MID:
			state->activeEQ = 2;
			surface->SetLedByEnum(X32_BTN_EQ_HIGH_MID, 1);
			break;
		case X32_BTN_EQ_HIGH:
			state->activeEQ = 3;
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
	uint16_t changeflag = X32_MIXER_CHANGED_NONE;


	if (config->IsModelX32Full()){
		switch (p_button){
			case X32_BTN_CH_1_16:
				activeBank_inputFader = 0;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_CH_17_32:
				activeBank_inputFader = 1;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_AUX_IN_EFFECTS:
				activeBank_inputFader = 2;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_BUS_MASTER:
				activeBank_inputFader = 3;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_GROUP_DCA_1_8:
				activeBank_busFader = 0;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_BUS_1_8:
				activeBank_busFader = 1;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_BUS_9_16:
				activeBank_busFader = 2;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_MATRIX_MAIN_C:
				activeBank_busFader = 3;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			default:
				break;
		}
	}
	if (config->IsModelX32CompactOrProducer()){
		switch (p_button){
			case X32_BTN_CH_1_8:
				activeBank_inputFader = 0;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_CH_9_16:
				activeBank_inputFader = 1;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_CH_17_24:
				activeBank_inputFader = 2;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_CH_25_32:
				activeBank_inputFader = 3;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_AUX_IN_1_6_USB_REC:
				activeBank_inputFader = 4;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_EFFECTS_RETURNS:
				activeBank_inputFader = 5;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_BUS_1_8_MASTER:
				activeBank_inputFader = 6;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_BUS_9_16_MASTER:
				activeBank_inputFader = 7;
				changeflag = X32_MIXER_CHANGED_BANKING_INPUT;
				break;
			case X32_BTN_GROUP_DCA_1_8:
				activeBank_busFader = 0;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_BUS_1_8:
				activeBank_busFader = 1;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_BUS_9_16:
				activeBank_busFader = 2;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			case X32_BTN_MATRIX_MAIN_C:
				activeBank_busFader = 3;
				changeflag = X32_MIXER_CHANGED_BANKING_BUS;
				break;
			default:
				break;
		}
	}

	state->SetChangeFlags(changeflag);
}

//####################################################################
// 
// ########  ######## ########  ##     ##  ######   
// ##     ## ##       ##     ## ##     ## ##    ##  
// ##     ## ##       ##     ## ##     ## ##        
// ##     ## ######   ########  ##     ## ##   #### 
// ##     ## ##       ##     ## ##     ## ##    ##  
// ##     ## ##       ##     ## ##     ## ##    ##  
// ########  ######## ########   #######   ######   
//
//####################################################################


void X32Ctrl::DebugPrintBank(uint8_t bank)
{
	printf("################# BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		printf("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2VChannel[i]);
	}
	printf("END ############# BANK%d ############### END\n", bank);
}

void X32Ctrl::DebugPrintBusBank(uint8_t bank)
{
	printf("################# BUS BANK%d ###################\n", bank);
	for (uint8_t i=0;i<35;i++){
		printf("surfaceChannel%d -> vChannel%d\n", i, modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2VChannel[i]);
	}
	printf("END ############# BUS BANK%d ############### END\n", bank);
}
