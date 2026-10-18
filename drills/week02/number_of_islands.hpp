#pragma once

#include <string>
#include <vector>

// Number of islands (LeetCode 200). Timed: 30 minutes.
//
// grid is a rectangle of '1' (land) and '0' (water). An island is a group of
// land cells connected up, down, left or right (not diagonally). Return the
// number of islands.
//
//   11000
//   11000
//   00100   ->  3
//   00011
//
// Target: O(rows * cols). Use an explicit stack or queue, not recursion: one
// test is a single island large enough to overflow the call stack.
int count_islands(const std::vector<std::string>& grid);
