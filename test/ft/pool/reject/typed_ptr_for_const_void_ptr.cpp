// Rejected: a `T*` is provided for a `const void*` dependency.
// Why: rule "only the library user casts" — erasing the type is a cast, too;
// the caller passes a `const void*` explicitly. Otherwise ANY pointer argument
// could fill a `const void*` slot, making the match ambiguous / surprising.
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
    return make_transition_table(*state<class idle> + event<e1> [([](const void* p) { return static_cast<const data*>(p)->value == 0; })] = X);
  }
};
}  // namespace

test reject = [] {
  data d{};
#if defined(SML_POOL_CONTROL)
  const void* ptr = &d;
  sml::sm<c> sm{ptr};
#else
  data* ptr = &d;
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
