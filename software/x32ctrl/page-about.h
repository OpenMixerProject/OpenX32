#pragma once
#include "page.h"
using namespace std;

class PageAbout: public Page {
    public:
        PageAbout(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::SETUP_CARD;
            nextPage = X32_PAGE::DEBUG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 2;
            led = X32_BTN_SETUP;
        }
};