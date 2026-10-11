// Use case: a dependency taken by *const reference* (`const T&`).
//
// Allowed: providing a const lvalue or a non-const lvalue of T.
// Semantics: non-owning. The pool aliases the caller's object, so the state
// machine observes the *live* value, not a snapshot taken at construction.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct config {
  bool enabled = false;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto enabled = [](const config& cfg) { return cfg.enabled; };
    return make_transition_table(*state<class idle> + event<e1>[enabled] = X);
  }
};
}  // namespace

test const_ref_dep_from_const_lvalue = [] {
  const config cfg{true};
  sml::sm<c> sm{cfg};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_ref_dep_from_mutable_lvalue = [] {
  config cfg{true};
  sml::sm<c> sm{cfg};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_ref_dep_observes_live_value = [] {
  config cfg{false};
  sml::sm<c> sm{cfg};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  cfg.enabled = true;  // must be observed: the pool aliases cfg
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
