// Use case: a `const Derived*` is provided for a `const Base*` dependency
// (pointer conversion derived-to-base), e.g. a const mock behind an interface.
//
// Allowed: as an rvalue (`&d`) or as an lvalue pointer variable.
// Semantics: the pool stores the converted pointer value — it points at the
// caller's object (no copy, no slice), so virtual dispatch reaches the
// implementation and the pointee is observed live.
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

test const_derived_pointer_rvalue = [] {
  const impl i{true};
  sml::sm<c> sm{&i};
  sm.process_event(e1{});
  expect(static_cast<const iface*>(&i) == seen);
  expect(sm.is(sml::X));
};

test const_derived_pointer_lvalue = [] {
  const impl i{true};
  const impl* ptr = &i;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(static_cast<const iface*>(&i) == seen);
  expect(sm.is(sml::X));
};

test const_derived_pointer_observes_live_pointee = [] {
  impl i{false};
  const impl* ptr = &i;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  i.is_ready = true;
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
