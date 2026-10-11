// Rejected: a `T&` dependency needed only by a sub state machine is not
// provided to the outer state machine (without
// BOOST_SML_CREATE_DEFAULT_CONSTRUCTIBLE_DEPS).
// Why: sub state machine dependencies are part of the outer pool; the same
// rules apply as for the outer state machine's own dependencies.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct e2 {};
struct counter {
  int n = 0;
};
struct sub {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class sub_idle> + event<e2> / [](counter& cnt) { ++cnt.n; } = X);
  }
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> = state<sub>);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  counter cnt{};
  sml::sm<c> sm{cnt};
#else
  sml::sm<c> sm{};
#endif
  sm.process_event(e1{});
  sm.process_event(e2{});
};
