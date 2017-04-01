#include "agent.hpp"

#include <cassert>
#include <iostream>
#include <limits>
#include <vector>

#ifdef MEASURE_BRANCHING_FACTOR
#include <atomic>
#endif

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

int alphabeta::eval(const position &p, int d, int alpha, int beta) {
  assert(alpha <= beta);
  if (d <= 0) {
    if ((depth & 1) == (side == BLACK))
      return e(p);
    else
      return -e(p);
  }

  vector<position> moves = p.moves();
#ifdef MEASURE_BRANCHING_FACTOR
  int branches = 0;
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
#endif

  if (moves.empty())
    return p.player() == side ? -inf + depth - d : inf - depth + d;

  int best_score = -inf;

  for (const position &next : moves) {
#ifdef MEASURE_BRANCHING_FACTOR
    branches++;
#endif
    const int score = -eval(next, d - 1, -beta, -alpha);
    best_score = max(best_score, score);
    if (best_score > beta)
      break;
    alpha = max(alpha, best_score);
  }
#ifdef MEASURE_BRANCHING_FACTOR
  branch_sum += branches;
  branch_square_sum += branches * branches;
#endif
  return best_score;
}

position alphabeta::get_move(const position &p) {
  vector<position> moves = p.moves();
#ifdef MEASURE_BRANCHING_FACTOR
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
  branch_sum += moves.size();
  branch_square_sum += moves.size() * moves.size();
#endif

  if (moves.size() == 1)
    return moves[0];
  else if (moves.empty())
    throw resign();

  for (int i = 0; i + 1 < (int)moves.size(); i++)
    swap(moves[i],
         moves[uniform_int_distribution<int>{i, (int)moves.size() - 1}(r)]);

  int best_score = -inf;
  position best_position = moves.front();

  for (const position &next : moves) {
    const int score = -eval(next, depth, -inf, -best_score);
    if (score > best_score) {
      best_score = score;
      best_position = next;
    }
  }

  return best_position;
}

#ifdef MEASURE_BRANCHING_FACTOR
alphabeta::~alphabeta() {
  branch_sum_g += branch_sum;
  branch_square_sum_g += branch_square_sum;
  move_sum_g += move_sum;
  move_square_sum_g += move_square_sum;
  branch_count_g += branch_count;
}

void alphabeta::report_branching() {
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
