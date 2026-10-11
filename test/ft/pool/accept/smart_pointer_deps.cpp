// Use case: dependencies are held by smart pointers.
//
// Allowed:
//  - `std::shared_ptr<T>&` / `const std::shared_ptr<T>&` aliasing the caller's
//    shared_ptr (the pool never copies it, so it never shares ownership),
//  - `std::unique_ptr<T>&` / `const std::unique_ptr<T>&` aliasing the caller's
//    unique_ptr (a move-only type is never copied into the pool).
#include <boost/sml.hpp>
#include <memory>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct data {
  int value = 42;
};

struct shared {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const std::shared_ptr<data>& d) { return d && d->value == 42; };
    const auto action = [](std::shared_ptr<data>& d) { ++d->value; };
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};

struct unique {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const std::unique_ptr<data>& d) { return d != nullptr; };
    const auto action = [](std::unique_ptr<data>& d) { d.reset(); };
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};
}  // namespace

test shared_ptr_dep_from_lvalue = [] {
  auto d = std::make_shared<data>();
  sml::sm<shared> sm{d};
  expect(1 == d.use_count());  // aliased, not copied
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(43 == d->value);
};

test unique_ptr_dep_by_ref = [] {
  auto d = std::make_unique<data>();
  sml::sm<unique> sm{d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(d == nullptr);  // the action reset the caller's unique_ptr
};
