#pragma once

#include <iostream>
#include <utility>
#include <vector>

#include "evaluators.hpp"

class position;

class evaluator {
public:
  enum side { black, white, both };
  struct formation {
    formation(unsigned pattern, int weight, char power, char fields)
        : pattern{pattern}, weight{weight}, power{power}, fields{fields} {}
    unsigned pattern;
    int weight;
    char power;
    char fields;
    int operator()(const position &) const;
    void operator++() { weight++; }
    void operator--() { weight++; }
  };

  evaluator() : kingweight{20} {}
  void mutate();
  void add_formation(formation f) { formations.emplace_back(f); };
  int operator()(const position &) const;

  friend std::ostream &operator<<(std::ostream &o, const evaluator &);

private:
  std::vector<formation> formations;
  std::vector<std::pair<eval_names,int>> spec_evaluators;
  int kingweight;
};
