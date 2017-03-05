#include "position.hpp"
#include <cassert>
#include <iostream>
#include <random>

using namespace std;

constexpr unsigned square_to_piece(char square) { return 1 << (square - 1); }
constexpr char piece_to_square(unsigned piece) {
  return 32 - __builtin_clz(piece);
}

#define UP 2
#define DOWN 1

#ifdef amd64
static const size_t zobrist_player = 0xedf5d879fc98a912;
static const size_t zobrist[4][32] = {
    {0x40eab28baa02a864, 0x874a7dca277a4fb0, 0x581ea36751b3d470,
     0x4f735c246ca4f479, 0x2f1eedbac341d484, 0x91b4679784acbd94,
     0xe166ffc3da42a83e, 0x9485b9ea33046e0d, 0x4e5671b10f23be50,
     0x58f495a72d8dc7c,  0xb6d0a232181df436, 0x308186c3513b59ce,
     0x20c07220106735bc, 0x1e57f138a9b50800, 0xb27ed7460b005ffd,
     0xa121c15ce49afe65, 0x3b67d70e6790e175, 0x54067aeace73cf75,
     0x339f2ccfed4b46a3, 0x8828eec94bf0e45b, 0x811402a947f8a379,
     0x328fdd85f101030a, 0x73b518092a8e52f9, 0xb938ce6c0b0a6e1c,
     0x690752fd731c48ab, 0x382820b3dc759b0,  0x35725de5c414ee22,
     0x924c964e68eddf32, 0xad74e8792dc7cea6, 0x58d1d037111171f1,
     0xefd2cc034dccc7b8, 0x2d18289854e06e1},
    {0xe978684d16f45ec9, 0xeed0f142a9647fb9, 0x64038937ea94fb5c,
     0x9dabbcb474785b76, 0x5afc788458cbdbee, 0x644d741753243da6,
     0xd8cdb585aef40c97, 0x9a64cf3d74b2551e, 0x76634ccb011b526b,
     0x50c98c182ce2c776, 0xa56034298155a3d9, 0x7559b78c801d4126,
     0xbd95ff4bf442cad4, 0x7c0f6717059cdf5f, 0xcc42e6d646a4de57,
     0x50cdb4f1e268b34e, 0x8c509bb0a789ca05, 0x824ac0630ad7f0f,
     0x72c80d967db5cd19, 0x7af732115f6a9761, 0xc459beb5e9c521b5,
     0x8ba252dad40801e5, 0xe0af0bcc72fec70f, 0x87ce0deaf901053c,
     0xf87220163c0e8696, 0xde364614de64a446, 0x87c17b2ed8e922a8,
     0x184bebcfb6237344, 0x9a6ada6e38667db,  0xa8fc491e7e0f1f50,
     0x752212d20d1fdde4, 0x362ad2cd1f405877},
    {0xe8ca4c65df984c61, 0x6684fa9f3e223b9c, 0xcbe14bbd545896a9,
     0xbaba3d09a3de7818, 0x7458433c03899e93, 0x30dbb92099a75e4e,
     0x71e9470f49877dd3, 0x29ce78b9dd3cb672, 0x43787c7ce50422e1,
     0x5783cab3ed4d9c62, 0xc8ef78982a9d7dc8, 0x9e51d92df36fc8fc,
     0x1f07244f448e879,  0x275a97095c5800be, 0xc4bb4dedbfb3fa1a,
     0xc1ebfb6cb34bf3e4, 0xc58bf4fcb1c6e2ce, 0x6b70c9dacc9b241c,
     0x502d5860904fd435, 0x109d698b383b8c1f, 0x96c0df136f7dfee2,
     0x4fb1db2e7866d82c, 0xb3981adb7a000935, 0x5066a0ec505eee56,
     0x51097ab0f2a55bfa, 0xb3ab27704a22c3bb, 0x62a558920273600,
     0xe10f3da80a04aaaa, 0x6cc9f3ec0c39c5f3, 0x963f28c190ee4fc7,
     0x9fafda24ab0b4747, 0x8b963ffc9d459993},
    {0x907337ab331e42a6, 0xd8ec9044c7dc499c, 0x50f40a9f1254ac55,
     0xad00218ab190cbb9, 0x1ccb0de99f089832, 0xf6f05e3993c6639e,
     0xa90e481809392b89, 0x8b676cd1c974e6a6, 0x3c9a803234e0812f,
     0x7d5d7ff127fe46c6, 0x333b53bf6c44b7b8, 0x92f1b372d70ca60b,
     0xffba64c6cd3f7e39, 0xf9c6853a2f5b5a20, 0xc6dc784d6d54a191,
     0xfa06b6d390c4b6fd, 0xe278403b891c9225, 0x796ce30804d58d78,
     0x46564c69a9c22c2d, 0x9a2e5374905a21e2, 0xf0d65d150a4ee56c,
     0x8a277f9f892ca67d, 0xdd74b3613a5a7de4, 0xc0bbc635b5b60a25,
     0x96b599e615561722, 0xe855114fc5654372, 0xeb9d6adec5b6f81d,
     0x1e0d5afe2323dffe, 0xbece13ddbd419395, 0x5166b79643aa7ea4,
     0xdc5052f721e7c12c, 0x4e2159eaf7bbb0ce}};
