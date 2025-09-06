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

TEST_F(StaticMapForEachTest, BasicForEach) {
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

TEST_F(StaticMapForEachTest, ModifyValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each([](auto& item) { item.val *= 10; });

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapForEachTest, StringManipulation) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  map.for_each([](auto& item) { item.val += " world"; });

  EXPECT_EQ(map.template at<'a'>(), "hello world");
}

TEST_F(StaticMapForEachTest, TypeSpecificOperations) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  map.for_each([](auto& item) {
    using ItemType = std::decay_t<decltype(item)>;
    if constexpr (std::is_same_v<typename ItemType::val_t, int>) {
      item.val += 100;
    } else if constexpr (std::is_same_v<typename ItemType::val_t, std::string>) {
      item.val = "modified_" + item.val;
    }
  });

  EXPECT_EQ(map.template at<1>(), 142);
  EXPECT_EQ(map.template at<'a'>(), "modified_test");
}

TEST_F(StaticMapForEachTest, EmptyLambda) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each([](auto& item) {
    // Do nothing
  });

  EXPECT_EQ(map.template at<1>(), 1);
  EXPECT_EQ(map.template at<2>(), 2);
}

TEST_F(StaticMapForEachTest, CaptureByValue) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(5), IntItem2(10));

  int multiplier = 3;

  map.for_each([multiplier](auto& item) { item.val *= multiplier; });

  EXPECT_EQ(map.template at<1>(), 15);
  EXPECT_EQ(map.template at<2>(), 30);
}

TEST_F(StaticMapForEachTest, MoveOnlyCapture) {
  smap::StaticMap<IntItem1> map(IntItem1(1));

  auto move_only = std::make_unique<int>(42);

  map.for_each([captured = std::move(move_only)](auto& item) { item.val += *captured; });

  EXPECT_EQ(map.template at<1>(), 43);
}

TEST_F(StaticMapForEachTest, ExceptionSafety) {
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

TEST_F(StaticMapForEachTest, SingleItemMap) {
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

struct Adder {
  int increment;
  int total;

  void operator()(auto& item) {
    item.val += increment;
    total += item.val;
  }
};

TEST_F(StaticMapForEachTest, FunctionObject) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  Adder adder(10, 0);
  map.for_each(adder);

  EXPECT_EQ(map.template at<1>(), 11);
  EXPECT_EQ(map.template at<2>(), 12);
}

TEST_F(StaticMapForEachTest, LambdaWithReturn) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.for_each([](auto& item) -> void {
    item.val *= 2;
    return;  // Explicit return
  });

  EXPECT_EQ(map.template at<1>(), 2);
  EXPECT_EQ(map.template at<2>(), 4);
}

TEST_F(StaticMapForEachTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 1000; ++i) {
    map.for_each([](auto& item) { item.val += 1; });
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be fast

  EXPECT_EQ(map.template at<1>(), 1001);
  EXPECT_EQ(map.template at<2>(), 1002);
  EXPECT_EQ(map.template at<3>(), 1003);
}

TEST_F(StaticMapForEachTest, NestedForEach) {
  smap::StaticMap<IntItem1, IntItem2> outer_map(IntItem1(1), IntItem2(2));

  int outer_calls = 0;
  int inner_calls = 0;

  outer_map.for_each([&outer_calls, &inner_calls, &outer_map](auto& outer_item) {
    outer_calls++;
    outer_map.for_each([&inner_calls](auto& inner_item) {
      inner_calls++;
      inner_item.val += 10;
    });
  });

  EXPECT_EQ(outer_calls, 2);
  EXPECT_EQ(inner_calls, 4);  // 2 outer iterations * 2 inner iterations

  EXPECT_EQ(outer_map.template at<1>(), 21);  // 1 + 2*10
  EXPECT_EQ(outer_map.template at<2>(), 22);  // 2 + 2*10
}

TEST_F(StaticMapForEachTest, TemplateLambda) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  map.for_each([]<class T>(T& item) {
    if constexpr (std::is_same_v<typename T::val_t, int>) {
      item.val *= 2;
    } else if constexpr (std::is_same_v<typename T::val_t, std::string>) {
      item.val += "!";
    }
  });

  EXPECT_EQ(map.template at<1>(), 84);
  EXPECT_EQ(map.template at<'a'>(), "hello!");
}
