#pragma once
#include "page.h"
using namespace std;

class PageHome : public Page {
    public:
        PageHome(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            nextPage = X32_PAGE::CONFIG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 0;
            tabLayer1 = objects.hometab;
            tabIndex1 = 0;
            led = X32_BTN_HOME;
            hideEncoders = true;
        }
};