#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using StringItem = smap::Item<'a', std::string>;

class StaticMapValsTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapValsTest, BasicValsAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  auto vals = map.vals();

  EXPECT_EQ(std::get<0>(vals), 10);
  EXPECT_EQ(std::get<1>(vals), 20);
}

TEST_F(StaticMapValsTest, ModifyValuesThroughVals) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto vals = map.vals();
  std::get<0>(vals) = 100;
  std::get<1>(vals) = 200;

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
}

TEST_F(StaticMapValsTest, ConstValsAccess) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(42), IntItem2(24));

  auto vals = map.vals();

  EXPECT_EQ(std::get<0>(vals), 42);
  EXPECT_EQ(std::get<1>(vals), 24);
}

TEST_F(StaticMapValsTest, MixedTypesVals) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(100), StringItem("hello"));

  auto vals = map.vals();

  EXPECT_EQ(std::get<0>(vals), 100);
  EXPECT_EQ(std::get<1>(vals), "hello");
}

TEST_F(StaticMapValsTest, SingleItemVals) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  auto vals = map.vals();

  EXPECT_EQ(std::get<0>(vals), 42);
}

TEST_F(StaticMapValsTest, EmptyMapVals) {
  smap::StaticMap<> map;

  auto vals = map.vals();

  // Should compile and return empty tuple
  EXPECT_EQ(std::tuple_size_v<decltype(vals)>, 0);
}

TEST_F(StaticMapValsTest, StringManipulationThroughVals) {
  smap::StaticMap<StringItem> map(StringItem("hello"));

  auto vals = map.vals();
  std::get<0>(vals) += " world";

  EXPECT_EQ(map.template at<'a'>(), "hello world");
}

TEST_F(StaticMapValsTest, MultipleModifications) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto vals = map.vals();
  std::get<0>(vals) *= 10;
  std::get<1>(vals) *= 20;
  std::get<2>(vals) *= 30;

  EXPECT_EQ(map.template at<1>(), 10);
  EXPECT_EQ(map.template at<2>(), 40);
  EXPECT_EQ(map.template at<3>(), 90);
}

TEST_F(StaticMapValsTest, StructuredBinding) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(100), IntItem2(200));

  auto [val1, val2] = map.vals();

  EXPECT_EQ(val1, 100);
  EXPECT_EQ(val2, 200);
}

TEST_F(StaticMapValsTest, ConstStructuredBinding) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(100), IntItem2(200));

  auto [val1, val2] = map.vals();

  EXPECT_EQ(val1, 100);
  EXPECT_EQ(val2, 200);
}

TEST_F(StaticMapValsTest, ReferenceSemantics) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto vals = map.vals();
  std::get<0>(vals) = 42;

  // Verify that the original map is modified
  EXPECT_EQ(map.template at<1>(), 42);
}

TEST_F(StaticMapValsTest, ConstReferenceSemantics) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto vals = map.vals();

  // Should be able to read values
  EXPECT_EQ(std::get<0>(vals), 1);
  EXPECT_EQ(std::get<1>(vals), 2);

  // Should not be able to modify through const vals
  // std::get<0>(vals) = 42; // This should not compile
}

TEST_F(StaticMapValsTest, TupleSize) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto vals = map.vals();

  EXPECT_EQ(std::tuple_size_v<decltype(vals)>, 3);
}

TEST_F(StaticMapValsTest, TupleElementTypes) {
  smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  auto vals = map.vals();

  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(vals)>, int&>);
  static_assert(std::is_same_v<std::tuple_element_t<1, decltype(vals)>, std::string&>);
}

TEST_F(StaticMapValsTest, ConstTupleElementTypes) {
  const smap::StaticMap<IntItem1, StringItem> map(IntItem1(42), StringItem("test"));

  auto vals = map.vals();

  static_assert(std::is_same_v<std::tuple_element_t<0, decltype(vals)>, const int&>);
  static_assert(
      std::is_same_v<std::tuple_element_t<1, decltype(vals)>, const std::string&>);
}

TEST_F(StaticMapValsTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(1), IntItem2(2), IntItem3(3));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000; ++i) {
    auto vals = map.vals();
    std::get<0>(vals) += 1;
    std::get<1>(vals) += 1;
    std::get<2>(vals) += 1;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 50000);  // Should be fast

  EXPECT_EQ(map.template at<1>(), 10001);
  EXPECT_EQ(map.template at<2>(), 10002);
  EXPECT_EQ(map.template at<3>(), 10003);
}

TEST_F(StaticMapValsTest, UseWithStandardAlgorithms) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  auto vals = map.vals();

  int sum = 0;
  std::apply([&sum](auto&... values) { ((sum += values), ...); }, vals);

  EXPECT_EQ(sum, 60);
}

TEST_F(StaticMapValsTest, AssignmentThroughTuple) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(1), IntItem2(2));

  auto vals = map.vals();
  vals = std::make_tuple(100, 200);

  EXPECT_EQ(map.template at<1>(), 100);
  EXPECT_EQ(map.template at<2>(), 200);
}
