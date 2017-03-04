#include <array>
#include <iostream>
#include <random>

#include "agent.hpp"
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

void gametest() {
  position p;

  evaluator e;
  e.add_formation({0x0000046eu, 20, 2, 1});
  e.add_formation({0x76200000u, -20, 2, 2});
  e.add_formation({0x00666600u, 100, 1, 4});
  e.add_formation({0x00666600u, -100, 1, 8});

  minimax black(e, 7, BLACK);
  minimax white(evaluator(), 7, WHITE);

  int i = 0;
  try {
    for (; i < 200; i++) {
      cout << p;
      p = black.get_move(p);
      cout << p;
      p = white.get_move(p);
    }
  } catch (agent::resign) {
  }
  cout << "Game ends after move (2x ply) " << i << endl << p;
}

int main() { gametest(); }
