#include "Pepper.h"
#include "pulser.h"
#include "envelope.h"

class ScopeDemo final : public Pepper<ScopeDemo> {
public:
  ScopeDemo() : Pepper<ScopeDemo>(makeCfg()) {}

  void control() {
    bool gate = gen_.tick(potToRate(pot(1)), controlRate(), buttonRose(1));
    float e = env_.tick(gate, decayCoeff(potToDecayTime(pot(2)), controlRate()));

    cvOutV(1, e * 5.f); // envelope out, 0–5V
    led(1, gate); // gate on LED 1
    scope(gate ? 1.f : 0.f, e); // ch0 = gate, ch1 = envelope
  }

private:
  static PepperConfig makeCfg() {
    PepperConfig c;
    c.scopeChannels = 2;

    return c;
  }
  PulseGen gen_;
  DecayEnv env_;
};

PEPPER_MAIN(ScopeDemo)
