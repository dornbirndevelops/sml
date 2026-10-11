// Rejected: `T&` and `T*` arguments are provided for a `T*` dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates (the pointer itself, or the address of the object).
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
  data a{};
  data b{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{&b};
#else
  sml::sm<c> sm{a, &b};
#endif
  sm.process_event(e1{});
};
