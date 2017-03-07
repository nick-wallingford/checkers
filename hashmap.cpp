#include "hashmap.hpp"

#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

bool hashmap::contains(unsigned char d, size_t h) const {
  size_t index = h & mask;
  char i = -1;

  for (;;) {
    if (depth[index] >= d && hash[index] == h)
      return true;

    index += i += 2;
    index &= mask;

    if (i > 9)
      return false;
  }
}

int &hashmap::operator()(unsigned char d, size_t h) {
  size_t index = h & mask;
  char i = -1;
  for (;;) {
    if (depth[index] == 0) {
      hash[index] = h;
      depth[index] = d;
      return score[index] = numeric_limits<int>::min();
    } else if (depth[index] >= d && hash[index] == h) {
      return score[index];
    }

    index += i += 2;
    index &= mask;

    if (i > 9) {
      depth[index] = 0;
      depth[h & mask] = d;
      hash[h & mask] = h;
      return score[h % mask] = numeric_limits<int>::min();
    }
  }
}

hashmap::~hashmap() {
  size_t hits = 0;
  for (size_t i = mask + 1; i--;)
    if (depth[i])
      hits++;
  cout << (float)hits / mask << '%' << endl;
}
