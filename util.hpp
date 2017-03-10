#pragma once

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
  char square() const { return 32 - __builtin_clz(piece); }
};

void gen_hash_64();
