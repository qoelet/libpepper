#include "doctest.h"
#include "Bela.h"
#include "libraries/Scope/Scope.h"
#include "../Pepper.h"

TEST_CASE("scope is disabled by default: setup does not configure a Scope") {
  scopeProbe() = ScopeProbe{};
  BelaContext c;
  struct Card : Pepper<Card> {
    using Pepper::Pepper;
  };
  Card card;
  REQUIRE(card._setup(&c) == true);
  CHECK(scopeProbe().setupCalled == false);
}

TEST_CASE("scopeChannels > 0 configures the Scope at control rate") {
  scopeProbe() = ScopeProbe{};
  BelaContext c;
  c.analogSampleRate = 22050.f;
  PepperConfig cfg;
  cfg.scopeChannels = 2;
  struct Card : Pepper<Card> {
    using Pepper::Pepper;
  };
  Card card(cfg);
  REQUIRE(card._setup(&c) == true);
  CHECK(scopeProbe().setupCalled == true);
  CHECK(scopeProbe().channels == 2);
  CHECK(scopeProbe().rate == doctest::Approx(22050.f)); // analog/control rate
}

TEST_CASE("scope() forwards channel values to Scope::log when enabled") {
  scopeProbe() = ScopeProbe{};
  BelaContext c;
  PepperConfig cfg;
  cfg.scopeChannels = 2;
  struct Card : Pepper<Card> {
    using Pepper::Pepper;
    void control() { scope(0.25f, 0.75f); }
  };
  Card card(cfg);
  REQUIRE(card._setup(&c) == true);
  card._render(&c);
  CHECK(scopeProbe().logCount > 0);
  CHECK(scopeProbe().last[0] == doctest::Approx(0.25f));
  CHECK(scopeProbe().last[1] == doctest::Approx(0.75f));
}

TEST_CASE("scope() is a no-op when disabled") {
  scopeProbe() = ScopeProbe{};
  BelaContext c;
  struct Card : Pepper<Card> {
    using Pepper::Pepper;
    void control() { scope(1.f, 2.f); }
  };
  Card card; // scopeChannels defaults to 0
  REQUIRE(card._setup(&c) == true);
  card._render(&c);
  CHECK(scopeProbe().logCount == 0);
}
