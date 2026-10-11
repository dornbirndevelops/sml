// Rejected: a `T&` and a `const T*` argument are provided while callables
// request `T&` and `const T*` (e.g. "current" and "previous" of one type).
// Why: rule "ambiguous transformations are invalid" — both arguments are valid
// candidates for `const T*` (the pointer itself, or the address of the
// object). (Replaces the former accept/same_type_as_ref_and_pointer.cpp.)
// Workaround shown by the control: give the second role its own type.
//
// Must NOT compile. With SML_POOL_CONTROL defined this TU contains the closest
// valid usage instead and must compile.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};
struct node {
  int value = 0;
};
#if defined(SML_POOL_CONTROL)
struct previous_node {
  const node* ptr;
};
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](node& current, const previous_node& previous) { current.value = previous.ptr->value + 1; };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
#else
struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto action = [](node& current, const node* previous) { current.value = previous->value + 1; };
    return make_transition_table(*state<class idle> + event<e1> / action = X);
  }
};
#endif
}  // namespace

test reject = [] {
  node current{};
  const node previous{41};
#if defined(SML_POOL_CONTROL)
  const previous_node prev{&previous};
  sml::sm<c> sm{current, prev};
#else
  sml::sm<c> sm{current, &previous};
#endif
  sm.process_event(e1{});
};
