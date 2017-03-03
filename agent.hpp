#pragma once

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
  int recursive_eval(const position &, int) const;
  position get_move(const position &);
};
