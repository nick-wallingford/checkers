#include "agent.hpp"

#include <cassert>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;

// We use -max() instead of min() because we have to negate them a lot.
// -min() is zero.

constexpr const int inf = numeric_limits<int>::max();

int minimax::minimize(const position &p, int depth) const {
  if (depth <= 0) {
    if (side == BLACK)
      return e(p);
    else
      return -e(p);
  }

  int best_score = inf;

  for (const position &next : p.moves()) {
    const int score = maximize(next, depth - 1);
    if (score < best_score)
      best_score = score;
  }

  return best_score;
}

int minimax::maximize(const position &p, int depth) const {
  if (depth <= 0) {
    if (side == BLACK)
      return e(p);
    else
      return -e(p);
  }
  int best_score = -inf;

  for (const position &next : p.moves()) {
    const int score = minimize(next, depth - 1);
    if (score > best_score)
      best_score = score;
  }

  return best_score;
}

position minimax::get_move(const position &p) {
  const vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];

  vector<position> candidates;
  int best_score = -inf;

  for (const position &next : moves) {
    const int score = minimize(next, depth);
    if (score > best_score) {
      candidates.clear();
      candidates.push_back(next);
      best_score = score;
    } else if (score == best_score) {
      candidates.push_back(next);
    }
  }

  if (best_score == -inf)
    throw resign();

  assert(!candidates.empty());

  cout << e(p) << ' ' << best_score << ' ' << candidates.size() << endl;

  const int ret =
      uniform_int_distribution<int>{0, (int)candidates.size() - 1}(r);
  return candidates[ret];
}
