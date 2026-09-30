# Berkeley Search

A learning project that explores C++, web crawling, and search-engine
fundamentals by crawling a small set of UC Berkeley EECS pages.

## Build and test

This project currently uses C++20 named modules with Apple Clang. Configure,
build, and run the tests with:

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

The crawler executable is written to `build/berkeley-search`. Running it makes
live HTTP requests, so keep the configured page limit small during development.

## Search from the terminal

Run the crawler and interactive search prompt with:

```sh
./build/berkeley-search
```

After crawling and indexing finish, enter queries such as `operating systems`.
An empty query exits. The first ranking method is intentionally simple: for
each distinct normalized query term, it adds that term's frequency in each
matching document. Results with equal scores are ordered by document ID.

## Headers compared with modules

The first version separated each component into a header and source file. A
consumer used `#include "tokenizer.h"`, and the preprocessor copied that header
into every translation unit that included it.

The module version puts the public declaration in a `.cppm` interface. A
consumer uses `import berkeley_search.tokenizer;`, and Clang loads a compiled
module interface (`.pcm`) instead of repeatedly processing the declaration as
text. The `.cpp` implementation file still exists; modules replace the public
header boundary, not the need for implementation code.

The tradeoff is build complexity. A module interface must be compiled before
its implementation and consumers. CMake normally discovers that ordering with
Ninja and `clang-scan-deps`, but those tools are not installed in the current
Apple Command Line Tools environment. `CMakeLists.txt` therefore spells out the
Clang module compilation order explicitly. This is useful for learning, but it
is intentionally Clang-specific and should be replaced with CMake's native
`FILE_SET CXX_MODULES` support when a compatible scanner and Ninja are
available.
