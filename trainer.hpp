#pragma once

#include <mutex>
#include <vector>

#include "heuristic.hpp"

class trainer {
  std::vector<heuristic> heuristics;
  std::vector<std::pair<heuristic &, heuristic &>> matches;
  std::mutex m;
  int size;
  char depth;

  std::pair<heuristic &, heuristic &> pop();
  void exec_match(std::pair<heuristic &, heuristic &> &);
  void work();

  class done {};

public:
  trainer(std::initializer_list<heuristic> seed, int size, char depth);
  void operator()();

  friend std::ostream &operator<<(std::ostream &o, const trainer &);
};
