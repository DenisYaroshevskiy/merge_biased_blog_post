#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <map>
#include <random>
#include <set>

#include "../partition_point_biased_blog_post/result.h"
#include "other_algorithms.h"
#include "result.h"

namespace {

constexpr bool kOnlyLastElements = true;
constexpr std::size_t kBigStep = 20;

constexpr std::size_t kProblemSize = 2000u;
constexpr std::size_t kStep = kOnlyLastElements ? 1 : kBigStep;
constexpr std::size_t kMaxRhsSize =
    kOnlyLastElements ? kBigStep * 2 : kProblemSize;

void set_benchmark_input_sizes(benchmark::internal::Benchmark* bench) {
  std::size_t lhs_size = kProblemSize;
  std::size_t rhs_size = 0;

  do {
    bench->Args({static_cast<int>(lhs_size), static_cast<int>(rhs_size)});
    lhs_size -= kStep;
    rhs_size += kStep;
  } while (rhs_size <= kMaxRhsSize);
}

using test_type = std::int64_t;
using test_type_vec = std::vector<test_type>;
using test_merge_input = std::pair<test_type_vec, test_type_vec>;

test_type random_test_type_value() {
  static std::mt19937 g;
  static std::uniform_int_distribution<> dis(1, int(kProblemSize) * 100);
  return dis(g);
}

test_type_vec random_test_type_sorted_vec(std::size_t size) {
  test_type_vec res(size);
  std::generate(res.begin(), res.end(), random_test_type_value);
  std::sort(res.begin(), res.end());
  return res;
}

const test_merge_input& input_data(std::size_t lhs_size, std::size_t rhs_size) {
  // Cache makes sence if we would want to call this function multiple times for
  // the same input. Don't think it's a fantastic idea though because having
  // multiple benchmarks might screw up code alignment.
  static std::map<std::pair<std::size_t, std::size_t>, test_merge_input> cache;

  auto in_cache = cache.find({lhs_size, rhs_size});
  if (in_cache != cache.end()) return in_cache->second;

  return cache
      .insert({{lhs_size, rhs_size},
               {random_test_type_sorted_vec(lhs_size),
                random_test_type_sorted_vec(rhs_size)}})
      .first->second;
}

struct upper_bound_based_merge {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::upper_bound_based::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct std_merge {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return std::merge(f1, l1, f2, l2, o);
  }
};

struct libstd_merge {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::libstd::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v1 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v1::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v2 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v2::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v3 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v3::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v4 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v4::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v5 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v5::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v6 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v6::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v7 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v7::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

struct merge_v8 {
  template <typename I1, typename I2, typename O>
  O operator()(I1 f1, I1 l1, I2 f2, I2 l2, O o) {
    return srt::v8::merge(f1, l1, f2, l2, o, std::less<>{});
  }
};

}  // namespace

template <typename Merger>
void benchmark_merge(benchmark::State& state) {
  const size_t lhs_size = static_cast<size_t>(state.range(0));
  const size_t rhs_size = static_cast<size_t>(state.range(1));

  const test_merge_input& input = input_data(lhs_size, rhs_size);
  for (auto _ : state) {
    test_type_vec res(lhs_size + rhs_size);
    Merger{}(input.first.begin(), input.first.end(),
                                      input.second.begin(), input.second.end(),
                                      res.begin());
  }
}

BENCHMARK_TEMPLATE(benchmark_merge, merge_v8)
    ->Apply(set_benchmark_input_sizes);