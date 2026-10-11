// Rejected: a `const T*` is provided for a `T*` dependency.
// Why: const-correctness — no implicit const_cast.
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
  data* ptr = &d;
#else
  const data d{};
  const data* ptr = &d;
#endif
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
};
