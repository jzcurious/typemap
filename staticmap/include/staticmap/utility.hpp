#ifndef STATICMAP_UTILITY_HPP
#define STATICMAP_UTILITY_HPP

#include "./item.hpp"
#include "./staticmap_kind.hpp"

#include <concepts>

namespace smap {

template <ItemKind... ItemT>
struct StaticMap;

template <auto... keys_candidates>
constexpr bool all_unique_keys() {
  if constexpr (sizeof...(keys_candidates) == 0) {
    return true;
  } else {
    constexpr const std::size_t unique = ([]<auto key>() {
      return (static_cast<std::size_t>(keys_candidates == key) + ...);
    }.template operator()<keys_candidates>()
                                          + ...);
    return unique == sizeof...(keys_candidates);
  }
}

template <class T>
concept PredicateKind
    = std::predicate<decltype([]() { return T{}.template operator()<Item<0, int>>(); })>;

}  // namespace smap

namespace smap::internal {

template <PredicateKind auto pred, ItemKind ItemT, StaticMapKind StaticMapT>
struct add_item_if;

template <PredicateKind auto pred, ItemKind ItemT>
struct add_item_if<pred, ItemT, StaticMap<>> {
  using type = std::
      conditional_t<pred.template operator()<ItemT>(), StaticMap<ItemT>, StaticMap<>>;
};

template <PredicateKind auto pred, ItemKind U, ItemKind... T>
struct add_item_if<pred, U, StaticMap<T...>> {
  using type = std::
      conditional_t<pred.template operator()<U>(), StaticMap<U, T...>, StaticMap<T...>>;
};

template <PredicateKind auto pred, StaticMapKind StaticMapT, class... ItemT>
struct filter;

template <PredicateKind auto pred, StaticMapKind StaticMapT>
struct filter<pred, StaticMapT> {
  using type = StaticMapT;
};

template <PredicateKind auto pred, StaticMapKind StaticMapT, class U>
struct filter<pred, StaticMapT, U> {
  using type = add_item_if<pred, U, StaticMapT>::type;
};

template <PredicateKind auto pred, StaticMapKind StaticMapT, class U, class... T>
struct filter<pred, StaticMapT, U, T...> {
  using type = add_item_if<pred, U, typename filter<pred, StaticMapT, T...>::type>::type;
};

template <PredicateKind auto pred, class... ItemT>
using filter_t = filter<pred, StaticMap<>, ItemT...>::type;

template <StaticMapKind StaticMapT, class... ItemT>
struct merge;

template <StaticMapKind StaticMapT>
struct merge<StaticMapT> {
  using type = StaticMapT;
};

template <StaticMapKind StaticMapT, class U>
struct merge<StaticMapT, U> {
  using type = add_item_if<[]<ItemKind ItemT>() {
    return not StaticMapT::template contains<ItemT::key>();
  },
      U,
      StaticMapT>::type;
};

template <StaticMapKind StaticMapT, class U, class... T>
struct merge<StaticMapT, U, T...> {
  using type = merge<typename merge<StaticMapT, T...>::type, U>::type;
};

template <StaticMapKind StaticMapT, class... ItemT>
using merge_t = merge<StaticMapT, ItemT...>::type;

}  // namespace smap::internal

#endif  // STATICMAP_UTILITY_HPP
