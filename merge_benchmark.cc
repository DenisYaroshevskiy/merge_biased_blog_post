#include <benchmark/benchmark.h>

#include <algorithm>
#include <cstdint>
#include <forward_list>
#include <random>
#include <set>

#include "../partition_point_biased_blog_post/result.h"
#include "result.h"

namespace {

constexpr std::size_t kProblemSize = 1000u;
constexpr std::size_t kStep = 1;
constexpr std::size_t kMaxIdx = kProblemSize;

const std::vector<std::int64_t>& ints_test() {
  static const auto res = [] () -> std::vector<std::int64_t> {
    std::mt19937 g;
    std::uniform_int_distribution<std::int64_t> dis(
        1, static_cast<std::int64_t>(kProblemSize) * 10);

    std::set<std::int64_t> unique_sorted_ints;
    while (unique_sorted_ints.size() < kProblemSize)
      unique_sorted_ints.insert(dis(g));

    return {unique_sorted_ints.begin(), unique_sorted_ints.end()};
  }();

  return res;
}

void set_looking_for_index(benchmark::internal::Benchmark* bench) {
  for (std::size_t looking_for_idx = 0; looking_for_idx < kMaxIdx; looking_for_idx += kStep)
    bench->Arg(static_cast<int>(looking_for_idx));
}

struct binary {
  template <typename I, typename V>
  I operator()(I f, I l, const V& v) {
    return std::lower_bound(f, l, v);
  }
};

}  // namespace

template <typename Searcher>
void benchmark_search(benchmark::State& state) {
  auto input = ints_test();
  auto looking_for = input[static_cast<std::size_t>(state.range(0))];

  for (auto _ : state)
    benchmark::DoNotOptimize(Searcher{}(input.begin(), input.end(), looking_for));
}

BENCHMARK_TEMPLATE(benchmark_search, binary)->Apply(set_looking_for_index);