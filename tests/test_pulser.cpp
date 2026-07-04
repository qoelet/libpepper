#include "doctest.h"
#include "Bela.h"
#include "../examples/pulser/pulser.h"

TEST_CASE("Fire holds the gate high for the duration of pulse width") {
  PulseGen gen;

  CHECK_EQ(gen.tick(0, 1000, true), true);
  for (int i = 0; i < 4; ++i) {
    CHECK_EQ(gen.tick(0, 1000, false), true);
  }
  CHECK_EQ(gen.tick(0, 1000, false), false);
}

TEST_CASE("Clock fires at expected interval") {
  PulseGen gen;

  CHECK_EQ(gen.tick(250, 1000, false), false); // phase = 0.25
  CHECK_EQ(gen.tick(250, 1000, false), false); // 0.5
  CHECK_EQ(gen.tick(250, 1000, false), false); // 0.75
  CHECK_EQ(gen.tick(250, 1000, false), true);
}

TEST_CASE("potToRate") {
  CHECK_EQ(potToRate(0.f), 0.f);
  CHECK_EQ(potToRate(0.01f), 0.f);
  CHECK(potToRate(0.5f) > 0.f);
  CHECK_EQ(potToRate(1.f), doctest::Approx(8.f));
}
