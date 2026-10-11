// Use case: policy `sml::deps<Ts...>` — a dependency is only reached through a
// fully generic callable (`aux::get<T&>(deps)`), so no signature names it (#437).
//
// Allowed:
//  - `sml::deps<T>` / `sml::deps<T&>` / `sml::deps<const T&>` (all normalise to
//    the same `T&` slot),
//  - naming a type in `sml::deps<>` that some typed callable also requests,
//  - several types in one `sml::deps<>`.
// Semantics: identical to a `T&` dependency — non-owning, one shared slot.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct e2 {};

struct dep_a {
  int value = 0;
};
struct dep_b {
  int value = 0;
};

const auto generic_action = [](auto, auto&, auto& deps, auto&) {
  sml::aux::get<dep_a&>(deps).value += 1;
  sml::aux::get<dep_b&>(deps).value += 2;
};

struct only_generic {
  auto operator()() const noexcept {
    using namespace sml;
    return make_transition_table(*state<class idle> + event<e1> / generic_action = X);
  }
};

struct generic_and_typed {
  auto operator()() const noexcept {
    using namespace sml;
    const auto typed = [](const dep_a& a) { return a.value == 1; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + event<e1> / generic_action = state<class done>,
       state<class done> + event<e2> [typed] = X
    );
    // clang-format on
  }
};
}  // namespace

test explicit_deps_plain_types = [] {
  dep_a a{};
  dep_b b{};
  sml::sm<only_generic, sml::deps<dep_a, dep_b>> sm{a, b};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == a.value);
  expect(2 == b.value);
};

test explicit_deps_reference_spellings = [] {
  dep_a a{};
  dep_b b{};
  sml::sm<only_generic, sml::deps<dep_a&, const dep_b&>> sm{b, a};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == a.value);
  expect(2 == b.value);
};

test explicit_deps_overlapping_typed_callable = [] {
  dep_a a{};
  dep_b b{};
  sml::sm<generic_and_typed, sml::deps<dep_a, dep_b>> sm{a, b};
  sm.process_event(e1{});
  sm.process_event(e2{});
  expect(sm.is(sml::X));
};
