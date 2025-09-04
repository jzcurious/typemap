#ifndef STATICMAP_STATICMAP_HPP
#define STATICMAP_STATICMAP_HPP

#include "./item.hpp"  // IWYU pragma: export

#include "./item_kind.hpp"
#include "./iterator.hpp"

#include <tuple>

namespace smap {

template <auto... keys_candidates>
constexpr bool all_unique_keys() {
  constexpr const std::size_t unique = ([]<auto key>() {
    return (static_cast<std::size_t>(keys_candidates == key) + ...);
  }.template operator()<keys_candidates>()
                                        + ...);
  return unique == sizeof...(keys_candidates);
}

template <ItemKind... ItemT>
  requires((sizeof...(ItemT) == 0) or all_unique_keys<ItemT::key...>())
struct StaticMap {
 public:
  struct smap_static_map_explicit_feature {};

  using keys_t = std::tuple<typename ItemT::key_t...>;
  using vals_t = std::tuple<typename ItemT::val_t...>;
  using items_t = std::tuple<ItemT...>;

  static constexpr const std::size_t size = sizeof...(ItemT);

 private:
  static constexpr const keys_t keys_ = {ItemT::key...};

  items_t items_;

 public:
  StaticMap() = default;

  template <class... T>
    requires(sizeof...(T) == sizeof...(ItemT))
  StaticMap(T&&... args)
      : items_(std::forward<T>(args)...) {}

 private:
  template <std::size_t... indices, class... ValT>
  constexpr auto init_partial(std::index_sequence<indices...>, ValT&&... vals) {
    auto val_tuple = std::make_tuple(std::forward<ValT>(vals)...);

    return [&]<std::size_t... all_indices>(std::index_sequence<all_indices...>) {
      return std::tuple<ItemT...>{[&]() -> decltype(auto) {
        if constexpr (all_indices < sizeof...(ValT)) {
          return ItemT(std::get<all_indices>(val_tuple));
        } else {
          using ItemType = std::tuple_element_t<all_indices, std::tuple<ItemT...>>;
          using ValueType = typename ItemType::val_t;
          static_assert(std::is_default_constructible_v<ValueType>,
              "Value type must be default constructible for partial init");
          return ItemType(ValueType{});
        }
      }()...};
    }(std::make_index_sequence<sizeof...(ItemT)>{});
  }

 public:
  template <class... ValT>
    requires(sizeof...(ValT) > 0 and sizeof...(ValT) < sizeof...(ItemT))
  StaticMap(ValT&&... vals)
      : items_(init_partial(
            std::index_sequence_for<ValT...>{}, std::forward<ValT>(vals)...)) {}

  [[nodiscard]] constexpr bool empty() const {
    return size == 0;
  }

  constexpr const keys_t& keys() const {
    return keys_;
  }

  std::tuple<typename ItemT::val_t&...> vals() {
    return std::apply([](auto&... items) { return std::tie(items.val...); }, items_);
  }

  std::tuple<const typename ItemT::val_t&...> vals() const {
    return std::apply([](auto&... items) { return std::tie(items.val...); }, items_);
  }

  items_t& items() {
    return items_;
  }

  const items_t& items() const {
    return items_;
  }

 private:
  struct find_result_t {
    bool found;
    std::size_t index;

    constexpr operator bool() const {
      return found;
    }

    constexpr operator std::size_t() const {
      return index;
    }
  };

  template <auto key, std::size_t... i>
  static constexpr find_result_t _find_item(std::index_sequence<i...>) {
    std::size_t item_idx = 0;
    bool found = false;
    ((std::get<i>(keys_) == key ? (item_idx = i, found = true) : false) or ...);
    return {found, item_idx};
  }

  template <auto key>
  static constexpr find_result_t find_item() {
    return _find_item<key>(std::make_index_sequence<size>{});
  }

 public:
  template <auto key>
  static constexpr bool contains() {
    return find_item<key>();
  }

  template <auto... keys>
  static constexpr bool contains_all() {
    return (contains<keys>() and ...);
  }

  template <auto... keys>
  static constexpr bool contains_any() {
    return (contains<keys>() or ...);
  }

