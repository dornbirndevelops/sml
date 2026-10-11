// Rejected: a `void*` and a `const void*` argument are provided while callables
// request `void*` and `const void*`.
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates for `const void*` (`void*` converts to `const void*`).
// (Replaces the former accept/void_and_const_void_pointer_deps.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
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
    const auto write = [](void* p) { ++static_cast<data*>(p)->value; };
    return make_transition_table(*state<class idle> + event<e1>[read] / write = X);
  }
};
}  // namespace

test reject = [] {
  const data in{42};
  data out{42};
  void* out_ptr = &out;
#if defined(SML_POOL_CONTROL)
  // A single void* satisfies both requests unambiguously.
  (void)in;
  sml::sm<c> sm{out_ptr};
#else
  const void* in_ptr = &in;
  sml::sm<c> sm{out_ptr, in_ptr};
#endif
  sm.process_event(e1{});
};
