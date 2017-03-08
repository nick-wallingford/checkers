#pragma once

#include <memory>

#include "agent.hpp"

class alphabeta_cache : public agent {
  const size_t mask;

  std::unique_ptr<unsigned char[]> depth;
  std::unique_ptr<size_t[]> hash;
  std::unique_ptr<int[]> minimum;
  std::unique_ptr<int[]> exact;
  std::unique_ptr<int[]> maximum;

  std::default_random_engine r;
  int eval(const position &, unsigned char, int alpha, int beta, bool maximize);

public:
  alphabeta_cache(const heuristic &e, int depth, char side)
      : agent(e, depth, side), mask{((size_t)1 << 16) - 1},
        depth{new unsigned char[mask + 1]()}, hash{new size_t[mask + 1]},
        minimum{new int[mask + 1]}, exact{new int[mask + 1]},
        maximum{new int[mask + 1]}, r{std::random_device{}()} {}
  position get_move(const position &);
};
