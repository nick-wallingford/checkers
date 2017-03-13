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
  const vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];
  else if (moves.empty())
    throw resign();

  vector<position> candidates;
  int best_score = -inf;
  cout << (side == BLACK ? e(p) : -e(p)) << ' ' << flush;

  for (const position &next : moves) {
    const int score = -eval(next, depth, -inf, -best_score);
    if (score > best_score) {
      candidates.clear();
      candidates.push_back(next);
      best_score = score;
    } else if (score == best_score) {
      candidates.push_back(next);
    }
  }

  cout << best_score << ' ' << candidates.size() << endl;

  const int ret =
      uniform_int_distribution<int>{0, (int)candidates.size() - 1}(r);
  return candidates[ret];
}
