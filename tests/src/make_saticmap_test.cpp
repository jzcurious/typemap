#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestItem1 = smap::Item<10, int>;
using TestItem2 = smap::Item<20, char>;
using TestItem3 = smap::Item<30, double>;

TEST(StaticMapFactoryTest, MakeStaticMapWithConstReferences) {
  TestItem1 item1(100);
  TestItem2 item2('A');
  TestItem3 item3(3.14);

  auto map = smap::make_static_map(item1, item2, item3);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapFactoryTest, MakeStaticMapWithTemporaryItems) {
  auto map = smap::make_static_map(TestItem1(100), TestItem2('A'), TestItem3(3.14));

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapFactoryTest, MakeStaticMapTypeDeduction) {
  auto map = smap::make_static_map(smap::Item<10, int>(100), smap::Item<20, char>('A'));

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
}

TEST(StaticMapFactoryTest, MakeStaticMapConstCorrectness) {
  const TestItem1 const_item(100);
  TestItem2 item2('A');

  auto map = smap::make_static_map(const_item, item2);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
}

TEST(StaticMapFactoryTest, MakeStaticMapEmpty) {
  auto empty_map = smap::make_static_map();

  EXPECT_EQ(empty_map.size, 0);
}

TEST(StaticMapFactoryTest, MakeStaticMapSingleItem) {
  auto map = smap::make_static_map(TestItem1(42));

  EXPECT_EQ(map.at<10>(), 42);
  EXPECT_EQ(map.size, 1);
}

TEST(StaticMapFactoryTest, MakeStaticMapDifferentValueTypes) {
  auto map = smap::make_static_map(smap::Item<1, std::string>("hello"),
      smap::Item<2, int>(42),
      smap::Item<3, double>(3.14));

  EXPECT_EQ(map.size, 3);
}
