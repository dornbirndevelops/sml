//
// Copyright (c) 2016-2020 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/sml.hpp>

namespace sml = boost::sml;

struct e1 {};
const auto idle = sml::state<class idle>;

struct base {
  int val = 1;
};
struct derived_a : base {};
struct derived_b : base {};

// A mutable and a const dep derive from the requested type. Reading the mutable one
// for base& and the const one for const base& would give the guard and the action
// different instances where the author expects one, so the covariant lookup is
// ambiguous for both slots (`several_covariant_deps`), as for several_derived_deps.
struct mutable_and_const_derived_deps {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        *idle + event<e1> [([](const base &) { return true; })] / [](base &) {} = X // a or b?
    );
    // clang-format on
  }
};

// Only has to fail to compile: common/test.hpp (force-included) already defines main().
void construct_sm() {
  derived_a a;
  const derived_b b{};
  sml::sm<mutable_and_const_derived_deps> sm{a, b};
  sm.process_event(e1{});
}
