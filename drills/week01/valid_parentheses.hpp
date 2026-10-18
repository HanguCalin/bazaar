#pragma once

#include <string_view>

// Valid parentheses (LeetCode 20). Timed: 20 minutes.
//
// s contains only the characters ()[]{}. Return true if every bracket is
// closed by the same kind of bracket, in the correct order. The empty string
// is valid.
//
//   "()[]{}" -> true     "(]" -> false     "([)]" -> false     "{[]}" -> true
//
// Target: O(n) time.
bool is_valid_parentheses(std::string_view s);
