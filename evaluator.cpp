#include "evaluator.hpp"

#include "position.hpp"

using namespace std;

constexpr unsigned square_to_piece(char square) { return 1 << (square - 1); }
constexpr char piece_to_square(unsigned piece) {
  return 32 - __builtin_clz(piece);
}
constexpr unsigned next_piece(unsigned board) {
  return 0x80000000u >> __builtin_clz(board);
}

static const array<unsigned, 7> diagonals{{
    0x11224488u, // d line
    0x88844221u, // black a line, white g line
    0x44888442u, // black b line, white f line
    0x22448884u, // black c line, white e line
    0x21112244u, // black e line, white c line
    0x42211122u, // black f line, white b line
    0x84422111u, // black g line, white a line
}};

template <int d> int diagonal(const position &p, int weight) {
  static_assert(d > 0, "Invalid diagonal. Must be > 0");
  static_assert(d < 7, "Invalid diagonal. Must be < 7");

  const array<unsigned, 2> territory{{0xffeecc88u, 0x113377ffu}};
  const int available_count =
      8 + __builtin_popcount(diagonals[d] & 0xffeecc88u);

  int black = __builtin_popcount(p[0] & diagonals[d]);
  black += __builtin_popcount(p[0] & diagonals[d] & territory[0]);
  black *= black;

  int white = __builtin_popcount(p[1] & diagonals[7 - d]);
  white *= white;

  return (black - white) * weight / (available_count * available_count);
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

int eval_a_diagonal(const position &p, int weight) {
  return diagonal<1>(p, weight);
}

int eval_b_diagonal(const position &p, int weight) {
  return diagonal<2>(p, weight);
}

int eval_c_diagonal(const position &p, int weight) {
  return diagonal<3>(p, weight);
}

int eval_e_diagonal(const position &p, int weight) {
  return diagonal<4>(p, weight);
}

int eval_f_diagonal(const position &p, int weight) {
  return diagonal<5>(p, weight);
}

int eval_g_diagonal(const position &p, int weight) {
  return diagonal<6>(p, weight);
}

int eval_d_diagonal(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & diagonals[0]);
  black *= black;

  int white = __builtin_popcount(p[1] & diagonals[0]);
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
