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

  template <class ValTT>
    requires std::convertible_to<ValTT, ValT>
  Item(ValTT&& val)
      : val(std::forward<ValTT>(val)) {}

  template <ItemKind ItemT>
  static constexpr bool is_compatible() {
    return std::is_same_v<typename ItemT::key_t, key_t> and ItemT::key == key
           and std::is_convertible_v<typename ItemT::val_t, val_t>;
  }

  template <ItemKind ItemT>
    requires(is_compatible<std::decay_t<ItemT>>())
  Item& operator=(ItemT&& other) {
    val = std::forward<ItemT>(other).val;
    return *this;
  }

  template <ItemKind ItemT>
  bool operator==(const ItemT& other) {
    if constexpr (not is_compatible<ItemT>()) {
      return false;
    } else {
      return val == other.val;
    }
  }
};

}  // namespace smap

#ifndef NDEBUG
static_assert(smap::ItemKind<smap::Item<100, int>>);
#endif

#endif  // STATICMAP_ITEM_HPP
