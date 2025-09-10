#include "mixer.h"
#include "demo.h"
#include "surface.h"

s_Mixer mixer;

// ####################################################################
// #
// #
// #        Init
// #
// #
// ####################################################################

void mixerInit(char model[]) {
    if (strcmp(model, "X32Core") == 0) {
        mixer.model = X32_MODEL_CORE;
    }else if (strcmp(model, "X32RACK") == 0) {
        mixer.model = X32_MODEL_RACK;
    }else if (strcmp(model, "X32Producer") == 0) {
        mixer.model = X32_MODEL_PRODUCER;
    }else if (strcmp(model, "X32C") == 0) {
        mixer.model =  X32_MODEL_COMPACT;
    }else if (strcmp(model, "X32") == 0) {
        mixer.model = X32_MODEL_FULL;
    }else{
        x32log("ERROR: No model detected!\n");
        mixer.model = X32_MODEL_NONE;
    }

    x32debug("############# InitMixer(model_index=%d) #############\n", mixer.model);
    mixerInitPages();
    initDefinitions();

    // disable all vChannels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        mixerSetVChannelDefaults(&mixer.vChannel[i], i, true);
    }

    // empty all banks
    for (uint8_t b = 0; b < 8; b++){
        for (uint8_t sCh = 0; sCh < 16; sCh++){
            mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
            mixer.modes[X32_SURFACE_MODE_BANKING_USER].inputBanks[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }
    for (uint8_t b = 0; b < 4; b++){
        for (uint8_t sCh = 0; sCh < 8; sCh++){
            mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
            mixer.modes[X32_SURFACE_MODE_BANKING_USER].busBanks[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }



    //##################################################################################
    //#
    //#   create default vChannels (what the user is refering to as "mixer channel")
    //#
    //#   0   -  31  traditional channels, xlr input 1-32 connected as source, xlr output 1-8 as destination on first 8 vChannels
    //#   32  -  39  AUX / USB
    //#   40  -  47  Effects Returns
    //#   48  -  55  Bus 1-8
    //#   56  -  63  Bis 9-16
    //#
    //#   64  -  71  Matrix 1-6 / Main LR / this one special mono channel für subwoofer
    //#   72  -  79  DCA
    //#
    //##################################################################################
    for (int i=0; i<=31; i++) {
        s_vChannel *vChannel = &(mixer.vChannel[i]);
        mixerSetVChannelDefaults(vChannel, i, false);
        sprintf(&(vChannel->name[0]), "Kanal %d", i+1); // german label :-)

        s_inputSource *input = &vChannel->inputSource;
        input->phantomPower = false;
        input->phaseInvert = false;
        input->dspChannel = i+1;

        // TODO: assign vChannel to "bus vChannel"
// #if DEBUG
//         // route input straight to output - just for testing
//         if (i<=7) {
//             s_outputDestination *output = &vChannel->outputDestination;
//             output->hardwareGroup = 'x';
//             output->hardwareChannel = i+1;       
//         }
// #endif
    }

// #if DEBUG
//     // set color on first 8 channels for testing
//     // set input to Off for channelindex 5
//     mixer.vChannel[0].color = SURFACE_COLOR_WHITE;
//     mixer.vChannel[0].volume = 0;
//     sprintf(mixer.vChannel[0].name, "White");
//     mixer.vChannel[1].color = SURFACE_COLOR_YELLOW;
//     mixer.vChannel[1].volume = 0;
//     sprintf(mixer.vChannel[1].name, "Yellow");
//     mixer.vChannel[2].color = SURFACE_COLOR_BLUE;
//     mixer.vChannel[2].volume = 0;
//     sprintf(mixer.vChannel[2].name, "Blue");
//     mixer.vChannel[3].color = SURFACE_COLOR_CYAN;
//     mixer.vChannel[3].volume = 0;
//     sprintf(mixer.vChannel[3].name, "Cyan has 30 characters - long.");
//     mixer.vChannel[4].color = SURFACE_COLOR_GREEN;
//     mixer.vChannel[4].volume = 0;
//     sprintf(mixer.vChannel[4].name, "Green");
//     mixer.vChannel[5].color = SURFACE_COLOR_PINK;
//     mixer.vChannel[5].volume = 0;
//     sprintf(mixer.vChannel[5].name, "Pink (Off)");
//     mixer.vChannel[6].color = SURFACE_COLOR_RED;
//     mixer.vChannel[6].volume = 0;
//     sprintf(mixer.vChannel[6].name, "Red");
//     mixer.vChannel[7].color = SURFACE_COLOR_BLACK;
//     mixer.vChannel[7].volume = 0;
//     sprintf(mixer.vChannel[7].name, "Black");

//     // set inverted color on next 8 channels for testing
//     mixer.vChannel[8].color = SURFACE_COLOR_WHITE | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[8].volume = -20;
//     sprintf(mixer.vChannel[8].name, "Inverted");
//     mixer.vChannel[9].color = SURFACE_COLOR_YELLOW | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[9].volume = -20;
//     sprintf(mixer.vChannel[9].name, "Inverted");
//     mixer.vChannel[10].color = SURFACE_COLOR_BLUE | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[10].volume = -20;
//     sprintf(mixer.vChannel[10].name, "Inverted");
//     mixer.vChannel[11].color = SURFACE_COLOR_CYAN | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[11].volume = -20;
//     sprintf(mixer.vChannel[11].name, "Inverted");
//     mixer.vChannel[12].color = SURFACE_COLOR_GREEN | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[12].volume = -20;
//     sprintf(mixer.vChannel[12].name, "Inverted");
//     mixer.vChannel[13].color = SURFACE_COLOR_PINK | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[13].volume = -20;
//     sprintf(mixer.vChannel[13].name, "Inverted");
//     mixer.vChannel[14].color = SURFACE_COLOR_RED | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[14].volume = -20;
//     sprintf(mixer.vChannel[14].name, "Inverted");
//     mixer.vChannel[15].color = SURFACE_COLOR_BLACK | SURFACE_COLOR_INVERTED;
//     mixer.vChannel[15].volume = -20;
//     sprintf(mixer.vChannel[15].name, "Inverted");
// #endif

    mixerDebugPrintvChannels();

    // AUX 1-6 / USB
    x32debug("Setting up AUX\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 32 + i;
        s_vChannel *vChannel = &mixer.vChannel[index];
        mixerSetVChannelDefaults(vChannel, index, false);
        s_inputSource *input = &vChannel->inputSource;
        input->phantomPower = false;
        input->phaseInvert = false;
        input->dspChannel = index + 1;

        if(i <=5){
            sprintf(&vChannel->name[0], "AUX%d", i+1);
            vChannel->color = SURFACE_COLOR_GREEN;
        } else {
            sprintf(&vChannel->name[0], "USB");
            vChannel->color = SURFACE_COLOR_YELLOW;
        }
    }

    // FX Returns 1-8
    x32debug("Setting up FX Returns\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 40 + i;
        s_vChannel *vChannel = &mixer.vChannel[index];
        mixerSetVChannelDefaults(vChannel, index, false);
        sprintf(&vChannel->name[0], "FX Ret%d", i+1);
        vChannel->color = SURFACE_COLOR_BLUE;
    }

    // Bus 1-16
    x32debug("Setting up Busses\n");
    for (uint8_t i=0; i<=15;i++){
        uint8_t index = 48 + i;
        s_vChannel *vChannel = &mixer.vChannel[index];
        mixerSetVChannelDefaults(vChannel, index, false);
        sprintf(&vChannel->name[0], "BUS%d", i+1);
        vChannel->color = SURFACE_COLOR_CYAN;
    }

    // Matrix 1-6 / Special / SUB
    x32debug("Setting up Matrix / SPECIAL / SUB\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 64 + i;
        s_vChannel *vChannel = &mixer.vChannel[index];
        mixerSetVChannelDefaults(vChannel, index, false);
        if(i <=5){
            sprintf(&vChannel->name[0], "MATRIX%d", i+1);
            vChannel->color = SURFACE_COLOR_PINK;
        } else if (i == 6){
            sprintf(&vChannel->name[0], "SPECIAL");
            vChannel->color = SURFACE_COLOR_RED;
        } else if (i == 7){
            sprintf(&vChannel->name[0], "M/C");
            vChannel->color = SURFACE_COLOR_WHITE;
        }
    }

    // DCA 1-8
    x32debug("Setting up DCA\n");
    for (uint8_t i=0; i<=7;i++){
        uint8_t index = 72 + i;
        s_vChannel *vChannel = &mixer.vChannel[index];
        mixerSetVChannelDefaults(vChannel, index, false);
        sprintf(&vChannel->name[0], "DCA%d", i+1);
        vChannel->color = SURFACE_COLOR_PINK;
    }


    // main-channel
    {
        s_vChannel *vChannel = &mixer.vChannel[80];
        mixerSetVChannelDefaults(vChannel, 80, false);
        sprintf(&vChannel->name[0], "Main");
        vChannel->color = SURFACE_COLOR_WHITE;
        vChannel->vChannelType = 1; // main channel
    }

    // vChannel0 selected by default
    mixerSetSelect(0, true);

    //##################################################################################
    //#
    //#   assign vChannels to input fader bank - X32 Default Layout
    //#
    //##################################################################################

    if (mixerIsModelX32Full()){
        // bank is 16 channels wide
        for (uint8_t bank=0;bank<4;bank++){
            for (int i = 0; i <=15; i++) {
                mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2vChannel[i] = i + (bank * 16);
                x32debug("Assing bank%d: surfaceChannel%d <-> vChannel%d\n", bank, i, i + (bank * 16));
            }
        }
    }

    if (mixerIsModelX32CompacrOrProducer()){
        // bank is 8 channels wide
        for (uint8_t bank=0;bank<8;bank++){
            for (int i = 0; i <=7; i++) {
                mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2vChannel[i] = i + (bank * 8);
                x32debug("Assing bank%d: surfaceChannel%d <-> vChannel%d\n", bank, i, i + (bank * 8));
            }
        }
    }

    //##################################################################################
    //#
    //#   assign vChannels to bus fader bank - X32 Default Layout
    //#
    //##################################################################################

    // DCA - starts at vChannel 72
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[0].surfaceChannel2vChannel[i] = i + 72;
    }

    // Bus 1-8 - starts at vChannel 48
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[1].surfaceChannel2vChannel[i] = i + 48;
    }

    // Bus 9-16 - starts at vChannel 56
    for (int i = 0; i <=7; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[2].surfaceChannel2vChannel[i] = i + 56;
    }

    // Matrix / SPECIAL / SUB - starts at vChannel 64
    for (int i = 0; i <=9; i++) {
        mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[3].surfaceChannel2vChannel[i] = i + 64;
    }

    mixer.activeMode = X32_SURFACE_MODE_BANKING_X32;
    mixer.activeBank_inputFader = 0;
    mixer.activeBank_busFader = 0;

    x32debug("END ############# InitMixer() ############# END \n\n");
}

void mixerSetVChannelDefaults(s_vChannel* p_vChannel, uint8_t p_vChannelIndex, bool p_disabled){
    p_vChannel->index = p_vChannelIndex;
    if (p_disabled){
        p_vChannel->color = 0;
    } else {
        //p_vChannel->color = SURFACE_COLOR_WHITE;
        p_vChannel->color = SURFACE_COLOR_YELLOW;
    }
    p_vChannel->icon = 0;
    p_vChannel->selected = false;
    p_vChannel->solo = false;
    p_vChannel->mute = false;
    p_vChannel->volume = VOLUME_MIN;
    p_vChannel->vChannelType = 0; // normal channel

    // disable all audio
    p_vChannel->inputSource.dspChannel = 0;
}

// ####################################################################
// #
// #
// #        Routing + Hardware channel assignment
// #
// #
// ###################################################################


// change the input of the selected vChannel
void mixerChangeVChannel(int8_t amount){
    // each vChannel is routed within the DSP and has access to either the 40 input-channels or one of the 16 busses or FX-returns

    s_vChannel *chan = mixerGetSelectedvChannel();
    x32debug("mixerChangeVChannel(): dspChannel=%d\n", chan->inputSource.dspChannel);

    int16_t newValue = (int16_t)chan->inputSource.dspChannel + amount;

    if (newValue > NUM_DSP_CHANNEL) {
        newValue -= NUM_DSP_CHANNEL;
    }else if (newValue < 0) {
        newValue += NUM_DSP_CHANNEL;
    }

    chan->inputSource.dspChannel = newValue;
    mixerSetVChannelChangeFlags(chan, X32_VCHANNEL_CHANGED_INPUT);
    mixerSetChangeFlags(X32_MIXER_CHANGED_ROUTING);
}

void mixerChangeHardwareOutput(int8_t amount) {
    // output-taps
    // 1-16 = XLR-outputs
    // 17-32 = UltraNet/P16-outputs
    // 33-64 = Card-outputs
    // 65-72 = AUX-outputs
    // 73-112 = DSP-inputs
    // 113-160 = AES50A-outputs
    // 161-208 = AES50B-outputs

    int16_t newValue = (int16_t)mixer.selectedOutputChannelIndex + amount;

    if (newValue > NUM_OUTPUT_CHANNEL) {
        newValue -= NUM_OUTPUT_CHANNEL;
    }else if (newValue < 1){
        newValue += NUM_OUTPUT_CHANNEL;
    }
    if (newValue == 0) {
        newValue = 1; // dont allow "OFF" for Output-channel
    }
    mixer.selectedOutputChannelIndex = newValue;
    // no sending to FPGA as we are not changing the hardware-routing here
    mixerSetChangeFlags(X32_MIXER_CHANGED_GUI);
}

void mixerChangeHardwareInput(int8_t amount) {
    // get current routingIndex
    int16_t newValue = routingGetOutputSourceByIndex(mixer.selectedOutputChannelIndex) + amount;

    if (newValue > NUM_INPUT_CHANNEL) {
        newValue -= NUM_INPUT_CHANNEL;
    }else if (newValue < 0){
        newValue += NUM_INPUT_CHANNEL;
    }
    routingSetOutputSourceByIndex(mixer.selectedOutputChannelIndex, newValue);
    routingSendConfigToFpga();
    mixerSetChangeFlags(X32_MIXER_CHANGED_GUI);
}

// ####################################################################
// #
// #
// #        Changeflags
// #
// #
// ####################################################################


void mixerSetChangeFlags(uint16_t p_flag){
    mixer.changed |= p_flag;
}

bool mixerHasAnyChanged(void){
    return (mixer.changed != X32_MIXER_CHANGED_NONE);
}

bool mixerHasChanged(uint16_t p_flag){
    return ((mixer.changed & p_flag) == p_flag);
}

void mixerResetChangeFlags(void){
    mixer.changed = X32_MIXER_CHANGED_NONE;
    for (uint8_t i=0;i<MAX_VCHANNELS;i++){
        mixerResetVChannelChangeFlags(&(mixer.vChannel[i]));
    }
}

void mixerSetVChannelChangeFlagsFromIndex(uint8_t p_chanIndex, uint16_t p_flag){
    mixer.vChannel[p_chanIndex].changed |= p_flag;
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void mixerSetVChannelChangeFlags(s_vChannel *p_chan, uint16_t p_flag){
    p_chan->changed |= p_flag;
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

bool mixerHasVChannelChanged(s_vChannel *p_chan, uint16_t p_flag){
    return ((p_chan->changed & p_flag) == p_flag);
}

bool mixerHasVChannelAnyChanged(s_vChannel *p_chan){
    return ((p_chan->changed) != X32_VCHANNEL_CHANGED_NONE);
}

void mixerResetVChannelChangeFlags(s_vChannel *p_chan){
    p_chan->changed = X32_VCHANNEL_CHANGED_NONE;
}

// ####################################################################
// #
// #
// #        vChannel Assigment
// #
// #
// ####################################################################

uint8_t mixerGetSelectedvChannelIndex(void){
    return  mixer.selectedVChannel;
}

s_vChannel* mixerGetSelectedvChannel(void){
    return  &(mixer.vChannel[mixerGetSelectedvChannelIndex()]);
}

uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel){
    if ((mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) || (mixer.activeMode == X32_SURFACE_MODE_BANKING_USER)){
        if (mixerIsModelX32Full()){
            if (surfaceChannel <= 15){
                // input-section
                return mixer.modes[mixer.activeMode].inputBanks[mixer.activeBank_inputFader].surfaceChannel2vChannel[surfaceChannel];
            } else if (surfaceChannel == 24) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return mixer.modes[mixer.activeMode].busBanks[mixer.activeBank_busFader].surfaceChannel2vChannel[surfaceChannel-16];
            }
        }
        if (mixerIsModelX32CompacrOrProducer()){
            if (surfaceChannel <= 7){
                // input-section
                return mixer.modes[mixer.activeMode].inputBanks[mixer.activeBank_inputFader].surfaceChannel2vChannel[surfaceChannel];
            } else if (surfaceChannel == 16) {
                // main-channel
                return 80;
            } else {
                // bus-section and mainfader
                return mixer.modes[mixer.activeMode].busBanks[mixer.activeBank_busFader].surfaceChannel2vChannel[surfaceChannel-8];
            }
            return 0;
        }
        if (mixerIsModelX32Core()){
            // TODO
        }
    }

    return 0;
}

uint8_t mixerGetvChannelIndexFromButtonOrFaderIndex(X32_BOARD p_board, uint16_t p_buttonIndex) {
    uint8_t offset = 0;
    if (p_board == X32_BOARD_M) { offset=8; }
    if (mixerIsModelX32Full()){
        if (p_board == X32_BOARD_R) { offset=16; }
    }
    if (mixerIsModelX32CompacrOrProducer()){
        if (p_board == X32_BOARD_R) { offset=8; }
    }
    return mixerSurfaceChannel2vChannel(p_buttonIndex + offset);
}

// ####################################################################
// #
// #
// #        Fader moved, button pressed, encoder turned and such
// #
// #
// ####################################################################

void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue){
    uint8_t vChannelIndex = VCHANNEL_NOT_SET;

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        uint8_t offset = 0;
        if (p_board == X32_BOARD_M) { offset=8;}
        if (mixerIsModelX32Full()){
            if (p_board == X32_BOARD_R) { offset=16;}
        }
        if (mixerIsModelX32CompacrOrProducer()){
            if (p_board == X32_BOARD_R) { offset=8;}
        }
        vChannelIndex = mixerSurfaceChannel2vChannel(p_faderIndex + offset);
        mixerSetVolume(vChannelIndex, fader2dBfs(p_faderValue));

        mixer.touchcontrol.board = p_board;
        mixer.touchcontrol.faderIndex = p_faderIndex;
        mixer.touchcontrol.value = 5;

        x32debug("mixerSurfaceFaderMoved(p_board=%d, p_faderIndex=%d, p_faderValue=0x%02X): vChannel%d TouchControl=%d\n", p_board, p_faderIndex, p_faderValue, vChannelIndex, mixer.touchcontrol.value);
    }
}


void mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_BTN button = button2enum(((uint16_t)p_board << 8) + (uint16_t)(p_value & 0x7F));
    bool buttonPressed = (p_value >> 7) == 1;

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        if (buttonPressed){
            switch (button) {
                case X32_BTN_LEFT:
                    mixerShowPrevPage();
                    break;
                case X32_BTN_RIGHT:
                    mixerShowNextPage();
                    break;
                case X32_BTN_HOME:
                    mixerShowPage(X32_PAGE_HOME);
                    break;
                case X32_BTN_VIEW_CONFIG:
                    mixerShowPage(X32_PAGE_CONFIG);
                    break;
                case X32_BTN_METERS:
                    mixerShowPage(X32_PAGE_METERS);
                    break;
                case X32_BTN_ROUTING:
                    mixerShowPage(X32_PAGE_ROUTING);
                    break;
                case X32_BTN_SETUP:
                    mixerShowPage(X32_PAGE_SETUP);
                    break;
                case X32_BTN_LIBRARY:
                    mixerShowPage(X32_PAGE_LIBRARY);
                    break;
                case X32_BTN_EFFECTS:
                    mixerShowPage(X32_PAGE_EFFECTS);
                    break;
                case X32_BTN_MUTE_GRP:
                    mixerShowPage(X32_PAGE_MUTE_GRP);
                    break;
                case X32_BTN_UTILITY:
                    mixerShowPage(X32_PAGE_UTILITY);
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
                    mixerBanking(button);
                    break;
                case X32_BTN_BOARD_L_CH_1_SELECT:
                case X32_BTN_BOARD_L_CH_2_SELECT:
                case X32_BTN_BOARD_L_CH_3_SELECT:
                case X32_BTN_BOARD_L_CH_4_SELECT:
                case X32_BTN_BOARD_L_CH_5_SELECT:
                case X32_BTN_BOARD_L_CH_6_SELECT:
                case X32_BTN_BOARD_L_CH_7_SELECT:
                case X32_BTN_BOARD_L_CH_8_SELECT:
                    mixerToggleSelect(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_M_CH_1_SELECT:
                case X32_BTN_BOARD_M_CH_2_SELECT:
                case X32_BTN_BOARD_M_CH_3_SELECT:
                case X32_BTN_BOARD_M_CH_4_SELECT:
                case X32_BTN_BOARD_M_CH_5_SELECT:
                case X32_BTN_BOARD_M_CH_6_SELECT:
                case X32_BTN_BOARD_M_CH_7_SELECT:
                case X32_BTN_BOARD_M_CH_8_SELECT:
                    mixerToggleSelect(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SELECT));
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
                    mixerToggleSelect(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_L_CH_1_SOLO:
                case X32_BTN_BOARD_L_CH_2_SOLO:
                case X32_BTN_BOARD_L_CH_3_SOLO:
                case X32_BTN_BOARD_L_CH_4_SOLO:
                case X32_BTN_BOARD_L_CH_5_SOLO:
                case X32_BTN_BOARD_L_CH_6_SOLO:
                case X32_BTN_BOARD_L_CH_7_SOLO:
                case X32_BTN_BOARD_L_CH_8_SOLO:
                    mixerToggleSolo(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_M_CH_1_SOLO:
                case X32_BTN_BOARD_M_CH_2_SOLO:
                case X32_BTN_BOARD_M_CH_3_SOLO:
                case X32_BTN_BOARD_M_CH_4_SOLO:
                case X32_BTN_BOARD_M_CH_5_SOLO:
                case X32_BTN_BOARD_M_CH_6_SOLO:
                case X32_BTN_BOARD_M_CH_7_SOLO:
                case X32_BTN_BOARD_M_CH_8_SOLO:
                    mixerToggleSolo(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_SOLO));
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
                    mixerToggleSolo(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_L_CH_1_MUTE:
                case X32_BTN_BOARD_L_CH_2_MUTE:
                case X32_BTN_BOARD_L_CH_3_MUTE:
                case X32_BTN_BOARD_L_CH_4_MUTE:
                case X32_BTN_BOARD_L_CH_5_MUTE:
                case X32_BTN_BOARD_L_CH_6_MUTE:
                case X32_BTN_BOARD_L_CH_7_MUTE:
                case X32_BTN_BOARD_L_CH_8_MUTE:
                    mixerToggleMute(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_L_CH_1_MUTE));
                    break;
                case X32_BTN_BOARD_M_CH_1_MUTE:
                case X32_BTN_BOARD_M_CH_2_MUTE:
                case X32_BTN_BOARD_M_CH_3_MUTE:
                case X32_BTN_BOARD_M_CH_4_MUTE:
                case X32_BTN_BOARD_M_CH_5_MUTE:
                case X32_BTN_BOARD_M_CH_6_MUTE:
                case X32_BTN_BOARD_M_CH_7_MUTE:
                case X32_BTN_BOARD_M_CH_8_MUTE:
                    mixerToggleMute(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_M_CH_1_MUTE));
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
                    mixerToggleMute(mixerGetvChannelIndexFromButtonOrFaderIndex(p_board, button - X32_BTN_BOARD_R_CH_1_MUTE));
                    break;
                case X32_BTN_CLEAR_SOLO:
                    mixerClearSolo();
                    break;
                case X32_BTN_PHANTOM_48V:
                    mixerTogglePhantom(mixerGetSelectedvChannelIndex());
                    break;
                case X32_BTN_PHASE_INVERT:
                    mixerTogglePhaseInvert(mixerGetSelectedvChannelIndex());
                    break;
                case X32_BTN_CHANNEL_SOLO: // only X32 Rack
                    mixerToggleSolo(mixerGetSelectedvChannelIndex());
                    break;
                case X32_BTN_CHANNEL_MUTE: // only X32 Rack
                    mixerToggleMute(mixerGetSelectedvChannelIndex());
                    break;
                default:
                    // TODO: Callback to x32ctrl if needed
                    x32debug("Unhandled button detected.\n");
                    break;
            }
        }
    }

    // Display Encoders
    // - are independent from Surface Modes!
    if (mixerIsModelX32FullOrCompacrOrProducerOrRack()){
        if (mixer.activePage == X32_PAGE_CONFIG){
            if (buttonPressed){
                switch (button){
                    case X32_BTN_ENCODER3:
                        mixerTogglePhantom(mixerGetSelectedvChannelIndex());
                        break;
                }
            }
        }
    }
}

