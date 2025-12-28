/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "page.h"

Page::Page(PageBaseParameter* pagebasepar) : X32Base(pagebasepar) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Page()");
    mixer = pagebasepar->mixer;
}

void Page::Init() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Init()");
    OnInit();
    initDone = true;
}

void Page::OnInit() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnInit()");
}

void Page::Show() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::Show()");

    // open tab on Layer0
	if (tabLayer0 != nullptr) {
		lv_tabview_set_active(tabLayer0, tabIndex0, LV_ANIM_OFF);
	}

	// open tab on Layer1
	if (tabLayer1 != nullptr) {
		lv_tabview_set_active(tabLayer1, tabIndex1, LV_ANIM_OFF);
	}

    OnShow();
}

void Page::OnShow() {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnShow()");
	SetEncoderText("-", "-", "-", "-", "-", "-");
}

void Page::Change() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Change()");
    if (initDone) {
        OnChange();
    }
}

void Page::OnChange() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::OnChange()");
}

void Page::Time10ms() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::Time10ms()");
    if (initDone) {
        OnTime10ms();
    }
}

void Page::OnTime10ms() {
    helper->DEBUG_GUI(DEBUGLEVEL_TRACE, "Page::OnTime10ms()");
}

void Page::OnDisplayEncoderTurned(X32_ENC encoder, int8_t amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnDisplayEncoderTurned()");
}

void Page::OnDisplayButton(X32_BTN button, bool pressed) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "Page::OnDisplayButton()");
}

X32_PAGE Page::GetNextPage() {
    return nextPage;
}

X32_PAGE Page::GetPrevPage() {
    return prevPage;
}

X32_BTN Page::GetLed() {
    if (!(config->IsModelX32Rack() && noLedOnRack))
    {
        return led;
    }

    return X32_BTN_NONE;
}

void Page::SetEncoderText(String enc1, String enc2, String enc3, String enc4, String enc5, String enc6) {
	sprintf(&displayEncoderText[0][0], "%s", enc1.c_str());
	sprintf(&displayEncoderText[1][0], "%s", enc2.c_str());
	sprintf(&displayEncoderText[2][0], "%s", enc3.c_str());
	sprintf(&displayEncoderText[3][0], "%s", enc4.c_str());
	sprintf(&displayEncoderText[4][0], "%s", enc5.c_str());
	sprintf(&displayEncoderText[5][0], "%s", enc6.c_str());
	lv_btnmatrix_set_map(objects.display_encoders, displayEncoderButtonMap);
}