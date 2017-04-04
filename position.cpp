#include "position.hpp"
#include <cassert>
#include <iostream>
#include <random>

#include "util.hpp"

using namespace std;

// These values are automatically generated in util.cpp and copy-pasted here.
// These values each correspond to a type of piece in a specific board position.

static const unsigned zobrist_player = 0x6cb604b5;
static const std::array<std::array<unsigned, 32>, 4> zobrist{
    {{{0xe398febf, 0x3cf54331, 0x5e127ef5, 0x9b305601, 0xd7c2d8ef, 0x12b9188f,
       0x897f0fdf, 0x310f92c4, 0xcd66e12e, 0xefab7ac9, 0x3666a518, 0x2fd73234,
       0xfd0e0747, 0x546741fd, 0xa8de9b68, 0x5c97e2c9, 0x8c6c8a82, 0xef468c1,
       0xe30be5b5, 0xd7227047, 0x93b3071c, 0xf3c9a041, 0xa7581095, 0x8d54b28a,
       0xe1b1d25f, 0xf59e27fb, 0x82ca1f74, 0x313c5f13, 0x96b7673d, 0x1510bb70,
       0xd91f8cec, 0x5e570d89}},
     {{0x50dfb203, 0x13dc1f47, 0x3b1448e5, 0xc81c84ef, 0x6af6eac8, 0x1fc491f7,
       0xc158b401, 0x68fed1d8, 0x869a6407, 0xb0ce9e57, 0x24818f97, 0x4cb25260,
       0x1e43bf7c, 0x112e93d9, 0x9012504d, 0x164b10a7, 0x2ef786a3, 0xdbdd9f7b,
       0x5e01ed47, 0x41b50343, 0x29364802, 0xe5c29c0,  0xa9d8f68a, 0x830b6498,
       0x9da9232,  0xd86142d0, 0x54125372, 0xf3aaff60, 0xe34c0d98, 0x37dad68f,
       0xa61ec8ab, 0xea432ef7}},
     {{0xdb48768c, 0xba67c1d5, 0xe49f82b8, 0x3e31e1d1, 0xcc20d234, 0xb7218c68,
       0xcc5f02bc, 0x4376eeed, 0x4fac2e0e, 0x410e1f8a, 0x5b6293fa, 0x3c822fe4,
       0x17bfbc0a, 0xbacf9c02, 0xe8b10cb9, 0x505c5989, 0x432c46a5, 0xd3e8c72f,
       0x54339de6, 0xf10437ca, 0x446a0a36, 0xbc500c53, 0xdd356cfd, 0xed6bfb64,
       0x6e25cb20, 0x24249376, 0x7f3b4e66, 0xb0f8ed98, 0xde512299, 0xf5dc64c0,
       0x4773e85a, 0x4bb5f0ed}},
     {{0x81df1f36, 0x67006a34, 0xfb5e4517, 0x44460998, 0xc67ef59e, 0x41947394,
       0x5b1eed8b, 0x89aed0b2, 0x821ea85a, 0x8e686313, 0x559a5729, 0xf8d827d7,
       0x45900ab,  0x908c9bd6, 0x673d9b30, 0xff8597d1, 0xa6a46c1d, 0x448f3be7,
       0x2854ef27, 0x3c3924d6, 0x9f361d2d, 0x8c2854cb, 0x29ba7f5e, 0xa8b7c53,
       0x3a2d60e1, 0x17c0c01c, 0x6c67a13a, 0xe1124db4, 0x23e0b402, 0xbf0f1211,
       0xab3838,   0x9e6fada3}}}};

static inline unsigned zob(unsigned piece, bool player, bool king) {
  return zobrist[2 * king + player][__builtin_clz(piece)];
}

// Adds the capture moves a given piece with a given mobility has available to
// the list. Returns true if moves were added, false if no moves were added.
bool position::capture_moves(vector<position> &list, const unsigned piece,
                             const char mobility) const {
  bool capture = false;
  assert(piece & mine());
  assert(mobility == (UP | DOWN) ? (piece & pieces[to_play + 2])
                                 : (piece & pieces[to_play]));

  for (capture_iterator it{piece, mobility}; it.valid(); ++it) {
    if (all() & it.capturing()) // if there's a piece where we're going, skip
      continue;
    if (!(theirs() & it.captured()))
      continue; // if we're not capturing an enemy piece, skip

    // This is a valid capture.
    capture = true;

    position p{*this};

    // We are USING a king to capture.
    const bool capturing_king = mobility == (UP | DOWN);
    // We have captured a king.
    const bool captured_king = it.captured() & p(to_play ^ 1, 1);

    assert(p(p.to_play, capturing_king) & piece);
    assert(p(1 ^ p.to_play, captured_king) & it.captured());

    // Actually perform the capture on the board.

    // Remove the piece from where we are.
    p(p.to_play, capturing_king) ^= piece;
    // Add the piece to where we're going.
    p(p.to_play, capturing_king) ^= it.capturing();
    // Remove the piece we have captured.
    p(1 ^ to_play, captured_king) ^= it.captured();

    // Update the hash for after the capture has taken place.
    p._hash ^= zob(piece, p.to_play, capturing_king);
    p._hash ^= zob(it.capturing(), p.to_play, capturing_king);
    p._hash ^= zob(it.captured(), 1 ^ p.to_play, captured_king);

    // Check for double jumps and...
    if (!p.capture_moves(list, it.capturing(), mobility))
      // If there are no double jumps, add this move to the move list.
      list.emplace_back(p);
  }
  return capture;
}

