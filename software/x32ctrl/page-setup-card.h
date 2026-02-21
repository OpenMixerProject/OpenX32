#pragma once
#include "page.h"
using namespace std;

class PageSetupCard: public Page {
    private:
        uint8_t card_channelmode;

        void Encodertext() {
            String cardmode = String("Channelmode\n"); // TODO: + mixer->GetCardChannelModeString(card_channelmode);
            custom_encoder[0].label = cardmode;
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
            BindEncoder(DISPLAY_ENCODER_1, MP_ID::CARD_NUMBER_OF_CHANNELS);

            lv_label_set_text_fmt(objects.setup_card_detected, "%s", mixer->GetCardModelString().c_str());
        }

        void OnChange(bool force_update) override
        {
            if (config->HasParameterChanged(CARD_NUMBER_OF_CHANNELS) || force_update)
            {
                lv_label_set_text(objects.setup_card_channelmode, config->GetParameter(CARD_NUMBER_OF_CHANNELS)->GetFormatedValue().c_str());
            }
        }
};