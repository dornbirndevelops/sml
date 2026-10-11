// Rejected: a ctor argument is provided to a state machine that requests no
// dependencies at all.
// Why: same as unrelated_ctor_argument.cpp — the empty pool must not swallow
// arbitrary arguments.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{};
#else
  const int i = 42;
  sml::sm<c> sm{i};
#endif
  sm.process_event(e1{});
};
