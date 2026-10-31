# bazaar: 12-week C++ interview-prep project

A small e-commerce backend written in C++20, designed and written by you from
an empty repository. Scope is **12 weeks at 8 to 10 hours a week, about 100 to
120 hours total**, including reading.

Goal: be ready for C++ technical interviews. That means being able to (a) design
a small system and defend the design, (b) write correct, idiomatic modern C++,
(c) implement and explain the standard data structures, and (d) talk about
ownership, memory and threads with confidence.

All the data structures and algorithms (graph traversal, hashing, LRU, heaps and
top-K, tokenising, checksums, log replay) are built as parts of bazaar itself,
so each one has a real reason to exist and real code to point at in an
interview.

---

## 1. How we work

### Who does what

| You                                                                 | Claude                                                                |
| ------------------------------------------------------------------- | --------------------------------------------------------------------- |
| The architecture: which modules and classes exist and how they talk | The spec for each week (`docs/WEEKn.md`): requirements, edge cases, reading |
| Every header: classes, signatures, data members                     | Build and run infrastructure: CMake, presets, dependencies, CI, editor config |
| Every implementation, including the command line tool               | Sample data and expected outputs                                      |
| A regression test for every bug you find yourself                   | Unit tests written against *your* headers, and end-to-end tests       |
| The architecture section of the README                              | Design reviews, code reviews, explanations, hints when you ask        |

Claude never edits `include/`, `src/` or `apps/`. When a review finds something
there, Claude describes the problem and you change the code.

The specs point at architecture without dictating it. They ask questions such as
"what owns the product data, and who only reads it?" and leave the answers to
you. Claude may say "I would split this in two" in a review, but the decision
and the code are yours.

### The weekly loop

1. **Spec.** Claude writes `docs/WEEKn.md`: what the feature must do, inputs and
   outputs, edge cases, the design questions to answer, and the two or three
   learncpp lessons that matter. One week at a time, so each spec builds on the
   design you actually have.
2. **Design.** You write the headers for the week: classes, function
   declarations, data members, no function bodies yet. Add a few lines to
   `docs/DESIGN.md` answering the week's design questions. Commit
   (`design: ...`).
3. **Design review.** Claude reads the headers like an interviewer reading a
   whiteboard design: ownership, `const`, what each function returns on failure,
   what is hard to test. You revise until you are happy with it.
4. **Tests.** Claude writes the unit tests against your headers, plus the end-to-end
   test, and wires them into CMake and CI. They all fail at first.
5. **Build.** You implement until the tests are green. When you're stuck, use the
   hint ladder below. If implementing shows the design was wrong, change the
   header and tell Claude; the tests get adjusted. That is normal, not failure.
6. **Code review.** Claude reviews the code the way an interviewer would: missing
   `const`, copies where moves belong, raw pointers that should be smart pointers,
   O(n^2) where O(n log n) was available. You fix what the review finds. The
   week is done.

### The hint ladder

When stuck, ask for one rung at a time, and try again between rungs.

1. **Concept.** Which idea or library feature applies, and where to read about it.
2. **Approach.** The steps in plain words, with no code.
3. **Sketch.** Pseudocode, or a few lines of real C++ on a *different* problem
   that shows the technique. You write the version for bazaar.

There is no rung 4 where Claude writes your function. If even rung 3 isn't
enough, ask Claude to explain the concept with you at a smaller scale first,
then come back.

### Rules

- Design before tests. Claude writes tests only after your headers for the week
  are committed.
- Every week ends with a green test suite. If a week runs over, cut the week's
  stretch item, never the tests.
- Commit at the end of every session. Small commits with clear messages.
- If you can't explain a line you wrote, it isn't done.

Hour budget per week: about 1 on design, 6 to 8 on implementation, 1 on reading.

---

## 2. Toolchain: MinGW-w64 on Windows, sanitizers in CI

The project lives and builds on Windows, with MinGW-w64 g++ from MSYS2 UCRT64
(`C:\msys64\ucrt64\bin` on PATH): g++, gdb, CMake, Ninja, clangd, clang-format
and clang-tidy. GCC on Windows has no AddressSanitizer or ThreadSanitizer, so
both run in CI on GitHub's Linux runners. The MSVC build also runs in CI only,
so compiler differences still show up on every push.

