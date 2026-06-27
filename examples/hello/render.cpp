#include "Pepper.h"
#include "meter.h"

// No audio is produced; this is purely an input-visualisation demo.
class Hello final : public Pepper<Hello> {
    static const int kKnobs = 8;
    static const int kLeds  = 10;
    static constexpr float kMoveThreshold = 0.02f;

    float ref_[kKnobs] = {};
    int   active_ = 1;          // 1-based knob currently displayed

public:
    void control() {
        // Read all knobs and find the one currently being turned. trackActiveKnob
        // accumulates slow turns (per-frame deltas are far below threshold) and
        // updates ref_ in place, so we don't snapshot prev each frame.
        float cur[kKnobs];
        for (int i = 0; i < kKnobs; ++i) cur[i] = pot(i + 1);
        int moved = trackActiveKnob(cur, ref_, kKnobs, kMoveThreshold);
        if (moved) active_ = moved;

        // Buttons override the bar meter while held.
        bool anyButton = false;
        for (int i = 1; i <= 4; ++i) if (button(i)) anyButton = true;

        for (int i = 1; i <= kLeds; ++i) led(i, false);
        if (anyButton) {
            for (int i = 1; i <= 4; ++i) led(i, button(i));   // Button i -> LED i
        } else {
            int lit = meterCount(cur[active_ - 1], kLeds);
            for (int i = 1; i <= lit; ++i) led(i, true);
        }
    }
};

PEPPER_MAIN(Hello)
