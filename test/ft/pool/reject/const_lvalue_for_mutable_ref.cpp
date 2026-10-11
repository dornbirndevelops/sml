// Rejected: a const lvalue is provided for a `T&` dependency.
// Why: const-correctness — the state machine would be able to mutate an object
// the caller declared const.
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
#else
  const counter cnt{};
#endif
  sml::sm<c> sm{cnt};
  sm.process_event(e1{});
};