- **No package manager.** GoogleTest, nlohmann-json and Google Benchmark are
  downloaded by CMake `FetchContent` at configure time, pinned to fixed versions.
- **VS Code:** "CMake Tools" and "clangd" extensions. The Microsoft "C/C++"
  IntelliSense stays disabled, because the two conflict.
- **Ignore `C:\MinGW`.** It holds g++ 6.3, which cannot compile C++20. Never
  select it in CMake Tools.

Check: in a new PowerShell, `g++ --version` (13 or newer), `cmake --version`,
`ninja --version`, `gdb --version`, `clangd --version` and `git --version` all
print a version.

Project location: `C:\Users\clutch\Desktop\Everything\Projects\bazaar`. From WSL
that is `/mnt/c/Users/clutch/Desktop/Everything/Projects/bazaar`.

**CI (GitHub Actions).** Claude adds `.github/workflows/ci.yml` in week 1, once
there is code to build. Every push runs these jobs on fresh cloud machines:

| Job            | Runner                                     | From   | What it proves                                             |
| -------------- | ------------------------------------------ | ------ | ---------------------------------------------------------- |
| `windows-msvc` | Windows, MSVC                              | week 1 | the code also builds and passes under Microsoft's compiler |
| `linux-gcc`    | Linux, GCC                                 | week 1 | a plain build and test run on Linux                        |
| `linux-asan`   | Linux, GCC, `-fsanitize=address,undefined` | week 1 | no memory errors or undefined behaviour in the tests       |
| `linux-tsan`   | Linux, clang, `-fsanitize=thread`          | week 9 | no data races in the thread tests                          |

A red job shows its full log on GitHub, including any sanitizer report. Read it
there, fix locally, push again.

---

## 3. Repository layout

What the repository will look like by the end. Only the files marked "now" exist
on day one; everything else appears when a week needs it.

```
bazaar/
  PLAN.md                    this file                                  now
  README.md                                                             now
  docs/WEEKn.md              one spec per week, written just in time    now (week 1)
  docs/DESIGN.md             your answers to each week's design questions
  data/                      sample input and expected output           now
  .clang-format .clangd .vscode/                                        now
  include/bazaar/            your public headers                        YOU
  src/                       your implementations                       YOU
  apps/                      your command line tool(s)                  YOU
  CMakeLists.txt  CMakePresets.json  cmake/                             Claude
  tests/                     GoogleTest, written against your headers   Claude (+ your regression tests)
  bench/                     Google Benchmark, from week 6              Claude
  .github/workflows/ci.yml                                              Claude
```

You decide how the code under `include/`, `src/` and `apps/` is split into
files. The only fixed points are the namespace `bazaar::` and that the logic
lives in a library the tests can link against, not inside `main()`.

---

## 4. The 12 weeks

Each week below says *what* to build and which questions the design must answer.
The `docs/WEEKn.md` spec adds the exact inputs, outputs and edge cases.

### Week 1: data model and loading

**Orientation (about 1.5 hours, before any CMake).** Build a two-file program by
hand with `g++`: compile each `.cpp` to an object file, link them, then turn one
into a static library with `ar` and link against that. Cause a linker error on
purpose (declared but never defined) and an ODR violation (defined twice). After your
week 1 headers pass design review, Claude adds the CMake build around them, and
you walk Claude through what each line of it does in terms of the commands you
typed by hand.

**Build.** Types for categories, products and browse requests, loaders for the
three data files using nlohmann-json, and a writer for the results file.

**Design questions.** Which fields can be missing, and how does the type say so?
Free functions or a class? What happens when a file can't be opened? Should your
public header mention nlohmann-json at all?

**C++.** Compilation model, headers vs `.cpp`, `#pragma once`, the One
Definition Rule, static libraries, `std::string_view`, `std::optional`, structs
with default member initialisers.

