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

struct yourbetterdependency : mydependency {
  yourbetterdependency() { value = 2; }
};

struct myguard { bool operator()(const mydependency& d) {
  const void* p = std::addressof(d);
  std::printf("address of const mydependency& d: %p\n", p);
  return true;
} };

struct root { auto operator()() noexcept {
  using namespace boost::sml;
  const auto g = myguard{};
  return make_transition_table(
    *"idle"_s + event<myevent> [g] = X
  );
} };

} // namespace

int main() {
  mybetterdependency d{};
  const void* p = static_cast<mydependency*>(std::addressof(d));
  std::printf("address of mydependency d: %p\n", p);

  const yourbetterdependency d2{};
  const void* p2 = static_cast<const mydependency*>(std::addressof(d2));
  std::printf("address of const mydependency d2: %p\n", p2);

  boost::sml::sm<root> sm{d, d2};
  sm.process_event(myevent{});
}

// expecting compilation failure due to ambiguous candidates (like scenario4).
// but currently produces
// address of mydependency d: 0x7fff04d7d7d8
// address of const mydependency d2: 0x7fff04d7d7dc
// address of const mydependency& d: 0x7fff04d7d7dc