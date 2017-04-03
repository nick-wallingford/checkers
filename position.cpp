#include "position.hpp"
#include <cassert>
#include <iostream>
#include <random>

#include "util.hpp"

using namespace std;

// These values are automatically generated in util.cpp and copy-pasted here.
// These values each correspond to a type of piece in a specific board position.
static const uint64_t zobrist_player = 0xedf5d879fc98a912;
static const std::array<std::array<uint64_t, 32>, 4> zobrist{
    {{{0x40eab28baa02a864, 0x874a7dca277a4fb0, 0x581ea36751b3d470,
       0x4f735c246ca4f479, 0x2f1eedbac341d484, 0x91b4679784acbd94,
       0xe166ffc3da42a83e, 0x9485b9ea33046e0d, 0x4e5671b10f23be50,
       0x58f495a72d8dc7c,  0xb6d0a232181df436, 0x308186c3513b59ce,
       0x20c07220106735bc, 0x1e57f138a9b50800, 0xb27ed7460b005ffd,
       0xa121c15ce49afe65, 0x3b67d70e6790e175, 0x54067aeace73cf75,
       0x339f2ccfed4b46a3, 0x8828eec94bf0e45b, 0x811402a947f8a379,
       0x328fdd85f101030a, 0x73b518092a8e52f9, 0xb938ce6c0b0a6e1c,
       0x690752fd731c48ab, 0x382820b3dc759b0,  0x35725de5c414ee22,
       0x924c964e68eddf32, 0xad74e8792dc7cea6, 0x58d1d037111171f1,
       0xefd2cc034dccc7b8, 0x2d18289854e06e1}},
     {{0xe978684d16f45ec9, 0xeed0f142a9647fb9, 0x64038937ea94fb5c,
       0x9dabbcb474785b76, 0x5afc788458cbdbee, 0x644d741753243da6,
       0xd8cdb585aef40c97, 0x9a64cf3d74b2551e, 0x76634ccb011b526b,
       0x50c98c182ce2c776, 0xa56034298155a3d9, 0x7559b78c801d4126,
       0xbd95ff4bf442cad4, 0x7c0f6717059cdf5f, 0xcc42e6d646a4de57,
       0x50cdb4f1e268b34e, 0x8c509bb0a789ca05, 0x824ac0630ad7f0f,
       0x72c80d967db5cd19, 0x7af732115f6a9761, 0xc459beb5e9c521b5,
       0x8ba252dad40801e5, 0xe0af0bcc72fec70f, 0x87ce0deaf901053c,
       0xf87220163c0e8696, 0xde364614de64a446, 0x87c17b2ed8e922a8,
       0x184bebcfb6237344, 0x9a6ada6e38667db,  0xa8fc491e7e0f1f50,
       0x752212d20d1fdde4, 0x362ad2cd1f405877}},
     {{0xe8ca4c65df984c61, 0x6684fa9f3e223b9c, 0xcbe14bbd545896a9,
       0xbaba3d09a3de7818, 0x7458433c03899e93, 0x30dbb92099a75e4e,
       0x71e9470f49877dd3, 0x29ce78b9dd3cb672, 0x43787c7ce50422e1,
       0x5783cab3ed4d9c62, 0xc8ef78982a9d7dc8, 0x9e51d92df36fc8fc,
       0x1f07244f448e879,  0x275a97095c5800be, 0xc4bb4dedbfb3fa1a,
       0xc1ebfb6cb34bf3e4, 0xc58bf4fcb1c6e2ce, 0x6b70c9dacc9b241c,
       0x502d5860904fd435, 0x109d698b383b8c1f, 0x96c0df136f7dfee2,
       0x4fb1db2e7866d82c, 0xb3981adb7a000935, 0x5066a0ec505eee56,
       0x51097ab0f2a55bfa, 0xb3ab27704a22c3bb, 0x62a558920273600,
       0xe10f3da80a04aaaa, 0x6cc9f3ec0c39c5f3, 0x963f28c190ee4fc7,
       0x9fafda24ab0b4747, 0x8b963ffc9d459993}},
     {{0x907337ab331e42a6, 0xd8ec9044c7dc499c, 0x50f40a9f1254ac55,
       0xad00218ab190cbb9, 0x1ccb0de99f089832, 0xf6f05e3993c6639e,
       0xa90e481809392b89, 0x8b676cd1c974e6a6, 0x3c9a803234e0812f,
       0x7d5d7ff127fe46c6, 0x333b53bf6c44b7b8, 0x92f1b372d70ca60b,
       0xffba64c6cd3f7e39, 0xf9c6853a2f5b5a20, 0xc6dc784d6d54a191,
       0xfa06b6d390c4b6fd, 0xe278403b891c9225, 0x796ce30804d58d78,
       0x46564c69a9c22c2d, 0x9a2e5374905a21e2, 0xf0d65d150a4ee56c,
       0x8a277f9f892ca67d, 0xdd74b3613a5a7de4, 0xc0bbc635b5b60a25,
       0x96b599e615561722, 0xe855114fc5654372, 0xeb9d6adec5b6f81d,
       0x1e0d5afe2323dffe, 0xbece13ddbd419395, 0x5166b79643aa7ea4,
       0xdc5052f721e7c12c, 0x4e2159eaf7bbb0ce}}}};

static inline uint64_t zob(unsigned piece, bool player, bool king) {
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
constexpr uint64_t start_hash(char a) {
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

  uint64_t h = to_play == BLACK ? 0 : zobrist_player;

  for (int i = 4; i--;) {
    for (board_iterator it{pieces[i]}; it.valid(); ++it) {
      assert(pieces[i] & *it);

      h ^= zob(*it, i & 1, i & 2);
    }
  }

  assert(h == _hash);
}
#endif
