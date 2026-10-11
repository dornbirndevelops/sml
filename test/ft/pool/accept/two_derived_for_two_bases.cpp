// Use case: callables depend on two unrelated interfaces (`BaseA&`, `BaseB&`)
// and the caller provides one implementation of each (`DerivedA`, `DerivedB`),
// e.g. two mocks.
//
// Allowed: in any order.
// Semantics: each `Base&` request is matched to the one argument derived from
// it; non-owning, no slicing (virtual dispatch reaches the implementations).
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct base_a {
  base_a() = default;
  base_a(const base_a&) = delete;  // like NiceMock<T>: neither copyable nor movable
  base_a& operator=(const base_a&) = delete;
  virtual ~base_a() = default;
  virtual void a() = 0;
};
struct base_b {
  base_b() = default;
  base_b(const base_b&) = delete;
  base_b& operator=(const base_b&) = delete;
  virtual ~base_b() = default;
  virtual void b() = 0;
};

struct derived_a : base_a {
  void a() override { ++calls; }
  int calls = 0;
};
struct derived_b : base_b {
  void b() override { ++calls; }
  int calls = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto use_a = [](base_a& x) { x.a(); };
    const auto use_b = [](base_b& x) { x.b(); };
    const auto use_both = [](base_a& x, base_b& y) {
      x.a();
      y.b();
    };
    return make_transition_table(*state<class idle> + event<e1> / (use_a, use_b, use_both) = X);
  }
};
}  // namespace

test derived_a_then_derived_b = [] {
  derived_a da{};
  derived_b db{};
  sml::sm<c> sm{da, db};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(2 == da.calls);
  expect(2 == db.calls);
};

test derived_b_then_derived_a = [] {
  derived_a da{};
  derived_b db{};
  sml::sm<c> sm{db, da};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(2 == da.calls);
  expect(2 == db.calls);
};
