#include "agent.hpp"

#include <cassert>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;

// We use -max() instead of min() because we have to negate them a lot.
// -min() is zero.
constexpr const int neg_inf = -numeric_limits<int>::max();

int minimax::recursive_eval(const position &p, int d) const {
  int best_score = neg_inf;
  const vector<position> moves = p.moves();
  if (moves.empty())
    return best_score;
  if (d == 0) {
    if (side)
      return -e(p);
    else
      return e(p);
  }
  for (const position &p : moves)
    best_score = max(-recursive_eval(p, d - 1), best_score);

  return best_score;
}

position minimax::get_move(const position &p) {
  const vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];

  vector<position> candidates;
  int best_score = neg_inf;

  for (const position &p : moves) {
    const int score = recursive_eval(p, depth);
    if (score > best_score) {
      candidates.clear();
      candidates.push_back(p);
      best_score = score;
    }
  }

  if (best_score == neg_inf)
    throw resign();

  assert(!candidates.empty());

  return candidates[0];
}
