#include "util.hpp"
#include <cinttypes>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <random>

using namespace std;

template <typename T> void generate_hashes(T seed) {
  uniform_int_distribution<T> dist{0, numeric_limits<T>::max()};
  mt19937 rand{seed};
  rand.discard(10000);

  ios_base::fmtflags oldFlags = cout.flags();
  streamsize oldPrec = cout.precision();
  char oldFill = cout.fill();

  cout << hex << showbase << internal << setfill('0');
  cout << "static const size_t zobrist_player = " << dist(rand) << ";\n";
  cout << "static const size_t zobrist[4][32] = {\n";
  for (int i = 4; i--;) {
    cout << '{';
    for (int j = 32; j--;) {
      cout << dist(rand);
      if (j)
        cout << ',';
    }
    cout << '}';
    if (i)
      cout << ',';
  }
  cout << "};\n";

  cout.flags(oldFlags);
  cout.precision(oldPrec);
  cout.fill(oldFill);
}

#pragma GCC diagnostic ignored "-Wstrict-aliasing"
void gen_hash_32() {
  const float f = M_PI;
  const uint32_t i = *(uint32_t *)&f;
  generate_hashes<uint32_t>(i);
}
void gen_hash_64() {
  const double f = M_PI;
  const uint64_t i = *(uint64_t *)&f;
  generate_hashes<uint64_t>(i);
}
#pragma GCC diagnostic warning "-Wstrict-aliasing"

