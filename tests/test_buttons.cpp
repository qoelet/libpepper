#include "doctest.h"
#include "Bela.h"
#include "../Pepper.h"

struct BtnProbe : Pepper {
    using Pepper::Pepper;
    bool lvl(int i)  { return button(i); }
    bool rose(int i) { return buttonRose(i); }
    bool fell(int i) { return buttonFell(i); }
    void latch()     { latchButtons(); }
};

// Set raw level of Btn i (1..4) in digital frame 0.
// Button i -> digital channel {15,14,13,12}.
static void setBtn(BelaContext& c, int i, bool pressed) {
    static const int kBtnCh[4] = {15, 14, 13, 12};
    c.digital[0 * 16 + kBtnCh[i - 1]] = pressed ? 1.f : 0.f;
}

TEST_CASE("button flips only after debounceFrames consecutive blocks") {
    BelaContext c;                       // debounceFrames=2
    BtnProbe p; p._setup(&c);
    CHECK(p.lvl(1) == false);
    setBtn(c, 1, true);
    p.latch();                           // block 1: 1 of 2
    CHECK(p.lvl(1) == false);
    p.latch();                           // block 2: 2 of 2 -> flips
    CHECK(p.lvl(1) == true);
    CHECK(p.rose(1) == true);
}

TEST_CASE("buttonRose/Fell are only true on the flip block") {
    BelaContext c;
    BtnProbe p; p._setup(&c);
    setBtn(c, 2, true);
    p.latch(); p.latch();                // flips up
    CHECK(p.rose(2) == true);
    p.latch();                           // stable high
    CHECK(p.rose(2) == false);
    CHECK(p.lvl(2) == true);
    setBtn(c, 2, false);
    p.latch(); p.latch();                // flips down
    CHECK(p.fell(2) == true);
    CHECK(p.lvl(2) == false);
}

TEST_CASE("a single-block glitch does not flip the button") {
    BelaContext c;
    BtnProbe p; p._setup(&c);
    setBtn(c, 3, true);
    p.latch();                           // 1 of 2
    setBtn(c, 3, false);                 // glitch cleared
    p.latch();                           // counter resets
    CHECK(p.lvl(3) == false);
    CHECK(p.rose(3) == false);
}
