#pragma once

#include <memory>
#include <random>

#include "agent.hpp"

class position;

/**
 * An agent which performs alpha beta search, guided by principle variation.
 */
class alphabeta_pv : public agent {
  const size_t mask_max;
  const size_t mask_min;
  std::unique_ptr<uint64_t[]> pv_max;
  std::unique_ptr<uint64_t[]> pv_min;
  std::ranlux24 r{std::random_device{}()};

#ifdef MEASURE_BRANCHING_FACTOR
  uint64_t branch_sum;
  uint64_t branch_square_sum;
  uint64_t move_sum;
  uint64_t move_square_sum;
  uint64_t branch_count;
#endif

  int eval(const position &, unsigned char depth, int alpha, int beta,
           bool maximize);
  void sort(std::vector<position> &, uint64_t);

public:
  /**
   * Constructs a new alphabeta_pv agent.
   *
   * @param e The herustic to use to prioritize a given postiion.
   * @param d The depth to search to. Larger values result in better moves, but
   * take exponentially more time.
   * @param side WHITE or BLACK: chooses which side the agent is.
   */
  alphabeta_pv(const heuristic &e, int d, char side);
  /**
   * Searches for a good move and returns the best move found.
   *
   * @param The current position.
   * @return The position the board will be in after the agent has executed the
   * move it chooses.
   * @throws resign If the agent has lost or chooses to resign.
   */
  position get_move(const position &);

#ifdef MEASURE_BRANCHING_FACTOR
  ~alphabeta_pv();
#endif
};

#ifdef MEASURE_BRANCHING_FACTOR
void alphabeta_pv_report_branching();
#endif
