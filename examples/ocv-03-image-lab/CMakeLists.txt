# ocv-03-image-lab: a gradient test card through the OpenCV geometry, tone and gradient stages.
cmake_minimum_required(VERSION 3.25)
project(paperboat_ocv_lab LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT PAPERBOAT_ENGINE_DIR)
  foreach(dir
      "${CMAKE_CURRENT_LIST_DIR}/../../../../"
      "${CMAKE_CURRENT_LIST_DIR}/../../engine"
      "${CMAKE_CURRENT_LIST_DIR}/../../../engine"
      "${CMAKE_CURRENT_LIST_DIR}/../../../../../../paperboat/engine")
    if(EXISTS "${dir}/CMakeLists.txt")
      set(PAPERBOAT_ENGINE_DIR "${dir}")
      break()
    endif()
  endforeach()
endif()
if(NOT PAPERBOAT_ENGINE_DIR)
  message(FATAL_ERROR "No Paperboat engine found: set PAPERBOAT_ENGINE_DIR")
endif()
set(PAPERBOAT_ENGINE_DIR "${PAPERBOAT_ENGINE_DIR}" CACHE PATH "Paperboat engine sources")
add_subdirectory(${PAPERBOAT_ENGINE_DIR} paperboat EXCLUDE_FROM_ALL)

if(NOT TARGET paperboat::toolkit_opencv)
  foreach(dir
      "${PAPERBOAT_ENGINE_DIR}/toolkit/opencv"
      "${CMAKE_CURRENT_LIST_DIR}/../.."
      "${CMAKE_CURRENT_LIST_DIR}/../../engine/toolkit/opencv")
    if(EXISTS "${dir}/toolkit.toml")
      add_subdirectory("${dir}" ocv-toolkit)
      break()
    endif()
  endforeach()
endif()
if(NOT TARGET paperboat::toolkit_opencv)
  message(FATAL_ERROR "The opencv toolkit is not installed (paperboat toolkit install opencv)")
endif()

add_executable(paperboat-ocv-lab main.cpp domain/scene.cpp)
target_link_libraries(paperboat-ocv-lab PRIVATE paperboat::core paperboat::toolkit_opencv paperboat_warnings)
target_compile_definitions(paperboat-ocv-lab PRIVATE
  PAPERBOAT_DESCRIPTOR="${CMAKE_CURRENT_SOURCE_DIR}/descriptor.toml")
