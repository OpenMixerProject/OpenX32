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

X32Config config;

void fxRecalcFilterCoefficients_PEQ(sPEQ* peq) {
  // Online-Calculator: https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3
  // Source: https://www.earlevel.com/main/2012/11/26/biquad-c-source-code
  double V = pow(10.0, fabs(peq->gain)/20.0);
  double K = tan(PI * peq->fc / config.GetSamplerate());
  double norm;

  switch (peq->type) {
    case 0: // allpass
      norm = 1.0 / (1.0 + K * 1.0/peq->Q + K * K);
      peq->a[0] = (1.0 - K * 1.0/peq->Q + K * K) * norm;
      peq->a[1] = 2.0 * (K * K - 1.0) * norm;
      peq->a[2] = 1.0;
      peq->b[1] = peq->a[1];
      peq->b[2] = peq->a[0];
      break;
    case 1: // peak-filter
      if (peq->gain >= 0) {
        norm = 1.0 / (1.0 + 1.0/peq->Q * K + K * K);
        peq->a[0] = (1.0 + V/peq->Q * K + K * K) * norm;
        peq->a[1] = 2.0 * (K * K - 1.0) * norm;
        peq->a[2] = (1.0 - V/peq->Q * K + K * K) * norm;
        peq->b[1] = peq->a[1];
        peq->b[2] = (1.0 - 1.0/peq->Q * K + K * K) * norm;
      }else{
        norm = 1.0 / (1.0 + V/peq->Q * K + K * K);
        peq->a[0] = (1.0 + 1.0/peq->Q * K + K * K) * norm;
        peq->a[1] = 2.0 * (K * K - 1.0) * norm;
        peq->a[2] = (1.0 - 1.0/peq->Q * K + K * K) * norm;
        peq->b[1] = peq->a[1];
        peq->b[2] = (1.0 - V/peq->Q * K + K * K) * norm;
      }
      break;
    case 2: // low-shelf
      if (peq->gain >= 0) {    // boost
        norm = 1.0 / (1.0 + sqrt(2.0) * K + K * K);
        peq->a[0] = (1.0 + sqrt(2.0*V) * K + V * K * K) * norm;
        peq->a[1] = 2.0 * (V * K * K - 1.0) * norm;
        peq->a[2] = (1.0 - sqrt(2.0*V) * K + V * K * K) * norm;
        peq->b[1] = 2.0 * (K * K - 1.0) * norm;
        peq->b[2] = (1.0 - sqrt(2.0) * K + K * K) * norm;
      }
      else {    // cut
        norm = 1.0 / (1.0 + sqrt(2.0*V) * K + V * K * K);
        peq->a[0] = (1.0 + sqrt(2.0) * K + K * K) * norm;
        peq->a[1] = 2.0 * (K * K - 1.0) * norm;
        peq->a[2] = (1.0 - sqrt(2) * K + K * K) * norm;
        peq->b[1] = 2.0 * (V * K * K - 1.0) * norm;
        peq->b[2] = (1.0 - sqrt(2.0*V) * K + V * K * K) * norm;
      }
      break;
    case 3: // high-shelf
        if (peq->gain >= 0) {    // boost
          norm = 1.0 / (1.0 + sqrt(2.0) * K + K * K);
          peq->a[0] = (V + sqrt(2.0*V) * K + K * K) * norm;
          peq->a[1] = 2.0 * (K * K - V) * norm;
          peq->a[2] = (V - sqrt(2.0*V) * K + K * K) * norm;
          peq->b[1] = 2.0 * (K * K - 1.0) * norm;
          peq->b[2] = (1.0 - sqrt(2.0) * K + K * K) * norm;
        }
        else {    // cut
          norm = 1.0 / (V + sqrt(2.0*V) * K + K * K);
          peq->a[0] = (1.0 + sqrt(2.0) * K + K * K) * norm;
          peq->a[1] = 2.0 * (K * K - 1.0) * norm;
          peq->a[2] = (1.0 - sqrt(2.0) * K + K * K) * norm;
          peq->b[1] = 2.0 * (K * K - V) * norm;
          peq->b[2] = (V - sqrt(2.0*V) * K + K * K) * norm;
        }
      break;
    case 4: // bandpass
        norm = 1.0 / (1.0 + K / peq->Q + K * K);
        peq->a[0] = (K / peq->Q) * norm;
        peq->a[1] = 0;
        peq->a[2] = -peq->a[0];
        peq->b[1] = 2.0 * (K * K - 1.0) * norm;
        peq->b[2] = (1.0 - K / peq->Q + K * K) * norm;
      break;
    case 5: // notch
        norm = 1.0 / (1.0 + K / peq->Q + K * K);
        peq->a[0] = (1.0 + K * K) * norm;
        peq->a[1] = 2.0 * (K * K - 1.0) * norm;
        peq->a[2] = peq->a[0];
        peq->b[1] = peq->a[1];
        peq->b[2] = (1.0 - K / peq->Q + K * K) * norm;
      break;
    case 6: // lowpass
        norm = 1.0 / (1.0 + K / peq->Q + K * K);
        peq->a[0] = K * K * norm;
        peq->a[1] = 2.0 * peq->a[0];
        peq->a[2] = peq->a[0];
        peq->b[1] = 2.0 * (K * K - 1.0) * norm;
        peq->b[2] = (1.0 - K / peq->Q + K * K) * norm;
      break;
    case 7: // highpass
        norm = 1.0 / (1.0 + K / peq->Q + K * K);
        peq->a[0] = 1.0 * norm;
        peq->a[1] = -2.0 * peq->a[0];
        peq->a[2] = peq->a[0];
        peq->b[1] = 2.0 * (K * K - 1.0) * norm;
        peq->b[2] = (1.0 - K / peq->Q + K * K) * norm;
      break;
  }
}

