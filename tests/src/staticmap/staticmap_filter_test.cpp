#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestMap = smap::StaticMap<smap::Item<1, int>,
    smap::Item<2, char>,
    smap::Item<3, double>,
    smap::Item<4, float>>;

using EmptyMap = smap::StaticMap<>;

struct AlwaysTruePredicate {
  template <class ItemT>
  constexpr bool operator()() const {
    return true;
  }
};

struct AlwaysFalsePredicate {
  template <class ItemT>
  constexpr bool operator()() const {
    return false;
  }
};

class StaticMapFilterTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_map = TestMap{10, 'A', 3.14, 2.71f};
  }

  TestMap test_map;  // NOLINT
};

TEST_F(StaticMapFilterTest, FilterEvenKeys) {
  auto result = test_map.filter<[]<smap::ItemKind ItemT>() {
    return ItemT::key % 2 == 0;
  }>();

  static_assert(result.size == 2);

  EXPECT_TRUE(result.contains<2>());
  EXPECT_TRUE(result.contains<4>());
  EXPECT_FALSE(result.contains<1>());
  EXPECT_FALSE(result.contains<3>());

  EXPECT_EQ(result.get<2>(), 'A');
  EXPECT_FLOAT_EQ(result.get<4>(), 2.71f);
}

TEST_F(StaticMapFilterTest, FilterGreaterThan) {
  auto result = test_map.filter<[]<smap::ItemKind ItemT>() {
    return (ItemT::key > 2);
  }>();

  static_assert(result.size == 2);

  EXPECT_TRUE(result.contains<3>());
  EXPECT_TRUE(result.contains<4>());
  EXPECT_FALSE(result.contains<1>());
  EXPECT_FALSE(result.contains<2>());

  EXPECT_DOUBLE_EQ(result.get<3>(), 3.14);
  EXPECT_FLOAT_EQ(result.get<4>(), 2.71f);
}

TEST_F(StaticMapFilterTest, FilterAlwaysTrue) {
  auto result = test_map.filter<AlwaysTruePredicate{}>();

  static_assert(result.size == 4);

  EXPECT_TRUE(result.contains<1>());
  EXPECT_TRUE(result.contains<2>());
  EXPECT_TRUE(result.contains<3>());
  EXPECT_TRUE(result.contains<4>());

  EXPECT_EQ(result.get<1>(), 10);
  EXPECT_EQ(result.get<2>(), 'A');
  EXPECT_DOUBLE_EQ(result.get<3>(), 3.14);
  EXPECT_FLOAT_EQ(result.get<4>(), 2.71f);
}

TEST_F(StaticMapFilterTest, FilterAlwaysFalse) {
  auto result = test_map.filter<AlwaysFalsePredicate{}>();

  static_assert(result.size == 0);

  EXPECT_FALSE(result.contains<1>());
  EXPECT_FALSE(result.contains<2>());
  EXPECT_FALSE(result.contains<3>());
  EXPECT_FALSE(result.contains<4>());
}

TEST_F(StaticMapFilterTest, FilterEmptyMap) {
  EmptyMap empty_map;
  auto result = empty_map.filter<[]<smap::ItemKind ItemT>() {
    return ItemT::key % 2 == 0;
  }>();

  EXPECT_EQ(result.size, 0);
}

TEST_F(StaticMapFilterTest, FilterWithLambda) {
  auto result = test_map.filter<[]<smap::ItemKind ItemT>() {
    return ItemT::key == 1 || ItemT::key == 3;
  }>();

  EXPECT_TRUE(result.contains<1>());
  EXPECT_TRUE(result.contains<3>());
  EXPECT_FALSE(result.contains<2>());
  EXPECT_FALSE(result.contains<4>());

  EXPECT_EQ(result.get<1>(), 10);
  EXPECT_DOUBLE_EQ(result.get<3>(), 3.14);
}

TEST_F(StaticMapFilterTest, OriginalMapUnchanged) {
  int original_1 = test_map.get<1>();
  char original_2 = test_map.get<2>();
  double original_3 = test_map.get<3>();
  float original_4 = test_map.get<4>();

  test_map.filter<[]<smap::ItemKind ItemT>() {
    return ItemT::key % 2 == 0;
  }>();

  EXPECT_EQ(test_map.get<1>(), original_1);
  EXPECT_EQ(test_map.get<2>(), original_2);
  EXPECT_DOUBLE_EQ(test_map.get<3>(), original_3);
  EXPECT_FLOAT_EQ(test_map.get<4>(), original_4);
}

TEST_F(StaticMapFilterTest, FilterByValueType) {
  auto result = test_map.filter<[]<smap::ItemKind ItemT> {
    return std::is_integral_v<typename ItemT::val_t>;
  }>();

  EXPECT_TRUE(result.contains<1>());
  EXPECT_TRUE(result.contains<2>());
  EXPECT_FALSE(result.contains<3>());
  EXPECT_FALSE(result.contains<4>());

  EXPECT_EQ(result.get<1>(), 10);
  EXPECT_EQ(result.get<2>(), 'A');
}

class StaticMapFilterParamTest : public ::testing::TestWithParam<std::pair<int, int>> {
 protected:
  void SetUp() override {
    test_map = TestMap{10, 'A', 3.14, 2.71f};
  }

  TestMap test_map;  // NOLINT
};

struct ThresholdPredicate {
  int threshold;

  constexpr ThresholdPredicate(int threshold)
      : threshold(threshold) {}

  template <smap::ItemKind ItemT>
  constexpr bool operator()() const {
    return ItemT::key > threshold;
  }
};
