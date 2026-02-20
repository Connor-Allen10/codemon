cmake_minimum_required(VERSION 3.20)

project(codemon LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# This helps CMake find Homebrew SFML on Apple Silicon Macs
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew")
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