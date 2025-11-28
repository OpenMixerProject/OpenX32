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

#include "surface-event.h"

SurfaceEvent::SurfaceEvent(X32_BOARD _boardId, uint8_t _classId, uint8_t _index, uint16_t _value){
  this->boardId = _boardId;
  this->classId = _classId;
  this->index = _index;
  this->value = _value;
}

String SurfaceEvent::ToString(void){
  String s = String("Board: ") + String(boardId) + " Class: " + String(classId) + " Index: " + String(index) + " Value: " + String(value);
  return s;
}