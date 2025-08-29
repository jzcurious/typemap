#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestMap
    = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>, smap::Item<30, double>>;

TEST(StaticMapTest, Size) {
  EXPECT_EQ(TestMap::size, 3);
}

TEST(StaticMapTest, Keys) {
  TestMap map{};
  auto keys = map.keys();
  EXPECT_EQ(std::get<0>(keys), 10);
  EXPECT_EQ(std::get<1>(keys), 20);
  EXPECT_EQ(std::get<2>(keys), 30);
}

TEST(StaticMapTest, Vals) {
  TestMap map{100, 'A', 3.14};
  auto vals = map.vals();
  EXPECT_EQ(std::get<0>(vals), 100);
  EXPECT_EQ(std::get<1>(vals), 'A');
  EXPECT_DOUBLE_EQ(std::get<2>(vals), 3.14);
}

TEST(StaticMapTest, Items) {
  TestMap map{100, 'A', 3.14};
  const auto& items = map.items();
  EXPECT_EQ(std::get<0>(items).val, 100);
  EXPECT_EQ(std::get<1>(items).val, 'A');
  EXPECT_DOUBLE_EQ(std::get<2>(items).val, 3.14);
}

TEST(StaticMapTest, Contains) {
  EXPECT_TRUE((TestMap::contains<10>()));
  EXPECT_TRUE((TestMap::contains<20>()));
  EXPECT_TRUE((TestMap::contains<30>()));
  EXPECT_FALSE((TestMap::contains<40>()));
}

TEST(StaticMapTest, At) {
  TestMap map{100, 'A', 3.14};

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);

  const auto& cmap = map;
  EXPECT_EQ(cmap.at<10>(), 100);
  EXPECT_EQ(cmap.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(cmap.at<30>(), 3.14);
}

TEST(StaticMapTest, GetWithDefault) {
  TestMap map{100, 'A', 3.14};

  EXPECT_EQ(map.get<10>(-1), 100);
  EXPECT_EQ(map.get<20>('Z'), 'A');
  EXPECT_DOUBLE_EQ(map.get<30>(0.0), 3.14);

  EXPECT_EQ(map.get<999>(-1), -1);
  EXPECT_EQ(map.get<999>('Z'), 'Z');
  EXPECT_DOUBLE_EQ(map.get<999>(0.0), 0.0);
}

// TEST(StaticMapTest, Iterator) {
//   TestMap map{100, 'A', 3.14};

//   auto it = map.begin();
//   ASSERT_NE(it, map.end());
//   EXPECT_EQ(it->val, 100);
//   ++it;

//   ASSERT_NE(it, map.end());
//   EXPECT_EQ(it->val, 'A');
//   ++it;

//   ASSERT_NE(it, map.end());
//   EXPECT_DOUBLE_EQ(it->val, 3.14);
//   ++it;

//   EXPECT_EQ(it, map.end());
// }
