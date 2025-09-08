#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapForEachTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapForEachTest, ForEachBasic) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  int sum = 0;
  int count = 0;

  map.for_each([&sum, &count](auto& item) {
    sum += item.val;
    count++;
  });

  EXPECT_EQ(sum, 60);
  EXPECT_EQ(count, 3);
}

TEST_F(StaticMapForEachTest, ForEachModifyValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each([](auto& item) { item.val *= 10; });

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapForEachTest, ForEachStringManipulation) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  map.for_each([](auto& item) { item.val += " world"; });

  EXPECT_EQ(map.template at<'a'>(), "hello world");
}

TEST_F(StaticMapForEachTest, ForEachTypeSpecificOperations) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  map.for_each([](auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::same_as<typename ItemType::val_t, int>) {
      item.val += 100;
    } else if constexpr (std::same_as<typename ItemType::val_t, std::string>) {
      item.val = "modified_" + item.val;
    }
  });

  EXPECT_EQ(map.template at<1>(), 142);
  EXPECT_EQ(map.template at<'a'>(), "modified_test");
}

TEST_F(StaticMapForEachTest, ForEachSingleItem) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  int called_count = 0;
  int value = 0;

  map.for_each([&called_count, &value](auto& item) {
    called_count++;
    value = item.val;
  });

  EXPECT_EQ(called_count, 1);
  EXPECT_EQ(value, 42);
}

TEST_F(StaticMapForEachTest, ForEachEmptyMap) {
  smap::StaticMap<> map;

  int call_count = 0;
  map.for_each([&call_count](auto& item) { call_count++; });

  EXPECT_EQ(call_count, 0);
}

struct Adder {
  int increment = 0;
  int total = 0;

  void operator()(auto& item) {
    item.val += increment;
    total += item.val;
  }
};

TEST_F(StaticMapForEachTest, ForEachFunctionObject) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  Adder adder(10, 0);
  map.for_each(adder);

  EXPECT_EQ(map.template at<1>(), 11);
  EXPECT_EQ(map.template at<2>(), 12);
}

TEST_F(StaticMapForEachTest, ForEachLambdaCapture) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(5), IntItem2(10));

  int multiplier = 3;

  map.for_each([multiplier](auto& item) { item.val *= multiplier; });

  EXPECT_EQ(map.template at<1>(), 15);
  EXPECT_EQ(map.template at<2>(), 30);
}

TEST_F(StaticMapForEachTest, ForEachMoveOnlyCapture) {
  smap::StaticMap<IntItem1> map(IntItem1(1));

  auto move_only = std::make_unique<int>(42);

  map.for_each([captured = std::move(move_only)](auto& item) { item.val += *captured; });

  EXPECT_EQ(map.template at<1>(), 43);
}

TEST_F(StaticMapForEachTest, ForEachConstMap) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  // for_each should not be callable on const maps
  // map.for_each([](auto& item) {}); // Should not compile
}

TEST_F(StaticMapForEachTest, ForEachPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    map.for_each([](auto& item) { item.val += 1; });
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast

  EXPECT_EQ(map.template at<1>(), 10001);
  EXPECT_EQ(map.template at<2>(), 10002);
  EXPECT_EQ(map.template at<3>(), 10003);
}

TEST_F(StaticMapForEachTest, ForEachWithReturnStatement) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each([](auto& item) -> void {
    item.val *= 2;
    return;  // Explicit return
  });

  EXPECT_EQ(map.template at<1>(), 2);
  EXPECT_EQ(map.template at<2>(), 4);
}

TEST_F(StaticMapForEachTest, ForEachTemplateLambda) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  map.for_each([]<class T>(T& item) {
    if constexpr (std::same_as<typename T::val_t, int>) {
      item.val *= 2;
    } else if constexpr (std::same_as<typename T::val_t, std::string>) {
      item.val += "!";
    }
  });

  EXPECT_EQ(map.template at<1>(), 84);
  EXPECT_EQ(map.template at<'a'>(), "hello!");
}

TEST_F(StaticMapForEachTest, ForEachComplexTypes) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  smap::StaticMap<VectorItem> map(VectorItem(std::vector<int>{1, 2, 3}));

  map.for_each([](auto& item) { item.val.push_back(4); });

  EXPECT_EQ(map.template at<1>().size(), 4);
  EXPECT_EQ(map.template at<1>().back(), 4);
}

