// Use case: a dependency taken by *pointer* (`T*`).
//
// Allowed: providing a `T*` as an rvalue (`&t`) or as an lvalue pointer variable.
// Semantics: the pool stores the pointer value (cf. #485: an lvalue pointer must
// not degrade to nullptr). Pointee mutations are shared with the caller.
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
    const auto action = [](data* d) {
      expect(d != nullptr);
      ++d->value;
    };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
}  // namespace

test pointer_dep_from_rvalue = [] {
  data d{};
  sml::sm<c> sm{&d};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == d.value);
};

test pointer_dep_from_lvalue = [] {
  data d{};
  data* ptr = &d;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == d.value);
};

test pointer_dep_from_const_lvalue_pointer = [] {
  data d{};
  data* const ptr = &d;
  sml::sm<c> sm{ptr};
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == d.value);
};
