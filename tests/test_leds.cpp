#include "doctest.h"
#include "Bela.h"
#include "../Pepper.h"

struct LedProbe : Pepper {
    using Pepper::Pepper;
    void set(int i, bool on)     { led(i, on); }
    void setLevel(int i, float b){ ledLevel(i, b); }
    void flush()                 { applyLeds(); }
};

TEST_CASE("led writes the mapped digital channel across all digital frames") {
    BelaContext c;                       // digitalChannels=16, digitalFrames=8
    LedProbe p; p._setup(&c);
    p.set(1, true);                      // LED1 -> channel 6
    p.set(10, true);                     // LED10 -> channel 8
    p.flush();
    for (unsigned f = 0; f < c.digitalFrames; ++f) {
        CHECK(c.digital[f * 16 + 6] == doctest::Approx(1.f));   // Led1
        CHECK(c.digital[f * 16 + 8] == doctest::Approx(1.f));   // Led10
        CHECK(c.digital[f * 16 + 10] == doctest::Approx(0.f));  // Led3 untouched
    }
}

TEST_CASE("ledLevel thresholds at 0.5 for v1") {
    BelaContext c;
    LedProbe p; p._setup(&c);
    p.setLevel(1, 0.4f);                 // LED1 -> channel 6
    p.setLevel(2, 0.6f);                 // LED2 -> channel 7
    p.flush();
    CHECK(c.digital[0 * 16 + 6] == doctest::Approx(0.f));
    CHECK(c.digital[0 * 16 + 7] == doctest::Approx(1.f));
}
