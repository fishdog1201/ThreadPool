cmake_minimum_required(VERSION 3.20)
project(ThreadPool)

set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

add_executable(ThreadPool main.cpp thread_pool.cpp)