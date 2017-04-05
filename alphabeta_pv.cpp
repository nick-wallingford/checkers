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

// So this isn't actually infinity, it's just very large.
constexpr const int inf = 1000000000;

// This does *NOT* actually sort the moves.
// It just randomizes the moves, except for moves which match the PV hash. These
// moves are promoted to the front of the list. Except in extraordinarily
// unusual circumstances, only one move should match pv. With fairly high
// probability, one move should match pv.
void alphabeta_pv::sort(vector<position> &moves, uint8_t pv) {
  // Move the front move into a random position in the list.
  swap(moves.front(), moves[uniform_int_distribution<unsigned>{
                          0, (unsigned)moves.size() - 1}(r)]);
  vector<position>::iterator front_it = moves.begin();
  for (unsigned i = moves.size(); --i;)
    if ((uint8_t)moves[i].hash() == pv) // Move a PV match to the front.
      swap(*front_it++, moves[i]);
    else // Otherwise put it into a random position.
      swap(moves[1], moves[uniform_int_distribution<unsigned>{1, i}(r)]);
}

constexpr size_t pow(size_t base, int exp) {
  return exp == 0 ? 1
                  : (exp & 1) ? base * pow(base, exp)
                              : pow(base, exp / 2) * pow(base, exp / 2);
}

constexpr size_t mask_calc(int depth) { return pow(2, depth + 14) - 1; }

alphabeta_pv::alphabeta_pv(const heuristic &e, int d, char side)
    : agent{e, d, side}, mask_max{mask_calc(d)}, mask_min{mask_calc(d - 1)},
      pv_max(new uint8_t[mask_max + 1]()), pv_min(new uint8_t[mask_max + 1]())
#ifdef MEASURE_BRANCHING_FACTOR
      ,
      branch_sum(0), branch_square_sum(0), move_sum(0), move_square_sum(0),
      branch_count(0)
#endif
{
  // Build up the entries in pv_max/pv_min. This moderately slows down
  // construction, but significantly speeds up the first call to get_move()
  position p;
  for (int i = 0; i < depth; i += 2)
    eval(p, i, -inf, inf, side == BLACK);
}

// Evaluate a position.
// p: the position to evaluate.
// d: the depth to evaluate to.
// alpha/beta: minmium/maximum bounds on scores we accept.
// maximize: true => we are trying to maximize the score at this depth.
int alphabeta_pv::eval(const position &p, unsigned char d, int alpha, int beta,
                       bool maximize) {
  // Base case. Return the true value of this position.
  if (!d)
    return side == BLACK ? e(p) : -e(p);

  // pv is a reference to the hash value in the
  uint8_t &pv = maximize ? pv_max[p.hash() & (size_t)mask_max]
                         : pv_min[p.hash() & (size_t)mask_min];
  vector<position> moves = p.moves();

#ifdef MEASURE_BRANCHING_FACTOR
  int branches = 0;
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
#endif
  // No moves are available: this is a loss for whoever's turn it is.
  if (moves.empty())
    // The depth/d is to delay a loss as long as possible, and to prioritize a
    // win as soon as possible
    return p.player() == side ? -inf + depth - d : inf - depth + d;

  // Pull the pv to the front, randomize the remaining moves.
  sort(moves, pv);
  // Just in case we didn't pull the pv to the front.
  pv = moves[0].hash();

  if (maximize) {
    for (const position &p : moves) {
      const int score = eval(p, d - 1, alpha, beta, false);
#ifdef MEASURE_BRANCHING_FACTOR
      branches++;
#endif
      // This move is a good one
      if (score > alpha) {
        alpha = score;
        // Update the hash table
        pv = p.hash();
        // We have also invalidated this move by alpha-beta pruning.
        if (alpha >= beta) {
#ifdef MEASURE_BRANCHING_FACTOR
          branch_sum += branches;
          branch_square_sum += branches * branches;
#endif
          // Fail hard.
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
      // This move is a good one.
      if (score < beta) {
        beta = score;
        // update the hash table
        pv = p.hash();
        // We have failed the alpha-beta condition. (which is a good thing)
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

// Gets the 'best' move from a given starting point.
// Throws agent::resign if the match is lost.
position alphabeta_pv::get_move(const position &p) {
  uint8_t &pv = pv_max[p.hash() & (size_t)mask_max];
  vector<position> moves = p.moves();
#ifdef MEASURE_BRANCHING_FACTOR
  branch_count++;
  move_sum += moves.size();
  move_square_sum += moves.size() * moves.size();
  branch_sum += moves.size();
  branch_square_sum += moves.size() * moves.size();
#endif
  // We have lost. Throw in the towel.
  if (moves.empty())
    throw resign();

  // Again, this does not sort. It only moves the PV move to the front.
  sort(moves, pv);

  position best_position = moves.front();
  int best_score = -inf;

  // linear search the move list for the best move.
  for (const position &p : moves) {
    const int score = eval(p, depth, best_score, inf, false);
    if (score > best_score) {
      best_score = score;
      best_position = p;
    }
  }

  // Return the best move found.
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