**learncpp.** 2.1 to 2.11, 5.7 to 5.9, 13.7 to 13.10, 12.15.

**Done.** The loaders read the sample files and the tests check the results;
green locally and on all three CI jobs.

### Week 2: category browse engine

**Build.** For each request: collect a category and all its descendants, keep
the products there whose region matches exactly, sort by popularity descending
then id ascending, report the count before truncation and the first N ids. A
command line tool turns the three input files into `results.json`. The data is
not trusted: cycles must not hang, deep trees must not overflow the stack.

**Design questions.** What owns the data, and what only reads it? When is the
parent-to-children index built, and how often? Which member functions are
`const`? What does one browse result look like as a type? How thin can `main()`
be?

**C++.** Classes and invariants, constructors, `explicit`, `const` correctness,
rule of zero, `std::unordered_map` vs `std::map`, `std::sort` with a lambda,
strict weak ordering, references vs copies in range-for.

**learncpp.** 12.1 to 12.6, 14, 15.1 to 15.4, 18.2 to 18.3, 20.6 to 20.7.

**DSA.** Iterative DFS over an adjacency list, visited sets, custom comparators.

**Done.** The end-to-end test runs your CLI on the sample data and the output
matches `data/expected_results.json`.

### Week 3: hash map, part 1

**Read first (the one exception to build-first).** 22.1 to 22.4 (move
semantics), 13.13 to 13.14 (class templates). About 2 hours.

**Build.** `HashMap<K, V, Hash>` with open addressing and linear probing:
`insert`, `find`, `erase`, `operator[]`, `size`, tombstones, a maximum load
factor and rehashing.

**Design questions.** What does `find` return when the key is missing? What does
`insert` return when the key is already there? How is one slot represented
(empty, full, deleted)? Do tombstones count toward the load factor? Why might the
capacity be a power of two?

**C++.** Class templates, `std::hash`, `std::pair`, templates in headers and why.

**learncpp.** 26.1 to 26.2, 21.1 to 21.4, 21.12.

**DSA.** Hashing, collision strategies, amortised analysis.

**Done.** Tests green locally and in `linux-asan`.

### Week 4: hash map, part 2

**Build.** The slot array is now managed by hand (`new[]`/`delete[]`, no
`std::vector`), so copy and move are yours to write. A forward iterator and
`const` iterator that work with range-for. Support for move-only values such as
`std::unique_ptr<int>`. A `std::hash` specialisation for one of your own key
types and a C++20 concept on `K`. Then replace an `std::unordered_map` in the
week 2 code with your map. Stretch: `emplace` with perfect forwarding.

**Design questions.** Which of the five special members do you need, and which
can be `= delete` or `= default`? What does the iterator hold? When are
iterators invalidated, and does your interface say so?

**C++.** Rule of three/five/zero, move constructor and move assignment,
`noexcept` and why `std::vector` cares, iterator categories, concepts.

**learncpp.** 19.1 to 19.3, 22.4 again with your own code in mind.

**Done.** Tests green in `linux-asan`, including the move-only value test;
week 2 tests still green on top of your map.

### Week 5: LRU cache

**Build.** `LruCache<K, V>` with a capacity, O(1) `get` and `put`, eviction of
the least recently used entry. Use it to cache browse results, keyed by the
request.

**Design questions.** Linked list plus map of iterators, or your own nodes? Who
owns a node? What does `get` return, and is that still valid after the next
`put`? If `browse()` is `const` but now writes to a cache, what changes
(`mutable`, and what it means later for threads)?

**C++.** `std::list`, `splice`, iterator invalidation, `std::unique_ptr` for
ownership vs raw non-owning pointers, `mutable`, hashing a struct key.

**learncpp.** 22.5 to 22.7.

**DSA.** LRU, doubly linked lists.

**Done.** Tests green in `linux-asan`; the browse tests still pass with the
cache on.

### Week 6: heap, top-K and benchmarks

**Build.** `MinHeap<T, Compare>` with `push`, `pop`, `top`, and building a heap
from a range in O(n). Rewrite the browse ranking as top-K with the heap: O(n log
k) instead of O(n log n). Claude adds Google Benchmark; you predict each result
before running it.

