// Use case: policy `sml::logger<TLogger>` — the logger lives in the pool as a
// `TLogger&` dependency.
//
// Allowed:
//  - providing the logger as a non-const lvalue, mixed with other dependencies,
//  - a callable also requesting the logger type as an ordinary `TLogger&`.
// Semantics: non-owning; the state machine logs into the caller's logger.
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
  void log_state_change(const TSrcState&, const TDstState&) {
    ++state_changes;
  }

  int events = 0;
  int state_changes = 0;
  int manual = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](const int& i, logger& l) { l.manual += i; };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
}  // namespace

test logger_is_the_callers_object = [] {
  logger l{};
  const int i = 42;
  sml::sm<c, sml::logger<logger>> sm{l, i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(l.events >= 1);
  expect(1 == l.state_changes);
  expect(42 == l.manual);
};