void mixerSurfaceEncoderTurned(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_ENC encoder = encoder2enum(((uint16_t)p_board << 8) + (uint16_t)(p_index));
    int8_t amount = 0;

    if (p_value > 0 && p_value < 128){
        amount = (uint8_t)p_value;
    } else if (p_value > 128 && p_value < 256) {
        amount = -(256 - (uint8_t)(p_value));
    }
    x32debug("Encoder: %d\n", amount);

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING_X32) {
        switch (encoder){
            case X32_ENC_CHANNEL_SELECT:  // only X32 Rack and Core - Channel Select    TODO: Implement on Core
                mixerChangeSelect(amount);
                break;
            case X32_ENC_CHANNEL_LEVEL:
                mixerChangeVolume(mixerGetSelectedvChannelIndex(), amount);
                break;
            case X32_ENC_GAIN:
                mixerChangeGain(mixerGetSelectedvChannelIndex(), amount);
                break;
            default:
                // TODO: Callback to x32ctrl if needed
                x32debug("Unhandled encoder detected.\n");
                break;
        }
    }

    // Display Encoders
    // - are independent from Surface Modes!
    if (mixerIsModelX32FullOrCompacrOrProducerOrRack()) {
        if (mixer.activePage == X32_PAGE_CONFIG){
            switch (encoder){
                case X32_ENC_ENCODER2:
                    mixerChangeVChannel(amount);
                    break;
                case X32_ENC_ENCODER3:
                    mixerChangeGain(mixerGetSelectedvChannelIndex(), amount);
                    break;
            }
        }else if (mixer.activePage == X32_PAGE_ROUTING) {
            switch (encoder){
                case X32_ENC_ENCODER1:
                    mixerChangeHardwareOutput(amount);
                    break;
                case X32_ENC_ENCODER2:
                    mixerChangeHardwareInput(amount);
                    break;
            }
        }
    }

}

