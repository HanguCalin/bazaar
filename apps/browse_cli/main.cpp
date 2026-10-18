// browse_cli: reads the catalog and the requests, writes the results.
// All the logic is in the bazaar library; this file only wires it together.

#include "bazaar/browse_engine.hpp"
#include "bazaar/catalog.hpp"
#include "bazaar/json_io.hpp"

#include <exception>
#include <iostream>

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "usage: browse_cli <categories.json> <products.json> <requests.jsonl> "
                     "<results.json>\n";
        return 2;
    }

    try {
        const bazaar::Catalog catalog(bazaar::load_categories(argv[1]),
                                      bazaar::load_products(argv[2]));
        const bazaar::BrowseEngine engine(catalog);
        const auto results = engine.browse_all(bazaar::load_requests(argv[3]));
        bazaar::write_results(results, argv[4]);
    } catch (const std::exception& e) {
        std::cerr << "browse_cli: " << e.what() << '\n';
        return 1;
    }
    return 0;
}
