// Week 2: the whole pipeline on the files in data/, compared with
// data/expected_results.json. Needs the week 1 loaders too.

#include "bazaar/browse_engine.hpp"
#include "bazaar/catalog.hpp"
#include "bazaar/json_io.hpp"
#include "test_helpers.hpp"

#include <gtest/gtest.h>

namespace bazaar {
namespace {

using test_support::data_dir;
using test_support::read_json;
using test_support::temp_path;

TEST(EndToEnd, SampleDataMatchesExpectedResults) {
    const Catalog catalog(load_categories(data_dir() / "categories.json"),
                          load_products(data_dir() / "products.json"));
    const BrowseEngine engine(catalog);
    const auto results = engine.browse_all(load_requests(data_dir() / "requests.jsonl"));
    const auto out = temp_path("bazaar_end_to_end_results.json");

    write_results(results, out);

    // Compared as JSON values, so spacing and key order in the file do not matter.
    EXPECT_EQ(read_json(out), read_json(data_dir() / "expected_results.json"));
}

} // namespace
} // namespace bazaar
