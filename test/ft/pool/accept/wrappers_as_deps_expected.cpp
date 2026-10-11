// Use case: callables request a `std::expected<T, E>` ITSELF as the dependency
// and unpack it inside the callable.
//
// Allowed: by `const&` and `&`, provided as the wrapper (an lvalue).
// Semantics: the wrapper is an ordinary dependency type — the framework never
// unpacks it (see reject/expected_for_*); the callable handles the error case.
// (optional / vector: wrappers_as_deps.cpp.)
// Needs C++23 (<expected>); CMake builds *expected* TUs with CXX_STANDARD 23.
#include <boost/sml.hpp>
#include <expected>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 0;
};

using result = std::expected<data, int>;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto ok = [](const result& r) { return r.has_value() && r->value == 42; };
    const auto bump = [](result& r) {
      if (r) {
        ++r->value;
      }
    };
    return make_transition_table(*state<class idle> + event<e1>[ok] / bump = X);
  }
};
}  // namespace

test expected_unpacked_by_the_callable = [] {
  result r{data{42}};
  sml::sm<c> sm{r};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(43 == r->value);  // the caller's object, not a copy
};

test error_is_the_callables_business = [] {
  result r{std::unexpected{7}};
  sml::sm<c> sm{r};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
};
