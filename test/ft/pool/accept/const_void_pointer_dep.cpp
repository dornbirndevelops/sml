// Use case: a callable requests `const void*` (type erasure) and the caller
// provides a `const void*`; the callable casts it back itself.
//
// Allowed: as rvalue or lvalue, like any other pointer dependency.
// Semantics: the pool stores the pointer value unchanged. Only the callable
// casts it back to a typed pointer — the framework never does (see
// reject/void_ptr_for_*, reject/*_for_void_ptr).
// (Siblings: void_pointer_dep.cpp, reject/ambiguous_void_and_const_void_for_const_void.cpp,
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
    const auto read = [](const void* p) { return static_cast<const data*>(p)->value == 42; };
    return make_transition_table(*state<class idle> + event<e1>[read] = X);
  }
};
}  // namespace

test as_rvalues = [] {
  const data in{42};
  sml::sm<c> sm{static_cast<const void*>(&in)};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test as_lvalues = [] {
  const data in{42};
  const void* in_ptr = &in;
  sml::sm<c> sm{in_ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};
