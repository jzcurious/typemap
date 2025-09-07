#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using CharItem = smap::Item<'a', int>;
using DoubleItem1 = smap::Item<1, double>;
using StringItem1 = smap::Item<1, std::string>;

class ItemIsCompatibleTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ItemIsCompatibleTest, CompatibleSameTypeSameKey) {
  using Item1 = smap::Item<1, int>;
  using Item2 = smap::Item<1, int>;

  EXPECT_TRUE((Item1::template is_compatible<Item2>()));
  EXPECT_TRUE((Item2::template is_compatible<Item1>()));
}

TEST_F(ItemIsCompatibleTest, CompatibleConvertibleTypesSameKey) {
  EXPECT_TRUE((smap::Item<1, double>::template is_compatible<IntItem1>()));
  EXPECT_TRUE((smap::Item<1, long>::template is_compatible<IntItem1>()));
  EXPECT_TRUE((smap::Item<'a',
      std::string>::template is_compatible<smap::Item<'a', const char*>>()));
}

TEST_F(ItemIsCompatibleTest, IncompatibleDifferentKeys) {
  EXPECT_FALSE((IntItem1::template is_compatible<IntItem2>()));
  EXPECT_FALSE((IntItem2::template is_compatible<IntItem1>()));
  EXPECT_FALSE((IntItem1::template is_compatible<CharItem>()));
}

TEST_F(ItemIsCompatibleTest, IncompatibleDifferentKeyTypes) {
  using IntKeyItem = smap::Item<1, int>;
  using CharKeyItem = smap::Item<'a', int>;

  EXPECT_FALSE((IntKeyItem::template is_compatible<CharKeyItem>()));
  EXPECT_FALSE((CharKeyItem::template is_compatible<IntKeyItem>()));
}

TEST_F(ItemIsCompatibleTest, IncompatibleNonConvertibleTypes) {
  EXPECT_FALSE((IntItem1::template is_compatible<StringItem1>()));
  EXPECT_FALSE((StringItem1::template is_compatible<IntItem1>()));
}

TEST_F(ItemIsCompatibleTest, SelfCompatibility) {
  EXPECT_TRUE((IntItem1::template is_compatible<IntItem1>()));
  EXPECT_TRUE((StringItem1::template is_compatible<StringItem1>()));
  EXPECT_TRUE((CharItem::template is_compatible<CharItem>()));
}

TEST_F(ItemIsCompatibleTest, CompileTimeEvaluation) {
  static_assert(IntItem1::template is_compatible<IntItem1>());
  static_assert(smap::Item<1, double>::template is_compatible<IntItem1>());
  static_assert(!IntItem1::template is_compatible<IntItem2>());
  static_assert(!IntItem1::template is_compatible<StringItem1>());
}

TEST_F(ItemIsCompatibleTest, CompatibleWithBaseTypes) {
  using FloatItem = smap::Item<1, double>;
  using IntToFloatCompatible = smap::Item<1, float>;

  EXPECT_TRUE((FloatItem::template is_compatible<IntToFloatCompatible>()));
}

TEST_F(ItemIsCompatibleTest, CompatibleStringTypes) {
  using StringItem = smap::Item<1, std::string>;
  using CharPtrItem = smap::Item<1, const char*>;

  EXPECT_TRUE((StringItem::template is_compatible<CharPtrItem>()));
}

TEST_F(ItemIsCompatibleTest, EdgeCases) {
  using ZeroKeyItem = smap::Item<0, int>;
  using NegativeKeyItem = smap::Item<-1, int>;
  using LargeKeyItem = smap::Item<999999, int>;

  EXPECT_TRUE((ZeroKeyItem::template is_compatible<smap::Item<0, long>>()));
  EXPECT_TRUE((NegativeKeyItem::template is_compatible<smap::Item<-1, short>>()));
  EXPECT_TRUE((LargeKeyItem::template is_compatible<smap::Item<999999, long long>>()));

  EXPECT_FALSE((ZeroKeyItem::template is_compatible<NegativeKeyItem>()));
  EXPECT_FALSE((NegativeKeyItem::template is_compatible<LargeKeyItem>()));
}

TEST_F(ItemIsCompatibleTest, CompatibleTemplateSpecializations) {
  using ItemTemplate1 = smap::Item<1, int>;
  using ItemTemplate2 = smap::Item<1, long>;
  using ItemTemplate3 = smap::Item<2, int>;

  EXPECT_TRUE((ItemTemplate1::template is_compatible<ItemTemplate2>()));
  EXPECT_FALSE((ItemTemplate1::template is_compatible<ItemTemplate3>()));
}

TEST_F(ItemIsCompatibleTest, BooleanReturnValues) {
  auto compatible_result = IntItem1::template is_compatible<DoubleItem1>();
  auto incompatible_result = IntItem1::template is_compatible<IntItem2>();

  static_assert(std::is_same_v<decltype(compatible_result), bool>);
  static_assert(std::is_same_v<decltype(incompatible_result), bool>);

  EXPECT_TRUE(compatible_result);
  EXPECT_FALSE(incompatible_result);
}

TEST_F(ItemIsCompatibleTest, CompatibleConstAndVolatile) {
  using ConstIntItem = smap::Item<1, const int>;
  using VolatileIntItem = smap::Item<1, volatile int>;
  using RegularIntItem = smap::Item<1, int>;

  EXPECT_TRUE((RegularIntItem::template is_compatible<ConstIntItem>()));
  EXPECT_TRUE((RegularIntItem::template is_compatible<VolatileIntItem>()));
}

TEST_F(ItemIsCompatibleTest, PerformanceTest) {
  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] constexpr bool result1
        = IntItem1::template is_compatible<IntItem1>();
    [[maybe_unused]] constexpr bool result2
        = IntItem1::template is_compatible<IntItem2>();
    [[maybe_unused]] constexpr bool result3
        = IntItem1::template is_compatible<DoubleItem1>();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be extremely fast
}
