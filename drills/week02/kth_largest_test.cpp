#include "kth_largest.hpp"

#include <gtest/gtest.h>

TEST(KthLargest, Examples) {
    EXPECT_EQ(kth_largest({3, 2, 1, 5, 6, 4}, 2), 5);
    EXPECT_EQ(kth_largest({3, 2, 3, 1, 2, 4, 5, 5, 6}, 4), 4);
}

TEST(KthLargest, FirstIsMaxAndLastIsMin) {
    const std::vector<int> nums = {7, -2, 9, 0, 4};
    EXPECT_EQ(kth_largest(nums, 1), 9);
    EXPECT_EQ(kth_largest(nums, 5), -2);
}

TEST(KthLargest, DuplicatesCount) {
    EXPECT_EQ(kth_largest({5, 5, 5, 1}, 3), 5);
    EXPECT_EQ(kth_largest({5, 5, 5, 1}, 4), 1);
}

TEST(KthLargest, SingleElement) {
    EXPECT_EQ(kth_largest({42}, 1), 42);
}

TEST(KthLargest, LargeInput) {
    // 7919 is prime and shares no factor with 1'000'000, so this is a
    // permutation of 0..999999.
    std::vector<int> nums(1'000'000);
    for (std::size_t i = 0; i < nums.size(); ++i) {
        nums[i] = static_cast<int>((i * 7919) % 1'000'000);
    }
    EXPECT_EQ(kth_largest(nums, 10), 999'990);
}
