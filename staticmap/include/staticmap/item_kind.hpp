#ifndef STATICMAP_ITEM_KIND_HPP
#define STATICMAP_ITEM_KIND_HPP

#include <concepts>  // IWYU pragma: keep

namespace smap {

template <class T>
concept ItemKind = requires { typename T::smap_item_explicit_feature; };

}  // namespace smap

#endif  // STATICMAP_ITEM_KIND_HPP
