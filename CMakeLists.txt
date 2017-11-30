cmake_minimum_required(VERSION 3.8)
project(cv330)

set(CMAKE_CXX_STANDARD 11)


#include_directories("/home/vmachado/work/opt/opencv-3.3.0/include/")
#link_directories("/home/vmachado/work/opt/opencv-3.3.0/build/lib/")

#find_package( OpenCV 3.3.0 PATHS "/home/vmachado/OpenCV3.3.0")

find_package(TensorflowCC COMPONENTS Shared)

find_package( OpenCV 3.3.1)

set(SOURCE_FILES main.cpp main2.cpp detection_tf.cpp)
add_executable(cv330 ${SOURCE_FILES})
target_link_libraries( cv330 ${OpenCV_LIBS})
target_link_libraries( cv330 TensorflowCC::Shared)