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

Mixerparameter* Config::DefParameter(MP_ID parameter_id, MP_CAT category, String name, uint count) {
	
	// create it
	Mixerparameter* newMpd = new Mixerparameter(parameter_id, category, name, count);

	// store in mixerparameter map (mpm)
    mpm[(uint)parameter_id] = newMpd;

	// return for further definition
	return newMpd;
}

void Config::DefineMixerparameters() {

	using enum MP_ID;

    // #####################
    // # Special Parameters
    // #####################

    DefParameter(NONE, MP_CAT::NONE, "");
    DefParameter(PAGE_CUSTOM_ENCODER, MP_CAT::NONE, "");

    // ############
    // # Settings
    // ###########

	MP_CAT cat = MP_CAT::SETTING;
    String group = "setting";

    DefParameter(DEBUG, cat, "DEBUG")
    ->DefConfig(group, "debug")
    ->DefStandard_Bool(true);

	DefParameter(LCD_CONTRAST, cat, "LCD Contrast")
    ->DefNameShort("Contrast")
	->DefMinMaxStandard_Uint(LCD_CONTRAST_MIN, LCD_CONTRAST_MAX, LCD_CONTRAST_DEFAULT)
    ->DefConfig(group, "lcd_contrast");
    
    DefParameter(LED_BRIGHTNESS, cat, "LED Brightness")
	->DefMinMaxStandard_Uint(LED_BRIGHTNESS_1, LED_BRIGHTNESS_4, LED_BRIGHTNESS_4)
    ->DefStepsize(64)
    ->DefConfig(group, "led_brightness");

    // TODO: DISPLAY_BRIGHTNESS

    DefParameter(SAMPLERATE, cat, "Samplerate")
    ->DefMinMaxStandard_Uint(44100, 48000, 48000)
    ->DefConfig(group, "samplerate");

    DefParameter(CARD_NUMBER_OF_CHANNELS, cat, "Channels")
    ->DefMinMaxStandard_Uint(0, 5, 0)
    ->DefUOM(MP_UOM::CARD_NUMBER_OF_CHANNELS)
    ->DefConfig(group, "card_channels");

    DefParameter(CHANNEL_LCD_MODE, cat, "LCD Mode")
    ->DefUOM(MP_UOM::CHANNEL_LCD_MODE)
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 1, 0)
    ->DefConfig(group, "lcd_mode");
    

    // ##########
    // # Routing 
    // ##########

    cat = MP_CAT::ROUTING;
    group = "routing";

    DefParameter(ROUTING_FPGA, cat, "Routing FPGA", 208)  // 208 -> size of fpga routing struct! (prepared for both AES50-ports, but only 160 are used at the moment)
    ->DefUOM(MP_UOM::FPGA_ROUTING)
    ->DefHideEncoderSlider()
    ->DefConfig(group, "fpga")
    ->DefMinMaxStandard_Uint(0, NUM_INPUT_CHANNEL, 0); // 161 = OFF + 160 channels

    DefParameter(ROUTING_DSP_INPUT, cat, "Input", MAX_FPGA_TO_DSP1_CHANNELS) // 40 routable DSP-input channels from FPGA
    ->DefUOM(MP_UOM::DSP_ROUTING)
    ->DefHideEncoderSlider()
    ->DefConfig(group, "dsp_input")
    ->DefMinMaxStandard_Uint(0, DSP_MAX_INTERNAL_CHANNELS - 1, 0); // 0=OFF, 92=Talkback

    DefParameter(ROUTING_DSP_INPUT_TAPPOINT, cat, "Tappoint", MAX_FPGA_TO_DSP1_CHANNELS) // 40 routable DSP-input channels from FPGA
    ->DefUOM(MP_UOM::TAPPOINT)
    ->DefConfig(group, "dsp_input_tappoint")
    ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::INPUT);

    DefParameter(ROUTING_DSP_OUTPUT, cat, "Output", (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS)) // 40 routable DSP-output-channels to FPGA and 24 routable DSP-output-channels to DSP2
    ->DefUOM(MP_UOM::DSP_ROUTING)
    ->DefHideEncoderSlider()
    ->DefConfig(group, "dsp_output")
    ->DefMinMaxStandard_Uint(0, DSP_MAX_INTERNAL_CHANNELS - 1, 0); // 0=OFF, 92=Talkback

    DefParameter(ROUTING_DSP_OUTPUT_TAPPOINT, cat, "Tappoint", (MAX_DSP1_TO_FPGA_CHANNELS + MAX_DSP1_TO_DSP2_CHANNELS)) // 40 routable DSP-output-channels to FPGA and 24 routable DSP-output-channels to DSP2
    ->DefUOM(MP_UOM::TAPPOINT)
    ->DefConfig(group, "dsp_output_tappoint")
    ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::POST_FADER);

    // ########
    // # State
    // ########

    cat = MP_CAT::STATE;
    group = "state";

    DefParameter(ACTIVE_SCENE, cat, "Active Scene")
    ->DefConfig(group, "active_scene")
    ->DefMinMaxStandard_Uint(0, 99, 0);

    DefParameter(SELECTED_CHANNEL, cat, "Selected Channel")
    ->DefUOM(MP_UOM::ZERO_BASED_INDEX__START_BY_ONE)
    ->DefHideEncoderReset()
    ->DefConfig(group, "selected_channel")
    ->DefMinMaxStandard_Uint(0, MAX_VCHANNELS - 1, 0);

    DefParameter(ACTIVE_PAGE, cat, "Active Page")
    ->DefHideEncoderSlider()
    ->DefConfig(group, "active_page")
    ->DefMinMaxStandard_Uint(0, 255, (uint)X32_PAGE::HOME);

    DefParameter(BANKING_EQ, cat, "Banking EQ")
    ->DefHideEncoderReset()
    ->DefConfig(group, "banking_eq")
    ->DefMinMaxStandard_Uint(0, 3, 0);

    DefParameter(BANKING_INPUT, cat, "Banking Input Section")
    ->DefHideEncoderReset()
    ->DefConfig(group, "banking_input")
    ->DefMinMaxStandard_Uint(0, 7, 0);

    DefParameter(BANKING_BUS, cat, "Banking Bus Section")
    ->DefHideEncoderReset()
    ->DefConfig(group, "banking_bus")
    ->DefMinMaxStandard_Uint(0, 3, 0);

    DefParameter(BANKING_BUS_SENDS, cat, "Banking Bus Sends")
    ->DefHideEncoderReset()
    ->DefConfig(group, "banking_bus_sends")
    ->DefMinMaxStandard_Uint(0, 3, 0);


    // ###########
    // # Global
    // ###########

	cat = MP_CAT::GLOBAL;
    group = "global";

    DefParameter(MONITOR_VOLUME, cat, "Monitor Volume")
    ->DefNameShort("Mon")
    ->DefConfig(group, "monitor_volume")
    ->DefUOM(MP_UOM::DB)
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);

    DefParameter(MONITOR_TAPPOINT, cat, "Monitor Tappoint")
    ->DefConfig(group, "monitor_tappoint")
    ->DefMinMaxStandard_Uint(0, 81, 0);

    // ###########
    // # Channels
    // ###########

	cat = MP_CAT::CHANNEL;
    group = "channel";

    DefParameter(CHANNEL_NAME_INTERN, cat, "Channelname Intern", MAX_VCHANNELS)->DefNameShort("Internalname")
    ->DefStandard_String("CH")
    ->DefReadonly();
    
    DefParameter(CHANNEL_NAME, cat, "Channelname", MAX_VCHANNELS)
    ->DefStandard_String("Kanal")
    ->DefConfig(group, "name");

    DefParameter(CHANNEL_COLOR, cat, "Channelcolor", MAX_VCHANNELS)
    ->DefMinMaxStandard_Uint((uint)X32_COLOR::BLACK, (uint)X32_COLOR::WHITE, (uint)X32_COLOR::YELLOW)
    ->DefConfig(group, "color");

    DefParameter(CHANNEL_COLOR_INVERTED, cat, "Color Inverted", MAX_VCHANNELS)
    ->DefStandard_Bool(false)
    ->DefConfig(group, "color_inverted");
    
    DefParameter(CHANNEL_PHASE_INVERT, cat, "Phase Inverted", MAX_VCHANNELS)->DefNameShort("Inverted")
    ->DefStandard_Bool(false)
    ->DefConfig(group, "phase_inverted");
    
    DefParameter(CHANNEL_PHANTOM, cat, "Phantom", MAX_VCHANNELS)->DefNameShort("48V")
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_GAIN, cat, "Gain", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "gain")
    ->DefMinMaxStandard_Float(CHANNEL_GAIN_MIN, CHANNEL_GAIN_MAX, 0.0f, 1);
    
    DefParameter(CHANNEL_VOLUME, cat, "Volume", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "volume")
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);

    DefParameter(CHANNEL_VOLUME_SUB, cat, "Sub", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "volume_sub")
    ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);
    
    DefParameter(CHANNEL_SOLO, cat, "Solo", MAX_VCHANNELS)
    ->DefConfig(group, "solo")
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_MUTE, cat, "Mute", MAX_VCHANNELS)
    ->DefConfig(group, "mute")
    ->DefStandard_Bool(false);
    
    DefParameter(CHANNEL_PANORAMA, cat, "Pan/Bal", MAX_VCHANNELS)
    ->DefConfig(group, "panorama")
    ->DefMinMaxStandard_Float(CHANNEL_PANORAMA_MIN, CHANNEL_PANORAMA_MAX, 0.0f)
    ->DefStepsize(2);

    DefParameter(CHANNEL_SEND_LR, cat, "LR", MAX_VCHANNELS)
    ->DefConfig(group, "send_lr")
    ->DefStandard_Bool(true);

    DefParameter(CHANNEL_SEND_SUB, cat, "SUB", MAX_VCHANNELS)
    ->DefConfig(group, "send_sub")
    ->DefStandard_Bool(false);

    // Sends
    cat = MP_CAT::CHANNEL_SENDS;

    vector<MP_ID> channel_send_busses = {
        CHANNEL_BUS_SEND01, CHANNEL_BUS_SEND02, CHANNEL_BUS_SEND03, CHANNEL_BUS_SEND04, CHANNEL_BUS_SEND05, CHANNEL_BUS_SEND06,
        CHANNEL_BUS_SEND07, CHANNEL_BUS_SEND08, CHANNEL_BUS_SEND09, CHANNEL_BUS_SEND10, CHANNEL_BUS_SEND11, CHANNEL_BUS_SEND12,
        CHANNEL_BUS_SEND13, CHANNEL_BUS_SEND14, CHANNEL_BUS_SEND15, CHANNEL_BUS_SEND16
    };

    for (uint i = 0; i < channel_send_busses.size(); i++)
    {
        DefParameter(channel_send_busses.at(i), cat, String("Send ") + (i + 1), MAX_VCHANNELS)
        ->DefUOM(MP_UOM::DB)
        ->DefConfig(group, "bus_send" + String(i))
        ->DefMinMaxStandard_Float(CHANNEL_VOLUME_MIN, CHANNEL_VOLUME_MAX, CHANNEL_VOLUME_MIN, 1);
    }

    vector<MP_ID> channel_send_busses_tappoints = {
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
        CHANNEL_BUS_SEND16_TAPPOINT};
    
    for (uint i = 0; i < channel_send_busses_tappoints.size(); i++)
    {
        DefParameter(channel_send_busses_tappoints.at(i), cat, String("Send ")  + (i + 1) + " Tap", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::TAPPOINT)
        ->DefConfig(group, "bus_send_tappoint" + String(i))
        ->DefMinMaxStandard_Uint(0, 4, (uint)DSP_TAP::INPUT);
    }    

    // gate
    cat = MP_CAT::CHANNEL_GATE;
   
    DefParameter(CHANNEL_GATE_TRESHOLD, cat, "Threshold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "gate_threshold")
    ->DefMinMaxStandard_Float(GATE_THRESHOLD_MIN, GATE_THRESHOLD_MAX, GATE_THRESHOLD_MIN, 0);
    
    DefParameter(CHANNEL_GATE_RANGE, cat, "Range", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "gate_range")
    ->DefMinMaxStandard_Float(GATE_RANGE_MIN, GATE_RANGE_MAX, GATE_RANGE_MAX, 1);
    
    DefParameter(CHANNEL_GATE_ATTACK, cat, "Attack", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "gate_attack")
    ->DefMinMaxStandard_Float(GATE_ATTACK_MIN, GATE_ATTACK_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_GATE_HOLD, cat, "Hold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "gate_hold")
    ->DefMinMaxStandard_Float(GATE_HOLD_MIN, GATE_HOLD_MAX, 50.0f, 0);
    
    DefParameter(CHANNEL_GATE_RELEASE, cat, "Release", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "gate_release")
    ->DefMinMaxStandard_Float(GATE_RELEASE_MIN, GATE_RELEASE_MAX, 250.0f, 0);

    // dynamics
    cat = MP_CAT::CHANNEL_DYNAMICS;

    DefParameter(CHANNEL_DYNAMICS_TRESHOLD, cat, "Threshold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "dynamics_threshold")
    ->DefMinMaxStandard_Float(DYNAMICS_THRESHOLD_MIN, DYNAMICS_THRESHOLD_MAX, DYNAMICS_THRESHOLD_MAX, 0);
    
    DefParameter(CHANNEL_DYNAMICS_RATIO, cat, "Ratio", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "dynamics_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(DYNAMICS_RATIO_MIN, DYNAMICS_RATIO_MAX, 3, 1);
    
    DefParameter(CHANNEL_DYNAMICS_MAKEUP, cat, "Makeup", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "dynamics_makeup")
    ->DefMinMaxStandard_Float(DYNAMICS_MAKEUP_MIN, DYNAMICS_MAKEUP_MAX, DYNAMICS_MAKEUP_MIN, 1);
    
    DefParameter(CHANNEL_DYNAMICS_ATTACK, cat, "Attack", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "dynamics_attack")
    ->DefMinMaxStandard_Float(DYNAMICS_ATTACK_MIN, DYNAMICS_ATTACK_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_DYNAMICS_HOLD, cat, "Hold", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "dynamics_hold")
    ->DefMinMaxStandard_Float(DYNAMICS_HOLD_MIN, DYNAMICS_HOLD_MAX, 10.0f, 0);
    
    DefParameter(CHANNEL_DYNAMICS_RELEASE, cat, "Release", MAX_VCHANNELS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "dynamics_release")
    ->DefMinMaxStandard_Float(DYNAMICS_RELEASE_MIN, DYNAMICS_RELEASE_MAX, 150.0f, 0);

    // EQ
    cat = MP_CAT::CHANNEL_EQ;

    DefParameter(CHANNEL_LOWCUT_FREQ, cat, "LowcutFreq", MAX_VCHANNELS)
    ->DefConfig(group, "lowcut_freq")
    ->DefUOM(MP_UOM::HZ)
    ->DefStepmode(1) // frequency mode
    ->DefStepsize(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 20.0f);

    uint channel_eq_count = 4;
    float channel_eq_freq[4] = {125.0f, 500.0f, 2000.0f, 10000.0f};
    for (uint i = 0; i < channel_eq_count; i++)
    {
        DefParameter((MP_ID)((uint)CHANNEL_EQ_TYPE1 + i), cat, "Type", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::EQ_TYPE)
        ->DefConfig(group, "eq_type_" + String(i))
        ->DefHideEncoderReset()
        ->DefMinMaxStandard_Uint(0, 7, 1);

        DefParameter((MP_ID)((uint)CHANNEL_EQ_FREQ1 + i), cat, "Frequenz", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::HZ)
        ->DefConfig(group, "eq_freq_" + String(i))
        ->DefStepmode(1) // frequency mode
        ->DefMinMaxStandard_Float(20.0f, 20000.0f, channel_eq_freq[i]);

        DefParameter((MP_ID)((uint)CHANNEL_EQ_GAIN1 + i), cat, "Gain", MAX_VCHANNELS)
        ->DefUOM(MP_UOM::DB)
        ->DefConfig(group, "eq_gain_" + String(i))
        ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);
 
        DefParameter((MP_ID)((uint)CHANNEL_EQ_Q1 + i), cat, "Q", MAX_VCHANNELS)
        ->DefStepsize(0.1f)
        ->DefConfig(group, "eq_q_" + String(i))
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
    DefParameter(FX_REVERB_ROOMSIZE, cat, "Roomsize", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "roomsize")
    ->DefMinMaxStandard_Float(FX_REVERB_ROOMSIZE_MIN, FX_REVERB_ROOMSIZE_MAX, FX_REVERB_ROOMSIZE_DEFAULT, 0);
    // reverb rt60
    DefParameter(FX_REVERB_RT60, cat, "RT60", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::SECONDS)
    ->DefConfig(group, "rt60")
    ->DefMinMaxStandard_Float(FX_REVERB_RT60_MIN, FX_REVERB_RT60_MAX, FX_REVERB_RT60_DEFAULT, 1);
    // reverb lowpass
    DefParameter(FX_REVERB_LPFREQ, cat, "LowPassFreq", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "lowpassfreq")
    ->DefStepmode(1) // frequency mode
    ->DefMinMaxStandard_Float(FX_REVERB_LPFREQ_MIN, FX_REVERB_LPFREQ_MAX, FX_REVERB_LPFREQ_DEFAULT, 0);
    // reverb dry
    DefParameter(FX_REVERB_DRY, cat, "Dry", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::PERCENT)
    ->DefConfig(group, "dry")
    ->DefMinMaxStandard_Float(FX_REVERB_DRY_MIN, FX_REVERB_DRY_MAX, FX_REVERB_DRY_DEFAULT);
    // reverb wet
    DefParameter(FX_REVERB_WET, cat, "Wet", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::PERCENT)
    ->DefConfig(group, "wet")
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
    #define FX_CHORUS_FREQ_A_MIN          0.05f //
    #define FX_CHORUS_FREQ_A_DEFAULT       1.5f //
    #define FX_CHORUS_FREQ_A_MAX          10.0f //
    #define FX_CHORUS_FREQ_B_MIN          0.05f //
    #define FX_CHORUS_FREQ_B_DEFAULT       1.6f //
    #define FX_CHORUS_FREQ_B_MAX          10.0f //

    // chorus depth
    DefParameter(FX_CHORUS_DEPTH_A, cat, "Depth A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "depth_a")
    ->DefMinMaxStandard_Float(FX_CHORUS_DEPTH_A_MIN, FX_CHORUS_DEPTH_A_MAX, FX_CHORUS_DEPTH_A_DEFAULT);

    DefParameter(FX_CHORUS_DEPTH_B, cat, "Depth B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "depth_b")
    ->DefMinMaxStandard_Float(FX_CHORUS_DEPTH_B_MIN, FX_CHORUS_DEPTH_B_MAX, FX_CHORUS_DEPTH_B_DEFAULT);

    // chorus delay
    DefParameter(FX_CHORUS_DELAY_A, cat, "Delay A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "delay_a")
    ->DefMinMaxStandard_Float(FX_CHORUS_DELAY_A_MIN, FX_CHORUS_DELAY_A_MAX, FX_CHORUS_DELAY_A_DEFAULT);

    DefParameter(FX_CHORUS_DELAY_B, cat, "Delay B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "delay_b")
    ->DefMinMaxStandard_Float(FX_CHORUS_DELAY_B_MIN, FX_CHORUS_DELAY_B_MAX, FX_CHORUS_DELAY_B_DEFAULT);

    // chorus phase
    DefParameter(FX_CHORUS_PHASE_A, cat, "Phase A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "phase_a")
    ->DefMinMaxStandard_Float(FX_CHORUS_PHASE_A_MIN, FX_CHORUS_PHASE_A_MAX, FX_CHORUS_PHASE_A_DEFAULT);

    DefParameter(FX_CHORUS_PHASE_B, cat, "Phase B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "phase_b")
    ->DefMinMaxStandard_Float(FX_CHORUS_PHASE_B_MIN, FX_CHORUS_PHASE_B_MAX, FX_CHORUS_PHASE_B_DEFAULT);
    
    // chorus freq
    DefParameter(FX_CHORUS_FREQ_A, cat, "Freq A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "freq_a")
    ->DefMinMaxStandard_Float(FX_CHORUS_FREQ_A_MIN, FX_CHORUS_FREQ_A_MAX, FX_CHORUS_FREQ_A_DEFAULT, 2)
    ->DefStepmode(1);
    
    DefParameter(FX_CHORUS_FREQ_B, cat, "Freq B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "freq_b")
    ->DefMinMaxStandard_Float(FX_CHORUS_FREQ_B_MIN, FX_CHORUS_FREQ_B_MAX, FX_CHORUS_FREQ_B_DEFAULT, 2)
    ->DefStepmode(1);

    // chorus mix
    DefParameter(FX_CHORUS_MIX, cat, "Mix", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::PERCENT)
    ->DefConfig(group, "mix")
    ->DefMinMaxStandard_Float(0.0f, 1.0f, 0.5f);

    // transientshaper
    group = "fx_transientshaper";

    // fast
    DefParameter(FX_TRANSIENTSHAPER_FAST, cat, "Fast", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "fast")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);

    // medium
    DefParameter(FX_TRANSIENTSHAPER_MEDIUM, cat, "Medium", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "mediun")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 15.0f);

    // slow
    DefParameter(FX_TRANSIENTSHAPER_SLOW, cat, "Slow", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "slow")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 150.0f);

    // attack
    DefParameter(FX_TRANSIENTSHAPER_ATTACK, cat, "Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "attack")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);

    // sustain
    DefParameter(FX_TRANSIENTSHAPER_SUSTAIN, cat, "Sustain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "sustain")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);

    // delay
    DefParameter(FX_TRANSIENTSHAPER_DELAY, cat, "Delay", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "delay")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 1.0f);

    // FX_TYPE_OVERDRIVE
    group = "fx_overdrive";

    DefParameter(FX_OVERDRIVE_PREGAIN, cat, "PreGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "pregain")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 10.0f);

    DefParameter(FX_OVERDRIVE_Q, cat, "Q", MAX_FX_SLOTS)
    ->DefStepsize(0.1f)
    ->DefConfig(group, "q")
    ->DefMinMaxStandard_Float(-10.0f, 10.0f, -0.2f, 1);

    DefParameter(FX_OVERDRIVE_HPF_INPUTFREQ, cat, "LC In", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "lc_in")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 300.0f);

    DefParameter(FX_OVERDRIVE_LPF_INPUTFREQ, cat, "HC In", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "hc_in")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 10000.0f);

    DefParameter(FX_OVERDRIVE_LPF_OUTPUTFREQ, cat, "HC Out", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "hc_out")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 10000.0f);

    // delay A/B
    group = "fx_delay";

    DefParameter(FX_DELAY_DELAY_A, cat, "Delay A", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "delay_a")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 350.0f);

    DefParameter(FX_DELAY_DELAY_B, cat, "Delay B", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "delay_b")
    ->DefMinMaxStandard_Float(0.0f, 1000.0f, 450.0f);

    // FX_TYPE_MULTIBANDCOMPRESOR   channel band threshold ratio attack  hold   release   makeup
    group = "fx_multibandcompressor";

    DefParameter(FX_MULTIBANDCOMPRESOR_L_FREQ1, cat, "[1L] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "1l_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 80.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_FREQ2, cat, "[2L] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "2l_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 350.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_FREQ3, cat, "[3L] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "3l_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 1500.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_FREQ4, cat, "[4L] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "4l_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 7500.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_THRESHOLD, cat, "[1L] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1l_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_RATIO, cat, "[1L] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "1l_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_ATTACK, cat, "[1L] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1l_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_HOLD, cat, "[1L] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1l_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_RELEASE, cat, "[1L] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1l_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND1_MAKEUP, cat, "[1L] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1l_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_THRESHOLD, cat, "[2L] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2l_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_RATIO, cat, "[2L] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "2l_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_ATTACK, cat, "[2L] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2l_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_HOLD, cat, "[2L] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2l_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_RELEASE, cat, "[2L] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2l_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND2_MAKEUP, cat, "[2L] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2l_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_THRESHOLD, cat, "[3L] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3l_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_RATIO, cat, "[3L] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "3l_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_ATTACK, cat, "[3L] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3l_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_HOLD, cat, "[3L] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3l_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_RELEASE, cat, "[3L] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3l_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND3_MAKEUP, cat, "[3L] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3l_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_THRESHOLD, cat, "[4L] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "4l_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_RATIO, cat, "[4L] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "4l_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_ATTACK, cat, "[4L] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4l_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_HOLD, cat, "[4L] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4l_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_RELEASE, cat, "[4L] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4l_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND4_MAKEUP, cat, "[4L] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "4l_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_THRESHOLD, cat, "[5L] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "5l_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_RATIO, cat, "[5L] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "5l_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_ATTACK, cat, "[5L] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5l_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_HOLD, cat, "[5L] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5l_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_RELEASE, cat, "[5L] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5l_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_L_BAND5_MAKEUP, cat, "[5L] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "5l_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);


    DefParameter(FX_MULTIBANDCOMPRESOR_R_FREQ1, cat, "[1R] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "1r_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 80.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_FREQ2, cat, "[2R] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "2r_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 350.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_FREQ3, cat, "[3R] Frequency ", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "3r_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 1500.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_FREQ4, cat, "[4R] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "4r_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 7500.0f);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_THRESHOLD, cat, "[1R] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1r_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_RATIO, cat, "[1R] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "1r_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_ATTACK, cat, "[1R] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1r_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_HOLD, cat, "[1R] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1r_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_RELEASE, cat, "[1R] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1r_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND1_MAKEUP, cat, "[1R] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1r_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_THRESHOLD, cat, "[2R] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2r_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_RATIO, cat, "[2R] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "2r_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_ATTACK, cat, "[2R] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2r_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_HOLD, cat, "[2R] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2r_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_RELEASE, cat, "[2R] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2r_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND2_MAKEUP, cat, "[2R] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2r_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_THRESHOLD, cat, "[3R] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3r_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_RATIO, cat, "[3R] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "3r_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_ATTACK, cat, "[3R] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3r_ratio")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_HOLD, cat, "[3R] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3r_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_RELEASE, cat, "[3R] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3r_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND3_MAKEUP, cat, "[3R] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3r_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_THRESHOLD, cat, "[4R] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "4r_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_RATIO, cat, "[4R] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "4r_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_ATTACK, cat, "[4R] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4r_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_HOLD, cat, "[4R] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4r_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_RELEASE, cat, "[4R] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "4r_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND4_MAKEUP, cat, "[4R] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "4r_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_THRESHOLD, cat, "[5R] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "5r_threshold")

    ->DefMinMaxStandard_Float(-15.0f, 15.0f, -5.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_RATIO, cat, "[5R] Ratio", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::NONE)
    ->DefConfig(group, "5r_ratio")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 1.5f, 1);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_ATTACK, cat, "[5R] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5r_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 10.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_HOLD, cat, "[5R] Hold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5r_hold")
    ->DefMinMaxStandard_Float(0.2f, 2000.0f, 100.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_RELEASE, cat, "[5R] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "5r_release")
    ->DefMinMaxStandard_Float(0.0f, 4000.0f, 40.0f, 0);

    DefParameter(FX_MULTIBANDCOMPRESOR_R_BAND5_MAKEUP, cat, "[5R] Makeup", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "5r_makeup")
    ->DefMinMaxStandard_Float(0.0f, 24.0f, 0.0f, 1);

    // FX_TYPE_DYNAMICEQ            band type freq staticGain  maxDynGain  Q  thresh  ratio  attack  release
    group = "fx_dynamiceq";

    DefParameter(FX_DYNAMICEQ_BAND1_TYPE, cat, "[1] Type", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::EQ_TYPE)
    ->DefConfig(group, "1_type")
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 7, 1);

    DefParameter(FX_DYNAMICEQ_BAND1_FREQ, cat, "[1] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "1_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 300.0f);

    DefParameter(FX_DYNAMICEQ_BAND1_STATICGAIN, cat, "[1] StaticGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1_staticgain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);
    
    DefParameter(FX_DYNAMICEQ_BAND1_MAXDYNGAIN, cat, "[1] DynamicGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1_dyngain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND1_Q, cat, "[1] Q", MAX_FX_SLOTS)
    ->DefStepsize(0.1f)
    ->DefConfig(group, "1_q")
    ->DefMinMaxStandard_Float(0.3f, 10.0f, 2.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND1_THRESHOLD, cat, "[1] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "1_threshold")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 0);

    DefParameter(FX_DYNAMICEQ_BAND1_RATIO, cat, "[1] Ratio", MAX_FX_SLOTS)
    ->DefStepmode(1)
    ->DefConfig(group, "1_ratio")
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 3, 1);

    DefParameter(FX_DYNAMICEQ_BAND1_ATTACK, cat, "[1] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 50.0f, 0);

    DefParameter(FX_DYNAMICEQ_BAND1_RELEASE, cat, "[1] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "1_release")
    ->DefMinMaxStandard_Float(5.0f, 4000.0f, 300.0f);

    
    DefParameter(FX_DYNAMICEQ_BAND2_TYPE, cat, "[2] Type", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::EQ_TYPE)
    ->DefConfig(group, "2_type")
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 7, 1);

    DefParameter(FX_DYNAMICEQ_BAND2_FREQ, cat, "[2] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "2_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 1000.0f);

    DefParameter(FX_DYNAMICEQ_BAND2_STATICGAIN, cat, "[2] StaticGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2_staticgain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND2_MAXDYNGAIN, cat, "[2] DynamicGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2_dyngain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND2_Q, cat, "[2] Q", MAX_FX_SLOTS)
    ->DefStepsize(0.1f)
    ->DefConfig(group, "2_q")
    ->DefMinMaxStandard_Float(0.3f, 10.0f, 2.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND2_THRESHOLD, cat, "[2] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "2_threshold")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 0);

    DefParameter(FX_DYNAMICEQ_BAND2_RATIO, cat, "[2] Ratio", MAX_FX_SLOTS)
    ->DefStepmode(1)
    ->DefConfig(group, "2_ratio")
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 3, 1);

    DefParameter(FX_DYNAMICEQ_BAND2_ATTACK, cat, "[2] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 50.0f);

    DefParameter(FX_DYNAMICEQ_BAND2_RELEASE, cat, "[2] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "2_release")
    ->DefMinMaxStandard_Float(5.0f, 4000.0f, 300.0f);


    DefParameter(FX_DYNAMICEQ_BAND3_TYPE, cat, "[3] Type", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::EQ_TYPE)
    ->DefConfig(group, "3_type")
    ->DefHideEncoderReset()
    ->DefMinMaxStandard_Uint(0, 7, 1);

    DefParameter(FX_DYNAMICEQ_BAND3_FREQ, cat, "[3] Frequency", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::HZ)
    ->DefConfig(group, "3_freq")
    ->DefStepmode(1)
    ->DefMinMaxStandard_Float(20.0f, 24000.0f, 5000.0f);

    DefParameter(FX_DYNAMICEQ_BAND3_STATICGAIN, cat, "[3] StaticGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3_staticgain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND3_MAXDYNGAIN, cat, "[3] DynamicGain", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3_dyngain")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND3_Q, cat, "[3] Q", MAX_FX_SLOTS)
    ->DefStepsize(0.1f)
    ->DefConfig(group, "3_q")
    ->DefMinMaxStandard_Float(0.3f, 10.0f, 2.0f, 1);

    DefParameter(FX_DYNAMICEQ_BAND3_THRESHOLD, cat, "[3] Threshold", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::DB)
    ->DefConfig(group, "3_threshold")
    ->DefMinMaxStandard_Float(-15.0f, 15.0f, 0.0f, 0);

    DefParameter(FX_DYNAMICEQ_BAND3_RATIO, cat, "[3] Ratio", MAX_FX_SLOTS)
    ->DefStepmode(1)
    ->DefConfig(group, "3_ratio")
    ->DefMinMaxStandard_Float(1.1f, 100.0f, 3, 1);

    DefParameter(FX_DYNAMICEQ_BAND3_ATTACK, cat, "[3] Attack", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3_attack")
    ->DefMinMaxStandard_Float(0.0f, 120.0f, 50.0f);

    DefParameter(FX_DYNAMICEQ_BAND3_RELEASE, cat, "[3] Release", MAX_FX_SLOTS)
    ->DefUOM(MP_UOM::MS)
    ->DefConfig(group, "3_release")
    ->DefMinMaxStandard_Float(5.0f, 4000.0f, 300.0f);


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

Mixerparameter** Config::GetParameterList()
{
    return mpm;
}

map<MP_ID, set<uint>>* Config::GetChangedParameterList()
{
    return mp_changedlist;
}

vector<uint> Config::GetChangedParameterIndexes(MP_CAT parameter_cat)
{
    vector<uint> changedIndexes;

    for (auto const& [parameter_id, indexSet] : *mp_changedlist)
    {
        if (mpm[(uint)parameter_id]->GetCategory() == parameter_cat)
        {
            for (auto const& index : indexSet)
            {
                if (std::find(changedIndexes.begin(), changedIndexes.end(), index) == changedIndexes.end())
                {
                    changedIndexes.push_back(index);
                } 
            }
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
            for (auto const& index : indexSet)
            {
                if (find(changedIndexes.begin(), changedIndexes.end(), index) == changedIndexes.end())
                {
                    changedIndexes.push_back(index);
                } 
            }
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
        if (mpm[(uint)parameter_id]->GetCategory() == parameter_cat)
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
        if (mpm[(uint)parameter_id]->GetCategory() == parameter_cat && indexSet.contains(index))
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
    return mpm[(uint)mp];
}

float Config::GetFloat(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetFloat(index);
}

int Config::GetInt(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetInt(index);
}

uint Config::GetUint(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetUint(index);
}

bool Config::GetBool(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetBool(index);
}

String Config::GetString(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetString(index);
}

uint Config::GetPercent(MP_ID mp, uint index)
{
    return mpm[(uint)mp]->GetPercent(index);
}

void Config::Set(MP_ID mp, float value, uint index)
{
    mpm[(uint)mp]->Set(value, index);

    SetParameterChanged(mp, index);
}

void Config::Set(MP_ID mp, String value_string, uint index)
{
    mpm[(uint)mp]->Set(value_string, index);

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
    mpm[(uint)mp]->Change(amount, index);

    SetParameterChanged(mp, index);
}

void Config::Toggle(MP_ID mp, uint index)
{
    mpm[(uint)mp]->Toggle(index);

    SetParameterChanged(mp, index);
}

void Config::Refresh(MP_ID mp, uint index)
{
    SetParameterChanged(mp, index);
}

void Config::Reset(MP_ID mp, uint index)
{
    mpm[(uint)mp]->Reset(index);
    
    SetParameterChanged(mp, index);
}