// Use case: a const lvalue of Derived is provided for a `const Base*`
// dependency (object-for-pointer combined with derived-to-base).
//
// Allowed.
// Semantics: non-owning — the pool stores the address of the caller's object
// (converted to `const Base*`, never of a copy or a slice), so virtual dispatch
// reaches the implementation and the pointee is observed live.
// (Related: const_lvalue_for_const_pointer_dep.cpp, base_ref_from_derived.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct iface {
  iface() = default;
  iface(const iface&) = delete;  // like NiceMock<T>: neither copyable nor movable
  iface& operator=(const iface&) = delete;
  virtual ~iface() = default;
  virtual bool ready() const = 0;
};

struct impl : iface {
  explicit impl(bool r) : is_ready{r} {}
  bool ready() const override { return is_ready; }
  bool is_ready;
};

const iface* seen = nullptr;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto ready = [](const iface* i) {
      seen = i;
      return i != nullptr && i->ready();
    };
    return make_transition_table(*state<class idle> + event<e1>[ready] = X);
  }
};
}  // namespace

test const_derived_for_const_base_pointer = [] {
  const impl i{true};
  sml::sm<c> sm{i};
  sm.process_event(e1{});
  expect(static_cast<const iface*>(&i) == seen);  // the caller's object
  expect(sm.is(sml::X));
};

test const_derived_view_observes_live_value = [] {
  impl i{false};
  const impl& view = i;  // the state machine only gets a const view
  sml::sm<c> sm{view};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  i.is_ready = true;  // mutated through the caller's non-const name
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
