// Rejected: a `Derived*` and a `const Derived*` are provided for a `const Base*`
// dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments could be
// injected (both convert to `const Base*`); constness does not pick one.
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
    return make_transition_table(*state<class idle> + event<e1> [([](const base* b) { return b->value == 0; })] = X);
  }
};
}  // namespace

test reject = [] {
  derived a{};
  const derived b{};
  const derived* cptr = &b;
#if defined(SML_POOL_CONTROL)
  (void)a;
  sml::sm<c> sm{cptr};
#else
  sml::sm<c> sm{&a, cptr};
#endif
  sm.process_event(e1{});
};
