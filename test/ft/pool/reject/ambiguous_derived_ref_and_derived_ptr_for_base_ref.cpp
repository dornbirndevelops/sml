// Rejected: a `Derived` lvalue and a `Derived*` are provided for a `Base&`
// dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments compete for
// the slot (both are of a type derived from Base), even though only the object
// could legally fill it (the pool never dereferences a pointer), as for `T&` and
// `T*` (ambiguous_ref_and_pointer_for_ref.cpp).
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct base {
  int value = 0;
};
struct derived : base {};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](base& b) { ++b.value; } = X);
  }
};
}  // namespace

test reject = [] {
  derived a{};
  derived b{};
#if defined(SML_POOL_CONTROL)
  (void)b;
  sml::sm<c> sm{a};
#else
  sml::sm<c> sm{a, &b};
#endif
  sm.process_event(e1{});
};
