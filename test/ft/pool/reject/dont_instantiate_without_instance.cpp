// Rejected: policy `sml::dont_instantiate_statemachine_class` without
// providing an instance of the table class.
// Why: the policy means "the caller owns the instance"; there is none.
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
  sml::sm<table, sml::dont_instantiate_statemachine_class> sm{t};
#else
  sml::sm<table, sml::dont_instantiate_statemachine_class> sm{};
#endif
  sm.process_event(e1{});
};
