#ifndef STATICMAP_ITERATOR_HPP
#define STATICMAP_ITERATOR_HPP

#include "./staticmap_kind.hpp"

#include <utility>

namespace smap {

template <StaticMapKind StaticMapT>
struct StaticMapIterator {
 public:
  struct smap_iterator_explicit_feature {};

  using smap_t = StaticMapT;

 private:
  std::size_t index_;
  StaticMapT& smap_;

 public:
  StaticMapIterator(StaticMapT& static_map, std::size_t index = 0)
      : index_(index)
      , smap_(static_map) {}

  StaticMapIterator& operator++() {
    ++index_;
    return *this;
  }

  StaticMapIterator operator++(int) {
    auto tmp = *this;
    ++(*this);
    return tmp;
  }

  StaticMapIterator& operator--() {
    --index_;
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

  std::size_t index() const {
    return index_;
  }

  template <class FuncT>
    requires std::is_invocable_v<FuncT, typename smap_t::items_t>
             or std::is_invocable_v<FuncT>
  constexpr void visit(FuncT&& func) const {
    _visit(std::forward<FuncT>(func), std::make_index_sequence<StaticMapT::size>{});
  }

 private:
  template <class FuncT, std::size_t... i>
  constexpr void _visit(FuncT&& func, std::index_sequence<i...>) const {
    ((index_ == i ? (func(std::get<i>(smap_.items())), true) : false) or ...);
  }
};

}  // namespace smap

#endif  // STATICMAP_ITERATOR_HPP
