#include "util.hpp"
#include <cinttypes>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

void gen_hash_64() {
  const double f = M_PI;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
  const uint64_t seed = *(uint64_t *)&f;
#pragma GCC diagnostic pop
  uniform_int_distribution<uint64_t> dist{0, numeric_limits<uint64_t>::max()};
  mt19937 rand{seed};
  rand.discard(10000);

  ios_base::fmtflags oldFlags = cout.flags();
  streamsize oldPrec = cout.precision();
  char oldFill = cout.fill();

  cout << hex << showbase << internal << setfill('0');
  cout << "static const uint64_t zobrist_player = " << dist(rand) << ";\n";
  cout << "static const std::array<std::array<uint64_t, 32>, 4> zobrist{{\n";
  for (int i = 4; i--;) {
    cout << "{{";

    for (int j = 32; j--;) {
      cout << dist(rand);
      if (j)
        cout << ',';
    }
    cout << "}}";
    if (i)
      cout << ',';
  }
  cout << "}};\n";

  cout.flags(oldFlags);
  cout.precision(oldPrec);
  cout.fill(oldFill);
}
