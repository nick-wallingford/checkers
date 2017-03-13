#include <array>
#include <iostream>
#include <vector>
#include <fstream>

#include "alphabeta_pv.hpp"
#include "heuristic.hpp"
#include "position.hpp"
#include "trainer.hpp"
#include "util.hpp"

using namespace std;

template <class S, class T, int depth = 8> char game_test() {
  heuristic e;
  e.add_evaluator(eval_pyramid, 15);
  e.add_evaluator(eval_d_diagonal, 15);
  e.add_evaluator(eval_dyke, 10);
  e.add_evaluator(eval_centralized_kings, 3);
  e.add_evaluator(eval_trapped_kings, 5);

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

  e.add_evaluator(eval_trapped_kings,11);
  e.add_evaluator(eval_pyramid,13);
  e.add_evaluator(eval_centralized_kings,9);
  e.add_evaluator(eval_dyke,11);
  e.add_evaluator(eval_a_diagonal,10);
  e.add_evaluator(eval_b_diagonal,11);
  e.add_evaluator(eval_c_diagonal,10);
  e.add_evaluator(eval_d_diagonal,9);
  e.add_evaluator(eval_e_diagonal,14);
  e.add_evaluator(eval_f_diagonal,10);
  e.add_evaluator(eval_g_diagonal,8);

  trainer t{{e}, 10, 10};
  for (;;) {
    t();
    ofstream o{"training_data.txt",ios_base::app};
    o << '\n' << t << endl;
  }
  return 0;
}
