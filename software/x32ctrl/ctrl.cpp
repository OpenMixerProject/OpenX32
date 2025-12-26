#include "ctrl.h"

char displayEncoderText[6][30];
static const char* displayEncoderButtonMap[] = {
    displayEncoderText[0],
    displayEncoderText[1],
    displayEncoderText[2],
    displayEncoderText[3],
    displayEncoderText[4],
    displayEncoderText[5],
    NULL};

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

	//##################################################################################
	//#
	//# 	Configure the fader bank layout (only X32 Full/Compact/Producer)
	//#
	//##################################################################################

	if(config->IsModelX32FullOrCompactOrProducer()) {

		ResetFaderBankLayout();
		LoadFaderBankLayout(FADER_BANK_LAYOUT_X32);

		activeBank_inputFader = 0;
		activeBank_busFader = 0;
		activeEQ = 0;
		activeBusSend = 0;
	}

	//##################################################################################
	//#
	//# 	Connect pages together
	//#
	//##################################################################################

	// Home
	pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
	pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

	pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
	pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_GATE;

	pages[X32_PAGE_GATE].prevPage = X32_PAGE_CONFIG;
	pages[X32_PAGE_GATE].nextPage = X32_PAGE_COMPRESSOR;

	pages[X32_PAGE_COMPRESSOR].prevPage = X32_PAGE_GATE;
	pages[X32_PAGE_COMPRESSOR].nextPage = X32_PAGE_EQ;

	pages[X32_PAGE_EQ].prevPage = X32_PAGE_COMPRESSOR;
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

	state->SetChangeFlags(X32_MIXER_CHANGED_ALL); // trigger first sync to gui/surface
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

	//DspChannels
	for (uint8_t i = 0; i < MAX_DSP_INPUTCHANNELS; i++)	{
		string section = string("dspchannel") + to_string(i);
		mixer->dsp->Channel[i].inputSource = mixer_ini[section]["inputSource"].as<int>();
		mixer->dsp->Channel[i].inputTapPoint = mixer_ini[section]["inputTapPoint"].as<int>();
	}
	//DspOutChannels
	for (uint8_t i = 0; i < MAX_DSP_OUTPUTCHANNELS; i++) {
		string section = string("dspoutchannel") + to_string(i);
		mixer->dsp->Dsp2FpgaChannel[i].outputSource = mixer_ini[section]["outputSource"].as<int>();
		mixer->dsp->Dsp2FpgaChannel[i].outputTapPoint = mixer_ini[section]["outputTapPoint"].as<int>();
	}
	//Dsp2FxChannels
	for (uint8_t i = 0; i < MAX_DSP_FXCHANNELS; i++) {
		string section = string("dspfxchannel") + to_string(i);
		mixer->dsp->Dsp2FxChannel[i].outputSource = mixer_ini[section]["outputSource"].as<int>();
		mixer->dsp->Dsp2FxChannel[i].outputTapPoint = mixer_ini[section]["outputTapPoint"].as<int>();
	}
	//Dsp2AuxChannels
	for (uint8_t i = 0; i < MAX_DSP_AUXCHANNELS; i++) {
		string section = string("dspauxchannel") + to_string(i);
		mixer->dsp->Dsp2AuxChannel[i].outputSource = mixer_ini[section]["outputSource"].as<int>();
		mixer->dsp->Dsp2AuxChannel[i].outputTapPoint = mixer_ini[section]["outputTapPoint"].as<int>();
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
		mixer->fpga->RoutingSendConfigToFpga(-1);
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
	//DspChannels
	for (uint8_t i = 0; i < MAX_DSP_INPUTCHANNELS; i++)	{
		string section = string("dspchannel") + to_string(i);
		mixer_ini[section]["inputSource"] = (int)mixer->dsp->Channel[i].inputSource;
		mixer_ini[section]["inputTapPoint"] = (int)mixer->dsp->Channel[i].inputTapPoint;
	}
	//DspOutChannels
	for (uint8_t i = 0; i < MAX_DSP_OUTPUTCHANNELS; i++) {
		string section = string("dspoutchannel") + to_string(i);
		mixer_ini[section]["outputSource"] = (int)mixer->dsp->Dsp2FpgaChannel[i].outputSource;
		mixer_ini[section]["outputTapPoint"] = (int)mixer->dsp->Dsp2FpgaChannel[i].outputTapPoint;
	}
	//Dsp2FxChannels
	for (uint8_t i = 0; i < MAX_DSP_FXCHANNELS; i++) {
		string section = string("dspfxchannel") + to_string(i);
		mixer_ini[section]["outputSource"] = (int)mixer->dsp->Dsp2FxChannel[i].outputSource;
		mixer_ini[section]["outputTapPoint"] = (int)mixer->dsp->Dsp2FxChannel[i].outputTapPoint;
	}
	//Dsp2AuxChannels
	for (uint8_t i = 0; i < MAX_DSP_AUXCHANNELS; i++) {
		string section = string("dspauxchannel") + to_string(i);
		mixer_ini[section]["outputSource"] = (int)mixer->dsp->Dsp2AuxChannel[i].outputSource;
		mixer_ini[section]["outputTapPoint"] = (int)mixer->dsp->Dsp2AuxChannel[i].outputTapPoint;
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
	if (state->timers){
		helper->stoptimer(10, "Tick10ms - delay between calls");
	}

	surface->Tick10ms();	
	mixer->Tick10ms();

	ProcessUartData();

	// communication with XRemote-clients via UDP (X32-Edit, MixingStation, etc.)
	UdpHandleCommunication();

	syncAll();

	guiFastRefresh();

	
	if (state->timers){
		helper->starttimer(10);
	}
}

void X32Ctrl::Tick100ms(void){
	if (state->timers){
		helper->stoptimer(0, "Tick100ms - delay between calls");
	}
	
	surfaceUpdateMeter();
	surface->Tick100ms();

	// update meters on XRemote-clients
	xremote->UpdateMeter(mixer);
	
	mixer->Tick100ms();

	if (!config->IsModelX32Core() && state->activePage == X32_PAGE_UTILITY) {
		// read the current DSP load
	 	lv_label_set_text_fmt(objects.debugtext, "DSP1: %.2f %% [v%.2f] | DSP2: %.2f %% [v%.2f]", (double)state->dspLoad[0], (double)state->dspVersion[0], (double)state->dspLoad[1], (double)state->dspVersion[1]); // show the received value (could be a bit older than the request)
	}

	if (state->timers){
		helper->starttimer(0);
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
//  ######   ##     ## #### 
// ##    ##  ##     ##  ##  
// ##        ##     ##  ##  
// ##   #### ##     ##  ##  
// ##    ##  ##     ##  ##  
// ##    ##  ##     ##  ##  
//  ######    #######  #### 
//
//#####################################################################################################################



void X32Ctrl::guiFastRefresh(void) {
    // update gate-display if visible
    if (state->activePage == X32_PAGE_GATE) {
        int32_t gateValueAudioDbfs = -120;

        uint8_t selectedChannelIndex = GetSelectedvChannelIndex();
        if (selectedChannelIndex < 40) {
            gateValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meterDecay) * 100.0f;
        }

        // add new value to chart
        lv_chart_set_next_value(objects.current_channel_gate, chartSeriesGateAudio, gateValueAudioDbfs);
        //lv_chart_refresh(objects.current_channel_gate);
    }else if (state->activePage == X32_PAGE_COMPRESSOR) {
        int32_t compValueAudioDbfs = -120;

        uint8_t selectedChannelIndex = GetSelectedvChannelIndex();
        if (selectedChannelIndex < 40) {
            compValueAudioDbfs = helper->sample2Dbfs(mixer->dsp->rChannel[selectedChannelIndex].meterDecay) * 100.0f;
        }

        // add new value to chart
        lv_chart_set_next_value(objects.current_channel_comp, chartSeriesCompressorAudio, compValueAudioDbfs);
        //lv_chart_refresh(objects.current_channel_comp);
    }
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

void X32Ctrl::DrawGate(uint8_t selectedChannelIndex) {
	if (selectedChannelIndex >= 40) {
		return;
	}

	// calculate the gate curve
	float gateValue[200];
	float inputLevel;
	float outputLevel;

	memset(&gateValue[0], 0, sizeof(gateValue));

	sGate* gate = &mixer->dsp->Channel[GetSelectedvChannelIndex()].gate;

	for (uint16_t pixel = 0; pixel < 200; pixel++) {
		inputLevel = (60.0f * ((float)pixel/199.0f)) - 60.0f; // from -60 dB to 0 dB

		if (inputLevel >= gate->threshold) {
			// above threshold
			outputLevel = inputLevel;
		}else{
			// below threshold -> apply reduction
			outputLevel = inputLevel - gate->range;
		}

		// scale outputLevel to -6000 .. 0
		gateValue[pixel] = outputLevel * 100.0f;
	}

	int32_t* chartSeriesGatePoints = lv_chart_get_series_y_array(objects.current_channel_gate, chartSeriesGate);
	for (uint16_t i = 0; i < 200; i++) {
	   chartSeriesGatePoints[i] = gateValue[i];
	}
	lv_chart_refresh(objects.current_channel_gate);
}

void X32Ctrl::DrawDynamics(uint8_t selectedChannelIndex) {
	if (selectedChannelIndex >= 40) {
		return;
	}

	// calculate the compressor curve
	float compValue[200];
	float inputLevel;
	float outputLevel;

	memset(&compValue[0], 0, sizeof(compValue));

	sCompressor* comp = &mixer->dsp->Channel[GetSelectedvChannelIndex()].compressor;

	for (uint16_t pixel = 0; pixel < 200; pixel++) {
		inputLevel = (60.0f * ((float)pixel/199.0f)) - 60.0f; // from -60 dB to 0 dB

		if (inputLevel < comp->threshold) {
			// below threshold
			outputLevel = inputLevel;
		}else{
			// above threshold -> calculate overshoot (inputLevel - comp->threshold) and take ratio into account
			outputLevel = comp->threshold + ((inputLevel - comp->threshold) / comp->ratio);
		}

		// scale outputLevel to -6000 .. 0
		compValue[pixel] = outputLevel * 100.0f;
	}

	int32_t* chartSeriesCompPoints = lv_chart_get_series_y_array(objects.current_channel_comp, chartSeriesCompressor);
	for (uint16_t i = 0; i < 200; i++) {
	   chartSeriesCompPoints[i] = compValue[i];
	}
	lv_chart_refresh(objects.current_channel_comp);
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
	   chartSeriesEqPoints[i] = eqValue[i] * 1000.0f;
	}
	lv_chart_refresh(objects.current_channel_eq);
}

//#####################################################################################################################
//
// ########     ###     ######   ########       ######   #######  ##    ## ######## ########   #######  ##       
// ##     ##   ## ##   ##    ##  ##            ##    ## ##     ## ###   ##    ##    ##     ## ##     ## ##       
// ##     ##  ##   ##  ##        ##            ##       ##     ## ####  ##    ##    ##     ## ##     ## ##       
// ########  ##     ## ##   #### ######        ##       ##     ## ## ## ##    ##    ########  ##     ## ##       
// ##        ######### ##    ##  ##            ##       ##     ## ##  ####    ##    ##   ##   ##     ## ##       
// ##        ##     ## ##    ##  ##            ##    ## ##     ## ##   ###    ##    ##    ##  ##     ## ##       
// ##        ##     ##  ######   ########       ######   #######  ##    ##    ##    ##     ##  #######  ######## 
//
//#####################################################################################################################

void X32Ctrl::ShowNextPage(void){
	if (pages[state->activePage].nextPage != X32_PAGE_NONE){
		ShowPage(pages[state->activePage].nextPage);
	}
}

void X32Ctrl::ShowPrevPage(void){
	if (pages[state->activePage].prevPage != X32_PAGE_NONE){
		ShowPage(pages[state->activePage].prevPage);
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

	state->lastPage = state->activePage;
	state->activePage = newPage;

	// Buttons on all models with display (Full, Compact, Producer, Rack)
	surface->SetLedByEnum(X32_BTN_HOME, state->activePage == X32_PAGE_HOME);
	surface->SetLedByEnum(X32_BTN_METERS, state->activePage == X32_PAGE_METERS);
	surface->SetLedByEnum(X32_BTN_ROUTING, state->activePage == X32_PAGE_ROUTING);
	surface->SetLedByEnum(X32_BTN_SETUP, state->activePage == X32_PAGE_SETUP);
	surface->SetLedByEnum(X32_BTN_LIBRARY, state->activePage == X32_PAGE_LIBRARY);
	surface->SetLedByEnum(X32_BTN_EFFECTS, state->activePage == X32_PAGE_EFFECTS);
	surface->SetLedByEnum(X32_BTN_MUTE_GRP, state->activePage == X32_PAGE_MUTE_GRP);
	surface->SetLedByEnum(X32_BTN_UTILITY, state->activePage == X32_PAGE_UTILITY);

	// Buttons on all models with display (Full, Compact, Producer) except Rack!
	if (config->IsModelX32FullOrCompactOrProducer())
	{
		surface->SetLedByEnum(X32_BTN_VIEW_CONFIG, state->activePage == X32_PAGE_CONFIG);
		surface->SetLedByEnum(X32_BTN_VIEW_GATE, state->activePage == X32_PAGE_GATE);
		surface->SetLedByEnum(X32_BTN_VIEW_COMPRESSOR, state->activePage == X32_PAGE_COMPRESSOR);
		surface->SetLedByEnum(X32_BTN_VIEW_EQ, state->activePage == X32_PAGE_EQ);
	}

	switch (state->activePage)
	{
		case X32_PAGE_HOME:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 0, LV_ANIM_OFF);
			break;
		case X32_PAGE_CONFIG:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 1, LV_ANIM_OFF);
			break;
		case X32_PAGE_GATE:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 2, LV_ANIM_OFF);
			break;
		case X32_PAGE_COMPRESSOR:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 3, LV_ANIM_OFF);
			break;
		case X32_PAGE_EQ:
			lv_tabview_set_active(objects.maintab, 0, LV_ANIM_OFF);
			lv_tabview_set_active(objects.hometab, 4, LV_ANIM_OFF);
			break;
		case X32_PAGE_METERS:
			lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
			break;
		case X32_PAGE_ROUTING:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 0, LV_ANIM_OFF);
			break;
		case X32_PAGE_ROUTING_FPGA:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 1, LV_ANIM_OFF);
			break;
		case X32_PAGE_ROUTING_DSP1:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 2, LV_ANIM_OFF);
			break;
		case X32_PAGE_ROUTING_DSP2:
			lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
			lv_tabview_set_active(objects.routingtab, 3, LV_ANIM_OFF);
			break;
		case X32_PAGE_SETUP:
			lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
			break;
		case X32_PAGE_LIBRARY:
			lv_tabview_set_active(objects.maintab, 4, LV_ANIM_OFF);
			break;
		case X32_PAGE_EFFECTS:
			lv_tabview_set_active(objects.maintab, 5, LV_ANIM_OFF);
			break;
		case X32_PAGE_MUTE_GRP:
			lv_tabview_set_active(objects.maintab, 6, LV_ANIM_OFF);
			break;
		case X32_PAGE_UTILITY:
			lv_tabview_set_active(objects.maintab, 7, LV_ANIM_OFF);
			break;
		default:
			state->activePage = X32_PAGE_NONE;
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

		helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "SyncAll ");

		guiSync();
		surfaceSync();
		xremoteSync(false);

		if (state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)) {
			mixer->Sync();
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

	// if these have not changed, do nothing
	if (!(
		state->HasChanged(X32_MIXER_CHANGED_PAGE)  		||
		state->HasChanged(X32_MIXER_CHANGED_SELECT)  	||
		state->HasChanged(X32_MIXER_CHANGED_VCHANNEL)	||
		state->HasChanged(X32_MIXER_CHANGED_ROUTING) 	||
		state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)	||
		state->HasChanged(X32_MIXER_CHANGED_GUI)
		)) {
		return;
	}

	VChannel* chan = GetSelectedvChannel();
	uint8_t chanIndex = GetSelectedvChannelIndex();
	bool pageInit = false; // Init page after page change
	
	if (state->HasChanged(X32_MIXER_CHANGED_PAGE)){
		helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page changed to: %d\n", state->activePage);
		pageInit = true;
	}

	//####################################
	//#         General
	//####################################

	if (state->HasChanged(X32_MIXER_CHANGED_SELECT)) {
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

	
	if (state->activePage == X32_PAGE_CONFIG){
	//####################################
	//#         Page Config
	//####################################
		char dspSourceName[5] = "";
		mixer->dsp->GetSourceName(&dspSourceName[0], GetSelectedvChannelIndex(), mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[GetSelectedvChannelIndex()].inputSource - 1]);
		lv_label_set_text_fmt(objects.current_channel_source, "%02d: %s", (chanIndex + 1), dspSourceName);

		lv_label_set_text_fmt(objects.current_channel_gain, "%f", (double)mixer->GetGain(chanIndex));
		lv_label_set_text_fmt(objects.current_channel_phantom, "%d", mixer->GetPhantomPower(GetSelectedvChannelIndex()));
		lv_label_set_text_fmt(objects.current_channel_invert, "%d", mixer->GetPhaseInvert(chanIndex));
		lv_label_set_text_fmt(objects.current_channel_pan_bal, "%f", (double)mixer->GetBalance(chanIndex));
		lv_label_set_text_fmt(objects.current_channel_volume, "%f", (double)mixer->GetVolumeDbfs(chanIndex));


		//char outputDestinationName[10] = "";
		//routingGetOutputName(&outputDestinationName[0], mixerGetSelectedChannel());
		//lv_label_set_text_fmt(objects.current_channel_destination, outputDestinationName);

		guiSetEncoderText("Source\n[Invert]", "Gain\n[48V]", "PAN/BAL\n[Center]", "Volume\n[Mute]", "-", "-");
	}else if (state->activePage == X32_PAGE_ROUTING_FPGA) {
	//####################################
	//#         Page Routing (FPGA)
	//####################################
		char outputDestinationName[15] = "";
		char inputSourceName[15] = "";
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
			if (state->gui_selected_item >= NUM_OUTPUT_CHANNEL) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
			}

			lv_table_set_row_count(objects.table_routing_fpga, NUM_OUTPUT_CHANNEL); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_fpga, 3);
			lv_table_set_column_width(objects.table_routing_fpga, 0, 200);
			lv_table_set_column_width(objects.table_routing_fpga, 1, 50);
			lv_table_set_column_width(objects.table_routing_fpga, 2, 200);
			for (uint8_t i=0; i < NUM_OUTPUT_CHANNEL; i++){
				mixer->fpga->RoutingGetOutputNameByIndex(&outputDestinationName[0], i+1);
				routingIndex = mixer->fpga->RoutingGetOutputSourceByIndex(i+1);
				mixer->fpga->RoutingGetSourceNameByIndex(&inputSourceName[0], routingIndex);
				lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 0, "%s", outputDestinationName);
				lv_table_set_cell_value_fmt(objects.table_routing_fpga, i, 2, "%s", inputSourceName);
			}
			lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
			state->page_routing_fpga_table_drawn = true;
		}

		// Update Table
		if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
			if (state->gui_selected_item >= NUM_OUTPUT_CHANNEL) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = NUM_OUTPUT_CHANNEL - 1;
			}

			if (state->gui_selected_item != state->gui_old_selected_item ) {
				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_fpga, state->gui_old_selected_item, 1, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_fpga, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
				
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

		guiSetEncoderText("\xEF\x81\xB7 Target \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "-", "-");
	}else if (state->activePage == X32_PAGE_ROUTING_DSP1) {
	//####################################
	//#         Page Input-Routing (DSP)
	//####################################
		char inputChannelName[25] = "";
		char inputSourceName[25] = "";
		char tapPointName[15] = "";

		// Table

		// DSP-input-channels:
		// 0-31		Full-Featured DSP-Channels
		// 32-39	Aux-Channel

		// DSP-output-channels:
		// 0-31		Main-Output to FPGA
		// 32-39	Aux-Output to FPGA
		// 40-56	FX-Sends 1-16 to DSP2
		// 57-64	FX-Aux to DSP2

		// DSP-Taps
		// 0		DSP_BUF_IDX_OFF
		// 1-33		DSP-Input 1-32 from FPGA
		// 33-40	AUX-Input 1-8 from FPGA
		// 41-56	FX-Return 1-8 from DSP2
		// 57-72	Mixbus 1-16 (internal)
		// 73-75	Main Left, Right, Sub (internal)
		// 76-81	Matrix 1-6 (internal)
		// 82-89	FX-Aux-Channel 1-8 from DSP2
		// 90-92	Monitor Left, Right, Talkback (internal)

		// Initial Table Draw
		if (!state->page_routing_dsp1_table_drawn){
			if (state->gui_selected_item >= MAX_DSP_INPUTCHANNELS) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = MAX_DSP_INPUTCHANNELS - 1;
			}

			lv_table_set_row_count(objects.table_routing_dsp_input, MAX_DSP_INPUTCHANNELS); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_input, 5); // Input | # | Source | # | Tap
			lv_table_set_column_width(objects.table_routing_dsp_input, 0, 200);
			lv_table_set_column_width(objects.table_routing_dsp_input, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_input, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_input, 4, 100);
			for (uint8_t i=0; i < MAX_DSP_INPUTCHANNELS; i++){
				mixer->dsp->RoutingGetInputNameByIndex(&inputChannelName[0], i+1);
				mixer->dsp->RoutingGetTapNameByIndex(&inputSourceName[0], mixer->dsp->Channel[i].inputSource, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[i].inputSource - 1]);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Channel[i].inputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 0, "%s", inputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 2, "%s", inputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, i, 4, "%s", tapPointName);
			}

			lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
			lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
			state->page_routing_fpga_table_drawn = true;
		}

		// Update Table
		if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
			if (state->gui_selected_item >= MAX_DSP_INPUTCHANNELS) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = MAX_DSP_INPUTCHANNELS - 1;
			}

			if (state->gui_selected_item != state->gui_old_selected_item ) {
				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_old_selected_item, 1, " ");
				lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_old_selected_item, 3, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
				lv_table_set_cell_value(objects.table_routing_dsp_input, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
				
				// set select to scroll table
				lv_table_set_selected_cell(objects.table_routing_dsp_input, state->gui_selected_item, 2);
				
				state->gui_old_selected_item = state->gui_selected_item;
			}
		} 
		
		if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
			mixer->dsp->RoutingGetTapNameByIndex(&inputSourceName[0], mixer->dsp->Channel[state->gui_selected_item].inputSource, mixer->fpga->fpgaRouting.dsp[mixer->dsp->Channel[state->gui_selected_item].inputSource - 1]);
			mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Channel[state->gui_selected_item].inputTapPoint);

			lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, state->gui_selected_item, 2, "%s", inputSourceName);
			lv_table_set_cell_value_fmt(objects.table_routing_dsp_input, state->gui_selected_item, 4, "%s", tapPointName);
		}

		guiSetEncoderText("\xEF\x81\xB7 Input \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "\xEF\x80\xA1 Tap", "-");
	}else if (state->activePage == X32_PAGE_ROUTING_DSP2) {
	//####################################
	//#         Page Output-Routing (DSP)
	//####################################
		char outputChannelName[25] = "";
		char outputSourceName[25] = "";
		char tapPointName[15] = "";

		// Table

		// DSP-input-channels:
		// 0-31		Full-Featured DSP-Channels
		// 32-39	Aux-Channel

		// DSP-output-channels:
		// 0-31		Main-Output to FPGA
		// 32-39	Aux-Output to FPGA
		// 40-56	FX-Sends 1-16 to DSP2
		// 57-64	FX-Aux to DSP2

		// DSP-Taps
		// 0		DSP_BUF_IDX_OFF
		// 1-33		DSP-Input 1-32 from FPGA
		// 33-40	AUX-Input 1-8 from FPGA
		// 41-56	FX-Return 1-8 from DSP2
		// 57-72	Mixbus 1-16 (internal)
		// 73-75	Main Left, Right, Sub (internal)
		// 76-81	Matrix 1-6 (internal)
		// 82-89	FX-Aux-Channel 1-8 from DSP2
		// 90-92	Monitor Left, Right, Talkback (internal)

		// Initial Table Draw
		if (!state->page_routing_dsp2_table_drawn){
			if (state->gui_selected_item >= (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS)) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS) - 1;
			}

			lv_table_set_row_count(objects.table_routing_dsp_output, (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS)); /*Not required but avoids a lot of memory reallocation lv_table_set_set_value*/
			lv_table_set_column_count(objects.table_routing_dsp_output, 5); // Input | # | Source | # | Tap | #
			lv_table_set_column_width(objects.table_routing_dsp_output, 0, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 1, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 2, 200);
			lv_table_set_column_width(objects.table_routing_dsp_output, 3, 50);
			lv_table_set_column_width(objects.table_routing_dsp_output, 4, 100);
			for (uint8_t i=0; i < MAX_DSP_OUTPUTCHANNELS; i++){
				mixer->dsp->RoutingGetOutputNameByIndex(&outputChannelName[0], i+1);
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2FpgaChannel[i].outputSource, mixer->dsp->Channel[i].inputSource);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2FpgaChannel[i].outputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 0, "%s", outputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i, 4, "%s", tapPointName);
			}

			for (uint8_t i=0; i < MAX_DSP_FXCHANNELS; i++){
				mixer->dsp->RoutingGetOutputNameByIndex(&outputChannelName[0], i+MAX_DSP_OUTPUTCHANNELS+1);
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2FxChannel[i].outputSource, 0);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2FxChannel[i].outputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS, 0, "%s", outputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS, 4, "%s", tapPointName);
			}

			for (uint8_t i=0; i < MAX_DSP_AUXCHANNELS; i++){
				mixer->dsp->RoutingGetOutputNameByIndex(&outputChannelName[0], i+MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+1);
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2AuxChannel[i].outputSource, 0);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2AuxChannel[i].outputTapPoint);

				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS, 0, "%s", outputChannelName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS, 2, "%s", outputSourceName);
				lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, i+MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS, 4, "%s", tapPointName);
			}

			lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
			lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
			state->page_routing_fpga_table_drawn = true;
		}

		// Update Table
		if(state->HasChanged(X32_MIXER_CHANGED_GUI_SELECT)) {
			if (state->gui_selected_item >= (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS)) {
				state->gui_selected_item = 0;
			}else if (state->gui_selected_item < 0) {
				state->gui_selected_item = (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS) - 1;
			}

			if (state->gui_selected_item != state->gui_old_selected_item ) {
				// remove old indicator
				lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_old_selected_item, 1, " ");
				lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_old_selected_item, 3, " ");
				
				// display new indicator
				lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 1, LV_SYMBOL_LEFT);
				lv_table_set_cell_value(objects.table_routing_dsp_output, state->gui_selected_item, 3, LV_SYMBOL_LEFT);
				
				// set select to scroll table
				lv_table_set_selected_cell(objects.table_routing_dsp_output, state->gui_selected_item, 2);
				
				state->gui_old_selected_item = state->gui_selected_item;
			}
		} 
		
		if(state->HasChanged(X32_MIXER_CHANGED_ROUTING)){
			if (state->gui_selected_item < MAX_DSP_OUTPUTCHANNELS) {
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2FxChannel[state->gui_selected_item].outputSource, mixer->dsp->Channel[state->gui_selected_item].inputSource);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2FxChannel[state->gui_selected_item].outputTapPoint);
			}else if ((state->gui_selected_item >= MAX_DSP_OUTPUTCHANNELS) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS))) {
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2FxChannel[state->gui_selected_item-MAX_DSP_OUTPUTCHANNELS].outputSource, 0);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2FxChannel[state->gui_selected_item-MAX_DSP_OUTPUTCHANNELS].outputTapPoint);
			}else if ((state->gui_selected_item >= (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS)) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS+MAX_DSP_FXCHANNELS+MAX_DSP_AUXCHANNELS))) {
				mixer->dsp->RoutingGetTapNameByIndex(&outputSourceName[0], mixer->dsp->Dsp2AuxChannel[state->gui_selected_item-MAX_DSP_OUTPUTCHANNELS-MAX_DSP_FXCHANNELS].outputSource, 0);
				mixer->dsp->RoutingGetTapPositionName(&tapPointName[0], mixer->dsp->Dsp2AuxChannel[state->gui_selected_item-MAX_DSP_OUTPUTCHANNELS-MAX_DSP_FXCHANNELS].outputTapPoint);
			}

			lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, state->gui_selected_item, 2, "%s", outputSourceName);
			lv_table_set_cell_value_fmt(objects.table_routing_dsp_output, state->gui_selected_item, 4, "%s", tapPointName);
		}

		guiSetEncoderText("\xEF\x81\xB7 Output \xEF\x81\xB8", "\xEF\x81\xB7 Group \xEF\x81\xB8", "\xEF\x80\xA1 Source", "\xEF\x80\xA1 Group-Source", "\xEF\x80\xA1 Tap", "-");
	}else if (state->activePage == X32_PAGE_GATE) {
	//####################################
	//#         Page GATE
	//####################################

		if (pageInit || chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)) {
			DrawGate(GetSelectedvChannelIndex());
			helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "DrawGate()");
		}

		if (chanIndex < 40) {
			if (pageInit || chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
				// Gate
				helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "guiSetEncoderText()");
				guiSetEncoderText("Thresh: " + String(mixer->dsp->Channel[chanIndex].gate.threshold, 1) + " dB",
					"Range: " + String(mixer->dsp->Channel[chanIndex].gate.range, 1) + " dB",
					"Attack: " + String(mixer->dsp->Channel[chanIndex].gate.attackTime_ms, 0) + " ms",
					"Hold: " + String(mixer->dsp->Channel[chanIndex].gate.holdTime_ms, 0) + " ms",
					"Release: " + String(mixer->dsp->Channel[chanIndex].gate.releaseTime_ms, 0) + " ms",
					"-"
				);
		}
		}else{
			if (pageInit) {
				// unsupported at the moment
				guiSetEncoderText("-", "-", "-", "-", "-", "-");
			}
		}

	}else if (state->activePage == X32_PAGE_COMPRESSOR) {
	//####################################
	//#         Page COMPRESSOR
	//####################################

		DrawDynamics(GetSelectedvChannelIndex());

		if (chanIndex < 40) {
			// support Compressor
			guiSetEncoderText("Thresh: " + String(mixer->dsp->Channel[chanIndex].compressor.threshold, 1) + " dB",
				"Ratio: " + String(mixer->dsp->Channel[chanIndex].compressor.ratio, 1) + ":1",
				"Makeup: " + String(mixer->dsp->Channel[chanIndex].compressor.makeup, 1) + " dB",
				"Attack: " + String(mixer->dsp->Channel[chanIndex].compressor.attackTime_ms, 0) + " ms",
				"Hold: " + String(mixer->dsp->Channel[chanIndex].compressor.holdTime_ms, 0) + " ms",
				"Release: " + String(mixer->dsp->Channel[chanIndex].compressor.releaseTime_ms, 0) + " ms"
			);
		}else{
			// unsupported at the moment
			guiSetEncoderText("-", "-", "-", "-", "-", "-");
		}

	}else if (state->activePage == X32_PAGE_EQ) {
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
	}else if (state->activePage == X32_PAGE_METERS) {
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
	}else if (state->activePage == X32_PAGE_SETUP) {
	//####################################
	//#         Page Setup
	//####################################

		// pChannelSelected.solo ?
		//     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
		//     lv_imagebutton_set_state(objects.setup_solo, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);

		// pChannelSelected.mute ?
		//     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_PRESSED):
		//     lv_imagebutton_set_state(objects.setup_mute, LV_IMAGEBUTTON_STATE_CHECKED_RELEASED);
	}else if (state->activePage == X32_PAGE_UTILITY) {
	//####################################
	//#         Page Utility
	//####################################
		guiSetEncoderText("Reload DSPs", "-", "-", "-", String("D2: ") + String(state->debugvalue2).c_str(), String("D1: ") + String(state->debugvalue).c_str());
	}else{
	//####################################
	//#         All other pages
	//####################################
		if (pageInit) {
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
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_GAIN) >> 8, surface->Enum2Encoder(X32_ENC_GAIN) & 0xFF, 0, (mixer->GetGain(chanIndex) + 12.0f)/0.72f, 1);
			}
			// Balance/Panorama
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_BALANCE)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_PAN) >> 8, surface->Enum2Encoder(X32_ENC_PAN) & 0xFF, 2, (mixer->GetBalance(chanIndex) + 100.0f)/2.0f, 1);
			}
			// Bus sends
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_SENDS)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_1) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_1) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 0)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_2) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_2) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 1)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_3) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_3) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 2)/20.0f) * 100.0f, 1);
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_BUS_SEND_4) >> 8, surface->Enum2Encoder(X32_ENC_BUS_SEND_4) & 0xFF, 0, pow(10.0f, mixer->GetBusSend(chanIndex, activeBusSend * 4 + 3)/20.0f) * 100.0f, 1);
			}
			// Gate
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_GATE)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_GATE) >> 8, surface->Enum2Encoder(X32_ENC_GATE) & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].gate.threshold + 80.0f)/0.8f), 1);
			}
			// Dynamics
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_DYNAMIC)){
				surface->SetEncoderRing(surface->Enum2Encoder(X32_ENC_DYNAMICS) >> 8, surface->Enum2Encoder(X32_ENC_DYNAMICS) & 0xFF, 4, 100.0f - ((mixer->dsp->Channel[chanIndex].compressor.threshold + 60.0f)/0.6f), 1);
			}
			// EQ
			if (fullSync || chan->HasChanged(X32_VCHANNEL_CHANGED_EQ)){
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
	// if (config->IsModelX32Full()) {
	// 	// TODO
	// 	surface->SetLcd(0, 0, 7, 0, 0, 0xA0, 0x20, 5, 5, "OpenX32", 0, 0, 0, "");
	// 	surface->SetLcd(0, 1, 7, 0, 0, 0xA0, 0x20, 5, 5, "is a", 0, 0, 0, "");
	// 	surface->SetLcd(0, 2, 7, 0, 0, 0xA0, 0x20, 5, 5, "cool", 0, 0, 0, "");
	// 	surface->SetLcd(0, 3, 7, 0, 0, 0xA0, 0x20, 5, 5, "Thing!", 0, 0, 0, "");
	// }
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
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[i].meterInfo);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->rChannel[i + 8].meterInfo);
				}
				break;
			case 1: // Input 17-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[16 + i].meterInfo);
					surface->SetMeterLed(X32_BOARD_M, i, mixer->dsp->rChannel[16 + i + 8].meterInfo);
				}
				break;
			case 2: // Aux 1-8 / FX-Return
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[32 + i].meterInfo);
					//setMeterLed(X32_BOARD_M, i, 0);
				}
				break;
			case 3: // Bus 1-16
				break;
		}
	}

	if (config->IsModelX32CompactOrProducer()) {

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

		switch (activeBank_inputFader) {
			case 0: // Input 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[i].meterInfo);
				}
				break;
			case 1: // Input 9-16
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[8 + i].meterInfo);
				}
				break;
			case 2: // Input 17-24
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[16 + i].meterInfo);
				}
				break;
			case 3: // Input 25-32
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[24 + i].meterInfo);
				}
				break;
			case 4: // Aux 1-8
				for (uint8_t i = 0; i < 8; i++) {
					surface->SetMeterLed(X32_BOARD_L, i, mixer->dsp->rChannel[32 + i].meterInfo);
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
		uint8_t chanIdx = GetSelectedvChannelIndex();
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

uint8_t X32Ctrl::surfaceCalcPreampMeter(uint8_t channel) {
	if (channel >= 40) {
		return 0; // no preamps outside the 40 dsp-channels
	}

	uint32_t audiodata = mixer->dsp->rChannel[channel].meter;
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
	helper->DEBUG_X32CTRL(DEBUGLEVEL_NORMAL, "ChangeSelect(): selected channel index: %d, direction: %d, new channel index: %d\n", GetSelectedvChannelIndex(), direction, newSelectedVChannel);
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
					helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Unhandled button detected.\n");
					break;
			}
		}
	}

	// Display Encoders
	// - are independent from Surface Modes!
	if (config->IsModelX32FullOrCompactOrProducerOrRack()){
		if (state->activePage == X32_PAGE_CONFIG){
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
		}else if (state->activePage == X32_PAGE_GATE){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetGate(GetSelectedvChannelIndex(), 'T', -80.0f);
						break;
					case X32_BTN_ENCODER2:
						mixer->SetGate(GetSelectedvChannelIndex(), 'R', 60.0f);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetGate(GetSelectedvChannelIndex(), 'A', 10.0f);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetGate(GetSelectedvChannelIndex(), 'H', 50.0f);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetGate(GetSelectedvChannelIndex(), 'r', 250.0f);
						break;
					case X32_BTN_ENCODER6:
						break;
					default:
						break;
				}
			}
		}else if (state->activePage == X32_PAGE_COMPRESSOR){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'T', 0.0f);
						break;
					case X32_BTN_ENCODER2:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'R', 60.0f);
						break;
					case X32_BTN_ENCODER3:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'M', 0.0f);
						break;
					case X32_BTN_ENCODER4:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'A', 10.0f);
						break;
					case X32_BTN_ENCODER5:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'H', 10.0f);
						break;
					case X32_BTN_ENCODER6:
						mixer->SetDynamics(GetSelectedvChannelIndex(), 'r', 150.0f);
						break;
					default:
						break;
				}
			}
		}else if (state->activePage == X32_PAGE_EQ){
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
		}else if (state->activePage == X32_PAGE_UTILITY){
			if (buttonPressed){
				switch (button){
					case X32_BTN_ENCODER1:
						// Reload DSP1
						mixer->dsp->spi->CloseConnectionDsps();
						mixer->dsp->spi->UploadBitstreamDsps();
						mixer->dsp->spi->OpenConnectionDsps();
						usleep(50000); // wait 50ms
						mixer->dsp->SendAll();
						mixer->dsp->SendAll(); // currently we need to send the data twice. Maybe a bug in the SPI-connection or a timing-issue?
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
						state->debugvalue = 0;
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
				mixer->ChangeVolume(GetSelectedvChannelIndex(), amount);				
				break;
			case X32_BTN_EQ_MODE:
				mixer->ChangePeq(GetSelectedvChannelIndex(), activeEQ, 'T', amount);
				break;
			case X32_ENC_GAIN:
				mixer->ChangeGain(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_GATE:
				mixer->ChangeGate(GetSelectedvChannelIndex(), 'T', amount);
				break;
			case X32_ENC_LOWCUT:
				mixer->ChangeLowcut(GetSelectedvChannelIndex(), amount);
				break;
			case X32_ENC_DYNAMICS:
				mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'T', amount);
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
		if (state->activePage == X32_PAGE_CONFIG){
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
		}else if (state->activePage == X32_PAGE_GATE) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGate(GetSelectedvChannelIndex(), 'T', amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeGate(GetSelectedvChannelIndex(), 'R', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeGate(GetSelectedvChannelIndex(), 'A', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeGate(GetSelectedvChannelIndex(), 'H', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeGate(GetSelectedvChannelIndex(), 'r', amount);
					break;
				case X32_ENC_ENCODER6:
					break;
				default:
					break;
			}
		}else if (state->activePage == X32_PAGE_COMPRESSOR) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'T', amount);
					break;
				case X32_ENC_ENCODER2:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'R', amount);
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'M', amount);
					break;
				case X32_ENC_ENCODER4:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'A', amount);
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'H', amount);
					break;
				case X32_ENC_ENCODER6:
					mixer->ChangeDynamics(GetSelectedvChannelIndex(), 'r', amount);
					break;
				default:
					break;
			}
		}else if (state->activePage == X32_PAGE_EQ) {
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
		}else if (state->activePage == X32_PAGE_ROUTING_FPGA) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGuiSelection(amount);
					break;
				case X32_ENC_ENCODER2:
					if (amount < 0) {
						mixer->ChangeGuiSelection(-8);
					}else{
						mixer->ChangeGuiSelection(8);
					}
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeHardwareInput(state->gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					uint8_t currentItem;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
						mixer->ChangeHardwareInput(i, absoluteChange);
					}
					break;
				case X32_ENC_ENCODER5:
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
		}else if (state->activePage == X32_PAGE_ROUTING_DSP1) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGuiSelection(amount);
					break;
				case X32_ENC_ENCODER2:
					if (amount < 0) {
						mixer->ChangeGuiSelection(-8);
					}else{
						mixer->ChangeGuiSelection(8);
					}
					break;
				case X32_ENC_ENCODER3:
					mixer->ChangeDspInput(state->gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
						mixer->ChangeDspInput(i, absoluteChange);
					}
					break;
				case X32_ENC_ENCODER5:
					mixer->ChangeDspInputTapPoint(state->gui_selected_item, amount);
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
		}else if (state->activePage == X32_PAGE_ROUTING_DSP2) {
			switch (encoder){
				case X32_ENC_ENCODER1:
					mixer->ChangeGuiSelection(amount);
					break;
				case X32_ENC_ENCODER2:
					if (amount < 0) {
						mixer->ChangeGuiSelection(-8);
					}else{
						mixer->ChangeGuiSelection(8);
					}
					break;
				case X32_ENC_ENCODER3:
					if (state->gui_selected_item < MAX_DSP_OUTPUTCHANNELS) {
						mixer->ChangeDspOutput(state->gui_selected_item, amount);
					}else if ((state->gui_selected_item >= MAX_DSP_OUTPUTCHANNELS) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS))) {
						mixer->ChangeDspFxOutput(state->gui_selected_item - MAX_DSP_OUTPUTCHANNELS, amount);
					}else if ((state->gui_selected_item >= (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS)) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS + MAX_DSP_AUXCHANNELS))) {
						mixer->ChangeDspAuxOutput(state->gui_selected_item - MAX_DSP_OUTPUTCHANNELS - MAX_DSP_AUXCHANNELS, amount);
					}
					break;
				case X32_ENC_ENCODER4:
					int8_t absoluteChange;
					if (amount < 0) {
						absoluteChange = -8;
					}else{
						absoluteChange = 8;
					}
					for (uint8_t i=state->gui_selected_item; i<(state->gui_selected_item+8); i++) {
						if (i < MAX_DSP_OUTPUTCHANNELS) {
							mixer->ChangeDspOutput(i, absoluteChange);
						}else if ((i >= MAX_DSP_OUTPUTCHANNELS) && (i < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS))) {
							mixer->ChangeDspFxOutput(i - MAX_DSP_OUTPUTCHANNELS, absoluteChange);
						}else if ((i >= (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS)) && (i < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS + MAX_DSP_AUXCHANNELS))) {
							mixer->ChangeDspAuxOutput(i - MAX_DSP_OUTPUTCHANNELS - MAX_DSP_AUXCHANNELS, absoluteChange);
						}
					}
					break;
				case X32_ENC_ENCODER5:
					if (state->gui_selected_item < MAX_DSP_OUTPUTCHANNELS) {
						mixer->ChangeDspOutputTapPoint(state->gui_selected_item, amount);
					}else if ((state->gui_selected_item >= MAX_DSP_OUTPUTCHANNELS) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS))) {
						mixer->ChangeDspFxOutputTapPoint(state->gui_selected_item - MAX_DSP_OUTPUTCHANNELS, amount);
					}else if ((state->gui_selected_item >= (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS)) && (state->gui_selected_item < (MAX_DSP_OUTPUTCHANNELS + MAX_DSP_FXCHANNELS + MAX_DSP_AUXCHANNELS))) {
						mixer->ChangeDspAuxOutputTapPoint(state->gui_selected_item - MAX_DSP_OUTPUTCHANNELS - MAX_DSP_AUXCHANNELS, amount);
					}
					break;
				case X32_ENC_ENCODER6:
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
		}else if (state->activePage == X32_PAGE_UTILITY) {
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
					state->debugvalue2++;
					state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
					break;
				case X32_ENC_ENCODER6:
					//surface->SetLed(state->debugvalue2,state->debugvalue++, true);
					surface->SetLcd(state->debugvalue2, state->debugvalue++, 1, 0, 0, 0xA0, 0x20, 10, 10, "huhu", 0x00, 0, 0, "");
					state->SetChangeFlags(X32_MIXER_CHANGED_GUI);
					break;
				default:  
					// just here to avoid compiler warnings                  
					break;
			}
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
