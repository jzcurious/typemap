#include <algorithm>
#include <array>
#include <iostream>
#include <staticmap/staticmap.hpp>
#include <string>
#include <vector>

template <size_t N>
struct FixedString {
  static constexpr std::size_t size = N;
  std::array<char, N> data;

  constexpr FixedString(const char (&str)[N]) /* NOLINT */ {
    std::copy_n(static_cast<const char*>(str), N, data.begin());
  }

  template <class FixedStringT>
  constexpr bool operator==(const FixedStringT& other) const {
    if constexpr (size == FixedStringT::size)
      return data == other.data;
    else
      return false;
  }

  friend std::ostream& operator<<(std::ostream& os, const FixedString& fs) {
    return os << fs.data.data();
  }
};

template <FixedString fixed_string>
constexpr auto operator""_fs() {
  return fixed_string;
}

// Example 1: Basic usage - application configuration
void basic_usage_example() {
  std::cout << "=== Basic Usage ===\n";

  // Create a static map with different data types
  auto config = smap::make_static_map(smap::Item<"debug"_fs, bool>(true),
      smap::Item<"max_connections"_fs, int>(100),
      smap::Item<"app_name"_fs, std::string>("MyApp"),
      smap::Item<"version"_fs, double>(1.5));

  std::cout << "App: " << config.template at<"app_name"_fs>() << " v"
            << config.template at<"version"_fs>() << std::endl;
  std::cout << "Debug mode: " << (config.template at<"debug"_fs>() ? "ON" : "OFF")
            << std::endl;
}

// Example 2: Compiler as feature configuration
void feature_flags_example() {
  std::cout << "\n=== Feature Flags ===\n";

  auto features = smap::make_static_map(smap::Item<"new_ui"_fs, bool>(true),
      smap::Item<"experimental_api"_fs, bool>(false),
      smap::Item<"logging_level"_fs, int>(2));

  // Feature checking
  if constexpr (features.template contains<"new_ui"_fs>()) {
    std::cout << "New UI is "
              << (features.template at<"new_ui"_fs>() ? "enabled" : "disabled")
              << std::endl;
  }

  // Batch checking
  if constexpr (features.template contains_all<"new_ui"_fs, "logging_level"_fs>()) {
    std::cout << "Core features are available\n";
  }
}

// Example 3: HTTP status codes
void http_status_example() {
  std::cout << "\n=== HTTP Status Codes ===\n";

  auto http_messages = smap::make_static_map(smap::Item<200, std::string>("OK"),
      smap::Item<404, std::string>("Not Found"),
      smap::Item<500, std::string>("Internal Server Error"),
      smap::Item<403, std::string>("Forbidden"));

  // Safe access with default values
  std::cout << "Status 200: " << http_messages.template get<200>() << std::endl;
  std::cout << "Status 999: " << http_messages.template get<999>("Unknown") << std::endl;

  // Existence checking
  std::cout << "Has 404: " << (http_messages.template contains<404>() ? "Yes" : "No")
            << std::endl;
}

// Example 4: Mathematical constants
void math_constants_example() {
  std::cout << "\n=== Mathematical Constants ===\n";

  auto constants = smap::make_static_map(smap::Item<"pi"_fs, double>(std::numbers::pi),
      smap::Item<"e"_fs, double>(std::numbers::e),
      smap::Item<"phi"_fs, double>(std::numbers::phi));

  // Iteration over all elements
  constants.for_each([](const auto& item) {
    using ValueType = typename std::decay_t<decltype(item)>::val_t;
    if constexpr (std::is_same_v<ValueType, double>) {
      std::cout << item.key << " = " << item.val << std::endl;
    }
  });
}

// Example 5: Finite state machine
void state_machine_example() {
  std::cout << "\n=== State Machine Configuration ===\n";

  enum class State { IDLE, RUNNING, STOPPED };
  enum class Event { START, STOP, RESET };

  auto transitions = smap::make_static_map(
      smap::Item<std::make_pair(State::IDLE, Event::START), State>(State::RUNNING),
      smap::Item<std::make_pair(State::RUNNING, Event::STOP), State>(State::STOPPED),
      smap::Item<std::make_pair(State::STOPPED, Event::RESET), State>(State::IDLE));

  auto current_state = State::IDLE;
  std::cout << "Initial state: " << static_cast<int>(current_state) << std::endl;

  // Simulate transition
  constexpr auto key = std::make_pair(State::IDLE, Event::START);
  if constexpr (transitions.template contains<key>()) {
    current_state = transitions.template at<key>();
    std::cout << "New state: " << static_cast<int>(current_state) << std::endl;
  }
}

// Example 6: User profiles
struct UserProfile {
  std::string name;
  int age;
  std::vector<std::string> permissions;
};

template <std::size_t id>
using user_record_t = smap::Item<id, UserProfile>;

void user_profiles_example() {
  std::cout << "\n=== User Profiles ===\n";

  // clang-format off
  auto profiles = smap::make_static_map(
      user_record_t<1>({
        .name = "Alice",
        .age = 25,
        .permissions = {"read", "write"}
      }),

      user_record_t<2>({
        .name = "Bob",
        .age = 30,
        .permissions = {"read"}
      }),

      user_record_t<3>({
        .name = "Charlie",
        .age = 35,
        .permissions = {"admin"}
      })
  );
  // clang-format on

  // Iteration with indices
  profiles.for_each_indexed([](std::size_t index, const auto& item) {
    std::cout << "User " << index << ": " << item.val.name << " (" << item.val.age
              << " years)\n";
  });

  // Getting profile
  const auto& alice = profiles.template at<1>();
  std::cout << alice.name << " has permissions: ";
  for (const auto& perm : alice.permissions) {
    std::cout << perm << " ";
  }
  std::cout << std::endl;
}

int main() {
  std::cout << "StaticMap Library Examples\n";
  std::cout << "=========================\n";

  basic_usage_example();
  feature_flags_example();
  http_status_example();
  math_constants_example();
  state_machine_example();
  user_profiles_example();

  std::cout << "\nAll examples completed successfully!\n";
  return 0;
}