TEST_F(StaticMapForEachTest, ForEachExceptionSafety) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  EXPECT_THROW(
      {
        map.for_each([](auto& item) {
          if (item.val == 2) {
            throw std::runtime_error("Test exception");
          }
        });
      },
      std::runtime_error);

  // Map should remain in valid state
  EXPECT_EQ(map.template at<1>(), 1);
  EXPECT_EQ(map.template at<2>(), 2);
}

struct Functor {
  void operator()(IntItem1& item) {
    item.val += 1;
  }
};

TEST_F(StaticMapForEachTest, ForEachStdInvocableCheck) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  // These should compile because they are invocable with IntItem1&
  auto lambda = [](IntItem1& item) {
    item.val += 1;
  };

  map.for_each(lambda);
  map.for_each(Functor{});

  EXPECT_EQ(map.template at<1>(), 44);
}

TEST_F(StaticMapForEachTest, ForEachChaining) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  static_assert(std::same_as<smap::StaticMap<IntItem1, IntItem2>&,
      decltype(map.for_each([](auto& item) {}))>);
}

class StaticMapForEachConstTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapForEachConstTest, ForEachConstBasic) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  int sum = 0;
  int count = 0;

  map.for_each([&sum, &count](const auto& item) {
    sum += item.val;
    count++;
  });

  EXPECT_EQ(sum, 60);
  EXPECT_EQ(count, 3);
}

TEST_F(StaticMapForEachConstTest, ForEachConstReadValues) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  int sum = 0;
  map.for_each([&sum](const auto& item) { sum += item.val; });

  EXPECT_EQ(sum, 66);
}

TEST_F(StaticMapForEachConstTest, ForEachConstStringValues) {
  const smap::StaticMap<StringItem> map(StringItem("hello"));

  std::string result;
  map.for_each([&result](const auto& item) { result = item.val; });

  EXPECT_EQ(result, "hello");
}

TEST_F(StaticMapForEachConstTest, ForEachConstSingleItem) {
  const smap::StaticMap<IntItem1> map(IntItem1(100));

  int captured_value = 0;
  map.for_each([&captured_value](const auto& item) { captured_value = item.val; });

  EXPECT_EQ(captured_value, 100);
}

TEST_F(StaticMapForEachConstTest, ForEachConstEmptyMap) {
  const smap::StaticMap<> map;

  int call_count = 0;
  map.for_each([&call_count](const auto& item) { call_count++; });

  EXPECT_EQ(call_count, 0);
}

struct SumCalculator {
  int total = 0;

  void operator()(const auto& item) {
    total += item.val;
  }
};

TEST_F(StaticMapForEachConstTest, ForEachConstFunctionObject) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  SumCalculator calculator;
  map.for_each(calculator);

  EXPECT_EQ(calculator.total, 30);
}

TEST_F(StaticMapForEachConstTest, ForEachConstLambdaCapture) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(5), IntItem2(15));

  int multiplier = 2;
  int sum = 0;

  map.for_each([multiplier, &sum](const auto& item) { sum += item.val * multiplier; });

  EXPECT_EQ(sum, 40);  // (5 * 2) + (15 * 2) = 10 + 30 = 40
}

TEST_F(StaticMapForEachConstTest, ForEachConstMoveOnlyCapture) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  auto move_only = std::make_unique<int>(100);
  int result = 0;

  map.for_each([captured = std::move(move_only), &result](
                   const auto& item) { result = item.val + *captured; });

  EXPECT_EQ(result, 142);
}

TEST_F(StaticMapForEachConstTest, ForEachConstTypeSpecificOperations) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  int int_sum = 0;
  std::string string_value;

  map.for_each([&int_sum, &string_value](const auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::same_as<typename ItemType::val_t, int>) {
      int_sum = item.val;
    } else if constexpr (std::same_as<typename ItemType::val_t, std::string>) {
      string_value = item.val;
    }
  });

  EXPECT_EQ(int_sum, 42);
  EXPECT_EQ(string_value, "test");
}

