#pragma once
#include "page.h"
using namespace std;

class PageUtility: public Page {
    public:
        PageUtilityAbout(PageBaseParameter* pagebasepar) : Page(pagebasepar) {
            prevPage = X32_PAGE::UTILITY;
            tabLayer0 = objects.maintab;
            tabIndex0 = 7;
            tabLayer1 = objects.utilitytab;
            tabIndex1 = 1;
        }
};