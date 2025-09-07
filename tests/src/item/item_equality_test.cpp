#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using CharItem = smap::Item<'a', int>;
using DoubleItem1 = smap::Item<1, double>;
using StringItemA = smap::Item<'a', std::string>;

class ItemEqualityTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ItemEqualityTest, EqualSameTypeSameKeySameValue) {
  IntItem1 item1(42);
  IntItem1 item2(42);

  EXPECT_TRUE(item1 == item2);
  EXPECT_TRUE(item2 == item1);
}

TEST_F(ItemEqualityTest, NotEqualSameTypeSameKeyDifferentValue) {
  IntItem1 item1(42);
  IntItem1 item2(24);

  EXPECT_FALSE(item1 == item2);
  EXPECT_FALSE(item2 == item1);
}

TEST_F(ItemEqualityTest, EqualConvertibleTypes) {
  IntItem1 int_item(42);
  smap::Item<1, double> double_item(42.0);
  smap::Item<1, long> long_item(42L);

  EXPECT_TRUE(int_item == double_item);
  EXPECT_TRUE(double_item == int_item);
  EXPECT_TRUE(int_item == long_item);
  EXPECT_TRUE(long_item == int_item);
}

TEST_F(ItemEqualityTest, NotEqualConvertibleTypesDifferentValues) {
  IntItem1 int_item(42);
  smap::Item<1, double> double_item(42.5);

  EXPECT_FALSE(int_item == double_item);
  EXPECT_FALSE(double_item == int_item);
}

TEST_F(ItemEqualityTest, NotEqualDifferentKeys) {
  IntItem1 item1(42);
  IntItem2 item2(42);

  EXPECT_FALSE(item1 == item2);
  EXPECT_FALSE(item2 == item1);
}

TEST_F(ItemEqualityTest, NotEqualDifferentKeyTypes) {
  IntItem1 int_item(42);
  CharItem char_item(42);

  EXPECT_FALSE(int_item == char_item);
  EXPECT_FALSE(char_item == int_item);
}

TEST_F(ItemEqualityTest, NotEqualIncompatibleTypes) {
  IntItem1 int_item(42);
  StringItemA string_item("42");

  EXPECT_FALSE(int_item == string_item);
  EXPECT_FALSE(string_item == int_item);
}

TEST_F(ItemEqualityTest, StringEquality) {
  StringItemA item1("hello");
  StringItemA item2("hello");
  StringItemA item3("world");

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
  EXPECT_FALSE(item2 == item3);
}

TEST_F(ItemEqualityTest, StringEqualityWithConvertibleTypes) {
  StringItemA string_item("hello");
  smap::Item<'a', const char*> char_ptr_item("hello");
  smap::Item<'a', const char*> different_char_ptr_item("world");

  EXPECT_TRUE(string_item == char_ptr_item);
  EXPECT_TRUE(char_ptr_item == string_item);

  EXPECT_FALSE(string_item == different_char_ptr_item);
}

TEST_F(ItemEqualityTest, SelfEquality) {
  IntItem1 item(42);

  EXPECT_TRUE(item == item);
}

TEST_F(ItemEqualityTest, ZeroValuesEquality) {
  IntItem1 item1(0);
  IntItem1 item2(0);
  IntItem1 item3(1);

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
}

TEST_F(ItemEqualityTest, NegativeValuesEquality) {
  smap::Item<1, int> item1(-42);
  smap::Item<1, int> item2(-42);
  smap::Item<1, int> item3(-24);

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
}

TEST_F(ItemEqualityTest, FloatingPointEquality) {
  smap::Item<1, double> item1(3.14159);  // NOLINT
  smap::Item<1, double> item2(3.14159);  // NOLINT
  smap::Item<1, double> item3(3.14160);  // NOLINT

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
}

TEST_F(ItemEqualityTest, BooleanValuesEquality) {
  using BoolItem = smap::Item<1, bool>;
  BoolItem item1(true);
  BoolItem item2(true);
  BoolItem item3(false);

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
  EXPECT_FALSE(item2 == item3);
}

TEST_F(ItemEqualityTest, ComplexTypeEquality) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  VectorItem item1(std::vector<int>{1, 2, 3});
  VectorItem item2(std::vector<int>{1, 2, 3});
  VectorItem item3(std::vector<int>{1, 2, 4});

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
  EXPECT_FALSE(item2 == item3);
}

TEST_F(ItemEqualityTest, EmptyContainerEquality) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  VectorItem item1(std::vector<int>{});
  VectorItem item2(std::vector<int>{});
  VectorItem item3(std::vector<int>{1});

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
}

TEST_F(ItemEqualityTest, PerformanceTest) {
  IntItem1 item1(42);
  IntItem1 item2(42);
  IntItem1 item3(24);

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] bool result1 = (item1 == item2);
    [[maybe_unused]] bool result2 = (item1 == item3);
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be fast
}

// TEST_F(ItemEqualityTest, CompileTimeEvaluation) {
//   constexpr IntItem1 item1(42);
//   constexpr IntItem1 item2(42);
//   constexpr IntItem1 item3(24);

//   static_assert(item1 == item2);
//   static_assert(!(item1 == item3));
// }

TEST_F(ItemEqualityTest, LargeStringEquality) {
  std::string large_string1(10000, 'x');
  std::string large_string2(10000, 'x');
  std::string large_string3(10000, 'y');

  StringItemA item1(large_string1);
  StringItemA item2(large_string2);
  StringItemA item3(large_string3);

  EXPECT_TRUE(item1 == item2);
  EXPECT_FALSE(item1 == item3);
}

TEST_F(ItemEqualityTest, ReturnValueType) {
  IntItem1 item1(42);
  IntItem1 item2(42);

  auto result = (item1 == item2);

  static_assert(std::is_same_v<decltype(result), bool>);
  EXPECT_TRUE(result);
}

TEST_F(ItemEqualityTest, ConstCorrectness) {
  const IntItem1 const_item1(42);
  const IntItem1 const_item2(42);
  const IntItem1 const_item3(24);
  IntItem1 mutable_item(42);

  EXPECT_TRUE(const_item1 == const_item2);
  EXPECT_FALSE(const_item1 == const_item3);
  EXPECT_TRUE(const_item1 == mutable_item);
  EXPECT_TRUE(mutable_item == const_item1);
}

TEST_F(ItemEqualityTest, EdgeCases) {
  // Test with maximum and minimum values
  smap::Item<1, int> max_item(std::numeric_limits<int>::max());
  smap::Item<1, int> min_item(std::numeric_limits<int>::min());
  smap::Item<1, int> zero_item(0);

  EXPECT_FALSE(max_item == min_item);
  EXPECT_FALSE(max_item == zero_item);
  EXPECT_FALSE(min_item == zero_item);

  // Self-comparison should always be true
  EXPECT_TRUE(max_item == max_item);
  EXPECT_TRUE(min_item == min_item);
  EXPECT_TRUE(zero_item == zero_item);
}
