#include <cmath>
#include "Pepper.h"

class Drone : public Pepper {
    float phase = 0.f, freq = 110.f;

    void control() override {
        freq = 55.f * (1.f + 7.f * pot(1));   // knob 1 -> pitch
        led(1, button(1));
        cvOut(1, phase);                       // ramp LFO out on CVout1
    }
    void audio() override {
        float s = sinf(phase * 2.f * (float)M_PI);
        audioOut(1, s);
        audioOut(2, s);
        phase += freq / audioRate();
        if (phase >= 1.f) phase -= 1.f;
    }
};

PEPPER_MAIN(Drone)
