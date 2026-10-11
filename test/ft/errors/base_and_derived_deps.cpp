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
struct derived : base {};

// A dep of the requested type itself and one derived from it: the lookup is ambiguous;
// an exact match does not win (before #715 the exact base& was read next to a derived&,
// but a const derived& won over it).
struct base_and_derived_deps {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        *idle + event<e1> / [](base &) {} = X // base& or derived&?
    );
    // clang-format on
  }
};

// Only has to fail to compile: common/test.hpp (force-included) already defines main().
void construct_sm() {
  base b;
  derived d;
  sml::sm<base_and_derived_deps> sm{b, d};
  sm.process_event(e1{});
}
