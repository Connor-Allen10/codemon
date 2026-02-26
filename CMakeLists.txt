cmake_minimum_required(VERSION 3.20)

project(codemon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This helps CMake find Homebrew SFML on Apple Silicon Macs
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()

# Help Windows find SFML if using vcpkg
if(WIN32 AND NOT SFML_DIR)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_INSTALLATION_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

# 1. Find SFML
# This works on Mac (Homebrew) and Windows (if SFML is in their PATH or CMake prefix)
find_package(SFML COMPONENTS Network Graphics Window Audio System CONFIG REQUIRED)

# 2. Automatically find all source files in src/ and its subfolders
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 3. Create the executable using all found sources
add_executable(codemon ${SOURCES})

# 4. Tell the compiler where your header files are
target_include_directories(codemon PRIVATE include)

# 5. Link the SFML libraries
target_link_libraries(codemon PRIVATE
    SFML::Graphics
    SFML::Window
    SFML::System
    SFML::Audio
    SFML::Network
)

# 6. Copy assets to the build folder so the game can find them
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# =========================================================
# 7. Milestone 5: Automated Testing (GoogleTest)
# =========================================================

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
# For Windows compatibility
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create a separate executable for running tests
add_executable(run_tests tests/test_stats.cpp)

# Link GoogleTest to the test executable
target_link_libraries(run_tests PRIVATE GTest::gtest_main)

# Allow CMake to discover and list tests in the VS Code Test Explorer
include(GoogleTest)
gtest_discover_tests(run_tests DISCOVERY_MODE PRE_TEST)