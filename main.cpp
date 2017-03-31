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

static heuristic best_heuristic() {
  heuristic e{14};
  e.add_evaluator(eval_trapped_kings, 2);
  e.add_evaluator(eval_pyramid, 13);
  e.add_evaluator(eval_centralized_kings, 2);
  e.add_evaluator(eval_dyke, 8);
  e.add_evaluator(eval_a_diagonal, 12);
  e.add_evaluator(eval_b_diagonal, 13);
  e.add_evaluator(eval_c_diagonal, 13);
  e.add_evaluator(eval_d_diagonal, 8);
  e.add_evaluator(eval_e_diagonal, 11);
  e.add_evaluator(eval_f_diagonal, 14);
  e.add_evaluator(eval_g_diagonal, 12);

  return e;
}

template <class S, class T, int depth = 8> char game_test() {
  heuristic e = best_heuristic();

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
  heuristic e = best_heuristic();

  trainer t{{e}, 12, 12};
  for (;;) {
    t();
    ofstream o{"training_data.txt", ios_base::app};
    o << '\n' << t << endl;
#ifdef MEASURE_BRANCHING_FACTOR
    alphabeta_pv_report_branching();
    alphabeta_report_branching();
#endif
  }
}

int main(int argc, char **argv) {
  if (argc == 1)
    usage();

  for (int i = 1; i < argc; i++)
    if (string{"--train"}.compare(argv[i]) == 0)
      train();
    else if (string{"--cpu-game"}.compare(argv[i]) == 0)
      game_test<alphabeta_pv, alphabeta, 12>();
    else if (string{"--game"}.compare(argv[i]) == 0)
      game();
    else
      usage();

#ifdef MEASURE_BRANCHING_FACTOR
  alphabeta_pv_report_branching();
  alphabeta_report_branching();
#endif

  return 0;
}
