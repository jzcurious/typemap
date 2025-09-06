#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

TEST(MakeStaticMapTest, BasicUsage) {
  auto map = smap::make_static_map(IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_EQ(map.size, 3);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
  EXPECT_EQ(map.template at<3>(), 30);
}

TEST(MakeStaticMapTest, SingleItem) {
  auto map = smap::make_static_map(IntItem1(42));

  EXPECT_EQ(map.size, 1);
  EXPECT_EQ(map.template at<1>(), 42);
}

TEST(MakeStaticMapTest, MixedTypes) {
  auto map = smap::make_static_map(IntItem1(100), StringItem("test"));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<'a'>(), "test");
}

TEST(MakeStaticMapTest, MoveSemantics) {
  std::string test_value = "moved_string";
  auto map = smap::make_static_map(StringItem(std::move(test_value)));

  EXPECT_EQ(map.size, 1);
  EXPECT_EQ(map.template at<'a'>(), "moved_string");
}

TEST(MakeStaticMapTest, TypeDeduction) {
  auto map = smap::make_static_map(smap::Item<1, int>(42),
      smap::Item<2, std::string>("hello"),
      smap::Item<3, double>(3.14));

  EXPECT_EQ(map.size, 3);
  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<2>(), "hello");
  EXPECT_DOUBLE_EQ(map.template at<3>(), 3.14);
}

TEST(MakeStaticMapTest, ConstCorrectness) {
  const auto map = smap::make_static_map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST(MakeStaticMapTest, EmptyMap) {
  auto map = smap::make_static_map();

  EXPECT_EQ(map.size, 0);
  EXPECT_TRUE(map.empty());
}

TEST(MakeStaticMapTest, TemplateArgumentDeduction) {
  auto map = smap::make_static_map(
      smap::Item<'x', int>(100), smap::Item<'y', std::string>("value"));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<'x'>(), 100);
  EXPECT_EQ(map.template at<'y'>(), "value");
}

TEST(MakeStaticMapTest, ChainedOperations) {
  auto map = smap::make_static_map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.size, 2);

  auto updated_map = map.update(IntItem1(100));
  EXPECT_EQ(updated_map.template at<1>(), 100);
  EXPECT_EQ(updated_map.template at<2>(), 20);
}

TEST(MakeStaticMapTest, PerfectForwarding) {
  auto map = smap::make_static_map(IntItem1(10), StringItem(std::string("test")));

  EXPECT_EQ(map.size, 2);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<'a'>(), "test");
}

TEST(MakeStaticMapTest, ComparisonWithDirectConstruction) {
  auto made_map = smap::make_static_map(IntItem1(10), IntItem2(20));

  smap::StaticMap<IntItem1, IntItem2> direct_map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(made_map.size, direct_map.size);
  EXPECT_EQ(made_map.template at<1>(), direct_map.template at<1>());
  EXPECT_EQ(made_map.template at<2>(), direct_map.template at<2>());
}
