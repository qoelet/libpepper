#pragma once

#include <cmath>

constexpr float DRIVE = 4.f;

static inline float clamp1(float x) {
  return x < -1.f ? -1.f : (x > 1.f ? 1.f : x);
}

inline float hardClip(float x, float amount) {
  float y = DRIVE * x;
  float wet = clamp1(y);

  return (1.f - amount) * x + amount * wet;
}

inline float softClip(float x, float amount) {
  float wet = tanhf(DRIVE * x) / tanhf(DRIVE);

  return (1.f - amount) * x + amount * wet;
}

inline float cubic(float x, float amount) {
  float y = clamp1(DRIVE * x);
  float wet = 1.5f * y - 0.5f * y * y * y;
  
  return (1.f - amount) * x + amount * wet;
}

inline float foldback(float x, float amount) {
  float y = DRIVE * x;
  while(y > 1.f || y < -1.f) {
    if(y > 1.f) {
      y = 2.f - y;
    }
    if(y < -1.f) {
      y = -2.f - y;
    }
  }

  return (1.f - amount) * x + amount * y;
}

enum class distortionType : int {
  hard = 1,
  soft,
  cubic,
  fold
};

inline float distort(distortionType type, float x, float amount) {
  switch(type) {
    case distortionType::hard: {
      return hardClip(x, amount);
    }
    case distortionType::soft: {
      return softClip(x, amount);
    }
    case distortionType::cubic: {
      return cubic(x, amount);
    }
    case distortionType::fold: {
      return foldback(x, amount);
    }
  }

  return x; // passthrough otherwise
}
