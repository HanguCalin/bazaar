# Week 1: data model and loading

## Goal

Understand what the compiler and the linker each do. Then design and write the
first piece of bazaar: C++ types for the input data, loaders for the three data
files, and a writer for the results file.

This file lists the tasks and the exact spec. Background for every task is in
[WEEK1_EXPLANATIONS.md](WEEK1_EXPLANATIONS.md); each task says which section to
read ("Explanations §N").

## The order this week

1. **You:** the orientation exercise (Task 1, about 1.5 hours).
2. **You:** design the headers (no function bodies) and write the week 1 section
   of `docs/DESIGN.md` (Task 2). Commit.
3. **Claude:** design review. You revise.
4. **Claude:** `CMakeLists.txt`, presets, pinned dependencies (GoogleTest,
   nlohmann-json), CI, and tests against your headers. You walk Claude through
   the CMake, mapping each part to a command you typed in Task 1.
5. **You:** implement until the tests are green, locally and in CI.
6. **Claude:** code review. You fix.

---

## Task 1. Orientation: compile by hand

Read first: Explanations §1 (how C++ gets built).

Work in a scratch folder **outside** the repository, for example
`C:\Users\clutch\Desktop\Everything\Projects\cpp-scratch`. Use PowerShell or the
VS Code terminal. Write the code yourself; it is three tiny files.

Before each step that breaks something, write down your prediction: compiler or
linker, and why. Then run it and compare.

### 1.1 Write three files

- `greet.h`: `#pragma once`, `#include <string>`, and the declaration of one
  function that returns a greeting for a name.
- `greet.cpp`: includes `greet.h` and holds the definition.
- `main.cpp`: includes `greet.h` and `<iostream>`, calls the function, prints
  the result.

The shape, on a different function: Explanations §2.1.

### 1.2 Compile each file on its own, then link

```
g++ -std=c++20 -Wall -Wextra -c greet.cpp -o greet.o
g++ -std=c++20 -Wall -Wextra -c main.cpp -o main.o
g++ main.o greet.o -o app.exe
.\app.exe
```

### 1.3 Look inside

- `g++ -std=c++20 -E main.cpp -o main.i`, then open `main.i`. How many lines did
  your includes turn into?
- `nm greet.o` and `nm -C greet.o`. Find your function. What is the strange
  name in the first output?
- `nm main.o`. What letter is next to your function, and what does it mean?

### 1.4 Break it on purpose

Undo each case before the next one.

1. Delete the definition from `greet.cpp`. Rebuild.
2. Move the definition into `greet.h` (no `inline`). Add an `other.cpp` that
   includes `greet.h` and defines a small function of its own. Compile it and
   link `main.o greet.o other.o`.
3. Fix case 2 with `inline`. Why does `inline` fix it?
4. Add a struct definition to `greet.h`, remove `#pragma once`, and include the
   header twice in `main.cpp`.

| Case | Compiler or linker? | Key words of the error | Why |
| ---- | ------------------- | ---------------------- | --- |
| 1    |                     |                        |     |
| 2    |                     |                        |     |
| 3    | (no error)          |                        |     |
| 4    |                     |                        |     |

Stuck telling them apart? Explanations §2.2.

### 1.5 A static library

```
ar rcs libgreet.a greet.o
g++ main.o -L. -lgreet -o app.exe
```

Now try `g++ -L. -lgreet main.o -o app.exe`. Why does the order matter? Hint:
Explanations §2.3.

### 1.6 Bring back

The filled-in table, the line count of `main.i`, the mangled name, and your
answer on link order. Claude will ask you to explain each one.

---

## Task 2. Design the headers

Read first: Explanations §3 (the data) and §4 (tools and library features).

### 2.1 Input files (in `data/`)

**`categories.json`**: a JSON array of objects.

| Field         | Type            | Notes                                          |
| ------------- | --------------- | ---------------------------------------------- |
| `category_id` | integer         | required                                       |
| `name`        | string          | required                                       |
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

| Field         | Type            | Notes                                             |
| ------------- | --------------- | ------------------------------------------------- |
| `query_id`    | string          | required                                          |
| `category_id` | integer         | required                                          |
| `region`      | string          | required                                          |
| `max_results` | integer or null | optional; when absent, week 2 uses a default of 5 |

### 2.2 Output file

**`results.json`**: a JSON array, one object per request, in request order.

