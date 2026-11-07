#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <vector>

namespace bazaar {

class Category {
private:
    int id;
    std::string name;
    std::optional<int> parent_id;

public:
    explicit Category(int id, std::string name, std::optional<int> parent_id);

    int get_id() const;
    std::string get_name() const;
    std::optional<int> get_parent_id() const;
};

class Product {
private:
    int id;
    std::string name;
    int category_id;
    std::string region;
    int popularity_score;

public:
    explicit Product(int id, std::string name, int category_id, std::string region, int popularity_score);

    int get_id() const;
    std::string get_name() const;
    int get_category_id() const;
    std::string get_region() const;
    int get_popularity_score() const;
};

class Result {
private:
    std::string query_id;
    int matched_count;
    std::vector<int> product_ids;

public:
    explicit Result(std::string query_id, int matched_count, std::vector<int> product_ids);

    std::string get_query_id() const;
    int get_matched_count() const;
    std::vector<int> get_product_ids() const;
};

class Request {
private:
    std::string query_id;
    int category_id;
    std::string region;
    std::optional<int> max_results;

public:
    explicit Request(std::string query_id, int category_id, std::string region, std::optional<int> max_results);

    std::string get_query_id() const;
    int get_category_id() const;
    std::string get_region() const;
    std::optional<int> get_max_results() const;
};

class DataError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error; // inherit constructors from std::runtime_error
};

// Reads the categories from the JSON file at `path`.
//
// The file must hold a JSON array of objects. `parent_id` may be missing or
// null; both load as std::nullopt. Unknown fields are ignored.
//
// Throws DataError if the file can't be opened, isn't valid JSON, isn't an
// array, contains an element that isn't an object, has a required field that
// is missing, null or of the wrong type (such as "3" or 3.5 for an id), or has
// a `parent_id` that is present but not an integer. The message contains
// `path` exactly as passed in.
std::vector<Category> load_categories(const std::string& path);

// Reads the products from the JSON file at `path`.
//
// The file must hold a JSON array of objects. Every field is required.
// Unknown fields are ignored.
//
// Throws DataError if the file can't be opened, isn't valid JSON, isn't an
// array, contains an element that isn't an object, or has a field that is
// missing, null or of the wrong type (such as "3" or 3.5 for an id).
// The message contains `path` exactly as passed in.
std::vector<Product> load_products(const std::string& path);

// Reads the requests from the JSON Lines file at `path`: one JSON object per
// line. Lines that are empty or only whitespace are skipped. `max_results` may
// be missing or null; both load as std::nullopt (the default of 5 is applied by
// the search, not here). Unknown fields are ignored.
//
// Throws DataError if the file can't be opened, a line isn't valid JSON or
// isn't an object, a required field is missing, null or of the wrong type, or
// `max_results` is present but not an integer. The message contains `path`
// exactly as passed in, and `line N` for the line at fault, counting every line
// from 1, skipped ones included.
std::vector<Request> load_requests(const std::string& path);

// Writes `results` to `path` as a JSON array, one object per result, in the
// given order. An empty `product_ids` is written as [], and no results as [].
// An existing file is replaced.
//
// Throws DataError if the file can't be opened for writing (for example, a path
// inside a folder that doesn't exist). The message contains `path` exactly as
// passed in.
void write_results(const std::string& path, const std::vector<Result>& results);

} // namespace bazaar
