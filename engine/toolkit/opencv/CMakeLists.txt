# The optional opencv toolkit: its manifest lets the engine discover this folder; it is a
# separate target because it depends on OpenCV. See include/paperboat/toolkit/opencv.hpp.

find_package(OpenCV QUIET COMPONENTS core imgproc imgcodecs)
option(PAPERBOAT_WITH_OPENCV "Build the opencv toolkit package (paperboat::toolkit_opencv)" ${OpenCV_FOUND})
if(NOT PAPERBOAT_WITH_OPENCV)
  return()
endif()
find_package(OpenCV REQUIRED COMPONENTS core imgproc imgcodecs)

# One folder per block: src/opencv/<block>/<block>.cpp. src/ holds the image helpers every block
# shares (image.cpp, common.hpp) and register.cpp.
file(GLOB PAPERBOAT_OPENCV_BLOCKS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/opencv/*/*.cpp")
add_library(paperboat_toolkit_opencv src/image.cpp src/register.cpp ${PAPERBOAT_OPENCV_BLOCKS})
add_library(paperboat::toolkit_opencv ALIAS paperboat_toolkit_opencv)
target_include_directories(paperboat_toolkit_opencv PUBLIC include PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(paperboat_toolkit_opencv
  PUBLIC paperboat::toolkit opencv_core opencv_imgproc
  PRIVATE opencv_imgcodecs paperboat_warnings)
