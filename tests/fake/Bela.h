#pragma once

#include <vector>
#include "Utilities.h"

// Fake Bela context for host-side unit tests. Sized for Bela Pepper:
// digital channels 0-15 (LEDs + buttons), analog channels 0-7 (CV in/out).
struct BelaContext {
  unsigned int audioFrames = 8;
  unsigned int analogFrames = 4;
  unsigned int digitalFrames = 8;
  unsigned int audioInChannels = 2;
  unsigned int audioOutChannels = 2;
  unsigned int analogInChannels = 8;
  unsigned int analogOutChannels = 8;
  unsigned int digitalChannels = 16;
  float audioSampleRate = 44100.f;
  float analogSampleRate = 22050.f;
  std::vector<float> audioIn = std::vector<float>(audioInChannels * audioFrames, 0.f);
  std::vector<float> audioOut = std::vector<float>(audioOutChannels * audioFrames, 0.f);
  std::vector<float> analogIn = std::vector<float>(analogInChannels * analogFrames, 0.f);
  std::vector<float> analogOut = std::vector<float>(analogOutChannels * analogFrames, 0.f);
  // digital[frame*digitalChannels + channel] = level (0/1). digitalDir tracks pinMode.
  std::vector<float> digital = std::vector<float>(digitalChannels * digitalFrames, 0.f);
  std::vector<int> digitalDir = std::vector<int>(digitalChannels, INPUT);
};

static inline float analogRead(BelaContext* c, int f, int ch) {
  return c->analogIn[f * c->analogInChannels + ch];
}

static inline void analogWrite(BelaContext* c, int f, int ch, float v) {
  c->analogOut[f * c->analogOutChannels + ch] = v;
}

static inline void analogWriteOnce(BelaContext* c, int f, int ch, float v) {
  analogWrite(c, f, ch, v);
}

static inline float audioRead(BelaContext* c, int f, int ch) {
  return c->audioIn[f * c->audioInChannels + ch];
}

static inline void audioWrite(BelaContext* c, int f, int ch, float v) {
  c->audioOut[f * c->audioOutChannels + ch] = v;
}

static inline float digitalRead(BelaContext* c, int f, int ch) {
  return c->digital[f * c->digitalChannels + ch];
}

static inline void digitalWrite(BelaContext* c, int f, int ch, float v) {
  c->digital[f * c->digitalChannels + ch] = v;
}

static inline void pinMode(BelaContext* c, int /*f*/, int ch, int mode) {
  c->digitalDir[ch] = mode;
}
