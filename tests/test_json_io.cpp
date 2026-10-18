// Week 1: the loaders and the results writer.

#include "bazaar/json_io.hpp"
#include "test_helpers.hpp"

#include <gtest/gtest.h>
#include <stdexcept>

namespace bazaar {
namespace {

using test_support::data_dir;
using test_support::read_json;
using test_support::temp_path;
using test_support::write_temp_file;

TEST(LoadCategories, ReadsSampleFileInOrder) {
    const auto categories = load_categories(data_dir() / "categories.json");

    ASSERT_EQ(categories.size(), 9u);
    EXPECT_EQ(categories[0].category_id, 1);
    EXPECT_EQ(categories[0].name, "Electronics");
    EXPECT_FALSE(categories[0].parent_id.has_value());
    EXPECT_EQ(categories[3].category_id, 4);
    EXPECT_EQ(categories[3].parent_id, 2);
}

TEST(LoadCategories, MissingParentIdMeansRoot) {
    const auto path =
        write_temp_file("bazaar_missing_parent.json", R"([{"category_id": 7, "name": "Garden"}])");

    const auto categories = load_categories(path);

    ASSERT_EQ(categories.size(), 1u);
    EXPECT_EQ(categories[0].category_id, 7);
    EXPECT_FALSE(categories[0].parent_id.has_value());
}

TEST(LoadProducts, ReadsSampleFileInOrder) {
    const auto products = load_products(data_dir() / "products.json");

    ASSERT_EQ(products.size(), 15u);
    const Product& blender = products[9];
    EXPECT_EQ(blender.product_id, 110);
    EXPECT_EQ(blender.name, "Blender");
    EXPECT_EQ(blender.category_id, 7);
    EXPECT_EQ(blender.region, "EU-West");
    EXPECT_EQ(blender.popularity_score, 75);
}

TEST(LoadRequests, ReadsJsonLinesInOrder) {
    const auto requests = load_requests(data_dir() / "requests.jsonl");

    ASSERT_EQ(requests.size(), 8u);
    EXPECT_EQ(requests[0].query_id, "electronics-eu");
    EXPECT_EQ(requests[0].category_id, 1);
    EXPECT_EQ(requests[0].region, "EU");
    EXPECT_FALSE(requests[0].max_results.has_value());
    EXPECT_EQ(requests[1].query_id, "phones-eu-top3");
    EXPECT_EQ(requests[1].max_results, 3);
}

TEST(LoadRequests, SkipsBlankLines) {
    const auto path = write_temp_file("bazaar_blank_lines.jsonl",
                                      "\n"
                                      R"({"query_id": "a", "category_id": 1, "region": "EU"})"
                                      "\n\n"
                                      R"({"query_id": "b", "category_id": 2, "region": "US"})"
                                      "\n\n");

    const auto requests = load_requests(path);

    ASSERT_EQ(requests.size(), 2u);
    EXPECT_EQ(requests[0].query_id, "a");
    EXPECT_EQ(requests[1].query_id, "b");
}

TEST(Loaders, MissingFileThrowsRuntimeError) {
    const auto missing = data_dir() / "does_not_exist.json";

    EXPECT_THROW(load_categories(missing), std::runtime_error);
    EXPECT_THROW(load_products(missing), std::runtime_error);
    EXPECT_THROW(load_requests(missing), std::runtime_error);
}

TEST(WriteResults, WritesOneObjectPerResultInOrder) {
    const std::vector<BrowseResult> results = {
        {.query_id = "second", .matched_count = 3, .product_ids = {7, 5}},
        {.query_id = "first", .matched_count = 0, .product_ids = {}},
    };
    const auto path = temp_path("bazaar_write_results.json");

    write_results(results, path);

    const auto json = read_json(path);
    ASSERT_TRUE(json.is_array());
    ASSERT_EQ(json.size(), 2u);
    EXPECT_EQ(json[0].at("query_id").get<std::string>(), "second");
    EXPECT_EQ(json[0].at("matched_count").get<int>(), 3);
    EXPECT_EQ(json[0].at("product_ids"), nlohmann::json::array({7, 5}));
    EXPECT_EQ(json[1].at("query_id").get<std::string>(), "first");
    EXPECT_EQ(json[1].at("product_ids"), nlohmann::json::array()) << "empty array, not null";
}

} // namespace
} // namespace bazaar
