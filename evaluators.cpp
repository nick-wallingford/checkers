#include "evaluators.hpp"

using namespace std;

constexpr unsigned square_to_piece(char square) { return 1 << (square - 1); }
constexpr char piece_to_square(unsigned piece) {
  return 32 - __builtin_clz(piece);
}

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
  int retval = 0;
  if (player) { // white to play
    for (unsigned a = pieces[2]; a;) { // search for trapped black kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
    for (unsigned a = pieces[3]; a;) { // search for trapped white kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
  } else { // black to play
    for (unsigned a = pieces[2]; a;) { //search for trapped black kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
    for (unsigned a = pieces[3]; a;) { // search for trapped white kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
  }
  return retval;
}
}
