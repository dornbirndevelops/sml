// Rejected: a `T*` is provided for a `const T&` dependency.
// Why: the pool never dereferences an argument implicitly (a pointer may be
// null); adding const does not change that. The mismatched argument must not be
// silently ignored either. (Siblings: pointer_for_ref_dep.cpp,
// const_pointer_for_const_ref_dep.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct config {
  bool enabled = true;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1>[([](const config& k) { return k.enabled; })] = X);
  }
};
}  // namespace

test reject = [] {
  config k{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{k};
#else
  config* ptr = &k;
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
