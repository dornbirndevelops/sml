// Rejected: a `const T*` dependency is not provided.
// Why: same as missing_pointer_dep.cpp — a pointer dependency is a non-owning
// handle; silently injecting nullptr turns a forgotten ctor argument into a
// runtime crash. Callers that really want "no object" pass
// `static_cast<const T*>(nullptr)`.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 42;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1>[([](const data* d) { return d->value == 42; })] = X);
  }
};
}  // namespace

test reject = [] {
#if defined(SML_POOL_CONTROL)
  const data d{};
  sml::sm<c> sm{&d};
#else
  sml::sm<c> sm{};
#endif
  sm.process_event(e1{});
};
