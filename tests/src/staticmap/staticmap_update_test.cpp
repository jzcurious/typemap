#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;
using DoubleItem
    = smap::Item<1, double>;  // Same key as IntItem1 but different value type

class StaticMapUpdateTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapUpdateTest, UpdateWithIndividualItems) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
  EXPECT_EQ(map.template at<3>(), 30);

  map.update(IntItem1(100), IntItem2(200));

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
  EXPECT_EQ(map.template at<3>(), 30);  // Should remain unchanged
}

TEST_F(StaticMapUpdateTest, UpdateWithSingleItem) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.update(IntItem1(42));

  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<2>(), 2);
}

TEST_F(StaticMapUpdateTest, UpdateWithCompatibleItem) {
  smap::StaticMap<DoubleItem> map(DoubleItem(3.14));

  map.update(IntItem1(42));  // IntItem1 has same key as DoubleItem

  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapUpdateTest, UpdateWithMultipleItems) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3, StringItem> map(
      IntItem1(1), IntItem2(2), IntItem3(3), StringItem("original"));

  map.update(IntItem1(100), IntItem3(300), StringItem("updated"));

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 2);  // Unchanged
  EXPECT_EQ(map.template at<3>(), 300);
  EXPECT_EQ(map.template at<'a'>(), "updated");
}

TEST_F(StaticMapUpdateTest, UpdateWithAnotherStaticMap) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map1(
      IntItem1(10), IntItem2(20), IntItem3(30));

  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(100), IntItem2(200));

  map1.update(map2);

  EXPECT_EQ(map1.template at<1>(), 100);
  EXPECT_EQ(map1.template at<2>(), 200);
  EXPECT_EQ(map1.template at<3>(), 30);  // Should remain unchanged
}

TEST_F(StaticMapUpdateTest, UpdateWithEmptyStaticMap) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  smap::StaticMap<> empty_map;

  map.update(empty_map);

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapUpdateTest, UpdateSelfWithSameMap) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  map.update(map);

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapUpdateTest, UpdateWithMoveSemantics) {
  smap::StaticMap<StringItem> map(StringItem("original"));

  std::string new_value = "moved_value";
  map.update(StringItem(std::move(new_value)));

  EXPECT_EQ(map.template at<'a'>(), "moved_value");
}

TEST_F(StaticMapUpdateTest, UpdateNonExistentKey) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  // This should compile but not change anything for non-existent keys
  map.update(IntItem1(100));  // This key exists
  // IntItem3(30) would not compile since key 3 doesn't exist in the map

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapUpdateTest, UpdateChaining) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  map.update(IntItem1(10)).update(IntItem2(20));

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
  EXPECT_EQ(map.template at<3>(), 3);
}

TEST_F(StaticMapUpdateTest, UpdateWithConvertibleTypes) {
  smap::StaticMap<DoubleItem> double_map(DoubleItem(1.5));

  double_map.update(IntItem1(42));  // int should be convertible to double

  EXPECT_DOUBLE_EQ(double_map.template at<1>(), 42.0);
}

TEST_F(StaticMapUpdateTest, UpdateConstMap) {
  const smap::StaticMap<IntItem1, IntItem2> const_map(IntItem1(10), IntItem2(20));

  // Update should not be available for const maps
  // This test is more of a compile-time check
}

TEST_F(StaticMapUpdateTest, UpdatePerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  map.update(IntItem1(100), IntItem2(200));

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 1000000);  // Should be very fast (less than 1ms)

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
  EXPECT_EQ(map.template at<3>(), 3);
}

TEST_F(StaticMapUpdateTest, UpdateWithSameValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  map.update(IntItem1(42), IntItem2(24));  // Same values

  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<2>(), 24);
}

TEST_F(StaticMapUpdateTest, UpdateReturnReference) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto& returned_map = map.update(IntItem1(10));

  EXPECT_EQ(&returned_map, &map);  // Should return reference to self
  EXPECT_EQ(map.template at<1>(), 10);
}
