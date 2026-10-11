// Use case: callables depend on an *interface* (`Base&` / `const Base&`) and the
// caller provides an implementation (`Derived`), e.g. a production object or a
// (non-copyable) mock such as gmock's NiceMock<T> (cf. #467).
//
// Allowed: providing a non-const lvalue of Derived for `Base&`; a const or
// non-const lvalue of Derived for `const Base&`.
// Semantics: non-owning; the slot aliases the caller's Derived object, so
// virtual dispatch reaches the implementation and no slicing occurs.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct iface {
  iface() = default;
  iface(const iface&) = delete;  // like NiceMock<T>: neither copyable nor movable
  iface& operator=(const iface&) = delete;
  virtual ~iface() = default;
  virtual int id() const = 0;
  virtual void hit() = 0;
};

struct impl : iface {
  int id() const override { return 42; }
  void hit() override { ++hits; }
  int hits = 0;
};

struct uses_mutable {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](iface& i) { i.hit(); };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};

struct uses_const {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const iface& i) { return i.id() == 42; };
    return make_transition_table(*state<class idle> + event<e1>[guard] = X);
  }
};

struct uses_both {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const iface& i) { return i.id() == 42; };
    const auto action = [](iface& i) { i.hit(); };
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};
}  // namespace

test derived_lvalue_for_base_ref = [] {
  impl i{};
  sml::sm<uses_mutable> sm{i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == i.hits);
};

test derived_lvalue_for_const_base_ref = [] {
  impl i{};
  sml::sm<uses_const> sm{i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_derived_lvalue_for_const_base_ref = [] {
  const impl i{};
  sml::sm<uses_const> sm{i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test derived_lvalue_for_base_ref_and_const_base_ref = [] {
  impl i{};
  sml::sm<uses_both> sm{i};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == i.hits);
};
