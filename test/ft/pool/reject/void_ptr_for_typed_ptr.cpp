// Rejected: a `void*` is provided for a `T*` dependency.
// Why: rule "only the library user casts a void*" — the framework must not
// static_cast it to a typed pointer (it cannot know what it points to).
// (Allowed: accept/*void_pointer_dep.cpp — the callable requests `void*` /
// `const void*` and casts it itself.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 0;  // default constructible: no accidental compile error
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](data* p) { ++p->value; } = X);
  }
};
}  // namespace

test reject = [] {
  data d{};
#if defined(SML_POOL_CONTROL)
  data* ptr = &d;
  sml::sm<c> sm{ptr};
#else
  void* ptr = &d;
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
