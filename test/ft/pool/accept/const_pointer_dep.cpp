// Use case: a dependency taken by *pointer to const* (`const T*`).
//
// Allowed: providing a `const T*` or a `T*` (qualification conversion), each as
// rvalue or lvalue.
// Semantics: the pool stores the pointer value; the pointee is observed live.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 42;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const data* d) { return d != nullptr && d->value == 42; };
    return make_transition_table(*state<class idle> + event<e1>[guard] = X);
  }
};
}  // namespace

test const_pointer_dep_from_const_pointer_rvalue = [] {
  const data d{};
  sml::sm<c> sm{&d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_pointer_dep_from_const_pointer_lvalue = [] {
  const data d{};
  const data* ptr = &d;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_pointer_dep_from_mutable_pointer_rvalue = [] {
  data d{};
  sml::sm<c> sm{&d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_pointer_dep_from_mutable_pointer_lvalue = [] {
  data d{};
  data* ptr = &d;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_pointer_dep_observes_live_pointee = [] {
  data d{0};
  sml::sm<c> sm{&d};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  d.value = 42;
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
