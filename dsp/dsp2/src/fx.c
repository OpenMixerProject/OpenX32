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

#include "fx.h"

#if FX_USE_REVERB == 1
	#include "fxReverb.h"
#endif
#if FX_USE_OVERDRIVE == 1
	#include "fxOverdrive.h"
#endif
#if FX_USE_CHORUS == 1
	#include "fxChorus.h"
#endif
#if FX_USE_UPMIXER == 1
	#include "fxUpmixer.h"
#endif
#if FX_USE_MATRIXUPMIXER == 1
	#include "fxMatrixUpmixer.h"
#endif

void fxInit(void) {
	#if FX_USE_REVERB == 1
		fxReverbInit();
	#endif

	#if FX_USE_OVERDRIVE == 1
		fxOverdriveInit();
	#endif

	#if FX_USE_CHORUS == 1
		fxChorusInit();
	#endif

	#if FX_USE_UPMIXER == 1
		fxUpmixerInit();
	#endif

	#if FX_USE_MATRIXUPMIXER == 1
		fxMatrixUpmixerInit();
	#endif
}
