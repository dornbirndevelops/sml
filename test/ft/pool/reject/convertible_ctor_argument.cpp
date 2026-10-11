// Rejected: a ctor argument is provided whose type is merely *convertible* to a
// requested dependency (`const double&` for `const int&`).
// Why: arguments are routed by exact type; silently dropping the argument hides
// the mistake.
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
    return make_transition_table(*state<class idle> + event<e1>[([](const int& i) { return i == 42; })] = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  const int i = 42;
  sml::sm<c> sm{i};
#else
  const double d = 42.0;
  sml::sm<c> sm{d};
#endif
  sm.process_event(e1{});
};
