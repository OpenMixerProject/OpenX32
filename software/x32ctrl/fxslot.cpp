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

#include "fxslot.h"

FxSlot::FxSlot(X32BaseParameter* basepar) : X32Base(basepar){
	_basepar = basepar;
}

void FxSlot::LoadFx(FX_TYPE fxToLoad) {
	
	switch(fxToLoad) {
		case FX_TYPE_REVERB:
			fx = new FxReverb(_basepar);
			fxType = fxToLoad;
			break;
    	case FX_TYPE_CHORUS:
			fx = new FxChorus(_basepar);
			fxType = fxToLoad;
			break;
		case FX_TYPE_TRANSIENTSHAPER:
			fx = new FxTransientshaper(_basepar);
			fxType = fxToLoad;
			break;
		case FX_TYPE_DELAY:
			fx = new FxDelay(_basepar);
			fxType = fxToLoad;
			break;
		default:	
			helper->Error("Can not load unknown FX.");
		case FX_TYPE_NONE:
			ClearFx();
			break;
	}
	if (fxToLoad != FX_TYPE_NONE) {
		fx->Load();
	}
	state->SetChangeFlags(X32_MIXER_CHANGED_FX);
}

void FxSlot::ClearFx() {
	fx = nullptr;
	fxType = FX_TYPE_NONE;
	state->SetChangeFlags(X32_MIXER_CHANGED_FX);
}

bool FxSlot::HasFx(){
	return fxType != FX_TYPE_NONE;
}