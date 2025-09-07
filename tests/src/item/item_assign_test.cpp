#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using CharItem = smap::Item<'a', int>;
using DoubleItem1 = smap::Item<1, double>;
using StringItemA = smap::Item<'a', std::string>;

class ItemAssignmentTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ItemAssignmentTest, AssignCompatibleItem) {
  IntItem1 item1(10);
  IntItem1 item2(20);

  item1 = item2;

  EXPECT_EQ(item1.val, 20);
  EXPECT_EQ(item1.key, 1);
  EXPECT_EQ(item2.val, 20);
}

TEST_F(ItemAssignmentTest, AssignConvertibleTypes) {
  smap::Item<1, double> double_item(1.5);
  IntItem1 int_item(42);

  double_item = int_item;

  EXPECT_DOUBLE_EQ(double_item.val, 42.0);
  EXPECT_EQ(double_item.key, 1);
}

TEST_F(ItemAssignmentTest, AssignStringTypes) {
  StringItemA string_item("original");
  smap::Item<'a', const char*> char_ptr_item("assigned");

  string_item = char_ptr_item;

  EXPECT_EQ(string_item.val, "assigned");
  EXPECT_EQ(string_item.key, 'a');
}

TEST_F(ItemAssignmentTest, AssignWithMoveSemantics) {
  StringItemA string_item("original");
  smap::Item<'a', std::string> other_item("moved");

  string_item = std::move(other_item);

  EXPECT_EQ(string_item.val, "moved");
  EXPECT_EQ(string_item.key, 'a');
}

TEST_F(ItemAssignmentTest, ChainAssignment) {
  IntItem1 item1(10);
  IntItem1 item2(20);
  IntItem1 item3(30);

  item1 = item2 = item3;

  EXPECT_EQ(item1.val, 30);
  EXPECT_EQ(item2.val, 30);
  EXPECT_EQ(item3.val, 30);
}

TEST_F(ItemAssignmentTest, AssignDifferentValueSameKey) {
  IntItem1 item1(1);
  IntItem1 item2(100);

  item1 = item2;

  EXPECT_EQ(item1.val, 100);
  EXPECT_EQ(item1.key, 1);
}

TEST_F(ItemAssignmentTest, PerformanceTest) {
  IntItem1 item(0);
  IntItem1 source(42);

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    item = source;
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be fast
  EXPECT_EQ(item.val, 42);
}

// TEST_F(ItemAssignmentTest, AssignConstexpr) {
//   constexpr IntItem1 item1(10);
//   constexpr IntItem1 item2(20);

//   // Note: constexpr assignment works on non-const items
//   auto mutable_item1 = item1;
//   auto mutable_item2 = item2;

//   mutable_item1 = mutable_item2;

//   EXPECT_EQ(mutable_item1.val, 20);
// }

TEST_F(ItemAssignmentTest, AssignComplexTypes) {
  using VectorItem = smap::Item<1, std::vector<int>>;
  VectorItem vector_item1(std::vector<int>{1, 2, 3});
  VectorItem vector_item2(std::vector<int>{4, 5, 6, 7});

  vector_item1 = vector_item2;

  EXPECT_EQ(vector_item1.val.size(), 4);
  EXPECT_EQ(vector_item1.val[0], 4);
  EXPECT_EQ(vector_item1.val[3], 7);
}

TEST_F(ItemAssignmentTest, AssignMoveOnlyTypes) {
  using UniquePtrItem = smap::Item<1, std::unique_ptr<int>>;
  UniquePtrItem item1(std::make_unique<int>(42));
  UniquePtrItem item2(std::make_unique<int>(100));

  EXPECT_NE(item1.val, nullptr);
  EXPECT_EQ(*item1.val, 42);

  item1 = std::move(item2);

  EXPECT_NE(item1.val, nullptr);
  EXPECT_EQ(*item1.val, 100);
}

TEST_F(ItemAssignmentTest, AssignLargeStrings) {
  StringItemA item1("short");
  StringItemA item2(std::string(10000, 'x'));

  item1 = item2;

  EXPECT_EQ(item1.val.size(), 10000);
  EXPECT_EQ(item1.val[0], 'x');
  EXPECT_EQ(item1.val[9999], 'x');
}

TEST_F(ItemAssignmentTest, AssignmentReturnReference) {
  IntItem1 item1(10);
  IntItem1 item2(20);

  IntItem1& returned_ref = (item1 = item2);

  EXPECT_EQ(&returned_ref, &item1);
  EXPECT_EQ(item1.val, 20);
}

TEST_F(ItemAssignmentTest, AssignmentWithTemporary) {
  IntItem1 item(10);

  item = IntItem1(42);

  EXPECT_EQ(item.val, 42);
}

TEST_F(ItemAssignmentTest, CompileTimeRequirements) {
  // These should compile because they are compatible
  static_assert(IntItem1::is_compatible<IntItem1>());
  static_assert(smap::Item<1, double>::is_compatible<IntItem1>());

  // These should not compile due to incompatible constraints
  // IntItem1 item1(10);
  // IntItem2 item2(20);
  // item1 = item2; // Should not compile - different keys
}

TEST_F(ItemAssignmentTest, AssignmentDoesNotChangeKey) {
  IntItem1 item1(10);
  IntItem1 item2(20);

  auto original_key = item1.key;
  item1 = item2;

  EXPECT_EQ(item1.key, original_key);
  EXPECT_EQ(item1.key, 1);
  EXPECT_EQ(item1.val, 20);
}

TEST_F(ItemAssignmentTest, AssignmentWithZeroValues) {
  IntItem1 item1(42);
  IntItem1 item2(0);

  item1 = item2;

  EXPECT_EQ(item1.val, 0);
}

TEST_F(ItemAssignmentTest, AssignmentTypeConversion) {
  smap::Item<1, long> long_item(1000000L);
  smap::Item<1, int> int_item(42);

  int_item = long_item;

  EXPECT_EQ(int_item.val, 1000000);
}
