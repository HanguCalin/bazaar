# bazaar design notes

## Week 1: data model and loading

### 1. Record types

**Decision.** All four records (`Category`, `Product`, `Request`, `Result`) are
classes with private fields, one `explicit` constructor that takes every field,
`const` getters and no setters.

**Why.** A record can't change after it's built. So the parent-to-children
index that week 2 builds from `parent_id` always stays correct, and threads in
weeks 9 and 10 can share records with no locks.

**Cost.** More code than a struct: a constructor and a getter for every field.
To change a record you build a new one. Week 2's engine collects product ids in
a local `std::vector<int>` and builds the `Result` at the end. Because the
constructor is `explicit`, brace lists without the type name don't compile:
`Product p = {...};`, `return {...};` and `products.push_back({...});` must
become `Product p{...};`, `return Product{...};` and
`products.push_back(Product{...});` (or `emplace_back(...)`).

No record has a rule tying its own fields together. The spec doesn't say
`parent_id != id`, `max_results > 0`, `popularity_score >= 0` or a non-empty
`region`. "Every product belongs to an existing category" is a real rule, but a
single `Product` can't check it, because it can't see the categories. That rule
belongs to whatever type owns both lists in week 2.

Fields are private but not `const`. A `const` member would delete copy
assignment, and then sorting or reassigning a `std::vector<Product>` would stop
compiling. Whole-object assignment is fine. Changing one field at a time is
what's ruled out.

**Field types.**

| Field | Type | Why |
| --- | --- | --- |
| ids, `popularity_score` | `int` | The data is small. Cost: nlohmann's `get<int>()` doesn't range-check, so a value above about 2.1 billion would be silently corrupted. |
| text (`name`, `region`, `query_id`) | `std::string` | The record outlives the JSON it was parsed from, so it must own its characters. A `std::string_view` would dangle. |
| `Result::product_ids` | `std::vector<int>` | Always present and possibly empty. Writes as `[]` when empty. |

**Naming.** A record's own id drops the prefix (`category.id`, `product.id`).
References to other records keep it (`product.category_id`,
`category.parent_id`). One exception: the request keeps `query_id`, because the
JSON file and `Result` use that name too, and one value should have one name
(`result.query_id` comes from `request.query_id`).

**Getter return types.** Every getter returns by value
(`std::string name() const`). Why: it's the simplest to read and can never
dangle, because the caller always gets its own copy. bazaar is a learning
project, not a race for speed. Cost: every call to a text or vector getter makes
a copy. That means a heap allocation for strings longer than about 15
characters, and for any non-empty vector. `p.region() == req.region()` copies
two strings, and `for (int id : result.product_ids())` loops over a copy of the
vector. That's fine for this data size. The week 6 benchmarks can show whether
it ever matters, and switching to `const std::string&` later only changes the
header, not the callers.

**Constructor parameters.** String and vector parameters are taken by value and
moved into the members with `std::move`. The loader already has a fresh string,
so it moves it in and nothing is copied. It's also a chance to learn
`std::move` before week 3 relies on it. Cost: you have to understand moves and
moved-from objects. A caller that keeps its own copy still pays one copy, the
same as with `const&`.

### 2. Missing fields (`parent_id`, `max_results`)

**Decision.** `std::optional<int>`. Missing and `null` both load as
`std::nullopt`.

**Why.** If someone forgets the missing case, the code doesn't compile:
`std::optional<int>` has no arithmetic operators, so a read needs `*`,
`.value()` or `.value_or(...)`. The type also shows that the field may be
missing. A sentinel such as `0` collides with real values: `0` is a valid id,
and `"max_results": 0` is a present value the spec says to keep.

**Cost.** Every place that reads the field has to handle "missing", usually in
one line. It also takes 8 bytes instead of 4.

### 3. The default of 5 for `max_results`

**Decision.** In the week 2 engine. It reads
`request.max_results().value_or(kDefaultMaxResults)` at the moment it uses the
value, with the 5 kept in a named constant.

**Why.** The record stays a faithful copy of the file, which WEEK1.md §2.3
requires ("including whether `max_results` was present"). The default is a
search rule, and §2.1 puts it there ("week 2 uses a default of 5").

**Cost.** Every user of a request has to know the rule. The week 5 cache must
treat "missing" and `5` as the same key, because they give the same results. So
the rule lives in two places. Both should call one shared
`effective_max_results(const Request&)` rather than each writing its own
`value_or(5)`. That function is week 2 code, not part of the week 1 headers.

### 4. Free functions or a loader class

**Decision.** Free functions: `load_categories`, `load_products`,
`load_requests` and `write_results`. Each loader returns its
`std::vector<...>` by value.

