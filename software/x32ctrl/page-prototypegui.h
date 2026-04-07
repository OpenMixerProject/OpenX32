#pragma once
#include "page.h"
using namespace std;

class PagePrototypeGui: public Page
{
    private:

        int gui_selected_destination = 0;
		int gui_selected_destination_before = 0;

    public:
        PagePrototypeGui(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::DEBUG;
            tabLayer0 = objects.maintab;
            tabIndex0 = 3;
            tabLayer1 = objects.setuptab;
            tabIndex1 = 4;
            led = X32_BTN_SETUP;
        }       
};