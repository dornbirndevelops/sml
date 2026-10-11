// Rejected: a ctor argument is provided whose type no callable requests (and
// which is neither the table class nor a sub state machine class).
// Why: the docs require `is_base_of<TDeps, dependencies>` for every ctor
// argument; a silently ignored argument usually hides a typo or a wrong type.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct unrelated {};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1>[([](const int& i) { return i == 42; })] = X);
  }
};
}  // namespace

test reject = [] {
  const int i = 42;
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{i};
#else
  const unrelated u{};
  sml::sm<c> sm{i, u};
#endif
  sm.process_event(e1{});
};
