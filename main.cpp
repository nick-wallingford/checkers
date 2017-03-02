#include <array>
#include <iostream>
#include <random>

#include "position.hpp"
#include "util.hpp"

using namespace std;

static void random_agent_test() {
  int max_count = 0;
  int max_seed = 0;
  for (int i = 0; i < 10000; i++) {
    position p;
    int count = 0;
    mt19937 rand{i};
    rand.discard(10000);

    for (vector<position> moves = p.moves(); !moves.empty();
         moves = p.moves()) {
      ++count;

      uniform_int_distribution<size_t> d{0, moves.size() - 1};
      p = moves[d(rand)];
    }

    if (count > max_count) {
      max_count = count;
      max_seed = i;
      cout << "max count: " << max_count << " seed: " << max_seed << endl;
    }
  }

  position p;
  mt19937 rand{max_seed};
  rand.discard(10000);

  for (vector<position> moves = p.moves(); !moves.empty(); moves = p.moves()) {
    cout << p << endl;
    uniform_int_distribution<size_t> d{0, moves.size() - 1};
    p = moves[d(rand)];
  }

  cout << p << endl;
  cout << "max count: " << max_count << " seed: " << max_seed << endl;
}

int main() {
  position p;
  p.sanity();
  random_agent_test();
}
