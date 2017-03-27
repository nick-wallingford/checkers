#include <array>
#include <fstream>
#include <iostream>
#include <vector>

#include "alphabeta_pv.hpp"
#include "game.hpp"
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

static void usage() {
  cout << "Usage: checkers <arguments>\n"
       << " <arguments> can be any of:\n"
       << "  --train                Train the heuristic by playing the "
          "computer against itself.\n"
       << "  --cpu-game             Showcase a CPU vs CPU match.\n"
       << "  --game                 Play against the computer\n";

  exit(0);
}

static void train() {
  heuristic e;

  e.add_evaluator(eval_trapped_kings, 11);
  e.add_evaluator(eval_pyramid, 13);
  e.add_evaluator(eval_centralized_kings, 9);
  e.add_evaluator(eval_dyke, 11);
  e.add_evaluator(eval_a_diagonal, 10);
  e.add_evaluator(eval_b_diagonal, 11);
  e.add_evaluator(eval_c_diagonal, 10);
  e.add_evaluator(eval_d_diagonal, 9);
  e.add_evaluator(eval_e_diagonal, 14);
  e.add_evaluator(eval_f_diagonal, 10);
  e.add_evaluator(eval_g_diagonal, 8);

  trainer t{{e}, 10, 10};
  for (;;) {
    t();
    ofstream o{"training_data.txt", ios_base::app};
    o << '\n' << t << endl;
  }
}

int main(int argc, char **argv) {
  if (argc == 1)
    usage();

  for (int i = 1; i < argc; i++)
    if (string{"--train"}.compare(argv[i]) == 0)
      train();
    else if (string{"--cpu-game"}.compare(argv[i]) == 0)
      game_test<alphabeta_pv, alphabeta_pv, 2>();
    else if (string{"--game"}.compare(argv[i]) == 0)
      game();
    else
      usage();

  return 0;
}
