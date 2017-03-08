#pragma once

#include <memory>

class hashmap {
  const uint64_t mask;
  std::unique_ptr<unsigned char[]> depth;
  std::unique_ptr<uint64_t[]> hash;
  std::unique_ptr<int[]> score;

public:
  hashmap(unsigned char bits)
      : mask{((size_t)1 << bits) - 1},
        depth{new unsigned char[mask + 1]()}, // depth is initialized to 0
        hash{new uint64_t[mask + 1]},           // hash is NOT initialized to 0
        score{new int[mask + 1]}              // score is NOT initialized to 0
  {}
  ~hashmap();

  bool contains(unsigned char, uint64_t) const;
  int &operator()(unsigned char, uint64_t);
};
