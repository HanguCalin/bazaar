#include "number_of_islands.hpp"

#include <gtest/gtest.h>

TEST(NumberOfIslands, Example) {
    EXPECT_EQ(count_islands({"11000", "11000", "00100", "00011"}), 3);
}

TEST(NumberOfIslands, OneBigIsland) {
    EXPECT_EQ(count_islands({"11110", "11010", "11000", "00000"}), 1);
}

TEST(NumberOfIslands, EmptyAndAllWater) {
    EXPECT_EQ(count_islands({}), 0);
    EXPECT_EQ(count_islands({"000", "000"}), 0);
}

TEST(NumberOfIslands, DiagonalCellsAreSeparateIslands) {
    EXPECT_EQ(count_islands({"101", "010", "101"}), 5);
}

TEST(NumberOfIslands, SingleRowAndSingleColumn) {
    EXPECT_EQ(count_islands({"1011101"}), 3);
    EXPECT_EQ(count_islands({"1", "1", "0", "1"}), 2);
}

TEST(NumberOfIslands, GridIsNotModified) {
    const std::vector<std::string> grid = {"110", "011"};
    const auto copy = grid;
    EXPECT_EQ(count_islands(grid), 1);
    EXPECT_EQ(grid, copy);
}

TEST(NumberOfIslands, HugeIslandDoesNotOverflowTheStack) {
    const std::vector<std::string> grid(1000, std::string(1000, '1'));
    EXPECT_EQ(count_islands(grid), 1);
}
