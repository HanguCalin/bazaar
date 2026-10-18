cmake_minimum_required(VERSION 3.25)

project(bazaar VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# MinGW: link the C++ runtime into every .exe so it never picks up a wrong
# libstdc++-6.dll from somewhere else on PATH.
if(MINGW)
    add_link_options(-static)
endif()

include(cmake/Dependencies.cmake)

# Warning flags, attached to our own targets only (not to GoogleTest or json).
add_library(bazaar_warnings INTERFACE)
if(MSVC)
    target_compile_options(bazaar_warnings INTERFACE /W4 /permissive- /utf-8 /Zc:__cplusplus)
else()
    target_compile_options(bazaar_warnings INTERFACE -Wall -Wextra -Wpedantic -Wshadow)
endif()

# The library: all the logic lives here. The app and the tests link against it.
add_library(bazaar STATIC
    src/browse_engine.cpp
    src/catalog.cpp
    src/json_io.cpp
)
target_include_directories(bazaar PUBLIC include)
# PRIVATE: nlohmann-json is an implementation detail. No public header includes
# it, so code that uses bazaar does not need it.
target_link_libraries(bazaar
    PRIVATE nlohmann_json::nlohmann_json bazaar_warnings
)

add_subdirectory(apps/browse_cli)

enable_testing()
add_subdirectory(tests)
