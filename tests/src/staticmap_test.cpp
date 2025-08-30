#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestMap
    = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>, smap::Item<30, double>>;

TEST(StaticMapTest, Size) {
  EXPECT_EQ(TestMap::size, 3);
}

TEST(StaticMapConstructorTest, ConstructWithValues) {
  TestMap map(100, 'A', 3.14);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapConstructorTest, ConstructWithLValueReferences) {
  int val1 = 100;
  char val2 = 'A';
  double val3 = 3.14;

  TestMap map(val1, val2, val3);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapConstructorTest, ConstructWithConstValues) {
  const int val1 = 100;
  const char val2 = 'A';
  const double val3 = 3.14;

  TestMap map(val1, val2, val3);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapConstructorTest, PartialInitialization) {
  TestMap map(100, 'A');

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 0.0);
}

TEST(StaticMapConstructorTest, PartialInitializationSingleValue) {
  TestMap map(42);

  EXPECT_EQ(map.at<10>(), 42);
  EXPECT_EQ(map.at<20>(), '\0');
  EXPECT_DOUBLE_EQ(map.at<30>(), 0.0);
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

TEST(StaticMapUpdateTest, UpdateWithSameTypeMap) {
  TestMap map1(100, 'A', 3.14);
  TestMap map2(200, 'B', 6.28);

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 200);
  EXPECT_EQ(map1.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 6.28);
}

TEST(StaticMapUpdateTest, UpdateWithPartialMap) {
  using PartialMap = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>>;

  TestMap map1(100, 'A', 3.14);
  PartialMap map2(200, 'B');

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 200);
  EXPECT_EQ(map1.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);
}

TEST(StaticMapUpdateTest, UpdateWithDifferentKeys) {
  using DifferentMap = smap::StaticMap<smap::Item<40, int>, smap::Item<50, char>>;

  TestMap map1(100, 'A', 3.14);
  DifferentMap map2(999, 'Z');

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);
}

TEST(StaticMapUpdateTest, UpdateWithMixedKeys) {
  using MixedMap = smap::StaticMap<smap::Item<10, int>, smap::Item<40, char>>;

  TestMap map1(100, 'A', 3.14);
  MixedMap map2(200, 'X');

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 200);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);
}

TEST(StaticMapUpdateTest, UpdateWithEmptyMap) {
  TestMap map1(100, 'A', 3.14);
  TestMap map2(0, '\0', 0.0);

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 0);
  EXPECT_EQ(map1.at<20>(), '\0');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 0.0);
}

TEST(StaticMapUpdateTest, UpdateSelf) {
  TestMap map(100, 'A', 3.14);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);

  map.update(map);

  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateTest, UpdateConstMap) {
  const TestMap source(200, 'B', 6.28);
  TestMap target(100, 'A', 3.14);

  EXPECT_EQ(target.at<10>(), 100);
  EXPECT_EQ(target.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(target.at<30>(), 3.14);

  target.update(source);

  EXPECT_EQ(target.at<10>(), 200);
  EXPECT_EQ(target.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(target.at<30>(), 6.28);
}

TEST(StaticMapUpdateTest, UpdateDifferentValueTypes) {
  using TypeMap = smap::StaticMap<smap::Item<10, long>,  // int -> long
      smap::Item<20, wchar_t>>;  // char -> wchar_t

  TestMap map1(100, 'A', 3.14);
  TypeMap map2(200L, L'B');

  EXPECT_EQ(map1.at<10>(), 100);
  EXPECT_EQ(map1.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);

  map1.update(map2);

  EXPECT_EQ(map1.at<10>(), 200);
  EXPECT_EQ(map1.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 3.14);
}

TEST(StaticMapUpdateTest, MultipleUpdates) {
  TestMap map1(100, 'A', 3.14);
  TestMap map2(200, 'B', 6.28);
  TestMap map3(300, 'C', 9.42);

  map1.update(map2);
  EXPECT_EQ(map1.at<10>(), 200);
  EXPECT_EQ(map1.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 6.28);

  map1.update(map3);
  EXPECT_EQ(map1.at<10>(), 300);
  EXPECT_EQ(map1.at<20>(), 'C');
  EXPECT_DOUBLE_EQ(map1.at<30>(), 9.42);
}

TEST(StaticMapUpdateItemsTest, UpdateWithSingleItem) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, int>(200));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithMultipleItems) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, int>(200), smap::Item<20, char>('B'));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithAllItems) {
  TestMap map(100, 'A', 3.14);
  map.update(
      smap::Item<10, int>(200), smap::Item<20, char>('B'), smap::Item<30, double>(6.28));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 6.28);
}

TEST(StaticMapUpdateItemsTest, UpdateWithNonExistentKey) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<999, int>(999));
  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithMixedExistingAndNonExisting) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, int>(200),
      smap::Item<999, float>(1.0f),
      smap::Item<30, double>(6.28));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 6.28);
}

TEST(StaticMapUpdateItemsTest, UpdateWithDifferentValueTypes) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, long>(200L));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithMoveSemantics) {
  TestMap map(100, 'A', 3.14);
  smap::Item<10, int> item1(200);
  smap::Item<20, char> item2('B');
  map.update(std::move(item1), std::move(item2));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithConstItems) {
  TestMap map(100, 'A', 3.14);
  const smap::Item<10, int> const_item(200);
  const smap::Item<20, char> const_item2('B');
  map.update(const_item, const_item2);
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateWithLValueReferences) {
  TestMap map(100, 'A', 3.14);
  smap::Item<10, int> item1(200);
  smap::Item<20, char> item2('B');
  map.update(item1, item2);
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateEmptyParameterPack) {
  TestMap map(100, 'A', 3.14);
  map.update();
  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdatePerformance) {
  TestMap map(100, 'A', 3.14);
  for (int i = 0; i < 100; ++i) {
    map.update(smap::Item<10, int>(100 + i));
    EXPECT_EQ(map.at<10>(), 100 + i);
  }
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}

TEST(StaticMapUpdateItemsTest, UpdateChaining) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, int>(200));
  map.update(smap::Item<20, char>('B'));
  map.update(smap::Item<30, double>(6.28));
  EXPECT_EQ(map.at<10>(), 200);
  EXPECT_EQ(map.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(map.at<30>(), 6.28);
}

TEST(StaticMapUpdateItemsTest, UpdateWithSameValues) {
  TestMap map(100, 'A', 3.14);
  map.update(smap::Item<10, int>(100), smap::Item<20, char>('A'));
  EXPECT_EQ(map.at<10>(), 100);
  EXPECT_EQ(map.at<20>(), 'A');
  EXPECT_DOUBLE_EQ(map.at<30>(), 3.14);
}
