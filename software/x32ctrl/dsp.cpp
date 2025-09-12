#include "dsp.h"
#include "mixer.h"

sDsp dsp;

void dspInit(void) {
    dsp.samplerate = 48000;

    for (uint8_t i = 0; i < 40; i++) {
        dsp.dspChannel[i].gate.threshold = -80;
        dsp.dspChannel[i].gate.range = 60;
        dsp.dspChannel[i].gate.attackTime_ms = 10;
        dsp.dspChannel[i].gate.holdTime_ms = 50;
        dsp.dspChannel[i].gate.releaseTime_ms = 250;

        dsp.dspChannel[i].compressor.threshold = 0;
        dsp.dspChannel[i].compressor.ratio = 3;
        dsp.dspChannel[i].compressor.makeup = 0;
        dsp.dspChannel[i].compressor.attackTime_ms = 10;
        dsp.dspChannel[i].compressor.holdTime_ms = 10;
        dsp.dspChannel[i].compressor.releaseTime_ms = 150;

        for (uint8_t peq = 0; peq < MAX_CHAN_EQS; peq++) {
            dsp.dspChannel[i].peq[peq].type = 1;
            dsp.dspChannel[i].peq[peq].fc = 3000;
            dsp.dspChannel[i].peq[peq].Q = 2.0;
            dsp.dspChannel[i].peq[peq].gain = 0;
        }
    }
}
