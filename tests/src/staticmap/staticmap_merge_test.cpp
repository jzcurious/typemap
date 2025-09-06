#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestMap
    = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>, smap::Item<30, double>>;

using EmptyMap = smap::StaticMap<>;

class StaticMapMergeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_map1 = TestMap{100, 'A', 3.14};
    test_map2 = TestMap{200, 'B', 2.71};
    empty_map = EmptyMap{};
  }

  TestMap test_map1;  // NOLINT
  TestMap test_map2;  // NOLINT
  EmptyMap empty_map;  // NOLINT
};

TEST_F(StaticMapMergeTest, MergeTwoNonEmptyMaps) {
  auto result = test_map1.merge(test_map2);

  EXPECT_EQ(result.at<10>(), 200);
  EXPECT_EQ(result.at<20>(), 'B');
  EXPECT_DOUBLE_EQ(result.at<30>(), 2.71);
}

TEST_F(StaticMapMergeTest, MergeWithEmptyMap) {
  auto result = test_map1.merge(empty_map);

  static_assert(std::is_same_v<decltype(result), TestMap>);

  EXPECT_EQ(result.get<10>(), 100);
  EXPECT_EQ(result.get<20>(), 'A');
  EXPECT_DOUBLE_EQ(result.get<30>(), 3.14);
}

TEST_F(StaticMapMergeTest, MergeEmptyMapWithNonEmpty) {
  auto result = empty_map.merge(test_map1);

  static_assert(std::is_same_v<decltype(result), TestMap>);

  EXPECT_EQ(result.get<10>(), 100);
  EXPECT_EQ(result.get<20>(), 'A');
  EXPECT_DOUBLE_EQ(result.get<30>(), 3.14);
}

TEST_F(StaticMapMergeTest, MergeTwoEmptyMaps) {
  auto result = empty_map.merge(empty_map);

  static_assert(std::is_same_v<decltype(result), EmptyMap>);
}

TEST_F(StaticMapMergeTest, MergeWithRValue) {
  auto result = test_map1.merge(TestMap{300, 'C', 1.41});

  static_assert(std::is_same_v<decltype(result), TestMap>);

  EXPECT_EQ(result.get<10>(), 300);
  EXPECT_EQ(result.get<20>(), 'C');
  EXPECT_DOUBLE_EQ(result.get<30>(), 1.41);
}

TEST_F(StaticMapMergeTest, OriginalMapsUnchanged) {
  int original_value_10 = test_map1.get<10>();
  char original_value_20 = test_map1.get<20>();
  double original_value_30 = test_map1.get<30>();

  test_map1.merge(test_map2);

  EXPECT_EQ(test_map1.get<10>(), original_value_10);
  EXPECT_EQ(test_map1.get<20>(), original_value_20);
  EXPECT_DOUBLE_EQ(test_map1.get<30>(), original_value_30);
}

TEST_F(StaticMapMergeTest, MergeMapsWithDifferentKeys) {
  using Map1 = smap::StaticMap<smap::Item<1, int>, smap::Item<2, char>>;
  using Map2 = smap::StaticMap<smap::Item<3, double>, smap::Item<4, float>>;

  Map1 map1{10, 'A'};
  Map2 map2{3.14, 2.71f};

  auto result = map1.merge(map2);

  EXPECT_EQ(result.get<1>(), 10);
  EXPECT_EQ(result.get<2>(), 'A');
  EXPECT_DOUBLE_EQ(result.get<3>(), 3.14);
  EXPECT_FLOAT_EQ(result.get<4>(), 2.71f);
}

TEST_F(StaticMapMergeTest, MergeWithDifferentValueTypes) {
  using IntMap = smap::StaticMap<smap::Item<1, int>>;
  using DoubleMap = smap::StaticMap<smap::Item<1, double>>;

  IntMap int_map{42};
  DoubleMap double_map{3.14};

  auto result = int_map.merge(double_map);

  EXPECT_DOUBLE_EQ(result.get<1>(), 3.14);
}
