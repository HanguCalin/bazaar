#pragma once

#include <cstddef>
#include <optional>
#include <span>
#include <utility>

// Two sum (LeetCode 1). Timed: 20 minutes.
//
// Return indices (i, j) with i < j and nums[i] + nums[j] == target, or
// std::nullopt if there is no such pair. If several pairs exist, any one is
// fine. An element may not be used twice.
//
//   nums = [2, 7, 11, 15], target = 9  ->  (0, 1)
//
// Target: O(n) time. Say out loud what the O(n^2) version is and why you
// would not ship it.
std::optional<std::pair<std::size_t, std::size_t>> two_sum(std::span<const int> nums, int target);
