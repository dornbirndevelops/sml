// Use case: callables request a wrapper (`std::optional<T>`, `std::vector<T>`)
// ITSELF as the dependency and unpack it inside the callable.
//
// Allowed: by `const&` and `&`, provided as the wrapper (an lvalue).
// Semantics: the wrapper is an ordinary dependency type — the framework never
// unpacks it (see reject/*optional*, reject/*vector*); the callable decides
// how to deal with emptiness / which element to use.
// (std::expected: wrappers_as_deps_expected.cpp.)
#include <boost/sml.hpp>
#include <optional>
#include <vector>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto has_value = [](const std::optional<data>& o) { return o.has_value() && o->value == 42; };
    const auto bump_first = [](std::vector<data>& v) {
      if (!v.empty()) {
        ++v[0].value;
      }
    };
    const auto limit = [](const std::optional<int>& l) { expect(l.value_or(0) == 3); };
    return make_transition_table(*state<class idle> + event<e1>[has_value] / (bump_first, limit) = X);
  }
};
}  // namespace

test wrappers_unpacked_by_the_callable = [] {
  const std::optional<data> o{data{42}};
  std::vector<data> v{data{1}, data{2}};
  const std::optional<int> l{3};
  sml::sm<c> sm{o, v, l};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(2 == v[0].value);  // the caller's vector, not a copy
  expect(2 == v[1].value);
};

test empty_wrapper_is_the_callables_business = [] {
  const std::optional<data> o{};  // empty: the guard says no
  std::vector<data> v{};
  const std::optional<int> l{3};
  sml::sm<c> sm{o, v, l};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
};
