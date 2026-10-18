#include "bazaar/browse_engine.hpp"

#include <stdexcept>

namespace bazaar {

BrowseEngine::BrowseEngine(const Catalog& catalog) : catalog_(catalog) {}

BrowseResult BrowseEngine::browse(const BrowseRequest& /*request*/) const {
    throw std::logic_error("TODO(week 2): BrowseEngine::browse");
}

std::vector<BrowseResult>
BrowseEngine::browse_all(const std::vector<BrowseRequest>& /*requests*/) const {
    throw std::logic_error("TODO(week 2): BrowseEngine::browse_all");
}

} // namespace bazaar
