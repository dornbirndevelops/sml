// Rejected: a const lvalue of T is provided for a `T*` dependency.
// Why: const-correctness — taking its address yields `const T*`; no implicit
// const_cast. (A non-const lvalue is fine: accept/lvalue_for_pointer_dep.cpp.)
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
    return make_transition_table(*state<class idle> + event<e1> / [](data* d) { ++d->value; } = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  // `&d` rather than `d`: object-for-pointer is not implemented yet, and the
  // control must only depend on already-working behaviour.
  data d{};
  sml::sm<c> sm{&d};
#else
  const data d{};
  sml::sm<c> sm{d};
#endif
  sm.process_event(e1{});
};