// direction - positive or negative integer value
void mixerChangeSelect(uint8_t direction){
    int16_t newSelectedVChannel = mixer.selectedVChannel += direction;
    if (newSelectedVChannel < 0) {
        newSelectedVChannel = 0;
    } else if (newSelectedVChannel >= MAX_VCHANNELS){
        newSelectedVChannel = MAX_VCHANNELS-1;
    }

    mixerSetSelect(newSelectedVChannel, true);
}

void mixerSetSelect(uint8_t vChannelIndex, bool select){
    for(uint8_t i=0; i<MAX_VCHANNELS;i++){
        mixer.vChannel[i].selected = false;
        mixer.vChannel[i].changed |= X32_VCHANNEL_CHANGED_SELECT;
    }
    mixer.vChannel[vChannelIndex].selected = select;
    mixer.vChannel[vChannelIndex].changed |= X32_VCHANNEL_CHANGED_SELECT;

    mixer.selectedVChannel = vChannelIndex;
    mixerSetChangeFlags(X32_MIXER_CHANGED_SELECT);
}

void mixerToggleSelect(uint8_t vChannelIndex){
    mixerSetSelect(vChannelIndex, !mixer.vChannel[vChannelIndex].selected);
}

void mixerSetSolo(uint8_t p_vChannelIndex, bool solo){
    mixer.vChannel[p_vChannelIndex].solo = solo;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_SOLO);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);

    //TODOs
    // - switch monitor source
}

