#pragma once

#include <random>

#include "hashmap.hpp"
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

class negamax : public agent {
  std::default_random_engine r;
  int eval(const position &, int) const;

public:
  negamax(const heuristic &e, int depth, char side)
      : agent(e, depth, side), r{std::random_device()()} {}
  position get_move(const position &);
};

class alphabeta : public agent {
  hashmap cache;
  std::default_random_engine r;
  int eval(const position &, int, int alpha, int beta);

public:
  alphabeta(const heuristic &e, int depth, char side)
      : agent(e, depth, side), cache{20}, r{std::random_device{}()} {
    r.discard(100);
  }
  position get_move(const position &);
};
