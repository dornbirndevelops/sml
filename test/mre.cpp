#include <boost/sml.hpp>

#include <tuple>
#include <utility>

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
    auto operator()() const
    {
        using namespace boost::sml;
        return make_transition_table(*state<Idle> + event<Event> / Action{} = state<Done>);
    }
};

template <int... Ns>
void build(std::integer_sequence<int, Ns...>)
{
    std::tuple<Dep<Ns>...> deps;
    boost::sml::sm<Machine> sm{std::get<Ns>(deps)...};
    sm.process_event(Event{});
}

int main() { build(std::make_integer_sequence<int, NDEPS>{}); }
