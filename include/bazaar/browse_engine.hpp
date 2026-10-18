#pragma once

#include "bazaar/catalog.hpp"
#include "bazaar/model.hpp"

#include <vector>

namespace bazaar {

inline constexpr int kDefaultMaxResults = 5;

// Week 2. Answers browse requests against a Catalog. Holds a reference, so the
// Catalog must outlive the engine.
class BrowseEngine {
public:
    explicit BrowseEngine(const Catalog& catalog);

    // For one request:
    //  1. take the request's category and all its descendants (none if unknown);
    //  2. keep products in those categories whose region equals request.region;
    //  3. sort by popularity_score descending, then product_id ascending;
    //  4. matched_count = number of products after step 2;
    //  5. product_ids = the first max_results of them (kDefaultMaxResults if unset).
    [[nodiscard]] BrowseResult browse(const BrowseRequest& request) const;

    // One result per request, in the same order as the input.
    [[nodiscard]] std::vector<BrowseResult>
    browse_all(const std::vector<BrowseRequest>& requests) const;

private:
    const Catalog& catalog_;
};

} // namespace bazaar