  template <auto key, bool ignore_not_found = false>
  auto& at() {
    constexpr auto result = find_item<key>();
    static_assert(ignore_not_found or result.found, "[smap] Key not found");
    return std::get<result.index>(items_).val;
  }

  template <auto key, bool ignore_not_found = false>
  const auto& at() const {
    constexpr auto result = find_item<key>();
    static_assert(ignore_not_found or result.found, "[smap] Key not found");
    return std::get<result.index>(items_).val;
  }

  template <auto key, class DefaultValT>
  auto get(DefaultValT default_value = {}) const {
    constexpr auto result = find_item<key>();
    if constexpr (result.found)
      return std::get<result.index>(items_).val;
    else
      return default_value;
  }

  template <auto key, class ValT>
  constexpr void set(ValT&& value) {
    at<key>() = std::forward<ValT>(value);
  }

  constexpr void clear() {
    std::apply(
        [](auto&... items) { ((items.val = typename ItemT::val_t{}), ...); }, items_);
  }

  template <class FuncT>
  constexpr void for_each(FuncT&& func) {
    std::apply([&](auto&... items) { (std::forward<FuncT>(func)(items), ...); }, items_);
  }

  template <class FuncT>
  constexpr void for_each_indexed(FuncT&& func) {
    [&]<std::size_t... i>(std::index_sequence<i...>) {
      (std::forward<FuncT>(func)(i, std::get<i>(items_)), ...);
    }(std::make_index_sequence<size>{});
  }

  auto begin() {
    return StaticMapIterator(*this);
  }

  auto end() {
    return StaticMapIterator(*this, size);
  }

 public:
  template <class... ItemTT>
    requires(ItemKind<std::decay_t<ItemTT>> and ...)
  void update(ItemTT&&... items) {
    (
        [&]() {
          constexpr auto result = find_item<items.key>();
          if constexpr (result) {
            std::get<result.index>(items_) = std::forward<ItemTT>(items);
          }
        }(),
        ...);
  }

  template <StaticMapKind StaticMapT>
  void update(const StaticMapT& other) {
    std::apply(
        [&](const auto&... items) {
          (
              [&]() {
                constexpr auto result = find_item<items.key>();
                if constexpr (result) {
                  std::get<result.index>(items_) = items;
                }
              }(),
              ...);
        },
        other.items());
  }

  template <auto key>
  using val_t = typename std::remove_cvref_t<decltype(std::get<find_item<key>().index>(
      items_))>::val_t;

  template <StaticMapKind StaticMapT>
  bool operator==(const StaticMapT& other) const {
    if constexpr (StaticMapT::size != size) {
      return false;
    } else {
      bool result = true;
      std::apply(
          [&](const auto&... items) {
            (([&]() {
              constexpr auto key = items.key;
              constexpr auto find_result = find_item<key>();
              if constexpr (find_result.found) {
                if (std::get<find_result.index>(items_).val != items.val) {
                  result = false;
                }
              }
            }()),
                ...);
          },
          other.items());
      return result;
    }
  }

  // TODO: add merge
  // TODO: add filter
  // TODO: add join
};

template <class... ItemT>
  requires(ItemKind<std::decay_t<ItemT>> and ...)
auto make_static_map(ItemT&&... items) {
  return StaticMap<std::decay_t<ItemT>...>(std::forward<ItemT>(items)...);
}

}  // namespace smap

#ifndef NDEBUG
  #include "./iterator_kind.hpp"
  #include "./staticmap_kind.hpp"

using map_t = smap::StaticMap<smap::Item<10, int>, smap::Item<11, char>>;

static_assert(smap::StaticMapKind<map_t>);
static_assert(smap::StaticMapIteratorKind<smap::StaticMapIterator<map_t>>);

using bad_keys_map1_t
    = smap::StaticMap<smap::Item<1, int>, smap::Item<0, char>, smap::Item<1, int>>;

using bad_keys_map2_t
    = smap::StaticMap<smap::Item<10, int>, smap::Item<20, char>, smap::Item<10, double>>;
#endif
#endif  // STATICMAP_STATICMAP_HPP
