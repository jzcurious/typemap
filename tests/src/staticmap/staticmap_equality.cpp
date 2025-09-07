#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapEqualityTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapEqualityTest, IdenticalMaps) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(10), IntItem2(20));
  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(10), IntItem2(20));

  EXPECT_TRUE(map1 == map2);
  EXPECT_TRUE(map2 == map1);
}

TEST_F(StaticMapEqualityTest, DifferentValues) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(10), IntItem2(20));
  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(15), IntItem2(20));

  EXPECT_FALSE(map1 == map2);
  EXPECT_FALSE(map2 == map1);
}

TEST_F(StaticMapEqualityTest, DifferentValueInSecondItem) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(10), IntItem2(20));
  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(10), IntItem2(25));

  EXPECT_FALSE(map1 == map2);
  EXPECT_FALSE(map2 == map1);
}

TEST_F(StaticMapEqualityTest, MixedTypesEqual) {
  smap::StaticMap<IntItem1, StringItem> map1(IntItem1(42), StringItem("hello"));
  smap::StaticMap<IntItem1, StringItem> map2(IntItem1(42), StringItem("hello"));

  EXPECT_TRUE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, MixedTypesNotEqual) {
  smap::StaticMap<IntItem1, StringItem> map1(IntItem1(42), StringItem("hello"));
  smap::StaticMap<IntItem1, StringItem> map2(IntItem1(42), StringItem("world"));

  EXPECT_FALSE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, DifferentSizes) {
  smap::StaticMap<IntItem1, IntItem2> smaller_map(IntItem1(10), IntItem2(20));

  smap::StaticMap<IntItem1, IntItem2, IntItem3> larger_map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_FALSE(smaller_map == larger_map);
  EXPECT_FALSE(larger_map == smaller_map);
}

TEST_F(StaticMapEqualityTest, EmptyMaps) {
  smap::StaticMap<> empty_map1;
  smap::StaticMap<> empty_map2;

  EXPECT_TRUE(empty_map1 == empty_map2);
}

TEST_F(StaticMapEqualityTest, SingleItemMapsEqual) {
  smap::StaticMap<IntItem1> map1(IntItem1(42));
  smap::StaticMap<IntItem1> map2(IntItem1(42));

  EXPECT_TRUE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, SingleItemMapsNotEqual) {
  smap::StaticMap<IntItem1> map1(IntItem1(42));
  smap::StaticMap<IntItem1> map2(IntItem1(43));

  EXPECT_FALSE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, SameKeysDifferentOrder) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map1(
      IntItem1(10), IntItem2(20), IntItem3(30));

  smap::StaticMap<IntItem3, IntItem1, IntItem2> map2(
      IntItem3(30), IntItem1(10), IntItem2(20));

  EXPECT_TRUE(map1 == map2);
  EXPECT_TRUE(map2 == map1);
}

TEST_F(StaticMapEqualityTest, PartialOverlap) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(10), IntItem2(20));
  smap::StaticMap<IntItem2, IntItem3> map2(IntItem2(20), IntItem3(30));

  // Should be false because they have different sizes and different key sets
  EXPECT_FALSE(map1 == map2);
  EXPECT_FALSE(map2 == map1);
}

TEST_F(StaticMapEqualityTest, SelfEquality) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_TRUE(map == map);
}

TEST_F(StaticMapEqualityTest, DefaultConstructedMaps) {
  smap::StaticMap<IntItem1, IntItem2> map1;
  smap::StaticMap<IntItem1, IntItem2> map2;

  EXPECT_TRUE(map1 == map2);

  map1.template at<1>() = 42;
  EXPECT_FALSE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, UpdatedMaps) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(1), IntItem2(2));

  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(1), IntItem2(2));

  EXPECT_TRUE(map1 == map2);

  map1.update(IntItem1(10));
  EXPECT_FALSE(map1 == map2);

  map2.update(IntItem1(10));
  EXPECT_TRUE(map1 == map2);
}

TEST_F(StaticMapEqualityTest, ConstMaps) {
  const smap::StaticMap<IntItem1, IntItem2> const_map1(IntItem1(10), IntItem2(20));

  const smap::StaticMap<IntItem1, IntItem2> const_map2(IntItem1(10), IntItem2(20));

  EXPECT_TRUE(const_map1 == const_map2);
}

TEST_F(StaticMapEqualityTest, FloatingPointValues) {
  smap::StaticMap<smap::Item<1, double>> map1(smap::Item<1, double>(3.14159));  // NOLINT
  smap::StaticMap<smap::Item<1, double>> map2(smap::Item<1, double>(3.14159));  // NOLINT
  smap::StaticMap<smap::Item<1, double>> map3(smap::Item<1, double>(3.14160));  // NOLINT

  EXPECT_TRUE(map1 == map2);
  EXPECT_FALSE(map1 == map3);
}

TEST_F(StaticMapEqualityTest, StringEmptyValues) {
  smap::StaticMap<StringItem> map1(StringItem(""));
  smap::StaticMap<StringItem> map2(StringItem(""));
  smap::StaticMap<StringItem> map3(StringItem("non-empty"));

  EXPECT_TRUE(map1 == map2);
  EXPECT_FALSE(map1 == map3);
}

TEST_F(StaticMapEqualityTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map1(
      IntItem1(1000), IntItem2(2000), IntItem3(3000));

  smap::StaticMap<IntItem1, IntItem2, IntItem3> map2(
      IntItem1(1000), IntItem2(2000), IntItem3(3000));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    bool result = (map1 == map2);
    EXPECT_TRUE(result);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 100000);  // Should be fast
}
