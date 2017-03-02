#pragma once

#include <iostream>
#include <vector>

class evaluator {
public:
  enum side { black, white, both };
  class formation {
  public:
    formation(unsigned pattern, int weight, side s, bool kings, bool pieces);
    unsigned pattern;
    int weight;
    side s;
    bool kings;
    bool pieces;
  };

  evaluator() : kingweight{20} {}
  void mutate() const;
  void add_formation(formation);

  friend std::ostream &operator<<(std::ostream &o, const evaluator &);

private:
  std::vector<formation> formations;
  int kingweight;
};
