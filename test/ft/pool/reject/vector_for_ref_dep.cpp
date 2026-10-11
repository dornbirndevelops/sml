// Rejected: a `std::vector<T>` is provided for a `T&` dependency.
// Why: rule "never unpack a wrapper" — the framework must not take the value
// out of std::optional / std::expected / std::vector (e.g. its n-th element)
// on the caller's behalf: it may be empty, and which element is meant is the
// caller's decision. The argument must not be silently ignored either.
// Callers unpack themselves (`w[0]`), or request the wrapper itself and
// unpack inside the callable (accept/wrappers_as_deps.cpp).
// (Siblings: vector_for_const_ref_dep.cpp.)
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>
#include <vector>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 0;  // default constructible: no accidental compile error
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / [](data& d) { ++d.value; } = X);
  }
};
}  // namespace

test reject = [] {
  std::vector<data> w{data{42}};
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{w[0]};
#else
  sml::sm<c> sm{w};
#endif
  sm.process_event(e1{});
};
