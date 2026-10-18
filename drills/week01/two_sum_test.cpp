#include "two_sum.hpp"

#include <gtest/gtest.h>
#include <vector>

namespace {

// Checks the answer without assuming which valid pair was chosen.
void expect_valid_pair(const std::vector<int>& nums, int target) {
    const auto pair = two_sum(nums, target);
    ASSERT_TRUE(pair.has_value());
    const auto [i, j] = *pair;
    ASSERT_LT(i, j);
    ASSERT_LT(j, nums.size());
    EXPECT_EQ(nums[i] + nums[j], target);
}

TEST(TwoSum, Example) {
    const std::vector<int> nums = {2, 7, 11, 15};
    const auto pair = two_sum(nums, 9);
    ASSERT_TRUE(pair.has_value());
    EXPECT_EQ(*pair, std::make_pair(std::size_t{0}, std::size_t{1}));
}

TEST(TwoSum, PairNotAtTheStart) {
    expect_valid_pair({3, 2, 4}, 6);
}

TEST(TwoSum, DuplicateValues) {
    expect_valid_pair({3, 3}, 6);
}

TEST(TwoSum, NegativeNumbersAndZero) {
    expect_valid_pair({-3, 4, 3, 90}, 0);
    expect_valid_pair({0, 4, 3, 0}, 0);
}

TEST(TwoSum, CannotUseTheSameElementTwice) {
    const std::vector<int> nums = {3, 1};
    EXPECT_FALSE(two_sum(nums, 6).has_value());
}

TEST(TwoSum, NoSolution) {
    const std::vector<int> nums = {1, 2, 3};
    EXPECT_FALSE(two_sum(nums, 100).has_value());
    EXPECT_FALSE(two_sum(std::vector<int>{}, 0).has_value());
}

TEST(TwoSum, LargeInputIsFast) {
    // Quadratic code takes seconds here; linear code takes milliseconds.
    std::vector<int> nums(200'000);
    for (std::size_t k = 0; k < nums.size(); ++k) {
        nums[k] = static_cast<int>(k) * 2; // all even
    }
    nums.back() = 1; // the only odd number
    expect_valid_pair(nums, 1 + 2);
}

} // namespace
