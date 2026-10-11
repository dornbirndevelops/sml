// Use case: dependencies are requested by every kind of callable SML supports:
//  - guards and actions (lambdas, function objects),
//  - on_entry / on_exit actions,
//  - generic lambdas that mix an `auto` event with typed dependencies,
//  - callables composed with `,` (sequence) and `&&` / `!` (guard operators).
//
// Allowed: providing each dependency once.
// Semantics: the union of all requested dependency types forms the pool.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {
  int value = 1;
};

struct trace {
  int entries = 0;
  int exits = 0;
  int actions = 0;
  int generic = 0;
  int functor = 0;
  int composed = 0;
};

struct functor_action {
  void operator()(trace& t) const { ++t.functor; }
};

struct functor_guard {
  bool operator()(const trace& t, const int& i) const { return t.entries == 1 && i == 42; }
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto on_enter = [](trace& t) { ++t.entries; };
    const auto on_leave = [](trace& t) { ++t.exits; };
    const auto action = [](trace& t) { ++t.actions; };
    const auto generic = [](const auto& e, trace& t, const int& i) {
      t.generic += e.value + i;
    };
    const auto is_negative = [](const double& d) { return d < 0; };
    const auto is_one = [](const trace& t) { return t.actions == 1; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + on_entry<_> / on_enter,
       state<class idle> + sml::on_exit<_> / on_leave,
       state<class idle> + event<e1> [functor_guard{}] / (action, generic, functor_action{}) = state<class done>,
       state<class done> + event<e1> [is_one && !is_negative] / [](trace& t) { ++t.composed; } = X
    );
    // clang-format on
  }
};
}  // namespace

test deps_in_all_callable_kinds = [] {
  trace t{};
  const int i = 42;
  const double d = 1.0;
  sml::sm<c> sm{t, i, d};
  expect(1 == t.entries);
  sm.process_event(e1{});
  expect(1 == t.exits);
  expect(1 == t.actions);
  expect(1 + 42 == t.generic);
  expect(1 == t.functor);
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == t.composed);
};
