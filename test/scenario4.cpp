#include <boost/sml.hpp>
#include <cstdio>
#include <memory>

namespace {

struct myevent {};

struct mydependency {
  int value{};
};

struct myaction { void operator()(mydependency& d, const mydependency& d2) {
  void* p = std::addressof(d);
  std::printf("address of mydependency& d: %p\n", p);
  d.value++;

  const void* p2 = std::addressof(d2);
  std::printf("address of yourdependency& d2: %p\n", p2);
} };

struct root { auto operator()() noexcept {
  using namespace boost::sml;
  const auto a = myaction{};
  return make_transition_table(
    *"idle"_s + event<myevent> / a = X
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