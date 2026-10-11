// Use case: policy `sml::dont_instantiate_statemachine_class` — the caller owns
// the transition-table instance (and sub state machine instances).
//
// Allowed: providing an lvalue of the table class (and of each sub state
// machine class), together with ordinary dependencies, in any order.
// Semantics: non-owning. The state machine uses the caller's instance, so
// changes made through `this` in actions are visible to the caller and vice
// versa. The table class may be non-copyable.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct e2 {};

struct sink {
  int total = 0;
};

struct table {
  table() = default;
  table(const table&) = delete;
  table& operator=(const table&) = delete;

  auto operator()() {
    using namespace sml;
    const auto action = [this](sink& s) { s.total += ++calls; };
    return make_transition_table(*state<class idle> + event<e1> / action = state<class idle>);
  }

  int calls = 0;
};

struct sub {
  sub() = default;
  sub(const sub&) = delete;
  sub& operator=(const sub&) = delete;

  auto operator()() {
    using namespace sml;
    const auto action = [this] { ++calls; };
    return make_transition_table(*state<class sub_idle> + event<e2> / action = X);
  }

  int calls = 0;
};

struct parent {
  parent() = default;
  parent(const parent&) = delete;
  parent& operator=(const parent&) = delete;

  auto operator()() {
    using namespace sml;
    return make_transition_table(*state<class p_idle> + event<e1> = state<sub>);
  }
};
}  // namespace

test caller_owned_table_instance = [] {
  table t{};
  sink s{};
  sml::sm<table, sml::dont_instantiate_statemachine_class> sm{t, s};
  sm.process_event(e1{});
  sm.process_event(e1{});
  expect(2 == t.calls);
  expect(1 + 2 == s.total);
};

test caller_owned_table_and_sub_instances = [] {
  parent p{};
  sub su{};
  sml::sm<parent, sml::dont_instantiate_statemachine_class> sm{su, p};
  sm.process_event(e1{});
  sm.process_event(e2{});
  expect(1 == su.calls);
};
