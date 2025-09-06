#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>
#include <utility>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapTest, DefaultConstructor) {
  smap::StaticMap<IntItem1, IntItem2> map;

  EXPECT_EQ(map.size, 2);
  EXPECT_FALSE(map.empty());

  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<2>(), 0);
}

TEST_F(StaticMapTest, FullInitializationConstructor) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_EQ(map.size, 3);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
  EXPECT_EQ(map.template at<3>(), 30);
}

TEST_F(StaticMapTest, PartialInitializationConstructor) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(IntItem1(100));

  EXPECT_EQ(map.size, 3);
  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 0);
  EXPECT_EQ(map.template at<3>(), 0);
}

TEST_F(StaticMapTest, PartialMultipleInitializationConstructor) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(IntItem1(100), IntItem2(200));

  EXPECT_EQ(map.size, 3);
  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
  EXPECT_EQ(map.template at<3>(), 0);
}

TEST_F(StaticMapTest, MixedValueTypes) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<'a'>(), "hello");
}

TEST_F(StaticMapTest, UniqueKeysConstraint) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map;  // NOLINT
  SUCCEED();
}

TEST_F(StaticMapTest, MoveSemantics) {
  std::string test_string = "moved_value";
  smap::StaticMap<StringItem> map(StringItem(std::move(test_string)));

  EXPECT_EQ(map.template at<'a'>(), "moved_value");
}

TEST_F(StaticMapTest, EdgeCases) {
  smap::StaticMap<IntItem1> single_map(IntItem1(42));
  EXPECT_EQ(single_map.size, 1);
  EXPECT_EQ(single_map.template at<1>(), 42);
}

TEST_F(StaticMapTest, MakeStaticMapFunction) {
  auto map = smap::make_static_map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapTest, ConstCorrectness) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(100), IntItem2(200));

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);

  EXPECT_TRUE(map.template contains<1>());
  EXPECT_FALSE(map.template contains<3>());
}

TEST_F(StaticMapTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  EXPECT_EQ(map.template at<1>(), 1);
  EXPECT_EQ(map.template at<2>(), 2);
  EXPECT_EQ(map.template at<3>(), 3);
}

TEST_F(StaticMapTest, DefaultInitialization) {
  smap::StaticMap<IntItem1, StringItem> map;

  EXPECT_EQ(map.template at<1>(), 0);
  EXPECT_EQ(map.template at<'a'>(), "");
}

TEST_F(StaticMapTest, RecursiveConstruction) {
  // clang-format off
  auto map = smap::StaticMap<
    smap::Item<1,
      smap::StaticMap<
        smap::Item<2,
          smap::StaticMap<
            smap::Item<3, std::integral_constant<int, 888>>
          >
        >
      >
    >
  >{};
  // clang-format on

  auto val = map.at<1>().at<2>().at<3>();

  EXPECT_EQ(val, 888);
}
