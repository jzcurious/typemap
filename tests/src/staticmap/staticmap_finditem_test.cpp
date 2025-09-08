#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using CharItem = smap::Item<'a', std::string>;

class StaticMapFindItemTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapFindItemTest, FindExistingKeys) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  constexpr auto result1 = map.template find_item<1>();
  constexpr auto result2 = map.template find_item<2>();
  constexpr auto result3 = map.template find_item<3>();

  EXPECT_TRUE(result1.found);
  EXPECT_TRUE(result2.found);
  EXPECT_TRUE(result3.found);

  EXPECT_EQ(result1.index, 0);
  EXPECT_EQ(result2.index, 1);
  EXPECT_EQ(result3.index, 2);
}

TEST_F(StaticMapFindItemTest, FindNonExistingKeys) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr auto result1 = map.template find_item<3>();
  constexpr auto result2 = map.template find_item<0>();
  constexpr auto result3 = map.template find_item<-1>();

  EXPECT_FALSE(result1.found);
  EXPECT_FALSE(result2.found);
  EXPECT_FALSE(result3.found);
}

TEST_F(StaticMapFindItemTest, FindInEmptyMap) {
  smap::StaticMap<> map;

  constexpr auto result = map.template find_item<1>();

  EXPECT_FALSE(result.found);
  EXPECT_EQ(result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindInSingleItemMap) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  constexpr auto found_result = map.template find_item<1>();
  constexpr auto not_found_result = map.template find_item<2>();

  EXPECT_TRUE(found_result.found);
  EXPECT_EQ(found_result.index, 0);

  EXPECT_FALSE(not_found_result.found);
  EXPECT_EQ(not_found_result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindCharKeys) {
  smap::StaticMap<CharItem> map(CharItem("test"));

  constexpr auto found_result = map.template find_item<'a'>();
  constexpr auto not_found_result = map.template find_item<'b'>();

  EXPECT_TRUE(found_result.found);
  EXPECT_EQ(found_result.index, 0);

  EXPECT_FALSE(not_found_result.found);
  EXPECT_EQ(not_found_result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindZeroAndNegativeKeys) {
  using ZeroItem = smap::Item<0, int>;
  using NegativeItem = smap::Item<-1, int>;

  smap::StaticMap<ZeroItem, NegativeItem> map(ZeroItem(100), NegativeItem(-200));

  constexpr auto zero_result = map.template find_item<0>();
  constexpr auto negative_result = map.template find_item<-1>();
  constexpr auto not_found_result = map.template find_item<1>();

  EXPECT_TRUE(zero_result.found);
  EXPECT_EQ(zero_result.index, 0);

  EXPECT_TRUE(negative_result.found);
  EXPECT_EQ(negative_result.index, 1);

  EXPECT_FALSE(not_found_result.found);
  EXPECT_EQ(not_found_result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindResultConversionToBool) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  constexpr auto found_result = map.template find_item<1>();
  constexpr auto not_found_result = map.template find_item<2>();

  EXPECT_TRUE(static_cast<bool>(found_result));
  EXPECT_FALSE(static_cast<bool>(not_found_result));
}

TEST_F(StaticMapFindItemTest, FindResultConversionToSizeT) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr auto result1 = map.template find_item<1>();
  constexpr auto result2 = map.template find_item<2>();

  EXPECT_EQ(static_cast<std::size_t>(result1), 0);
  EXPECT_EQ(static_cast<std::size_t>(result2), 1);
}

// TEST_F(StaticMapFindItemTest, FindCompileTimeEvaluation) {
//   constexpr smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

//   constexpr auto result1 = map.template find_item<1>();
//   constexpr auto result2 = map.template find_item<3>();

//   static_assert(result1.found);
//   static_assert(result1.index == 0);

//   static_assert(!result2.found);
//   static_assert(result2.index == 0);
// }

// TEST_F(StaticMapFindItemTest, FindResultStructMembers) {
//   smap::StaticMap<IntItem1> map(IntItem1(42));

//   constexpr auto result = map.template find_item<1>();

//   EXPECT_TRUE(result.found);
//   EXPECT_EQ(result.index, 0);

//   // Test that the struct has the expected members
//   static_assert(std::same_as<decltype(result.found), const bool>);
//   static_assert(std::same_as<decltype(result.index), const std::size_t>);
// }

TEST_F(StaticMapFindItemTest, FindSameKeyDifferentMaps) {
  smap::StaticMap<IntItem1, IntItem2> map1(IntItem1(10), IntItem2(20));

  smap::StaticMap<IntItem2, IntItem3> map2(IntItem2(200), IntItem3(300));

  constexpr auto map1_result = map1.template find_item<1>();
  constexpr auto map2_result = map2.template find_item<1>();

  EXPECT_TRUE(map1_result.found);
  EXPECT_EQ(map1_result.index, 0);

  EXPECT_FALSE(map2_result.found);
  EXPECT_EQ(map2_result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] constexpr auto result1 = map.template find_item<1>();
    [[maybe_unused]] constexpr auto result2 = map.template find_item<4>();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be extremely fast
}

TEST_F(StaticMapFindItemTest, FindMixedKeyTypes) {
  smap::StaticMap<IntItem1, CharItem> map(IntItem1(10), CharItem("test"));

  constexpr auto int_result = map.template find_item<1>();
  constexpr auto char_result = map.template find_item<'a'>();
  constexpr auto not_found_result = map.template find_item<2>();

  EXPECT_TRUE(int_result.found);
  EXPECT_EQ(int_result.index, 0);

  EXPECT_TRUE(char_result.found);
  EXPECT_EQ(char_result.index, 1);

  EXPECT_FALSE(not_found_result.found);
  EXPECT_EQ(not_found_result.index, 0);
}

TEST_F(StaticMapFindItemTest, FindResultEquality) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  constexpr auto result1 = map.template find_item<1>();
  constexpr auto result2 = map.template find_item<1>();
  constexpr auto result3 = map.template find_item<2>();

  EXPECT_EQ(result1.found, result2.found);
  EXPECT_EQ(result1.index, result2.index);

  EXPECT_NE(result1.found, result3.found);
}

TEST_F(StaticMapFindItemTest, FindLargeMap) {
  using Item1 = smap::Item<1, int>;
  using Item2 = smap::Item<2, int>;
  using Item3 = smap::Item<3, int>;
  using Item4 = smap::Item<4, int>;
  using Item5 = smap::Item<5, int>;

  smap::StaticMap<Item1, Item2, Item3, Item4, Item5> map(
      Item1(10), Item2(20), Item3(30), Item4(40), Item5(50));

  constexpr auto result1 = map.template find_item<1>();
  constexpr auto result3 = map.template find_item<3>();
  constexpr auto result5 = map.template find_item<5>();
  constexpr auto result6 = map.template find_item<6>();

  EXPECT_TRUE(result1.found);
  EXPECT_EQ(result1.index, 0);

  EXPECT_TRUE(result3.found);
  EXPECT_EQ(result3.index, 2);

  EXPECT_TRUE(result5.found);
  EXPECT_EQ(result5.index, 4);

  EXPECT_FALSE(result6.found);
  EXPECT_EQ(result6.index, 0);
}

TEST_F(StaticMapFindItemTest, FindResultTypeTraits) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  constexpr auto result = map.template find_item<1>();

  // Test that find_result_t has the expected properties
  static_assert(std::is_standard_layout_v<decltype(result)>);
  static_assert(std::is_trivially_copyable_v<decltype(result)>);
}
