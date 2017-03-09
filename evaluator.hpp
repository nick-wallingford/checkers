#pragma once

#include <array>
#include <functional>
#include <string>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

class position;

#define REGISTER_ENUM(x) x,
enum eval_names {
#include "evaluator_names.hpp"
  eval_count
};
#undef REGISTER_ENUM

#define REGISTER_ENUM(x) #x,
static const std::array<std::string, eval_names::eval_count + 1>
    eval_names_text{{
#include "evaluator_names.hpp"
        "invalid"}};
#undef REGISTER_ENUM

namespace eval {
#define REGISTER_ENUM(x) int x(const position &, int);
#include "evaluator_names.hpp"
#undef REGISTER_ENUM
}

#define REGISTER_ENUM(x) &eval::x,
static const std::array<std::function<int(const position &, int)>,
                        eval_names::eval_count + 1>
    eval_funcs{{
#include "evaluator_names.hpp"
        NULL}};
#undef REGISTER_ENUM

#pragma GCC diagnostic pop
