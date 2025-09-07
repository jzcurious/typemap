#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using StringItem = smap::Item<'a', std::string>;
using DoubleItem = smap::Item<3.14, double>;

class ItemConstructorTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(ItemConstructorTest, DefaultConstructor) {
  smap::Item<1, int> int_item{};
  smap::Item<'a', std::string> string_item{};
  smap::Item<3.14, double> double_item{};

  EXPECT_EQ(int_item.key, 1);
  EXPECT_EQ(int_item.val, 0);

  EXPECT_EQ(string_item.key, 'a');
  EXPECT_EQ(string_item.val, "");

  EXPECT_DOUBLE_EQ(double_item.key, 3.14);
  EXPECT_DOUBLE_EQ(double_item.val, 0.0);
}

TEST_F(ItemConstructorTest, ValueConstructor) {
  smap::Item<1, int> int_item(42);
  smap::Item<'a', std::string> string_item("hello");
  smap::Item<3.14, double> double_item(2.71);

  EXPECT_EQ(int_item.key, 1);
  EXPECT_EQ(int_item.val, 42);

  EXPECT_EQ(string_item.key, 'a');
  EXPECT_EQ(string_item.val, "hello");

  EXPECT_DOUBLE_EQ(double_item.key, 3.14);
  EXPECT_DOUBLE_EQ(double_item.val, 2.71);
}

TEST_F(ItemConstructorTest, MoveConstructor) {
  std::string test_string = "moved_string";
  smap::Item<'a', std::string> string_item(std::move(test_string));

  EXPECT_EQ(string_item.key, 'a');
  EXPECT_EQ(string_item.val, "moved_string");
}

TEST_F(ItemConstructorTest, CopyConstructor) {
  smap::Item<1, int> original_item(42);
  smap::Item<1, int> copied_item(original_item);

  EXPECT_EQ(copied_item.key, 1);
  EXPECT_EQ(copied_item.val, 42);
}

TEST_F(ItemConstructorTest, ConvertibleTypes) {
  // int to double conversion
  smap::Item<1, double> double_item(42);
  EXPECT_DOUBLE_EQ(double_item.val, 42.0);

  // const char* to std::string conversion
  smap::Item<'a', std::string> string_item("test");
  EXPECT_EQ(string_item.val, "test");

  // float to double conversion
  smap::Item<2, double> float_to_double_item(3.14f);
  EXPECT_FLOAT_EQ(static_cast<float>(float_to_double_item.val), 3.14f);
}

TEST_F(ItemConstructorTest, TypeDeduction) {
  auto int_item = smap::Item<1, int>(42);
  auto string_item = smap::Item<'a', std::string>("hello");
  auto double_item = smap::Item<3.14, double>(2.718);  // NOLINT

  static_assert(std::is_same_v<decltype(int_item.val), int>);
  static_assert(std::is_same_v<decltype(string_item.val), std::string>);
  static_assert(std::is_same_v<decltype(double_item.val), double>);

  EXPECT_EQ(int_item.val, 42);
  EXPECT_EQ(string_item.val, "hello");
  EXPECT_DOUBLE_EQ(double_item.val, 2.718);
}

// TEST_F(ItemConstructorTest, ConstexprConstruction) {
//   constexpr smap::Item<1, int> constexpr_int_item(42);
//   constexpr smap::Item<'a', const char*> constexpr_string_item("test");

//   static_assert(constexpr_int_item.key == 1);
//   static_assert(constexpr_int_item.val == 42);
//   static_assert(constexpr_string_item.key == 'a');
// }

TEST_F(ItemConstructorTest, ForwardingReferences) {
  smap::Item<1, int> lvalue_item(42);
  smap::Item<1, int> move_constructed_item(std::move(lvalue_item));

  EXPECT_EQ(move_constructed_item.val, 42);
}

TEST_F(ItemConstructorTest, VariousKeyTypes) {
  smap::Item<0, int> zero_key_item(10);
  smap::Item<-1, int> negative_key_item(20);
  smap::Item<42ULL, int> unsigned_key_item(30);
  smap::Item<'x', int> char_key_item(40);

  EXPECT_EQ(zero_key_item.key, 0);
  EXPECT_EQ(zero_key_item.val, 10);

  EXPECT_EQ(negative_key_item.key, -1);
  EXPECT_EQ(negative_key_item.val, 20);

  EXPECT_EQ(unsigned_key_item.key, 42ULL);
  EXPECT_EQ(unsigned_key_item.val, 30);

  EXPECT_EQ(char_key_item.key, 'x');
  EXPECT_EQ(char_key_item.val, 40);
}

TEST_F(ItemConstructorTest, ComplexValueTypes) {
  smap::Item<1, std::vector<int>> vector_item(std::vector<int>{1, 2, 3});
  smap::Item<2, std::pair<int, std::string>> pair_item(std::make_pair(42, "test"));

  EXPECT_EQ(vector_item.val.size(), 3);
  EXPECT_EQ(vector_item.val[0], 1);
  EXPECT_EQ(vector_item.val[1], 2);
  EXPECT_EQ(vector_item.val[2], 3);

  EXPECT_EQ(pair_item.val.first, 42);
  EXPECT_EQ(pair_item.val.second, "test");
}

TEST_F(ItemConstructorTest, MoveOnlyTypes) {
  smap::Item<1, std::unique_ptr<int>> unique_ptr_item(std::make_unique<int>(42));

  EXPECT_NE(unique_ptr_item.val, nullptr);
  EXPECT_EQ(*unique_ptr_item.val, 42);
}

TEST_F(ItemConstructorTest, PerformanceTest) {
  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] smap::Item<1, int> item(i);
    [[maybe_unused]] smap::Item<'a', std::string> string_item("test");
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 100);  // Should be fast
}

TEST_F(ItemConstructorTest, ConstructorRequirements) {
  // Test that constructor requires convertible types
  smap::Item<1, double> double_item(42);  // int to double - should compile
  smap::Item<'a', std::string> string_item(
      "test");  // const char* to string - should compile

  EXPECT_DOUBLE_EQ(double_item.val, 42.0);
  EXPECT_EQ(string_item.val, "test");
}

TEST_F(ItemConstructorTest, EmptyAndDefaultValues) {
  smap::Item<1, std::string> empty_string_item("");
  smap::Item<2, std::vector<int>> empty_vector_item(std::vector<int>{});
  smap::Item<3, int> zero_int_item(0);

  EXPECT_TRUE(empty_string_item.val.empty());
  EXPECT_TRUE(empty_vector_item.val.empty());
  EXPECT_EQ(zero_int_item.val, 0);
}

TEST_F(ItemConstructorTest, LargeValues) {
  std::string large_string(10000, 'x');
  smap::Item<1, std::string> large_string_item(large_string);

  EXPECT_EQ(large_string_item.val.size(), 10000);
  EXPECT_EQ(large_string_item.val, large_string);
}

TEST_F(ItemConstructorTest, TemplateArgumentDeduction) {
  auto item1 = smap::Item<1, int>(42);
  auto item2 = smap::Item<'a', std::string>("hello");

  EXPECT_EQ(item1.val, 42);
  EXPECT_EQ(item2.val, "hello");
}
