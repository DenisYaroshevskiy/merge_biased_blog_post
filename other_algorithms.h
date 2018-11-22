#pragma once

#include <algorithm>

#include "../partition_point_biased_blog_post/result.h"

namespace srt {

// InputMergeRequirements<I1, I2, O, P> =
//       InputIterator<I1> && InputIterator<I2> && OutputIterator<O> &&
//       StrictWeakOrder<P(ValueType<I>, V)>

namespace libstd {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  while (f1 != l1 && f2 != l2) {
    if (p(*f2, *f1)) {
      *o = *f2;
      ++f2;
    } else {
      *o = *f1;
      ++f1;
    }
    ++o;
  }
  return std::copy(f2, l2, std::copy(f1, l1, o));
}

}  // namespace libstd

namespace v1 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  for (; f1 != l1; ++o) {
    if (f2 == l2) return std::copy(f1, l1, o);
    if (p(*f2, *f1)) {
      *o = *f2;
      ++f2;
    } else {
      *o = *f1;
      ++f1;
    }
  }
  return std::copy(f2, l2, o);
}

}  // namespace v1

namespace v2 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  if (f1 == l1) goto copySecond;
  if (f2 == l2) goto copyFirst;

  while (true) {
    if (p(*f2, *f1)) {
      *o++ = *f2++;
      if (f2 == l2) goto copyFirst;
    } else {
      *o++ = *f1++;
      if (f1 == l1) goto copySecond;
    }
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v2

namespace v3 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
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

}  // namespace v3

namespace v4 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  if (f1 == l1) goto copySecond;
  if (f2 == l2) goto copyFirst;
  goto start;

takeSecond:
  *o++ = *f2++;  if (f2 == l2) goto copyFirst;
start:
  if (p(*f2, *f1)) goto takeSecond;
  *o++ = *f1++; if (f1 == l1) goto copySecond;
  if (p(*f2, *f1)) goto takeSecond;
  *o++ = *f1++; if (f1 == l1) goto copySecond;
  if (p(*f2, *f1)) goto takeSecond;
  *o++ = *f1++; if (f1 == l1) goto copySecond;
  if (p(*f2, *f1)) goto takeSecond;
  *o++ = *f1++; if (f1 == l1) goto copySecond;
  goto start;

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v4

namespace v5 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
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
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v5

namespace v6 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  I1 next_f1 = f1;

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

    next_f1 = std::upper_bound(f1, l1, *f2, p);
    o = std::copy(f1, next_f1, o);
    f1 = next_f1; if (f1 == l1) goto copySecond;

    goto takeSecond;
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v6


namespace v7 {

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
  I1 next_f1 = f1;

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

    next_f1 = srt::upper_bound_biased(f1, l1, *f2, p);
    o = std::copy(f1, next_f1, o);
    f1 = next_f1; if (f1 == l1) goto copySecond;

    goto takeSecond;
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v7

namespace v8 {

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

template <typename I1, typename I2, typename O, typename P>
// requiers InputMergeRequirements<I1, I2, O, P>
O merge(I1 f1, I1 l1, I2 f2, I2 l2, O o, P p) {
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

    I1 next_f1 = find_boundary(f1, l1, [&](const auto& x) { return !p(*f2, x); });
    o = std::copy(f1, next_f1, o);
    f1 = next_f1;
  }

copySecond:
  return std::copy(f2, l2, o);
copyFirst:
  return std::copy(f1, l1, o);
}

}  // namespace v8

}  // namespace srt