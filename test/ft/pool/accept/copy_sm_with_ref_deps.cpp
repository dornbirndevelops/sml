// Use case: a state machine whose reference/pointer dependencies were provided
// by the caller is copied or moved.
//
// Allowed.
// Semantics: non-owning slots stay non-owning — the copy / moved-to state
// machine aliases the same caller objects as the original, also after the
// original was destroyed.
#include <boost/sml.hpp>
#include <memory>
#include <utility>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct counter {
  int n = 0;
};
struct marker {};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](counter& cnt, const marker* p) { cnt.n += 1 + (p ? 0 : 1000); };
    return make_transition_table(*state<class idle> + event<e1> / action = state<class idle>);
  }
};

using sm_t = sml::sm<c>;
}  // namespace

test copy_aliases_same_caller_objects = [] {
  counter cnt{};
  const marker m{};
  sm_t original{cnt, &m};
  sm_t copy{original};
  original.process_event(e1{});
  copy.process_event(e1{});
  expect(2 == cnt.n);
};

test copy_outlives_original = [] {
  counter cnt{};
  const marker m{};
  auto original = std::make_unique<sm_t>(cnt, &m);
  sm_t copy{*original};
  original.reset();
  copy.process_event(e1{});
  expect(1 == cnt.n);
};

test move_keeps_aliasing_caller_objects = [] {
  counter cnt{};
  const marker m{};
  auto original = std::make_unique<sm_t>(cnt, &m);
  sm_t moved{std::move(*original)};
  original.reset();
  moved.process_event(e1{});
  expect(1 == cnt.n);
};
