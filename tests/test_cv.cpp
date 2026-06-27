#include "doctest.h"
#include "Bela.h"
#include "../Pepper.h"

struct CvProbe : Pepper<CvProbe> {
    using Pepper::Pepper;
    float in(int ch)  { return cvIn(ch); }
    float inV(int ch) { return cvInV(ch); }
};

TEST_CASE("cvIn maps 1-based channel to Bela analog input, frame 0") {
    BelaContext c;                    // analogInChannels = 8, analogFrames = 4
    c.analogIn[0 * 8 + 0] = 0.25f;    // CVin1
    c.analogIn[0 * 8 + 7] = 0.9f;     // CVin8
    CvProbe p; p._setup(&c);
    CHECK(p.in(1) == doctest::Approx(0.25f));
    CHECK(p.in(8) == doctest::Approx(0.9f));
}

TEST_CASE("cvInV scales 0..1 to 0..cvInVoltMax") {
    BelaContext c;
    c.analogIn[0 * 8 + 0] = 0.5f;
    CvProbe p; p._setup(&c);
    CHECK(p.inV(1) == doctest::Approx(5.0f));   // 0.5 * 10 V
}

TEST_CASE("cvIn honors a custom cvInPin mapping") {
    PepperConfig cfg; cfg.cvInPin[0] = 2;   // CVin1 -> analog channel 2
    BelaContext c;
    c.analogIn[0 * 8 + 2] = 0.42f;
    CvProbe p(cfg); p._setup(&c);
    CHECK(p.in(1) == doctest::Approx(0.42f));
}

struct CvOutProbe : Pepper<CvOutProbe> {
    using Pepper::Pepper;
    void out(int ch, float v)  { cvOut(ch, v); }
    void outV(int ch, float v) { cvOutV(ch, v); }
};

TEST_CASE("cvOut writes 1-based channel to Bela analog output, frame 0") {
    BelaContext c;
    CvOutProbe p; p._setup(&c);
    p.out(1, 0.3f);
    p.out(8, 0.7f);
    CHECK(c.analogOut[0 * 8 + 0] == doctest::Approx(0.3f));
    CHECK(c.analogOut[0 * 8 + 7] == doctest::Approx(0.7f));
}

TEST_CASE("cvOut clamps out-of-range values to 0..1") {
    BelaContext c;
    CvOutProbe p; p._setup(&c);
    p.out(1, 1.5f);
    p.out(2, -0.4f);
    CHECK(c.analogOut[0 * 8 + 0] == doctest::Approx(1.0f));
    CHECK(c.analogOut[0 * 8 + 1] == doctest::Approx(0.0f));
}

TEST_CASE("cvOutV scales volts to 0..1 by cvOutVoltMax") {
    BelaContext c;
    CvOutProbe p; p._setup(&c);
    p.outV(1, 2.5f);                  // 2.5 / 5 V = 0.5
    CHECK(c.analogOut[0 * 8 + 0] == doctest::Approx(0.5f));
}

TEST_CASE("pot reads the same channel as cvIn") {
    BelaContext c;
    c.analogIn[0 * 8 + 3] = 0.66f;   // channel for CVin4 / Pot4
    CvProbe p; p._setup(&c);
    struct PotProbe : Pepper<PotProbe> { using Pepper::Pepper; float k(int ch){ return pot(ch); } };
    PotProbe pp; pp._setup(&c);
    CHECK(pp.k(4) == doctest::Approx(p.in(4)));
    CHECK(pp.k(4) == doctest::Approx(0.66f));
}
