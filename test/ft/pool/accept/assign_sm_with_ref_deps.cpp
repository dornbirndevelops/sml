// Use case: a state machine whose reference/pointer dependencies were provided
// by the caller is copy-assigned (sm declares `operator=(const sm&) = default`).
//
// Allowed.
// Semantics: rebinding, not writing through — after `second = first`, `second`
// aliases first's caller objects; the objects `second` aliased before are
// neither modified nor used any more.
#include <boost/sml.hpp>

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

test copy_assignment_aliases_same_caller_objects = [] {
  counter a{};
  counter b{};
  const marker m{};
  sm_t first{a, &m};
  sm_t second{b, &m};
  second = first;
  second.process_event(e1{});
  expect(1 == a.n);
  expect(0 == b.n);
};
