// Rejected: a const lvalue of T is provided while callables request BOTH
// `const T&` and `T&`.
// Why: rule "never cast away const" — the `T&` request would be served from a
// const object. (`T&` and `const T&` share one slot; the slot is mutable as
// soon as any callable needs `T&`.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 0;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto check = [](const data& d) { return d.value == 0; };
    const auto write = [](data& d) { d.value = 42; };
    return make_transition_table(*state<class idle> + event<e1>[check] / write = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  data d{};
#else
  const data d{};
#endif
  sml::sm<c> sm{d};
  sm.process_event(e1{});
};
