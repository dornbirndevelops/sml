// Rejected: a `const T&` dependency is not provided (without
// BOOST_SML_CREATE_DEFAULT_CONSTRUCTIBLE_DEPS), even though T is default
// constructible.
// Why: same as for `T&` — there is no object to alias; binding the reference to
// a temporary default-constructed T would dangle.
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
#if defined(SML_POOL_CONTROL)
  const config k{};
  sml::sm<c> sm{k};
#else
  sml::sm<c> sm{};
#endif
  sm.process_event(e1{});
};
