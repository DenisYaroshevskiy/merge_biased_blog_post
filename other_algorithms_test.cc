#include "third_party/catch.h"

#include "other_algorithms.h"

#include <algorithm>
#include <iostream>
#include <list>
#include <random>
#include <utility>
#include <vector>

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

struct stability_less {
  bool operator()(int x, int y) const { return x < y; }

  template <typename T, typename U>
  bool operator()(const std::pair<T, U>& x, const std::pair<T, U>& y) const {
    return x.first < y.first;
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

template <typename R, typename Merger>
void run_plain_test(const R& lhs_in, const R& rhs_in, Merger merger) {
  std::size_t res_size =
      static_cast<std::size_t>(lhs_in.size() + rhs_in.size());

  using value_type = std::pair<typename R::value_type, int>;

  std::vector<value_type> lhs(lhs_in.size());
  std::transform(lhs_in.begin(), lhs_in.end(), lhs.begin(),
                 [](auto x) { return std::make_pair(x, 0); });

  std::vector<value_type> rhs(rhs_in.size());
  std::transform(rhs_in.begin(), rhs_in.end(), rhs.begin(),
                 [](auto x) { return std::make_pair(x, 1); });

  std::vector<value_type> actual(res_size);
  std::vector<value_type> expected(res_size);

  std::merge(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), expected.begin(),
             stability_less{});
  merger(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(), actual.begin());
  REQUIRE(expected == actual);
}

constexpr std::size_t kTestSize = 200;

// Using back_inserter to emulate a nasty output iterator.
template <typename Merger, template <typename, typename> class LhsC,
          template <typename, typename> class RhsC>
void run_merge_test(LhsC<test_t, std::allocator<test_t>> lhs,
                    RhsC<test_t, std::allocator<test_t>> rhs, Merger merger) {
  auto lhs_copy = copy_test_t_container<LhsC>(lhs);
  auto rhs_copy = copy_test_t_container<RhsC>(rhs);

  std::vector<test_t> actual_output;
  actual_output.reserve(kTestSize);

  merger(std::make_move_iterator(lhs.begin()),
         std::make_move_iterator(lhs.end()),
         std::make_move_iterator(rhs.begin()),
         std::make_move_iterator(rhs.end()), std::back_inserter(actual_output));

  lhs = std::move(lhs_copy);
  rhs = std::move(rhs_copy);

  std::vector<test_t> expected_output;
  expected_output.reserve(kTestSize);

  std::merge(
      std::make_move_iterator(lhs.begin()), std::make_move_iterator(lhs.end()),
      std::make_move_iterator(rhs.begin()), std::make_move_iterator(rhs.end()),
      std::back_inserter(expected_output), stability_less{});

  REQUIRE(expected_output == actual_output);
}

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
void special_cases(Merger merger) {
  std::vector<int> lhs(1, 100);
  std::fill(lhs.begin() + 30, lhs.end(), 3);
  std::vector<int> rhs{3};
  run_plain_test(lhs, rhs, merger);
}

template <typename Merger>
void test_merge(Merger merger) {
  special_cases(merger);

  const auto& test_ints = test_data();

  for (std::size_t total_size = 0; total_size <= kTestSize; ++total_size) {
    for (std::size_t lhs_size = 0; lhs_size <= total_size; ++lhs_size) {
      std::vector<int> lhs_ints{test_ints.begin(),
                                test_ints.begin() + lhs_size};
      std::vector<int> rhs_ints{test_ints.begin() + lhs_size,
                                test_ints.begin() + total_size};

      std::sort(lhs_ints.begin(), lhs_ints.end());
      std::sort(rhs_ints.begin(), rhs_ints.end());

      run_plain_test(lhs_ints, rhs_ints, merger);

      std::vector<test_t> vector_test_lhs, vector_test_rhs;

      // Generate test data
      vector_test_lhs.resize(lhs_size);
      std::transform(lhs_ints.begin(), lhs_ints.end(), vector_test_lhs.begin(),
                     [](int in) {
                       return test_t{zeroed_int(in), 0};
                     });

      vector_test_rhs.resize(total_size - lhs_size);
      std::transform(rhs_ints.begin(), rhs_ints.end(), vector_test_rhs.begin(),
                     [](int in) {
                       return test_t{zeroed_int(in), 1};
                     });

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
    return srt::libstd::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v1") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v1::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v2") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v2::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v3") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v3::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v4") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v4::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v5") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v5::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v6") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v6::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v7") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v7::merge(f1, l1, f2, l2, o, stability_less{});
  });
}

TEST_CASE("merge_v8") {
  test_merge([](auto f1, auto l1, auto f2, auto l2, auto o) {
    return srt::v8::merge(f1, l1, f2, l2, o, stability_less{});
  });
}