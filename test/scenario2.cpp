#include <boost/sml.hpp>
#include <cstdio>
#include <memory>

namespace {

struct myevent {};

struct mydependency {
  int value{};
};

struct mybetterdependency : mydependency {
  mybetterdependency() { value = 1; }
};

struct myguard { bool operator()(const mydependency& d) {
  const void* p = std::addressof(d);
  std::printf("address of const mydependency& d: %p\n", p);
  return true;
} };

struct myaction { void operator()(mydependency& d) {
  void* p = std::addressof(d);
  std::printf("address of mydependency& d: %p\n", p);
  d.value++;
} };

struct root { auto operator()() noexcept {
  using namespace boost::sml;
  const auto g = myguard{};
  const auto a = myaction{};
  return make_transition_table(
    *"idle"_s + event<myevent> [g] / a = X
  );
} };

} // namespace

int main() {
  mybetterdependency d{};
  boost::sml::sm<root> sm{d};
  sm.process_event(myevent{});
}

// expecting same address (same instance) as output, e.g.
// address of const mydependency& d: 0x7ffd308f616c
// address of mydependency& d: 0x7ffd308f616c