#include "evaluator.hpp"

#include <random>

#include "position.hpp"

using namespace std;

static inline int pow_int(int base, int pow) {
  int retval = 1;
  while (pow--)
    retval *= base;
  return retval;
}

int evaluator::formation::operator()(const position &p) const {
  const char pow = __builtin_popcount(pattern);
  return weight * pow_int(p.eval(pattern, fields), power) / pow_int(pow, power);
}

int evaluator::operator()(const position &p) const {
  const std::array<unsigned, 4> &pieces = p.get_board();
  const char king_count = __builtin_popcount(pieces[2] | pieces[3]);

  int retval = p.piece_differential() * 10;
  retval += p.king_differential() * kingweight;
  if (king_count < 6)
    retval += p.king_differential() * (6 - king_count);

  for (const formation &f : formations)
    retval += f(p);
  for (const auto &eval : spec_evaluators)
    retval += eval_funcs[eval.first](pieces, p.player(), eval.second);

  return retval;
}

void evaluator::mutate() {
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

  for (formation &f : formations)
    switch (dist(r)) {
    case 0:
      ++f;
      break;
    case 1:
      --f;
      break;
    }

  for (std::pair<eval_names, int> &eval : spec_evaluators)
    switch (dist(r)) {
    case 0:
      eval.second++;
      break;
    case 1:
      eval.second--;
      break;
    }
}
