#pragma once

#include <random>

#include "heuristic.hpp"

class position;

class agent {
protected:
  heuristic e;
  int depth;
  char side;

public:
  class resign {};
  agent(const heuristic &e, int depth, char side)
      : e{e}, depth{depth}, side{side} {}
  char get_side() const { return side; }
  virtual ~agent() {}
  virtual position get_move(const position &) = 0;
};

class minimax : public agent {
  std::default_random_engine r;
  int minimize(const position &, int) const;
  int maximize(const position &, int) const;

public:
  minimax(const heuristic &e, int depth, char side)
      : agent(e, depth, side), r{std::random_device()()} {}
  position get_move(const position &);
};

class alphabeta : public agent {
  std::ranlux24 r{std::random_device{}()};
  int eval(const position &, int, int alpha, int beta);

#ifdef MEASURE_BRANCHING_FACTOR
  uint64_t branch_sum;
  uint64_t branch_square_sum;
  uint64_t move_sum;
  uint64_t move_square_sum;
  uint64_t branch_count;
#endif

public:
  alphabeta(const heuristic &e, int depth, char side)
      : agent(e, depth, side)
#ifdef MEASURE_BRANCHING_FACTOR
        ,
        branch_sum(0), branch_square_sum(0), move_sum(0), move_square_sum(0),
        branch_count(0)
#endif
  {
  }
  position get_move(const position &);
#ifdef MEASURE_BRANCHING_FACTOR
  ~alphabeta();
  static void report_branching();
#endif
};
