# Packaging integration smoke test.
#
# This script is run by CTest (PackageDistSmoke) and verifies that the
# local distribution folder can be produced and contains required runtime
# artifacts for launching Codemon outside the source tree.

if(NOT DEFINED CMAKE_COMMAND_PATH)
  message(FATAL_ERROR "CMAKE_COMMAND_PATH is required")
endif()
if(NOT DEFINED BUILD_DIR)
  message(FATAL_ERROR "BUILD_DIR is required")
endif()
if(NOT DEFINED SOURCE_DIR)
  message(FATAL_ERROR "SOURCE_DIR is required")
endif()

# Build the distribution folder using the project-defined packaging target.
execute_process(
  COMMAND "${CMAKE_COMMAND_PATH}" --build "${BUILD_DIR}" --target package_dist
  RESULT_VARIABLE package_result
)

if(NOT package_result EQUAL 0)
  message(FATAL_ERROR "package_dist target failed with code ${package_result}")
endif()

set(DIST_DIR "${SOURCE_DIR}/dist/codemon")

if(NOT EXISTS "${DIST_DIR}")
  message(FATAL_ERROR "Missing distribution directory: ${DIST_DIR}")
endif()

if(WIN32)
  set(EXECUTABLE_PATH "${DIST_DIR}/codemon.exe")
else()
  set(EXECUTABLE_PATH "${DIST_DIR}/codemon")
endif()

if(NOT EXISTS "${EXECUTABLE_PATH}")
  message(FATAL_ERROR "Missing packaged executable: ${EXECUTABLE_PATH}")
endif()

if(NOT EXISTS "${DIST_DIR}/assets")
  message(FATAL_ERROR "Missing packaged assets directory: ${DIST_DIR}/assets")
endif()

if(NOT EXISTS "${DIST_DIR}/challenges.txt")
  message(FATAL_ERROR "Missing packaged runtime file: ${DIST_DIR}/challenges.txt")
endif()

if(NOT EXISTS "${DIST_DIR}/codemonNames.txt")
  message(FATAL_ERROR "Missing packaged runtime file: ${DIST_DIR}/codemonNames.txt")
endif()

message(STATUS "PackageDistSmoke passed: ${DIST_DIR}")
