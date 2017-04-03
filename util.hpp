#pragma once

#define UP 2
#define DOWN 1

// Returns a mask of squares above and/or below a piece, depending on mobility.
static inline unsigned mobility_mask(unsigned piece, char mobility) {
  unsigned retval = 0;
  if (mobility & UP)
    retval |= piece - 1;
  if (mobility & DOWN)
    retval |= piece ^ ~(piece - 1);
  return retval;
}

// Returns the squares a piece can jump to, should there be a piece between
// them.
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

// Returns the squares a piece can make regular moves to, should those squares
// be empty.
static inline unsigned get_moves(unsigned piece, char mobility) {
  unsigned retval = (piece >> 4) | (piece << 4);

  if (piece & 0xe0e0e0e0u) {
    retval |= piece >> 5;
    retval |= piece << 3;
  } else if (piece & 0x07070707u) {
    retval |= piece >> 3;
    retval |= piece << 5;
  }
  return retval & mobility_mask(piece, mobility);
}

// Iterates over pairs of squares a piece can capture, gives both the square the
// piece moves to as well as the piece the square is capturing.
// Should be used like:
// for(capture_iterator it{piece,mobility}; it.valid(); ++it) { ... }
class capture_iterator {
  unsigned capturing_board;
  unsigned capturing_piece;
  unsigned captured_board;
  unsigned captured_piece;

public:
  capture_iterator(unsigned piece, char mobility)
      : capturing_board{get_jumps(piece, mobility)},
        capturing_piece{0x80000000u >> __builtin_clz(capturing_board)},
        captured_board{get_moves(piece, mobility) & 0x07e7e7e0u},
        captured_piece{0x80000000u >> __builtin_clz(captured_board)} {}
  // Returns true if the iterator has more captures to give
  bool valid() const { return capturing_board; }
  // Updates the iterator.
  void operator++() {
    capturing_board ^= capturing_piece;
    capturing_piece = 0x80000000u >> __builtin_clz(capturing_board);
    captured_board ^= captured_piece;
    captured_piece = 0x80000000u >> __builtin_clz(captured_board);
  }
  // Returns the square a piece while move to while it is jumping.
  unsigned capturing() { return capturing_piece; }
  // Returns the piece that would be being captured.
  unsigned captured() { return captured_piece; }
};

// Iterates over the pieces on a board.
// Should be used like:
// for(board_iterator it{board}; it.valid(); ++it) { ... }
class board_iterator {
  unsigned piece;
  unsigned board;

public:
  board_iterator(unsigned board)
      : piece{0x80000000u >> __builtin_clz(board)}, board{board} {}
  // Returns the piece for this iteration.
  unsigned operator*() const { return piece; }
  // Returns true if there are still pieces to iterator over.
  bool valid() const { return board; }
  // Updates the iterators state.
  void operator++() {
    board ^= piece;
    piece = 0x80000000u >> __builtin_clz(board);
  }
};

void gen_hash_64();
