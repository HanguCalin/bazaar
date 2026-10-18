#pragma once

#include <vector>

// Kth largest element in an array (LeetCode 215). Timed: 25 minutes.
//
// Return the k-th largest element (1-based, counting duplicates), so k = 1 is
// the maximum and k = nums.size() is the minimum. Precondition:
// 1 <= k <= nums.size().
//
//   nums = [3, 2, 3, 1, 2, 4, 5, 5, 6], k = 4  ->  4
//
// Be ready to explain three approaches and their costs: full sort, a min-heap
// of size k, and std::nth_element. Why does this function take nums by value?
int kth_largest(std::vector<int> nums, int k);
