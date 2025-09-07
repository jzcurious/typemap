#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using CharItem = smap::Item<'a', std::string>;

class StaticMapContainsTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapContainsTest, ContainsSingleKey) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_TRUE(map.template contains<1>());
  EXPECT_TRUE(map.template contains<2>());
  EXPECT_TRUE(map.template contains<3>());
  EXPECT_FALSE(map.template contains<4>());
  EXPECT_FALSE(map.template contains<0>());
}

TEST_F(StaticMapContainsTest, ContainsSingleKeyChar) {
  smap::StaticMap<CharItem> map(CharItem("test"));

  EXPECT_TRUE(map.template contains<'a'>());
  EXPECT_FALSE(map.template contains<'b'>());
}

TEST_F(StaticMapContainsTest, ContainsEmptyMap) {
  smap::StaticMap<> map;

  EXPECT_FALSE(map.template contains<1>());
  EXPECT_FALSE(map.template contains<0>());
  EXPECT_FALSE(map.template contains<'a'>());
}

TEST_F(StaticMapContainsTest, ContainsSingleItemMap) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  EXPECT_TRUE(map.template contains<1>());
  EXPECT_FALSE(map.template contains<2>());
  EXPECT_FALSE(map.template contains<0>());
}

TEST_F(StaticMapContainsTest, ContainsAllBasic) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_TRUE((map.template contains_all<1>()));
  EXPECT_TRUE((map.template contains_all<1, 2>()));
  EXPECT_TRUE((map.template contains_all<1, 2, 3>()));
  EXPECT_FALSE((map.template contains_all<1, 4>()));
  EXPECT_FALSE((map.template contains_all<4, 5>()));
  EXPECT_FALSE((map.template contains_all<1, 2, 4>()));
}

TEST_F(StaticMapContainsTest, ContainsAllEmptyKeys) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_TRUE((map.template contains_all<>()));
}

TEST_F(StaticMapContainsTest, ContainsAllSingleKey) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_TRUE((map.template contains_all<1>()));
  EXPECT_FALSE((map.template contains_all<3>()));
}

TEST_F(StaticMapContainsTest, ContainsAllMixedExistence) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_FALSE((map.template contains_all<1, 3>()));
  EXPECT_FALSE((map.template contains_all<3, 1>()));
  EXPECT_FALSE((map.template contains_all<0, 1, 2>()));
}

TEST_F(StaticMapContainsTest, ContainsAnyBasic) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  EXPECT_TRUE((map.template contains_any<1>()));
  EXPECT_TRUE((map.template contains_any<1, 2>()));
  EXPECT_TRUE((map.template contains_any<1, 2, 3>()));
  EXPECT_TRUE((map.template contains_any<1, 4>()));
  EXPECT_TRUE((map.template contains_any<4, 1>()));
  EXPECT_FALSE((map.template contains_any<4, 5>()));
  EXPECT_FALSE((map.template contains_any<0, 4, 5>()));
}

TEST_F(StaticMapContainsTest, ContainsAnyEmptyKeys) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_FALSE((map.template contains_any<>()));
}

TEST_F(StaticMapContainsTest, ContainsAnySingleKey) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_TRUE((map.template contains_any<1>()));
  EXPECT_FALSE((map.template contains_any<3>()));
}

TEST_F(StaticMapContainsTest, ContainsAnyWithDuplicates) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_TRUE((map.template contains_any<1, 1, 1>()));
  EXPECT_TRUE((map.template contains_any<1, 3, 3>()));
  EXPECT_FALSE((map.template contains_any<3, 4, 5>()));
}

TEST_F(StaticMapContainsTest, ContainsMixedKeyTypes) {
  smap::StaticMap<IntItem1, CharItem> map(IntItem1(10), CharItem("test"));

  EXPECT_TRUE(map.template contains<1>());
  EXPECT_TRUE(map.template contains<'a'>());
  EXPECT_FALSE(map.template contains<2>());
  EXPECT_FALSE(map.template contains<'b'>());

  EXPECT_TRUE((map.template contains_all<1, 'a'>()));
  EXPECT_FALSE((map.template contains_all<1, 'b'>()));
  EXPECT_TRUE((map.template contains_any<1, 'b'>()));
  EXPECT_FALSE((map.template contains_any<2, 'b'>()));
}

// TEST_F(StaticMapContainsTest, ContainsCompileTimeEvaluation) {
//   constexpr smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20)); //
//   TODO: implement constexpr constructor

//   static_assert(map.template contains<1>());
//   static_assert(map.template contains<2>());
//   static_assert(!map.template contains<3>());

//   static_assert((map.template contains_all<1, 2>()));
//   static_assert(!(map.template contains_all<1, 3>()));

//   static_assert((map.template contains_any<1, 3>()));
//   static_assert(!(map.template contains_any<3, 4>()));
// }

TEST_F(StaticMapContainsTest, ContainsPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] bool result1 = map.template contains<1>();
    [[maybe_unused]] bool result2 = map.template contains<4>();
    [[maybe_unused]] bool result3 = (map.template contains_all<1, 2>());
    [[maybe_unused]] bool result4 = (map.template contains_any<3, 4>());
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be very fast
}

TEST_F(StaticMapContainsTest, ContainsEdgeCases) {
  smap::StaticMap<IntItem1> single_map(IntItem1(42));

  EXPECT_TRUE(single_map.template contains<1>());
  EXPECT_FALSE(single_map.template contains<0>());
  EXPECT_FALSE(single_map.template contains<-1>());

  EXPECT_TRUE((single_map.template contains_all<1>()));
  EXPECT_FALSE((single_map.template contains_all<1, 2>()));

  EXPECT_TRUE((single_map.template contains_any<1>()));
  EXPECT_TRUE((single_map.template contains_any<1, 2>()));
  EXPECT_FALSE((single_map.template contains_any<2, 3>()));
}

TEST_F(StaticMapContainsTest, ContainsZeroAndNegativeKeys) {
  using ZeroItem = smap::Item<0, int>;
  using NegativeItem = smap::Item<-1, int>;

  smap::StaticMap<ZeroItem, NegativeItem> map(ZeroItem(0), NegativeItem(-10));

  EXPECT_TRUE(map.template contains<0>());
  EXPECT_TRUE(map.template contains<-1>());
  EXPECT_FALSE(map.template contains<1>());
  EXPECT_FALSE(map.template contains<-2>());
}

TEST_F(StaticMapContainsTest, ContainsAllVariadicEdgeCases) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  // Test with repeated keys
  EXPECT_TRUE((map.template contains_all<1, 1, 1>()));

  // Test with many keys (all existing)
  EXPECT_TRUE((map.template contains_all<1, 2, 3, 1, 2, 3>()));

  // Test with many keys (some missing)
  EXPECT_FALSE((map.template contains_all<1, 2, 3, 4, 5>()));
}

TEST_F(StaticMapContainsTest, ContainsAnyVariadicEdgeCases) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  // Test with repeated keys
  EXPECT_TRUE((map.template contains_any<1, 1, 1>()));
  EXPECT_FALSE((map.template contains_any<3, 3, 3>()));

  // Test with many keys (some existing)
  EXPECT_TRUE((map.template contains_any<1, 3, 4, 5>()));

  // Test with many keys (none existing)
  EXPECT_FALSE((map.template contains_any<3, 4, 5, 6>()));
}
