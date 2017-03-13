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

public:
  alphabeta(const heuristic &e, int depth, char side) : agent(e, depth, side) {}
  position get_move(const position &);
};
