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

struct yourdependency {
  int value{};
};

struct yourbetterdependency : yourdependency {
  yourbetterdependency() { value = 1; }
};

struct myguard { bool operator()(const mydependency& d, const yourdependency& d2) {
  const void* p = std::addressof(d);
  std::printf("address of const mydependency& d: %p\n", p);

  const void* p2 = std::addressof(d2);
  std::printf("address of const yourdependency& d2: %p\n", p2);
  return true;
} };

struct myaction { void operator()(mydependency& d, yourdependency& d2) {
  void* p = std::addressof(d);
  std::printf("address of mydependency& d: %p\n", p);
  d.value++;

  void* p2 = std::addressof(d2);
  std::printf("address of yourdependency& d2: %p\n", p2);
  d2.value++;
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
  yourbetterdependency d2{};
  boost::sml::sm<root> sm{d, d2};
  sm.process_event(myevent{});
}

// expecting same address (same instance) as output, e.g.
// address of const mydependency& d: 0x7ffd308f616c
// address of const yourdependency& d2: ...
// address of mydependency& d: 0x7ffd308f616c
// address of yourdependency& d2: ...