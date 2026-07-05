#include "doctest.h"
#include "../examples/scope/envelope.h"

TEST_CASE("DecayEnv holds at 1.0 while gated, then decays by coeff") {
  DecayEnv env;

  CHECK_EQ(env.tick(true, 0.5f), doctest::Approx(1.f)); // gated -> 1.0
  CHECK_EQ(env.tick(true, 0.5f), doctest::Approx(1.f)); // still gated
  CHECK_EQ(env.tick(false, 0.5f), doctest::Approx(0.5f)); // released -> *0.5
  CHECK_EQ(env.tick(false, 0.5f), doctest::Approx(0.25f));
}

TEST_CASE("potToDecayTime maps knob across a decay range") {
  CHECK(potToDecayTime(0.f) > 0.f); // always a usable minimum
  CHECK(potToDecayTime(1.f) > potToDecayTime(0.f)); // knob up = longer decay
}

TEST_CASE("decayCoeff is in (0,1) and longer decay -> closer to 1") {
  float rate = 1000.f;
  float fast = decayCoeff(0.01f, rate);
  float slow = decayCoeff(1.0f, rate);
  CHECK(fast > 0.f);
  CHECK(slow < 1.f);
  CHECK(slow > fast);
  // when decaySec * rate == 1, level falls to ~e^-1 after one time-constant
  CHECK_EQ(decayCoeff(1.f, 1.f), doctest::Approx(0.3679f).epsilon(0.001));
}
