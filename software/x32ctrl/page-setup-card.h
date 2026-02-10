#pragma once
#include "page.h"
using namespace std;

class PageSetupCard: public Page {
    private:
        uint8_t card_channelmode;

        void Encodertext() {
            String cardmode = String("Channelmode\n"); // TODO: + mixer->GetCardChannelModeString(card_channelmode);
            encoderSliders[0].label = cardmode;
        }

    public:
        PageSetupCard(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SETUP;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 1;
        }

        void OnInit() override {

            BindEncoder(DISPLAY_ENCODER_1, false, MP_ID::CARD_NUMBER_OF_CHANNELS);
        }

        void OnChange(bool force_update) override {

            // TODO: Implement new

            // if (state->HasChanged(X32_MIXER_CHANGED_CARD) || force_update)
            // {
            //     lv_label_set_text_fmt(objects.setup_card_detected, "%s", mixer->GetCardModelString().c_str());
            //     lv_label_set_text_fmt(objects.setup_card_channelmode, "%s", mixer->GetCardChannelModeString().c_str());
            //     Encodertext();
            // }
        }
};