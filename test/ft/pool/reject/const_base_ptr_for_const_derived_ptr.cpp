// Rejected: a `const Base*` is provided for a `const Derived*` dependency —
// even when it happens to point at a Derived object.
// Why: rule "never downcast" — adding const does not make it legal. The
// argument must not be silently ignored either (which would leave the slot
// null). (Mutable sibling: base_ptr_for_derived_ptr.cpp.)
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
    return make_transition_table(*state<class idle> + event<e1>[([](const derived* d) { return d->n == 0; })] = X);
  }
};
}  // namespace

test reject = [] {
  const derived d{};
#if defined(SML_POOL_CONTROL)
  const derived* ptr = &d;
#else
  const base* ptr = &d;
#endif
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
};
