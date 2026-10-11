// Use case: one `Base&` dependency is satisfied by a `Derived` argument, while
// the caller ALSO provides unrelated `T&` dependencies.
//
// Allowed: in any order.
// Semantics: the derived-to-base (covariant) match must not depend on how many
// other reference arguments are provided — it is decided per requested type.
// (base_ref_from_derived.cpp covers the single-argument case.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct base {
  int n = 0;
};
struct derived : base {};
struct other {
  int n = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](base& b, other& o, const int& i) {
      b.n += i;
      o.n += i;
    };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
}  // namespace

test derived_first = [] {
  derived d{};
  other o{};
  const int i = 1;
  sml::sm<c> sm{d, o, i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == d.n);
  expect(1 == o.n);
};

test derived_last = [] {
  derived d{};
  other o{};
  const int i = 1;
  sml::sm<c> sm{i, o, d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == d.n);
  expect(1 == o.n);
};
