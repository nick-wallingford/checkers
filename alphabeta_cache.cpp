#include "alphabeta_cache.hpp"

#include <cassert>
#include <limits>
#include <vector>

#include "position.hpp"

using namespace std;
constexpr const int inf = numeric_limits<int>::max();

int alphabeta_cache::eval(const position &p, unsigned char d, int alpha,
                          int beta, bool maximize, node_type type) {
  vector<position> moves = p.moves();
  if (moves.empty())
    return p.player() == side ? -inf : inf;

  if (!d) {
    const int retval = side == BLACK ? e(p) : -e(p);
    return retval;
  }

  int best_score = maximize ? -inf : inf;

  if (tt.lookup(p, d, alpha, beta, best_score))
    return best_score;

  tt.sort(moves, maximize);
  node_type child_type = type == all ? cut : type == cut ? all : pv;

  for (const position &p : moves) {
    const int score = eval(p, d - 1, alpha, beta, !maximize, child_type);
    if (child_type == pv)
      child_type = cut;

    if (maximize) {
      best_score = max(score, best_score);
      alpha = max(alpha, best_score);
    } else {
      best_score = min(score, best_score);
      beta = min(beta, best_score);
    }

    if (beta < alpha) {
      break;
    }
  }

  tt.insert(p, best_score, d, type);

  return best_score;
}

position alphabeta_cache::get_move(const position &p) {
  const vector<position> moves = p.moves();
  if (moves.size() == 1)
    return moves[0];
  else if (moves.empty())
    throw resign();

  vector<position> candidates;

  cout << (side == BLACK ? e(p) : -e(p)) << ' ' << flush;
  ++tt;

  for (int i = 1; i < depth; i++) {
    eval(p, i, -inf, inf, true, pv);
    tt.sort(moves, true);
  }

  int best_score = -inf;
  for (const position &next : moves) {
    const int score = eval(next, depth, best_score, inf, false,
                           best_score == -inf ? pv : cut);
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

void transposition_table::sort(std::vector<position> moves, bool maximize) {
  if (moves.size() <= 1)
    return;
  vector<int> score;
  score.reserve(moves.size());
  int64_t sum = 0;

  for (int i = moves.size(); i--;) {
    const uint64_t hash = moves[i].hash();
    int best_score = maximize ? -inf : inf;

    for (index j{hash, mask}; j.has_next(); ++j) {
      const node &n = table[*j];
      if (hash == n.hash) {
        if (maximize)
          best_score = max(best_score, n.score);
        else
          best_score = min(best_score, n.score);
      }
    }

    sum += best_score;
    score.push_back(best_score);
  }

  // insertion sort
  if (maximize)
    for (size_t i = 1; i < moves.size(); i++) {
      const position p = moves[i];
      const int s = score[i];
      int j;
      for (j = i - 1; j >= 0 && score[j] < s; j--) {
        moves[j + 1] = moves[j];
        score[j + 1] = score[j];
      }
      moves[j + 1] = p;
      score[j + 1] = s;
    }
  else
    for (size_t i = 1; i < moves.size(); i++) {
      const position p = moves[i];
      const int s = score[i];
      int j;
      for (j = i - 1; j >= 0 && score[j] > s; j--) {
        moves[j + 1] = moves[j];
        score[j + 1] = score[j];
      }
      moves[j + 1] = p;
      score[j + 1] = s;
    }

  for (int s : score)
    sum -= s;
}

bool transposition_table::lookup(const position &p, int depth, int &alpha,
                                 int &beta, int &score) {
  for (index i{p.hash(), mask}; i.has_next() && table[*i].age == age; ++i) {
    const node &n = table[*i];

    if (n.hash == p.hash() && n.depth >= depth) {
      switch (n.type) {
      case pv:
        score = n.score;
        return true;
      case cut:
        alpha = max(alpha, n.score);
        break;
      case all:
        beta = min(beta, n.score);
        break;
      }
    }
  }

  return false;
}

void transposition_table::insert(const position &p, int score, char depth,
                                 node_type type) {
  for (index i{p.hash(), mask}; i.has_next(); ++i) {
    node &n = table[*i];

    if (n.age != age) {
      n.hash = p.hash();
      n.score = score;
      n.depth = depth;
      n.type = type;
      n.age = age;
      return;
    } else if (p.hash() == n.hash && depth >= n.depth) {
      n.depth = depth;
      n.type = type;
      n.score = score;

      return;
    }
  }
}
