#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapItemsTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapItemsTest, BasicItemsAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).val, 10);
  EXPECT_EQ(std::get<1>(items).val, 20);
}

TEST_F(StaticMapItemsTest, ModifyItemsThroughItemsMethod) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto& items = map.items();
  std::get<0>(items).val = 100;
  std::get<1>(items).val = 200;

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
}

TEST_F(StaticMapItemsTest, ConstItemsAccess) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  const auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).val, 42);
  EXPECT_EQ(std::get<1>(items).val, 24);
}

TEST_F(StaticMapItemsTest, MixedTypesItems) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(100), StringItem("hello"));

  auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).val, 100);
  EXPECT_EQ(std::get<1>(items).val, "hello");
}

TEST_F(StaticMapItemsTest, SingleItemItems) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).val, 42);
}

TEST_F(StaticMapItemsTest, EmptyMapItems) {
  smap::StaticMap<> map;

  auto& items = map.items();

  EXPECT_EQ(std::tuple_size_v<std::decay_t<decltype(items)>>, 0);
}

TEST_F(StaticMapItemsTest, StringManipulationThroughItems) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  auto& items = map.items();
  std::get<0>(items).val += " world";

  EXPECT_EQ(map.template at<'a'>(), "hello world");
}

TEST_F(StaticMapItemsTest, MultipleModifications) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto& items = map.items();
  std::get<0>(items).val *= 10;
  std::get<1>(items).val *= 20;
  std::get<2>(items).val *= 30;

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 40);
  EXPECT_EQ(map.template at<3>(), 90);
}

TEST_F(StaticMapItemsTest, ReferenceSemantics) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto& items = map.items();
  std::get<0>(items).val = 42;

  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapItemsTest, ConstReferenceSemantics) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  const auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).val, 1);
  EXPECT_EQ(std::get<1>(items).val, 2);
}

TEST_F(StaticMapItemsTest, ItemsType) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto& items = map.items();

  static_assert(std::same_as<decltype(items), std::tuple<IntItem1, IntItem2, IntItem3>&>);
}

TEST_F(StaticMapItemsTest, ConstItemsType) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  const auto& items = map.items();

  static_assert(std::same_as<decltype(items), const std::tuple<IntItem1, IntItem2>&>);
}

TEST_F(StaticMapItemsTest, ItemsSize) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto& items = map.items();

  EXPECT_EQ(std::tuple_size_v<std::decay_t<decltype(items)>>, 3);
}

TEST_F(StaticMapItemsTest, DirectItemAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  auto& items = map.items();

  EXPECT_EQ(std::get<IntItem1>(items).val, 10);
  EXPECT_EQ(std::get<IntItem2>(items).val, 20);
}

TEST_F(StaticMapItemsTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    auto& items = map.items();
    std::get<0>(items).val += 1;
    std::get<1>(items).val += 1;
    std::get<2>(items).val += 1;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);

  EXPECT_EQ(map.template at<1>(), 10001);
  EXPECT_EQ(map.template at<2>(), 10002);
  EXPECT_EQ(map.template at<3>(), 10003);
}

TEST_F(StaticMapItemsTest, UseWithStandardAlgorithms) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  auto& items = map.items();

  std::apply([](auto&... item) { ((item.val *= 2), ...); }, items);

  EXPECT_EQ(map.template at<1>(), 20);
  EXPECT_EQ(map.template at<2>(), 40);
  EXPECT_EQ(map.template at<3>(), 60);
}

TEST_F(StaticMapItemsTest, AssignmentThroughItems) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(1), IntItem2(2));

  smap::StaticMap<IntItem1, IntItem2> map2(IntItem1(100), IntItem2(200));

  map1.items() = map2.items();

  EXPECT_EQ(map1.template at<1>(), 100);
  EXPECT_EQ(map1.template at<2>(), 200);
}

TEST_F(StaticMapItemsTest, SelfAssignment) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  auto& items = map.items();
  map.items() = items;  // Self-assignment

  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<2>(), 24);
}

TEST_F(StaticMapItemsTest, ItemsKeyAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  auto& items = map.items();

  EXPECT_EQ(std::get<0>(items).key, 1);
  EXPECT_EQ(std::get<1>(items).key, 2);
  EXPECT_EQ(std::get<0>(items).val, 10);
  EXPECT_EQ(std::get<1>(items).val, 20);
}
