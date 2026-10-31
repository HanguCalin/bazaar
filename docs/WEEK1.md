# Week 1: data model and loading

## Goal

Understand what the compiler and the linker each do, then design and write the
first piece of bazaar: C++ types for the input data, loaders for the three data
files, and a writer for the results file.

## The order this week

1. **You:** the orientation exercise below (about 1.5 hours).
2. **You:** design the headers (no function bodies) and write the week 1 section
   of `docs/DESIGN.md`. Commit.
3. **Claude:** design review. You revise.
4. **Claude:** `CMakeLists.txt`, presets, pinned dependencies (GoogleTest,
   nlohmann-json), CI, and tests against your headers. You walk Claude through
   the CMake, mapping each part to a command you typed in step 1.
5. **You:** implement until the tests are green, locally and in CI.
6. **Claude:** code review. You fix.

## 1. Orientation: compile by hand

Work in a scratch folder **outside** the repository, for example
`C:\Users\clutch\Desktop\Everything\Projects\cpp-scratch`. Use PowerShell or the
VS Code terminal. Write the code yourself; it is three tiny files.

1. Write a header declaring one function (say, one that returns a greeting for a
   name), a `.cpp` that defines it, and a `main.cpp` that calls it.
2. Compile each `.cpp` on its own, then link:
   ```
   g++ -std=c++20 -Wall -Wextra -c greet.cpp -o greet.o
   g++ -std=c++20 -Wall -Wextra -c main.cpp -o main.o
   g++ main.o greet.o -o app.exe
   ```
3. Look inside:
   - `g++ -std=c++20 -E main.cpp > main.i` and open `main.i`. How many lines did
     your one `#include` turn into?
   - `nm greet.o` and `nm -C greet.o`. Find your function. What is the strange
     name in the first output?
4. Break it on purpose, and for each one note whether the **compiler** or the
   **linker** complained, and why:
   - Delete the definition from `greet.cpp`, rebuild.
   - Move the definition into the header (no `inline`), and include the header
     from a second `.cpp` that is also linked in.
   - Fix that with `inline`. Why does `inline` fix it?
   - Put a struct definition in the header, remove `#pragma once`, and include
     the header twice in one file.
5. Make a static library and link against it:
   ```
   ar rcs libgreet.a greet.o
   g++ main.o -L. -lgreet -o app.exe
   ```
   Now try `g++ -L. -lgreet main.o -o app.exe`. Why does the order matter?

Tell Claude what you found. Claude will ask you to explain each failure; that
conversation is the point of the exercise.

## 2. The spec

### Input files (in `data/`)

**`categories.json`**: a JSON array of objects.

| Field         | Type            | Notes                                         |
| ------------- | --------------- | --------------------------------------------- |
| `category_id` | integer         | required                                      |
| `name`        | string          | required                                      |
| `parent_id`   | integer or null | `null` or missing means the category is a root |

**`products.json`**: a JSON array of objects.

| Field              | Type    | Notes    |
| ------------------ | ------- | -------- |
| `product_id`       | integer | required |
| `name`             | string  | required |
| `category_id`      | integer | required |
| `region`           | string  | required |
| `popularity_score` | integer | required |

**`requests.jsonl`**: JSON Lines, one JSON object per line. Blank lines are
skipped.

| Field         | Type            | Notes                                                |
| ------------- | --------------- | ---------------------------------------------------- |
| `query_id`    | string          | required                                             |
| `category_id` | integer         | required                                             |
| `region`      | string          | required                                             |
| `max_results` | integer or null | optional; when absent, week 2 uses a default of 5    |

### Output file

**`results.json`**: a JSON array, one object per request, in request order.

| Field           | Type              | Notes                         |
| --------------- | ----------------- | ----------------------------- |
| `query_id`      | string            | copied from the request       |
| `matched_count` | integer           | filled in by week 2           |
| `product_ids`   | array of integers | always an array, even if empty |

`data/expected_results.json` shows the exact shape.

### Requirements

- Each loader takes the path of the file to read, so tests can point it at a
  temporary file instead of `data/`.
- A file that can't be opened, invalid JSON, a missing required field, or a field
  of the wrong type is an error. The error message names the file (and for JSON
  Lines, the line number).
- Loading the sample files gives 9 categories, the products in `products.json`
  and the requests in `requests.jsonl`, with every field intact.
- Writing results and reading the file back as JSON gives the same data.
- The code is in namespace `bazaar`, in a library that tests can link against.

## 3. Design questions

Answer these in `docs/DESIGN.md` (a few lines each), and let the headers show
the answers:

1. What type represents each record, and why that kind of type (struct or
   class)?
2. How does the type say that `parent_id` or `max_results` can be missing?
3. Where does the default of 5 for `max_results` live: in the loader, in the
   type, or in the week 2 engine? What does each choice cost?
4. Free functions or a loader class? What would a class hold?
5. How do callers find out that loading failed: exception, return value, or
   something else? Why?
6. Does your public header include `<nlohmann/json.hpp>`? What happens to every
   file that includes your header if it does?
7. How many headers, and which declarations go in which?

## Reading (learncpp.com)

- 2.1 to 2.11: functions and files (headers, `#pragma once`, the ODR)
- 5.7 to 5.9: `std::string` and `std::string_view`
- 13.7 to 13.10: structs and default member initialisers
- 12.15: `std::optional`

## Done when

- You can explain each failure from the orientation exercise.
- The week 1 tests are green locally and on all three CI jobs.
- `docs/DESIGN.md` has the week 1 answers, and the code review is addressed.
- At least one commit per session, pushed.
