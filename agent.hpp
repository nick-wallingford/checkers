#pragma once

#include <random>

#include "evaluator.hpp"

class position;

class agent {
protected:
  evaluator e;
  int depth;
  char side;

public:
  class resign {};
  agent(const evaluator &e, int depth, char side)
      : e{e}, depth{depth}, side{side} {}
  virtual ~agent() {}
  virtual position get_move(const position &) = 0;
};

class minimax : public agent {
  std::default_random_engine r;
  int minimize(const position &, int) const;
  int maximize(const position &, int) const;

public:
  minimax(const evaluator &e, int depth, char side)
      : agent(e, depth, side), r{std::random_device()()} {}
  position get_move(const position &);
};

class negamax : public agent {
  std::default_random_engine r;
  int eval(const position &, int) const;

public:
  negamax(const evaluator &e, int depth, char side)
      : agent(e, depth, side), r{std::random_device()()} {}
  position get_move(const position &);
};
