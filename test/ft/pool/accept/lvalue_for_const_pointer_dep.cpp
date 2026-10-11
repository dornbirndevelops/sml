// Use case: a non-const lvalue of T is provided for a `const T*` dependency.
//
// Allowed.
// Semantics: non-owning — the pool stores the ADDRESS of the caller's object
// (never of a copy), so mutations by the caller are visible through the
// pointer.
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

test const_pointer_to_caller_object = [] {
  data d{};
  sml::sm<c> sm{d};
  sm.process_event(e1{});
  expect(&d == seen);  // the caller's object, not a copy
  expect(!sm.is(sml::X));
  d.value = 10;  // observed through the pointer
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
