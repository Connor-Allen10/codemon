cmake_minimum_required(VERSION 3.20)

project(codemon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Handle SFML Pathing for macOS (Homebrew), Linux (system packages), and Windows (vcpkg)
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
endif()
if(UNIX AND NOT APPLE)
    # On Linux, try standard system paths
    list(APPEND CMAKE_PREFIX_PATH "/usr" "/usr/local")
endif()
if(WIN32)
    # Windows with vcpkg integration
    # CMAKE_TOOLCHAIN_FILE should be set via command line or vcpkg
endif()

# 2. Find SFML (flexible version: accept 2.6+ or 3.0+)
# Try SFML 3.0 first, fall back to 2.6 if not found
find_package(SFML 3.0 QUIET COMPONENTS Network Graphics Window Audio System)
if(NOT SFML_FOUND)
    message(STATUS "SFML 3.0 not found, trying SFML 2.6...")
    find_package(SFML 2.6 REQUIRED COMPONENTS network graphics window audio system)
    # SFML 2.x uses lowercase component names
    set(SFML_LIBS sfml-graphics sfml-window sfml-system sfml-audio sfml-network)
else()
    message(STATUS "Found SFML 3.0")
    # SFML 3.x uses modern target names
    set(SFML_LIBS SFML::Graphics SFML::Window SFML::System SFML::Audio SFML::Network)
endif()

# Optional TGUI setup
# - macOS: build from source to avoid incompatible prebuilt archives
# - Other platforms: use vendored config if present, otherwise system package
set(HAS_TGUI OFF)
if(APPLE)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/TGUI/CMakeLists.txt")
        set(TGUI_BACKEND SFML_GRAPHICS CACHE STRING "TGUI backend")
        add_subdirectory(external/TGUI)
        set(HAS_TGUI ON)
    else()
        find_package(TGUI QUIET CONFIG)
        if(TGUI_FOUND)
            set(HAS_TGUI ON)
        endif()
    endif()
else()
    set(TGUI_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/TGUI/lib/cmake/TGUI")
    find_package(TGUI QUIET CONFIG)
    if(TGUI_FOUND)
        set(HAS_TGUI ON)
    else()
        unset(TGUI_DIR)
        find_package(TGUI QUIET CONFIG)
        if(TGUI_FOUND)
            set(HAS_TGUI ON)
        endif()
    endif()
endif()

if(NOT HAS_TGUI)
    message(STATUS "TGUI not found; continuing without TGUI linkage")
endif()

# 3. Find all source files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# 4. Create the main game executable
add_executable(codemon ${SOURCES})

target_include_directories(codemon PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# 5. Include headers and link SFML using modern target approach
target_link_libraries(codemon PRIVATE
    ${SFML_LIBS}
)

if(HAS_TGUI)
    target_link_libraries(codemon PRIVATE TGUI::TGUI)
endif()

# 6. Copy assets to the build folder
if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/assets")
    file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/src/assets" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
endif()

# =========================================================
# 6b. Local release packaging (dist/)
# =========================================================
# Creates a runnable distribution folder containing:
# - codemon executable
# - assets
# - runtime data files (challenge/name lists)
#
# Usage:
#   cmake --build build --target package_dist
set(CODEMON_DIST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/dist/codemon")

add_custom_target(package_dist
    COMMAND ${CMAKE_COMMAND} -E rm -rf "${CODEMON_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CODEMON_DIST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:codemon>" "${CODEMON_DIST_DIR}/"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_BINARY_DIR}/assets" "${CODEMON_DIST_DIR}/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/challenges.txt" "${CODEMON_DIST_DIR}/challenges.txt"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/codemonNames.txt" "${CODEMON_DIST_DIR}/codemonNames.txt"
    COMMAND ${CMAKE_COMMAND} -E echo "Packaged Codemon to ${CODEMON_DIST_DIR}"
    DEPENDS codemon
    COMMENT "Creating local distribution folder in dist/codemon"
)

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

# Some GoogleTest cases rely on implementation files (e.g. TileMap, DebugChallenge, WorldState)
# that are not themselves part of the main codemon target.  List them explicitly
# so that the test binary links correctly.
target_sources(run_tests PRIVATE
    src/world/TileMap.cpp
    src/battle/Party.cpp
    src/debug/DebugChallenge.cpp
    src/debug/ChallengeLoader.cpp
    src/states/WorldState.cpp
    src/states/BattleState.cpp
    src/states/MainMenuState.cpp
    src/game/StateStack.cpp
    src/game/Game.cpp
)

# Let tests find project headers via the source tree root
target_include_directories(run_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Link GoogleTest + SFML using modern target approach
target_link_libraries(run_tests PRIVATE
    GTest::gtest_main
    ${SFML_LIBS}
)

# Modern test discovery (Works perfectly on Mac and Linux)
include(GoogleTest)
gtest_discover_tests(run_tests)

# =========================================================
# 8. Packaging smoke test (CTest integration)
# =========================================================
# Runs package_dist and verifies required distribution artifacts exist.
add_test(
    NAME PackageDistSmoke
    COMMAND ${CMAKE_COMMAND}
        -DCMAKE_COMMAND_PATH=${CMAKE_COMMAND}
        -DBUILD_DIR=${CMAKE_CURRENT_BINARY_DIR}
        -DSOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}
        -P ${CMAKE_CURRENT_SOURCE_DIR}/cmake/VerifyPackageDist.cmake
)
