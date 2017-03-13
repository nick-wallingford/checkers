#pragma once

#include <memory>
#include <random>

#include "agent.hpp"

class position;

class alphabeta_pv : public agent {
  const size_t mask_max;
  const size_t mask_min;
  std::unique_ptr<uint64_t[]> pv_max;
  std::unique_ptr<uint64_t[]> pv_min;
  std::ranlux24 r{std::random_device{}()};

  int eval(const position &, unsigned char depth, int alpha, int beta,
           bool maximize);

public:
  alphabeta_pv(const heuristic &e, int d, char side);
  position get_move(const position &);
};
