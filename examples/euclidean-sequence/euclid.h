#pragma once

struct Seq {
  const static int size_{8};
  bool sequence_[size_]{};
};

inline void euclid(int k, int n, Seq* out) {
  int steps = out->size_ < n ? out->size_ : n;
  for (int i = 0; i < steps; i++) {
    out->sequence_[i] = (i * k) % steps < k;
  }
}