float fxCalcFrequencyResponse_LC(float f, float fc, float fs) {
  float numerator = f / fc;
  return 10.0f * log10( numerator / sqrt(1 + numerator * numerator) );
}

float fxCalcFrequencyResponse_HC(float f, float fc, float fs) {
  return 10.0f * log10( 1.0f/sqrt( 1.0f + pow(f/fc, 2.0f) ) );
}

float fxCalcFrequencyResponse_PEQ(float a0, float a1, float a2,  float b1, float b2, float f, float fs) {
  float omega = (2.0f * PI * f) / fs;
  float phi = (1.0f - cosf(omega)) * 0.5f;
  float p_phi = phi * phi;

  float num_sum = a0 + a1 + a2;
  float num_term2 = a0*a1 + 4.0f*a0*a2 + a1*a2;
  float num_term3 = a0*a2;
  float numerator = (num_sum * num_sum) - (4.0f * num_term2 * phi) + (16.0f * num_term3 * p_phi);

  float b0 = 1.0f;
  float den_sum = b0 + b1 + b2;
  float den_term2 = b0*b1 + 4.0f*b0*b2 + b1*b2;
  float den_term3 = b0*b2;
  float denominator = (den_sum * den_sum) - (4.0f * den_term2 * phi) + (16.0f * den_term3 * p_phi);

  return 10.0f * log10f(numerator / denominator);

/*
  // this is the condensed form of the above code, but not optimized for speed
	float omega = (2.0f * PI * f)/fs;
	float phi = pow(sin(omega / 2.0f), 2.0f);
	float b0 = 1.0f;
	
	return ( 10.0f * log10( pow(a0+a1+a2,2.0f) - 4.0f*(a0*a1 + 4.0f*a0*a2 + a1*a2) * phi + 16.0f*a0*a2*pow(phi,2.0f) ) - 10.0f * log10( pow(b0+b1+b2, 2.0f) - 4.0f*(b0*b1 + 4.0f*b0*b2 + b1*b2)*phi + 16.0f*b0*b2*pow(phi, 2.0f) ) );
	//return 20*Math.log((Math.sqrt(square(a0*square(Math.cos(w))-a0*square(Math.sin(w))+a1*Math.cos(w)+a2)+square(2*a0*Math.cos(w)*Math.sin(w)+a1*(Math.sin(w)))) / Math.sqrt(square(square(Math.cos(w))-   square(Math.sin(w))+b1*Math.cos(w)+b2)+square(2*   Math.cos(w)*Math.sin(w)+b1*(Math.sin(w))))))
*/
}

void fxRecalcFilterCoefficients_LR12(sLR12* LR12) {
  double wc = 2.0 * PI * LR12->fc;
  double wc2 = wc * wc;
  double wc22 = 2 * wc2;
  double k = wc / tan(PI * (LR12->fc / config.GetSamplerate()));
  double k2 = k * k;
  double k22 = 2 * k2;
  double wck2 = 2 * wc * k;
  double norm = (k2 + wc2 + wck2);

  if (LR12->isHighpass) {
    // coefficients for HighPass-Filter
    LR12->a[0] = k2 / norm;
    LR12->a[1] = -k22 / norm;
    LR12->a[2] = k2 / norm;
  }else{
    // coefficients for LowPass-Filter
    LR12->a[0] = wc2 / norm;
    LR12->a[1] = wc22 / norm;
    LR12->a[2] = wc2 / norm;
  }

  LR12->b[0] = 0; // we are not using this coefficient but keep it to not confuse with indices
  LR12->b[1] = (-k22 + wc22) / norm;
  LR12->b[2] = (-wck2 + k2 + wc2) / norm;
}

