// Rejected: a const lvalue of Derived is provided for a `Base*` dependency.
// Why: const-correctness — its address is `const Derived*`, which converts to
// `const Base*` but never to `Base*` (no implicit const_cast). Must also hold
// once object-for-pointer and derived-to-base are combined.
// (Allowed sibling: accept/const_derived_for_const_base_pointer.cpp.)
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
  // An exact `Base*` rather than `Derived&`: object-for-pointer is not
  // implemented yet, and the control must only depend on working behaviour.
  derived d{};
  base* ptr = &d;
  sml::sm<c> sm{ptr};
#else
  const derived d{};
  sml::sm<c> sm{d};
#endif
  sm.process_event(e1{});
};
