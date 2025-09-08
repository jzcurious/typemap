#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapForEachIndexedConstTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstBasic) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  std::vector<std::pair<std::size_t, int>> results;

  map.for_each_indexed([&results](std::size_t index, const auto& item) {
    results.emplace_back(index, item.val);
  });

  EXPECT_EQ(results.size(), 3);
  EXPECT_EQ(results[0].first, 0);
  EXPECT_EQ(results[0].second, 10);
  EXPECT_EQ(results[1].first, 1);
  EXPECT_EQ(results[1].second, 20);
  EXPECT_EQ(results[2].first, 2);
  EXPECT_EQ(results[2].second, 30);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstReadValues) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  int sum = 0;
  map.for_each_indexed([&sum](std::size_t index, const auto& item) {
    sum += item.val;
    EXPECT_EQ(index, item.val == 42 ? 0 : 1);
  });

  EXPECT_EQ(sum, 66);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstSingleItem) {
  const smap::StaticMap<IntItem1> map(IntItem1(100));

  std::size_t captured_index = 999;
  int captured_value = 0;

  map.for_each_indexed(
      [&captured_index, &captured_value](std::size_t index, const auto& item) {
        captured_index = index;
        captured_value = item.val;
      });

  EXPECT_EQ(captured_index, 0);
  EXPECT_EQ(captured_value, 100);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstEmptyMap) {
  const smap::StaticMap<> map;

  int call_count = 0;
  map.for_each_indexed(
      [&call_count](std::size_t index, const auto& item) { call_count++; });

  EXPECT_EQ(call_count, 0);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstStringItems) {
  const smap::StaticMap<StringItem> map(StringItem("hello"));

  std::string result;
  std::size_t result_index = 0;

  map.for_each_indexed([&result, &result_index](std::size_t index, const auto& item) {
    result_index = index;
    result = std::to_string(index) + ":" + item.val;
  });

  EXPECT_EQ(result_index, 0);
  EXPECT_EQ(result, "0:hello");
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstMixedTypes) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  std::vector<std::pair<std::size_t, std::string>> results;

  map.for_each_indexed([&results](std::size_t index, const auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::same_as<typename ItemType::val_t, int>) {
      results.emplace_back(index, "int:" + std::to_string(item.val));
    } else if constexpr (std::same_as<typename ItemType::val_t, std::string>) {
      results.emplace_back(index, "string:" + item.val);
    }
  });

  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].first, 0);
  EXPECT_EQ(results[0].second, "int:42");
  EXPECT_EQ(results[1].first, 1);
  EXPECT_EQ(results[1].second, "string:test");
}

struct IndexProcessor {
  std::vector<std::size_t> indices;
  int sum = 0;

  void operator()(std::size_t index, const auto& item) {
    indices.push_back(index);
    sum += item.val;
  }
};

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstFunctionObject) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  IndexProcessor processor;
  map.for_each_indexed(processor);

  EXPECT_EQ(processor.indices, std::vector<std::size_t>({0, 1}));
  EXPECT_EQ(processor.sum, 30);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstLambdaCapture) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(5), IntItem2(15));

  std::vector<int> multipliers = {2, 3};
  int total = 0;

  map.for_each_indexed([&multipliers, &total](std::size_t index, const auto& item) {
    total += item.val * multipliers[index];
  });

  EXPECT_EQ(total, 55);  // (5 * 2) + (15 * 3) = 10 + 45 = 55
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstMoveOnlyCapture) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  auto move_only = std::make_unique<int>(100);
  int result = 0;

  map.for_each_indexed(
      [captured = std::move(move_only), &result](std::size_t index, const auto& item) {
        result = item.val + *captured + static_cast<int>(index);
      });

  EXPECT_EQ(result, 142);  // 42 + 100 + 0
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstReturnReference) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto& returned_ref = map.for_each_indexed(
      [](std::size_t index, const auto& item) { [[maybe_unused]] int val = item.val; });

  EXPECT_EQ(&returned_ref, &map);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstChaining) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto& result = map.for_each_indexed([](std::size_t index, const auto& item) {
                      [[maybe_unused]] int val = item.val;
                    })
                     .for_each_indexed([](std::size_t index, const auto& item) {
                       [[maybe_unused]] int val = item.val;
                     });

  EXPECT_EQ(&result, &map);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstPerformance) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(100), IntItem2(200), IntItem3(300));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    int sum = 0;
    map.for_each_indexed(
        [&sum](std::size_t index, const auto& item) { sum += item.val; });
    EXPECT_EQ(sum, 600);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast
}

