#include "hashmap.hpp"

#include <cassert>
#include <limits>

using namespace std;

int &hashmap::operator()(unsigned char d, size_t h) {
  size_t index = h & mask;
  for (int i = 0;;) {
    if (depth[index] == d) {
      if (hash[index] == h)
        return score[index];
    } else if (depth[index] == 0) {
      hash[index] = h;
      depth[index] = d;
      return score[index] = numeric_limits<int>::min();
    } else if (index > 10) {
      for (int j = 4; j--;) {
        index += ++i;
        index &= mask;
        depth[index] = 0;
      }
      depth[h & mask] = d;
      hash[h & mask] = h;
      return score[h % mask] = numeric_limits<int>::min();
    }

    index += ++i;
    index &= mask;
  }
}
