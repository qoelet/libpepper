#include "doctest.h"
#include "Bela.h"
#include "../examples/distortion/Distortion.h"

static void setBtn(BelaContext& c, int i, bool pressed) {
  static const int kBtnCh[4] = {15, 14, 13, 12};
  c.digital[0 * 16 + kBtnCh[i - 1]] = pressed ? 1.f : 0.f;
}

struct DistProbe : Distortion {
  distortionType type() const { return type_; }
  float amount() const { return amount_; }
  bool ledOn(int i) const { return ledState_[i - 1]; }
  int flash() const { return flashFrames_; }
};

TEST_CASE("Passthrough at zero") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 0.f; // amount = 0
  }
  for (unsigned f = 0; f < c.audioFrames; ++f) {
    c.audioIn[f * c.audioInChannels + 0] = 0.3f;
    c.audioIn[f * c.audioInChannels + 1] = 0.3f;
  }

  DistProbe d;
  REQUIRE(d._setup(&c));
  d._render(&c);

  CHECK(c.audioOut[0] == doctest::Approx(0.3f));
  CHECK(c.audioOut[1] == doctest::Approx(0.3f));
}

TEST_CASE("Distort at max") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 1.f; // amount = 1
  }
  for (unsigned f = 0; f < c.audioFrames; ++f) {
    c.audioIn[f * c.audioInChannels + 0] = 0.3f;
    c.audioIn[f * c.audioInChannels + 1] = 0.3f;
  }

  DistProbe d;
  REQUIRE(d._setup(&c));
  d._render(&c);

  CHECK(c.audioOut[0] == doctest::Approx(distort(distortionType::hard, 0.3f, 1.f)));
}

TEST_CASE("Buttons selects distortion type") {
  BelaContext c;
  DistProbe d;
  REQUIRE(d._setup(&c));

  setBtn(c, 2, true);
  d._render(&c);
  d._render(&c); // debounce flips on 2nd block

  CHECK(d.type() == distortionType::soft);

  setBtn(c, 3, true);
  d._render(&c);
  d._render(&c);

  CHECK(d.type() == distortionType::cubic);

  setBtn(c, 4, true);
  d._render(&c);
  d._render(&c);

  CHECK(d.type() == distortionType::fold);

  setBtn(c, 1, true);
  d._render(&c);
  d._render(&c);

  CHECK(d.type() == distortionType::hard);
}

TEST_CASE("Display shows distortion amount") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 0.5f;
  }

  DistProbe d;
  REQUIRE(d._setup(&c));
  d._render(&c);

  CHECK(d.ledOn(5) == true);
  CHECK(d.ledOn(6) == false);
}

TEST_CASE("Pressing a button displays the selection") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 1.f; // entire meter is on
  }

  DistProbe d;
  REQUIRE(d._setup(&c));

  setBtn(c, 2, true);
  d._render(&c);
  d._render(&c);

  CHECK(d.ledOn(2) == true); // selection displayed
  CHECK(d.ledOn(1) == false); // and distortion amount is suppressed (for now)
  CHECK(d.ledOn(10) == false);
}

TEST_CASE("Selection expires back to distortion meter") {
  BelaContext c;
  for (unsigned f = 0; f < c.analogFrames; ++f) {
    c.analogIn[f * 8 + 0] = 0.5f;
  }

  DistProbe d;
  REQUIRE(d._setup(&c));

  setBtn(c, 2, true);
  d._render(&c);
  d._render(&c);

  for (int i = 0; i<5000 & d.flash()> 0; ++i) {
    d._render(&c);
  }

  CHECK(d.ledOn(5) == true);
  CHECK(d.ledOn(6) == false);
}
