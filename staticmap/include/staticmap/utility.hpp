#ifndef STATICMAP_UTILITY_HPP
#define STATICMAP_UTILITY_HPP

#include "./item.hpp"
#include "./staticmap_kind.hpp"

#include <concepts>

namespace smap {

template <ItemKind... ItemT>
struct StaticMap;

}  // namespace smap

namespace smap::internal {

template <auto... keys_candidates>
constexpr bool all_unique_keys() {
  constexpr const std::size_t unique = ([]<auto key>() {
    return (static_cast<std::size_t>(keys_candidates == key) + ...);
  }.template operator()<keys_candidates>()
                                        + ...);
  return unique == sizeof...(keys_candidates);
}

template <class T>
concept PredicateKind
    = std::predicate<decltype([]() { return T{}.template operator()<Item<0, int>>(); })>;
template <PredicateKind PredT, ItemKind ItemT, StaticMapKind StaticMapT>
struct add_item_if;

template <PredicateKind PredT, ItemKind ItemT>
struct add_item_if<PredT, ItemT, StaticMap<>> {
  using type = std::
      conditional_t<PredT{}.template operator()<ItemT>(), StaticMap<ItemT>, StaticMap<>>;
};

template <PredicateKind PredT, ItemKind U, ItemKind... T>
struct add_item_if<PredT, U, StaticMap<T...>> {
  using type = std::conditional_t<PredT{}.template operator()<U>(),
      StaticMap<U, T...>,
      StaticMap<T...>>;
};

template <PredicateKind PredT, StaticMapKind StaticMapT, class... ItemT>
struct filter;

template <PredicateKind PredT, StaticMapKind StaticMapT>
struct filter<PredT, StaticMapT> {
  using type = StaticMapT;
};

template <PredicateKind PredT, StaticMapKind StaticMapT, class U>
struct filter<PredT, StaticMapT, U> {
  using type = add_item_if<PredT, U, StaticMapT>::type;
};

template <PredicateKind PredT, StaticMapKind StaticMapT, class U, class... T>
struct filter<PredT, StaticMapT, U, T...> {
  using type
      = add_item_if<PredT, U, typename filter<PredT, StaticMapT, T...>::type>::type;
};

template <PredicateKind PredT, class... ItemT>
using filter_t = filter<PredT, StaticMap<>, ItemT...>::type;

template <class LambdaT>
struct predicate_t {
  LambdaT lambda;

  template <class ItemT>
  constexpr bool operator()() const {
    return lambda.template operator()<ItemT>();
  }
};

template <StaticMapKind StaticMapT>
using NotContains = predicate_t<decltype([]<ItemKind ItemT>() {
  return not StaticMapT::template contains<ItemT::key>();
})>;

template <StaticMapKind StaticMapT, class... ItemT>
struct merge;

template <StaticMapKind StaticMapT>
struct merge<StaticMapT> {
  using type = StaticMapT;
};

template <StaticMapKind StaticMapT, class U>
struct merge<StaticMapT, U> {
  using type = add_item_if<NotContains<StaticMapT>, U, StaticMapT>::type;
};

template <StaticMapKind StaticMapT, class U, class... T>
struct merge<StaticMapT, U, T...> {
  using type = merge<typename merge<StaticMapT, T...>::type, U>::type;
};

template <StaticMapKind StaticMapT, class... ItemT>
using merge_t = merge<StaticMapT, ItemT...>::type;

}  // namespace smap::internal

#endif  // STATICMAP_UTILITY_HPP
