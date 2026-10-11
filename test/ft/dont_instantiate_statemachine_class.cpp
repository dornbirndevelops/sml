 #include <boost/sml.hpp>

namespace sml = boost::sml;

const auto idle = sml::state<class idle>;
const auto idle2 = sml::state<class idle2>;
const auto s1 = sml::state<class s1>;
const auto s2 = sml::state<class s2>;

test non_empty_statemachine_class_with_deleted_copy_constructor = []() {
  struct non_empty_statemachine_class {
    non_empty_statemachine_class() = default;
    non_empty_statemachine_class(const non_empty_statemachine_class &) = delete;

    auto operator()() {
      using namespace sml;
      return make_transition_table(*"start"_s + on_entry<_> / [this]() {});
    }

    int some_variable_to_make_class_not_empty = 0;
  };


  non_empty_statemachine_class instance;

  boost::sml::sm<non_empty_statemachine_class, sml::dont_instantiate_statemachine_class>{
    instance
  };
};

test non_empty_statemachine_class_with_sub_statemachine = []() {
  struct sub {
    sub() = default;
    sub(const sub &) = delete;

    auto operator()() noexcept {
      using namespace sml;

      // clang-format off
      return make_transition_table(
         *"idle"_s + on_entry<_> / [this] { }
      );
      // clang-format on
    }

    int a_in_sub = 0;
  };

  struct StateMachine {
    StateMachine() = default;
    StateMachine(const StateMachine &) = delete;

    auto operator()() {
      using namespace sml;
      return make_transition_table(*"start"_s = state<sub>);
    }

    int privateMemberVariable = 0;
  };

  StateMachine stateMachineInstance;
  sub subInstance;

  sml::sm<StateMachine, sml::dont_instantiate_statemachine_class> sm{stateMachineInstance, subInstance};
};

// Issue #715: the state machine class and the sub state machine classes are no deps, so
// only an instance of that very class is read for them, also next to an instance of a
// class derived from it (the covariant lookup of a dep would find both).
struct e1_715 {};
struct e2_715 {};
struct e3_715 {};

struct sub715 {
  auto operator()() {
    using namespace sml;
    return make_transition_table(*idle + event<e1_715> / [this] { ++hits; } = s1);
  }
  int hits = 0;
};
struct derived_sub715 : sub715 {
  auto operator()() {
    using namespace sml;
    return make_transition_table(*idle2 + event<e1_715> / [this] { hits += 10; } = s2);
  }
};
struct top715 {
  auto operator()() {
    using namespace sml;
    // clang-format off
    return make_transition_table(
       *idle + event<e2_715> = state<sub715>,
        state<sub715> + event<e3_715> = state<derived_sub715>
    );
    // clang-format on
  }
  int a_member = 0;
};

test sub_statemachine_class_derived_from_another_sub_statemachine_class = [] {
  top715 top;
  sub715 sub;
  derived_sub715 derived_sub;
  sml::sm<top715, sml::dont_instantiate_statemachine_class> sm{top, sub, derived_sub};
  sm.process_event(e2_715{});
  sm.process_event(e1_715{});
  sm.process_event(e3_715{});
  sm.process_event(e1_715{});
  expect(1 == sub.hits);
  expect(10 == derived_sub.hits);
};

struct recorder715;
struct table715 {
  auto operator()() const;
  int a_member = 0;
};
struct recorder715 : table715 {
  int n = 0;
};
inline auto table715::operator()() const {
  using namespace sml;
  return make_transition_table(*idle + event<e1_715> / [](recorder715 &r) { ++r.n; } = X);
}

test dep_derived_from_the_statemachine_class = [] {
  table715 table;
  recorder715 recorder;
  sml::sm<table715, sml::dont_instantiate_statemachine_class> sm{table, recorder};
  sm.process_event(e1_715{});
  expect(sm.is(sml::X));
  expect(1 == recorder.n);
};
