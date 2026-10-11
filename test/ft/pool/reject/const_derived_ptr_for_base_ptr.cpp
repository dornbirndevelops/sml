// Rejected: a `const Derived*` is provided for a `Base*` dependency.
// Why: const-correctness — `const Derived*` converts to `const Base*` but never
// to `Base*` (no implicit const_cast). The argument must not be silently
// ignored either (which would leave the `Base*` slot null).
// (Allowed sibling: accept/derived_ptr_for_const_base_ptr.cpp.)
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
    return make_transition_table(*state<class idle> + event<e1> / [](base* b) { ++b->n; } = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  // An exact `Base*`: derived-to-base pointer conversion is not implemented
  // yet, and the control must only depend on working behaviour.
  derived d{};
  base* ptr = &d;
#else
  const derived d{};
  const derived* ptr = &d;
#endif
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
};
