cmake_minimum_required(VERSION 3.4.1)

include_directories(include)

add_library(thread SHARED src/ExecutionManager.cpp)
add_dependencies(thread stack println)
target_link_libraries(thread stack println)
#add_subdirectory(tests)
