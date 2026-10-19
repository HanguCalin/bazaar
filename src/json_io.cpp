#include "bazaar/json_io.hpp"

#include <fstream>
#include <nlohmann/json.hpp>
#include <stdexcept>

namespace bazaar {

std::vector<Category> load_categories(const std::filesystem::path& path) {
    std::ifstream in(path);

    if (!in.is_open()) {
        throw std::runtime_error("Failed to open categories files: " + path.string());
    }

    nlohmann::json js;
    in >> js;

    std::vector<Category> categories;
    for (const auto& item : js) {
        Category cat;

        cat.category_id = item.at("category_id").get<int>();
        cat.name = item.at("name").get<std::string>();

        // Handle null or missing id
        if (item.contains("parent_id") && !item["parent_id"].is_null()) {
            cat.parent_id = item["parent_id"].get<int>();
        } else {
            cat.parent_id = std::nullopt;
        }

        categories.push_back(cat);
    }

    return categories;
}

std::vector<Product> load_products(const std::filesystem::path& path) {
    std::ifstream in(path);

    if (!in.is_open()) {
        throw std::runtime_error("Failed to open products files: " + path.string());
    }

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

        products.push_back(prod);
    }

    return products;
}

std::vector<BrowseRequest> load_requests(const std::filesystem::path& path) {
    std::ifstream in(path);

    if (!in.is_open()) {
        throw std::runtime_error("Failed to open request files: " + path.string());
    }

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

        if (item.contains("max_results") && !item["max_results"].is_null()) {
            req.max_results = item["max_results"].get<int>();
        } else {
            req.max_results = std::nullopt;
        }

        requests.push_back(req);
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

        out_array.push_back(item);
    }

    out << out_array.dump(4) << std::endl;
}

} // namespace bazaar
