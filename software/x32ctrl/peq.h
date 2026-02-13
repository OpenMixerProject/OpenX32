#pragma once

class sPEQ
{
    public:

    // user-settings
        uint type; // 0=allpass, 1=peak, 2=low-shelf, 3=high-shelf, 4=bandpass, 5=notch, 6=lowpass, 7=highpass
        float fc; // center-frequency of PEQ
        float Q; // Quality of PEQ (bandwidth)
        float gain; // gain of PEQ

        // filter-coefficients
        float a[3];
        float b[3];
};