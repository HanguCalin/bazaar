# Week 2: category browse engine

## Goal

A small, fully specified feature done end to end, the way an online coding
test would ask for it.

## Spec

Categories form a forest: each has a `category_id`, a `name` and a `parent_id`
(or none, for a root). Products have `product_id`, `name`, `category_id`,
`region` and `popularity_score`. A request has `query_id`, `category_id`,
`region` and an optional `max_results` (default 5).

For each request:

1. Collect the category and all its descendants. An unknown category gives 0
   matches.
2. Keep products in those categories whose `region` equals the request's
   region exactly (case-sensitive; `EU-West` is not `EU`).
3. Sort by `popularity_score` descending, then `product_id` ascending.
4. `matched_count` is the number of products after step 2, before truncation.
5. Return the first `max_results` product ids.

Output is a JSON array with one result per request, in input order. The data
is not trusted: category cycles must not hang the program, and a very deep
tree must not overflow the stack.

## Already in place (scaffold)

| Path | What it is |
|---|---|
| `include/bazaar/catalog.hpp` | `Catalog`: owns the data, answers `subtree()` |
| `include/bazaar/browse_engine.hpp` | `BrowseEngine`: `browse()` and `browse_all()` |
| `apps/browse_cli/main.cpp` | the command line tool, already wired up |
| `data/*.json`, `data/requests.jsonl` | sample input |
| `data/expected_results.json` | the correct output for the sample input |
| `tests/test_catalog.cpp` | subtree, unknown category, isolation, cycles, depth |
| `tests/test_browse_engine.cpp` | region, ordering with ties, default, truncation, order |
| `tests/test_end_to_end.cpp` | the sample files through the whole pipeline |
| `drills/week02/` | number of islands, kth largest |

## Your tasks

1. **`src/catalog.cpp`.** Store the data and build a children index once, in
   the constructor. `subtree()` is an iterative DFS (or BFS) with a visited
   set. Questions to answer in review: why `std::unordered_map` and not
   `std::map` here? What is the cost of `subtree()`?
2. **`src/browse_engine.cpp`.** Filter, sort with a lambda comparator,
   truncate. Know what a strict weak ordering is, and what goes wrong with
   `std::sort` if your comparator is not one.
3. **Run the CLI** on the sample data and compare with the expected file:
   ```
   build\debug\apps\browse_cli\browse_cli.exe data\categories.json data\products.json data\requests.jsonl results.json
   ```
4. **Widen CI**: in `.github/workflows/ci.yml`, change `TEST_LABELS` from
   `week1` to `week[12]`.
5. **Drills**, timed, no IDE help on the first attempt.

`ctest --preset debug -L week2` runs this week's tests.

## Broken-repo drill 1 (end of week)

When week 2 is green, ask Claude for the broken branch: 3 planted bugs, failing
tests, 45 minutes on a timer. Reproduce, narrow down, fix, and keep a note of
how you found each one.

## Reading (learncpp.com)

- 12.1 to 12.6: references
- 14: introduction to classes
- 15.1 to 15.4: more on classes
- 18.2 to 18.3: iterators and standard algorithms
- 20.6 to 20.7: lambdas

## Done when

- `ctest --preset debug -L "week[12]"` is green locally and in all three CI
  jobs.
- The CLI's `results.json` matches `data/expected_results.json`.
- The broken branch is fixed inside the timer.
