// Use case: one implementation of two unrelated interfaces (`Impl : BaseA, BaseB`)
// is provided once, while callables request `const BaseA&`, `BaseB&` and `Impl&`.
//
// Allowed: one argument may serve several requests, as long as it is the only
// argument that could serve each of them.
// Semantics: every request refers to the caller's one object; non-owning, no
// slicing (virtual dispatch reaches the implementation).
// (One argument per interface: two_derived_for_two_bases.cpp.)
#include <boost/sml.hpp>

namespace sml = boost::sml;

namespace {
struct e1 {};

struct base_a {
  virtual ~base_a() = default;
  virtual bool ready() const = 0;
};
struct base_b {
  virtual ~base_b() = default;
  virtual void hit() = 0;
};

struct impl : base_a, base_b {
  bool ready() const override { return is_ready; }
  void hit() override { ++hits; }
  bool is_ready = false;
  int hits = 0;
};

const impl* seen = nullptr;

struct c {
  auto operator()() const noexcept {
    using namespace sml;
    const auto ready = [](const base_a& a) { return a.ready(); };
    const auto use_b = [](base_b& b) { b.hit(); };
    const auto use_impl = [](impl& i) { seen = &i; };
    return make_transition_table(*state<class idle> + event<e1>[ready] / (use_b, use_impl) = X);
  }
};
}  // namespace

test one_argument_for_two_bases_and_itself = [] {
  impl i{};
  sml::sm<c> sm{i};
  sm.process_event(e1{});
  expect(!sm.is(sml::X));
  i.is_ready = true;  // observed live through `const base_a&`
  sm.process_event(e1{});
  expect(sm.is(sml::X));
  expect(1 == i.hits);
  expect(&i == seen);
};
