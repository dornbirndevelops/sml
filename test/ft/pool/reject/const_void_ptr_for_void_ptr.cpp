// Rejected: a `const void*` is provided for a `void*` dependency.
// Why: rule "never cast away const" — type erasure does not change that.
// (Allowed: accept/*void_pointer_dep.cpp — the callable requests `void*` /
// `const void*` and casts it itself.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 0;  // default constructible: no accidental compile error
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](void* p) { ++static_cast<data*>(p)->value; } = X);
  }
};
}  // namespace

test reject = [] {
  data d{};
#if defined(SML_POOL_CONTROL)
  void* ptr = &d;
  sml::sm<c> sm{ptr};
#else
  const void* ptr = &d;
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
