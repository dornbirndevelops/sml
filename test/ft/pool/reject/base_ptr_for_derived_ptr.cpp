// Rejected: a `Base*` is provided for a `Derived*` dependency — even when it
// happens to point at a Derived object.
// Why: rule "never downcast". The argument must not be silently ignored either
// (which would leave the `Derived*` slot null).
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
    return make_transition_table(*state<class idle> + event<e1> / [](derived* d) { ++d->n; } = X);
  }
};
}  // namespace

test reject = [] {
  derived d{};
#if defined(SML_POOL_CONTROL)
  derived* ptr = &d;
#else
  base* ptr = &d;
#endif
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
};
