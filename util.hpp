#pragma once

#define UP 2
#define DOWN 1

static inline unsigned mobility_mask(unsigned piece, char mobility) {
  unsigned retval = 0;
  if (mobility & UP)
    retval |= piece - 1;
  if (mobility & DOWN)
    retval |= piece ^ ~(piece - 1);
  return retval;
}

static inline unsigned get_jumps(unsigned piece, char mobility) {
  unsigned retval = 0;
  if (piece & 0xeeeeee00u)
    retval |= piece >> 9;
  if (piece & 0x77777700u)
    retval |= piece >> 7;
  if (piece & 0x00eeeeeeu)
    retval |= piece << 7;
  if (piece & 0x00777777u)
    retval |= piece << 9;
  return retval & mobility_mask(piece, mobility);
}

static inline unsigned get_moves(unsigned piece, char mobility) {
  unsigned retval = (piece >> 4) | (piece << 4);

  if (piece & 0xe0e0e0e0u) {
    retval |= piece >> 5;
    retval |= piece << 3;
  } else if (piece & 0x7070700u) {
    retval |= piece >> 3;
    retval |= piece << 5;
  }
  return retval & mobility_mask(piece, mobility);
}

class capture_iterator {
  unsigned capturing_board;
  unsigned capturing_piece;
  unsigned captured_board;
  unsigned captured_piece;

public:
  capture_iterator(unsigned piece, char mobility)
      : capturing_board{get_jumps(piece, mobility)},
        capturing_piece{0x80000000 >> __builtin_clz(capturing_board)},
        captured_board{get_moves(piece, mobility) & 0x07e7e7e0},
        captured_piece{0x80000000 >> __builtin_clz(captured_board)} {}
  bool valid() const { return capturing_board; }
  void operator++() {
    capturing_board ^= capturing_piece;
    capturing_piece = 0x80000000u >> __builtin_clz(capturing_board);
    captured_board ^= captured_piece;
    captured_piece = 0x80000000u >> __builtin_clz(captured_board);
  }
  unsigned capturing() { return capturing_piece; }
  unsigned captured() { return captured_piece; }
};

class board_iterator {
  unsigned piece;
  unsigned board;

public:
  board_iterator(unsigned board)
      : piece{0x80000000u >> __builtin_clz(board)}, board{board} {}
  unsigned operator*() const { return piece; }
  bool valid() const { return board; }
  void operator++() {
    board ^= piece;
    piece = 0x80000000u >> __builtin_clz(board);
  }
};

void gen_hash_64();
