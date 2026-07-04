#pragma once
// Fake Bela Scope for host tests. Mirrors the subset of the real Bela
// `Scope` API that libpepper uses (`setup(numChannels, sampleRate)` and
// `log(const float* values)`), and records calls into an inspectable probe
// so tests can assert on them.

struct ScopeProbe {
  bool setupCalled = false;
  int channels = 0;
  float rate = 0.f;
  int logCount = 0;
  float last[16] = {};
};

inline ScopeProbe& scopeProbe() {
  static ScopeProbe p;
  return p;
}

class Scope {
public:
  void setup(int numChannels, float sampleRate) {
    ScopeProbe& p = scopeProbe();
    p.setupCalled = true;
    p.channels = numChannels;
    p.rate = sampleRate;
  }
  void log(const float* values) {
    ScopeProbe& p = scopeProbe();
    p.logCount++;
    for (int i = 0; i < p.channels && i < 16; ++i) p.last[i] = values[i];
  }
};
