#pragma once

#include "bazaar/model.hpp"

#include <vector>

namespace bazaar {

// Week 2. Owns all categories and products. Categories form a forest through
// parent_id. The data is not trusted: it may contain cycles.
class Catalog {
public:
    Catalog(std::vector<Category> categories, std::vector<Product> products);

    [[nodiscard]] const std::vector<Product>& products() const { return products_; }

    [[nodiscard]] bool has_category(int category_id) const;

    // The ids of category_id and all of its descendants, each exactly once, in
    // no particular order. Empty if category_id is unknown. Must terminate on
    // cyclic data and must not overflow the stack on very deep trees.
    [[nodiscard]] std::vector<int> subtree(int category_id) const;

private:
    std::vector<Category> categories_;
    std::vector<Product> products_;
    // TODO(week 2): add whatever index you need so that subtree() does not
    // rescan every category for every node it visits. Build it once, in the
    // constructor.
};

} // namespace bazaar
