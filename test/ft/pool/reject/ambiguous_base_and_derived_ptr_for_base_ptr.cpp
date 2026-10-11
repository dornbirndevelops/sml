// Rejected: `Base*` and `Derived*` arguments are provided for a `Base*` dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates — an exact match does not win over a derived-to-base one.
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
  base b{};
  derived d{};
  base* bptr = &b;
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{bptr};
#else
  sml::sm<c> sm{bptr, &d};
#endif
  sm.process_event(e1{});
};