void mixerToggleSolo(uint8_t vChannelIndex){
    mixerSetSolo(vChannelIndex, !mixer.vChannel[vChannelIndex].solo);
}

void mixerClearSolo(void){
    if (mixerIsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            mixerSetSolo(i, false);
        }
    }
}

void mixerSetPhantom(uint8_t p_vChannelIndex, bool p_phantom){
    mixer.vChannel[p_vChannelIndex].inputSource.phantomPower = p_phantom;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHANTOM);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void mixerTogglePhantom(uint8_t p_vChannelIndex){
    mixerSetPhantom(mixer.selectedVChannel, !mixer.vChannel[p_vChannelIndex].inputSource.phantomPower);
}

void mixerSetPhaseInvert(uint8_t p_vChannelIndex, bool p_phaseInvert){
    mixer.vChannel[p_vChannelIndex].inputSource.phaseInvert = p_phaseInvert;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_PHASE_INVERT);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void mixerTogglePhaseInvert(uint8_t vChannelIndex){
    mixerSetPhaseInvert(mixer.selectedVChannel, !mixer.vChannel[vChannelIndex].inputSource.phaseInvert);
}

void mixerSetMute(uint8_t p_vChannelIndex, bool mute){
    mixer.vChannel[p_vChannelIndex].mute = mute;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_MUTE);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

