#ifndef STATICMAP_ITEM_HPP
#define STATICMAP_ITEM_HPP

#include <concepts>

namespace smap {

template <class T>
concept Comparable = requires(T x, T y) {
  { x == y } -> std::convertible_to<bool>;
};

template <class T>
concept ItemKind = requires {
  typename T::key_t;
  typename T::val_t;
  Comparable<typename T::key_t>;
  { T::key } -> std::convertible_to<typename T::key_t>;
} and requires(T x) {
  { x.val } -> std::same_as<typename T::val_t&>;
};

template <Comparable auto _key, class ValT>
struct Item {
  using key_t = decltype(_key);
  using val_t = ValT;

  static constexpr const key_t key = _key;
  val_t val;
};

}  // namespace smap

#endif  // STATICMAP_ITEM_HPP
