cmake_minimum_required(VERSION 2.8)
project( DisplayImage )
find_package( OpenCV REQUIRED )
add_definitions(-std=c++11)
set(CONTRIB_PATH "/home/hurton/Documents/opencv-4.2.0/build/lib")

include_directories( ${OpenCV_INCLUDE_DIRS} )
#include_directories("/home/hurton/Documents/opencv-4.2.0/opencv_contrib-4.2.0/modules/saliency/include") # ohhhhhhhhhhhh
include_directories("/home/hurton/Documents/opencv-4.2.0/opencv_contrib-4.2.0/modules/xfeatures2d/include")

add_executable( DisplayImage DisplayImage.cpp )
#target_link_libraries( DisplayImage ${OpenCV_LIBS} "${CONTRIB_PATH}/libopencv_saliency.so" "${SAL_PATH}/libopencv_saliency.so.4.2" "${SAL_PATH}/libopencv_saliency.so.4.2.0")# ohhhhhhhhhhhh

target_link_libraries( DisplayImage ${OpenCV_LIBS} "${CONTRIB_PATH}/libopencv_xfeatures2d.so")

