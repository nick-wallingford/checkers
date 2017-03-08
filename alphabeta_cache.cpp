#include "alphabeta_cache.hpp"

#include <cassert>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;
constexpr const int inf = numeric_limits<int>::max();

int alphabeta_cache::eval(const position &p, unsigned char d, int alpha,
                          int beta, bool maximize) {
  bool set_exact = true;
  const vector<position> moves = p.moves();
  if (moves.empty())
    return p.player() == side ? -inf : inf;

  if (!d)
    return (side == BLACK) ? e(p) : -e(p);

  int best_score = maximize ? -inf : inf;

  for (const position &next : moves) {
    const int score = eval(next, d - 1, alpha, beta, !maximize);
    if (maximize) {
      best_score = max(best_score, score);
      alpha = max(alpha, best_score);
    } else {
      best_score = min(best_score, score);
      beta = min(beta, best_score);
    }
    if (beta < alpha) {
      set_exact = false;
      break;
    }
  }

  return best_score;
}

position alphabeta_cache::get_move(const position &p) {
  const vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];
  else if (moves.empty())
    throw resign();

  vector<position> candidates;
  int best_score = -inf;

  cout << (side == BLACK ? e(p) : -e(p)) << ' ' << flush;

  for (const position &next : moves) {
    const int score = eval(next, depth - 1, best_score, inf, false);
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