#else
static const size_t zobrist_player = 0xfe23ecab;
static const size_t zobrist[4][32] = {
    {0x4d96c80c, 0x2d44d6f3, 0x7193ae62, 0x648d49f9, 0x8e7b0263, 0x798caa2,
     0x53cdbd89, 0x93615976, 0xb9f5be3e, 0xa6ae1304, 0xb6fb95d8, 0x1af3f6c7,
     0x2162b75,  0xe26ace92, 0xa13e53f1, 0x2b55080e, 0x8e1d7abf, 0x43e96560,
     0x5c3ab9d6, 0xdab865c0, 0x522a9268, 0x1644b600, 0x44a81f90, 0xf11a8bc,
     0xd1af5eb1, 0x58fd2a21, 0xe3d1bf3a, 0xc8bf217d, 0xfb302123, 0xef5fb7cf,
     0x43b6c649, 0xc940965},
    {0x3f92e630, 0xa156a8aa, 0xde8fb64f, 0x826b5e75, 0xd3fdfacf, 0xb3ba6d0a,
     0xc68281f9, 0xc732e685, 0xc9c65255, 0x13c6aaf6, 0x2a2280a9, 0xd3c39cfd,
     0xb4296a51, 0xc2224c61, 0x2d13a3e4, 0x70f9a6f2, 0x99e1cc30, 0x47b86dcb,
     0xf675e8a5, 0xe5c7ac7,  0xf3f2c1ea, 0xd7e3fb1f, 0xa320de52, 0xb275f3c3,
     0x7c3269b9, 0x7e6d9b66, 0x8ae0dc02, 0x940d0129, 0xf1d8f4f1, 0x422cb769,
     0xe7c6b593, 0x5b99db49},
    {0x1d9430da, 0xa616c0f9, 0xec2064e3, 0xe9c9d43,  0x1f71703e, 0xd19b4249,
     0x45721f4,  0x9025fa2b, 0xaba374cf, 0x587f7c3f, 0x12176baf, 0x989e7b0d,
     0xfbaf0ecc, 0x9c413012, 0xe98cde56, 0x37d021e8, 0x69830cd9, 0x4e7fd254,
     0x76898cd9, 0xf989444f, 0xf8dd6256, 0xd23e1509, 0x10c3840,  0xea82f9c2,
     0x10cd36e1, 0x1c1af6be, 0xb66ba323, 0x89513bd3, 0xeffa37b1, 0x1e8d940,
     0x4ff1c106, 0x9cbfb178},
    {0x4566e96b, 0xabce74c3, 0xd359776e, 0x78eb739b, 0xa7989c57, 0x506a76ef,
     0xe1acc802, 0x613c5999, 0xf0927592, 0x3baa754b, 0xd0c26fd1, 0x8552e457,
     0x700f8349, 0xe1770523, 0x53716e77, 0xacc83215, 0x45a3c521, 0xd0b953b3,
     0xa260b984, 0xc6eeb9bc, 0x648421dd, 0x5937e2ed, 0x17d5dfa,  0x7e8d4ea5,
     0x111c20f1, 0x4f4ce708, 0xea772a48, 0xef90cbf2, 0x877f2636, 0x2a3e60ef,
     0x81666e0c, 0x3e5f42b9}};
