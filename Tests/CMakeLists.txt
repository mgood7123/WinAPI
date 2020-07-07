cmake_minimum_required(VERSION 3.4.1)

# for each test there exists a linux executable and a WinAPI shared library

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -g3 -O0")

add_executable(Threads Threads/Threads.cpp)
target_link_libraries(Threads WinAPI)

add_library(ThreadsWindowsApplication SHARED Threads/ThreadsWindowsApplication.cpp)
target_link_libraries(ThreadsWindowsApplication WinAPI)
