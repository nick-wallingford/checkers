#include "alphabeta_pv.hpp"

#include <atomic>

#include "position.hpp"

using namespace std;

#ifdef MEASURE_BRANCHING_FACTOR
static atomic<uint64_t> branch_sum_g{0};
static atomic<uint64_t> branch_square_sum_g{0};
static atomic<uint64_t> move_sum_g{0};
static atomic<uint64_t> move_square_sum_g{0};
static atomic<uint64_t> branch_count_g{0};
#endif

constexpr const int inf = numeric_limits<int>::max();

void alphabeta_pv::sort(vector<position> &moves, uint64_t pv) {
  swap(moves.front(), moves[uniform_int_distribution<unsigned>{
                          0, (unsigned)moves.size() - 1}(r)]);
  vector<position>::iterator front_it = moves.begin();
  for (unsigned i = moves.size(); --i;)
    if (moves[i].hash() == pv)
      swap(*front_it++, moves[i]);
    else
      swap(moves[1], moves[uniform_int_distribution<unsigned>{1, i}(r)]);
}

alphabeta_pv::alphabeta_pv(const heuristic &e, int d, char side)
    : agent{e, d, side}, mask_max{(1 << 20) - 1}, mask_min{(1 << 19) - 1},
      pv_max(new uint64_t[mask_max + 1]()), pv_min(new uint64_t[mask_max + 1]())
#ifdef MEASURE_BRANCHING_FACTOR
      ,
      branch_sum(0), branch_square_sum(0), move_sum(0), move_square_sum(0),
      branch_count(0)
#endif
{
  position p;
  for (int i = 0; i < depth; i += 2)
    eval(p, i, -inf, inf, side == BLACK);
}

int alphabeta_pv::eval(const position &p, unsigned char d, int alpha, int beta,
                       bool maximize) {
  if (!d)
    return side == BLACK ? e(p) : -e(p);

  uint64_t &pv = maximize ? pv_max[p.hash() & (size_t)mask_max]
                          : pv_min[p.hash() & (size_t)mask_min];
  vector<position> moves = p.moves();

#ifdef MEASURE_BRANCHING_FACTOR
  int branches = 0;
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
#endif
  if (moves.empty())
    return p.player() == side ? -inf + depth - d : inf - depth + d;

  sort(moves, pv);

  if (maximize) {
    for (const position &p : moves) {
      const int score = eval(p, d - 1, alpha, beta, false);
#ifdef MEASURE_BRANCHING_FACTOR
      branches++;
#endif
      if (score > alpha) {
        alpha = score;
        pv = p.hash();
        if (alpha >= beta) {
#ifdef MEASURE_BRANCHING_FACTOR
          branch_sum += branches;
          branch_square_sum += branches * branches;
#endif
          return beta;
        }
      }
    }
#ifdef MEASURE_BRANCHING_FACTOR
    branch_sum += branches;
    branch_square_sum += branches * branches;
#endif
    return alpha;
  } else {
    for (const position &p : moves) {
      const int score = eval(p, d - 1, alpha, beta, true);
#ifdef MEASURE_BRANCHING_FACTOR
      branches++;
#endif
      if (score < beta) {
        beta = score;
        pv = p.hash();
        if (alpha >= beta) {
#ifdef MEASURE_BRANCHING_FACTOR
          branch_sum += branches;
          branch_square_sum += branches * branches;
#endif
          return alpha;
        }
      }
    }
#ifdef MEASURE_BRANCHING_FACTOR
    branch_sum += branches;
    branch_square_sum += branches * branches;
#endif
    return beta;
  }
}

position alphabeta_pv::get_move(const position &p) {
  uint64_t &pv = pv_max[p.hash() & (size_t)mask_max];
  vector<position> moves = p.moves();
#ifdef MEASURE_BRANCHING_FACTOR
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
  branch_sum += moves.size();
  branch_square_sum += moves.size() * moves.size();
#endif
  if (moves.empty())
    throw resign();

  sort(moves, pv);

  position best_position = moves.front();
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

#ifdef MEASURE_BRANCHING_FACTOR
alphabeta_pv::~alphabeta_pv() {
  branch_sum_g += branch_sum;
  branch_square_sum_g += branch_square_sum;
  move_sum_g += move_sum;
  move_square_sum_g += move_square_sum;
  branch_count_g += branch_count;
}

void alphabeta_pv::report_branching() {
  if (!branch_count_g)
    return;

  const double branch_square_sum = branch_square_sum_g;
  const double branch_sum = branch_sum_g;
  const double move_sum = move_sum_g;
  const double move_square_sum = move_square_sum_g;
  const int64_t count = branch_count_g;

  const double branch_mean = branch_sum / count;
  const double branch_variance =
      (branch_square_sum - branch_sum * branch_mean) / (count - 1);
  const double branch_sqrt = sqrt(branch_variance);

  const double move_mean = move_sum / count;
  const double move_variance =
      (move_square_sum - move_sum * move_mean) / (count - 1);
  const double move_sqrt = sqrt(move_variance);

  cout << "Positions evaluated: " << count << endl
       << "Average branching factor: " << branch_mean << endl
       << "Branching standard deviation: " << branch_sqrt << endl
       << "Average moves: " << move_mean << endl
       << "Moves standard deviation: " << move_sqrt << endl;
}
#endif
