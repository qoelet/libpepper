#include "doctest.h"
#include "Bela.h"
#include "../Pepper.h"

struct AudioProbe : Pepper {
    using Pepper::Pepper;
    float in(int ch)            { return audioIn(ch); }
    void  out(int ch, float v)  { audioOut(ch, v); }
};

TEST_CASE("audioIn reads 1-based channel at frame 0") {
    BelaContext c;                       // audioInChannels = 2
    c.audioIn[0 * 2 + 0] = -0.5f;
    c.audioIn[0 * 2 + 1] =  0.25f;
    AudioProbe p; p._setup(&c);
    CHECK(p.in(1) == doctest::Approx(-0.5f));
    CHECK(p.in(2) == doctest::Approx(0.25f));
}

TEST_CASE("audioOut writes 1-based channel at frame 0 and clamps to -1..1") {
    BelaContext c;
    AudioProbe p; p._setup(&c);
    p.out(1, 0.8f);
    p.out(2, 2.0f);                      // clamps to 1.0
    CHECK(c.audioOut[0 * 2 + 0] == doctest::Approx(0.8f));
    CHECK(c.audioOut[0 * 2 + 1] == doctest::Approx(1.0f));
}
