// Rejected: a const lvalue of Base is provided for a `const Derived&`
// dependency.
// Why: rule "never downcast" — adding const does not make it legal.
// (Mutable sibling: base_for_derived_ref.cpp.)
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
struct derived : base {};  // default constructible: no accidental compile error
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1>[([](const derived& d) { return d.n == 0; })] = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  const derived obj{};
#else
  const base obj{};
#endif
  sml::sm<c> sm{obj};
  sm.process_event(e1{});
};
