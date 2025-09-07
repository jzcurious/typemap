#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapGetTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapGetTest, GetExistingKey) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.template get<1>(), 10);
  EXPECT_EQ(map.template get<2>(), 20);
}

TEST_F(StaticMapGetTest, GetNonExistingKeyWithDefault) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.template get<3>(42), 42);
  EXPECT_EQ(map.template get<0>(-1), -1);
}

TEST_F(StaticMapGetTest, GetNonExistingKeyDefaultInt) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.template get<3>(), 0);  // Default int value
}

TEST_F(StaticMapGetTest, GetStringValues) {
  const smap::StaticMap<StringItem> map(StringItem("hello"));

  EXPECT_EQ(map.template get<'a'>(), "hello");
  EXPECT_EQ(map.template get<'b'>("default"), "default");
  EXPECT_EQ(map.template get<'c'>(), 0);
}

TEST_F(StaticMapGetTest, GetMixedTypes) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  EXPECT_EQ(map.template get<1>(), 42);
  EXPECT_EQ(map.template get<'a'>(), "test");
  EXPECT_EQ(map.template get<2>(100), 100);
  EXPECT_EQ(map.template get<'b'>("default"), "default");
}

TEST_F(StaticMapGetTest, GetSingleItemMap) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  EXPECT_EQ(map.template get<1>(), 42);
  EXPECT_EQ(map.template get<2>(100), 100);
  EXPECT_EQ(map.template get<3>(), 0);
}

TEST_F(StaticMapGetTest, GetEmptyMap) {
  const smap::StaticMap<> map;

  EXPECT_EQ(map.template get<1>(42), 42);
  EXPECT_EQ(map.template get<0>(), 0);
}

TEST_F(StaticMapGetTest, GetCustomDefaultValues) {
  const smap::StaticMap<IntItem1> map(IntItem1(10));

  EXPECT_EQ(map.template get<1>(), 10);  // Existing key
  EXPECT_EQ(map.template get<2>(-1), -1);  // Custom int default
  EXPECT_EQ(map.template get<3>(42.5), 42.5);  // Custom double default
  EXPECT_EQ(map.template get<4>("not found"), "not found");  // Custom string default
}

TEST_F(StaticMapGetTest, GetZeroAndNegativeKeys) {
  using ZeroItem = smap::Item<0, int>;
  using NegativeItem = smap::Item<-1, int>;

  const smap::StaticMap<ZeroItem, NegativeItem> map(ZeroItem(100), NegativeItem(-200));

  EXPECT_EQ(map.template get<0>(), 100);
  EXPECT_EQ(map.template get<-1>(), -200);
  EXPECT_EQ(map.template get<1>(42), 42);
  EXPECT_EQ(map.template get<-2>(-1), -1);
}

// TEST_F(StaticMapGetTest, GetCompileTimeEvaluation) {
//   constexpr smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

//   static_assert(map.template get<1>() == 10);
//   static_assert(map.template get<2>() == 20);
//   static_assert(map.template get<3>(42) == 42);
// }

TEST_F(StaticMapGetTest, GetPerformance) {
  const smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] auto val1 = map.template get<1>();
    [[maybe_unused]] auto val2 = map.template get<4>(-1);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be very fast
}

TEST_F(StaticMapGetTest, GetDifferentDefaultTypes) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // Test with different default value types
  EXPECT_EQ(map.template get<2>(3.14), 3.14);
  EXPECT_EQ(map.template get<3>(std::string("default")), "default");
  EXPECT_EQ(map.template get<4>(true), true);
  EXPECT_EQ(map.template get<5>('x'), 'x');
}

// TEST_F(StaticMapGetTest, GetConstexprWithCustomDefaults) {
//   constexpr smap::StaticMap<IntItem1> map(IntItem1(42));

//   static_assert(map.template get<1>() == 42);
//   static_assert(map.template get<2>(100) == 100);
// }

TEST_F(StaticMapGetTest, GetLargeDefaultValue) {
  const smap::StaticMap<IntItem1> map(IntItem1(1));

  std::vector<int> large_default = {1, 2, 3, 4, 5};
  auto result = map.template get<2>(large_default);

  EXPECT_EQ(result, std::vector<int>({1, 2, 3, 4, 5}));
}

TEST_F(StaticMapGetTest, GetMoveOnlyDefault) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  auto move_only_default = std::make_unique<int>(100);
  auto result = map.template get<2>(std::move(move_only_default));

  EXPECT_NE(result, nullptr);
  EXPECT_EQ(*result, 100);
}

TEST_F(StaticMapGetTest, GetTypeDeduction) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  auto val1 = map.template get<1>();  // Should deduce to int
  auto val2 = map.template get<'a'>();  // Should deduce to std::string
  auto val3 = map.template get<2>(3.14);  // Should deduce to double

  static_assert(std::is_same_v<decltype(val1), int>);
  static_assert(std::is_same_v<decltype(val2), std::string>);
  static_assert(std::is_same_v<decltype(val3), double>);

  EXPECT_EQ(val1, 42);
  EXPECT_EQ(val2, "hello");
  EXPECT_DOUBLE_EQ(val3, 3.14);
}

TEST_F(StaticMapGetTest, GetWithExplicitTemplateDefaultType) {
  const smap::StaticMap<IntItem1> map(IntItem1(42));

  // Explicitly specify default value type
  EXPECT_EQ((map.template get<2, double>(3.14)), 3.14);
  EXPECT_EQ((map.template get<3, std::string>("not found")), "not found");
}

TEST_F(StaticMapGetTest, GetBooleanValues) {
  using BoolItem = smap::Item<1, bool>;
  const smap::StaticMap<BoolItem> map(BoolItem(true));

  EXPECT_EQ(map.template get<1>(), true);
  EXPECT_EQ(map.template get<2>(false), false);
  EXPECT_EQ(map.template get<3>(), false);  // Default bool value
}

TEST_F(StaticMapGetTest, GetEdgeCases) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(0),  // Zero value
      IntItem2(-1)  // Negative value
  );

  EXPECT_EQ(map.template get<1>(), 0);
  EXPECT_EQ(map.template get<2>(), -1);
  EXPECT_EQ(map.template get<3>(), 0);  // Default

  // Distinguish between zero value and default zero
  EXPECT_EQ(map.template get<1>(42), 0);  // Actual zero value, not default
  EXPECT_EQ(map.template get<4>(42), 42);  // Default value used
}
