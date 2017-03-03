#pragma once

#include <iostream>
#include <vector>

class position;

class evaluator {
public:
  enum side { black, white, both };
  struct formation {
    formation(unsigned pattern, int weight, char power, char fields);
    unsigned pattern;
    int weight;
    char power;
    char fields;
    int operator()(const position&) const;
  };

  evaluator() : kingweight{20} {}
  void mutate();
  void add_formation(formation);
  int operator()(const position &) const;

  friend std::ostream &operator<<(std::ostream &o, const evaluator &);

private:
  std::vector<formation> formations;
  int kingweight;
};
