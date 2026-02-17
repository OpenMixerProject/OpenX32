#include "config.h"

Config::Config(Helper* h)
{
    this->helper = h;

    DefineMixerparameters();
}

void Config::SetModel(String model){

    if (model == "X32CORE") {
        _model = X32_MODEL::CORE;
    }else if (model == "X32RACK") {
        _model = X32_MODEL::RACK;
    }else if (model == "X32Producer") {
        _model = X32_MODEL::PRODUCER;
    }else if (model == "X32C") {
        _model =  X32_MODEL::COMPACT;
    }else if (model == "X32") {
        _model = X32_MODEL::FULL;
    }else{
        //x32log("ERROR: No model detected!\n");
        _model = X32_MODEL::NONE;
    }
};

bool Config::IsModelX32Full() {
    return (_model == X32_MODEL::FULL);
}
bool Config::IsModelX32FullOrCompactOrProducer() {
    return ((_model == X32_MODEL::FULL) || (_model == X32_MODEL::COMPACT) || (_model == X32_MODEL::PRODUCER));
}
bool Config::IsModelX32FullOrCompactOrProducerOrRack() {
    return (IsModelX32FullOrCompactOrProducer() || (_model == X32_MODEL::RACK));
}
bool Config::IsModelX32CompactOrProducer() {
    return ((_model == X32_MODEL::COMPACT) || (_model == X32_MODEL::PRODUCER));
}
bool Config::IsModelX32Core() {
    return (_model == X32_MODEL::CORE);
}
bool Config::IsModelX32Rack() {
    return (_model == X32_MODEL::RACK);
}
bool Config::IsModelX32Producer() {
    return (_model == X32_MODEL::PRODUCER);
}
bool Config::IsModelX32Compact() {
    return (_model == X32_MODEL::COMPACT);
}
bool Config::IsModelX32FullOrCompact() {
    return ((_model == X32_MODEL::FULL) || (_model == X32_MODEL::COMPACT));
}
bool Config::IsModelX32ProducerOrRack() {
    return ((_model == X32_MODEL::PRODUCER) || (_model == X32_MODEL::RACK));
}

void Config::SetBankMode(X32_SURFACE_MODE_BANKING mode) {
    _activeBankMode = mode;
}

X32_SURFACE_MODE_BANKING Config::GetBankMode() {
    return _activeBankMode;
}


//######################################################################################################################################
//#
//#  ##     ## #### ##      ## ######## ########  ########     ###    ########     ###    ##     ## ######## ######## ######## ########  
//#  ###   ###  ##   ##    ##  ##       ##     ## ##     ##   ## ##   ##     ##   ## ##   ###   ### ##          ##    ##       ##     ## 
//#  #### ####  ##    ##  ##   ##       ##     ## ##     ##  ##   ##  ##     ##  ##   ##  #### #### ##          ##    ##       ##     ## 
//#  ## ### ##  ##     ####    ######   ########  ########  ##     ## ########  ##     ## ## ### ## ######      ##    ######   ########  
//#  ##     ##  ##    ##  ##   ##       ##   ##   ##        ######### ##   ##   ######### ##     ## ##          ##    ##       ##   ##   
//#  ##     ##  ##   ##    ##  ##       ##    ##  ##        ##     ## ##    ##  ##     ## ##     ## ##          ##    ##       ##    ##  
//#  ##     ## #### ##      ## ######## ##     ## ##        ##     ## ##     ## ##     ## ##     ## ########    ##    ######## ##     ## 
//#
//#
//#  ########  ######## ######## #### ##    ## #### ######## ####  #######  ##    ##  ######  
//#  ##     ## ##       ##        ##  ###   ##  ##     ##     ##  ##     ## ###   ## ##    ## 
//#  ##     ## ##       ##        ##  ####  ##  ##     ##     ##  ##     ## ####  ## ##       
//#  ##     ## ######   ######    ##  ## ## ##  ##     ##     ##  ##     ## ## ## ##  ######  
//#  ##     ## ##       ##        ##  ##  ####  ##     ##     ##  ##     ## ##  ####       ## 
//#  ##     ## ##       ##        ##  ##   ###  ##     ##     ##  ##     ## ##   ### ##    ## 
//#  ########  ######## ##       #### ##    ## ####    ##    ####  #######  ##    ##  ######  
//#
//######################################################################################################################################

Mixerparameter* Config::DefParameter(MP_ID parameter_id, MP_CAT category, String group, String name, uint count) {
	
	// create it
	Mixerparameter* newMpd = new Mixerparameter(parameter_id, category, group, name, count);

	// store in mixerparameter map (mpm)
    #ifdef MPM_AS_ARRAY
    mpm[(uint)parameter_id] = newMpd;
    #else
	mpm->insert({parameter_id, newMpd});
    #endif

	// return for further definition
	return newMpd;
}