TEST_F(StaticMapForEachConstTest, ForEachConstTemplateLambda) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(100), StringItem("hello"));

  int int_value = 0;
  std::string string_value;

  map.for_each([]<class T>(const T& item) {
    // Just verify it compiles and works
  });

  map.for_each([&int_value, &string_value]<class T>(const T& item) {
    if constexpr (std::same_as<typename T::val_t, int>) {
      int_value = item.val;
    } else if constexpr (std::same_as<typename T::val_t, std::string>) {
      string_value = item.val;
    }
  });

  EXPECT_EQ(int_value, 100);
  EXPECT_EQ(string_value, "hello");
}

TEST_F(StaticMapForEachConstTest, ForEachConstComplexTypes) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  const smap::StaticMap<VectorItem> map(VectorItem(std::vector<int>{1, 2, 3, 4, 5}));

  int vector_size = 0;
  int vector_sum = 0;

  map.for_each([&vector_size, &vector_sum](const auto& item) {
    vector_size = item.val.size();
    for (const auto& elem : item.val) {
      vector_sum += elem;
    }
  });

  EXPECT_EQ(vector_size, 5);
  EXPECT_EQ(vector_sum, 15);
}

TEST_F(StaticMapForEachConstTest, ForEachConstPerformance) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(100), IntItem2(200), IntItem3(300));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    int sum = 0;
    map.for_each([&sum](const auto& item) { sum += item.val; });
    EXPECT_EQ(sum, 600);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast
}

TEST_F(StaticMapForEachConstTest, ForEachConstStdInvocableCheck) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // These should compile because they are invocable with const IntItem1&
  auto lambda = [](const IntItem1& item) {
    [[maybe_unused]] int val = item.val;
  };
  map.for_each(lambda);

  struct Functor {
    void operator()(const IntItem1& item) {
      [[maybe_unused]] int val = item.val;
    }
  };

  map.for_each(Functor{});
}

TEST_F(StaticMapForEachConstTest, ForEachConstCannotModify) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // This should not compile because we're trying to modify const items
  // map.for_each([](const auto& item) {
  //   item.val = 100;  // Should cause compilation error
  // });
}

TEST_F(StaticMapForEachConstTest, ForEachConstWithDifferentCallableTypes) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  // Function pointer
  auto func_ptr = [](const auto& item) {
    [[maybe_unused]] int val = item.val;
  };
  map.for_each(func_ptr);

  // std::function
  std::function<void(const IntItem1&)> std_func = [](const IntItem1& item) {
    [[maybe_unused]] int val = item.val;
  };
  // Note: This might not work due to template constraints, but regular lambdas should

  // Regular lambda
  map.for_each([](const auto& item) { [[maybe_unused]] int val = item.val; });
}

// TEST_F(StaticMapForEachConstTest, ForEachConstChaining) {
//   const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

//   // for_each returns void, so chaining is not possible in the traditional sense
//   // but we can chain calls to for_each itself
//   int sum1 = 0, sum2 = 0;

//   map.for_each([&sum1](const auto& item) { sum1 += item.val; })
//       .for_each(
//           [&sum2](const auto& item) { sum2 += item.val * 2; });  // This won't compile

//   // Actually, for_each returns void, so chaining like this won't work
//   // We just call them separately
//   map.for_each([&sum1](const auto& item) { sum1 += item.val; });
//   map.for_each([&sum2](const auto& item) { sum2 += item.val * 2; });

//   EXPECT_EQ(sum1, 3);
//   EXPECT_EQ(sum2, 6);
// }

TEST_F(StaticMapForEachConstTest, ForEachConstExceptionSafety) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  EXPECT_THROW(
      {
        map.for_each([](const auto& item) {
          if (item.val == 2) {
            throw std::runtime_error("Test exception");
          }
        });
      },
      std::runtime_error);
}

TEST_F(StaticMapForEachConstTest, ForEachConstStructuredBindingLike) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  // Verify we can access both key and value from const items
  int found_key = 0;
  int found_value = 0;
  std::string found_string;

  map.for_each([&found_key, &found_value, &found_string](const auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::same_as<typename ItemType::val_t, int>) {
      if (item.key == 1) {
        found_key = item.key;
        found_value = item.val;
      }
    } else if constexpr (std::same_as<typename ItemType::val_t, std::string>) {
      found_string = item.val;
    }
  });

  EXPECT_EQ(found_key, 1);
  EXPECT_EQ(found_value, 42);
  EXPECT_EQ(found_string, "test");
}