void mixerToggleMute(uint8_t vChannelIndex){
    mixerSetMute(vChannelIndex, !mixer.vChannel[vChannelIndex].mute);
}

void mixerBanking(X32_BTN p_button){
    if (mixerIsModelX32Full()){
        switch (p_button){
            case X32_BTN_CH_1_16:
                mixer.activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_17_32:
                mixer.activeBank_inputFader = 1;
                break;
            case X32_BTN_AUX_IN_EFFECTS:
                mixer.activeBank_inputFader = 2;
                break;
            case X32_BTN_BUS_MASTER:
                mixer.activeBank_inputFader = 3;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                mixer.activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                mixer.activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                mixer.activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                mixer.activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }
    if (mixerIsModelX32CompacrOrProducer()){
        switch (p_button){
            case X32_BTN_CH_1_8:
                mixer.activeBank_inputFader = 0;
                break;
            case X32_BTN_CH_9_16:
                mixer.activeBank_inputFader = 1;
                break;
            case X32_BTN_CH_17_24:
                mixer.activeBank_inputFader = 2;
                break;
            case X32_BTN_CH_25_32:
                mixer.activeBank_inputFader = 3;
                break;
            case X32_BTN_AUX_IN_1_6_USB_REC:
                mixer.activeBank_inputFader = 4;
                break;
            case X32_BTN_EFFECTS_RETURNS:
                mixer.activeBank_inputFader = 5;
                break;
            case X32_BTN_BUS_1_8_MASTER:
                mixer.activeBank_inputFader = 6;
                break;
            case X32_BTN_BUS_9_16_MASTER:
                mixer.activeBank_inputFader = 7;
                break;
            case X32_BTN_GROUP_DCA_1_8:
                mixer.activeBank_busFader = 0;
                break;
            case X32_BTN_BUS_1_8:
                mixer.activeBank_busFader = 1;
                break;
            case X32_BTN_BUS_9_16:
                mixer.activeBank_busFader = 2;
                break;
            case X32_BTN_MATRIX_MAIN_C:
                mixer.activeBank_busFader = 3;
                break;
            default:
                break;
        }
    }

    mixerSetChangeFlags(X32_MIXER_CHANGED_BANKING);
}

// volume in dBfs
void mixerChangeGain(uint8_t p_vChannelIndex, int8_t p_amount){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    mixer.vChannel[p_vChannelIndex].inputSource.gain += (2.5 * p_amount);
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_GAIN);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void mixerChangeVolume(uint8_t p_vChannelIndex, int8_t p_amount){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    mixer.vChannel[p_vChannelIndex].volume += p_amount;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}

// volume in dBfs
void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    mixer.vChannel[p_vChannelIndex].volume = p_volume;
    mixerSetVChannelChangeFlagsFromIndex(p_vChannelIndex, X32_VCHANNEL_CHANGED_VOLUME);
    mixerSetChangeFlags(X32_MIXER_CHANGED_VCHANNEL);
}