void Config::DefineMixerparameters() {

	using enum MP_ID;

    // #####################
    // # Special Parameters
    // #####################

    DefParameter(NONE, MP_CAT::NONE, "", "")->DefNoConfigfile();
    DefParameter(PAGE_CUSTOM_ENCODER, MP_CAT::NONE, "", "")->DefNoConfigfile();

    // ############
    // # Settings
    // ###########

	MP_CAT cat = MP_CAT::SETTING;
    String group = "setting";

	DefParameter(LCD_CONTRAST, cat, group, "LCD Contrast")
	->DefMinMaxStandard_Uint(LCD_CONTRAST_MIN, LCD_CONTRAST_MAX, LCD_CONTRAST_DEFAULT);
    
    DefParameter(LED_BRIGHTNESS, cat, group, "LED Brightness")
	->DefMinMaxStandard_Uint(LED_BRIGHTNESS_1, LED_BRIGHTNESS_4, LED_BRIGHTNESS_4)
    ->DefStepsize(64);

    // TODO: DISPLAY_BRIGHTNESS

    DefParameter(SAMPLERATE, cat, group, "Samplerate")
    ->DefMinMaxStandard_Uint(44100, 48000, 48000);

    DefParameter(CARD_NUMBER_OF_CHANNELS, cat, group, "Card Channels")
    ->DefMinMaxStandard_Uint(0, 5, 0);

    DefParameter(CHANNEL_LCD_MODE, cat, group, "LCD Mode")
    ->DefUOM(MP_UOM::CHANNEL_LCD_MODE)
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 1, 0);
    

    // ##########
    // # Routing 
    // ##########

    cat = MP_CAT::ROUTING;
    group = "routing";

    DefParameter(ROUTING_FPGA, cat, group, "Routing FPGA", 208)  // 208 -> size of fpga routing struct! (prepared for both AES50-ports, but only 160 are used at the moment)
    ->DefUOM(MP_UOM::FPGA_ROUTING)
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, NUM_INPUT_CHANNEL, 0); // 161 = OFF + 160 channels

    DefParameter(ROUTING_DSP_INPUT, cat, group, "Routing DSP-Input", MAX_FPGA_TO_DSP1_CHANNELS) // 40 routable DSP-input channels from FPGA
    ->DefUOM(MP_UOM::DSP_ROUTING)
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, DSP_MAX_INTERNAL_CHANNELS - 1, 0); // 0=OFF, 92=Talkback

    DefParameter(ROUTING_DSP_INPUT_TAPPOINT, cat, group, "Routing DSP-Input Tappoint", MAX_FPGA_TO_DSP1_CHANNELS) // 40 routable DSP-input channels from FPGA
    ->DefUOM(MP_UOM::TAPPOINT)
    ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::INPUT);

    DefParameter(ROUTING_DSP_OUTPUT, cat, group, "Routing DSP-Output", (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS)) // 40 routable DSP-output-channels to FPGA and 24 routable DSP-output-channels to DSP2
    ->DefUOM(MP_UOM::DSP_ROUTING)
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, DSP_MAX_INTERNAL_CHANNELS - 1, 0); // 0=OFF, 92=Talkback

    DefParameter(ROUTING_DSP_OUTPUT_TAPPOINT, cat, group, "Routing DSP-Output Tappoint", (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS)) // 40 routable DSP-output-channels to FPGA and 24 routable DSP-output-channels to DSP2
    ->DefUOM(MP_UOM::TAPPOINT)
    ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::POST_FADER);

    // ########
    // # State
    // ########

    cat = MP_CAT::STATE;
    group = "state";

    DefParameter(SELECTED_CHANNEL, cat, group, "Selected Channel")
    ->DefUOM(MP_UOM::CHANNEL)
    ->DefHideEncoderSlider()
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, MAX_VCHANNELS - 1, 0);

    DefParameter(ACTIVE_PAGE, cat, group, "Active Page")
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, 255, (uint)X32_PAGE::HOME);

    DefParameter(BANKING_EQ, cat, group, "Banking EQ")
    ->DefHideEncoderSlider()
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 3, 0);

    DefParameter(BANKING_INPUT, cat, group, "Banking Input Section")
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, 7, 0);

    DefParameter(BANKING_BUS, cat, group, "Banking Bus Section")
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, 3, 0);

    DefParameter(BANKING_BUS_SENDS, cat, group, "Banking Bus Section")
    ->DefHideEncoderSlider()
    ->DefMinMaxStandard_Uint(0, 3, 0);


    // ###########
    // # Global
    // ###########

	cat = MP_CAT::GLOBAL;
    group = "global";

    DefParameter(MONITOR_VOLUME, cat, group, "Monitor Volume")
    ->DefNameShort("Mon")
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);

    DefParameter(MONITOR_TAPPOINT, cat, group, "Monitor Tappoint")
    ->DefMinMaxStandard_Uint(0, 81, 0);

    // ###########
    // # Channels
    // ###########

	cat = MP_CAT::CHANNEL;
    group = "channel";

    DefParameter(CHANNEL_NAME_INTERN, cat, group, "Channelname Intern", MAX_VCHANNELS)->DefNameShort("Internalname")
    ->DefStandard_String("CH")
    ->DefReadonly()
    ->DefNoConfigfile();
    
    DefParameter(CHANNEL_NAME, cat, group, "Channelname", MAX_VCHANNELS)
    ->DefStandard_String("Kanal");

    DefParameter(CHANNEL_COLOR, cat, group, "Channelcolor", MAX_VCHANNELS)
    ->DefMinMaxStandard_Uint((uint)X32_COLOR::BLACK, (uint)X32_COLOR::WHITE, (uint)X32_COLOR::YELLOW);

    DefParameter(CHANNEL_COLOR_INVERTED, cat, group, "Color Inverted", MAX_VCHANNELS)
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_PHASE_INVERT, cat, group, "Phase Inverted", MAX_VCHANNELS)->DefNameShort("Inverted")
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_PHANTOM, cat, group, "Phantom", MAX_VCHANNELS)->DefNameShort("48V")
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_GAIN, cat, group, "Gain", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(CHANNEL_GAIN_MIN, CHANNEL_GAIN_MAX, 0.0f, 1);
    
    DefParameter(CHANNEL_VOLUME, cat, group, "Volume", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);

    DefParameter(CHANNEL_VOLUME_SUB, cat, group, "Sub", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);
    
    DefParameter(CHANNEL_SELECTED, cat, group, "Selected", MAX_VCHANNELS)
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_SOLO, cat, group, "Solo", MAX_VCHANNELS)
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_MUTE, cat, group, "Mute", MAX_VCHANNELS)
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_PANORAMA, cat, group, "Pan/Bal", MAX_VCHANNELS)
    ->DefMinMaxStandard_Float(CHANNEL_PANORAMA_MIN, CHANNEL_PANORAMA_MAX, 0.0f)
    ->DefStepsize(2);

    DefParameter(CHANNEL_LOWCUT_FREQ, cat, group, "LowcutFreq", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::HZ)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 20.0f);

    // Sends
    cat = MP_CAT::CHANNEL_SENDS;

    for(auto const& parameter_id : {
        CHANNEL_BUS_SEND01, CHANNEL_BUS_SEND02, CHANNEL_BUS_SEND03, CHANNEL_BUS_SEND04, CHANNEL_BUS_SEND05, CHANNEL_BUS_SEND06,
        CHANNEL_BUS_SEND07, CHANNEL_BUS_SEND08, CHANNEL_BUS_SEND09, CHANNEL_BUS_SEND10, CHANNEL_BUS_SEND11, CHANNEL_BUS_SEND12,
        CHANNEL_BUS_SEND11, CHANNEL_BUS_SEND13, CHANNEL_BUS_SEND14, CHANNEL_BUS_SEND15, CHANNEL_BUS_SEND16
    } )
    {
        DefParameter(parameter_id, cat, group, String("Bus Send"), MAX_VCHANNELS)
        ->DefUOM(MP_UOM::DB)
        ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);
    }

    for(auto const& parameter_id : {
        CHANNEL_BUS_SEND01_TAPPOINT,
        CHANNEL_BUS_SEND02_TAPPOINT,
        CHANNEL_BUS_SEND03_TAPPOINT,
        CHANNEL_BUS_SEND04_TAPPOINT,
        CHANNEL_BUS_SEND05_TAPPOINT,
        CHANNEL_BUS_SEND06_TAPPOINT,
        CHANNEL_BUS_SEND07_TAPPOINT,
        CHANNEL_BUS_SEND08_TAPPOINT,
        CHANNEL_BUS_SEND09_TAPPOINT,
        CHANNEL_BUS_SEND10_TAPPOINT,
        CHANNEL_BUS_SEND11_TAPPOINT,
        CHANNEL_BUS_SEND12_TAPPOINT,
        CHANNEL_BUS_SEND13_TAPPOINT,
        CHANNEL_BUS_SEND14_TAPPOINT,
        CHANNEL_BUS_SEND15_TAPPOINT,
        CHANNEL_BUS_SEND16_TAPPOINT
    } )
    {
        DefParameter(parameter_id, cat, group, "Bus Send Tappoint", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::TAPPOINT)
        ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::INPUT);
    }    


    // gate
    cat = MP_CAT::CHANNEL_GATE;
    group = "channel_gate";

    DefParameter(CHANNEL_GATE_TRESHOLD, cat, group, "Threshold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(GATE_THRESHOLD_MIN, GATE_THRESHOLD_MAX, GATE_THRESHOLD_MIN, 0);
    
    DefParameter(CHANNEL_GATE_RANGE, cat, group, "Range", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(GATE_RANGE_MIN, GATE_RANGE_MAX, GATE_RANGE_MAX, 1);
    
    DefParameter(CHANNEL_GATE_ATTACK, cat, group, "Attack", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(GATE_ATTACK_MIN, GATE_ATTACK_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_GATE_HOLD, cat, group, "Hold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(GATE_HOLD_MIN, GATE_HOLD_MAX, 50.0f, 0);
    
    DefParameter(CHANNEL_GATE_RELEASE, cat, group, "Release", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(GATE_RELEASE_MIN, GATE_RELEASE_MAX, 250.0f, 0);

    // dynamics
    group = "channel_dynamics";
    cat = MP_CAT::CHANNEL_DYNAMICS;

    DefParameter(CHANNEL_DYNAMICS_TRESHOLD, cat, group, "Threshold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(DYNAMICS_THRESHOLD_MIN, DYNAMICS_THRESHOLD_MAX, DYNAMICS_THRESHOLD_MAX, 0);
    
    DefParameter(CHANNEL_DYNAMICS_RATIO, cat, group, "Ratio", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::NONE)
    ->DefMinMaxStandard_Float(DYNAMICS_RATIO_MIN, DYNAMICS_RATIO_MAX, 3, 1);
    
    DefParameter(CHANNEL_DYNAMICS_MAKEUP, cat, group, "Makeup", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(DYNAMICS_MAKEUP_MIN, DYNAMICS_MAKEUP_MAX, DYNAMICS_MAKEUP_MIN, 1);
    
    DefParameter(CHANNEL_DYNAMICS_ATTACK, cat, group, "Attack", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(DYNAMICS_ATTACK_MIN, DYNAMICS_ATTACK_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_DYNAMICS_HOLD, cat, group, "Hold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(DYNAMICS_HOLD_MIN, DYNAMICS_HOLD_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_DYNAMICS_RELEASE, cat, group, "Release", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(DYNAMICS_RELEASE_MIN, DYNAMICS_RELEASE_MAX, 150.0f, 0);

    // EQ
    group = "channel_eq";
    cat = MP_CAT::CHANNEL_EQ;

    for(auto const& parameter_id : {CHANNEL_EQ_TYPE1, CHANNEL_EQ_TYPE2, CHANNEL_EQ_TYPE3, CHANNEL_EQ_TYPE4} )
    {
        DefParameter(parameter_id, cat, group, "Type", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::EQ_TYPE)
        ->DefHideEncoderReset()
        ->DefMinMaxStandard_Uint(0, 7, 1);
    }

	DefParameter(CHANNEL_EQ_FREQ1, cat, group, "Frequenz", MAX_VCHANNELS)
	->DefUOM(MP_UOM::HZ)
	->DefMinMaxStandard_Float(20.0f, 20000.0f, 125.0f);

	DefParameter(CHANNEL_EQ_FREQ2, cat, group, "Frequenz", MAX_VCHANNELS)
	->DefUOM(MP_UOM::HZ)
	->DefMinMaxStandard_Float(20.0f, 20000.0f, 500.0f);

	DefParameter(CHANNEL_EQ_FREQ3, cat, group, "Frequenz", MAX_VCHANNELS)
	->DefUOM(MP_UOM::HZ)
	->DefMinMaxStandard_Float(20.0f, 20000.0f, 2000.0f);

	DefParameter(CHANNEL_EQ_FREQ4, cat, group, "Frequenz", MAX_VCHANNELS)
	->DefUOM(MP_UOM::HZ)
	->DefMinMaxStandard_Float(20.0f, 20000.0f, 10000.0f);

    for(auto const& parameter_id : {CHANNEL_EQ_GAIN1, CHANNEL_EQ_GAIN2, CHANNEL_EQ_GAIN3, CHANNEL_EQ_GAIN4} )
    {
        DefParameter(parameter_id, cat, group, "Gain", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::DB)
        ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);
    }

    for(auto const& parameter_id : {CHANNEL_EQ_Q1, CHANNEL_EQ_Q2, CHANNEL_EQ_Q3, CHANNEL_EQ_Q4} )
    {
        DefParameter(parameter_id, cat, group, "Q", MAX_VCHANNELS)
        ->DefStepsize(0.1f)
        ->DefMinMaxStandard_Float(0.3f, 10.0f, 2.0f, 1);
    }

    // ###########
    // # FX
    // ###########
	cat = MP_CAT::FX;

    // reverb
    group = "fx_reverb";

    #define FX_REVERB_ROOMSIZE_MIN         0.0f // ms
    #define FX_REVERB_ROOMSIZE_DEFAULT   150.0f // ms
    #define FX_REVERB_ROOMSIZE_MAX      1000.0f // ms
    #define FX_REVERB_RT60_MIN             0.0f // s
    #define FX_REVERB_RT60_DEFAULT         3.0f // s
    #define FX_REVERB_RT60_MAX           100.0f // s
    #define FX_REVERB_LPFREQ_MIN          20.0f // Hz
    #define FX_REVERB_LPFREQ_DEFAULT   14000.0f // Hz
    #define FX_REVERB_LPFREQ_MAX       20000.0f // Hz
    #define FX_REVERB_DRY_MIN              0.0f //
    #define FX_REVERB_DRY_DEFAULT          1.0f //
    #define FX_REVERB_DRY_MAX              1.0f //
    #define FX_REVERB_WET_MIN              0.0f //
    #define FX_REVERB_WET_DEFAULT          0.25f //
    #define FX_REVERB_WET_MAX              1.0f //

    // reverb roomsize
    DefParameter(FX_REVERB_ROOMSIZE, cat, group, "Roomsize", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(FX_REVERB_ROOMSIZE_MIN, FX_REVERB_ROOMSIZE_MAX, FX_REVERB_ROOMSIZE_DEFAULT, 0);
    // reverb rt60
    DefParameter(FX_REVERB_RT60, cat, group, "RT60", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::SECONDS)
    ->DefMinMaxStandard_Float(FX_REVERB_RT60_MIN, FX_REVERB_RT60_MAX, FX_REVERB_RT60_DEFAULT, 1);
    // reverb lowpass
    DefParameter(FX_REVERB_LPFREQ, cat, group, "LowPassFreq", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::KHZ)
    ->DefMinMaxStandard_Float(FX_REVERB_LPFREQ_MIN, FX_REVERB_LPFREQ_MAX, FX_REVERB_LPFREQ_DEFAULT, 0);
    // reverb dry
    DefParameter(FX_REVERB_DRY, cat, group, "Dry", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::PERCENT)
    ->DefMinMaxStandard_Float(FX_REVERB_DRY_MIN, FX_REVERB_DRY_MAX, FX_REVERB_DRY_DEFAULT);
    // reverb wet
    DefParameter(FX_REVERB_WET, cat, group, "Wet", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::PERCENT)
    ->DefMinMaxStandard_Float(FX_REVERB_WET_MIN, FX_REVERB_WET_MAX, FX_REVERB_WET_DEFAULT);

    // chorus
    group = "fx_chorus";
    #define FX_CHORUS_DEPTH_A_MIN          0.0f //
    #define FX_CHORUS_DEPTH_A_DEFAULT     10.0f //
    #define FX_CHORUS_DEPTH_A_MAX        100.0f //
    #define FX_CHORUS_DEPTH_B_MIN          0.0f //
    #define FX_CHORUS_DEPTH_B_DEFAULT     10.0f //
    #define FX_CHORUS_DEPTH_B_MAX        100.0f //
    #define FX_CHORUS_DELAY_A_MIN          0.0f // ms
    #define FX_CHORUS_DELAY_A_DEFAULT     15.0f // ms
    #define FX_CHORUS_DELAY_A_MAX        100.0f // ms
    #define FX_CHORUS_DELAY_B_MIN          0.0f // ms
    #define FX_CHORUS_DELAY_B_DEFAULT     20.0f // ms
    #define FX_CHORUS_DELAY_B_MAX        100.0f // ms
    #define FX_CHORUS_PHASE_A_MIN          0.0f //
    #define FX_CHORUS_PHASE_A_DEFAULT      0.0f //
    #define FX_CHORUS_PHASE_A_MAX        100.0f //
    #define FX_CHORUS_PHASE_B_MIN          0.0f //
    #define FX_CHORUS_PHASE_B_DEFAULT      0.0f //
    #define FX_CHORUS_PHASE_B_MAX        100.0f //
    #define FX_CHORUS_FREQ_A_MIN           0.0f //
    #define FX_CHORUS_FREQ_A_DEFAULT       1.5f //
    #define FX_CHORUS_FREQ_A_MAX         100.0f //
    #define FX_CHORUS_FREQ_B_MIN           0.0f //
    #define FX_CHORUS_FREQ_B_DEFAULT       1.6f //
    #define FX_CHORUS_FREQ_B_MAX         100.0f //

    // chorus depth
    DefParameter(FX_CHORUS_DEPTH_A, cat, group, "Depth A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefMinMaxStandard_Float(FX_CHORUS_DEPTH_A_MIN, FX_CHORUS_DEPTH_A_MAX, FX_CHORUS_DEPTH_A_DEFAULT);
    DefParameter(FX_CHORUS_DEPTH_B, cat, group, "Depth B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefMinMaxStandard_Float(FX_CHORUS_DEPTH_B_MIN, FX_CHORUS_DEPTH_B_MAX, FX_CHORUS_DEPTH_B_DEFAULT);
    // chorus delay
    DefParameter(FX_CHORUS_DELAY_A, cat, group, "Delay A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(FX_CHORUS_DELAY_A_MIN, FX_CHORUS_DELAY_A_MAX, FX_CHORUS_DELAY_A_DEFAULT);
    DefParameter(FX_CHORUS_DELAY_B, cat, group, "Delay B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefMinMaxStandard_Float(FX_CHORUS_DELAY_B_MIN, FX_CHORUS_DELAY_B_MAX, FX_CHORUS_DELAY_B_DEFAULT);
    // chorus phase
    DefParameter(FX_CHORUS_PHASE_A, cat, group, "Phase A", MAX_FX_SLOTS)->DefUOM(MP_UOM::NONE)->DefMinMaxStandard_Float(FX_CHORUS_PHASE_A_MIN, FX_CHORUS_PHASE_A_MAX, FX_CHORUS_PHASE_A_DEFAULT);
    DefParameter(FX_CHORUS_PHASE_B, cat, group, "Phase B", MAX_FX_SLOTS)->DefUOM(MP_UOM::NONE)->DefMinMaxStandard_Float(FX_CHORUS_PHASE_B_MIN, FX_CHORUS_PHASE_B_MAX, FX_CHORUS_PHASE_B_DEFAULT);
    // chorus freq
    DefParameter(FX_CHORUS_FREQ_A, cat, group, "Freq A", MAX_FX_SLOTS)->DefUOM(MP_UOM::KHZ)->DefMinMaxStandard_Float(FX_CHORUS_FREQ_A_MIN, FX_CHORUS_FREQ_A_MAX, FX_CHORUS_FREQ_A_DEFAULT);
    DefParameter(FX_CHORUS_FREQ_B, cat, group, "Freq B", MAX_FX_SLOTS)->DefUOM(MP_UOM::KHZ)->DefMinMaxStandard_Float(FX_CHORUS_FREQ_B_MIN, FX_CHORUS_FREQ_B_MAX, FX_CHORUS_FREQ_B_DEFAULT);
    // chorus mix
    DefParameter(FX_CHORUS_MIX, cat, group, "Mix", MAX_FX_SLOTS)->DefUOM(MP_UOM::PERCENT)->DefMinMaxStandard_Float(0.0f, 1.0f, 0.5f);

    // transientshaper
    group = "fx_transientshaper";

    // fast
    DefParameter(FX_TRANSIENTSHAPER_FAST, cat, group, "Fast", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);
    // medium
    DefParameter(FX_TRANSIENTSHAPER_MEDIUM, cat, group, "Medium", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 15.0f);
    // slow
    DefParameter(FX_TRANSIENTSHAPER_SLOW, cat, group, "Slow", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 150.0f);
    // attack
    DefParameter(FX_TRANSIENTSHAPER_ATTACK, cat, group, "Attack", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);
    // sustain
    DefParameter(FX_TRANSIENTSHAPER_SUSTAIN, cat, group, "Sustain", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);
    // delay
    DefParameter(FX_TRANSIENTSHAPER_DELAY, cat, group, "Delay", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);

    // FX_TYPE_OVERDRIVE            preGain Q hpfInputFreq lpfInputFreq lpfOutputFreq
    group = "fx_overdrive";

    // delay A/B
    group = "fx_delay";
    DefParameter(FX_DELAY_DELAY_A, cat, group, "Delay A", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 350.0f);
    DefParameter(FX_DELAY_DELAY_B, cat, group, "Delay B", MAX_FX_SLOTS)->DefUOM(MP_UOM::MS)->DefMinMaxStandard_Float(0.0f, 1000.0f, 450.0f);

    // FX_TYPE_MULTIBANDCOMPRESOR   channel band threshold ratio attack  hold   release   makeup
    group = "fx_multibandcompressor";

    // FX_TYPE_DYNAMICEQ            band type freq staticGain  maxDynGain  Q  thresh  ratio  attack  release
    group = "fx_dynamiceq";



    #ifdef MPM_AS_ARRAY
    //#####################################################
    //#  fill all empty parameter indexes with MP_ID::NONE
    //#####################################################
    for (uint i = 0; i < (uint)MP_ID::__ELEMENT_COUNTER_DO_NOT_MOVE; i++)
    {
        if (mpm[i] == 0)
        {
            mpm[i] = mpm[(uint)MP_ID::NONE];
        }
    }
    #endif
}


//########################################################################################################################################
//#
//#    ######  ######## ########       ##  ######   ######## ########       ##  ######  ##     ##    ###    ##    ##  ######   ######## 
//#   ##    ## ##          ##         ##  ##    ##  ##          ##         ##  ##    ## ##     ##   ## ##   ###   ## ##    ##  ##       
//#   ##       ##          ##        ##   ##        ##          ##        ##   ##       ##     ##  ##   ##  ####  ## ##        ##       
//#    ######  ######      ##       ##    ##   #### ######      ##       ##    ##       ######### ##     ## ## ## ## ##   #### ######   
//#         ## ##          ##      ##     ##    ##  ##          ##      ##     ##       ##     ## ######### ##  #### ##    ##  ##       
//#   ##    ## ##          ##     ##      ##    ##  ##          ##     ##      ##    ## ##     ## ##     ## ##   ### ##    ##  ##       
//#    ######  ########    ##    ##        ######   ########    ##    ##        ######  ##     ## ##     ## ##    ##  ######   ######## 
//#
//########################################################################################################################################

#ifdef MPM_AS_ARRAY
Mixerparameter** Config::GetParameterList()
#else
mixerparameter_map_t* Config::GetParameterList()
#endif
{
    return mpm;
}

mixerparameter_changed_t* Config::GetChangedParameterList()
{
    return mp_changedlist;
}

vector<uint> Config::GetChangedParameterIndexes(MP_CAT parameter_cat)
{
    vector<uint> changedIndexes;

    for (auto const& [parameter_id, indexSet] : *mp_changedlist)
    {
        if (
            #ifdef MPM_AS_ARRAY
            mpm[(uint)parameter_id]
            #else
            mpm->at(parameter_id)
            #endif
            ->GetCategory() == parameter_cat
        )
        {
            changedIndexes.insert(changedIndexes.end(), indexSet.begin(), indexSet.end());
        }
    }

    return changedIndexes;
}

vector<uint> Config::GetChangedParameterIndexes(vector<MP_ID> filter_ids)
{
    vector<uint> changedIndexes;
    
    // go over the list of changed Mixerparameter
    for (auto const& [parameter_id, indexSet] : *mp_changedlist)
    {
        // changed Mixerparameter matches to filter
        if (find(filter_ids.begin(), filter_ids.end(), parameter_id) != filter_ids.end())
        {
            changedIndexes.insert(changedIndexes.end(), indexSet.begin(), indexSet.end());
        }
    }

    return changedIndexes;
}

/// @brief Checks if the data of any of the Mixerparameters has changed.
/// @param parameter_id The ids of the Mixerparameters to check.
/// @param index The index of the Mixerparameters (usual the vchannel index or FX slot index).
/// @return True if any data has changed.
bool Config::HasParametersChanged(vector<MP_ID> parameter_id)
{
    for(uint i = 0; i < parameter_id.size(); i++)
    {
        if (mp_changedlist->contains(parameter_id.at(i)))
        {
            return true;
        }
    }

    return false;
}

/// @brief Checks if the data of any of the Mixerparameters has changed.
/// @param parameter_id The ids of the Mixerparameters to check.
/// @param index The index of the Mixerparameters (usual the vchannel index or FX slot index).
/// @return True if any data has changed.
bool Config::HasParametersChanged(vector<MP_ID> parameter_id, uint index)
{
    for(uint i = 0; i < parameter_id.size(); i++)
    {
        if (mp_changedlist->contains(parameter_id.at(i)) &&
            mp_changedlist->at(parameter_id.at(i)).contains(index))
        {
            return true;
        }
    }

    return false;
}

/// @brief Checks if the data of any of the Mixerparameters has changed.
/// @param parameter_cat The category of the Mixerparameters to check.
/// @param index The index of the Mixerparameters (usual the vchannel index or FX slot index).
/// @return True if any data has changed.
bool Config::HasParametersChanged(MP_CAT parameter_cat)
{
    for (auto const& [parameter_id, indexSet] : *mp_changedlist)
    {
        if (
            #ifdef MPM_AS_ARRAY
            mpm[(uint)parameter_id]
            #else
            mpm->at(parameter_id)
            #endif
            ->GetCategory() == parameter_cat)
        {
            return true;
        }
    }

    return false;
}

/// @brief Checks if the data of any of the Mixerparameters has changed.
/// @param parameter_cat The category of the Mixerparameters to check.
/// @param index The index of the Mixerparameters (usual the vchannel index or FX slot index).
/// @return True if any data has changed.
bool Config::HasParametersChanged(MP_CAT parameter_cat, uint index)
{
    for (auto const& [parameter_id, indexSet] : *mp_changedlist)
    {
        if (
            #ifdef MPM_AS_ARRAY
            mpm[(uint)parameter_id]
            #else
            mpm->at(parameter_id)
            #endif
            ->GetCategory() == parameter_cat &&
            indexSet.contains(index))
        {
            return true;
        }
    }

    return false;
}

/// @brief Checks if the data of the Mixerparameter has changed.
/// @param parameter_id The id of the Mixerparameter to check.
/// @param index The index of the Mixerparameter (usual the vchannel index or FX slot index).
/// @return True if the data has changed.
bool Config::HasParameterChanged(MP_ID parameter_id)
{
    return mp_changedlist->contains(parameter_id);
}

/// @brief Checks if the data of the Mixerparameter has changed.
/// @param parameter_id The id of the Mixerparameter to check.
/// @param index The index of the Mixerparameter (usual the vchannel index or FX slot index).
/// @return True if the data has changed.
bool Config::HasParameterChanged(MP_ID parameter_id, uint index)
{
    return mp_changedlist->contains(parameter_id) &&
           mp_changedlist->at(parameter_id).contains(index);
}

/// @brief Checks if any data in the Mixerparameters has changed.
/// @return True if the data in any Mixerparameter has changed.
bool Config::HasAnyParameterChanged()
{
    return mp_changedlist->size() > 0;
}

void Config::ResetChangedParameterList()
{
    mp_changedlist->clear();
}

Mixerparameter* Config::GetParameter(MP_ID mp)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp];
    #else
    if (!mpm->contains(mp))
    {
        return mpm->at(MP_ID::NONE);
    }
    return mpm->at(mp);
    #endif
}

float Config::GetFloat(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetFloat(index);
    #else
    return mpm->at(mp)->GetFloat(index);
    #endif
}

int Config::GetInt(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetInt(index);
    #else
    return mpm->at(mp)->GetInt(index);
    #endif
}

uint Config::GetUint(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetUint(index);
    #else
    return mpm->at(mp)->GetUint(index);
    #endif
}

bool Config::GetBool(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetBool(index);
    #else
    return mpm->at(mp)->GetBool(index);
    #endif
}

String Config::GetString(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetString(index);
    #else
    return mpm->at(mp)->GetString(index);
    #endif
}

uint Config::GetPercent(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    return mpm[(uint)mp]->GetPercent(index);
    #else
    return mpm->at(mp)->GetPercent(index);
    #endif
}

void Config::Set(MP_ID mp, float value, uint index)
{
    #ifdef MPM_AS_ARRAY
    mpm[(uint)mp]
    #else
    mpm->at(mp)
    #endif
    ->Set(value, index);

    SetParameterChanged(mp, index);
}

void Config::Set(MP_ID mp, String value_string, uint index)
{
    #ifdef MPM_AS_ARRAY
    mpm[(uint)mp]
    #else
    mpm->at(mp)
    #endif
    ->Set(value_string, index);

    SetParameterChanged(mp, index);
}

void Config::SetParameterChanged(MP_ID mp, uint index)
{
    if (mp_changedlist->contains(mp))
    {
        mp_changedlist->at(mp).insert(index);
    }
    else
    {
        mp_changedlist->insert({mp, {index}});
    }

    if (helper->DEBUG_MIXER(DEBUGLEVEL_VERBOSE))
    {
        Mixerparameter* parameter = GetParameter(mp);

        String message = "Mixerparameter \"" + parameter->GetName(index) + "\" ";
        
        if (parameter->GetInstances() > 1)
        {
            message += String(index) + " ";
        }
        
        message += "has changed to: " + parameter->GetFormatedValue(index) + "\n";

        helper->Log(message.c_str());
    }
}

void Config::Change(MP_ID mp, int amount, uint index)
{
    #ifdef MPM_AS_ARRAY
    mpm[(uint)mp]
    #else
    mpm->at(mp)
    #endif
    ->Change(amount, index);
    SetParameterChanged(mp, index);
}

void Config::Toggle(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    mpm[(uint)mp]
    #else
    mpm->at(mp)
    #endif
    ->Toggle(index);
    SetParameterChanged(mp, index);
}

void Config::Refresh(MP_ID mp, uint index)
{
    SetParameterChanged(mp, index);
}

void Config::Reset(MP_ID mp, uint index)
{
    #ifdef MPM_AS_ARRAY
    mpm[(uint)mp]
    #else
    mpm->at(mp)
    #endif
    ->Reset(index);
    SetParameterChanged(mp, index);
}