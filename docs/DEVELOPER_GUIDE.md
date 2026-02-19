# Codémon: Developer Guidelines

This document provides instructions for setting up the development 
environment, understanding the architecture, and contributing to 
the Codémon project.

## 1. Obtaining the Source Code
Clone the repository using Git:
```bash
git clone [https://github.com/Connor-Allen10/codemon.git](https://github.com/Connor-Allen10/codemon.git)
```

## 2. Directory Structure
* `/src`: Contains all .cpp and .hpp source files (Game logic, UI, Engine).
* `/assets`: JSON data (species.json), textures, and graphical assets.
* `/docs`: Project documentation, user manuals, and milestone reports.
* `/lib`: External SFML headers and library binaries.
* `/status_reports`: Weekly progress updates and sprint planning.

## 3. How to Build
We use CMake as our build system to manage dependencies like SFML.

### Prerequisites:
* C++ Compiler: GCC, Clang, or MSVC (supporting C++17 or higher).
* CMake: Version 3.10 or higher.
* SFML: Version 2.5.1.

### Build Steps:
1. Open a terminal in the project root.
2. Create a build directory: `mkdir build && cd build`
3. Configure the project: `cmake ..`
4. Compile the software: `cmake --build .`

## 4. Testing the Software
We use an automated unit testing suite to verify game logic.

To run tests, execute the test binary generated in the /build folder: `./CodemonTests`

### Adding New Tests:
* Test files are located in the /src or a dedicated /tests folder.
* Naming Convention: New test files should follow the pattern `test_[ClassName].cpp`.

## 5. Building a Release
To package a release for end-users:
1. Switch the CMake configuration to Release mode: `cmake -DCMAKE_BUILD_TYPE=Release ..`
2. Build the project.
3. Manually copy the `/assets` folder into the directory containing the executable.
4. Ensure all necessary SFML binaries (.dll or .so files) are included.

## 6. Coding Guidelines
* Style: We follow the Google C++ Style Guide for consistency.
* Memory: Use Smart Pointers (std::unique_ptr, std::shared_ptr) to avoid memory leaks.
* Workflow: All feature contributions must be made via feature branches. Each Pull Request must be reviewed before merging to main.
