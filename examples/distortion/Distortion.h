#pragma once
#include "Pepper.h"
#include "meter.h"
#include "distort.h"

class Distortion : public Pepper<Distortion> {
protected:
  distortionType type_ = distortionType::hard;
  float amount_ = 0.f;
  int flashFrames_ = 0;
  static constexpr float kFlashSeconds = 0.4f;

public:
  void control() {
    amount_ = pot(1);

    bool selected = false;
    if (buttonRose(1)) {
      type_ = distortionType::hard;
      selected = true;
    }
    if (buttonRose(2)) {
      type_ = distortionType::soft;
      selected = true;
    }
    if (buttonRose(3)) {
      type_ = distortionType::cubic;
      selected = true;
    }
    if (buttonRose(4)) {
      type_ = distortionType::fold;
      selected = true;
    }

    if (selected) {
      flashFrames_ = static_cast<int>(controlRate() * kFlashSeconds);
    }

    if (flashFrames_ > 0) {
      int sel = static_cast<int>(type_);
      for (int i = 1; i <= 10; ++i) {
        led(i, i == sel);
      }
      --flashFrames_;
    } else {
      int to_display = meterCount(amount_, 10);
      for (int i = 1; i <= 10; ++i) {
        led(i, i <= to_display);
      }
    }
  }

  void audio() {
    float x = audioIn(1);
    float y = distort(type_, x, amount_);
    audioOut(1, y);
    audioOut(2, y); // dual mono out
  }
};
