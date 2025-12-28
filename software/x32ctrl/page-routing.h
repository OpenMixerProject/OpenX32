#pragma once

#include "page.h"

using namespace std;

class PageRouting: public Page {
    public:
        PageRouting(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE_NONE;
            nextPage = X32_PAGE_ROUTING_FPGA;
            tabLayer0 = objects.maintab;
            tabIndex0 = 2;
            tabLayer1 = objects.routingtab;
            tabIndex1 = 0;
            led = X32_BTN_ROUTING;
        }
};