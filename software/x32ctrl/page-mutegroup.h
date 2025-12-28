#pragma once
#include "page.h"
using namespace std;

class PageMutegroup: public Page {
    public:
        PageMutegroup(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 6;
            led = X32_BTN_MUTE_GRP;
        }
};