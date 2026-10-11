// Use case: `sml::make_action<Ts...>(f)` names the dependency (and event) types
// of a generic or constrained callable explicitly (#629).
//
// Allowed: providing the named dependencies as ctor arguments.
// Semantics: the named types are requested exactly as if `f` had a concrete
// signature `(Ts...)`.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {
  int value = 1;
};

struct counter {
  int n = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = make_action<const counter&>([](const auto& cnt) { return cnt.n == 0; });
    const auto action = make_action<const e1&, counter&, const int&>([](const auto& e, auto& cnt, const auto& i) { cnt.n += e.value + i; });
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};
}  // namespace

test make_action_named_deps = [] {
  counter cnt{};
  const int i = 41;
  sml::sm<c> sm{i, cnt};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(42 == cnt.n);
};
