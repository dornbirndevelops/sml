// Rejected: a `std::unique_ptr<T>` is provided for a `T&` / `const T&` dependency.
// (Sibling: shared_ptr_for_ref_dep.cpp.)
// Why: rule "never dereference a pointer" — smart pointers included (they may
// be null). The argument must not be silently ignored either. Callers that
// want the pointee pass `*ptr` themselves.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>
#include <memory>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct data {
  int value = 0;  // default constructible: no accidental compile error
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto check = [](const data& d) { return d.value == 0; };
    const auto write = [](data& d) { d.value = 42; };
    return make_transition_table(*state<class idle> + event<e1>[check] / write = X);
  }
};
}  // namespace

test reject = [] {
  auto ptr = std::make_unique<data>();
#if defined(SML_POOL_CONTROL)
  sml::sm<c> sm{*ptr};
#else
  sml::sm<c> sm{ptr};
#endif
  sm.process_event(e1{});
};
