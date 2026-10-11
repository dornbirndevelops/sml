// Use case: composite state machines. Dependencies are needed by the parent and
// by (nested) sub state machines.
//
// Allowed: providing every dependency once, to the outermost state machine only
// — including dependencies that only a sub state machine needs.
// Semantics: parent and all sub state machines share the very same slot, so a
// `T&` dependency aliases the caller's object at every level (cf. #504: the
// reference must not dangle after construction).
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct enter {};
struct work {};

struct counter {
  int n = 0;
};
struct cfg {
  int step = 1;
};

struct inner {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](counter& cnt, const cfg& k) { cnt.n += k.step * 100; };
    return make_transition_table(*state<class inner_idle> + event<work> / action = X);
  }
};

struct middle {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](counter& cnt) { cnt.n += 10; };
    return make_transition_table(*state<class middle_idle> + event<enter> / action = state<inner>);
  }
};

struct outer {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](counter& cnt) { cnt.n += 1; };
    return make_transition_table(*state<class outer_idle> + event<enter> / action = state<middle>);
  }
};

// Only the sub state machine needs a dependency; the parent names none.
struct parent_without_deps {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class p_idle> + event<enter> = state<inner>);
  }
};
}  // namespace

test deps_shared_by_all_levels = [] {
  counter cnt{};
  const cfg k{2};
  sml::sm<outer> sm{cnt, k};
  sm.process_event(enter{});  // outer: +1, -> middle
  expect(1 == cnt.n);
  sm.process_event(enter{});  // middle: +10, -> inner
  expect(11 == cnt.n);
  sm.process_event(work{});  // inner: +200
  expect(211 == cnt.n);
};

test deps_only_used_by_sub_sm = [] {
  counter cnt{};
  const cfg k{1};
  sml::sm<parent_without_deps> sm{k, cnt};
  sm.process_event(enter{});
  sm.process_event(work{});
  expect(100 == cnt.n);
};

test sub_sm_observes_caller_mutation = [] {
  counter cnt{};
  cfg k{1};
  sml::sm<parent_without_deps> sm{cnt, k};
  sm.process_event(enter{});
  k.step = 3;  // const cfg& must be live in the sub state machine, too
  sm.process_event(work{});
  expect(300 == cnt.n);
};
