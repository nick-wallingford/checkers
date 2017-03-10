#pragma once

#include <array>
#include <cassert>
#include <cstddef>
#include <vector>

#define WHITE 1
#define BLACK 0

class position {
private:
  uint64_t _hash;
  /**
   * Array of positions on the board of the four different types of pieces.
   * pieces[0] are the black regular pieces.
   * pieces[1] are the white regular pieces.
   * pieces[2] are the black king pieces.
   * pieces[3] are the white king pieces.
   */
  std::array<unsigned, 4> pieces;

  /**
   * Indicates which color is to play.
   */
  char to_play;

  bool capture_moves(std::vector<position> &, const char, const unsigned) const;
  unsigned theirs() const {
    return pieces[WHITE - to_play] | pieces[WHITE - to_play + 2];
  }
  unsigned mine() const { return pieces[to_play] | pieces[to_play + 2]; }
  unsigned all() const { return mine() | theirs(); }

public:
  position();
  bool operator==(const position &o) const {
    for (int i = 4; i--;)
      if (o.pieces[i] != pieces[i])
        return false;
    if (o.to_play != to_play)
      return false;

    return true;
  }

  std::vector<position> moves() const;
  std::uint64_t hash() const { return _hash; }
  void sanity() const
#ifndef NDEBUG
      ;
#else
  {
  }
#endif

  char player() const { return to_play; }
  unsigned operator[](char a) const { return pieces[(int)a]; }

  friend std::ostream &operator<<(std::ostream &, const position &);
};
