// Use case: a `void*` is provided for a `const void*` dependency (adding const,
// like `T*` for `const T*`).
//
// Allowed: as rvalue or lvalue.
// Semantics: the pool stores the pointer value; only the callable casts it back
// to a typed pointer.
// (Exact void* / const void*: void_pointer_dep.cpp, const_void_pointer_dep.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 42;
};

const void* seen = nullptr;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto read = [](const void* p) {
      seen = p;
      return p != nullptr && static_cast<const data*>(p)->value == 42;
    };
    return make_transition_table(*state<class idle> + event<e1>[read] = X);
  }
};
}  // namespace

test void_pointer_rvalue_for_const_void_pointer = [] {
  data d{};
  sml::sm<c> sm{static_cast<void*>(&d)};
  sm.process_event(e1{});
  expect(static_cast<const void*>(&d) == seen);
  expect(sm.is(sml::X));
};

test void_pointer_lvalue_for_const_void_pointer = [] {
  data d{};
  void* ptr = &d;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(static_cast<const void*>(&d) == seen);
  expect(sm.is(sml::X));
};