**Why.** A `Loader` class would hold nothing. The spec passes the path on every
call (§2.3), and an open file is only needed during one load. A class would
only add a line that creates an empty object
(`bazaar::Loader l; l.load_products(tmp);` instead of
`bazaar::load_products(tmp);`). Returning by value is cheap: on `return
result;` the compiler may skip the copy (NRVO, which is allowed but not
guaranteed), and if it doesn't, the language guarantees a move. The caller also
can't pass in a vector that already holds data. If loading throws, the caller's
variables are left untouched instead of half filled.

**Cost.** If a setting appears later, such as a base folder or a
"strict mode" that rejects extra fields, every call site has to pass it. A
class would have stored it once.

**Path parameter.** `const std::string& path`. `std::ifstream`
accepts it directly, and error messages must contain the path *exactly as
passed in*. A `std::string` goes into the message unchanged. A
`std::filesystem::path` needs care: writing it to a stream with `<<` adds
quotes (`"data/products.json"`), and on Windows it stores wide characters, so
`.string()` can fail on a non-ASCII file name. Cost: the type doesn't say "this
is a path".

### 5. Reporting failure

**Decision.** Exceptions. Every loader and the writer throw
`bazaar::DataError`, which derives from `std::runtime_error` and reuses its
constructor. The message comes from `.what()`.

**Why.** The command line tool needs one `try`/`catch` in `main` that prints
`e.what()` and exits, and a test is just
`EXPECT_THROW(load_products(bad), bazaar::DataError)`. A returned "data or
error" struct would have to be checked at every call, and I'd have to write it
myself, because `std::expected` is C++23. My own type lets a caller catch only
bazaar's data errors and not some other `std::runtime_error` from elsewhere.

**Name.** One type named `DataError` (not `LoadError`), because the
writer throws it too, and "load error" would be wrong for a file that can't be
written. One type is enough this week, since the CLI and the tests treat every
failure the same way. Cost: a caller can't catch write failures separately from
load failures. If that's ever needed, add two derived types then.

**nlohmann's exceptions.** The loaders catch them and throw a `DataError` with
the path, and `line N` for JSON Lines, because §2.4 requires the path and
nlohmann's messages don't contain it. That catch is a safety net. The main
checks are my own: `contains` for required fields, `is_null`, and
`is_number_integer()` / `is_string()` *before* calling `get`. `get<int>()`
silently turns `3.5` into `3` and `true` into `1`. Doing my own checks also
lets the message name the field, as in
`data/requests.jsonl: line 3: missing field "region"`.

**Cost.** Nothing in `std::vector<Product> load_products(const std::string&
path)` tells a reader that it can throw, or which type. So a comment above each
declaration says what is thrown and when. `main` catches
`const std::exception&`, so an unexpected `std::bad_alloc` still prints a
message instead of crashing.

### 6. nlohmann-json in the public header

**Decision.** No. `<nlohmann/json.hpp>` is included only in the `.cpp` files.
Turning a JSON object into a record is done by helper functions in an unnamed
namespace inside the loader's `.cpp`. Each helper checks the fields and then
calls the record's constructor.

**Why.** No declaration in the public header needs a json type, because the
loaders take a path and return vectors of bazaar records. Including it anyway
would paste tens of thousands of lines into every file that includes a bazaar
header, including week 2's engine, which never touches JSON. It would also make
all of them depend on nlohmann, so swapping JSON libraries would touch all of
them. nlohmann's `from_json(const json&, Product&)` pattern doesn't fit this
design anyway. It fills in an object that already exists, which needs a default
constructor and a way to set fields, and the records have neither. Keeping
nlohmann in the `.cpp` files also lets the build link it `PRIVATE`, so it never
reaches the tests' or the CLI's include paths.

**Cost.** Tests can't hand a json object straight to the parsing code. They
have to write a temporary file and call the loader. The spec already requires
loaders to take a path (§2.3), so tests work that way regardless. The helpers
themselves can't be tested on their own.

### 7. Headers

**Decision.** One public header, `include/bazaar/bazaar.h`, holding the four
records, `DataError`, the three loaders and `write_results`. It includes
`<optional>`, `<stdexcept>`, `<string>` and `<vector>`.

**Why.** It's the simplest option: one file to read and one `#include` for
every user. A split only pays off when some file needs part of it, and the
first such file (week 2's engine, which needs the records but not the loaders)
doesn't exist yet.

**Cost.** Every file that includes it gets everything, including declarations
it doesn't use. That's cheap now because the header is small and nlohmann stays
out of it (question 6). When week 2 arrives, splitting into a records header
and an I/O header is a small change: move the declarations and update the
includes.
