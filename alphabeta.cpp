#include "agent.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;

constexpr const int inf = numeric_limits<int>::max();

int alphabeta::eval(const position &p, int d, int alpha, int beta) {
  assert(alpha <= beta);
  vector<position> moves = p.moves();
  if (moves.empty())
    return -inf;

  if (d <= 0) {
    if ((depth & 1) == (side == BLACK))
      return e(p);
    else
      return -e(p);
  }

  int best_score = -inf;

  for (const position &next : moves) {
    const int score = -eval(next, d - 1, -beta, -alpha);
    best_score = max(best_score, score);
    if (best_score > beta)
      break;
    alpha = max(alpha, best_score);
  }

  return best_score;
}

position alphabeta::get_move(const position &p) {
  vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];
  else if (moves.empty())
    throw resign();

  for (int i = 0; i + 1 < (int)moves.size(); i++)
    swap(moves[i],
         moves[uniform_int_distribution<int>{i, (int)moves.size() - 1}(r)]);

  int best_score = -inf;
  position best_position = moves.front();
  cout << (side == BLACK ? e(p) : -e(p)) << ' ' << flush;

  for (const position &next : moves) {
    const int score = -eval(next, depth, -inf, -best_score);
    if (score > best_score) {
      best_score = score;
      best_position = next;
    }
  }

  cout << best_score << endl;
  return best_position;
}
