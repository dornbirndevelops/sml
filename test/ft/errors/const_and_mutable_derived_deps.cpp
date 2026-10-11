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

struct iface {
  virtual ~iface() = default;
  virtual int id() const = 0;
};
struct impl_a : iface {
  int id() const override { return 1; }
};
struct impl_b : iface {
  int id() const override { return 2; }
};

// A mutable and a const dep derive from the requested type: the lookup is ambiguous for
// the guard's const iface& and for the action's iface& alike, so that both always see the
// same object (before #715 the const iface& read the const one, and the iface& did not
// compile; constness must not pick one of them).
struct const_and_mutable_derived_deps {
  auto operator()() const noexcept {
    using namespace sml;
    // clang-format off
    return make_transition_table(
        *idle + event<e1> [([](const iface &f) { return f.id() > 0; })] / [](iface &) {} = X // impl_a& or const impl_b&?
    );
    // clang-format on
  }
};

// Only has to fail to compile: common/test.hpp (force-included) already defines main().
void construct_sm() {
  impl_a a;
  const impl_b b{};
  sml::sm<const_and_mutable_derived_deps> sm{a, b};
  sm.process_event(e1{});
}
