// Rejected: two different Derived objects are provided for one `Base&`
// dependency.
// Why: rule "ambiguous transformations are invalid" — either one could satisfy
// `Base&`. (An additional exact `Base` argument would not help either — see
// ambiguous_base_and_derived_for_base_ref.cpp.)
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
struct derived_a : base {};
struct derived_b : base {};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](base& b) { ++b.n; } = X);
  }
};
}  // namespace

test reject = [] {
  derived_a a{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{a};
#else
  derived_b b{};
  sml::sm<c> sm{a, b};
#endif
  sm.process_event(e1{});
};
