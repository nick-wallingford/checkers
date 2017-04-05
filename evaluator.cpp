#include "evaluator.hpp"

#include "position.hpp"
#include "util.hpp"

using namespace std;

static const array<unsigned, 7> diagonals{{
    0x11224488u, // d line
    0x88844221u, // black a line, white g line
    0x44888442u, // black b line, white f line
    0x22448884u, // black c line, white e line
    0x21112244u, // black e line, white c line
    0x42211122u, // black f line, white b line
    0x84422111u, // black g line, white a line
}};

// Evaluates a diagonal. Does not evaluate the d diagonal.
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

int eval_trade_material(const position &p, int weight) {
  const int white = __builtin_popcount(p[1] | p[3]) + __builtin_popcount(p[3]);
  const int black = __builtin_popcount(p[0] | p[2]) + __builtin_popcount(p[2]);

  if (black + white < weight) {
    if (black > white)
      return -white;
    else if (white > black)
      return black;
  }
  return 0;
}

int eval_dyke(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & 0x44223u);
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

// Gives points for having the pyramid intact
int eval_pyramid(const position &p, int weight) {
  int black = __builtin_popcount(p[0] & 0x267u);
  black *= black;

  int white = __builtin_popcount(p[1] & 0xe6400000u);
  white *= white;

  return (black - white) * weight / 36;
}

int eval_safe_kings(const position &p, int weight) {
  if (__builtin_popcount(p[0] | p[1] | p[2] | p[3]) > 6)
    return 0;

  int retval = 0;

  if (__builtin_popcount(p[2]) > __builtin_popcount(p[3])) {
    if (__builtin_popcount(p[3] & 0x00000011) == 1)
      retval--;
    if (__builtin_popcount(p[3] & 0x88000000) == 1)
      retval--;
  } else if (__builtin_popcount(p[3]) > __builtin_popcount(p[2])) {
    if (__builtin_popcount(p[2] & 0x00000011) == 1)
      retval++;
    if (__builtin_popcount(p[2] & 0x88000000) == 1)
      retval++;
  }

  return retval * weight;
}

int eval_centralized_kings(const position &p, int weight) {
  int retval = __builtin_popcount(p[2] & 0x00466200u);
  retval -= __builtin_popcount(p[3] & 0x00466200u);
  return weight * retval;
}

int eval_trapped_kings(const position &p, int weight) {
  char retval = 0;
  if (p.player()) {
    // white's turn.

    // white kings in bottom left
    for (board_iterator it{p[3] & 0x73311000u}; it.valid(); ++it)
      if ((p[0] | p[2]) & (*it >> 8 | *it << 1))
        retval++;
      // trapped in the corner
      else if (*it == 0x10000000u && (p[0] | p[2]) & 0x200000u)
        retval++;

    // white kings in top right
    for (board_iterator it{p[3] & 0x88cceu}; it.valid(); ++it)
      if (p[2] & (*it << 8 | *it >> 1))
        retval++;
      // trapped in the corner
      else if (*it == 0x4u && (p[0] & 0x4u) | (p[2] & 0x400u))
        retval++;

    // black kings in 13,21,22,29,30,31
    for (board_iterator it{p[2] & 0x70301000u}; it.valid(); ++it)
      if (0xe070301u & p[3] & (*it << 5 | *it >> 3 | *it >> 12 | *it >> 13))
        retval--;
      else if (*it == 0x10000000u && p[3] & 0x20000u)
        retval--;

    // black kings in 17,25,26
    for (board_iterator it{p[2] & 0x03010000u}; it.valid(); ++it)
      if (p[3] & (*it << 6 | *it >> 2 | *it >> 11 | *it >> 12))
        retval--;

    // black kings in 2,3,4,11,12,20
    for (board_iterator it{p[2] & 0x80c0eu}; it.valid(); ++it)
      // trapped vertically
      if (0x80c0e000u & (p[3] | p[1]) & (*it << 12 | *it << 13))
        retval--;
      // trapped horizontally
      else if (p[3] & (*it << 3 | *it >> 5))
        retval--;
      // trapped in the corner
      else if (*it == 0x8u && (p[1] | p[3]) & 0x4000u)
        retval--;

    // black kings in 7,8,16
    for (board_iterator it{p[2] & 0x80c0u}; it.valid(); ++it)
      // trapped vertically
      if (0x0c0e0000u & (p[3] | p[1]) & (*it << 11 | *it << 12))
        retval--;
      // trapped horizontally
      else if (p[3] & (*it << 2 | *it >> 6))
        retval--;
  } else {
    // black's turn.

    // black kings in bottom left
    for (board_iterator it{p[2] & 0x73311000u}; it.valid(); ++it)
      if (p[3] & (*it >> 8 | *it << 1))
        retval--;
      // trapped in the corner
      else if (p[2] == 0x10000000u && (p[1] & 0x20000000u) | (p[3] & 0x200000u))
        retval--;

    // black kings in top right
    for (board_iterator it{p[2] & 0x88cceu}; it.valid(); ++it)
      if ((p[3] | p[1]) & (*it << 8 | *it >> 1))
        retval--;
      // trapped in the corner
      else if (p[2] == 0x8u && (p[1] | p[3]) & 0x400u)
        retval--;

    // white kings in 13,21,22,29,30,31
    for (board_iterator it{p[3] & 0x70301000u}; it.valid(); ++it)
      // trapped vertically
      if (0x70301u & (p[0] | p[2]) & (*it >> 13 | *it >> 12))
        retval++;
      // trapped horizontally
      else if (p[2] & (*it << 5 | *it >> 3))
        retval++;
      // trapped in the corner
      else if (*it == 0x10000000u && (p[1] | p[3]) & 0x20000u)
        retval++;

    // white kings in 17,25,26
    for (board_iterator it{p[3] & 0x3010000u}; it.valid(); ++it)
      // trapped vertically
      if (0x7030u & (p[0] | p[2]) & (*it >> 12 | *it >> 11))
        retval++;
      // trapped horizontally
      else if (p[2] & (*it << 6 | *it >> 2))
        retval++;

    // white kings in 2,3,4,11,12,20
    for (board_iterator it{p[3] & 0x80c0eu}; it.valid(); ++it)
      if (0x80c0e070u & p[2] & (*it << 13 | *it << 12 | *it << 3 | *it >> 5))
        retval++;
      // trapped in the corner
      else if (*it == 0x8u && p[3] & 0x4000u)
        retval++;

    // white kings in 7,8,16
    for (board_iterator it{p[3] & 0x80c0u}; it.valid(); ++it)
      if (0xc0e0303u & p[2] & (*it << 12 | *it << 11 | *it << 2 | *it >> 6))
        retval++;
  }
  return retval * weight;
}
}