#endif

static const array<char, 4> moves_regular(char square, char mobility) {
  array<char, 4> retval{
      {char(square - 4), char(square - 3), char(square + 4), char(square + 5)}};
  if ((square - 1) & 4)
    for (char &a : retval)
      --a;
  const int mask = square & 7;

  if (square <= 4 || !(mobility & UP))
    retval[0] = retval[1] = 0;
  if (square >= 29 || !(mobility & DOWN))
    retval[2] = retval[3] = 0;

  if (mask == 4)
    retval[1] = retval[3] = 0;
  if (mask == 5)
    retval[0] = retval[2] = 0;

#ifndef NDEBUG
  for (char s : retval)
    assert(s < 33);
#endif

  return retval;
}

static const array<char, 4> moves_capture(char square, char mobility) {
  array<char, 4> retval{
      {char(square - 9), char(square - 7), char(square + 7), char(square + 9)}};
  const char mask = square & 7;

  if (square <= 8 || !(mobility & UP))
    retval[0] = retval[1] = 0;
  if (square >= 25 || !(mobility & DOWN))
    retval[2] = retval[3] = 0;

  if (mask == 4 || mask == 0)
    retval[1] = retval[3] = 0;
  if (mask == 5 || mask == 1)
    retval[0] = retval[2] = 0;

#ifndef NDEBUG
  for (char s : retval)
    assert(s < 33 && s >= 0);
#endif

  return retval;
}

static char get_captured_square(char now, char later) {
  const char offset = 1 - (((now - 1) >> 2) & 1);
  return (now + later) / 2 + offset;
}

bool position::capture_moves(vector<position> &list, const char square,
                             const unsigned piece) const {
  bool capture = false;
  array<char, 4> moves = moves_capture(
      square, (piece & pieces[to_play]) ? (to_play + 1) : (UP | DOWN));
  assert(piece & mine());

  for (const char m : moves) {
    assert(m <= 32);
    if (!m) // if the move is off the board, skip
      continue;

    const unsigned capturing_piece = square_to_piece(m);
    assert(__builtin_popcount(capturing_piece) <= 1);
    if (all() & capturing_piece) // if there's a piece where we're going, skip
      continue;

    const char captured_square = get_captured_square(square, m);
    const unsigned captured_piece = square_to_piece(captured_square);
    if (!(theirs() & captured_piece))
      continue; // if we're not capturing an enemy piece, skip

    position p{*this};

    const char capturing_king = (piece & pieces[to_play]) ? 0 : 2;
    assert(piece & pieces[to_play + capturing_king]);

    const char captured_king =
        (captured_piece & pieces[WHITE - to_play]) ? 0 : 2;
    assert(captured_piece & pieces[WHITE - to_play + captured_king]);

    p._hash ^= zobrist[p.to_play + capturing_king][square - 1];
    p._hash ^= zobrist[p.to_play + capturing_king][m - 1];
    p._hash ^= zobrist[WHITE - p.to_play + captured_king][captured_square - 1];

    p.pieces[p.to_play + capturing_king] ^= piece;
    p.pieces[p.to_play + capturing_king] ^= capturing_piece;
    p.pieces[WHITE - p.to_play + captured_king] ^= captured_piece;

    capture = true;
    if (!p.capture_moves(list, m, capturing_piece)) {
      list.emplace_back(p);
    }
  }
  return capture;
}

