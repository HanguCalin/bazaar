#include "bazaar/json_io.hpp"

#include <nlohmann/json.hpp>
#include <stdexcept>

namespace bazaar {

// TODO(week 1): implement these four functions with nlohmann::json.
// Uncomment the parameter names when you use them.

std::vector<Category> load_categories(const std::filesystem::path& /*path*/) {
    throw std::logic_error("TODO(week 1): load_categories");
}

std::vector<Product> load_products(const std::filesystem::path& /*path*/) {
    throw std::logic_error("TODO(week 1): load_products");
}

std::vector<BrowseRequest> load_requests(const std::filesystem::path& /*path*/) {
    throw std::logic_error("TODO(week 1): load_requests");
}

void write_results(const std::vector<BrowseResult>& /*results*/,
                   const std::filesystem::path& /*path*/) {
    throw std::logic_error("TODO(week 1): write_results");
}

} // namespace bazaar
