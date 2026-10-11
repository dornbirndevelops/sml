// Use case: callables take parameters that are NOT dependencies:
//  - the event (`E`, `const E&`, `auto`),
//  - `sml::back::process<Es...>` (process queue handle),
//  - a reference to a sub state machine class (`Sub&`, the live sub state
//    machine instance — cf. #659).
//
// Allowed: constructing the state machine without providing any of them.
// Semantics: these parameters are injected by SML itself and never occupy a
// slot that the caller has to fill.
#include <boost/sml.hpp>
#include <queue>

namespace sml = boost::sml;

namespace {
struct e1 {
  int value = 42;
};
struct e2 {};
struct e3 {};

struct sub {
  auto operator()() noexcept {
    using namespace sml;
    return make_transition_table(*state<class sub_idle> + event<e3> / [this] { ++hits; } = X);
  }
  int seeded = 0;
  int hits = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto by_value = [](e1 e) { expect(42 == e.value); };
    const auto by_cref = [](const e1& e) { return e.value == 42; };
    const auto push = [](sml::back::process<e2> p) { p(e2{}); };
    const auto seed_sub = [](sub& s) { s.seeded = 7; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + event<e1> [by_cref] / (by_value, push) = state<class waiting>,
       state<class waiting> + event<e2> / seed_sub = state<sub>
    );
    // clang-format on
  }
};
}  // namespace

test no_ctor_arguments_needed = [] {
  sml::sm<c, sml::process_queue<std::queue>> sm{};
  sm.process_event(e1{});
  expect(sm.is(sml::state<sub>));
  sm.process_event(e3{});
  sub& s = sm;  // the live sub state machine instance
  expect(7 == s.seeded);
  expect(1 == s.hits);
};
