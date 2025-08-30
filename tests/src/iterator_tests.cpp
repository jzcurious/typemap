#include <gtest/gtest.h>
#include <staticmap/staticmap.hpp>

using TestMap
    = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>, smap::Item<30, double>>;

TEST(StaticMapIteratorTest, Constructor) {
  TestMap map(100, 'A', 3.14);

  auto it1 = smap::StaticMapIterator<TestMap>(map);
  EXPECT_EQ(it1.index(), 0);

  auto it2 = smap::StaticMapIterator<TestMap>(map, 2);
  EXPECT_EQ(it2.index(), 2);
}

TEST(StaticMapIteratorTest, DereferenceOperator) {
  TestMap map(100, 'A', 3.14);
  auto it = smap::StaticMapIterator<TestMap>(map);

  int visited_value = 0;
  it.visit(
      [&visited_value](const auto& item) { visited_value = item.val; });  // linter bug
  EXPECT_EQ(visited_value, 100);
}

TEST(StaticMapIteratorTest, IncrementOperators) {
  TestMap map(100, 'A', 3.14);
  auto it = smap::StaticMapIterator<TestMap>(map);

  EXPECT_EQ(it.index(), 0);

  ++it;
  EXPECT_EQ(it.index(), 1);

  auto it2 = it++;
  EXPECT_EQ(it2.index(), 1);
  EXPECT_EQ(it.index(), 2);
}

TEST(StaticMapIteratorTest, DecrementOperators) {
  TestMap map(100, 'A', 3.14);
  auto it = smap::StaticMapIterator<TestMap>(map, 2);

  EXPECT_EQ(it.index(), 2);

  --it;
  EXPECT_EQ(it.index(), 1);

  auto it2 = it--;
  EXPECT_EQ(it2.index(), 1);
  EXPECT_EQ(it.index(), 0);
}

TEST(StaticMapIteratorTest, EqualityOperators) {
  TestMap map1(100, 'A', 3.14);
  TestMap map2(200, 'B', 6.28);

  auto it1 = smap::StaticMapIterator<TestMap>(map1, 1);
  auto it2 = smap::StaticMapIterator<TestMap>(map1, 1);
  auto it3 = smap::StaticMapIterator<TestMap>(map1, 2);
  auto it4 = smap::StaticMapIterator<TestMap>(map2, 1);

  EXPECT_TRUE(it1 == it2);

  EXPECT_FALSE(it1 == it3);

  EXPECT_FALSE(it1 == it4);

  EXPECT_TRUE(it1 != it3);
  EXPECT_FALSE(it1 != it2);
}

TEST(StaticMapIteratorTest, VisitFunction) {
  TestMap map(100, 'A', 3.14);
  auto it = smap::StaticMapIterator<TestMap>(map, 1);

  char visited_char = '\0';
  it.visit([&visited_char](const auto& item) { visited_char = item.val; });
  EXPECT_EQ(visited_char, 'A');

  bool visited = false;
  it.visit([&visited](const auto&) { visited = true; });
  EXPECT_TRUE(visited);
}

TEST(StaticMapIteratorTest, VisitDifferentPositions) {
  TestMap map(100, 'A', 3.14);

  {
    auto it = smap::StaticMapIterator<TestMap>(map, 0);
    int val = 0;
    it.visit([&val](const auto& item) { val = item.val; });
    EXPECT_EQ(val, 100);
  }

  {
    auto it = smap::StaticMapIterator<TestMap>(map, 1);
    char val = '\0';
    it.visit([&val](const auto& item) { val = item.val; });
    EXPECT_EQ(val, 'A');
  }

  {
    auto it = smap::StaticMapIterator<TestMap>(map, 2);
    double val = 0.0;
    it.visit([&val](const auto& item) { val = item.val; });
    EXPECT_DOUBLE_EQ(val, 3.14);
  }
}

TEST(StaticMapIteratorTest, RangeBasedForLoop) {
  TestMap map(100, 'A', 3.14);

  std::vector<std::pair<int, std::variant<int, char, double>>> results;

  int index = 0;
  for (auto it = map.begin(); it != map.end(); ++it) {
    it.visit([index, &results](const auto& item) {
      if constexpr (std::is_same_v<decltype(item.val), int>) {
        results.push_back({index, item.val});
      } else if constexpr (std::is_same_v<decltype(item.val), char>) {
        results.push_back({index, item.val});
      } else if constexpr (std::is_same_v<decltype(item.val), double>) {
        results.push_back({index, item.val});
      }
    });
    ++index;
  }

  EXPECT_EQ(results.size(), 3);
  EXPECT_EQ(std::get<int>(results[0].second), 100);
  EXPECT_EQ(std::get<char>(results[1].second), 'A');
  EXPECT_DOUBLE_EQ(std::get<double>(results[2].second), 3.14);
}

TEST(StaticMapIteratorTest, OutOfBoundsVisit) {
  TestMap map(100, 'A', 3.14);

  auto it = smap::StaticMapIterator<TestMap>(map, 10);

  bool visited = false;
  it.visit([&visited](const auto&) { visited = true; });
}

TEST(StaticMapIteratorTest, ConstCorrectness) {
  const TestMap map(100, 'A', 3.14);
  auto it = smap::StaticMapIterator<const TestMap>(map);

  int visited_value = 0;
  it.visit([&visited_value](const auto& item) { visited_value = item.val; });
  EXPECT_EQ(visited_value, 100);
}

TEST(StaticMapIteratorTest, IteratorArithmetic) {
  TestMap map(100, 'A', 3.14);

  auto it = smap::StaticMapIterator<TestMap>(map);
  EXPECT_EQ(it.index(), 0);

  ++it;
  --it;
  EXPECT_EQ(it.index(), 0);

  it++;
  it--;
  EXPECT_EQ(it.index(), 0);

  auto it2 = it;
  ++it2;
  ++it2;
  --it2;
  --it2;
  EXPECT_TRUE(it == it2);
}

TEST(StaticMapIteratorTest, IncrementBoundsChecking) {
  TestMap map(100, 'A', 3.14);

  auto it = map.begin();
  EXPECT_EQ(it.index(), 0);

  ++it;
  EXPECT_EQ(it.index(), 1);

  ++it;
  EXPECT_EQ(it.index(), 2);

  ++it;
  EXPECT_EQ(it.index(), 3);  // end position

  ++it;
  EXPECT_EQ(it.index(), 3);
}

TEST(StaticMapIteratorTest, DecrementBoundsChecking) {
  TestMap map(100, 'A', 3.14);

  auto it = map.end();
  EXPECT_EQ(it.index(), 3);

  --it;
  EXPECT_EQ(it.index(), 2);

  --it;
  EXPECT_EQ(it.index(), 1);

  --it;
  EXPECT_EQ(it.index(), 0);

  --it;
  EXPECT_EQ(it.index(), 0);
}
