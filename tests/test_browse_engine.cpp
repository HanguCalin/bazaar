// Week 2: BrowseEngine against a small catalog built in code.

#include "bazaar/browse_engine.hpp"

#include <gtest/gtest.h>

namespace bazaar {
namespace {

// Categories: 1 -> 2 -> 3 and a separate tree 4 -> 5.
// Product 102 comes before 101 on purpose: they tie on score, so only a real
// product_id tie-breaker puts 101 first.
Catalog make_catalog() {
    return Catalog(
        {
            {.category_id = 1, .name = "Electronics", .parent_id = std::nullopt},
            {.category_id = 2, .name = "Phones", .parent_id = 1},
            {.category_id = 3, .name = "Smartphones", .parent_id = 2},
            {.category_id = 4, .name = "Home", .parent_id = std::nullopt},
            {.category_id = 5, .name = "Kitchen", .parent_id = 4},
        },
        {
            {.product_id = 100,
             .name = "Cable",
             .category_id = 1,
             .region = "EU",
             .popularity_score = 10},
            {.product_id = 102,
             .name = "Case",
             .category_id = 3,
             .region = "EU",
             .popularity_score = 50},
            {.product_id = 101,
             .name = "Charger",
             .category_id = 2,
             .region = "EU",
             .popularity_score = 50},
            {.product_id = 103,
             .name = "Pixel",
             .category_id = 3,
             .region = "EU",
             .popularity_score = 70},
            {.product_id = 104,
             .name = "Galaxy",
             .category_id = 3,
             .region = "US",
             .popularity_score = 99},
            {.product_id = 105,
             .name = "iPhone",
             .category_id = 3,
             .region = "EU-West",
             .popularity_score = 99},
            {.product_id = 106,
             .name = "Kettle",
             .category_id = 5,
             .region = "EU",
             .popularity_score = 80},
            {.product_id = 107,
             .name = "Stand",
             .category_id = 2,
             .region = "EU",
             .popularity_score = 20},
            {.product_id = 108,
             .name = "Speaker",
             .category_id = 1,
             .region = "EU",
             .popularity_score = 30},
        });
}

class BrowseTest : public ::testing::Test {
protected:
    const Catalog catalog_ = make_catalog();
    const BrowseEngine engine_{catalog_};
};

TEST_F(BrowseTest, MatchesCategoryAndDescendants) {
    const auto result =
        engine_.browse({.query_id = "q", .category_id = 2, .region = "EU", .max_results = 10});

    EXPECT_EQ(result.query_id, "q");
    EXPECT_EQ(result.matched_count, 4);
    EXPECT_EQ(result.product_ids, (std::vector<int>{103, 101, 102, 107}));
}

TEST_F(BrowseTest, UnknownCategoryGivesZeroMatches) {
    const auto result =
        engine_.browse({.query_id = "none", .category_id = 999, .region = "EU", .max_results = 10});

    EXPECT_EQ(result.query_id, "none");
    EXPECT_EQ(result.matched_count, 0);
    EXPECT_TRUE(result.product_ids.empty());
}

TEST_F(BrowseTest, RegionMustMatchExactly) {
    const auto eu =
        engine_.browse({.query_id = "eu", .category_id = 3, .region = "EU", .max_results = 10});
    EXPECT_EQ(eu.product_ids, (std::vector<int>{103, 102})) << "EU-West is not EU";

    const auto eu_west =
        engine_.browse({.query_id = "w", .category_id = 3, .region = "EU-West", .max_results = 10});
    EXPECT_EQ(eu_west.product_ids, (std::vector<int>{105}));

    const auto lower =
        engine_.browse({.query_id = "l", .category_id = 3, .region = "eu", .max_results = 10});
    EXPECT_EQ(lower.matched_count, 0) << "comparison is case-sensitive";
}

TEST_F(BrowseTest, TiesOnScoreBreakByProductIdAscending) {
    const auto result =
        engine_.browse({.query_id = "t", .category_id = 2, .region = "EU", .max_results = 10});

    // 101 and 102 both score 50.
    EXPECT_EQ(result.product_ids, (std::vector<int>{103, 101, 102, 107}));
}

TEST_F(BrowseTest, MaxResultsDefaultsToFive) {
    const auto result = engine_.browse(
        {.query_id = "d", .category_id = 1, .region = "EU", .max_results = std::nullopt});

    EXPECT_EQ(result.product_ids, (std::vector<int>{103, 101, 102, 108, 107}));
}

TEST_F(BrowseTest, TruncatesToMaxResults) {
    const auto result =
        engine_.browse({.query_id = "t", .category_id = 1, .region = "EU", .max_results = 2});

    EXPECT_EQ(result.product_ids, (std::vector<int>{103, 101}));
}

TEST_F(BrowseTest, MatchedCountIsBeforeTruncation) {
    const auto result =
        engine_.browse({.query_id = "c", .category_id = 1, .region = "EU", .max_results = 2});

    EXPECT_EQ(result.matched_count, 6);
}

TEST_F(BrowseTest, DoesNotMatchAnotherTree) {
    const auto home =
        engine_.browse({.query_id = "h", .category_id = 4, .region = "EU", .max_results = 10});
    EXPECT_EQ(home.product_ids, (std::vector<int>{106}));

    const auto electronics =
        engine_.browse({.query_id = "e", .category_id = 1, .region = "EU", .max_results = 10});
    EXPECT_EQ(electronics.matched_count, 6) << "Kettle (106) belongs to Home, not Electronics";
}

TEST(Browse, CyclicCategoriesAreSafe) {
    const Catalog catalog(
        {
            {.category_id = 1, .name = "A", .parent_id = 2},
            {.category_id = 2, .name = "B", .parent_id = 1},
        },
        {
            {.product_id = 10,
             .name = "x",
             .category_id = 1,
             .region = "EU",
             .popularity_score = 5},
            {.product_id = 11,
             .name = "y",
             .category_id = 2,
             .region = "EU",
             .popularity_score = 6},
        });
    const BrowseEngine engine(catalog);

    const auto result =
        engine.browse({.query_id = "c", .category_id = 1, .region = "EU", .max_results = 10});

    EXPECT_EQ(result.matched_count, 2);
    EXPECT_EQ(result.product_ids, (std::vector<int>{11, 10}));
}

TEST_F(BrowseTest, BrowseAllKeepsInputOrder) {
    const std::vector<BrowseRequest> requests = {
        {.query_id = "zeta", .category_id = 4, .region = "EU", .max_results = std::nullopt},
        {.query_id = "alpha", .category_id = 1, .region = "EU", .max_results = 1},
        {.query_id = "mu", .category_id = 999, .region = "EU", .max_results = std::nullopt},
    };

    const auto results = engine_.browse_all(requests);

    ASSERT_EQ(results.size(), 3u);
    EXPECT_EQ(results[0].query_id, "zeta");
    EXPECT_EQ(results[1].query_id, "alpha");
    EXPECT_EQ(results[1].product_ids, (std::vector<int>{103}));
    EXPECT_EQ(results[2].query_id, "mu");
}

} // namespace
} // namespace bazaar
