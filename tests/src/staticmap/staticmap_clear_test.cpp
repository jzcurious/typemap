#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapClearTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapClearTest, ClearBasicValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  map.clear();

  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
}

TEST_F(StaticMapClearTest, ClearStringValues) {
  smap::StaticMap<StringItem> map(StringItem("hello world"));

  map.clear();

  EXPECT_EQ(map.template at<'a'>(), "");
}

TEST_F(StaticMapClearTest, ClearMixedTypes) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test string"));

  map.clear();

  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<'a'>(), "");
}

TEST_F(StaticMapClearTest, ClearSingleItemMap) {
  smap::StaticMap<IntItem1> map(IntItem1(100));

  map.clear();

  EXPECT_EQ(map.template at<1>(), 0);
}

TEST_F(StaticMapClearTest, ClearEmptyMap) {
  smap::StaticMap<> map;

  EXPECT_NO_THROW(map.clear());
  EXPECT_TRUE(map.empty());
}

TEST_F(StaticMapClearTest, ClearMultipleTimes) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  map.template at<1>() = 100;
  map.template at<2>() = 200;

  map.clear();
  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);

  map.template at<1>() = 50;
  map.template at<2>() = 60;

  map.clear();
  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
}

TEST_F(StaticMapClearTest, ClearPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1000), IntItem2(2000), IntItem3(3000));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    map.template at<1>() = i;
    map.template at<2>() = i * 2;
    map.template at<3>() = i * 3;
    map.clear();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast

  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
  EXPECT_EQ(map.template at<3>(), 0);
}

TEST_F(StaticMapClearTest, ClearAfterUpdates) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.update(IntItem1(100), IntItem2(200));
  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);

  map.clear();
  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
}

TEST_F(StaticMapClearTest, ClearWithZeroDefaultValue) {
  using ZeroDefaultItem = smap::Item<1, int>;
  smap::StaticMap<ZeroDefaultItem> map(ZeroDefaultItem(42));

  map.clear();
  EXPECT_EQ(map.template at<1>(), 0);
}

TEST_F(StaticMapClearTest, ClearBooleanValues) {
  using BoolItem = smap::Item<1, bool>;
  smap::StaticMap<BoolItem> map(BoolItem(true));

  map.clear();
  EXPECT_EQ(map.template at<1>(), false);
}

TEST_F(StaticMapClearTest, ClearFloatingPointValues) {
  using DoubleItem = smap::Item<1, double>;
  smap::StaticMap<DoubleItem> map(DoubleItem(3.14159));  // NOLINT

  map.clear();
  EXPECT_DOUBLE_EQ(map.template at<1>(), 0.0);
}

TEST_F(StaticMapClearTest, ClearComplexTypes) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  smap::StaticMap<VectorItem> map(VectorItem(std::vector<int>{1, 2, 3, 4, 5}));

  EXPECT_FALSE(map.template at<1>().empty());

  map.clear();
  EXPECT_TRUE(map.template at<1>().empty());
}

// TEST_F(StaticMapClearTest, ClearMoveOnlyTypes) {
//   using UniquePtrItem = smap::Item<1, std::unique_ptr<int>>;
//   smap::StaticMap<UniquePtrItem> map(UniquePtrItem(std::make_unique<int>(42)));

//   EXPECT_NE(map.template at<1>(), nullptr);

//   map.clear();
//   EXPECT_EQ(map.template at<1>(), nullptr);
// }

TEST_F(StaticMapClearTest, ClearLargeStrings) {
  smap::StaticMap<StringItem> map(StringItem(std::string(10000, 'x')));

  EXPECT_FALSE(map.template at<'a'>().empty());

  map.clear();
  EXPECT_TRUE(map.template at<'a'>().empty());
}

TEST_F(StaticMapClearTest, ClearReturn) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  static_assert(std::same_as<smap::StaticMap<IntItem1>&, decltype(map.clear())>);
  EXPECT_NO_THROW(map.clear());
}

TEST_F(StaticMapClearTest, ClearConstMap) {
  const smap::StaticMap<IntItem1> const_map(IntItem1(42));

  // clear() should not be available for const maps
  // const_map.clear(); // Should not compile
}

TEST_F(StaticMapClearTest, ClearDefaultConstructibleRequirement) {
  // This test ensures that all value types are default constructible
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  // Should compile and work because int and std::string are default constructible
  EXPECT_NO_THROW(map.clear());
}

TEST_F(StaticMapClearTest, ClearStateAfterClear) {
  smap::StaticMap<IntItem1, IntItem2, StringItem> map(
      IntItem1(100), IntItem2(-200), StringItem("non-empty string"));

  // Verify initial state
  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), -200);
  EXPECT_EQ(map.template at<'a'>(), "non-empty string");

  map.clear();

  // Verify cleared state
  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
  EXPECT_EQ(map.template at<'a'>(), "");

  // Verify map is still functional
  map.template at<1>() = 50;
  map.template at<2>() = -100;
  map.template at<'a'>() = "new string";

  EXPECT_EQ(map.template at<1>(), 50);
  EXPECT_EQ(map.template at<2>(), -100);
  EXPECT_EQ(map.template at<'a'>(), "new string");
}
