#pragma once

#include <iostream>
#include <utility>
#include <vector>

#include "evaluator.hpp"

class position;

class heuristic {
  std::vector<std::pair<eval_names, int>> evaluators;
  int kingweight;
  int score;

public:
  heuristic(int kingweight = 20) : kingweight{kingweight}, score{0} {}
  void mutate();
  void add_evaluator(eval_names n, int weight) {
    evaluators.emplace_back(n, weight);
  }

  int operator()(const position &) const;

  void operator++() { ++score; }
  void operator--() { --score; }
  bool operator<(const heuristic &o) const { return score > o.score; }
  void reset() { score = 0; }

  friend std::ostream &operator<<(std::ostream &o, const heuristic &);
};