struct Functor {
  void operator()(std::size_t index, const IntItem1& item) {
    [[maybe_unused]] int val = item.val;
  }
};

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstStdInvocableCheck) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // These should compile because they are invocable with (std::size_t, const IntItem1&)
  auto lambda = [](std::size_t index, const IntItem1& item) {
    [[maybe_unused]] int val = item.val;
  };
  map.for_each_indexed(lambda);

  map.for_each_indexed(Functor{});
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstCannotModify) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // This should not compile because we're trying to modify const items
  // map.for_each_indexed([](std::size_t index, const auto& item) {
  //     item.val = 100; // Should cause compilation error
  // });
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstIndexValidation) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  std::vector<std::size_t> expected_indices = {0, 1, 2};
  std::vector<std::size_t> actual_indices;

  map.for_each_indexed([&actual_indices](std::size_t index, const auto& item) {
    actual_indices.push_back(index);
    EXPECT_LT(index, 3);
    EXPECT_GE(index, 0);
  });

  EXPECT_EQ(actual_indices, expected_indices);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstTemplateLambda) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  map.for_each_indexed([]<class T>(std::size_t index, const T& item) {
    // Just verify it compiles and works
    [[maybe_unused]] auto val = item.val;
  });
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstExceptionSafety) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  EXPECT_THROW(
      {
        map.for_each_indexed([](std::size_t index, const auto& item) {
          if (index == 1) {
            throw std::runtime_error("Test exception");
          }
        });
      },
      std::runtime_error);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstComplexTypes) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  const smap::StaticMap<VectorItem> map(VectorItem(std::vector<int>{1, 2, 3, 4, 5}));

  int vector_size = 0;
  int vector_sum = 0;

  map.for_each_indexed([&vector_size, &vector_sum](std::size_t index, const auto& item) {
    vector_size = item.val.size();
    for (const auto& elem : item.val) {
      vector_sum += elem;
    }
  });

  EXPECT_EQ(vector_size, 5);
  EXPECT_EQ(vector_sum, 15);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstReturnValue) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // for_each_indexed returns const reference to *this
  static_assert(
      std::same_as<decltype(map.for_each_indexed([](std::size_t, const auto&) {})),
          const smap::StaticMap<IntItem1>&>);

  auto& result = map.for_each_indexed(
      [](std::size_t, const auto& item) { [[maybe_unused]] int val = item.val; });

  EXPECT_EQ(&result, &map);
}

TEST_F(StaticMapForEachIndexedConstTest, ForEachIndexedConstStructuredBindingLike) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  // Verify we can access both key and value from const items with indices
  int found_key = 0;
  int found_value = 0;
  std::string found_string;
  std::size_t found_index = 999;

  map.for_each_indexed([&found_key, &found_value, &found_string, &found_index](
                           std::size_t index, const auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::same_as<typename ItemType::val_t, int>) {
      if (item.key == 1) {
        found_key = item.key;
        found_value = item.val;
        found_index = index;
      }
    } else if constexpr (std::same_as<typename ItemType::val_t, std::string>) {
      found_string = item.val;
    }
  });

  EXPECT_EQ(found_key, 1);
  EXPECT_EQ(found_value, 42);
  EXPECT_EQ(found_string, "test");
  EXPECT_EQ(found_index, 0);
}
