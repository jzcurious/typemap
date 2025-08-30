#ifndef STATICMAP_STATICMAP_HPP
#define STATICMAP_STATICMAP_HPP

#include "./item.hpp"  // IWYU pragma: export

#include "./item_kind.hpp"
#include "./iterator.hpp"

#include <tuple>

namespace smap {

template <ItemKind... ItemT>
struct StaticMap {
  // TODO: check unique

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
  StaticMap()
      : items_() {}

  template <class... ValT>
    requires(sizeof...(ValT) == sizeof...(ItemT))
  StaticMap(ValT&&... vals)
      : items_(std::forward<ValT>(vals)...) {}

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
  };

  template <auto key, std::size_t... i>
  static constexpr find_result_t find_item(std::index_sequence<i...>) {
    std::size_t item_idx = 0;
    bool found = false;
    ((std::get<i>(keys_) == key ? (item_idx = i, found = true) : false) or ...);
    return {found, item_idx};
  }

 public:
  template <auto key>
  static constexpr bool contains() {
    return find_item<key>(std::make_index_sequence<size>{}).found;
  }

  template <auto key, bool ignore_not_found = false>
  auto& at() {
    constexpr auto result = find_item<key>(std::make_index_sequence<size>{});
    static_assert(ignore_not_found or result.found, "[smap] Key not found.");
    return std::get<result.index>(items_).val;
  }

  template <auto key, bool ignore_not_found = false>
  const auto& at() const {
    constexpr auto result = find_item<key>(std::make_index_sequence<size>{});
    static_assert(ignore_not_found or result.found, "[smap] Key not found.");
    return std::get<result.index>(items_).val;
  }

  template <auto key, class DefaultValT>
  auto get(DefaultValT default_value = {}) const {
    constexpr auto result = find_item<key>(std::make_index_sequence<size>{});
    if constexpr (result.found)
      return std::get<result.index>(items_).val;
    else
      return default_value;
  }

  auto begin() {
    return StaticMapIterator(*this);
  }

  auto end() {
    return StaticMapIterator(*this, size);
  }
};

template <ItemKind... ItemT>
auto make_static_map(const ItemT&... items) {
  return StaticMap<ItemT...>(items...);
}

template <ItemKind... ItemT>
auto make_static_map_no_decay(ItemT&&... items) {
  return StaticMap<ItemT...>(items...);
}

}  // namespace smap

#ifndef NDEBUG
  #include "./iterator_kind.hpp"
  #include "./staticmap_kind.hpp"

static_assert(
    smap::StaticMapKind<smap::StaticMap<smap::Item<10, int>, smap::Item<11, char>>>);

static_assert(smap::StaticMapIteratorKind<
    smap::StaticMapIterator<smap::StaticMap<smap::Item<10, int>, smap::Item<11, char>>>>);
#endif

#endif  // STATICMAP_STATICMAP_HPP
