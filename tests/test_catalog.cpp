// Week 2: Catalog and subtree collection.

#include "bazaar/catalog.hpp"

#include <algorithm>
#include <gtest/gtest.h>

namespace bazaar {
namespace {

// Two trees:  1 -> 2 -> 3        4 -> 5
Catalog two_trees() {
    return Catalog(
        {
            {.category_id = 1, .name = "Electronics", .parent_id = std::nullopt},
            {.category_id = 2, .name = "Phones", .parent_id = 1},
            {.category_id = 3, .name = "Smartphones", .parent_id = 2},
            {.category_id = 4, .name = "Home", .parent_id = std::nullopt},
            {.category_id = 5, .name = "Kitchen", .parent_id = 4},
        },
        {});
}

std::vector<int> sorted(std::vector<int> ids) {
    std::sort(ids.begin(), ids.end());
    return ids;
}

TEST(Catalog, HasCategory) {
    const Catalog catalog = two_trees();

    EXPECT_TRUE(catalog.has_category(1));
    EXPECT_TRUE(catalog.has_category(5));
    EXPECT_FALSE(catalog.has_category(42));
}

TEST(Catalog, KeepsProducts) {
    const Catalog catalog(
        {{.category_id = 1, .name = "A", .parent_id = std::nullopt}},
        {{.product_id = 10, .name = "x", .category_id = 1, .region = "EU", .popularity_score = 1}});

    ASSERT_EQ(catalog.products().size(), 1u);
    EXPECT_EQ(catalog.products()[0].product_id, 10);
}

TEST(Subtree, IncludesRootAndAllDescendants) {
    EXPECT_EQ(sorted(two_trees().subtree(1)), (std::vector<int>{1, 2, 3}));
}

TEST(Subtree, LeafIsJustItself) {
    EXPECT_EQ(two_trees().subtree(3), (std::vector<int>{3}));
}

TEST(Subtree, UnknownCategoryIsEmpty) {
    EXPECT_TRUE(two_trees().subtree(42).empty());
}

TEST(Subtree, DoesNotLeakIntoAnotherTree) {
    EXPECT_EQ(sorted(two_trees().subtree(4)), (std::vector<int>{4, 5}));
    EXPECT_EQ(sorted(two_trees().subtree(2)), (std::vector<int>{2, 3}));
}

TEST(Subtree, CycleTerminatesAndListsEachIdOnce) {
    // 10 and 11 are each other's parent; 12 is its own parent.
    const Catalog catalog(
        {
            {.category_id = 10, .name = "A", .parent_id = 11},
            {.category_id = 11, .name = "B", .parent_id = 10},
            {.category_id = 12, .name = "C", .parent_id = 12},
        },
        {});

    EXPECT_EQ(sorted(catalog.subtree(10)), (std::vector<int>{10, 11}));
    EXPECT_EQ(catalog.subtree(12), (std::vector<int>{12}));
}

TEST(Subtree, DeepChainDoesNotOverflowTheStack) {
    // 0 -> 1 -> 2 -> ... -> 199999. A recursive walk this deep usually
    // overflows the default 1 MB stack on Windows.
    constexpr int kDepth = 200'000;
    std::vector<Category> chain;
    chain.reserve(kDepth);
    chain.push_back({.category_id = 0, .name = "root", .parent_id = std::nullopt});
    for (int id = 1; id < kDepth; ++id) {
        chain.push_back({.category_id = id, .name = "c", .parent_id = id - 1});
    }
    const Catalog catalog(std::move(chain), {});

    EXPECT_EQ(catalog.subtree(0).size(), static_cast<std::size_t>(kDepth));
}

} // namespace
} // namespace bazaar
