cmake_minimum_required(VERSION 3.20)

project(codemon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Handle SFML Pathing for macOS (Homebrew)
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()

# 2. Find SFML
# This will now work on Mac (via brew) and Ubuntu (via apt-get)
find_package(SFML COMPONENTS Network Graphics Window Audio System REQUIRED)

# 3. Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. Create the main game executable
add_executable(codemon ${SOURCES})

# 5. Include headers and link SFML
target_include_directories(codemon PRIVATE include)
target_link_libraries(codemon PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
    sfml-network
)

# 6. Copy assets to the build folder
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# =========================================================
# 7. Automated Testing (GoogleTest)
# =========================================================

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/heads/main.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Create the test executable by collecting all sources under tests/
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")
add_executable(run_tests ${TEST_SOURCES})

# Some GoogleTest cases rely on implementation files (e.g. TileMap) that are
# not themselves part of the main codemon target.  List them explicitly so
# that the test binary links correctly.
target_sources(run_tests PRIVATE
    src/world/TileMap.cpp
)

# Let tests find project headers via the source tree root
target_include_directories(run_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link GoogleTest + SFML because some tests use SFML types
# (TileMap, etc.)
target_link_libraries(run_tests PRIVATE
    GTest::gtest_main
    sfml-graphics
    sfml-window
    sfml-system
)

# Modern test discovery (Works perfectly on Mac and Linux)
include(GoogleTest)
gtest_discover_tests(run_tests)
