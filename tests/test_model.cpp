// Week 1. This one passes from the start: it proves the toolchain, CMake and
// GoogleTest all work before you write any code.

#include "bazaar/model.hpp"

#include <gtest/gtest.h>

namespace bazaar {
namespace {

TEST(Model, DefaultMemberInitialisers) {
    const Category category;
    EXPECT_EQ(category.category_id, 0);
    EXPECT_FALSE(category.parent_id.has_value());

    const Product product;
    EXPECT_EQ(product.product_id, 0);
    EXPECT_EQ(product.popularity_score, 0);
    EXPECT_TRUE(product.region.empty());

    const BrowseRequest request;
    EXPECT_FALSE(request.max_results.has_value());

    const BrowseResult result;
    EXPECT_EQ(result.matched_count, 0);
    EXPECT_TRUE(result.product_ids.empty());
}

} // namespace
} // namespace bazaar