**Design questions.** What does `Compare` default to? Why does top-K use a
*min*-heap? What does `pop` return, and why does `std::priority_queue::pop`
return `void`?

**C++.** Templates with a comparator parameter, function objects,
`std::priority_queue` internals, measuring rather than guessing.

**learncpp.** 17.1 to 17.3, 18.4.

**DSA.** Binary heaps, sift up and down, heapify, top-K.

**Done.** Tests green; benchmarks of heap top-K vs full sort and cache hit vs
recompute run locally, and you can explain each number in two sentences.

### Week 7: two kinds of polymorphism

**Build.** Pluggable ranking: an abstract `Ranker` with popularity, recency and
name-match rankers (with a small tokeniser), and a composite that weights
others. The browse engine takes a ranker and never changes when a new one is
added. Filters (region, price range, category subtree) as a `std::variant`
evaluated with `std::visit`.

**Design questions.** Who owns the ranker? Why must the base have a virtual
destructor? Why are filters a variant and rankers a class hierarchy, and when
would you swap the choice?

**C++.** Inheritance, virtual functions, `override` and `final`, slicing, pure
virtual interfaces, `unique_ptr` to a base, Strategy and Composite patterns,
`std::variant`, `std::visit`, `std::function`.

**learncpp.** 23.1 to 23.4, 24.1 to 24.5, 25.1 to 25.6, 20.1.

**DSA.** Tokenising, simple scoring.

**Done.** Adding a ranker needs no change to the browse engine; tests green.

### Week 8: value types, errors and checkpoint

**Build, first part (about 6 hours).** A `Money` type (integer minor units,
never `double`) with arithmetic and comparison. Strong id types so a product id
can't be passed as an order id. A result type (`std::expected` or your own) for
operations that can fail. A minimal `Order` with an `enum class` status, and an
`Inventory` whose `reserve` rejects overselling.

**Checkpoint, second part (about 3 hours).** `clang-tidy` clean on the
modernize and performance checks. One refactor you choose from the review notes
of weeks 2 to 7. First pass through the section 5 checklist out loud, with the
code open.

