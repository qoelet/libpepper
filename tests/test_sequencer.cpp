#include "doctest.h"
#include "Bela.h"
#include "../examples/euclidean-sequence/EuclidSeq.h"

struct Probe : EuclidSeq {
  const Seq& pattern() const { return pattern_; }
  int step() const { return currentStep_; }
};

TEST_CASE("Pots quantize to E(3, 8)") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 0.40f; // pot 1 (k) = 3
    c.analogIn[f * 8 + 1] = 0.90f; // pot 2 (4) = 8
  }

  Probe seq;
  REQUIRE(seq._setup(&c));
  seq._render(&c);
  const bool* p = seq.pattern().sequence_;
  bool expected[8] = {1, 0, 0, 1, 0, 0, 1, 0};

  for (int i = 0; i < 8; ++i) {
    CHECK(p[i] == expected[i]);
  }
}

TEST_CASE("Clock advances playhead at tempo") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 0.40f; // k = 3
    c.analogIn[f * 8 + 1] = 0.90f; // n = 8
    c.analogIn[f * 8 + 2] = 1.00f; // tempo = max (16 steps/sec)
  }

  Probe seq;
  REQUIRE(seq._setup(&c));
  for (int b = 0; b < 500; ++b) {
    seq._render(&c);
  }

  CHECK(seq.step() == 1); // ~1.45 steps
}

TEST_CASE("Outputs a trigger pulse") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 1.0f; // k = n
    c.analogIn[f * 8 + 1] = 0.90f; // n = 8
    c.analogIn[f * 8 + 2] = 1.00f; // tempo = max (16 steps/sec)
  }

  Probe seq;
  REQUIRE(seq._setup(&c));
  bool high = false, low = false;
  for (int b = 0; b < 500; ++b) {
    seq._render(&c);
    if (c.analogOut[0] > 0.5f) {
      high = true;
    }
    if (c.analogOut[0] < 0.5f) {
      low = true;
    }
  }

  CHECK(high);
  CHECK(low);
}
