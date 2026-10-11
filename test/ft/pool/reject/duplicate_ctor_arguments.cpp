// Rejected: two ctor arguments of the same type are provided.
// Why: arguments are routed by type, so the second one would be ambiguous.
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
    return make_transition_table(*state<class idle> + event<e1>[([](const int& i, const double& d) { return i == 1 && d > 0; })] = X);
  }
};
}  // namespace

test reject = [] {
  const int a = 1;
  const int b = 2;
  const double d = 2.0;
#if defined(SML_POOL_CONTROL)
  (void)b;
  sml::sm<c> sm{a, d};
#else
  (void)d;
  sml::sm<c> sm{a, b};
#endif
  sm.process_event(e1{});
};