vector<position> position::moves() const {
  sanity();

  vector<position> captures;
  vector<position> regular;

  for (char king = 0; king <= 2; king += 2) {
    const char mobility = king ? (UP | DOWN) : (to_play + 1);
    for (unsigned a = pieces[to_play + king]; a;) {
      const unsigned piece = 0x80000000u >> __builtin_clz(a);
      const char square = 32 - __builtin_clz(a);

      assert(square == piece_to_square(piece));
      assert(piece == square_to_piece(square));

      capture_moves(captures, square, piece);

      if (captures.empty()) {
        for (char s : moves_regular(square, mobility)) {
          if (!s)
            continue;
          if (square_to_piece(s) & all())
            continue;

          position p{*this};
          const unsigned next_piece = square_to_piece(s);

          p._hash ^= zobrist[p.to_play + king][s - 1];
          p._hash ^= zobrist[p.to_play + king][square - 1];

          p.pieces[p.to_play + king] ^= piece;
          p.pieces[p.to_play + king] ^= next_piece;

          regular.emplace_back(p);
        }
      }

      a ^= piece;
    }
  }

  if (captures.empty())
    captures = std::move(regular);

  for (position &p : captures) {
    for (unsigned a = p.pieces[0] & 0xf0000000; a;) {
      const char square = 32 - __builtin_clz(a);
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      p._hash ^= zobrist[0][square - 1] ^ zobrist[2][square - 1];
      p.pieces[0] ^= piece;
      p.pieces[2] ^= piece;
      a ^= piece;
    }
    for (unsigned a = p.pieces[1] & 0x0000000f; a;) {
      const char square = 32 - __builtin_clz(a);
      const unsigned piece = 0x80000000u >> __builtin_clz(a);

      p._hash ^= zobrist[1][square - 1] ^ zobrist[3][square - 1];
      p.pieces[1] ^= piece;
      p.pieces[3] ^= piece;

      a ^= piece;
    }

    p._hash ^= zobrist_player;
    p.to_play = 1 - p.to_play;
  }

  return captures;
}

constexpr size_t start_hash(char a) {
  return a ? (start_hash(a - 1) ^ zobrist[0][a - 1] ^ zobrist[1][32 - a]) : 0;
}

void get_captured_square_test() {
  assert(get_captured_square(10, 19) == 15);
  assert(get_captured_square(10, 17) == 14);
  assert(get_captured_square(10, 1) == 6);
  assert(get_captured_square(10, 3) == 7);
  assert(get_captured_square(14, 5) == 9);
  assert(get_captured_square(14, 7) == 10);
  assert(get_captured_square(14, 21) == 17);
  assert(get_captured_square(14, 23) == 18);
}

position::position()
    : _hash{start_hash(12)}, pieces{{0x00000fff, 0xfff00000, 0, 0}},
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
  for (char i = 0; i < 8; i++) {
    if (!(i & 1))
      o << null_square;

    for (int j = 0; j < 4; j++) {
      const char square = 1 + i * 4 + j;
      const unsigned piece = square_to_piece(square);
      if (piece & p.pieces[0])
        o << black << regular;
      else if (piece & p.pieces[1])
        o << white << regular;
      else if (piece & p.pieces[2])
        o << black << king;
      else if (piece & p.pieces[3])
        o << white << king;
      else
        o << black << ' ';

      if (j != 3)
        o << null_square;
    }

    if (i & 1)
      o << null_square;
    o << color_reset << '\n';
  }

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

  size_t h = to_play == BLACK ? 0 : zobrist_player;

  for (int i = 4; i--;) {
    for (unsigned a = pieces[i]; a;) {
      const char square = 32 - __builtin_clz(a);
      const unsigned piece = 0x80000000u >> __builtin_clz(a);
      assert(piece == square_to_piece(square));
      assert(pieces[i] & piece);

      h ^= zobrist[i][(int)square - 1];
      a ^= piece;
    }
  }

  assert(h == _hash);
}
#endif
