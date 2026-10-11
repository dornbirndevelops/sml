// Rejected: a const lvalue of T is provided for a dependency declared with the
// `sml::deps<T>` policy (which grants mutable `aux::get<T&>(deps)` access).
// Why: rule "never cast away const" — applies to explicit deps just as to
// dependencies deduced from callable signatures.
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
#if defined(SML_POOL_CONTROL)
  counter cnt{};
#else
  const counter cnt{};
#endif
  sml::sm<c, sml::deps<counter>> sm{cnt};
  sm.process_event(e1{});
};
