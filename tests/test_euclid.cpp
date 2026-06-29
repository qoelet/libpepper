#include "doctest.h"
#include "../examples/euclidean-sequence/euclid.h"

TEST_CASE("E(3, 8) must produce x..x..x.") {
  Seq result;

  euclid(3, 8, &result);

  CHECK_EQ(result.sequence_[0], 1);
  CHECK_EQ(result.sequence_[1], 0);
  CHECK_EQ(result.sequence_[2], 0);
  CHECK_EQ(result.sequence_[3], 1);
  CHECK_EQ(result.sequence_[4], 0);
  CHECK_EQ(result.sequence_[5], 0);
  CHECK_EQ(result.sequence_[6], 1);
  CHECK_EQ(result.sequence_[7], 0);
}

TEST_CASE("Every call generates over the entire buffer") {
  Seq result;

  euclid(5, 8, &result);
  euclid(3, 8, &result);

  CHECK_EQ(result.sequence_[0], 1);
  CHECK_EQ(result.sequence_[1], 0);
  CHECK_EQ(result.sequence_[2], 0);
  CHECK_EQ(result.sequence_[3], 1);
  CHECK_EQ(result.sequence_[4], 0);
  CHECK_EQ(result.sequence_[5], 0);
  CHECK_EQ(result.sequence_[6], 1);
  CHECK_EQ(result.sequence_[7], 0);
}

TEST_CASE("Clamps to buffer size when impossible length") {

  Seq result;

  euclid(3, 99, &result);

  CHECK_EQ(result.sequence_[0], 1);
  CHECK_EQ(result.sequence_[1], 0);
  CHECK_EQ(result.sequence_[2], 0);
  CHECK_EQ(result.sequence_[3], 1);
  CHECK_EQ(result.sequence_[4], 0);
  CHECK_EQ(result.sequence_[5], 0);
  CHECK_EQ(result.sequence_[6], 1);
  CHECK_EQ(result.sequence_[7], 0);
}
