#include "evaluator.hpp"

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

int eval_black_dyke(const array<unsigned, 4> &pieces, char, int weight) {
  int retval = __builtin_popcount(pieces[0] & 0x44223);
  retval *= retval * weight;
  retval /= 36;
  return retval;
}

int eval_white_dyke(const array<unsigned, 4> &pieces, char, int weight) {
  int retval = __builtin_popcount(pieces[1] & 0xc4422000u);
  retval *= retval * -weight;
  retval /= 36;
  return retval;
}

int eval_d_diagonal(const array<unsigned, 4> &pieces, char, int weight) {
  int black = __builtin_popcount(pieces[0] & 0x11224488);
  black *= black * weight;
  black /= 64;

  int white = __builtin_popcount(pieces[0] & 0x11224488);
  white *= white * weight;
  white /= 64;

  return black - white;
}

int eval_black_pyramid(const array<unsigned, 4> &pieces, char, int weight) {
  int retval = __builtin_popcount(pieces[0] & 0x267);
  retval *= retval * weight;
  retval /= 36;
  return retval;
}

int eval_white_pyramid(const array<unsigned, 4> &pieces, char, int weight) {
  int retval = __builtin_popcount(pieces[1] & 0xe6400000u);
  retval *= retval * -weight;
  retval /= 36;
  return retval;
}

int eval_centralized_kings(const array<unsigned, 4> &pieces,
                           char player __attribute((unused)), int weight) {
  int retval = __builtin_popcount(pieces[2] & 0x00666600u);
  retval -= __builtin_popcount(pieces[3] & 0x00666600u);
  return weight * retval;
}

int eval_trapped_kings(const array<unsigned, 4> &pieces, char player, int) {
  return 0;

  int retval = 0;
  if (player) {                        // white to play
    for (unsigned a = pieces[2]; a;) { // search for trapped black kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
    for (unsigned a = pieces[3]; a;) { // search for trapped white kings
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      a ^= piece;
    }
  } else {                             // black to play
    for (unsigned a = pieces[2]; a;) { // search for trapped black kings
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
