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
Current CI matrix runs on **macOS** and **Ubuntu Linux**.

### CI Options Considered

| Service        | Pros                                 | Cons                                   |
|---------------|--------------------------------------|----------------------------------------|
| GitHub Actions| Native to GitHub, free for OSS,      | YAML syntax can be verbose,
|               | easy trigger configuration           | limited Windows/macOS resources        |
| Travis CI     | Simple configuration, runs on Mac/Win| Free tier limited, slower builds       |
| CircleCI      | Fast containers, good caching        | Requires external signup, not native   |

### CI Triggers & Test Execution

* Tests are executed on pushes to `main`, `feature-lon`, `develop`, and `feature/**`,
   and on PRs targeting `main`.
* The workflow builds both the game executable and the `run_tests` binary,
  then executes the full test suite via `ctest --output-on-failure`.
  Any failing test causes the workflow to fail, preventing merges.
* On successful CI runs, the workflow also uploads a downloadable packaged
   build artifact named `codemon-dist-${{ matrix.os }}` containing `dist/codemon`.
* Developers are expected to run the local test flow before pushing:
   `cmake --build build --target run_tests && ./build/run_tests` (or CTest).

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
5. Run `cmake --build build --target run_tests && ./build/run_tests` to
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

## Current Local Test Commands

From repository root:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build -j
cmake --build build --target run_tests
./build/run_tests
```

Alternative via CTest:

```bash
ctest --test-dir build --output-on-failure
```

Verbose diagnostic mode (optional):

**macOS/Linux (bash/zsh)**
```bash
CODEMON_VERBOSE_CHALLENGE_LOADER=1 ./build/run_tests
CODEMON_VERBOSE_STATE_LOGS=1 ./build/run_tests
```

**Windows (PowerShell)**
```powershell
$env:CODEMON_VERBOSE_CHALLENGE_LOADER='1'; .\build\Debug\run_tests.exe
$env:CODEMON_VERBOSE_STATE_LOGS='1'; .\build\Debug\run_tests.exe
```

**Windows (CMD)**
```bat
set CODEMON_VERBOSE_CHALLENGE_LOADER=1 && .\build\Debug\run_tests.exe
set CODEMON_VERBOSE_STATE_LOGS=1 && .\build\Debug\run_tests.exe
```

Use this mode only when investigating parser/state-flow behavior. Default runs should remain quiet for readable CI/local output.

Windows multi-config note:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=C:/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Debug --target run_tests
.\build\Debug\run_tests.exe
ctest --test-dir build -C Debug --output-on-failure
```

## Packaging Smoke Test

We include a lightweight integration check to validate local release packaging output.

Run only the packaging smoke test:

```bash
ctest --test-dir build -R PackageDistSmoke --output-on-failure
```

This verifies that `dist/codemon` is produced and contains required runtime files:
- executable
- `assets/`
- `challenges.txt`
- `codemonNames.txt`

Current total local CTest checks: **58**
- **57** GoogleTest checks from `run_tests`
- **1** packaging integration check (`PackageDistSmoke`)
