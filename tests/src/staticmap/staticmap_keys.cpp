#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using IntItem1 = smap::Item<1, int>;
using IntItem2 = smap::Item<2, int>;
using IntItem3 = smap::Item<3, int>;
using CharItem = smap::Item<'a', std::string>;

class StaticMapKeysTest : public ::testing::Test {
 protected:
  void SetUp() override {}

  void TearDown() override {}
};

TEST_F(StaticMapKeysTest, BasicKeysAccess) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 2);
}

TEST_F(StaticMapKeysTest, MultipleKeys) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 2);
  EXPECT_EQ(std::get<2>(keys), 3);
}

TEST_F(StaticMapKeysTest, MixedKeyTypes) {
  smap::StaticMap<IntItem1, CharItem> map(IntItem1(10), CharItem("test"));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 'a');
}

TEST_F(StaticMapKeysTest, SingleKey) {
  smap::StaticMap<IntItem1> map(IntItem1(42));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::tuple_size_v<std::decay_t<decltype(keys)>>, 1);
}

TEST_F(StaticMapKeysTest, EmptyMapKeys) {
  smap::StaticMap<> map;

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::tuple_size_v<std::decay_t<decltype(keys)>>, 0);
}

TEST_F(StaticMapKeysTest, ConstCorrectness) {
  const smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 2);
}

TEST_F(StaticMapKeysTest, KeysType) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  constexpr const auto& keys = map.keys();

  static_assert(std::is_same_v<decltype(keys), const std::tuple<int, int, int>&>);
}

TEST_F(StaticMapKeysTest, KeysTupleSize) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::tuple_size_v<std::decay_t<decltype(keys)>>, 3);
}

TEST_F(StaticMapKeysTest, KeysInOrder) {
  smap::StaticMap<IntItem3, IntItem1, IntItem2> map(
      IntItem3(30), IntItem1(10), IntItem2(20));

  constexpr const auto& keys = map.keys();

  // Keys should be in the order they were declared in the template
  EXPECT_EQ(std::get<0>(keys), 3);
  EXPECT_EQ(std::get<1>(keys), 1);
  EXPECT_EQ(std::get<2>(keys), 2);
}

TEST_F(StaticMapKeysTest, CharacterKeys) {
  smap::StaticMap<CharItem> map(CharItem("test"));

  constexpr const auto& keys = map.keys();

  EXPECT_EQ(std::get<0>(keys), 'a');
}

TEST_F(StaticMapKeysTest, KeysReference) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr const auto& keys = map.keys();

  // Verify it's actually a reference to the internal keys
  EXPECT_EQ(&keys, &map.keys());
}

TEST_F(StaticMapKeysTest, KeysImmutability) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr const auto& keys = map.keys();

  // Keys should be immutable
  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 2);

  // These values should not change even if map values change
  map.template at<1>() = 100;
  map.template at<2>() = 200;

  EXPECT_EQ(std::get<0>(keys), 1);
  EXPECT_EQ(std::get<1>(keys), 2);
}

TEST_F(StaticMapKeysTest, PerformanceTest) {
  smap::StaticMap<IntItem1, IntItem2, IntItem3> map(
      IntItem1(10), IntItem2(20), IntItem3(30));

  auto start_time = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 100000; ++i) {
    [[maybe_unused]] constexpr const auto& keys = map.keys();
    // Just accessing keys - should be very fast
  }

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration
      = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);

  EXPECT_LT(duration.count(), 10000);  // Should be extremely fast
}

TEST_F(StaticMapKeysTest, KeysWithUpdate) {
  smap::StaticMap<IntItem1, IntItem2> map(IntItem1(10), IntItem2(20));

  constexpr const auto& original_keys = map.keys();

  // Update values but keys should remain the same
  map.update(IntItem1(100));

  constexpr const auto& keys_after_update = map.keys();

  EXPECT_EQ(std::get<0>(original_keys), std::get<0>(keys_after_update));
  EXPECT_EQ(std::get<1>(original_keys), std::get<1>(keys_after_update));
  EXPECT_EQ(std::get<0>(keys_after_update), 1);
  EXPECT_EQ(std::get<1>(keys_after_update), 2);
}
