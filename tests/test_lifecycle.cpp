#include "doctest.h"
#include "Bela.h"
#include "../Pepper.h"

struct BareCard : Pepper<BareCard> { using Pepper::Pepper; };

TEST_CASE("setup stores sample rates from context") {
    BelaContext c;
    c.audioSampleRate = 44100.f;
    c.analogSampleRate = 22050.f;
    BareCard card;
    REQUIRE(card._setup(&c) == true);
    struct Probe : Pepper<Probe> { using Pepper::Pepper;
        float aR(){ return audioRate(); } float cR(){ return controlRate(); } };
    Probe p; p._setup(&c);
    CHECK(p.aR() == doctest::Approx(44100.f));
    CHECK(p.cR() == doctest::Approx(22050.f));
}

TEST_CASE("setup fails when too few analog channels") {
    BelaContext c;
    c.analogInChannels = 4;              // Pepper needs 8
    BareCard card;
    CHECK(card._setup(&c) == false);
}

TEST_CASE("setup fails when too few digital channels for buttons") {
    BelaContext c;
    c.digitalChannels = 8;               // need channels 0..15
    BareCard card;
    CHECK(card._setup(&c) == false);
}

TEST_CASE("setup sets LED pins to OUTPUT and button pins to INPUT") {
    BelaContext c;
    BareCard card;
    REQUIRE(card._setup(&c) == true);
    static const int kLedCh[10] = {6, 7, 10, 2, 3, 0, 1, 4, 5, 8};
    static const int kBtnCh[4]  = {15, 14, 13, 12};
    for (int i = 0; i < 10; ++i) CHECK(c.digitalDir[kLedCh[i]] == OUTPUT);
    for (int i = 0; i < 4;  ++i) CHECK(c.digitalDir[kBtnCh[i]] == INPUT);
}

struct RampCard : Pepper<RampCard> {
    using Pepper::Pepper;
    int controlCalls = 0, audioCalls = 0;
    void control() {
        cvOut(1, 0.1f * cf_);          // distinct value per analog frame
        controlCalls++;
    }
    void audio() {
        audioOut(1, 0.01f * af_);
        audioCalls++;
    }
};

TEST_CASE("render calls control per analog frame and audio per audio frame") {
    BelaContext c;                       // analogFrames=4, audioFrames=8
    RampCard card; REQUIRE(card._setup(&c));
    card._render(&c);
    CHECK(card.controlCalls == 4);
    CHECK(card.audioCalls == 8);
    CHECK(c.analogOut[0 * 8 + 0] == doctest::Approx(0.0f));
    CHECK(c.analogOut[3 * 8 + 0] == doctest::Approx(0.3f));
    CHECK(c.audioOut[0 * 2 + 0] == doctest::Approx(0.0f));
    CHECK(c.audioOut[7 * 2 + 0] == doctest::Approx(0.07f));
}

TEST_CASE("render latches buttons once per block before control runs") {
    BelaContext c;
    struct BtnReader : Pepper<BtnReader> { using Pepper::Pepper;
        bool seen = false;
        void control() { if (button(1)) seen = true; } };
    BtnReader card; REQUIRE(card._setup(&c));
    c.digital[0 * 16 + 15] = 1.f;        // Btn1 pressed (channel 15)
    card._render(&c);                    // block 1: debounce not met
    card._render(&c);                    // block 2: flips, control sees it
    CHECK(card.seen == true);
}
