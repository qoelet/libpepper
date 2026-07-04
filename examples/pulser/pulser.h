#pragma once

#include <cmath>

static constexpr float kPulseWidthSec = 0.005f;
static constexpr float kRateMax = 8.f; // Hz at full
static constexpr float kPotThreshold = 0.02f;

inline float potToRate(float pot) {
  if (pot < kPotThreshold) {
    return 0.f;
  }

  return kRateMax * pot;
}

struct PulseGen {
  bool tick(float rateHz, float controlRate, bool fire) {
    phase_ += rateHz / controlRate;

    if (phase_ >= 1.f) {
      phase_ -= 1.f;
      fire = true;
    }

    if (fire) {
      gateFrames_ = round(controlRate * kPulseWidthSec);
    }

    bool high = gateFrames_ > 0;

    if (gateFrames_ > 0) {
      gateFrames_--;
    }

    return high;
  }

private:
  float phase_ = 0.f; // clock
  int gateFrames_ = 0; // pulse-width countdown
};
