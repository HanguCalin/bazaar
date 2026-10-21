#include "bazaar/json_io.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>
#include <string>
#include <utility>

namespace bazaar {

namespace {

std::ifstream open_input(const std::filesystem::path& path) {
    std::ifstream in(path);

    if (!in.is_open()) {
        throw std::runtime_error("Failed to open input file: " + path.string());
    }

    return in;
}

} // namespace

std::vector<Category> load_categories(const std::filesystem::path& path) {
    std::ifstream in = open_input(path);

    nlohmann::json js;
    in >> js;

    std::vector<Category> categories;
    for (const auto& item : js) {
        Category cat;

        cat.category_id = item.at("category_id").get<int>();
        cat.name = item.at("name").get<std::string>();

        // A missing or null parent_id leaves the optional empty: a root.
        if (item.contains("parent_id") && !item.at("parent_id").is_null()) {
            cat.parent_id = item.at("parent_id").get<int>();
        }

        categories.push_back(std::move(cat));
    }

    return categories;
}

std::vector<Product> load_products(const std::filesystem::path& path) {
    std::ifstream in = open_input(path);

    nlohmann::json js;
    in >> js;

    std::vector<Product> products;
    for (const auto& item : js) {
        Product prod;

        prod.product_id = item.at("product_id").get<int>();
        prod.category_id = item.at("category_id").get<int>();
        prod.name = item.at("name").get<std::string>();
        prod.region = item.at("region").get<std::string>();
        prod.popularity_score = item.at("popularity_score").get<int>();

        products.push_back(std::move(prod));
    }

    return products;
}

std::vector<BrowseRequest> load_requests(const std::filesystem::path& path) {
    std::ifstream in = open_input(path);

    std::vector<BrowseRequest> requests;
    std::string line;

    while (std::getline(in, line)) {
        if (line.empty()) {
            continue;
        }

        nlohmann::json item = nlohmann::json::parse(line);
        BrowseRequest req;

        req.query_id = item.at("query_id").get<std::string>();
        req.category_id = item.at("category_id").get<int>();
        req.region = item.at("region").get<std::string>();

        // A missing or null max_results leaves the optional empty.
        if (item.contains("max_results") && !item.at("max_results").is_null()) {
            req.max_results = item.at("max_results").get<int>();
        }

        requests.push_back(std::move(req));
    }

    return requests;
}

void write_results(const std::vector<BrowseResult>& results, const std::filesystem::path& path) {
    std::ofstream out(path);
    if (!out.is_open()) {
        throw std::runtime_error("Failed to open output file for writing: " + path.string());
    }

    nlohmann::json out_array = nlohmann::json::array();
    for (const auto& res : results) {
        nlohmann::json item;
        item["query_id"] = res.query_id;
        item["matched_count"] = res.matched_count;
        item["product_ids"] = res.product_ids;

        out_array.push_back(std::move(item));
    }

    out << out_array.dump(4) << std::endl;
}

} // namespace bazaar
