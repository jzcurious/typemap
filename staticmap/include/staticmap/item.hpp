#ifndef STATICMAP_ITEM_HPP
#define STATICMAP_ITEM_HPP

#include "./item_kind.hpp"

namespace smap {

template <Comparable auto _key, class ValT>
struct Item {
  struct smap_item_explicit_feature {};

  using key_t = decltype(_key);
  using val_t = ValT;

  static constexpr const key_t key = _key;
  val_t val;
};

}  // namespace smap

#ifndef NDEBUG
static_assert(smap::ItemKind<smap::Item<100, int>>);
#endif

#endif  // STATICMAP_ITEM_HPP
