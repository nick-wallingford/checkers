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
  return weight * pow_int(p.eval(pattern, fields), pow) / pow_int(weight, pow);
}

int evaluator::operator()(const position &p) const {
  int retval = p.piece_differential() * 10;
  retval += p.king_differential() * kingweight;
  for (const formation &f : formations)
    retval += f(p);
  return retval;
}

void evaluator::mutate() {
  mt19937 r{random_device()()};
  uniform_int_distribution<> dist{0, (int)formations.size()};

  const int modify = dist(r);
  dist.param(uniform_int_distribution<>::param_type(0, 1));
  if ((size_t)modify == formations.size()) { // modify king weight
    if (dist(r))
      kingweight++;
    else
      kingweight--;
  } else {
    dist.param(uniform_int_distribution<>::param_type(0, 3));
    if (dist(r)) { // modify the weight
      int change = dist(r);
      if (change <= 1)
        formations[modify].weight += change - 2;
      else
        formations[modify].weight += change - 1;
    } else { // modify the power
      dist.param(uniform_int_distribution<>::param_type(0, 1));
      if (dist(r))
        formations[modify].power++;
      else
        formations[modify].power--;
    }
  }
}
