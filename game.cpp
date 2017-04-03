#include "game.hpp"

#include <iostream>
#include <ncursesw/ncurses.h>

#include "alphabeta_pv.hpp"
#include "position.hpp"

using namespace std;

enum color { null = 1, black, white };

class selection_window {
  WINDOW *win;
  const int height;

public:
  selection_window(int rows, int cols)
      : win{newwin(rows - 2, cols - 15, 1, 14)}, height(rows - 4) {
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(win);
  }
  selection_window(const selection_window &) = delete;
  ~selection_window() { delwin(win); }
  void operator()(const vector<string> &s, int n) {
    wclear(win);
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');

    for (int i = s.size(); i--;) {
      if (n == i)
        wattron(win, COLOR_PAIR(null));
      mvwprintw(win, height - i, 1, s[i].c_str());
      if (n == i)
        wattroff(win, COLOR_PAIR(null));
    }

    refresh();
    wrefresh(win);
  }
};

class game_window {
  WINDOW *win;

public:
  game_window(int row) : win{newwin(10, 10, row, 2)} {
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(win);
  }
  game_window(const game_window &) = delete;
  ~game_window() { delwin(win); }
  void operator()(const position &p) {
    wmove(win, 1, 1);
    int y = 1;
    for (unsigned piece = 1; piece; piece <<= 1) {
      if (piece & 0x0f0f0f0fu) {
        wattron(win, COLOR_PAIR(null));
        waddch(win, ' ');
        wattroff(win, COLOR_PAIR(null));
      }

      if (piece & p[0]) {
        wattron(win, COLOR_PAIR(black));
        waddstr(win, "●");
        wattroff(win, COLOR_PAIR(black));
      } else if (piece & p[1]) {
        wattron(win, COLOR_PAIR(white));
        waddstr(win, "●");
        wattroff(win, COLOR_PAIR(white));
      } else if (piece & p[2]) {
        wattron(win, COLOR_PAIR(black));
        waddstr(win, "♣");
        wattroff(win, COLOR_PAIR(black));
      } else if (piece & p[3]) {
        wattron(win, COLOR_PAIR(white));
        waddstr(win, "♣");
        wattroff(win, COLOR_PAIR(white));
      } else {
        wattron(win, COLOR_PAIR(white));
        waddch(win, ' ');
        wattroff(win, COLOR_PAIR(white));
      }

      if (piece & 0xf0f0f0f0u) {
        wattron(win, COLOR_PAIR(null));
        waddch(win, ' ');
        wattroff(win, COLOR_PAIR(null));
      }
      if (piece & 0x88888888u)
        wmove(win, ++y, 1);
    }
    refresh();
    wrefresh(win);
  }
  void clear() {
    wclear(win);
    wborder(win, '|', '|', '-', '-', '+', '+', '+', '+');
    refresh();
    wrefresh(win);
  }
};

class game_manager {
  game_window move_win;
  game_window current_win;
  selection_window sel_win;
  alphabeta_pv agent;

public:
  game_manager(int rows, int cols, alphabeta_pv &&a)
      : move_win{rows - 11}, current_win{rows - 22}, sel_win{rows, cols},
        agent(move(a)) {}

  void operator()() {
    position p;

    try {
      for (int i = 0; i < 256; i++) {
        if (p.player() == agent.get_side())
          p = agent.get_move(p);
        current_win(p);

        const vector<position> moves{p.moves()};
        if (moves.empty()) {
          move_win.clear();
          sel_win({"You have been defeated."}, 0);
          getch();
          return;
        }

        vector<string> move_str;
        for (unsigned j = 0; j < moves.size(); j++)
          move_str.emplace_back(to_string(j) + ". " +
                                to_string(moves[j].hash()));

        int n = 0;
        while (1) {
          move_win(moves[n]);
          sel_win(move_str, n);

          int ch = getch();
          if (ch == KEY_DOWN) {
            if (--n < 0)
              n = 0;
          } else if (ch == KEY_UP) {
            if (++n >= (int)moves.size())
              n = moves.size() - 1;
          } else if (ch == 10) {
            break;
          }
        }
        p = moves[n];
      }
      move_win.clear();
      sel_win({"The game has ended in a draw."}, 0);
      getch();
      return;
    } catch (agent::resign) {
      move_win.clear();
      sel_win({"The computer has resigned. You win!"}, 0);
      getch();
      return;
    }
  }
};

void play_game() {
  int row, col;

  int depth = 0;
  do {
    cout << "Enter the desired search depth: ";
    cin >> depth;
  } while (depth <= 0 && depth >= 20);

  int player = 0;
  do {
    cout << "Enter 2 to play as black, 1 to play as white: ";
    cin >> player;
  } while (player < 1 || player > 2);

  initscr();
  if (has_colors() == FALSE) {
    endwin();
    cout << "Terminal does not support colors." << endl;
    return;
  }
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  start_color();

  init_pair(null, COLOR_BLACK, COLOR_WHITE);
  init_pair(black, COLOR_RED, COLOR_GREEN);
  init_pair(white, COLOR_WHITE, COLOR_GREEN);

  getmaxyx(stdscr, row, col);

  game_manager w{row, col, {best_heuristic(), depth, (char)(player - 1)}};
  w();

  endwin();
}
