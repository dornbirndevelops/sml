#include <boost/sml.hpp>
#include <cstdio>
#include <memory>

namespace {

struct myevent {};

struct mydependency {
  int value{};
};

struct myguard { bool operator()(const mydependency& d) {
  const void* p = std::addressof(d);
  std::printf("address of const mydependency& d: %p\n", p);
  return true;
} };

struct myaction { void operator()(mydependency& d) {
  void* p = std::addressof(d);
  std::printf("address of mydependency& d: %p\n", p);
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
  mydependency d{};
  const mydependency d2{};
  boost::sml::sm<root> sm{d, d2};
  sm.process_event(myevent{});
}

// expecting compilation failure due to ambiguous candidates.
