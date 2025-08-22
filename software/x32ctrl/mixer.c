#include "mixer.h"


s_Mixer mixer;

void initMixer(X32_MODEL p_model) {

    x32debug("############# InitMixer() #############\n");

    // disable all vChannels
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        mixerSetVChannelDefaults(&mixer.vChannel[i], i, true);
    }

    // empty all banks
    for (uint8_t b = 0; b < MAX_BANKS; b++){
        for (uint8_t sCh = 0; sCh < MAX_SURFACECHANNELS; sCh++){
            mixer.bank[b].surfaceChannel2vChannel[sCh] = VCHANNEL_NOT_SET;
        }
    }

    mixer.model = p_model;

    //##################################################################################
    //#
    //#   create default vChannels (what the user is refering to as "mixer channel")
    //#
    //#   0-31     traditional channels, xlr input 1-32 connected as source, xlr output 1-8 as destination on first 8 vChannels
    //#   32       main bus - not really implemented right now
    //#
    //##################################################################################
    for (int i=0; i<=31; i++) {
        s_vChannel *vChannel = &mixer.vChannel[i];
        mixerSetVChannelDefaults(vChannel, i+1, false);

        s_inputSource *input = &vChannel->inputSource;
        input->phantomPower = false;
        input->phaseInvert = false;
        input->group = 'x';
        input->hardwareChannel = i;

        if ((i>0) && (i<=7)) {
            s_outputDestination *output = &vChannel->outputDestination;
            output->group = 'x';
            output->hardwareChannel = i;       
        }
    }

#if DEBUG
    // set color on first 8 channels for testing
    // set input to Off for channelindex 5
    mixer.vChannel[0].color = SURFACE_COLOR_WHITE;
    sprintf(mixer.vChannel[0].name, "White");
    mixer.vChannel[1].color = SURFACE_COLOR_YELLOW;
    sprintf(mixer.vChannel[1].name, "Yellow");
    mixer.vChannel[2].color = SURFACE_COLOR_BLUE;
    sprintf(mixer.vChannel[2].name, "Blue");
    mixer.vChannel[3].color = SURFACE_COLOR_CYAN;
    sprintf(mixer.vChannel[3].name, "Cyan has 30 characters - long.");
    mixer.vChannel[4].color = SURFACE_COLOR_GREEN;
    sprintf(mixer.vChannel[4].name, "Green");
    mixer.vChannel[5].color = SURFACE_COLOR_PINK;
    sprintf(mixer.vChannel[5].name, "Pink (Off)");
    mixer.vChannel[5].inputSource.group = 0;
    mixer.vChannel[5].inputSource.hardwareChannel = 0;
    mixer.vChannel[6].color = SURFACE_COLOR_RED;
    sprintf(mixer.vChannel[6].name, "Red");
    mixer.vChannel[7].color = SURFACE_COLOR_BLACK;
    sprintf(mixer.vChannel[7].name, "Black");
#endif

    s_vChannel *mainChannel = &mixer.vChannel[VCHANNEL_IDX_MAIN];
    mixerSetVChannelDefaults(mainChannel, VCHANNEL_IDX_MAIN + 1, false);
    sprintf(mainChannel->name, "Main");
    mainChannel->vChannelType = 1; // main channel

    // vChannel0 selected by default
    mixerSetSelect(0, 1);

    //##################################################################################
    //#
    //#   assign vChannels to bank0, 1, (2, 3) - (easy mode - 1:1)
    //#
    //##################################################################################

    if (mixerIsModelX32Full()){
        // bank is 16 channels wide
        for (int i = 0; i <=15; i++) {
            // bank0 - vChannel 0-15
            mixer.bank[0].surfaceChannel2vChannel[i] = i;
            // bank1 - vChannel 16-31
            mixer.bank[1].surfaceChannel2vChannel[i] = i+16;
        }
    } 

    if (mixerIsModelX32CompacrOrProducer()){
        // bank is 8 channels wide
        for (int i = 0; i <=7; i++) {
            // bank0 - vChannel 0-7
            mixer.bank[0].surfaceChannel2vChannel[i] = i;
            // bank1 - vChannel 8-15
            mixer.bank[1].surfaceChannel2vChannel[i] = i+8;
            // bank2 - vChannel 16-23
            mixer.bank[2].surfaceChannel2vChannel[i] = i+16;
            // bank3 - vChannel 24-31
            mixer.bank[3].surfaceChannel2vChannel[i] = i+24;
        }
    }

    // Main Fader
    //mixer.bank[0].surfaceChannel2vChannel[24] = 24;
    
    mixer.activeMode = X32_SURFACE_MODE_BANKING;
    mixer.activeBank = 0;

    x32debug("END ############# InitMixer() ############# END \n\n");
}

