#ifndef STATICMAP_ITEM_KIND_HPP
#define STATICMAP_ITEM_KIND_HPP

#include <concepts>

namespace smap {

template <class T>
concept Comparable = requires(T x, T y) {
  { x == y } -> std::convertible_to<bool>;
};

template <class T>
concept ItemKind = requires {
  typename T::smap_item_explicit_feature;
  typename T::key_t;
  typename T::val_t;
  { T::key } -> std::convertible_to<typename T::key_t>;
} and Comparable<typename T::key_t> and requires(T x) {
  { x.val } -> std::same_as<typename T::val_t&>;
};

}  // namespace smap

#endif  // STATICMAP_ITEM_KIND_HPP
