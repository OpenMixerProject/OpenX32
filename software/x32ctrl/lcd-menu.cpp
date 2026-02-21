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

#include "lcd-menu.h"

LcdMenu::LcdMenu(X32BaseParameter* basepar, Mixer* _mixer, Surface* _surface) : X32Base(basepar) {

    this->mixer = _mixer;
    this->surface = _surface;
}

void LcdMenu::OnInit() {

    d = new LcdData();

    d->boardId = 0;
    d->color = SURFACE_COLOR_WHITE;
    d->lcdIndex = 0;
    d->texts[0].size = 0x20;
    d->texts[0].x = 0;
    d->texts[0].y = 0;
    d->texts[0].text = "Setup";

    d->texts[1].size = 0;
    d->texts[1].x = 0;
    d->texts[1].y = 25;
    d->texts[1].text = "";

    initDone = true;
}


void LcdMenu::OnShow() {
    OnChange();
}

void LcdMenu::OnChange() {
    if (initDone) {
        
        // TODO
        // if (config->HasParameterChanged(MP_ID::LCD_CONTENT_CHANGED)) {
        //     surface->SetLcdX(d, 2);
        // }
    }
}

void LcdMenu::OnLcdEncoderTurned(int8_t amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "LcdMenu::OnLcdEncoderTurned()");

    Mixerparameter* parameter = config->GetParameter(MP_ID::LCD_CONTRAST);

    parameter->Change(amount);    
    surface->SetContrast(0, parameter->GetUint());
    helper->DEBUG_SURFACE(DEBUGLEVEL_NORMAL, "Set %s to %d", parameter->GetName(), parameter->GetUint());

    d->texts[1].text = parameter->GetLabelAndValue().c_str();
}

void LcdMenu::OnLcdButton(bool pressed) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "LcdMenu::OnLcdButton()");
}