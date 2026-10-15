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

struct dep {
  int val = 1;
};

// The requested type itself is passed twice, as dep& and as const dep&. The author
// expects the guard and the action to see one instance, but the lookup would silently
// read one of the two (the const dep&, which the dep& slot cannot even bind), so it is
// an `ambiguous_ctor_parameter` error.
struct mutable_and_const_ref_of_same_dep {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        *idle + event<e1> [([](const dep &) { return true; })] / [](dep &) {} = X // a or b?
    );
    // clang-format on
  }
};

// Only has to fail to compile: common/test.hpp (force-included) already defines main().
void construct_sm() {
  dep a;
  const dep b{};
  sml::sm<mutable_and_const_ref_of_same_dep> sm{a, b};
  sm.process_event(e1{});
}
