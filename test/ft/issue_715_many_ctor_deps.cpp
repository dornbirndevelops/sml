// Issue #715: MSVC ran out of heap (C1060) when sm<> received many distinct
// constructor dependencies.  The covariant try_get overloads (#467) deduced D
// from the pool's pool_type<D&> bases, and MSVC's deduction through N matching
// bases is exponential in N.  With the old lookup, 28 deps exhausted ~34GB.
// cppcheck-suppress missingIncludeSystem
#include <boost/sml.hpp>
// cppcheck-suppress missingIncludeSystem
#include <tuple>
// cppcheck-suppress missingIncludeSystem
#include <utility>

namespace sml = boost::sml;

#ifndef NDEPS
#define NDEPS 28
#endif

template <int>
struct Dep {};

struct Event {};
struct Idle {};
struct Done {};

struct Action {
  void operator()(Dep<0>&) const {}
};

struct Machine {
  auto operator()() const {
    using namespace sml;
    return make_transition_table(*state<Idle> + event<Event> / Action{} = state<Done>);
  }
};

template <int... Ns>
void build(std::integer_sequence<int, Ns...>) {
  std::tuple<Dep<Ns>...> deps;
  sml::sm<Machine> sm{std::get<Ns>(deps)...};
  sm.process_event(Event{});
  expect(sm.is(sml::state<Done>));
}

test many_ctor_dependencies_compile = [] { build(std::make_integer_sequence<int, NDEPS>{}); };
