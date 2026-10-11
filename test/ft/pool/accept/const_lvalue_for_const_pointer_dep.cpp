// Use case: a const lvalue of T is provided for a `const T*` dependency.
//
// Allowed.
// Semantics: non-owning — the pool stores the ADDRESS of the caller's object
// (never of a copy), so the pointee is observed live.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 0;
};

const data* seen = nullptr;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto is_10 = [](const data* d) {
      seen = d;
      return d != nullptr && d->value == 10;
    };
    return make_transition_table(*state<class idle> + event<e1>[is_10] = X);
  }
};
}  // namespace

test const_pointer_to_const_object = [] {
  const data d{10};
  sml::sm<c> sm{d};
  sm.process_event(e1{});
  expect(&d == seen);  // the caller's object, not a copy
  expect(sm.is(sml::X));
};

test const_pointer_observes_live_value = [] {
  data d{};
  const data& view = d;  // the state machine only gets a const view
  sml::sm<c> sm{view};
  sm.process_event(e1{});
  expect(&d == seen);
  expect(!sm.is(sml::X));
  d.value = 10;  // mutated through the caller's non-const name
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
