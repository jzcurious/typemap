#ifndef STATICMAP_STATICMAP_KIND_HPP
#define STATICMAP_STATICMAP_KIND_HPP

#include <concepts>

namespace smap {

template <class T>
concept StaticMapKind = requires {
  typename T::smap_static_map_explicit_feature;
  typename T::keys_t;
  typename T::vals_t;
  typename T::items_t;
  { T::size } -> std::convertible_to<std::size_t>;
} and requires(T x) {
  { x.keys() } -> std::convertible_to<typename T::keys_t>;
  { x.vals() } -> std::convertible_to<typename T::keys_t>;
  { x.items() } -> std::convertible_to<typename T::items_t>;
  requires requires() { x.template contains<0>(); };
  requires requires() { x.template at<0, 1>(); };
  requires requires() { x.template get<0, int>(); };
};

}  // namespace smap

#endif  // STATICMAP_STATICMAP_KIND_HPP
