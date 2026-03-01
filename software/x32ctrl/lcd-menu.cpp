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
    d->texts[0].text = "";

    d->texts[1].size = 0;
    d->texts[1].x = 0;
    d->texts[1].y = 25;
    d->texts[1].text = "";

    d->texts[2].size = 0;
    d->texts[2].x = 0;
    d->texts[2].y = 48;
    d->texts[2].text = "";

    // Parameters for Setup Menu
    menuItems.push_back(LCD_CONTRAST);
    menuItems.push_back(CHANNEL_LCD_MODE);
    menuItems.push_back(LED_BRIGHTNESS);
    menuItems.push_back(SAMPLERATE);
    menuItems.push_back(CARD_NUMBER_OF_CHANNELS);
    menuItems.push_back(MONITOR_VOLUME);

    initDone = true;
}


void LcdMenu::OnShow() {
    OnChange(true);
}

void LcdMenu::OnChange(bool force_update)
{
    if (initDone) {

        MP_ID currentParameter = menuItems[selectedItem];
        
        if (config->HasParameterChanged(currentParameter) || selectedItem != selectedItemBefore || force_update)
        {
            d->texts[0].text = String("Setup");
            d->texts[1].text = config->GetParameter(currentParameter)->GetName();
            d->texts[2].text = 
                (encoderModeSelection ? String("") : String("[")) +
                config->GetParameter(currentParameter)->GetFormatedValue() +
                (encoderModeSelection ? String("") : String("]"));
            surface->SetLcdX(d, 3);

            selectedItemBefore = selectedItem;
        }
    }
}

void LcdMenu::OnLcdEncoderTurned(int8_t amount) {
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "LcdMenu::OnLcdEncoderTurned()");

    if (encoderModeSelection)
    {
        selectedItem += amount;

        if (selectedItem > (menuItems.size() -1))
        {
            selectedItem = 0;
        }
    }
    else
    {
        MP_ID currentParameter = menuItems[selectedItem];

        config->Change(currentParameter, amount);
    }

    OnChange(false);
}

void LcdMenu::OnLcdEncoderPressed()
{
    helper->DEBUG_GUI(DEBUGLEVEL_NORMAL, "LcdMenu::OnLcdEncoderPressed()");

    encoderModeSelection = !encoderModeSelection;

    OnChange(true);
}