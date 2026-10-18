# Week 1: skeleton and data loading

## Goal

A project that builds and tests the same way on your machine and on three CI
jobs, and a library that can read the three data files and write results.

## Already in place (scaffold)

| Path | What it is |
|---|---|
| `CMakeLists.txt` | the `bazaar` static library, warnings, subdirectories |
| `cmake/Dependencies.cmake` | GoogleTest and nlohmann-json, pinned by version and hash |
| `CMakePresets.json` | `debug`, `release` (local); `asan`, `tsan`, `ci-msvc` (CI) |
| `.github/workflows/ci.yml` | `windows-msvc`, `linux-gcc`, `linux-asan` |
| `include/bazaar/model.hpp` | the four record structs |
| `include/bazaar/json_io.hpp` | the four functions you implement this week |
| `tests/test_model.cpp` | passes already: proves the toolchain works |
| `tests/test_json_io.cpp` | your target this week |
| `drills/week01/` | valid parentheses, two sum |

## Your tasks

1. **First build and first push.** Follow "Build and test" below until you see
   one passing test, then `git init`, commit, add the remote and push. Check
   the Actions tab on GitHub: the three jobs build, and fail on the week 1
   tests. That is the starting point.
2. **Implement `src/json_io.cpp`** with nlohmann-json until
   `ctest --preset debug -L week1 -R "Load|Write"` is green. Things to decide
   and be able to explain:
   - how you open the file and what you throw when that fails;
   - how a missing or `null` `parent_id` becomes an empty `std::optional`;
   - how you read JSON Lines one line at a time and skip blank lines;
   - why `json_io.hpp` does not include `<nlohmann/json.hpp>` (look at
     `target_link_libraries(bazaar PRIVATE ...)` in `CMakeLists.txt`).
3. **Drills**, timed, no IDE help on the first attempt: write the solution on
   paper or in a plain editor, then type it in and run the tests.
4. **Read the CI logs once on purpose.** Push a deliberate MSVC-only or
   GCC-only mistake on a branch (ask Claude for one if you can't think of
   one), watch which job fails and read its error, then delete the branch.

## Build and test

All commands run from the project folder in PowerShell (or the VS Code
terminal).

```
cmake --preset debug           # configure: generate build files in build/debug
cmake --build --preset debug   # compile and link everything
ctest --preset debug           # run all tests
```

Useful variations:

```
ctest --preset debug -L week1           # only this week's tests (module + drills)
ctest --preset debug -L drill           # only the drills
ctest --preset debug -R LoadRequests    # only tests whose name matches a regex
ctest --preset debug -N                 # list tests without running them
build\debug\tests\test_json_io.exe      # run one test program directly
```

You only re-run the configure step when you change a `CMakeLists.txt` or a
preset. After editing `.cpp` or `.hpp` files, build and test is enough.

In VS Code: CMake Tools picks up the presets (choose `debug` if it asks).
`F7` builds, the Testing panel (flask icon) lists and runs tests, and the
status bar has build, debug and run buttons.

## Reading (learncpp.com)

- 2.1 to 2.11: functions and files (headers, `#pragma once`, the ODR)
- 5.7 to 5.9: `std::string` and `std::string_view`
- 13.7 to 13.10: structs and default member initialisers
- 12.15: `std::optional`

## Done when

- `ctest --preset debug -L week1` is green locally.
- All three CI jobs are green (CI runs `-L week1`, set by `TEST_LABELS` at the
  top of `ci.yml`).
- At least one commit per session, pushed.
