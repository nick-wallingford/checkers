#pragma once

#include <iostream>
#include <utility>
#include <vector>

#include "evaluator.hpp"

class position;

class heuristic {
public:
  heuristic() : kingweight{20} {}
  void mutate();
  void add_evaluator(eval_names n, int weight) {
    evaluators.emplace_back(n, weight);
  }
  int operator()(const position &) const;

  friend std::ostream &operator<<(std::ostream &o, const heuristic &);

private:
  std::vector<std::pair<eval_names, int>> evaluators;
  int kingweight;
};
