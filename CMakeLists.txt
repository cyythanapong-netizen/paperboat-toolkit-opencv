# paperboat-toolkit-opencv: the optional image-processing toolkit for Paperboat.
#
# Builds paperboat::toolkit_opencv. Two modes:
#   1. In-tree: drop/copy this folder into a paperboat checkout at
#      engine/toolkit/opencv/ and configure the engine normally (it is discovered
#      automatically from its toolkit.toml).
#   2. Standalone: set PAPERBOAT_SOURCE_DIR to a paperboat checkout, or let it
#      FetchContent the engine from GitHub.

cmake_minimum_required(VERSION 3.20)
project(paperboat_toolkit_opencv LANGUAGES CXX)

include(FetchContent)

if(NOT DEFINED PAPERBOAT_SOURCE_DIR OR NOT EXISTS "${PAPERBOAT_SOURCE_DIR}/engine/CMakeLists.txt")
  message(STATUS "PAPERBOAT_SOURCE_DIR not set; fetching paperboat engine from GitHub")
  FetchContent_Declare(paperboat
    GIT_REPOSITORY https://github.com/cyythanapong-netizen/paperboat.git
    GIT_TAG dev
  )
  FetchContent_MakeAvailable(paperboat)
  set(PAPERBOAT_SOURCE_DIR "${paperboat_SOURCE_DIR}" CACHE PATH "Path to the paperboat source tree")
endif()

add_subdirectory("${PAPERBOAT_SOURCE_DIR}/engine" "${CMAKE_BINARY_DIR}/paperboat-engine" EXCLUDE_FROM_ALL)
add_subdirectory(engine/toolkit/opencv)
