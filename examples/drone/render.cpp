#include <cmath>
#include <cstdlib>
#include "Pepper.h"
#include "meter.h"

class Drone : public Pepper {
    static constexpr float kFreqMin   = 55.f;   // knob 1 / random low
    static constexpr float kFreqMax   = 440.f;  // knob 1 / random high
    static constexpr float kDetuneMax = 0.03f;  // knob 2 full -> 3% detune
    static const int kKnobs = 2;                // knob 1 -> pitch, knob 2 -> detune
    static const int kLeds  = 10;
    static constexpr float kMoveThreshold = 0.02f;

    float phase = 0.f, phase2 = 0.f, freq = 110.f, detune = 1.f;
    float ref_[kKnobs] = {};
    int   active_ = 1;          // 1-based knob currently displayed
    bool  randomFreq = false;   // button 1 latched a random pitch

    void control() override {
        // knob 1 -> pitch, knob 2 -> detune amount (beat speed)
        float cur[kKnobs];
        for (int i = 0; i < kKnobs; ++i) cur[i] = pot(i + 1);
        detune = 1.f + kDetuneMax * cur[1];

        // LED bar follows whichever knob is currently being turned.
        int moved = trackActiveKnob(cur, ref_, kKnobs, kMoveThreshold);
        if (moved) active_ = moved;
        int lit = meterCount(cur[active_ - 1], kLeds);
        for (int i = 1; i <= kLeds; ++i) led(i, i <= lit);

        // Button 1 latches a random pitch; turning knob 1 hands control back.
        if (buttonRose(1)) {
            freq = kFreqMin + (kFreqMax - kFreqMin) * (rand() / (float)RAND_MAX);
            randomFreq = true;
        }
        if (moved == 1) randomFreq = false;
        if (!randomFreq) freq = kFreqMin + (kFreqMax - kFreqMin) * cur[0];

        // ramp out on CVout1, detuned ramp on CVout2 -> beating
        cvOut(1, phase);
        cvOut(2, phase2);
    }

    void audio() override {
        float s = sinf(phase * 2.f * (float)M_PI);
        audioOut(1, s);
        audioOut(2, s);
        phase  += freq / audioRate();
        phase2 += freq * detune / audioRate();
        if (phase  >= 1.f) phase  -= 1.f;
        if (phase2 >= 1.f) phase2 -= 1.f;
    }
};

PEPPER_MAIN(Drone)
