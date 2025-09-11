#include "touchcontrol.h"
#include "mixer.h"

// ####################################################################
// #
// #
// #        Touchcontrol
// #
// #
// ####################################################################

void touchcontrolTick(void){
    if (mixer.touchcontrol.value > 0) {
        mixer.touchcontrol.value--;
        if (mixer.touchcontrol.value == 0)
        {
            // trigger sync for this vChannel
            mixerSetVChannelChangeFlagsFromIndex(
                mixerGetvChannelIndexFromButtonOrFaderIndex(mixer.touchcontrol.board, mixer.touchcontrol.faderIndex),
                X32_VCHANNEL_CHANGED_VOLUME
            );
        }
        x32debug("TouchControl=%d\n", mixer.touchcontrol.value);
    }
}

bool touchcontrolCanSetFader(X32_BOARD p_board, uint8_t p_faderIndex) {
    if ((mixer.touchcontrol.board != p_board) && (mixer.touchcontrol.faderIndex != p_faderIndex)){
        return true;
    } 

    if (mixer.touchcontrol.value == 0){
        return true;
    }

    return false;
}
