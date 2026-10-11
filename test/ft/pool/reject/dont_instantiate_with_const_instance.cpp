// Rejected: policy `sml::dont_instantiate_statemachine_class` with a CONST
// instance of the table class.
// Why: rule "never cast away const" — the state machine calls the table's
// (non-const) `operator()` and its actions may mutate it through `this`.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct table {
  auto operator()() {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [this] { ++calls; } = X);
  }
  int calls = 0;
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  table t{};
#else
  const table t{};
#endif
  sml::sm<table, sml::dont_instantiate_statemachine_class> sm{t};
  sm.process_event(e1{});
};
