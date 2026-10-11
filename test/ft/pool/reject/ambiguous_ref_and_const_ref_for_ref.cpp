// Rejected: `T&` and `const T&` arguments are provided for a `T&` dependency.
// Why: rule "ambiguous transformations are invalid" — both arguments compete for the
// slot (same underlying type T).
// (Formerly accept/unambiguous_ref_and_const_ref_for_ref.cpp: even though only one argument
// could legally fill the slot, providing the same underlying type twice is
// ambiguous for the reader, so it is rejected.)
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
    return make_transition_table(*state<class idle> + event<e1> / [](data& d) { ++d.value; } = X);
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
