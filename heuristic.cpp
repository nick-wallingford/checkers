#include "heuristic.hpp"

#include <random>

#include "position.hpp"

using namespace std;

int heuristic::operator()(const position &p) const {
  const char king_count = __builtin_popcount(p[2] | p[3]);

  int retval = p.piece_differential() * 10;
  retval += p.king_differential() * kingweight;

  if (king_count < 6)
    retval += p.king_differential() * (6 - king_count);

  for (const auto &eval : evaluators)
    retval += eval_funcs[eval.first](p, eval.second);

  return retval;
}

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
