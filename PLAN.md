# bazaar: 8-week C++ interview-prep project

A small e-commerce backend written in C++20. Scope is fixed at **8 weeks at 8 to
10 hours a week, about 70 to 80 hours total**, including reading.
Everything that interviewers ask about is in the core. Everything else is in the
optional section at the end and can be ignored.

Goal: be ready for C++ technical interviews. That means being able to (a) write
correct, idiomatic modern C++, (b) implement and explain the standard data
structures, (c) read, debug and fix code in an existing codebase, and (d) talk
about ownership, memory and threads with confidence.

There are no separate interview problems. All the data structures and
algorithms (graph traversal, hashing, LRU, heaps and top-K, tokenising,
checksums, log replay) are built as parts of bazaar itself, so each one has a
real reason to exist and real code to point at in an interview.

---

## 1. How we work

Build first, study when stuck. Don't read learncpp cover to cover.

The loop for every week:

1. **Scaffold.** Claude creates the skeleton: empty classes, function signatures,
   failing tests, and a `WEEK.md` with the goal and the two or three learncpp
   lessons that matter.
2. **Attempt.** You make the tests pass with what you already know. Getting stuck
   is expected and is the point.
3. **Study in doses.** When stuck, read the specific lesson linked in `WEEK.md`,
   or ask Claude to explain the concept against your own code.
4. **Review.** Claude reviews your code the way an interviewer would: missing
   `const`, copies where moves belong, raw pointers that should be smart pointers,
   O(n^2) where O(n log n) was available. You fix what the review finds.

Rules:

- You write the module code. Claude writes scaffolding, tests and reviews. If
  Claude writes the hash map, you learn nothing.
- Every week ends with a green test suite. If a week runs over, cut the week's
  stretch item, never the tests.
- Commit at the end of every session. Small commits with clear messages.

Hour budget per week: 7 to 9 on the module, 1 on reading.

---

## 2. Toolchain: MinGW-w64 on Windows, sanitizers in CI

The project lives and builds on Windows, with MinGW-w64 g++ from MSYS2. GCC
on Windows has no AddressSanitizer or ThreadSanitizer, and no Windows compiler
has ThreadSanitizer. Both therefore run in CI on GitHub's Linux runners (see
below). The MSVC build also runs in CI only, so compiler differences still show
up on every push.

Install once (about 1 hour, counts toward week 1):

1. **MSYS2** is already installed at `C:\msys64`, with `C:\msys64\ucrt64\bin` on
   PATH. In an "MSYS2 UCRT64" terminal, update it and add the build tools:
   ```
   pacman -Syu
   pacman -S mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-cmake \
             mingw-w64-ucrt-x86_64-ninja mingw-w64-ucrt-x86_64-clang-tools-extra
   ```

   `-Syu` may ask you to close the terminal and run it again; that is normal.
   This gives you g++, gdb, CMake, Ninja, clangd, clang-format and clang-tidy.
2. **Git for Windows** (skip if `git --version` already works in PowerShell).
3. **VS Code extensions:** "CMake Tools" and "clangd". Do not also enable the
   Microsoft "C/C++" IntelliSense, because the two conflict. Online coding tests
   often run inside a VS Code setup with clangd and CMake Tools, so matching it
   is useful.
4. **No package manager.** GoogleTest, nlohmann-json and Google Benchmark are
   downloaded by CMake `FetchContent` at configure time, pinned to fixed
   versions. This works the same locally and on every CI runner.
5. **Ignore `C:\MinGW`.** It holds g++ 6.3, which cannot compile C++20. It is not
   on PATH; never select it in CMake Tools, or uninstall it.

Check: in a new PowerShell, `g++ --version` (13 or newer), `cmake --version`,
`ninja --version`, `gdb --version`, `clangd --version` and `git --version` all
print a version.

Optional, only if waiting for CI to see ASan results gets annoying (around week
3): install **Visual Studio 2022 Build Tools** with "Desktop development with
C++" to get local AddressSanitizer through MSVC (`/fsanitize=address`). This
adds one preset and changes no project code.

