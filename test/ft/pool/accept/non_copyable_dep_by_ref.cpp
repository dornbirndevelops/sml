// Use case: dependencies taken by reference whose type is restricted:
//  - move-only,
//  - neither copyable nor movable,
//  - not default constructible.
//
// Allowed: providing a (non-const for `T&`) lvalue.
// Semantics: a reference slot never copies, moves or default-constructs T.
#include <boost/sml.hpp>
#include <memory>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct move_only {
  move_only() = default;
  move_only(move_only&&) = default;
  move_only& operator=(move_only&&) = default;
  std::unique_ptr<int> value = std::make_unique<int>(42);
};

struct pinned {
  pinned() = default;
  pinned(const pinned&) = delete;
  pinned(pinned&&) = delete;
  pinned& operator=(const pinned&) = delete;
  pinned& operator=(pinned&&) = delete;
  int value = 42;
};

struct no_default {
  explicit no_default(int v) : value{v} {}
  int value;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const move_only& m, const pinned& p, const no_default& n) {
      return *m.value == 42 && p.value == 42 && n.value == 42;
    };
    const auto action = [](move_only& m, pinned& p, no_default& n) {
      ++*m.value;
      ++p.value;
      ++n.value;
    };
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};
}  // namespace

test restricted_types_by_ref = [] {
  move_only m{};
  pinned p{};
  no_default n{42};
  sml::sm<c> sm{m, p, n};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(43 == *m.value);
  expect(43 == p.value);
  expect(43 == n.value);
};
