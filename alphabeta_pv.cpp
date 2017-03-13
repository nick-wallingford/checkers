#include "alphabeta_pv.hpp"

#include "position.hpp"

using namespace std;

constexpr const int inf = numeric_limits<int>::max();

alphabeta_pv::alphabeta_pv(const heuristic &e, int d, char side)
    : agent{e, d, side}, mask_max{(1 << 20) - 1}, mask_min{(1 << 19) - 1},
      pv_max{new uint64_t[mask_max + 1]()},
      pv_min{new uint64_t[mask_max + 1]()} {
  position p;
  for (int i = 0; i < depth; i += 2)
    eval(p, i, -inf, inf, true);
}

int alphabeta_pv::eval(const position &p, unsigned char d, int alpha, int beta,
                       bool maximize) {
  if (!d)
    return side == BLACK ? e(p) : -e(p);

  uint64_t &pv = maximize ? pv_max[p.hash() & (size_t)mask_max]
                          : pv_min[p.hash() & (size_t)mask_min];

  vector<position> moves = p.moves();
  if (moves.empty())
    return p.player() == side ? -inf + depth - d : inf - depth + d;

  for (vector<position>::iterator it = ++moves.begin(); it != moves.end(); ++it)
    if (it->hash() == pv) {
      swap(*it, moves.front());
      break;
    }

  if (maximize) {
    for (const position &p : moves) {
      const int score = eval(p, d - 1, alpha, beta, false);
      if (score > alpha) {
        alpha = score;
        pv = p.hash();
        if (alpha >= beta)
          return beta;
      }
    }
    return alpha;
  } else {
    for (const position &p : moves) {
      const int score = eval(p, d - 1, alpha, beta, true);
      if (score < beta) {
        beta = score;
        pv = p.hash();
        if (alpha >= beta)
          return alpha;
      }
    }
    return beta;
  }
}

position alphabeta_pv::get_move(const position &p) {
  uint64_t &pv = pv_max[p.hash() & (size_t)mask_max];
  vector<position> moves = p.moves();
  if (moves.empty())
    throw resign();

  position best_position = moves.front();

  swap(moves.front(), moves[uniform_int_distribution<unsigned>{
                          0, (unsigned)moves.size() - 1}(r)]);
  for (unsigned i = moves.size(); --i;)
    if (moves[i].hash() == pv)
      swap(moves.front(), moves[i]);
    else
      swap(moves[1], moves[uniform_int_distribution<unsigned>{1, i}(r)]);

  int best_score = -inf;
  for (const position &p : moves) {
    const int score = eval(p, depth, best_score, inf, false);
    if (score > best_score) {
      best_score = score;
      best_position = p;
    }
  }

  return best_position;
}
