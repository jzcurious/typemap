#ifndef TYPEMAP_ITEM_HPP
#define TYPEMAP_ITEM_HPP

#include <concepts>

namespace tmap {

template <class T>
concept ItemKind = requires {
  typename T::key_t;
  typename T::val_t;
} and requires(T x) {
  { x.val } -> std::convertible_to<typename T::val_t>;
};

template <class KeyT, class ValT>
struct Item {
  using key_t = KeyT;
  using val_t = ValT;

  val_t val;
};

}  // namespace tmap

#endif  // TYPEMAP_ITEM_HPP
