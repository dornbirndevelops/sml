// Rejected: `T&` and `const T&` arguments are provided for a `const T&`
// dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates for `const T&`.
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
    return make_transition_table(*state<class idle> + event<e1> [([](const data& d) { return d.value == 0; })] = X);
  }
};
}  // namespace

test reject = [] {
  data a{};
  const data b{};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{a};
#else
  sml::sm<c> sm{a, b};
#endif
  sm.process_event(e1{});
};
