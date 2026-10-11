// Use case: a dependency taken by *mutable reference* (`T&`).
//
// Allowed: providing a non-const lvalue of T.
// Semantics: non-owning. The pool aliases the caller's object, so
//  - mutations done by actions are visible to the caller,
//  - mutations done by the caller are visible to guards/actions,
//  - every action/guard naming `T&` sees the very same object (one pool slot).
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct e2 {};

struct counter {
  int n = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto inc = [](counter& cnt) { ++cnt.n; };
    const auto is_ten = [](counter& cnt) { return cnt.n == 10; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + event<e1> / inc = state<class busy>,
       state<class busy> + event<e1> / inc = state<class idle>,
       state<class busy> + event<e2> [is_ten] = X
    );
    // clang-format on
  }
};
}  // namespace

test action_mutation_is_visible_to_caller = [] {
  counter cnt{};
  sml::sm<c> sm{cnt};
  sm.process_event(e1{});
  expect(1 == cnt.n);
  sm.process_event(e1{});
  expect(2 == cnt.n);  // both transitions share one slot
};

test caller_mutation_is_visible_to_guard = [] {
  counter cnt{};
  sml::sm<c> sm{cnt};
  sm.process_event(e1{});  // -> busy, n == 1
  sm.process_event(e2{});
  expect(!sm.is(sml::X));
  cnt.n = 10;
  sm.process_event(e2{});
  expect(sm.is(sml::X));
};