| Field           | Type              | Notes                          |
| --------------- | ----------------- | ------------------------------ |
| `query_id`      | string            | copied from the request        |
| `matched_count` | integer           | filled in by week 2            |
| `product_ids`   | array of integers | always an array, even if empty |

`data/expected_results.json` shows the exact shape. Whitespace in the output
doesn't matter; the tests parse it as JSON.

### 2.3 Requirements

- Each loader takes the path of the file to read, so tests can point it at a
  temporary file instead of `data/`. The writer takes the path to write to.
- A file that can't be opened, invalid JSON, a missing required field, or a field
  of the wrong type is an error. The error message names the file (and for JSON
  Lines, the line number).
- Loading the sample files gives 9 categories, 15 products and 8 requests, with
  every field intact: every value in memory equals the value in the file,
  including whether `parent_id` and `max_results` were present.
- Writing results and reading the file back as JSON gives the same data.
- The code is in namespace `bazaar`, in a library that tests can link against.
- A caller must be able to get any error message as text.

### 2.4 Edge cases the tests will check

- **Files.** A path that doesn't exist is an error.
- **JSON shape.** Invalid JSON is an error. In the two `.json` files, a top
  level that isn't an array is an error, and so is an element that isn't an
  object. An empty array `[]` loads zero records.
- **Required fields.** Missing is an error. `null` is an error.
- **Types.** A string where an integer belongs is an error. So is a number with
  a fractional part (`3.5`). (Explanations §4.7 has a warning about this.)
- **Optional fields.** `parent_id` and `max_results` may be missing or `null`;
  both mean "not given". A present value must be an integer.
- **Extra fields.** Fields not listed above are ignored.
- **JSON Lines.** A line that is empty or only whitespace (spaces, tabs, `\r`)
  is skipped. Line numbers start at 1 and count every line, skipped ones too.
- **Error messages.** Contain the path exactly as passed in. For JSON Lines
  they also contain `line N`, for example
  `data/requests.jsonl: line 3: missing field "region"`. Other wording is yours.
- **Writer.** A path that can't be opened (such as inside a missing folder) is
  an error. No product ids is written as `"product_ids": []`. No results is
  written as `[]`. An existing file is replaced.

### 2.5 Design questions

Answer each in `docs/DESIGN.md` in a few lines, and let the headers show the
answers. The options and their costs are in Explanations §5.

1. What type represents each record, and why that kind of type (struct or
   class)? Which C++ type does each field get?
2. How does the type say that `parent_id` or `max_results` can be missing?
3. Where does the default of 5 for `max_results` live: in the loader, in the
   type, or in the week 2 engine? What does each choice cost?
4. Free functions or a loader class? What would a class hold?
5. How do callers find out that loading failed: exception, return value, or
   something else? Why?
6. Does your public header include `<nlohmann/json.hpp>`? What happens to every
   file that includes your header if it does?
7. How many headers, and which declarations go in which?

### 2.6 Write it

- Headers go in `include/bazaar/` (Explanations §4.1 and §4.3). Declarations
  only, no function bodies.
- Check they compile (Explanations §4.3 has the command).
- `docs/DESIGN.md` has a week 1 section answering all 7 questions, each with
  decision, reason and cost (format: Explanations §6).
- Commit both together and push, with a message such as
  `design: Add week 1 data model and loader headers`. Then ask Claude for the
  design review.

---

## Reading (learncpp.com)

Lesson numbers can drift; titles are given too.

| Lesson      | Title / topic                                              | Read before         |
| ----------- | ---------------------------------------------------------- | ------------------- |
| 0.5         | Introduction to the compiler, linker, and libraries        | Task 1              |
| 2.7 to 2.12 | Forward declarations, multiple files, headers, header guards (2.7 has the ODR) | Task 1 |
| 7.9         | Inline functions and variables                             | Task 1, case 3      |
| 5.7 to 5.9  | `std::string` and `std::string_view`                       | Task 2              |
| 13.7 to 13.10 | Structs, aggregate initialisation, default member initialisers | design question 1 |
| 12.15       | `std::optional`                                            | design question 2   |
| 27.1 to 27.3 | Exceptions (optional)                                     | design question 5   |

## Done when

- You can explain each failure from the orientation exercise.
- The week 1 tests are green locally and on all three CI jobs.
- `docs/DESIGN.md` has the week 1 answers, and the code review is addressed.
- At least one commit per session, pushed.
