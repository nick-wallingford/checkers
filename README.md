# checkers
AI project to play checkers.

Can be compiled on most *nix based systems with the `make` command.

A debug build for use with GDB or Valgrind can be built with `make debug`.
Diagnostics for average branch factor can be enabled with
`CXXFLAGS=-DMEASURE_BRANCHING_FACTOR make`

The application can be run with ./checkers, displaying a usage summary.
A game vs the AI can be played with `./checkers --game`. The user will be prompted
for a search depth and which side to play as. A search depth of 14 results in
play of 3-10 seconds per computer move, while a search depth of 10 results in 1-2
seconds per move. Even search depths are preferred.

Alternatively, the computer can be instructed to play against itself with
`./checkers --cpu-game` with an optional depth argument, or the training mode
for the heuristic can be invoked with `./checkers --train`. If the executable
was built with average branch factor diagnostics, this mode can be invoked
with `./checkers --branch`.

Ensure UTF-8 support is enabled. In PuTTY, select:
Configure -> Window -> Translation -> Remote character set -> UTF-8
