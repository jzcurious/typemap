#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapAtTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapAtTest, AtBasicAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapAtTest, AtConstAccess) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
}

TEST_F(StaticMapAtTest, AtModifyValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  map.template at<1>() = 100;
  map.template at<2>() = 200;

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
}

TEST_F(StaticMapAtTest, AtReferenceSemantics) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  auto& ref = map.template at<1>();
  ref = 42;

  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapAtTest, AtConstReferenceSemantics) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  const auto& ref = map.template at<1>();
  EXPECT_EQ(ref, 10);
}

TEST_F(StaticMapAtTest, AtStringValues) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  EXPECT_EQ(map.template at<'a'>(), "hello");

  map.template at<'a'>() += " world";
  EXPECT_EQ(map.template at<'a'>(), "hello world");
}

TEST_F(StaticMapAtTest, AtMixedTypes) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  EXPECT_EQ(map.template at<1>(), 42);
  EXPECT_EQ(map.template at<'a'>(), "test");

  map.template at<1>() = 100;
  map.template at<'a'>() = "modified";

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<'a'>(), "modified");
}

TEST_F(StaticMapAtTest, AtSingleItem) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  EXPECT_EQ(map.template at<1>(), 42);
  map.template at<1>() = 100;
  EXPECT_EQ(map.template at<1>(), 100);
}

// TEST_F(StaticMapAtTest, AtCompileTimeEvaluation) {
//   constexpr smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

//   static_assert(map.template at<1>() == 10);
//   static_assert(map.template at<2>() == 20);
// }

TEST_F(StaticMapAtTest, AtNotFoundWithoutIgnore) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  // This should compile successfully for existing keys
  EXPECT_NO_THROW(map.template at<1>());
  EXPECT_NO_THROW(map.template at<2>());

  // This should cause compile-time error for non-existing keys
  // map.template at<3>(); // Should not compile
}

TEST_F(StaticMapAtTest, AtPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] auto& val1 = map.template at<1>();
    [[maybe_unused]] auto& val2 = map.template at<2>();
    [[maybe_unused]] auto& val3 = map.template at<3>();
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be very fast
}

TEST_F(StaticMapAtTest, AtWithZeroAndNegativeKeys) {
  using ZeroItem = smap::Item<0, int>;
  using NegativeItem = smap::Item<-1, int>;

  smap::StaticMap<ZeroItem, NegativeItem> map(ZeroItem(100), NegativeItem(-200));

  EXPECT_EQ(map.template at<0>(), 100);
  EXPECT_EQ(map.template at<-1>(), -200);

  map.template at<0>() = 0;
  map.template at<-1>() = -1;

  EXPECT_EQ(map.template at<0>(), 0);
  EXPECT_EQ(map.template at<-1>(), -1);
}

TEST_F(StaticMapAtTest, AtTypeDeduction) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  auto& int_ref = map.template at<1>();
  auto& string_ref = map.template at<'a'>();

  static_assert(std::same_as<decltype(int_ref), int&>);
  static_assert(std::same_as<decltype(string_ref), std::string&>);

  EXPECT_EQ(int_ref, 42);
  EXPECT_EQ(string_ref, "hello");
}

TEST_F(StaticMapAtTest, AtConstTypeDeduction) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("hello"));

  const auto& int_ref = map.template at<1>();
  const auto& string_ref = map.template at<'a'>();

  static_assert(std::same_as<decltype(int_ref), const int&>);
  static_assert(std::same_as<decltype(string_ref), const std::string&>);

  EXPECT_EQ(int_ref, 42);
  EXPECT_EQ(string_ref, "hello");
}

TEST_F(StaticMapAtTest, AtMultipleAccesses) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  // Access in different orders
  EXPECT_EQ(map.template at<3>(), 30);
  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);

  // Modify and verify
  map.template at<2>() *= 2;
  EXPECT_EQ(map.template at<2>(), 40);
}

TEST_F(StaticMapAtTest, AtChainedOperations) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  map.template at<'a'>() += " world";
  map.template at<'a'>() += "!";

  EXPECT_EQ(map.template at<'a'>(), "hello world!");
}

TEST_F(StaticMapAtTest, AtLargeMap) {
  using Item1 = smap::Item<1, int>;
  using Item2 = smap::Item<2, int>;
  using Item3 = smap::Item<3, int>;
  using Item4 = smap::Item<4, int>;
  using Item5 = smap::Item<5, int>;

  smap::StaticMap<Item1, Item2, Item3, Item4, Item5> map(
      Item1(10), Item2(20), Item3(30), Item4(40), Item5(50));

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 20);
  EXPECT_EQ(map.template at<3>(), 30);
  EXPECT_EQ(map.template at<4>(), 40);
  EXPECT_EQ(map.template at<5>(), 50);
}
