// Use case: a callable requests `void*` (type erasure) and the caller provides a
// `void*`; the callable casts it back itself.
//
// Allowed: as rvalue or lvalue, like any other pointer dependency.
// Semantics: the pool stores the pointer value unchanged. Only the callable
// casts it back to a typed pointer — the framework never does (see
// reject/void_ptr_for_*, reject/*_for_void_ptr).
// (Siblings: const_void_pointer_dep.cpp, reject/ambiguous_void_and_const_void_for_const_void.cpp,
// void_pointer_for_const_void_pointer.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto write = [](void* p) { ++static_cast<data*>(p)->value; };
    return make_transition_table(*state<class idle> + event<e1> / write = X);
  }
};
}  // namespace

test as_rvalues = [] {
  data out{};
  sml::sm<c> sm{static_cast<void*>(&out)};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == out.value);  // the caller's object, written by the callable
};

test as_lvalues = [] {
  data out{};
  void* out_ptr = &out;
  sml::sm<c> sm{out_ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == out.value);  // the caller's object, written by the callable
};
