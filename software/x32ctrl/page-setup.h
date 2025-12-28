#pragma once
#include "page.h"
using namespace std;

class PageSetup: public Page {
    public:
        PageSetup(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            led = X32_BTN_SETUP;
        }
};