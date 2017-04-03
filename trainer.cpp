#include "trainer.hpp"

#include <algorithm>
#include <thread>

#include "alphabeta_pv.hpp"
#include "position.hpp"

using namespace std;

trainer::trainer(initializer_list<heuristic> seed, int size, char depth)
    : heuristics{seed}, size{size}, depth{depth} {}

// Perform one round robin tournament of the different evaluators.
void trainer::operator()() {
  // Reset all the scores to 0.
  for (heuristic &h : heuristics)
    h.reset();

  heuristics.reserve(size);
  // Push mutated copies of the surviving heuristics into the pool
  for (vector<heuristic>::iterator a = heuristics.begin();
       (int)heuristics.size() < size; heuristics.push_back(*a))
    heuristics.back().mutate();

  // Add the heuristics to the tournament pool.
  // For each pair of heuristics...
  for (vector<heuristic>::iterator a = heuristics.begin();
       a != heuristics.end(); ++a) {
    for (vector<heuristic>::iterator b = a + 1; b != heuristics.end(); ++b) {
      // Add a white vs black and black vs white game to the pool.
      matches.push_back({*a, *b});
      matches.push_back({*b, *a});
    }
  }

  // Start up the threads to run the actual tournament, calling work().
  vector<thread> threads;
  for (int i = thread::hardware_concurrency(); i--;)
    threads.push_back(thread{[this] { work(); }});
  for (thread &t : threads)
    t.join();
  cout << '\n';

  // Put the tournament winners in the front of the array.
  stable_sort(heuristics.begin(), heuristics.end());
  // Delete the losers.
  for (; 2 * (int)heuristics.size() > size; heuristics.pop_back())
    ;
}

// Pop a match from the stack.
// This method is thread safe.
// Throws trainer::done if there are no more matches.
pair<heuristic &, heuristic &> trainer::pop() {
  lock_guard<mutex> g{m};
  if (matches.empty())
    throw done{};

  pair<heuristic &, heuristic &> retval = matches.back();
  matches.pop_back();
  return retval;
}

// Runs a match, and updates the scores of the pair of heuristics.
// This method is thread safe, and does not throw.
void trainer::exec_match(pair<heuristic &, heuristic &> &match) {
  alphabeta_pv black{match.first, depth, BLACK};
  alphabeta_pv white{match.second, depth, WHITE};
  position p;
  try {
    for (int i = 256; i--;) {
      p = black.get_move(p);
      p = white.get_move(p);
    }

    // The match has ended in a draw.
    lock_guard<mutex> g{m};
    cout << '.' << flush;
  } catch (agent::resign) {
    lock_guard<mutex> g{m};
    cout << '.' << flush;

    if (p.player() == BLACK) {
      // Black has resigned. Penalize black and reward white.
      --match.first;
      ++match.second;
    } else {
      // White has resigned. Penalize white and reward black.
      ++match.first;
      --match.second;
    }
  }
}

std::ostream &operator<<(std::ostream &o, const trainer &t) {
  for (const heuristic &h : t.heuristics)
    o << h << '\n';
  return o;
}

// Run matches until the matches stack is empty.
// This method is thread safe and does not throw.
void trainer::work() {
  try {
    for (;;) {
      std::pair<heuristic &, heuristic &> match = pop();
      exec_match(match);
    }
  } catch (done) {
  }
}
