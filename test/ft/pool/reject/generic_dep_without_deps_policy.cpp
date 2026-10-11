// Rejected: a dependency is reached only through a fully generic callable
// (`aux::get<T&>(deps)`) but is not declared with `sml::deps<T>` (#437).
// Why: no signature names T, so T is not part of the pool.
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
    const auto action = [](auto, auto&, auto& deps, auto&) { sml::aux::get<counter&>(deps).n++; };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
}  // namespace

test reject = [] {
  counter cnt{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c, sml::deps<counter>> sm{cnt};
#else
  sml::sm<c> sm{cnt};
#endif
  sm.process_event(e1{});
};
