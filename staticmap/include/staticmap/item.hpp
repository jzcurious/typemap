#ifndef STATICMAP_ITEM_HPP
#define STATICMAP_ITEM_HPP

#include "./item_kind.hpp"

#include <utility>

namespace smap {

template <class T>
concept Comparable = requires(T x, T y) {
  { x == y } -> std::convertible_to<bool>;
};

template <Comparable auto key_v, class ValT>
struct Item {
  struct smap_item_explicit_feature {};

  using key_t = decltype(key_v);
  using val_t = ValT;

  static constexpr const key_t key = key_v;
  val_t val;

  Item() = default;

  Item(const ValT& val)
      : val(val) {}

  Item(ValT&& val)
      : val(std::move(val)) {}

  template <ItemKind ItemT>
  static constexpr bool is_compatible() {
    return std::is_same_v<typename ItemT::key_t, key_t> and ItemT::key == key
           and (std::is_convertible_v<typename ItemT::val_t, val_t>
                or std::is_convertible_v<val_t, typename ItemT::val_t>);
  }

  template <class ItemT>
    requires(ItemKind<std::decay_t<ItemT>> and is_compatible<std::decay_t<ItemT>>())
  Item& operator=(ItemT&& other) {
    val = std::forward<ItemT>(other).val;
    return *this;
  }

  template <ItemKind ItemT>
  bool operator==(const ItemT& other) const {
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
