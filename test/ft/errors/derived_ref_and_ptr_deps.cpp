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

// A derived dep by reference and one by pointer: both could be injected into the base*
// (the object's address, or the converted pointer), so the lookup is ambiguous; neither
// form wins (before #715 the reference was read, which did not bind the base*).
struct derived_ref_and_ptr_deps {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        *idle + event<e1> / [](base *) {} = X // derived& or derived*?
    );
    // clang-format on
  }
};

// Only has to fail to compile: common/test.hpp (force-included) already defines main().
void construct_sm() {
  derived a;
  derived b;
  sml::sm<derived_ref_and_ptr_deps> sm{a, &b};
  sm.process_event(e1{});
}
