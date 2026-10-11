// Use case: a non-const lvalue of T is provided for a `T*` dependency.
//
// Allowed.
// Semantics: non-owning — the pool stores the ADDRESS of the caller's object
// (never of a copy), so
//  - mutations through the pointer are visible to the caller,
//  - mutations by the caller are visible through the pointer,
//  - a `T&` request for the same T refers to that same object.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct e2 {};

struct data {
  int value = 0;
};

struct only_pointer {
  auto operator()() const noexcept {
    using namespace sml;
    const auto inc = [](data* d) {
      expect(d != nullptr);
      ++d->value;
    };
    const auto is_10 = [](data* d) { return d != nullptr && d->value == 10; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + event<e1> / inc = state<class busy>,
       state<class busy> + event<e2> [is_10] = X
    );
    // clang-format on
  }
};

struct pointer_and_ref {
  auto operator()() const noexcept {
    using namespace sml;
    const auto same_object = [](data* p, data& r) { return p == &r; };
    return make_transition_table(*state<class idle> + event<e1>[same_object] = X);
  }
};
}  // namespace

test pointer_aliases_caller_object = [] {
  data d{};
  sml::sm<only_pointer> sm{d};
  sm.process_event(e1{});
  expect(1 == d.value);  // written through the pointer
  sm.process_event(e2{});
  expect(!sm.is(sml::X));
  d.value = 10;  // observed through the pointer
  sm.process_event(e2{});
  expect(sm.is(sml::X));
};

test pointer_and_ref_refer_to_same_object = [] {
  data d{};
  sml::sm<pointer_and_ref> sm{d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
