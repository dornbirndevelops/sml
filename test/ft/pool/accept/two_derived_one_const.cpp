// Use case: callables depend on two unrelated interfaces, one by const
// reference (`const BaseA&`) and one by mutable reference (`BaseB&`); the
// caller provides a CONST implementation of the first (`const DerivedA&`) and
// a non-const implementation of the second (`DerivedB&`).
//
// Allowed: in any order.
// Semantics: each request is matched to the one argument derived from it;
// non-owning, no slicing. The const view observes the caller's object live.
// (Non-const DerivedA: two_derived_for_const_and_mutable_bases.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct base_a {
  base_a() = default;
  base_a(const base_a&) = delete;  // like NiceMock<T>: neither copyable nor movable
  base_a& operator=(const base_a&) = delete;
  virtual ~base_a() = default;
  virtual bool ready() const = 0;
};
struct base_b {
  base_b() = default;
  base_b(const base_b&) = delete;
  base_b& operator=(const base_b&) = delete;
  virtual ~base_b() = default;
  virtual void b() = 0;
};

struct derived_a : base_a {
  explicit derived_a(bool r) : is_ready{r} {}
  bool ready() const override { return is_ready; }
  bool is_ready;
};
struct derived_b : base_b {
  void b() override { ++calls; }
  int calls = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto ready = [](const base_a& x) { return x.ready(); };
    const auto use_b = [](base_b& y) { y.b(); };
    return make_transition_table(*state<class idle> + event<e1>[ready] / use_b = X);
  }
};
}  // namespace

test const_derived_a_then_derived_b = [] {
  const derived_a da{true};
  derived_b db{};
  sml::sm<c> sm{da, db};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == db.calls);
};

test derived_b_then_const_derived_a = [] {
  const derived_a da{true};
  derived_b db{};
  sml::sm<c> sm{db, da};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == db.calls);
};

test const_view_observes_live_value = [] {
  derived_a da{false};
  const derived_a& view = da;  // the state machine only gets a const view
  derived_b db{};
  sml::sm<c> sm{view, db};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  da.is_ready = true;  // mutated through the caller's non-const name
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == db.calls);
};
