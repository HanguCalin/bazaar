#include "valid_parentheses.hpp"

#include <gtest/gtest.h>
#include <string>

TEST(ValidParentheses, Examples) {
    EXPECT_TRUE(is_valid_parentheses("()"));
    EXPECT_TRUE(is_valid_parentheses("()[]{}"));
    EXPECT_FALSE(is_valid_parentheses("(]"));
    EXPECT_FALSE(is_valid_parentheses("([)]"));
    EXPECT_TRUE(is_valid_parentheses("{[]}"));
}

TEST(ValidParentheses, EmptyStringIsValid) {
    EXPECT_TRUE(is_valid_parentheses(""));
}

TEST(ValidParentheses, UnbalancedEitherWay) {
    EXPECT_FALSE(is_valid_parentheses("("));
    EXPECT_FALSE(is_valid_parentheses(")"));
    EXPECT_FALSE(is_valid_parentheses("(()"));
    EXPECT_FALSE(is_valid_parentheses("())"));
    EXPECT_FALSE(is_valid_parentheses("]["));
}

TEST(ValidParentheses, DeepNesting) {
    const std::string deep = std::string(100'000, '(') + std::string(100'000, ')');
    EXPECT_TRUE(is_valid_parentheses(deep));
    EXPECT_FALSE(is_valid_parentheses(deep + "("));
}
