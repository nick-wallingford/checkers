#include "agent.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;

constexpr const int inf = numeric_limits<int>::max();

int alphabeta::eval(const position &p, int d, int alpha, int beta) {
  vector<position> moves = p.moves();
  if (moves.empty())
    return -inf;

  if (d <= 0) {
    if ((depth & 1) == (side == BLACK))
      return e(p);
    else
      return -e(p);
  }

  int &cached_score = cache(d, p.hash());
  if (cached_score != numeric_limits<int>::min())
    return cached_score;

  int best_score = -inf;
  for (const position &next : moves) {
    best_score = max(best_score, -eval(next, d - 1, -beta, -alpha));
    alpha = max(alpha, best_score);
    if (alpha > beta)
      return cached_score = best_score;
  }

  return cached_score = best_score;
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
