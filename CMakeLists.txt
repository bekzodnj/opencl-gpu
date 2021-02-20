cmake_minimum_required(VERSION 3.10)
project(T1)

set(CMAKE_CXX_STANDARD 11)
include_directories(/usr/local/cuda/include)

add_executable(T1 main.cpp)

target_link_libraries(${CMAKE_PROJECT_NAME} OpenCL)