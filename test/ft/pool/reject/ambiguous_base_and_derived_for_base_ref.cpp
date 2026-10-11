// Rejected: a `Base` and a `Derived` object are provided while callables
// request `Base&` (and `Derived&`).
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates for `Base&`; an exact match does not win over a derived-to-base
// one. (Replaces the former accept/exact_match_preferred_over_derived.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct base {
  int tag = 1;
};
struct derived : base {
  derived() { tag = 2; }
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](base& b, derived& d) { b.tag += d.tag; } = X);
  }
};
}  // namespace

test reject = [] {
  base b{};
  derived d{};
#if defined(SML_POOL_CONTROL)
  // A single Derived satisfies both requests unambiguously.
  (void)b;
  sml::sm<c> sm{d};
#else
  sml::sm<c> sm{b, d};
#endif
  sm.process_event(e1{});
};