Project location: `C:\Users\clutch\Desktop\Everything\Projects\bazaar` (no spaces
in the path, not inside OneDrive). From WSL that is
`/mnt/c/Users/clutch/Desktop/Everything/Projects/bazaar`.

Working in VS Code: open the folder, pick the `debug` configure preset when
CMake Tools asks, build with `Ctrl+Shift+B`, run tests from the Testing panel or
the "Run CTest" button, debug with F5 (gdb).

**CI (GitHub Actions).** Every push to the GitHub repository runs
`.github/workflows/ci.yml` on fresh cloud machines. The repository is public
from day one, because Actions minutes are unlimited for public repositories.
Jobs:

| Job              | Runner                                       | From   | What it proves                                             |
| ---------------- | -------------------------------------------- | ------ | ---------------------------------------------------------- |
| `windows-msvc` | Windows, MSVC                                | week 1 | the code also builds and passes under Microsoft's compiler |
| `linux-gcc`    | Linux, GCC                                   | week 1 | a plain build and test run on Linux                        |
| `linux-asan`   | Linux, GCC, `-fsanitize=address,undefined` | week 1 | no memory errors or undefined behaviour in the tests       |
| `linux-tsan`   | Linux, clang, `-fsanitize=thread`          | week 7 | no data races in the stress test                           |

A red job shows its full log on GitHub, including any sanitizer report. Read it
there, fix locally, push again.

---

## 3. Repository layout

```
bazaar/
  PLAN.md                    this file
  CMakeLists.txt
  CMakePresets.json          debug, release (MinGW locally, any compiler in CI),
                             asan and tsan (Linux only, used by CI)
  cmake/Dependencies.cmake   FetchContent: gtest, nlohmann-json, benchmark
  .clang-format
  .github/workflows/ci.yml   build + ctest on Windows (MSVC), Linux (GCC) and
                             Linux ASan, plus a Linux clang TSan job from week 7
  include/bazaar/            public headers, one per module
  src/                       implementations, mirrors include/
  apps/browse_cli/main.cpp   the command line tool
  tests/                     GoogleTest, one file per module
  bench/                     Google Benchmark, from week 4
  data/                      sample json / jsonl files
  docs/WEEK1.md .. WEEK8.md  goal, deliverables, reading, done-criteria
```

Library target `bazaar` (static, namespace `bazaar::`). The app and the tests
link against it. The split is itself a lesson: headers vs translation units,
what a library is, why the app contains almost no logic.

---

## 4. The 8 weeks

### Week 1: skeleton and data loading

**Build.** CMake project with the `bazaar` library, one executable, one passing
GoogleTest test, presets, clang-format, and loaders for the three data files
using nlohmann-json (we use a library here so the week stays short; writing a
JSON parser is in the optional section).

