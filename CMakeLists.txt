cmake_minimum_required(VERSION 3.5)
project(fftw)

INCLUDE_DIRECTORIES("/opt/local/include")
link_directories("/opt/local/lib")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")



set(SOURCE_FILES test1.c)
add_executable(test1 ${SOURCE_FILES})
target_link_libraries (test1 LINK_PUBLIC fftw3)


set(SOURCE_FILES test2.c)
add_executable(test2 ${SOURCE_FILES})
target_link_libraries (test2 LINK_PUBLIC fftw3)