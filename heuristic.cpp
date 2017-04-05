#include "heuristic.hpp"

#include <random>

#include "position.hpp"

using namespace std;

// Returns the best known heuristic. These values were found via the training.
heuristic best_heuristic() {
  heuristic e{14};
  e.add_evaluator(eval_trapped_kings, 4);
  e.add_evaluator(eval_pyramid, 10);
  e.add_evaluator(eval_centralized_kings, 1);
  e.add_evaluator(eval_dyke, 9);
  e.add_evaluator(eval_a_diagonal, 10);
  e.add_evaluator(eval_b_diagonal, 14);
  e.add_evaluator(eval_c_diagonal, 13);
  e.add_evaluator(eval_d_diagonal, 9);
  e.add_evaluator(eval_e_diagonal, 11);
  e.add_evaluator(eval_f_diagonal, 15);
  e.add_evaluator(eval_g_diagonal, 17);
  e.add_evaluator(eval_safe_kings, 3);

  return e;
}

// Evaluates a position, and returns the integer corresponding to its
int heuristic::operator()(const position &p) const {
  // Base heuristic is just the material count.
  int retval = (__builtin_popcount(p[0]) - __builtin_popcount(p[1])) * 10;
  retval += (__builtin_popcount(p[2]) - __builtin_popcount(p[3])) * kingweight;

  // These are a sufficient, but not necessary condition to ensure a loss.
  if (!__builtin_popcount(p[0] | p[2]))
    retval -= 1000000;
  if (!__builtin_popcount(p[1] | p[3]))
    retval += 1000000;

  for (const auto &eval : evaluators)
    retval += eval_funcs[eval.first](p, eval.second);

  return retval;
}

// Randomly increase, decrease, or leave unaffected each evaluator's weight.
void heuristic::mutate() {
  mt19937 r{random_device()()};
  uniform_int_distribution<> dist{0, 2};

  switch (dist(r)) {
  case 0:
    kingweight++;
    break;
  case 1:
    kingweight--;
    break;
  }

  for (std::pair<eval_names, int> &eval : evaluators)
    switch (dist(r)) {
    case 0:
      eval.second++;
      break;
    case 1:
      eval.second--;
      break;
    }
}

std::ostream &operator<<(std::ostream &o, const heuristic &h) {
  o << "{(score:" << h.score << "),(kingweight:" << h.kingweight << ')';
  for (pair<eval_names, int> e : h.evaluators)
    o << ",(" << eval_names_text[e.first] << ',' << e.second << ')';
  o << '}';
  return o;
}
