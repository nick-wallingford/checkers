#include "evaluators.hpp"

using namespace std;

namespace eval {
/*
 * For every function in this file, there should be a corresponding
 * 'REGISTER_ENUM(<func_name>)' entry in evaluator_names.hpp
 *
 * All functions in this file must have the signature:
 * int eval_<descriptive_name>(const array<unsigned, 4>, char, int)
 *
 * They must also be within namespace eval{}
 */

int eval_trapped_kings(const array<unsigned, 4> &pieces, char player,
                       int weight) {
  return 0;
}
}
