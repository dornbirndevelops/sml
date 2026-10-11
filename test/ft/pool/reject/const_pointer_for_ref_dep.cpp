// Rejected: a `const T*` is provided for a `T&` dependency.
// Why: doubly wrong — the pool never dereferences an argument implicitly, and
// even dereferenced the object would be const (no implicit const_cast). The
// mismatched argument must not be silently ignored either. (Siblings:
// pointer_for_ref_dep.cpp, pointer_for_const_ref_dep.cpp,
// const_pointer_for_const_ref_dep.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct counter {
  int n = 0;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](counter& cnt) { ++cnt.n; } = X);
  }
};
}  // namespace

test reject = [] {
  counter cnt{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{cnt};
#else
  const counter* ptr = &cnt;
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
