# libpepper

A small, single-header C++ library for the [Bela Pepper](https://github.com/BelaPlatform/Bela/wiki/Pepper)
Eurorack module.

Usage is simple: You extend from `Pepper`, implement two callbacks — `control()`
(analog/CV rate) and `audio()` (audio rate).

```cpp
#include "Pepper.h"

class Drone : public Pepper<Drone> {
    float phase = 0.f, freq = 110.f;

public:
    void control() {
        freq = 55.f * (1.f + 7.f * pot(1));   // knob 1 -> pitch
        led(1, button(1));
        cvOut(1, phase);                       // ramp LFO out
    }
    void audio() {
        float s = sinf(phase * 2 * M_PI);
        audioOut(1, s);
        audioOut(2, s);
        phase += freq / audioRate();
        if (phase >= 1.f) phase -= 1.f;
    }
};

PEPPER_MAIN(Drone)
```
