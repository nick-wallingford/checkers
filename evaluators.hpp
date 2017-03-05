#pragma once

#include <array>
#include <functional>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"

#define REGISTER_ENUM(x) x,
enum eval_names {
#include "evaluator_names.hpp"
  eval_count
};
#undef REGISTER_ENUM

#define REGISTER_ENUM(x) #x,
static const char *eval_names_text[] = {
#include "evaluator_names.hpp"
    "invalid"};
#undef REGISTER_ENUM

namespace eval {
#define REGISTER_ENUM(x) int x(const std::array<unsigned, 4> &, int);
#include "evaluator_names.hpp"
#undef REGISTER_ENUM
}

#define REGISTER_ENUM(x) &eval::x,
static const std::function<int(const std::array<unsigned, 4> &, int)>
    eval_funcs[]{
#include "evaluator_names.hpp"
        NULL};
#undef REGISTER_ENUM

#pragma GCC diagnostic pop
