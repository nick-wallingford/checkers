#include <array>
#include <iostream>
#include <random>

#include "agent.hpp"
#include "evaluators.hpp"
#include "position.hpp"
#include "util.hpp"

using namespace std;

#pragma GCC diagnostic ignored "-Wunused-function"

template <class S, class T, int depth = 8> char game_test(const evaluator &e) {
  position p;
  S black{e, depth, BLACK};
  T white{e, depth, WHITE};
  int i;
  try {
    for (i = 0; i < 256; i++) {
      cout << p;
      p = black.get_move(p);
      cout << p;
      p = white.get_move(p);
    }
    cout << "Draw after 256 moves." << endl;
    return 0;
  } catch (agent::resign) {
    if (p.player() == BLACK) {
      cout << "White wins after " << i << " moves." << endl;
      return -1;
    } else {
      cout << "Black wins after " << i << " moves." << endl;
      return 1;
    }
  }
}

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
  evaluator e;
  e.add_evaluator(eval_white_pyramid, 20);
  e.add_evaluator(eval_black_pyramid, 20);
  e.add_evaluator(eval_centralized_kings, 5);
  e.add_evaluator(eval_trapped_kings, 10);

  game_test<alphabeta, alphabeta, 6>(e);

  return 0;
}
