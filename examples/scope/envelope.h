#pragma once

#include <cmath>

static constexpr float kDecayMinSec = 0.02f; // 20 ms at knob 0
static constexpr float kDecayMaxSec = 2.0f; // 2 s at knob full

inline float potToDecayTime(float pot) {
  return kDecayMinSec + (kDecayMaxSec - kDecayMinSec) * pot;
}

inline float decayCoeff(float decaySec, float controlRate) {
  return expf(-1.f / (decaySec * controlRate));
}

struct DecayEnv {
  float tick(bool gate, float coeff) {
    if (gate) {
      level_ = 1.f;
    } else {
      level_ *= coeff;
    }

    return level_;
  }

private:
  float level_ = 0.f;
};
