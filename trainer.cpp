#include "trainer.hpp"

#include <algorithm>
#include <thread>

#include "alphabeta_pv.hpp"
#include "position.hpp"

using namespace std;

trainer::trainer(initializer_list<heuristic> seed, int size, char depth)
    : heuristics{seed}, size{size}, depth{depth} {}

void trainer::operator()() {
  for (heuristic &h : heuristics)
    h.reset();

  heuristics.reserve(size);
  for (vector<heuristic>::iterator a = heuristics.begin();
       (int)heuristics.size() < size; heuristics.push_back(*a))
    heuristics.back().mutate();

  for (vector<heuristic>::iterator a = heuristics.begin();
       a != heuristics.end(); ++a) {
    for (vector<heuristic>::iterator b = a + 1; b != heuristics.end(); ++b) {
      matches.push_back({*a, *b});
      matches.push_back({*b, *a});
    }
  }

  vector<thread> threads;
  for (int i = thread::hardware_concurrency(); i--;)
    threads.push_back(thread{[this] { work(); }});
  for (thread &t : threads)
    t.join();
  cout << '\n';

  stable_sort(heuristics.begin(), heuristics.end());
  for (; 2 * (int)heuristics.size() > size; heuristics.pop_back())
    ;
}

pair<heuristic &, heuristic &> trainer::pop() {
  lock_guard<mutex> g{m};
  if (matches.empty())
    throw done{};

  pair<heuristic &, heuristic &> retval = matches.back();
  matches.pop_back();
  return retval;
}

void trainer::exec_match(pair<heuristic &, heuristic &> &match) {
  alphabeta_pv black{match.first, depth, BLACK};
  alphabeta_pv white{match.second, depth, WHITE};
  position p;
  try {
    for (int i = 256; i--;) {
      p = black.get_move(p);
      p = white.get_move(p);
    }

    lock_guard<mutex> g{m};
    cout << '.' << flush;
  } catch (agent::resign) {
    lock_guard<mutex> g{m};
    cout << '.' << flush;

    if (p.player() == BLACK) {
      --match.first;
      ++match.second;
    } else {
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

void trainer::work() {
  try {
    for (;;) {
      std::pair<heuristic &, heuristic &> match = pop();
      exec_match(match);
    }
  } catch (done) {
  }
}
