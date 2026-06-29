#pragma once

static const int INPUT = 0;
static const int OUTPUT = 1;

static inline float constrain(float x, float lo, float hi) {
  return x < lo ? lo : (x > hi ? hi : x);
}

static inline float map(float x, float a, float b, float c, float d) {
  return c + (x - a) * (d - c) / (b - a);
}