// ####################################################################
// #
// #
// #        Pages
// #
// #
// ###################################################################


void mixerInitPages(){
    // Init Pages
    mixer.pages[X32_PAGE_HOME].prevPage = X32_PAGE_NONE;
    mixer.pages[X32_PAGE_HOME].nextPage = X32_PAGE_CONFIG;

    mixer.pages[X32_PAGE_CONFIG].prevPage = X32_PAGE_HOME;
    mixer.pages[X32_PAGE_CONFIG].nextPage = X32_PAGE_NONE;
}

void mixerShowNextPage(void){
    if (mixer.pages[mixer.activePage].nextPage != X32_PAGE_NONE){
        mixerShowPage(mixer.pages[mixer.activePage].nextPage);
    }
}

void mixerShowPrevPage(void){
    if (mixer.pages[mixer.activePage].prevPage != X32_PAGE_NONE){
        mixerShowPage(mixer.pages[mixer.activePage].prevPage);
    }
}


void mixerShowPage(X32_PAGE p_page) {  // TODO: move to GUI Update section

    // first turn all page LEDs off
    setLedByEnum(X32_BTN_HOME, 0);
    setLedByEnum(X32_BTN_METERS, 0);
    setLedByEnum(X32_BTN_ROUTING, 0);
    setLedByEnum(X32_BTN_SETUP, 0);
    setLedByEnum(X32_BTN_LIBRARY, 0);
    setLedByEnum(X32_BTN_EFFECTS, 0);
    setLedByEnum(X32_BTN_MUTE_GRP, 0);
    setLedByEnum(X32_BTN_UTILITY, 0);
    // turn all view LEDs of
    setLedByEnum(X32_BTN_VIEW_CONFIG, 0);

    mixer.activePage = p_page;

    switch (mixer.activePage)
    {
        case X32_PAGE_HOME:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            lv_tabview_set_active(objects.hometab, 0, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_HOME, 1);
            break;
        case X32_PAGE_CONFIG:
            lv_tabview_set_active(objects.maintab, 1, LV_ANIM_OFF);
            lv_tabview_set_active(objects.hometab, 1, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_VIEW_CONFIG, 1);
            break;

        case X32_PAGE_METERS:
            lv_tabview_set_active(objects.maintab, 2, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_METERS, 1);
            break;
        case X32_PAGE_ROUTING:
            lv_tabview_set_active(objects.maintab, 3, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_ROUTING, 1);
            break;
        case X32_PAGE_SETUP:
            lv_tabview_set_active(objects.maintab, 4, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_SETUP, 1);
            break;
        case X32_PAGE_LIBRARY:
            lv_tabview_set_active(objects.maintab, 5, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_LIBRARY, 1);
            break;
        case X32_PAGE_EFFECTS:
            demoSurface();
            lv_tabview_set_active(objects.maintab, 6, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_EFFECTS, 1);
            break;
        case X32_PAGE_MUTE_GRP:
            lv_tabview_set_active(objects.maintab, 7, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_MUTE_GRP, 1);
            break;
        case X32_PAGE_UTILITY:
            lv_tabview_set_active(objects.maintab, 8, LV_ANIM_OFF);
            setLedByEnum(X32_BTN_UTILITY, 1);
            break;
        default:
            mixer.activePage = X32_PAGE_NONE;
            x32debug("ERROR: Page not found! Show welcome page instead.\n");
    }

    mixerSetChangeFlags(X32_MIXER_CHANGED_PAGE);
}

