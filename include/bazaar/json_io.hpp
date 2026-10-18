#pragma once

#include "bazaar/model.hpp"

#include <filesystem>
#include <vector>

namespace bazaar {

// Week 1. Reading and writing the files in data/.
//
// Every function throws std::runtime_error when the file cannot be opened.
// Records come back in the order they appear in the file.

// A JSON array of categories. "parent_id" may be null or missing for a root.
std::vector<Category> load_categories(const std::filesystem::path& path);

// A JSON array of products.
std::vector<Product> load_products(const std::filesystem::path& path);

// JSON Lines: one request object per line. Blank lines are skipped.
// "max_results" may be missing.
std::vector<BrowseRequest> load_requests(const std::filesystem::path& path);

// Writes a JSON array with one object per result, in the given order:
//   {"query_id": "...", "matched_count": 3, "product_ids": [102, 101]}
// "product_ids" is always an array, possibly empty.
void write_results(const std::vector<BrowseResult>& results, const std::filesystem::path& path);

} // namespace bazaar
