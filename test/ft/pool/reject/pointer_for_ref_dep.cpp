// Rejected: a `T*` is provided for a `T&` dependency.
// Why: the pool never dereferences an argument implicitly; the mismatched
// argument must not be silently ignored either.
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
  counter cnt{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{cnt};
#else
  sml::sm<c> sm{&cnt};
#endif
  sm.process_event(e1{});
};
