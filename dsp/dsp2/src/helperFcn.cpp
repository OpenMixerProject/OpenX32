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
  Copyright 2025-2026 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  This file contains some helper-functions that will be used from multiple other files
*/

#include "helperFcn.h"

// Hilfsfunktion: dB zu Linear
float helperFcn_db2lin(float db) {
	return powf(10.0f, db * 0.05f); // lin = 10^(dB / 20)
}

// Hilfsfunktion: Linear zu dB
float helperFcn_lin2db(float lin) {
	return 20.0f * log10f(lin + 1e-9f);
}

void helperFcn_calcBiquadCoeffs(int type, float frequency, float Q, float gain, float peqCoeffs[], float samplerate) {
	float V = helperFcn_db2lin(fabsf(gain)); //powf(10.0f, fabs(gain) / 20.0f);
	float K = tanf(M_PI * frequency / samplerate);
	float K2 = K * K;
	float norm;

	switch (type) {
/*
		// we are not using allpass here -> save some program-memory here
		case 0: // Allpass
		      norm = 1.0f / (1.0f + K * 1.0f/Q + K2);
		      peqCoeffs[0] = (1.0f - K * 1.0f/Q + K2) ;
		      peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
		      peqCoeffs[2] = 1.0f;
		      peqCoeffs[3] = -peqCoeffs[1];
		      peqCoeffs[4] = -peqCoeffs[0];
			break;
*/
		case 1: // Peak
			if (gain >= 0) {
				norm = 1.0f / (1.0f + 1.0f/Q * K + K2);
				peqCoeffs[0] = (1.0f + V/Q * K + K2) ;
				peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
				peqCoeffs[2] = (1.0f - V/Q * K + K2) ;
				peqCoeffs[3] = -(peqCoeffs[1]);
				peqCoeffs[4] = -((1.0f - 1.0f/Q * K + K2));
			}else{
				norm = 1.0f / (1.0f + V/Q * K + K2);
				peqCoeffs[0] = (1.0f + 1.0f/Q * K + K2) ;
				peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
				peqCoeffs[2] = (1.0f - 1.0f/Q * K + K2) ;
				peqCoeffs[3] = -(peqCoeffs[1]);
				peqCoeffs[4] = -((1.0f - V/Q * K + K2));
			}
			break;
		case 2: // Low-Shelf
		      if (gain >= 0) {    // boost
		        norm = 1.0f / (1.0f + sqrtf(2.0f) * K + K2);
		        peqCoeffs[0] = (1.0f + sqrtf(2.0f * V) * K + V * K2) ;
		        peqCoeffs[1] = 2.0f * (V * K2 - 1.0f) ;
		        peqCoeffs[2] = (1.0f - sqrtf(2.0f * V) * K + V * K2) ;
		        peqCoeffs[3] = -(2.0f * (K2 - 1.0f));
		        peqCoeffs[4] = -((1.0f - sqrtf(2.0f) * K + K2));
		      }
		      else {    // cut
		        norm = 1.0f / (1.0f + sqrtf(2.0f * V) * K + V * K2);
		        peqCoeffs[0] = (1.0f + sqrtf(2.0f) * K + K2) ;
		        peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
		        peqCoeffs[2] = (1.0f - sqrtf(2.0f) * K + K2) ;
		        peqCoeffs[3] = -(2.0f * (V * K2 - 1.0f));
		        peqCoeffs[4] = -((1.0f - sqrtf(2.0f * V) * K + V * K2));
		      }
			break;
		case 3: // High-Shelf
	        if (gain >= 0) {    // boost
	          norm = 1.0f / (1.0f + sqrtf(2.0f) * K + K2);
	          peqCoeffs[0] = (V + sqrtf(2.0f * V) * K + K2) ;
	          peqCoeffs[1] = 2.0f * (K2 - V) ;
	          peqCoeffs[2] = (V - sqrtf(2.0f * V) * K + K2) ;
	          peqCoeffs[3] = -(2.0f * (K2 - 1.0f));
	          peqCoeffs[4] = -((1.0f - sqrtf(2.0f) * K + K2));
	        }
	        else {    // cut
	          norm = 1.0f / (V + sqrtf(2.0f * V) * K + K2);
	          peqCoeffs[0] = (1.0f + sqrtf(2.0f) * K + K2) ;
	          peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
	          peqCoeffs[2] = (1.0f - sqrtf(2.0f) * K + K2) ;
	          peqCoeffs[3] = -(2.0f * (K2 - V));
	          peqCoeffs[4] = -((V - sqrtf(2.0f * V) * K + K2));
	        }
			break;
		case 4: // Bandpass
		    norm = 1.0f / (1.0f + K / Q + K2);
		    peqCoeffs[0] = (K / Q) ;
		    peqCoeffs[1] = 0.0f;
		    peqCoeffs[2] = -peqCoeffs[0];
		    peqCoeffs[3] = -(2.0f * (K2 - 1.0f));
		    peqCoeffs[4] = -((1.0f - K / Q + K2));
			break;
/*
		// we are not using notch here -> save some program-memory here
		case 5: // Notch
	        norm = 1.0f / (1.0f + K / Q + K2);
	        peqCoeffs[0] = (1.0f + K2) ;
	        peqCoeffs[1] = 2.0f * (K2 - 1.0f) ;
	        peqCoeffs[2] = peqCoeffs[0];
	        peqCoeffs[3] = -(peqCoeffs[1]);
	        peqCoeffs[4] = -((1.0f - K / Q + K2));
			break;
*/
		case 6: // LowPass
	        norm = 1.0f / (1.0f + K / Q + K2);
	        peqCoeffs[0] = K2 ;
	        peqCoeffs[1] = 2.0f * peqCoeffs[0];
	        peqCoeffs[2] = peqCoeffs[0];
	        peqCoeffs[3] = -(2.0 * (K2 - 1.0f));
	        peqCoeffs[4] = -((1.0f - K / Q + K2));
			break;
		case 7: // HighPass
	        norm = 1.0f / (1.0f + K / Q + K2);
	        peqCoeffs[0] = 1.0f ;
	        peqCoeffs[1] = -2.0f * peqCoeffs[0];
	        peqCoeffs[2] = peqCoeffs[0];
	        peqCoeffs[3] = -(2.0f * (K2 - 1.0f));
	        peqCoeffs[4] = -((1.0f - K / Q + K2));
			break;
	}

	// apply normalization
	for (int i = 0; i < 5; i++) {
		peqCoeffs[i] *= norm;
	}
}
