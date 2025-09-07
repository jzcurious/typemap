#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapSetTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapSetTest, SetBasicValues) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  map.template set<1>(100);
  map.template set<2>(200);

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
}

TEST_F(StaticMapSetTest, SetStringValues) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  map.template set<'a'>("world");
  EXPECT_EQ(map.template at<'a'>(), "world");

  map.template set<'a'>("new value");
  EXPECT_EQ(map.template at<'a'>(), "new value");
}

TEST_F(StaticMapSetTest, SetMixedTypes) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  map.template set<1>(100);
  map.template set<'a'>("modified");

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<'a'>(), "modified");
}

TEST_F(StaticMapSetTest, SetWithMoveSemantics) {
  smap::StaticMap<StringItem> map(StringItem("original"));

  std::string new_value = "moved_value";
  map.template set<'a'>(std::move(new_value));

  EXPECT_EQ(map.template at<'a'>(), "moved_value");
}

TEST_F(StaticMapSetTest, SetSingleItemMap) {
  smap::StaticMap<IntItem1> map(IntItem1(1));

  map.template set<1>(42);
  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapSetTest, SetConvertibleTypes) {
  smap::StaticMap<smap::Item<1, double>> map(smap::Item<1, double>(1.5));

  map.template set<1>(42);  // int to double conversion
  EXPECT_DOUBLE_EQ(map.template at<1>(), 42.0);
}

TEST_F(StaticMapSetTest, SetPerformance) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    map.template set<1>(i);
    map.template set<2>(i * 2);
    map.template set<3>(i * 3);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast

  EXPECT_EQ(map.template at<1>(), 99999);
  EXPECT_EQ(map.template at<2>(), 99999 * 2);
  EXPECT_EQ(map.template at<3>(), 99999 * 3);
}

TEST_F(StaticMapSetTest, SetZeroAndNegativeKeys) {
  using ZeroItem = smap::Item<0, int>;
  using NegativeItem = smap::Item<-1, int>;

  smap::StaticMap<ZeroItem, NegativeItem> map(ZeroItem(100), NegativeItem(-200));

  map.template set<0>(0);
  map.template set<-1>(-1);

  EXPECT_EQ(map.template at<0>(), 0);
  EXPECT_EQ(map.template at<-1>(), -1);
}

TEST_F(StaticMapSetTest, SetChainingOperations) {
  smap::StaticMap<StringItem> map(StringItem("hello"));
  map.template set<'a'>("world");
  map.template set<'a'>("chained");

  EXPECT_EQ(map.template at<'a'>(), "chained");
}

TEST_F(StaticMapSetTest, SetLargeValues) {
  smap::StaticMap<StringItem> map(StringItem(""));

  std::string large_string(1000, 'x');
  map.template set<'a'>(large_string);

  EXPECT_EQ(map.template at<'a'>(), large_string);
}

// TEST_F(StaticMapSetTest, SetMoveOnlyTypes) { // TODO: study the case
//   using UniquePtrItem = smap::Item<1, std::unique_ptr<int>>;
//   smap::StaticMap<UniquePtrItem> map(UniquePtrItem(std::make_unique<int>(42)));

//   auto new_ptr = std::make_unique<int>(100);
//   map.template set<1>(std::move(new_ptr));

//   EXPECT_NE(map.template at<1>(), nullptr);
//   EXPECT_EQ(*map.template at<1>(), 100);
// }

// TEST_F(StaticMapSetTest, SetConstexpr) {
//   constexpr smap::StaticMap<IntItem1> map(IntItem1(1));

//   // Note: constexpr set operations work on non-const maps
//   auto mutable_map = map;
//   mutable_map.template set<1>(42);
//   EXPECT_EQ(mutable_map.template at<1>(), 42);
// }

TEST_F(StaticMapSetTest, SetWithRValueReferences) {
  smap::StaticMap<StringItem> map(StringItem("test"));

  map.template set<'a'>(std::string("rvalue"));
  EXPECT_EQ(map.template at<'a'>(), "rvalue");
}

TEST_F(StaticMapSetTest, SetMultipleTimes) {
  smap::StaticMap<IntItem1> map(IntItem1(0));

  for (int i = 0; i < 100; ++i) {
    map.template set<1>(i);
  }

  EXPECT_EQ(map.template at<1>(), 99);
}

TEST_F(StaticMapSetTest, SetBooleanValues) {
  using BoolItem = smap::Item<1, bool>;
  smap::StaticMap<BoolItem> map(BoolItem(false));

  map.template set<1>(true);
  EXPECT_EQ(map.template at<1>(), true);

  map.template set<1>(false);
  EXPECT_EQ(map.template at<1>(), false);
}

TEST_F(StaticMapSetTest, SetEmptyString) {
  smap::StaticMap<StringItem> map(StringItem("non-empty"));

  map.template set<'a'>("");
  EXPECT_EQ(map.template at<'a'>(), "");
}

TEST_F(StaticMapSetTest, SetSameValue) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  map.template set<1>(42);  // Set same value
  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapSetTest, SetWithCopyAssignment) {
  smap::StaticMap<StringItem> map(StringItem("original"));

  std::string copy_source = "copied";
  map.template set<'a'>(copy_source);

  EXPECT_EQ(map.template at<'a'>(), "copied");
}

TEST_F(StaticMapSetTest, SetTypeConversion) {
  using DoubleItem = smap::Item<1, double>;
  smap::StaticMap<DoubleItem> map(DoubleItem(1.5));

  map.template set<1>(42);  // int to double
  EXPECT_DOUBLE_EQ(map.template at<1>(), 42.0);

  map.template set<1>(3.14f);  // float to double
  EXPECT_FLOAT_EQ(static_cast<float>(map.template at<1>()), 3.14f);
}
