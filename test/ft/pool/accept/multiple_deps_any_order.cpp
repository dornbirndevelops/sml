// Use case: several non-owning dependencies of different kinds (`T&`,
// `const T&`, `T*`) are needed by different callables.
//
// Allowed: providing them as ctor arguments (lvalues) in any order.
// Semantics: each argument is routed to its slot by type, not by position.
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct out {
  int value = 0;
};
struct cfg {
  int factor = 2;
};
struct journal {
  int lines = 0;
};

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto guard = [](const int& i, const cfg& k) { return i * k.factor == 84; };
    const auto action = [](const int& i, const double& d, out& o, journal* l) {
      o.value = i + static_cast<int>(d);
      ++l->lines;
    };
    return make_transition_table(*state<class idle> + event<e1>[guard] / action = X);
  }
};

template <class TSm>
void run(TSm& sm, const out& o, const journal& l) {
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(42 + 87 == o.value);
  expect(1 == l.lines);
}
}  // namespace

test deps_in_declaration_order = [] {
  const int i = 42;
  const double d = 87.0;
  out o{};
  const cfg k{};
  journal l{};
  sml::sm<c> sm{i, d, o, k, &l};
  run(sm, o, l);
};

test deps_in_reverse_order = [] {
  const int i = 42;
  const double d = 87.0;
  out o{};
  const cfg k{};
  journal l{};
  sml::sm<c> sm{&l, k, o, d, i};
  run(sm, o, l);
};

test deps_in_shuffled_order = [] {
  const int i = 42;
  const double d = 87.0;
  out o{};
  const cfg k{};
  journal l{};
  sml::sm<c> sm{o, i, &l, d, k};
  run(sm, o, l);
};