**CI (about 2 hours, inside this week's budget).** Create a public GitHub
repository `bazaar` and push to it. `.github/workflows/ci.yml` configures,
builds and runs `ctest` on every push, in three jobs: `windows-msvc`,
`linux-gcc` and `linux-asan` (section 2). A green badge in the README from day
one. CI is the only place MSVC and the sanitizers run, so it is the evidence for
every "green under asan" criterion below, and the base for the TSan job in
week 7.

**C++.** Compilation model, headers vs `.cpp`, `#pragma once`, the One Definition
Rule, static libraries, `std::string_view`, `std::optional` for "field may be
missing", structs with default member initialisers, what a compiler difference
looks like when MSVC in CI rejects code your local GCC accepted.

**learncpp.** 2.1 to 2.11 (functions and files), 5.7 to 5.9 (`std::string`,
`std::string_view`), 13.7 to 13.10 (structs), 12.15 (`std::optional`).

**Done.** `cmake --preset debug`, build, `ctest` green locally with MinGW and on
all three CI jobs; the loaders read the sample files and a test checks the
counts.

### Week 2: category browse engine

A small, fully specified feature done end to end. The spec is modelled on a
sample online-test problem. It is a warm-up, not the point of the project.

**Spec.** Categories form a forest (`category_id`, `name`, `parent_id` or null).
Products have `product_id`, `name`, `category_id`, `region`,
`popularity_score`. A request has `query_id`, `category_id`, `region`, optional
`max_results` (default 5). For each request: collect the category and all its
descendants (unknown category gives 0 matches); keep products whose `region`
matches exactly; sort by `popularity_score` descending then `product_id`
ascending; report `matched_count` (before truncation) and the first
`max_results` products. Output is a JSON array in input order.

**Build.** `Catalog` (owns categories and products, builds the children map
once), `BrowseEngine` with a `const` `browse()` method, `browse_cli` that reads
the inputs and writes `results.json`. Tests for subtree collection, unknown
category, region exactness, ordering with ties, default and truncation,
`matched_count` before truncation, cross-tree isolation, cycle safety.

**C++.** Classes and invariants, constructors, `explicit`, `const` correctness,
rule of zero, `std::unordered_map` vs `std::map`, `std::sort` with a lambda
comparator and what a strict weak ordering is, references vs copies in
range-for.

**learncpp.** 12.1 to 12.6 (references), 14 (intro to classes), 15.1 to 15.4
(more on classes), 18.2 to 18.3 (iterators and algorithms), 20.6 to 20.7
(lambdas).

**DSA.** Iterative DFS over an adjacency list, visited sets, cycle detection,
custom comparators.

**Done.** All tests green, CLI matches the expected output for the sample data.

### Week 3: hash map

**Read first (the one exception to build-first).** 22.1 to 22.4 (move
semantics), 13.13 to 13.14 (class templates and CTAD). About 2 hours.

**Build.** `HashMap<K, V, Hash>`: open addressing with linear probing,
tombstones, load factor and rehash, `find`, `insert`, `erase`, `operator[]`,
a forward iterator that works with range-for. Then swap the `Catalog`'s
`unordered_map` for it to prove it works. Stretch: `emplace` with perfect
forwarding.

**C++.** Class templates, move constructor and move assignment, `noexcept` and
why `std::vector` cares about it, the rule of five when you really manage memory,
iterator basics, `std::hash` specialisation, a first C++20 concept on `K`.

**learncpp.** 26.1 to 26.2 (template classes, non-type parameters), 19.1 to
19.3 (`new`/`delete`, destructors), 21.1 to 21.4 and 21.12 (operator
overloading, including `operator[]`).

**DSA.** Hashing, collision strategies, amortised analysis.

**Done.** Tests green locally and in the `linux-asan` CI job, including a test
that stores a move-only value type such as `std::unique_ptr<int>`.

### Week 4: LRU cache and heap

**Build.** `LruCache<K, V>` (doubly linked list plus map, O(1) `get` and `put`),
used to cache browse results. `MinHeap<T, Compare>` with `push`, `pop`, `top`,
then rewrite the browse ranking as top-K with the heap: O(n log k) instead of
O(n log n). Google Benchmark comparing the heap top-K against full sort and the
cache hit path against recompute.

**C++.** Templates with a comparator parameter, `std::list` and iterator
invalidation, `std::unique_ptr` for node ownership versus raw back-pointers,
when raw pointers are fine (non-owning), `std::priority_queue` internals.

**learncpp.** 22.5 to 22.7 (smart pointers), 17.1 to 17.3 (`std::array`) for
heap storage, 18.4 (timing your code).

**DSA.** LRU, binary heaps, top-K.

**Done.** Both containers green in the `linux-asan` CI job, the benchmark runs
locally and you can explain the numbers in two sentences each.

### Week 5: inheritance and polymorphism

**Build.** Pluggable ranking. `Ranker` abstract base with pure virtual
`score()`, `PopularityRanker`, `RecencyRanker`, `CompositeRanker` that weights
others. A simple tokeniser and a `NameMatchRanker` that scores by matched
words. `BrowseEngine` takes a `std::unique_ptr<Ranker>` and never changes when a
new ranker is added. Filters as `std::variant<RegionFilter, PriceRange, CategorySubtree>` evaluated with `std::visit`.

**C++.** Inheritance, virtual functions, `override` and `final`, virtual
destructors, pure virtual interfaces, `unique_ptr` to a polymorphic base, the
Strategy pattern, composition over inheritance, `std::variant` and `std::visit`,
`std::function`.

**learncpp.** 23.1 to 23.4 (object relationships), 24.1 to 24.5 (inheritance),
25.1 to 25.6 (virtual functions), 20.1 (function pointers).

**DSA.** String tokenising, simple scoring.

**Done.** Swapping rankers requires no change to `BrowseEngine`; tests green.

### Week 6: value types (half week) and consolidation (half week)

**Build, first half (about 4 hours).** `Money` value type (integer minor units,
never `double`) with arithmetic and comparison operators. Strong id types so
`ProductId` cannot be mixed with `OrderId`. A `Result<T, E>` (or
`std::expected`) return type for failures. A minimal `Order` record with an
`enum class` status and one `Inventory::reserve()` that rejects oversell,
covered by a single test case each. No state machine, no release path. These
exist so week 8 has something to persist, not as business logic to polish.

**Consolidation, second half (about 3 hours).** `clang-tidy` clean on the
modernize and performance checks. One small refactor you choose from the review
notes of weeks 2 to 5. Draft the README: architecture overview, how to build,
CI badge. First pass through the section 5 checklist out loud, with the code
open; write a one-paragraph answer for each item you stumble on.

**C++.** Operator overloading, `enum class`, strong typedefs, error handling by
return value versus exceptions, `[[nodiscard]]`, `constexpr` for `Money`
arithmetic.

**learncpp.** 21.5 to 21.11 (remaining operator overloading), 13.2 to 13.6
(enums), 27.1 to 27.5 (exceptions), F.1 (constexpr functions).

**Done.** `Money` and the id types have tests for every operator; oversell is
rejected in a test; clang-tidy is clean; README draft committed; checklist
items 1 to 9 answered.

### Week 7: threads

**Build.** `ThreadPool` with a bounded task queue. `BlockingQueue<T>` (bounded
producer/consumer). Thread-safe `LruCache` by wrapping the week 4 version with a
mutex, then a sharded version, then measure both with the benchmark. A 16-thread
stress test that hammers the cache and the pool.

**ThreadSanitizer in CI (about 3 hours).** No Windows compiler has
ThreadSanitizer, so add the fourth CI job, `linux-tsan`: Linux runner, clang,
`-fsanitize=thread`, running the stress test.
AddressSanitizer is not a race detector; a clean asan run on racy code is the
normal outcome, so asan alone is no evidence of thread safety. TSan is what
turns "I wrote a thread pool" into a verified claim.

**C++.** `std::jthread`, `std::mutex`, `lock_guard` vs `unique_lock` vs
`scoped_lock`, `std::condition_variable` with a predicate and why, `std::atomic`
for counters, `std::shared_mutex`, data race vs race condition, deadlock
avoidance by lock ordering.

**Reading.** learncpp does not cover threads. Use cppreference pages for each
type, and "C++ Concurrency in Action" (Williams) chapters 2 to 4 if you want a
book.

**Done.** The 16-thread stress test runs clean under TSan in CI, and the
sharded `LruCache` measurably beats the single-mutex version in the benchmark.

### Week 8: persistence

**Build.** The catalog and orders survive a restart.

- Binary snapshot of the catalog with a versioned header and a CRC32 checksum;
  reject a snapshot with a bad version or checksum.
- Write-ahead log (WAL) for orders: append the record and flush before applying
  it in memory; replay the log on startup; a snapshot truncates the log.
- Crash test: a child process is killed mid-write, the engine restarts, state is
  verified against what was acknowledged. Run it in a loop locally, and in a
  loop in the `linux-asan` CI job.

**C++.** `std::fstream` in binary mode, `std::span` and `std::byte`, endianness
and why you never `memcpy` a struct to disk, alignment, RAII file handles,
`std::filesystem`, error handling at system boundaries, process spawning for the
crash test.

**learncpp.** 28.1 to 28.7 (input and output), O.1 to O.3 (bit manipulation).

**DSA.** Checksums, log-structured storage, idempotent replay.

**Wrap-up.** Finish the README: architecture overview, and a table of Google
Benchmark numbers for heap top-K vs full sort, cache hit vs recompute, and
sharded vs single-mutex cache. Second pass through the section 5 checklist,
now including the thread and persistence items.

**Done.** Crash test passes 100 times in a loop in the `linux-asan` CI job; README has the
architecture overview and the benchmark table; checklist fully answered; all four CI
jobs green on the public GitHub repo.

---

## 5. Interview readiness checklist

By the end you should be able to answer each of these with code from this repo
as evidence:

- What is RAII and where do you rely on it? (locks, `unique_ptr`, file handles)
- Rule of zero / three / five. Which of your classes follow which, and why.
- When does a copy happen that you didn't intend? (range-for by value,
  `push_back` vs `emplace_back`, returning by value vs `const&`)
