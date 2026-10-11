// Use case: a `Derived*` is provided for a `Base*` dependency (pointer
// conversion derived-to-base), e.g. a mock behind an interface.
//
// Allowed: as an rvalue (`&d`) or as an lvalue pointer variable.
// Semantics: the pool stores the converted pointer value — it points at the
// caller's object (no copy, no slice), so virtual dispatch reaches the
// implementation and mutations through the pointer are visible to the caller.
// (Const variants: derived_ptr_for_const_base_ptr.cpp,
// mutable_derived_ptr_for_const_base_ptr.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct iface {
  iface() = default;
  iface(const iface&) = delete;  // like NiceMock<T>: neither copyable nor movable
  iface& operator=(const iface&) = delete;
  virtual ~iface() = default;
  virtual void hit() = 0;
};

struct impl : iface {
  void hit() override { ++hits; }
  int hits = 0;
};

iface* seen = nullptr;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](iface* i) {
      seen = i;
      if (i != nullptr) {
        i->hit();
      }
    };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
}  // namespace

test derived_pointer_rvalue = [] {
  impl i{};
  sml::sm<c> sm{&i};
  sm.process_event(e1{});
  expect(static_cast<iface*>(&i) == seen);
  expect(1 == i.hits);
};

test derived_pointer_lvalue = [] {
  impl i{};
  impl* ptr = &i;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(static_cast<iface*>(&i) == seen);
  expect(1 == i.hits);
};
