// Use case: callables depend on two unrelated interfaces, one by const
// reference (`const BaseA&`) and one by mutable reference (`BaseB&`); the
// caller provides a non-const implementation of each (`DerivedA`, `DerivedB`).
//
// Allowed: in any order.
// Semantics: each request is matched to the one argument derived from it;
// non-owning, no slicing. The const view observes the caller's object live.
// (All-mutable variant: two_derived_for_two_bases.cpp.)
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
  bool ready() const override { return is_ready; }
  bool is_ready = false;
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

test derived_a_then_derived_b = [] {
  derived_a da{};
  derived_b db{};
  sml::sm<c> sm{da, db};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  da.is_ready = true;  // observed live through `const base_a&`
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == db.calls);
};

test derived_b_then_derived_a = [] {
  derived_a da{};
  derived_b db{};
  da.is_ready = true;
  sml::sm<c> sm{db, da};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == db.calls);
};
