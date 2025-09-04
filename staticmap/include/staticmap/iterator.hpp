#ifndef STATICMAP_ITERATOR_HPP
#define STATICMAP_ITERATOR_HPP

#include "./staticmap_kind.hpp"

#include <utility>

namespace smap {

template <StaticMapKind StaticMapT>
  requires(StaticMapT::size > 0)
struct StaticMapIterator {
 public:
  struct smap_iterator_explicit_feature {};

  using smap_t = StaticMapT;

 private:
  std::size_t index_;
  std::reference_wrapper<StaticMapT> smap_;

 public:
  StaticMapIterator(StaticMapT& static_map, std::size_t index = 0)
      : index_(index)
      , smap_(static_map) {}

  StaticMapIterator& operator++() {
    if (index_ < smap_t::size) ++index_;
    return *this;
  }

  StaticMapIterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  StaticMapIterator& operator--() {
    if (index_ > 0) --index_;
    return *this;
  }

  StaticMapIterator operator--(int) {
    auto tmp = *this;
    --(*this);
    return tmp;
  }

  bool operator==(const StaticMapIterator& other) const {
    return (index_ == other.index_) and (&smap_ == &other.smap_);
  }

  [[nodiscard]] std::size_t index() const {
    return index_;
  }

  template <class FuncT>
    requires std::is_invocable_v<FuncT,
        typename std::tuple_element_t<0, typename smap_t::items_t>>
  constexpr void visit(FuncT&& func) const {
    _visit(std::forward<FuncT>(func), std::make_index_sequence<StaticMapT::size>{});
  }

 private:
  template <class FuncT, std::size_t... i>
    requires((i < smap_t::size) and ...)
  constexpr void _visit(FuncT&& func, std::index_sequence<i...>) const {
    ((index_ == i ? ((void)std::forward<FuncT>(func)(std::get<i>(smap_.items())), true)
                  : false)
        or ...);
  }
};

}  // namespace smap

#endif  // STATICMAP_ITERATOR_HPP
