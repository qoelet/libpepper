#pragma once

#include "Pepper.h"
#include "euclid.h"

class EuclidSeq : public Pepper<EuclidSeq> {
protected:
  Seq pattern_; // generated rhythm
  int currentStep_ = 0; // playhead
  float phase_ = 0.f; // clock
  int lastK_ = -1, lastN_ = -1; // last pot values
  int pulseRemaining_ = 0;
  static constexpr float kPulseSeconds_ = 0.005f;
public:
  void control() {
    // Read (and clamp) pot values
    int n = 1 + static_cast<int>(pot(2) * Seq::size_);
    n = n > Seq::size_ ? Seq::size_ : n;
    int k = static_cast<int>(pot(1) * (n + 1));
    k = k > n ? n : k;

    bool changed = k != lastK_ || n != lastN_;
    if(changed) {
      euclid(k, n, &pattern_);
      lastK_ = k;
      lastN_ = n;

      if(currentStep_ >= n) {
        currentStep_ %= n; // wrap
      }

      for(int i = 1; i <= Seq::size_; ++i) {
        led(i, (i-1 < n) && pattern_.sequence_[i-1]);
      }
    }

    // Read tempo, advance step
    float stepsPerSecond = 1.f + pot(3) * 15.f;
    phase_ += stepsPerSecond / controlRate();
    if(phase_ >= 1.f) {
      phase_ -= 1.f;
      currentStep_ = (currentStep_ + 1) % n;

      if(pattern_.sequence_[currentStep_]) {
        pulseRemaining_ = static_cast<int>(controlRate() * kPulseSeconds_);
      }
    }

    if(pulseRemaining_ > 0) {
      cvOut(1, 1.f);
      --pulseRemaining_;
    }
    else {
      cvOut(1, 0.f);
    }
  }  
};
