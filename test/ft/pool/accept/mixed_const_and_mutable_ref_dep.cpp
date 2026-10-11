// Use case: the same dependency type is named as `T&` by one callable and as
// `const T&` by another.
//
// Allowed: providing a single non-const lvalue of T.
// Semantics: both spellings collapse onto ONE pool slot aliasing the caller's
// object, so a const guard sees what a mutable action wrote (cf. #530).
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
// state tags shared by the table and the checks below
class idle;
struct e1 {};

struct data {
  int value = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto write = [](data& d) { d.value = 42; };
    const auto check = [](const data& d) { return d.value == 42; };
    // clang-format off
    return make_transition_table(
      *state<class idle> + event<e1> [!check] / write = state<class written>,
       state<class written> + event<e1> [check] = X
    );
    // clang-format on
  }
};
}  // namespace

test const_and_mutable_ref_share_one_object = [] {
  data d{};
  sml::sm<c> sm{d};
  sm.process_event(e1{});
  expect(42 == d.value);
  sm.process_event(e1{});
  expect(sm.is(sml::X));
};

test const_view_observes_caller_mutation = [] {
  data d{};
  sml::sm<c> sm{d};
  d.value = 42;  // guard `!check` now fails, so the first transition is blocked
  sm.process_event(e1{});
  expect(sm.is(sml::state<class idle>));
};