void fxRecalcFilterCoefficients_LR24(sLR24* LR24) {
  double wc = 2.0 * PI * LR24->fc;
  double wc2 = wc * wc;
  double wc3 = wc2 * wc;
  double wc4 = wc2 * wc2;
  double k = wc / tan(PI * (LR24->fc / config.GetSamplerate()));
  double k2 = k * k;
  double k3 = k2 * k;
  double k4 = k2 * k2;
  double sq_tmp1 = sqrt(2.0) * wc3 * k;
  double sq_tmp2 = sqrt(2.0) * wc * k3;
  double norm = 4.0 * wc2 * k2 + 2.0 * sq_tmp1 + k4 + 2.0 * sq_tmp2 + wc4;

  if (LR24->isHighpass) {
    // coefficients for HighPass-Filter
    LR24->a[0] = k4 / norm;
    LR24->a[1] = -4.0 * k4 / norm;
    LR24->a[2] = 6.0 * k4 / norm;
    LR24->a[3] = LR24->a[1];
    LR24->a[4] = LR24->a[0];
  }else{
    // coefficients for LowPass-Filter
    LR24->a[0] = wc4 / norm;
    LR24->a[1] = 4.0 * wc4 / norm;
    LR24->a[2] = 6.0 * wc4 / norm;
    LR24->a[3] = LR24->a[1];
    LR24->a[4] = LR24->a[0];
  }

  LR24->b[0] = 0; // we are not using this coefficient but keep it to not confuse with indices
  LR24->b[1] = (4.0 * (wc4 + sq_tmp1 - k4 - sq_tmp2)) / norm;
  LR24->b[2] = (6.0 * wc4 - 8.0 * wc2 * k2 + 6.0 * k4) / norm;
  LR24->b[3] = (4.0 * (wc4 - sq_tmp1 + sq_tmp2 - k4)) / norm;
  LR24->b[4] = (k4 - 2.0 * sq_tmp1 + wc4 - 2.0 * sq_tmp2 + 4.0 * wc2 * k2) / norm;
}

void fxRecalcGate(sGate* gate) {
	float samplerate = config.GetSamplerate()/(float)DSP_SAMPLES_IN_BUFFER;

	gate->value_threshold = (pow(2.0f, 31.0f) - 1.0f) * pow(10.0f, gate->threshold/20.0f);

        // range of 60dB means that we will reduce the signal on active gate by 60dB. We have to convert logarithmic dB-value into linear value for gain
	gate->value_gainmin = 1.0f / pow(10.0f, gate->range/20.0f);

        // to get a smooth behaviour, we will use a low-pass with a damping to get 10%/90% changes within the desired time
        // ln(10%) - ln(90%) = -2.197224577
	gate->value_coeff_attack = exp(-2197.22457734f/(samplerate * gate->attackTime_ms));
	gate->value_hold_ticks = gate->holdTime_ms * samplerate / 1000.0f;
	gate->value_coeff_release = exp(-2197.22457734f/(samplerate * gate->releaseTime_ms));
}

void fxRecalcCompressor(sCompressor* compressor) {
	float samplerate = config.GetSamplerate()/(float)DSP_SAMPLES_IN_BUFFER;

	compressor->value_threshold = (pow(2.0f, 31.0f) - 1.0f) * pow(10.0f, compressor->threshold/20.0f);
        compressor->value_ratio = compressor->ratio;
	compressor->value_makeup = pow(10.0f, compressor->makeup/20.0f);

        // to get a smooth behaviour, we will use a low-pass with a damping to get 10%/90% changes within the desired time
        // ln(10%) - ln(90%) = -2.197224577
	compressor->value_coeff_attack = exp(-2197.22457734f/(samplerate * compressor->attackTime_ms));
	compressor->value_hold_ticks = compressor->holdTime_ms * samplerate / 1000.0f;
	compressor->value_coeff_release = exp(-2197.22457734f/(samplerate * compressor->releaseTime_ms));
}
