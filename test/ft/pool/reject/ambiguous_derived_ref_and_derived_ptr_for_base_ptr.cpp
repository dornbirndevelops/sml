// Rejected: a `Derived` lvalue and a `Derived*` are provided for a `Base*`
// dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments could be
// injected (the address of the object, or the converted pointer); neither the
// pointer nor the object wins.
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
    return make_transition_table(*state<class idle> + event<e1> / [](base* b) { ++b->value; } = X);
  }
};
}  // namespace

test reject = [] {
  derived a{};
  derived b{};
#if defined(SML_POOL_CONTROL)
  (void)a;
  sml::sm<c> sm{&b};
#else
  sml::sm<c> sm{a, &b};
#endif
  sm.process_event(e1{});
};
