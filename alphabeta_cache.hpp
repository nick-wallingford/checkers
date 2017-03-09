#pragma once

#include <memory>
#include <vector>

#include "agent.hpp"

enum node_type { pv, cut, all };

class transposition_table {
  class index {
    size_t mask;
    size_t i;
    char j;

  public:
    index(uint64_t h, size_t mask) : mask{mask}, i{h & mask}, j{0} {}
    size_t operator*() const { return i; }
    size_t operator++() {
      i += ++j;
      i &= mask;
      return i;
    }
    bool has_next() const { return j != 10; };
  };

  struct node {
    uint64_t hash;
    int score;
    char depth;
    char type;
    char age;
  };

  size_t hit_count;
  size_t lookup_count;
  const size_t mask;
  std::unique_ptr<node[]> table;
  char age;

public:
  transposition_table(size_t size)
      : hit_count{0}, lookup_count{0}, mask{((size_t)1 << size) - 1},
        table{new node[mask + 1]()}, age{0} {}
  ~transposition_table();

  bool lookup(const position &p, int depth, int &alpha, int &beta, int &score);
  void insert(const position &p, int score, char depth, node_type);
  void operator++() { ++age; }
  void sort(std::vector<position>, bool);
};

class alphabeta_cache : public agent {
  transposition_table tt;
  std::default_random_engine r;
  int eval(const position &, unsigned char, int alpha, int beta, bool maximize,
           node_type);

public:
  alphabeta_cache(const heuristic &e, int d, char side)
      : agent(e, d, side), tt(16), r{std::random_device{}()} {}
  position get_move(const position &);
};
