#include "Pepper.h"
#include "pulser.h"

class Pulser final : public Pepper<Pulser> {
  PulseGen gen_;

public:
  void control() {
    bool high = gen_.tick(potToRate(pot(1)), controlRate(), buttonRose(1));
    cvOutV(1, high ? 5.f : 0.f);
    led(1, high);
  }
};

PEPPER_MAIN(Pulser)
