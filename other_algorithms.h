#pragma once

#include <algorithm>
#include <iostream>

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

}  // namespace srt