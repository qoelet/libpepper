#include "doctest.h"
#include "../examples/hello/meter.h"

TEST_CASE("meterCount lights LEDs proportional to value (rounded, clamped)") {
    CHECK(meterCount(0.0f, 10) == 0);
    CHECK(meterCount(1.0f, 10) == 10);
    CHECK(meterCount(0.7f, 10) == 7);
    CHECK(meterCount(0.3f, 10) == 3);
    CHECK(meterCount(0.75f, 10) == 8);   // rounds up
    CHECK(meterCount(-0.5f, 10) == 0);   // clamps low
    CHECK(meterCount(1.5f, 10) == 10);   // clamps high
}

TEST_CASE("activeKnob picks the knob that moved past threshold (1-based)") {
    float prev[8] = {0,0,0,0,0,0,0,0};
    float cur[8]  = {0,0,0.5f,0,0,0,0,0};      // knob 3 moved
    CHECK(activeKnob(cur, prev, 8, 0.02f) == 3);
}

TEST_CASE("activeKnob returns 0 when nothing moves past threshold") {
    float prev[8] = {0.1f,0.2f,0.3f,0,0,0,0,0};
    float cur[8]  = {0.105f,0.2f,0.3f,0,0,0,0,0};  // jitter < 0.02
    CHECK(activeKnob(cur, prev, 8, 0.02f) == 0);
}

TEST_CASE("activeKnob: largest move wins when several move") {
    float prev[8] = {0,0,0,0,0,0,0,0};
    float cur[8]  = {0.1f,0,0,0,0.6f,0,0,0};   // knob 1 small, knob 5 big
    CHECK(activeKnob(cur, prev, 8, 0.02f) == 5);
}

TEST_CASE("trackActiveKnob detects a slow turn made of sub-threshold steps") {
    float ref[8] = {0,0,0,0,0,0,0,0};
    float cur[8] = {0,0,0,0,0,0,0,0};
    int detected = 0;
    // knob 4 creeps up by 0.005 per frame, each step below the 0.02 threshold
    for (int step = 0; step < 12; ++step) {
        cur[3] += 0.005f;
        int k = trackActiveKnob(cur, ref, 8, 0.02f);
        if (k) detected = k;
    }
    CHECK(detected == 4);                        // accumulated past threshold
}

TEST_CASE("trackActiveKnob updates only the mover's reference, leaves idle knobs") {
    float ref[8] = {0.3f,0.3f,0.3f,0.3f,0,0,0,0};
    float cur[8] = {0.3f,0.4f,0.3f,0.3f,0,0,0,0};   // knob 2 moved past threshold
    CHECK(trackActiveKnob(cur, ref, 8, 0.02f) == 2);
    CHECK(ref[1] == doctest::Approx(0.4f));         // mover's reference catches up
    CHECK(ref[0] == doctest::Approx(0.3f));         // idle knob reference unchanged
}

TEST_CASE("trackActiveKnob returns 0 and leaves refs untouched on sub-threshold jitter") {
    float ref[8] = {0.1f,0.2f,0,0,0,0,0,0};
    float cur[8] = {0.105f,0.2f,0,0,0,0,0,0};       // jitter < threshold
    CHECK(trackActiveKnob(cur, ref, 8, 0.02f) == 0);
    CHECK(ref[0] == doctest::Approx(0.1f));         // no resync without movement
}
