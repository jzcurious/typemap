#ifndef STATICMAP_ITEM_HPP
#define STATICMAP_ITEM_HPP

#include "./item_kind.hpp"

#include <utility>

namespace smap {

template <Comparable auto _key, class ValT>
struct Item {
  struct smap_item_explicit_feature {};

  using key_t = decltype(_key);
  using val_t = ValT;

  static constexpr const key_t key = _key;
  val_t val;

  Item() = default;

  template <class _ValT>
    requires std::convertible_to<_ValT, ValT>
  Item(_ValT&& val)
      : val(std::forward<_ValT>(val)) {}

  template <ItemKind ItemT>
  static constexpr bool is_compatible() {
    return std::is_same_v<typename ItemT::key_t, key_t> and ItemT::key == key
           and std::is_convertible_v<typename ItemT::val_t, val_t>;
  }

  template <ItemKind ItemT>
    requires(is_compatible<ItemT>())
  Item& operator=(const ItemT& other) {
    val = other.val;
    return *this;
  }

  template <ItemKind ItemT>
    requires(is_compatible<ItemT>())
  Item& operator=(ItemT&& other) {
    val = std::move(other.val);
    return *this;
  }
};

}  // namespace smap

#ifndef NDEBUG
static_assert(smap::ItemKind<smap::Item<100, int>>);
#endif

#endif  // STATICMAP_ITEM_HPP
