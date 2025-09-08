#ifndef STATICMAP_STATICMAP_KIND_HPP
#define STATICMAP_STATICMAP_KIND_HPP

#include <concepts>  // IWYU pragma: keep

namespace smap {

template <class T>
concept StaticMapKind = requires { typename T::smap_static_map_explicit_feature; };

}  // namespace smap

#endif  // STATICMAP_STATICMAP_KIND_HPP
