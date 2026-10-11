// Rejected: a Base object is provided for a `Derived&` dependency.
// Why: no implicit downcast.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct base {
  int n = 0;
};
struct derived : base {};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](derived& d) { ++d.n; } = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  derived obj{};
#else
  base obj{};
#endif
  sml::sm<c> sm{obj};
  sm.process_event(e1{});
};
