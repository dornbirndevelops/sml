// Rejected: a `T*` dependency is not provided.
// Why: a pointer dependency is a non-owning handle like `T&`; silently
// injecting nullptr turns a forgotten ctor argument into a runtime crash (cf.
// #485). Callers that really want "no object" pass `static_cast<T*>(nullptr)`.
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
  data d{};
  sml::sm<c> sm{&d};
#else
  sml::sm<c> sm{};
#endif
  sm.process_event(e1{});
};
