#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapForEachIndexedTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapForEachIndexedTest, BasicForEachIndexed) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
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

TEST_F(StaticMapForEachIndexedTest, ModifyValuesByIndex) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each_indexed(
      [](std::size_t index, auto& item) { item.val *= (index + 1) * 10; });

  EXPECT_EQ(map.template at<1>(), 10);  // index 0: 1 * (0+1) * 10 = 10
  EXPECT_EQ(map.template at<2>(), 40);  // index 1: 2 * (1+1) * 10 = 40
}

TEST_F(StaticMapForEachIndexedTest, IndexBasedOperations) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(5), IntItem2(10), IntItem3(15));

  std::vector<std::size_t> indices;
  int sum = 0;

  map.for_each_indexed([&indices, &sum](std::size_t index, const auto& item) {
    indices.push_back(index);
    sum += item.val;
  });

  EXPECT_EQ(indices, std::vector<std::size_t>({0, 1, 2}));
  EXPECT_EQ(sum, 30);
}

TEST_F(StaticMapForEachIndexedTest, SingleItemMap) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  std::size_t captured_index = 999;
  int captured_value = 0;

  map.for_each_indexed(
      [&captured_index, &captured_value](std::size_t index, const auto& item) {
        captured_index = index;
        captured_value = item.val;
      });

  EXPECT_EQ(captured_index, 0);
  EXPECT_EQ(captured_value, 42);
}

TEST_F(StaticMapForEachIndexedTest, EmptyMap) {
  smap::StaticMap<> map;

  int call_count = 0;
  map.for_each_indexed(
      [&call_count](std::size_t index, const auto& item) { call_count++; });

  EXPECT_EQ(call_count, 0);
}

TEST_F(StaticMapForEachIndexedTest, StringItemsWithIndex) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  std::string result;
  std::size_t result_index = 0;

  map.for_each_indexed([&result, &result_index](std::size_t index, const auto& item) {
    result_index = index;
    result = std::to_string(index) + ":" + item.val;
  });

  EXPECT_EQ(result_index, 0);
  EXPECT_EQ(result, "0:hello");
}

TEST_F(StaticMapForEachIndexedTest, MixedTypesWithIndex) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  std::vector<std::pair<std::size_t, std::string>> results;

  map.for_each_indexed([&results](std::size_t index, const auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::is_same_v<typename ItemType::val_t, int>) {
      results.emplace_back(index, "int:" + std::to_string(item.val));
    } else if constexpr (std::is_same_v<typename ItemType::val_t, std::string>) {
      results.emplace_back(index, "string:" + item.val);
    }
  });

  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].first, 0);
  EXPECT_EQ(results[0].second, "int:42");
  EXPECT_EQ(results[1].first, 1);
  EXPECT_EQ(results[1].second, "string:test");
}

TEST_F(StaticMapForEachIndexedTest, IndexValidation) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
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

struct IndexChecker {
  std::vector<std::size_t> indices;
  int sum = 0;

  void operator()(std::size_t index, const auto& item) {
    indices.push_back(index);
    sum += item.val;
  }
};

TEST_F(StaticMapForEachIndexedTest, FunctionObjectWithIndex) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  IndexChecker checker;
  map.for_each_indexed(checker);

  EXPECT_EQ(checker.indices, std::vector<std::size_t>({0, 1}));
  EXPECT_EQ(checker.sum, 30);
}

TEST_F(StaticMapForEachIndexedTest, LambdaCaptureWithIndex) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(5), IntItem2(15));

  std::vector<int> multipliers = {2, 3};
  int total = 0;

  map.for_each_indexed([&multipliers, &total](std::size_t index, auto& item) {
    item.val *= multipliers[index];
    total += item.val;
  });

  EXPECT_EQ(map.template at<1>(), 10);  // 5 * 2
  EXPECT_EQ(map.template at<2>(), 45);  // 15 * 3
  EXPECT_EQ(total, 55);
}

TEST_F(StaticMapForEachIndexedTest, ConstMapForEachIndexed) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(100), IntItem2(200));

  std::vector<std::pair<std::size_t, int>> results;

  map.for_each_indexed([&results](std::size_t index, const auto& item) {
    results.emplace_back(index, item.val);
  });

  EXPECT_EQ(results.size(), 2);
  EXPECT_EQ(results[0].first, 0);
  EXPECT_EQ(results[0].second, 100);
  EXPECT_EQ(results[1].first, 1);
  EXPECT_EQ(results[1].second, 200);
}

TEST_F(StaticMapForEachIndexedTest, IndexBasedConditionalOperations) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  map.for_each_indexed([](std::size_t index, auto& item) {
    if (index % 2 == 0) {
      item.val *= 10;
    } else {
      item.val *= 100;
    }
  });

  EXPECT_EQ(map.template at<1>(), 10);  // index 0: even
  EXPECT_EQ(map.template at<2>(), 200);  // index 1: odd
  EXPECT_EQ(map.template at<3>(), 30);  // index 2: even
}

TEST_F(StaticMapForEachIndexedTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 1000; ++i) {
    map.for_each_indexed(
        [](std::size_t index, auto& item) { item.val += static_cast<int>(index); });
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);

  EXPECT_EQ(map.template at<1>(), 1);
  EXPECT_EQ(map.template at<2>(), 1002);
  EXPECT_EQ(map.template at<3>(), 2003);
}
