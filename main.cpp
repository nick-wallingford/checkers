#include <array>
#include <iostream>
#include <random>
#include <unordered_map>
#include <vector>

#include "agent.hpp"
#include "alphabeta_cache.hpp"
#include "heuristic.hpp"
#include "position.hpp"
#include "util.hpp"

using namespace std;

template <class S, class T, int depth = 8> char game_test(const heuristic &e) {
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

int main() {
  heuristic e;
  e.add_evaluator(eval_pyramid, 15);
  e.add_evaluator(eval_d_diagonal, 15);
  e.add_evaluator(eval_dyke, 10);
  e.add_evaluator(eval_centralized_kings, 3);

  game_test<alphabeta_cache, alphabeta_cache, 11>(e);

  return 0;
}
