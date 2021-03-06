#include <array>
#include <fstream>
#include <iostream>
#include <locale.h>
#include <vector>

#ifdef MEASURE_BRANCHING_FACTOR
#include <thread>
#endif

#include "alphabeta_pv.hpp"
#include "game.hpp"
#include "heuristic.hpp"
#include "position.hpp"
#include "trainer.hpp"
#include "util.hpp"

using namespace std;

template <class S, class T> char game_test(int depth = 12) {
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
       << "  --cpu-game [depth]     Showcase a CPU vs CPU match.\n"
#ifdef MEASURE_BRANCHING_FACTOR
       << "  --branch               Plays a few games to test the branching "
          "factor.\n"
#endif
       << "  --game                 Play against the computer\n"
       << "Ensure UTF-8 support is enabled. In PuTTY, select:\n"
       << "Configure -> Window -> Translation -> Remote character set -> UTF-8"
       << endl;
}

static void train() {
  heuristic e = best_heuristic();

  trainer t{{e}, 12, 12};
  for (;;) {
    t();
    ofstream o{"training_data.txt", ios_base::app};
    o << '\n' << t << endl;
#ifdef MEASURE_BRANCHING_FACTOR
    alphabeta_pv::report_branching();
#endif
  }
}

#ifdef MEASURE_BRANCHING_FACTOR
template <class T, int depth = 12> void measure_branching() {
  vector<thread> threads;
  mutex m;

  for (int i = thread::hardware_concurrency(); i--;)
    threads.emplace_back(thread{[&] {
      while (1) {
        heuristic e = best_heuristic();
        T black{e, depth, BLACK};
        T white{e, depth, WHITE};
        position p;
        try {
          for (int i = 192; i--;) {
            p = black.get_move(p);
            p = white.get_move(p);
          }
        } catch (agent::resign) {
        }
        m.lock();
        T::report_branching();
        m.unlock();
      }
    }});

  for (thread &t : threads)
    t.join();
}
#endif

int main(int argc, char **argv) {
  setlocale(LC_ALL, ""); // UTF-8 characters are used to represent the pieces.
  if (argc == 1)
    usage();

  for (int i = 1; i < argc; i++)
    if (string{"--train"}.compare(argv[i]) == 0)
      train();
    else if (string{"--cpu-game"}.compare(argv[i]) == 0) {
      int depth = 0;
      if (i + 1 < argc)
        depth = atoi(argv[i + 1]);
      if (depth > 0)
        ++i;
      else
        depth = 12;
      game_test<alphabeta_pv, alphabeta_pv>(depth);
    } else if (string{"--game"}.compare(argv[i]) == 0)
      play_game();
#ifdef MEASURE_BRANCHING_FACTOR
    else if (string{"--branch"}.compare(argv[i]) == 0)
      measure_branching<alphabeta_pv, 12>();
#endif
    else
      usage();

#ifdef MEASURE_BRANCHING_FACTOR
  alphabeta_pv::report_branching();
  alphabeta::report_branching();
#endif

  return 0;
}
