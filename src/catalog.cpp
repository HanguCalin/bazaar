#include "bazaar/catalog.hpp"

#include <stdexcept>

namespace bazaar {

// TODO(week 2): store the data and build your index here.
Catalog::Catalog(std::vector<Category> /*categories*/, std::vector<Product> /*products*/) {}

bool Catalog::has_category(int /*category_id*/) const {
    throw std::logic_error("TODO(week 2): Catalog::has_category");
}

std::vector<int> Catalog::subtree(int /*category_id*/) const {
    throw std::logic_error("TODO(week 2): Catalog::subtree");
}

} // namespace bazaar
