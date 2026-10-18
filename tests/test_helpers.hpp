#pragma once

#include <filesystem>
#include <fstream>
#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

namespace bazaar::test_support {

inline std::filesystem::path data_dir() {
    return BAZAAR_DATA_DIR;
}

// Writes contents to a file in the system temp directory and returns its path.
// Give every test its own file name, because ctest may run tests in parallel.
inline std::filesystem::path write_temp_file(std::string_view file_name,
                                             std::string_view contents) {
    const auto path = std::filesystem::temp_directory_path() / file_name;
    std::ofstream out(path, std::ios::binary);
    out << contents;
    return path;
}

inline std::filesystem::path temp_path(std::string_view file_name) {
    return std::filesystem::temp_directory_path() / file_name;
}

inline nlohmann::json read_json(const std::filesystem::path& path) {
    std::ifstream in(path);
    return nlohmann::json::parse(in);
}

} // namespace bazaar::test_support
