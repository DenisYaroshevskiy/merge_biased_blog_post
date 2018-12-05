#pragma once

#include <algorithm>

namespace srt {

namespace detail {

struct less {
  template <typename T, typename U>
  bool operator()(const T& x, const U& y) {
    return x < y;
  }
};

}  // namespace detail

// InputMergeRequirements<I1, I2, O, P> =
//       InputIterator<I1> && InputIterator<I2> && OutputIterator<O> &&
//       StrictWeakOrder<P(ValueType<I>, V)>

// ForwardInputMergeRequirements<I1, I2, O, P> =
//       ForwardIterator<I1> && InputIterator<I2> && OutputIterator<O> &&
//       StrictWeakOrder<P(ValueType<I>, V)>

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge_linear(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  if (f1 == l1) goto copySecond;
  if (f2 == l2) goto copyFirst;

while(true) {
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;
    goto nextCheck;
  takeSecond:
    *o++ = *f2++;  if (f2 == l2) goto copyFirst;
  nextCheck:
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

template <typename I1, typename I2, typename O>
O merge_linear(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
  return merge_linear(f1, l1, f2, l2, o, detail::less{});
}

namespace detail {

template <typename I>
using DifferenceType = typename std::iterator_traits<I>::difference_type;

template <typename I, typename P>
I partition_point_biased_no_checks(I f, P p) {
  while(true) {
    if (!p(*f)) return f; ++f;
    if (!p(*f)) return f;  ++f;
    if (!p(*f)) return f;  ++f;
    for (DifferenceType<I> step = 2;; step += step) {
      I test = std::next(f, step);
      if (!p(*test)) break;
      f = ++test;
    }
  }
}

template <typename I>
I middle(I f, I l) {
  static_assert(
    std::numeric_limits<DifferenceType<I>>::max() <=
    std::numeric_limits<size_t>::max(),
    "iterators difference type is too big");
  return std::next(f, static_cast<size_t>(std::distance(f, l)) / 2);
}

template <typename I, typename P>
I find_boundary(I f, I l, P p) {
  I sent = middle(f, l);
  if (p(*sent)) return sent;
  return partition_point_biased_no_checks(f, p);
}

}  // namespace detail

template <typename I1, typename I2, typename O, typename P>
// requiers ForwardInputMergeRequirements<I1, I2, O, P>
O merge_biased(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  if (f1 == l1) goto copySecond;
  if (f2 == l2) goto copyFirst;

while(true) {
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;
    goto nextCheck;
  takeSecond:
    *o++ = *f2++;  if (f2 == l2) goto copyFirst;
  nextCheck:
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;
    if (p(*f2, *f1)) goto takeSecond;
    *o++ = *f1++; if (f1 == l1) goto copySecond;

    I1 next_f1 = detail::find_boundary(f1, l1, [&](const auto& x) { return !p(*f2, x); });
    o = std::copy(f1, next_f1, o);
    f1 = next_f1;
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

template <typename I1, typename I2, typename O>
O merge_biased(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
  return merge_biased(f1, l1, f2, l2, o, detail::less{});
}

}  // namespace srt