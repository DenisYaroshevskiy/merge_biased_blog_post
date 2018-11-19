#include "other_algorithms.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <random>
#include <utility>
#include <vector>

#include "third_party/catch.h"

namespace {

struct zeroed_int {
  int body = 0;

  zeroed_int() = default;
  explicit zeroed_int(int body) : body{body} {}
  zeroed_int(const zeroed_int&) = delete;
  zeroed_int(zeroed_int&& x) noexcept : body{std::exchange(x.body, 0)} {}
  zeroed_int& operator=(const zeroed_int&) = delete;
  zeroed_int& operator=(zeroed_int&& x) noexcept {
    body = std::exchange(x.body, 0);
    return *this;
  }
  ~zeroed_int() = default;

  friend bool operator==(const zeroed_int& x, const zeroed_int& y) {
    return x.body == y.body;
  }

  friend bool operator<(const zeroed_int& x, const zeroed_int& y) {
    return x.body < y.body;
  }
};

using test_t = std::pair<zeroed_int, int>;

template <template <typename, typename> class C,
          template <typename, typename> class InC>
C<test_t, std::allocator<test_t>> copy_test_t_container(
    const InC<test_t, std::allocator<test_t>>& in) {
  C<test_t, std::allocator<test_t>> res;
  std::transform(in.begin(), in.end(), std::back_inserter(res),
                 [](const test_t& x) {
                   return test_t{zeroed_int{x.first.body}, x.second};
                 });
  return res;
}

template <typename Merger>
void run_plain_int_test(const int* f1, const int* l1, const int* f2, const int* l2, Merger merger) {
  std::size_t res_size = static_cast<std::size_t>((l1 - f1) + (l2 - f2));
  std::vector<int> actual(res_size);
  std::vector<int> expected(res_size);

  std::merge(f1, l1, f2, l2, expected.begin());
  merger(f1, l1, f2, l2, actual.begin());
  REQUIRE(expected == actual);
}

// Using back_inserter to emulate a nasty output iterator.
template <typename Merger, template <typename, typename> class LhsC,
          template <typename, typename> class RhsC>
void run_merge_test(LhsC<test_t, std::allocator<test_t>> lhs,
                    RhsC<test_t, std::allocator<test_t>> rhs, Merger merger) {
  auto lhs_copy = copy_test_t_container<LhsC>(lhs);
  auto rhs_copy = copy_test_t_container<RhsC>(rhs);

  std::vector<test_t> actual_output;
  merger(std::make_move_iterator(lhs.begin()),
         std::make_move_iterator(lhs.end()),
         std::make_move_iterator(rhs.begin()),
         std::make_move_iterator(rhs.end()), std::back_inserter(actual_output));

  lhs = std::move(lhs_copy);
  rhs = std::move(rhs_copy);

  std::vector<test_t> expected_output;

  std::merge(
      std::make_move_iterator(lhs.begin()), std::make_move_iterator(lhs.end()),
      std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()),
      std::back_inserter(expected_output));

  REQUIRE(expected_output == actual_output);
}

constexpr std::size_t kTestSize = 200;

const std::vector<int>& test_data() {
  static std::vector<int> res = [] {
    std::mt19937 g;
    std::uniform_int_distribution<> dis(1, int(kTestSize) * 100);

    std::vector<int> res(kTestSize);
    std::generate(res.begin(), res.end(), [&]() mutable { return dis(g); });

    return res;
  }();
  return res;
}

template <typename Merger>
void test_merge(Merger merger) {
  const auto& test_ints = test_data();

  for (std::size_t total_size = 0; total_size <= kTestSize; ++total_size) {
    for (std::size_t lhs_size = 0; lhs_size <= total_size; ++lhs_size) {
      std::vector<test_t> vector_test_lhs, vector_test_rhs;

      const int* f_ints = &test_ints[0];
      const int* m_ints = f_ints + lhs_size;
      const int* l_ints = m_ints + (total_size - lhs_size);

      run_plain_int_test(f_ints, m_ints, m_ints, l_ints, merger);

      // Generate test data
      vector_test_lhs.resize(lhs_size);
      std::transform(f_ints, m_ints, vector_test_lhs.begin(), [](int in) {
        return test_t{zeroed_int(in), 0};
      });

      vector_test_rhs.resize(total_size - lhs_size);
      std::transform(m_ints, l_ints, vector_test_rhs.begin(), [](int in) {
        return test_t{zeroed_int(in), 1};
      });

      std::sort(vector_test_lhs.begin(), vector_test_lhs.end());
      std::sort(vector_test_rhs.begin(), vector_test_rhs.end());

      // Copy to lists
      auto list_test_lhs = copy_test_t_container<std::list>(vector_test_lhs);
      auto list_test_rhs = copy_test_t_container<std::list>(vector_test_rhs);

      // Run all tests.
      run_merge_test(std::move(vector_test_lhs), std::move(vector_test_rhs),
                     merger);
      run_merge_test(std::move(list_test_lhs), std::move(list_test_rhs),
                     merger);
    }
  }
}

}  // namespace

TEST_CASE("libstd_merge") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::libstd::merge(f1, l1, f2, l2, o, std::less<>{});
  });
}

TEST_CASE("merge_v1") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v1::merge(f1, l1, f2, l2, o, std::less<>{});
  });
}


TEST_CASE("merge_v2") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v2::merge(f1, l1, f2, l2, o, std::less<>{});
  });
}
