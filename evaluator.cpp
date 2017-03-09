#include "evaluator.hpp"

#include "position.hpp"

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

int eval_dyke(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & 0x44223);
  black *= black;

  int white = __builtin_popcount(p[1] & 0xc4422000u);
  white *= white;

  return (black - white) * weight / 36;
}

int eval_d_diagonal(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & 0x11224488);
  black *= black;

  int white = __builtin_popcount(p[1] & 0x11224488);
  white *= white;

  return (black - white) * weight / 36;
}

int eval_pyramid(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & 0x267);
  black *= black;

  int white = __builtin_popcount(p[1] & 0xe6400000u);
  white *= white;

  return (black - white) * weight / 36;
}

int eval_centralized_kings(const position &p, int weight) {
  int retval = __builtin_popcount(p[2] & 0x00666600u);
  retval -= __builtin_popcount(p[3] & 0x00666600u);
  return weight * retval;
}

int eval_trapped_kings(const position &p, int) {
  return 0;

  int retval = 0;
  if (p.player()) {               // white to play
    for (unsigned a = p[2]; a;) { // search for trapped black kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
    for (unsigned a = p[3]; a;) { // search for trapped white kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
  } else {                        // black to play
    for (unsigned a = p[2]; a;) { // search for trapped black kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
    for (unsigned a = p[3]; a;) { // search for trapped white kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
  }
  return retval;
}
}
