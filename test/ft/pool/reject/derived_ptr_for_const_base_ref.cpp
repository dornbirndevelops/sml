// Rejected: a `Derived*` is provided for a `const Base&` dependency.
// Why: the pool never dereferences an argument implicitly (a pointer may be
// null) — derived-to-base does not change that. The argument must not be
// silently ignored either (which would bind `const Base&` to a temporary).
// (Same-type siblings: pointer_for_const_ref_dep.cpp, pointer_for_ref_dep.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct base {
  bool enabled = true;  // default constructible: no accidental compile error
};
struct derived : base {};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1>[([](const base& b) { return b.enabled; })] = X);
  }
};
}  // namespace

test reject = [] {
  derived d{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{d};
#else
  sml::sm<c> sm{&d};
#endif
  sm.process_event(e1{});
};