// Retrieves a list of available moves.
vector<position> position::moves() const {
  sanity();

  vector<position> moves;
  moves.reserve(16);

  // Process all available captures.
  for (char king = 2; king--;) {
    const char mobility = king ? (UP | DOWN) : (to_play + 1);
    for (board_iterator it{(*this)(to_play, king)}; it.valid(); ++it)
      capture_moves(moves, *it, mobility);
  }

  // If a capture is available, it must be taken. So only update regular moves
  // if there are no captures.
  if (moves.empty()) {
    // Do kings first, (king == 1) regular pieces (king == 0) second.
    for (char king = 2; king--;) {
      const char mobility = king ? (UP | DOWN) : (to_play + 1);
      // Iterate over the pieces on the board
      for (board_iterator it{(*this)(to_play, king)}; it.valid(); ++it) {
        // Iterate over the places it can move to.
        for (board_iterator move{get_moves(*it, mobility)}; move.valid();
             ++move) {
          // This piece can't move here; continue;
          if (*move & all())
            continue;

          position p{*this};

          // Remove the piece from where we are
          p(p.to_play, king) ^= *it;
          // Add the piece where we're moving to.
          p(p.to_play, king) ^= *move;

          // Update hash.
          p._hash ^= zob(*move, p.to_play, king);
          p._hash ^= zob(*it, p.to_play, king);

          // Add it to the list.
          moves.emplace_back(p);
        }
      }
    }
  }

  // Upkeep the moves.
  for (position &p : moves) {
    // Upgrade black pieces to kings.
    for (board_iterator it{p[0] & 0xf0000000u}; it.valid(); ++it) {
      p._hash ^= zob(*it, 0, 0) ^ zob(*it, 0, 1);
      p[0] ^= *it;
      p[2] ^= *it;
    }

    // Upgrade white pieces to kings.
    for (board_iterator it{p[1] & 0x0000000fu}; it.valid(); ++it) {
      p._hash ^= zob(*it, 1, 0) ^ zob(*it, 1, 1);
      p[1] ^= *it;
      p[3] ^= *it;
    }
    // Update the active player.
    p.to_play ^= 1;
    p._hash ^= zobrist_player;
  }

  return moves;
}

// Calculate the hash of the starting board position. This is a compile time
// constant.
constexpr unsigned start_hash(char a) {
  return a ? (start_hash(a - 1) ^ zobrist[1][a - 1] ^ zobrist[0][32 - a]) : 0;
}

position::position()
    : _hash{start_hash(12)}, pieces{{0x00000fffu, 0xfff00000u, 0, 0}},
      to_play{BLACK} {}

ostream &operator<<(ostream &o, const position &p) {
  const string null_square{"\e[47;1m "};
  const string empty_square{"\e[42;1m "};
  const string color_reset{"\e[0m"};
  const string black{"\e[31;42;1m"};
  const string white{"\e[37;42;1m"};
  const string regular{"●"};
  const string king{"♣"};

  if (p.to_play)
    o << white << "To play: White" << color_reset << '\n';
  else
    o << black << "To play: Black" << color_reset << '\n';
  for (unsigned piece = 1; piece; piece <<= 1) {
    if (piece & 0x0f0f0f0fu)
      o << null_square;

    if (piece & p[0])
      o << black << regular;
    else if (piece & p[1])
      o << white << regular;
    else if (piece & p[2])
      o << black << king;
    else if (piece & p[3])
      o << white << king;
    else
      o << black << ' ';

    if (piece & 0xf0f0f0f0u)
      o << null_square;
    if (piece & 0x88888888u)
      o << color_reset << '\n';
  }
  o << color_reset;
  return o;
}

#ifndef NDEBUG
void position::sanity() const {
  for (int i = 0; i < 3; i++)
    for (int j = i + 1; j < 4; j++)
      assert(0 == (pieces[i] & pieces[j]));
  for (unsigned board : pieces)
    assert(__builtin_popcount(board) <= 12);
  assert(__builtin_popcount(pieces[0] | pieces[2]) <= 12);
  assert(__builtin_popcount(pieces[1] | pieces[3]) <= 12);

  unsigned h = to_play == BLACK ? 0 : zobrist_player;

  for (int i = 4; i--;) {
    for (board_iterator it{pieces[i]}; it.valid(); ++it) {
      assert(pieces[i] & *it);

      h ^= zob(*it, i & 1, i & 2);
    }
  }

  assert(h == _hash);
}
#endif