// ####################################################################
// #
// #
// #        Current state
// #
// #
// ###################################################################

bool mixerIsSoloActivated(void){
    for (int i=0; i<MAX_VCHANNELS; i++)
    {
        if (mixer.vChannel[i].solo){
            return true;
        }
    } 
    return false;
}



bool mixerIsModelX32Full(void){
    return (mixer.model == X32_MODEL_FULL);
}
bool mixerIsModelX32FullOrCompacrOrProducer(void){
    return ((mixer.model == X32_MODEL_FULL) || (mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER));
}
bool mixerIsModelX32FullOrCompacrOrProducerOrRack(void){
    return (mixerIsModelX32FullOrCompacrOrProducer() || (mixer.model == X32_MODEL_RACK));
}
bool mixerIsModelX32CompacrOrProducer(void){
    return ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER));
}
bool mixerIsModelX32Core(void){
    return (mixer.model == X32_MODEL_CORE);
}
bool mixerIsModelX32Rack(void){
    return (mixer.model == X32_MODEL_RACK);
}

// ####################################################################
// #
// #
// #        Debug
// #
// #
// ###################################################################

void mixerDebugPrintBank(uint8_t bank)
{
    x32debug("################# BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, mixer.modes[X32_SURFACE_MODE_BANKING_X32].inputBanks[bank].surfaceChannel2vChannel[i]);
    }
    x32debug("END ############# BANK%d ############### END\n", bank);
}

void mixerDebugPrintBusBank(uint8_t bank)
{
    x32debug("################# BUS BANK%d ###################\n", bank);
    for (uint8_t i=0;i<35;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, mixer.modes[X32_SURFACE_MODE_BANKING_X32].busBanks[bank].surfaceChannel2vChannel[i]);
    }
    x32debug("END ############# BUS BANK%d ############### END\n", bank);
}

void mixerDebugPrintvChannels(void){
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        s_vChannel* vChannel = &(mixer.vChannel[i]);
        x32debug("vChannel%-3d %-32s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1f input=dspCh%d\n",
                 i, vChannel->name[0], vChannel->color, vChannel->icon, vChannel->selected, vChannel->solo, vChannel->mute, (double)vChannel->volume, vChannel->inputSource.dspChannel);
    }
}
