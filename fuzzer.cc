#include "result.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace {

struct stability_less {
  bool operator()(int x, int y) const { return x < y; }

  template <typename T, typename U>
  bool operator()(const std::pair<T, U>& x, const std::pair<T, U>& y) const {
    return x.first < y.first;
  }
};

using test_t = std::pair<int, bool>;

}  // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data_bytes, size_t size_bytes) {
  const int* data = reinterpret_cast<const int*>(data_bytes);
  size_t size = size_bytes / sizeof(int);

  if (size < 1)
    return 0;

  const int distance_f_m = (*data) % size;

  ++data;
  --size;

  const int* data_f = data;
  const int* data_m = data_f + distance_f_m;
  const int* data_l = data_f + size;

  std::vector<test_t> lhs(distance_f_m);
  std::transform(data_f, data_m, lhs.begin(), [](int x) { return test_t{x, false}; });
  std::sort(lhs.begin(), lhs.end());

  std::vector<test_t> rhs(data_l - data_m);
  std::transform(data_m, data_l, rhs.begin(), [](int x) { return test_t{x, true}; });
  std::sort(rhs.begin(), rhs.end());

  std::vector<test_t> expected(size);
  std::merge(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), expected.begin(), stability_less{});

  std::vector<test_t> actual_linear(size);
  srt::merge_linear(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), actual_linear.begin(), stability_less{});

  std::vector<test_t> actual_biased(size);
  srt::merge_biased(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), actual_biased.begin(), stability_less{});

  assert(std::equal(expected.begin(), expected.end(), actual_linear.begin()));
  assert(std::equal(expected.begin(), expected.end(), actual_biased.begin()));

  return 0;  // Non-zero return values are reserved for future use.
}