void mixerSetVChannelDefaults(s_vChannel* p_vChannel, uint8_t p_number, bool p_disabled){
    p_vChannel->number = p_number;
    sprintf(p_vChannel->name, "Kanal %d", p_number); // german label :-)
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
    p_vChannel->inputSource.group = 0;
    p_vChannel->inputSource.hardwareChannel = 0;
    p_vChannel->outputDestination.group = 0;
    p_vChannel->outputDestination.hardwareChannel = 0;
}

void mixerSetAllDirty(void){
    mixer.dirty = X32_DIRTY_ALL;
}

void mixerSetVolumeDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_VOLUME;
}

void mixerSetSelectDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_SELECT;
}

void mixerSetSoloDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_SOLO;
}

void mixerSetMuteDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_MUTE;
}

void mixerSetLCDDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_LCD;
}

void mixerSetBankingDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_BANKING;
}

void mixerSetRoutingDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_ROUTING;
}

void mixerSetPhantomDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_PHANTOM;
}

void mixerSetPhaseInvertDirty(void){
    mixer.dirty = mixer.dirty | X32_DIRTY_PHASE_INVERT;
}

void mixerSetDirtClean(void){
    mixer.dirty = X32_DIRTY_NONE;
}

bool mixerIsDirty(void){
    return (mixer.dirty!=X32_DIRTY_NONE);
}

bool mixerIsVolumeDirty(void){
    return ((mixer.dirty & X32_DIRTY_VOLUME) == X32_DIRTY_VOLUME);
}

bool mixerIsSelectDirty(void){
    return ((mixer.dirty & X32_DIRTY_SELECT) == X32_DIRTY_SELECT);
}

bool mixerIsSoloDirty(void){
    return ((mixer.dirty & X32_DIRTY_SOLO) == X32_DIRTY_SOLO);
}

bool mixerIsMuteDirty(void){
    return ((mixer.dirty & X32_DIRTY_MUTE) == X32_DIRTY_MUTE);
}

bool mixerIsLCDDirty(void){
    return ((mixer.dirty & X32_DIRTY_LCD) == X32_DIRTY_LCD);
}

bool mixerIsBankingDirty(void){
    return ((mixer.dirty & X32_DIRTY_BANKING) == X32_DIRTY_BANKING);
}

bool mixerIsRoutingDirty(void){
    return ((mixer.dirty & X32_DIRTY_ROUTING) == X32_DIRTY_ROUTING);
}

bool mixerIsPhantomDirty(void){
    return ((mixer.dirty & X32_DIRTY_PHANTOM) == X32_DIRTY_PHANTOM);
}

bool mixerIsPhaseInvertDirty(void){
    return ((mixer.dirty & X32_DIRTY_PHASE_INVERT) == X32_DIRTY_PHASE_INVERT);
}


void mixerTouchControllTick(void){
    if (mixer.touchcontrol.value > 0) {
        mixer.touchcontrol.value--;
        if (mixer.touchcontrol.value == 0)
        {
            mixerSetVolumeDirty();
        }
        x32debug("TouchControl=%d\n", mixer.touchcontrol.value);
    }
}

bool mixerTouchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
    if ((mixer.touchcontrol.board != p_board) && (mixer.touchcontrol.faderIndex != p_faderIndex)){
        return true;
    } 

    if (mixer.touchcontrol.value == 0){
        return true;
    }

    return false;
}

uint8_t mixerSurfaceChannel2vChannel(uint8_t surfaceChannel)
{
    // if (mixer.model == X32_MODEL_COMPACT | mixer.model == X32_MODEL_PRODUCER){
    //     if (surfaceChannel == 17)
    //     {
    //         surfaceChannel= 24;
    //     }
    // }
    return mixer.bank[mixer.activeBank].surfaceChannel2vChannel[surfaceChannel];
}

