# Test Plan & CI Infrastructure

## Overview

Our project uses **GoogleTest** for C++ unit and integration tests.  The
choice was driven by its seamless CMake integration, widespread adoption in
C++ projects, and the fact that it is already bundled in the build system
through `FetchContent` (see CMakeLists.txt).  Adding a new test file is as
simple as creating a `tests/test_<name>.cpp` file containing `TEST()`
macros; the CMake configuration automatically glob‑collects any `.cpp` under
`tests/` and builds them into the `run_tests` executable.  Developers may also
link extra implementation sources via `target_sources` if the test needs to
instantiate non‑header‑only code.

## Continuous Integration

We run tests on every push and pull request using **GitHub Actions**.  This
choice was made because the repository is hosted on GitHub, Actions has free
minutes for open‑source projects, and its YAML workflows are easy to extend.
The workflow file (`.github/workflows/cmake-build.yml`) checks out the code, installs
SFML and necessary tools, runs CMake to configure and build the
project, and finally invokes `ctest` to execute all discovered tests.

### CI Options Considered

| Service        | Pros                                 | Cons                                   |
|---------------|--------------------------------------|----------------------------------------|
| GitHub Actions| Native to GitHub, free for OSS,      | YAML syntax can be verbose,
|               | easy trigger configuration           | limited Windows/macOS resources        |
| Travis CI     | Simple configuration, runs on Mac/Win| Free tier limited, slower builds       |
| CircleCI      | Fast containers, good caching        | Requires external signup, not native   |

### CI Triggers & Test Execution

* Tests are executed on every push to `main` and on every PR targeting
  `main`.
* The workflow builds both the game executable and the `run_tests` binary,
  then executes the full test suite via `ctest --output-on-failure`.
  Any failing test causes the workflow to fail, preventing merges.
* Developers are expected to run `cmake --build build && cd build && ctest`
  locally before pushing to ensure their changes don't break CI.

## How to Add a New Test

1. Create a new file under `tests/test_<component>.cpp`
2. Include the appropriate headers and Google Test header:
   ```cpp
   #include <gtest/gtest.h>
   #include "path/to/header.hpp"
   ```
3. Write your test cases using the `TEST()` macro:
   ```cpp
   TEST(TestSuiteName, TestCaseName) {
       // Setup
       MyClass obj("value");
       
       // Action & Assert
       EXPECT_EQ(obj.getValue(), "value");
   }
   ```
4. If your test needs to link additional implementation files, update
   `CMakeLists.txt` under the "# Some GoogleTest cases rely on..." section
   and add your source via `target_sources(run_tests PRIVATE src/path.cpp)`.
5. Run `cmake --build build && cd build && ctest --output-on-failure` to
   verify the test compiles and passes.
6. Commit your test file to the repository; the CI workflow will automatically
   run it on the next push.

## Best Practices

- **Unit tests** should test individual functions/methods in isolation.
- **Validation tests** should verify edge cases and error conditions.
- **Integration tests** should test interactions between multiple components.
- **System tests** should test the entire application in a real environment.
- Keep test names descriptive so failures are easy to understand.
- Avoid external dependencies (files, network) when possible; use mocks
  or temporary data.
- Aim for high code coverage, but prioritize testing critical paths.

By maintaining this infrastructure and following the above process, any new
team member can add tests and automatically have them run in CI, meeting the
requirements of Milestone 5.
