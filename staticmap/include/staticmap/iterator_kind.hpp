#ifndef STATICMAP_ITERATOR_KIND_HPP
#define STATICMAP_ITERATOR_KIND_HPP

#include <concepts>  // IWYU pragma: keep

namespace smap {

template <class T>
concept StaticMapIteratorKind = requires { typename T::smap_iterator_explicit_feature; };

}  // namespace smap

#endif  // STATICMAP_ITERATOR_KIND_HPP
