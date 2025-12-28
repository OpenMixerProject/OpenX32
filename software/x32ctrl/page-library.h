#pragma once
#include "page.h"
using namespace std;

class PageLibrary: public Page {
    public:
        PageLibrary(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            tabLayer0 = objects.maintab;
            tabIndex0 = 4;
            led = X32_BTN_LIBRARY;
        }
};