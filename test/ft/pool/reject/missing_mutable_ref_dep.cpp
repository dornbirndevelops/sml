// Rejected: a `T&` dependency is not provided (without
// BOOST_SML_CREATE_DEFAULT_CONSTRUCTIBLE_DEPS), even though T is default
// constructible.
// Why: the state machine has no object to alias and must not invent one.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct counter {
  int n = 0;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](counter& cnt) { ++cnt.n; } = X);
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
};