**Design questions.** Which operations make sense on `Money` (money times money
doesn't)? Exceptions or return values for "out of stock", and why?

**C++.** Operator overloading, `enum class`, strong typedefs, `[[nodiscard]]`,
`constexpr`, exceptions vs return values.

**learncpp.** 21.5 to 21.11, 13.2 to 13.6, 27.1 to 27.5, F.1.

**Done.** Every `Money` operator is tested; oversell is rejected; clang-tidy is
clean; checklist items 1 to 9 answered.

### Week 9: threads, part 1

**Build.** A bounded `BlockingQueue<T>` (producers wait when full, consumers
when empty), and a `ThreadPool` on top of it whose `submit` returns a
`std::future`. A clear shutdown: what happens to queued tasks when the pool is
destroyed? Claude adds the `linux-tsan` CI job this week.

**Design questions.** What does the pool own? How does a worker know to stop?
What happens if a task throws?

**C++.** `std::jthread`, `std::mutex`, `lock_guard` vs `unique_lock` vs
`scoped_lock`, `std::condition_variable` and why `wait` takes a predicate,
`std::packaged_task`, `std::future`.

**Reading.** learncpp does not cover threads. Use cppreference for each type,
and "C++ Concurrency in Action" (Williams) chapters 2 to 4 if you want a book.

**Done.** Tests green under TSan in CI.

### Week 10: threads, part 2

**Build.** A thread-safe LRU cache by wrapping week 5's with one mutex, then a
sharded version. Hit and miss counters with `std::atomic`. A 16-thread stress
test on the cache and the pool. Benchmark single mutex vs sharded.

**Design questions.** Why can't an LRU `get` take a shared (reader) lock? How do
you choose the shard? What lock ordering prevents deadlock if an operation
touches two shards?

**C++.** `std::atomic` and memory order basics, `std::shared_mutex`, data race vs
race condition, deadlock and lock ordering.

**Done.** The stress test runs clean under TSan in CI, and the sharded cache
measurably beats the single-mutex one.

### Week 11: persistence, part 1

**Build.** A binary snapshot of the catalog: a header with a magic number and a
version, records encoded field by field in little-endian, and a CRC32 checksum
you implement. Loading rejects a bad magic, version or checksum. Round-trip
test: save, load, compare.

**Design questions.** Why never `memcpy` a struct to disk? How are strings
stored? What does loading return when the file is corrupt?

**C++.** `std::fstream` in binary mode, `std::span`, `std::byte`, endianness,
alignment and padding, RAII file handles, `std::filesystem`.

**learncpp.** 28.1 to 28.7, O.1 to O.3.

**DSA.** Checksums, table-driven CRC.

**Done.** Round-trip and every corruption case tested; green in `linux-asan`.

### Week 12: persistence, part 2, and wrap-up

**Build.** A write-ahead log for orders: append the record and flush before
applying it in memory; replay on startup; a snapshot truncates the log. Claude
writes the crash test harness: a child process is killed mid-write, the engine
restarts, and the state is checked against what was acknowledged.

**Wrap-up.** You write the README architecture overview and a table of the
benchmark numbers. Second pass through the section 5 checklist, now with the
thread and persistence items.

**Design questions.** What does "acknowledged" mean exactly? How do you detect a
half-written last record? Why must replay be idempotent?

**C++.** Flushing and what it does and doesn't guarantee, error handling at
system boundaries, process spawning.

**DSA.** Log-structured storage, idempotent replay.

**Done.** The crash test passes 100 times in a loop in `linux-asan`; README and
checklist done; all four CI jobs green.

---

## 5. Interview readiness checklist

By the end you should be able to answer each of these with code from this repo
as evidence:

1. What is RAII and where do you rely on it? (locks, `unique_ptr`, file handles)
2. Rule of zero / three / five. Which of your classes follow which, and why.
3. When does a copy happen that you didn't intend? (range-for by value,
   `push_back` vs `emplace_back`, returning by value vs `const&`)
4. `unique_ptr` vs `shared_ptr` vs raw pointer: which did you use where, and why.
5. Strict weak ordering: what breaks if your comparator isn't one.
6. Hash map internals: load factor, probing, why `erase` needs tombstones.
7. How `std::vector` grows, and what `noexcept` on your move constructor changes.
8. Virtual functions: vtable, `final`, why destructors must be virtual, slicing.
9. Templates vs virtual dispatch vs `std::variant`: trade-offs.
10. Data race vs race condition, with an example from your thread pool.
11. Why `condition_variable::wait` needs a predicate.
12. How you'd shard a cache and what you'd measure.
13. What a write-ahead log guarantees, and what happens on a crash before and
    after the flush.
14. Walk through a design decision you changed after review, and why.
15. Walk through fixing a bug: reproduce with a test, narrow down, fix, add a
    regression test. Use a real bug from this repo as the example.

---

## 6. Optional extensions (only if you finish early or want more later)

Each is 1 to 2 weeks. None is needed for interviews at your target level.

- **Hand-written JSON parser** replacing nlohmann-json: recursive descent,
  `std::variant` value type, `std::from_chars`.
- **Trie** for product-name autocomplete with top-N completions.
- **Inverted index** with TF-IDF or BM25 ranking as another `Ranker`.
- **Full order lifecycle**: `Order` state machine, `Inventory` release path, and
  a discrete-event simulator with a priority queue of timestamped events.
- **Lock-free single-producer single-consumer ring buffer** with atomics and
  acquire/release ordering, verified under TSan.
- **Log compaction and snapshot scheduling** on top of the week 12 WAL.
- **REPL and TCP server** with the Command pattern and Asio.

---

## 7. First steps

1. Read section 1 again. The roles matter more than anything else in this file.
2. Open `docs/WEEK1.md` and do the orientation exercise.
3. Design the week 1 headers, commit them, and ask for a design review.
4. Claude adds the CMake build, CI and the week 1 tests; you explain the CMake
   back, then implement until green.
