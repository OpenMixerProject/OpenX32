#pragma once
#include "page.h"
using namespace std;

class PageEffects: public Page {
    public:
        PageEffects(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 5;
            led = X32_BTN_EFFECTS;
        }
};