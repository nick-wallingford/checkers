#include "game.hpp"

#include <ncurses.h>

void game() {
  initscr();
  printw("Hello world!");
  refresh();
  getch();
  endwin();
}
