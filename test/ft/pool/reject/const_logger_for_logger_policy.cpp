// Rejected: a const lvalue is provided as the logger of the `sml::logger<L>`
// policy.
// Why: rule "never cast away const" — the state machine calls the logger's
// non-const `log_*` member functions.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct logger {
  template <class SM, class TEvent>
  void log_process_event(const TEvent&) {
    ++events;
  }
  template <class SM, class TGuard, class TEvent>
  void log_guard(const TGuard&, const TEvent&, bool) {}
  template <class SM, class TAction, class TEvent>
  void log_action(const TAction&, const TEvent&) {}
  template <class SM, class TSrcState, class TDstState>
  void log_state_change(const TSrcState&, const TDstState&) {}
  int events = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  logger l{};
#else
  const logger l{};
#endif
  sml::sm<c, sml::logger<logger>> sm{l};
  sm.process_event(e1{});
};
