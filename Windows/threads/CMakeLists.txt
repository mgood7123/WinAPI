cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(gtest)

add_subdirectory(testBuilder)

#include_directories(libthread_safe/include)
#add_subdirectory(libthread_safe)
include_directories(include)
add_library(println SHARED include/println.cpp)

include_directories(libstack/include)
add_subdirectory(libstack)
include_directories(libthread/include)
add_subdirectory(libthread)
