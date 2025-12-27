#pragma once

#include "defines.h"
#include "enum.h"
#include "lv_port_linux/lvgl/lvgl.h"

using namespace std;

class Page {
    public:
        X32_PAGE nextPage;
        X32_PAGE prevPage;

        lv_obj_t* tabLayer0; 
        uint32_t tabIndex0;
        lv_obj_t* tabLayer1;
        uint32_t tabIndex1;

        X32_BTN led;
        bool noLedOnRack;

        Page(
            X32_PAGE _prevPage,
            X32_PAGE _nextPage,
            lv_obj_t* _tabLayer0,
            uint32_t _tabIndex0,
            lv_obj_t* _tabLayer1,
            uint32_t _tabIndex1,
            X32_BTN _led,
            bool _noLedOnRack = false) {
                this->nextPage = _nextPage;
                this->prevPage = _prevPage;
                this->tabLayer0 = _tabLayer0;
                this->tabIndex0 = _tabIndex0;
                this->tabLayer1 = _tabLayer1;
                this->tabIndex1 = _tabIndex1;
                this->led = _led;
                this->noLedOnRack = _noLedOnRack;
        }
};