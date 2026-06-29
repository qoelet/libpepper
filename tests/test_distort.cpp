#include "doctest.h"
#include "../examples/distortion/distort.h"

TEST_CASE("At zero, hardClip is just identity") {
  float amount = 0.f;

  CHECK_EQ(hardClip(-0.7f, amount), doctest::Approx(-0.7f));
  CHECK_EQ(hardClip(0.0f, amount), doctest::Approx(0.0f));
  CHECK_EQ(hardClip(0.5f, amount), doctest::Approx(0.5f));
}

TEST_CASE("At full, hardClip drives the sample with clamping") {
  float amount = 1.0f;

  CHECK_EQ(hardClip(0.1f, amount), doctest::Approx(0.4f));
  CHECK_EQ(hardClip(0.5f, amount), doctest::Approx(1.0f));
  CHECK_EQ(hardClip(-0.5f, amount), doctest::Approx(-1.f));
}

TEST_CASE("At zero, softClip is just identity") {
  float amount = 0.f;

  CHECK_EQ(softClip(-0.7f, amount), doctest::Approx(-0.7f));
  CHECK_EQ(softClip(0.0f, amount), doctest::Approx(0.0f));
  CHECK_EQ(softClip(0.5f, amount), doctest::Approx(0.5f));
}

TEST_CASE("At full, softClip drives the sample with clamping") {
  float amount = 1.0f;

  CHECK_EQ(softClip(1.0f, amount), doctest::Approx(1.f));
  CHECK_EQ(softClip(0.5f, amount), doctest::Approx(0.964675f));
}

TEST_CASE("At zero, cubic is just identity") {
  float amount = 0.f;

  CHECK_EQ(cubic(-0.7f, amount), doctest::Approx(-0.7f));
  CHECK_EQ(cubic(0.0f, amount), doctest::Approx(0.0f));
  CHECK_EQ(cubic(0.5f, amount), doctest::Approx(0.5f));
}

TEST_CASE("At full, cubic drives the sample with clamping") {
  float amount = 1.0f;

  CHECK_EQ(cubic(0.1f, amount), doctest::Approx(0.568f));
  CHECK_EQ(cubic(0.5f, amount), doctest::Approx(1.f));
  CHECK_EQ(cubic(-0.1f, amount), doctest::Approx(-0.568f));
}

TEST_CASE("At zero, foldback is just identity") {
  float amount = 0.f;

  CHECK_EQ(foldback(-0.7f, amount), doctest::Approx(-0.7f));
  CHECK_EQ(foldback(0.0f, amount), doctest::Approx(0.0f));
  CHECK_EQ(foldback(0.5f, amount), doctest::Approx(0.5f));
}

TEST_CASE("At full, foldback drives the sample with clamping") {
  float amount = 1.0f;

  CHECK_EQ(foldback(0.1f, amount), doctest::Approx(0.4f));
  CHECK_EQ(foldback(0.5f, amount), doctest::Approx(0.f));
  CHECK_EQ(foldback(0.375f, amount), doctest::Approx(0.5f));
}

TEST_CASE("Dispatch distort() against given type") {
  CHECK(distort(distortionType::hard, 0.3f, 0.7f) == doctest::Approx(hardClip(0.3f, 0.7f)));
  CHECK(distort(distortionType::soft, 0.3f, 0.7f) == doctest::Approx(softClip(0.3f, 0.7f)));
  CHECK(distort(distortionType::cubic, 0.3f, 0.7f) == doctest::Approx(cubic(0.3f, 0.7f)));
  CHECK(distort(distortionType::fold, 0.3f, 0.7f) == doctest::Approx(foldback(0.3f, 0.7f)));
}
