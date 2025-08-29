#ifndef STATICMAP_ITERATOR_KIND_HPP
#define STATICMAP_ITERATOR_KIND_HPP

#include <concepts>

namespace smap {

template <class T>
concept StaticMapIteratorKind = requires {
  typename T::smap_iterator_explicit_feature;
  typename T::smap_t;
} and requires(T x) {
  { x++ } -> std::convertible_to<T>;
  { ++x } -> std::convertible_to<T>;
  { x-- } -> std::convertible_to<T>;
  { --x } -> std::convertible_to<T>;
  { x == x } -> std::same_as<bool>;
  { x != x } -> std::same_as<bool>;
  { x.index() } -> std::convertible_to<std::size_t>;
  {
    x.visit([]() {})
  };
} and std::constructible_from<T, typename T::smap_t&>;

}  // namespace smap

#endif  // STATICMAP_ITERATOR_KIND_HPP
