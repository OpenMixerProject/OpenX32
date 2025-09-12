#include "fx.h"
#include "dsp.h"

void fxRecalcFilterCoefficients_PEQ(sPEQ* peq) {
  // Online-Calculator: https://www.earlevel.com/main/2021/09/02/biquad-calculator-v3
  // Source: https://www.earlevel.com/main/2012/11/26/biquad-c-source-code
  double V = pow(10.0, fabs(peq->gain)/20.0);
  double K = tan(PI * peq->fc / dsp.samplerate);
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

void fxRecalcFilterCoefficients_LR12(sLR12* LR12) {
  double wc = 2.0 * PI * LR12->fc;
  double wc2 = wc * wc;
  double wc22 = 2 * wc2;
  double k = wc / tan(PI * (LR12->fc / dsp.samplerate));
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
  double k = wc / tan(PI * (LR24->fc / dsp.samplerate));
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
	gate->value_threshold = (pow(2.0f, 31.0f) - 1) * pow(10.0f, gate->threshold/20.0f);

        // range of 60dB means that we will reduce the signal on active gate by 60dB. We have to convert logarithmic dB-value into linear value for gain
	gate->value_gainmin = 1.0f / pow(10.0f, gate->range/20.0f);
	gate->value_coeff_attack = exp(-2197.22457734f/(dsp.samplerate * gate->attackTime_ms));
	gate->value_hold_ticks = gate->holdTime_ms * (dsp.samplerate / 1000.0f);
	gate->value_coeff_release = exp(-2197.22457734f/(dsp.samplerate * gate->releaseTime_ms));
}

void fxRecalcCompressor(sCompressor* compressor) {
	compressor->value_threshold = (pow(2, 31) - 1) * pow(10.0f, compressor->threshold/20.0f);

	compressor->value_makeup = pow(10.0f, compressor->makeup/20.0f);
	compressor->value_coeff_attack = exp(-2197.22457734f/(dsp.samplerate * compressor->attackTime_ms));
	compressor->value_hold_ticks = compressor->holdTime_ms * (dsp.samplerate / 1000.0f);
	compressor->value_coeff_release = exp(-2197.22457734f/(dsp.samplerate * compressor->releaseTime_ms));
}