- `unique_ptr` vs `shared_ptr` vs raw pointer: which did you use where, and why.
- Strict weak ordering: what breaks if your comparator isn't one.
- Hash map internals: load factor, probing, why `erase` needs tombstones.
- How `std::vector` grows, and what `noexcept` on your move constructor changes.
- Virtual functions: vtable, `final`, why destructors must be virtual, slicing.
- Templates vs virtual dispatch: static vs dynamic polymorphism, trade-offs.
- Data race vs race condition, with an example from your thread pool.
- Why `condition_variable::wait` needs a predicate.
- How you'd shard a cache and what you'd measure.
- What a write-ahead log guarantees, and what happens on a crash before and
  after the flush.
- Walk through fixing a bug: reproduce with a test, narrow down, fix, add a
  regression test. Use a real bug from this repo (a review finding, a red CI
  job or a sanitizer report) as the example.

---

## 6. Optional extensions (only if you finish early or want more later)

Each is 1 to 2 weeks. None is needed for interviews at your target level.

- **Hand-written JSON parser** replacing nlohmann-json: recursive descent,
  `std::variant` value type, `std::from_chars`. Teaches parsing and variant.
- **Trie** for product-name autocomplete with top-N completions. LeetCode 208,
  211, 642.
- **Inverted index** with TF-IDF or BM25 ranking as another `Ranker`.
- **Full order lifecycle**: `Order` state machine, `Inventory` release path,
  and a discrete-event simulator with a priority queue of timestamped events,
  Observer pattern, `std::chrono`.
- **Lock-free single-producer single-consumer ring buffer** with atomics and
  acquire/release ordering, verified under the existing TSan job.
- **Log compaction and snapshot scheduling** on top of the week 8 WAL.
- **REPL and TCP server** with the Command pattern and Asio.
- **HTTP layer and a one-page web front** for demos only.

---

## 7. First steps

1. Install the toolchain from section 2 and run its version checks.
2. Create an empty public GitHub repository named `bazaar` (no README, no
   licence, so the first push is clean).
3. Tell Claude to scaffold week 1 and week 2 into
   `C:\Users\clutch\Desktop\Everything\Projects\bazaar`.
4. Open the folder in VS Code, pick the `debug` preset, build, run CTest. Expect
   28 tests: 1 passing and 27 failing. The failures are your week 1 and week 2
   targets (`ctest -L week1`, `ctest -L week2`).
5. Commit and push. CI runs only the `week1` label (`TEST_LABELS` in
   `ci.yml`) and fails on it until the week 1 loaders are done.
   Each week you widen the label to include that week.
