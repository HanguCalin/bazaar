#pragma once

#include <optional>
#include <string>
#include <vector>

namespace bazaar {

// Plain data records, one per kind of JSON object in data/. They have no
// invariants to protect, so they are structs with public members.

struct Category {
    int category_id = 0;
    std::string name;
    std::optional<int> parent_id; // empty for a root category
};

struct Product {
    int product_id = 0;
    std::string name;
    int category_id = 0;
    std::string region;
    int popularity_score = 0;
};

struct BrowseRequest {
    std::string query_id;
    int category_id = 0;
    std::string region;
    std::optional<int> max_results; // empty when the request did not set it
};

struct BrowseResult {
    std::string query_id;
    int matched_count = 0;        // number of matches before truncation
    std::vector<int> product_ids; // at most max_results ids, best first
};

} // namespace bazaar