void mixerSurfaceFaderMoved(X32_BOARD p_board, uint8_t p_faderIndex, uint16_t p_faderValue){
    uint8_t vChannelIndex = VCHANNEL_NOT_SET;

    if (mixer.activeMode == X32_SURFACE_MODE_BANKING) {
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


bool mixerSurfaceButtonPressed(X32_BOARD p_board, uint8_t p_index, uint16_t p_value) {
    X32_BTN button = button2enum(((uint16_t)p_board << 8) + (uint16_t)(p_value & 0x7F));
    bool buttonPressed = (p_value >> 7) == 1;
    
    if (mixer.activeMode == X32_SURFACE_MODE_BANKING) {      
        if (buttonPressed){
            switch (button) {
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
                case X32_BTN_CH_1_8:
                case X32_BTN_CH_9_16:
                case X32_BTN_CH_17_24:
                case X32_BTN_CH_25_32:
                case X32_BTN_CH_1_16:
                case X32_BTN_CH_17_32:
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
                    mixerToggleSelect(mixerSurfaceSelectSoloMuteButtonGetvChannel(p_board, button - X32_BTN_BOARD_L_CH_1_SELECT));
                    break;
                case X32_BTN_BOARD_L_CH_1_SOLO:
                case X32_BTN_BOARD_L_CH_2_SOLO:
                case X32_BTN_BOARD_L_CH_3_SOLO:
                case X32_BTN_BOARD_L_CH_4_SOLO:
                case X32_BTN_BOARD_L_CH_5_SOLO:
                case X32_BTN_BOARD_L_CH_6_SOLO:
                case X32_BTN_BOARD_L_CH_7_SOLO:
                case X32_BTN_BOARD_L_CH_8_SOLO:
                    mixerToggleSolo(mixerSurfaceSelectSoloMuteButtonGetvChannel(p_board, button - X32_BTN_BOARD_L_CH_1_SOLO));
                    break;
                case X32_BTN_BOARD_L_CH_1_MUTE:
                case X32_BTN_BOARD_L_CH_2_MUTE:
                case X32_BTN_BOARD_L_CH_3_MUTE:
                case X32_BTN_BOARD_L_CH_4_MUTE:
                case X32_BTN_BOARD_L_CH_5_MUTE:
                case X32_BTN_BOARD_L_CH_6_MUTE:
                case X32_BTN_BOARD_L_CH_7_MUTE:
                case X32_BTN_BOARD_L_CH_8_MUTE:
                    mixerToggleMute(mixerSurfaceSelectSoloMuteButtonGetvChannel(p_board, button - X32_BTN_BOARD_L_CH_1_MUTE));
                    break;            
                case X32_BTN_CLEAR_SOLO:
                    mixerClearSolo();
                    break;
                case X32_BTN_PHANTOM_48V:
                    mixerTogglePhantom(mixer.selectedVChannel);
                    break;
                case X32_BTN_PHASE_INVERT:
                    mixerTogglePhaseInvert(mixer.selectedVChannel);
                    break;
                default:
                    break;
            }
        }
    }

    return false;
}

uint8_t mixerSurfaceSelectSoloMuteButtonGetvChannel(X32_BOARD p_board, uint16_t p_buttonIndex) {
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

void mixerSetSelect(uint8_t vChannelIndex, bool select){
    for(uint8_t i=0; i<MAX_VCHANNELS;i++){
        mixer.vChannel[i].selected = false;
    }
    mixer.vChannel[vChannelIndex].selected = select;
    mixer.selectedVChannel=vChannelIndex;
    
    // set all dirty on Board_Main
    mixerSetSelectDirty();
    mixerSetPhantomDirty();
    mixerSetPhaseInvertDirty();
}

void mixerToggleSelect(uint8_t vChannelIndex){
    mixerSetSelect(vChannelIndex, !mixer.vChannel[vChannelIndex].selected);
}

uint8_t mixerGetSelectedvChannel(void){
    return mixer.selectedVChannel;
}

void mixerSetSolo(uint8_t vChannelIndex, bool solo){
    mixer.vChannel[vChannelIndex].solo = solo;
    mixerSetSoloDirty();

    //TODOs
    // - switch monitor source
}

void mixerToggleSolo(uint8_t vChannelIndex){
    mixerSetSolo(vChannelIndex, !mixer.vChannel[vChannelIndex].solo);
}

bool mixerIsSoloActivated(void){
    for (int i=0; i<MAX_VCHANNELS; i++)
    {
        if (mixer.vChannel[i].solo){
            return true;
        }
    } 
    return false;
}

void mixerClearSolo(void){
    if (mixerIsSoloActivated()){
        for (int i=0; i<MAX_VCHANNELS; i++){
            mixerSetSolo(i, false);
        } 
    }
}

void mixerSetPhantom(uint8_t vChannelIndex, bool p_phantom){
    mixer.vChannel[vChannelIndex].inputSource.phantomPower = p_phantom;
    mixerSetPhantomDirty();
    mixerSetLCDDirty();
}

void mixerTogglePhantom(uint8_t vChannelIndex){
    mixerSetPhantom(mixer.selectedVChannel, !mixer.vChannel[vChannelIndex].inputSource.phantomPower);
}

void mixerSetPhaseInvert(uint8_t vChannelIndex, bool p_phaseInvert){
    mixer.vChannel[vChannelIndex].inputSource.phaseInvert = p_phaseInvert;
    mixerSetPhaseInvertDirty();
    mixerSetLCDDirty();
}

void mixerTogglePhaseInvert(uint8_t vChannelIndex){
    mixerSetPhaseInvert(mixer.selectedVChannel, !mixer.vChannel[vChannelIndex].inputSource.phaseInvert);
}

void mixerSetMute(uint8_t vChannelIndex, bool mute){
    mixer.vChannel[vChannelIndex].mute = mute;
    mixerSetMuteDirty();
}

void mixerToggleMute(uint8_t vChannelIndex){
    mixerSetMute(vChannelIndex, !mixer.vChannel[vChannelIndex].mute);
}

void mixerBanking(X32_BTN p_button){
    if (mixerIsModelX32Full()){
        switch (p_button){
            case X32_BTN_CH_9_16:
                mixer.activeBank = 0;
                break;
            case X32_BTN_CH_17_32:
                mixer.activeBank = 1;
                break;
            default:
                break;    
        }
    }
    if (mixerIsModelX32CompacrOrProducer()){
        switch (p_button){
            case X32_BTN_CH_1_8:
                mixer.activeBank = 0;
                break;
            case X32_BTN_CH_9_16:
                mixer.activeBank = 1;
                break;
            case X32_BTN_CH_17_24:
                mixer.activeBank = 2;
                break;
            case X32_BTN_CH_25_32:
                mixer.activeBank = 3;
                break;
            default:
                break;    
        }
    }

    mixerSetAllDirty(); // banking affects all, so do a global refresh
}

// volume in dBfs
void mixerSetVolume(uint8_t p_vChannelIndex, float p_volume){
    if (p_vChannelIndex == VCHANNEL_NOT_SET) {
        return;
    }
    mixer.vChannel[p_vChannelIndex].volume = p_volume;

    mixerSetVolumeDirty();
    mixerSetLCDDirty();
}


void mixerShowPage(X32_PAGE page) {
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
    

    switch (page)
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
            surfaceDemo();
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
            x32debug("ERROR: Page not found! Show welcome page instead.\n");
    }
}


bool mixerIsModelX32Full(void){
    return (mixer.model == X32_MODEL_FULL);
}
bool mixerIsModelX32CompacrOrProducer(void){
    return ((mixer.model == X32_MODEL_COMPACT) || (mixer.model == X32_MODEL_PRODUCER));
}

bool mixerIsModelX32Core(void){
    return (mixer.model == X32_MODEL_CORE);
}

void mixerDebugPrintBank(uint8_t bank)
{
    x32debug("################# BANK%d ###################\n", bank);
    for (uint8_t i=0;i<MAX_SURFACECHANNELS;i++){
        x32debug("surfaceChannel%d -> vChannel%d\n", i, mixer.bank[bank].surfaceChannel2vChannel[i]);
    }
    x32debug("END ############# BANK ################ END\n");
}

void mixerDebugPrintvChannels(void){
    for (int i = 0; i < MAX_VCHANNELS; i++)
    {
        s_vChannel* vChannel = &mixer.vChannel[i];
        x32debug("vChannel%d: name=%s color=%d icon=%d selected=%d solo=%d mute=%d volume=%2.1f input=%c%d output=%c%d\n",
                 i, vChannel->name, vChannel->color, vChannel->icon, vChannel->selected, vChannel->solo, vChannel->mute, (double)vChannel->volume,
                 vChannel->inputSource.group, vChannel->inputSource.hardwareChannel,
                 vChannel->outputDestination.group, vChannel->outputDestination.hardwareChannel);
    }
}