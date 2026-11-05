# paperboat-toolkit-opencv: the optional image-processing toolkit for Paperboat.
#
# It builds paperboat::toolkit_opencv against an existing Paperboat engine source tree.
# Set PAPERBOAT_SOURCE_DIR to the root of a paperboat checkout (the repo that contains
# engine/). When #5 lands (installable engine), this will switch to find_package(paperboat).

cmake_minimum_required(VERSION 3.20)
project(paperboat_toolkit_opencv LANGUAGES CXX)

if(NOT DEFINED PAPERBOAT_SOURCE_DIR)
  set(PAPERBOAT_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../paperboat" CACHE PATH "Path to the paperboat source tree")
endif()

if(NOT EXISTS "${PAPERBOAT_SOURCE_DIR}/engine/CMakeLists.txt")
  message(FATAL_ERROR "PAPERBOAT_SOURCE_DIR must point at a paperboat checkout (missing engine/CMakeLists.txt): ${PAPERBOAT_SOURCE_DIR}")
endif()

# Build the engine (and its core toolkits) so the opencv toolkit can link against them.
# A backend that already has the engine can instead add this directory as a subdirectory.
add_subdirectory("${PAPERBOAT_SOURCE_DIR}/engine" "${CMAKE_BINARY_DIR}/paperboat-engine" EXCLUDE_FROM_ALL)

add_subdirectory(engine/toolkit/opencv)
