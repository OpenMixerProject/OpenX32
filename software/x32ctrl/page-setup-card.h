#pragma once
#include "page.h"
using namespace std;

class PageSetupCard: public Page {
    public:
        PageSetupCard(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_SETUP;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 1;
        }

        void OnChange(bool force_update) override {

            if (state->HasChanged(X32_MIXER_CHANGED_CARD) || force_update)
            {
                lv_label_set_text_fmt(objects.setup_card_detected, "%s", mixer->adda->GetExpansion().c_str());
            }
        }
};