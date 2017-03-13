#pragma once

#include <memory>

#include "agent.hpp"

class position;

class alphabeta_pv : public agent {
  const size_t mask_max;
  const size_t mask_min;
  std::unique_ptr<uint64_t[]> pv_max;
  std::unique_ptr<uint64_t[]> pv_min;
  size_t count_max;
  size_t count_min;

  int eval(const position &, unsigned char depth, int alpha, int beta,
           bool maximize);

public:
  alphabeta_pv(const heuristic &e, int d, char side);
  ~alphabeta_pv();
  position get_move(const position &);
};
