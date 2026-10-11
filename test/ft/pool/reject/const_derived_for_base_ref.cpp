// Rejected: a const lvalue of Derived is provided for a `Base&` dependency.
// Why: const-correctness must also hold on the derived-to-base (covariant) path.
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
    return make_transition_table(*state<class idle> + event<e1> / [](base& b) { ++b.n; } = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  derived d{};
#else
  const derived d{};
#endif
  sml::sm<c> sm{d};
  sm.process_event(e1{});
};
