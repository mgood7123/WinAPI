cmake_minimum_required(VERSION 3.4.1)

add_executable(
        thread_safe_tests

        ../src/Lock.cpp
        #            ../../libstack/src/new.cpp
        #            ../../libstack/src/direction.cpp
        thread_safe_lock.cpp

        ../src/thread_safeExceptions.cpp
            thread_safe_general.cpp
            thread_safe_thread.cpp
)

target_link_libraries(thread_safe_tests pthread gtest_main)

add_custom_command( TARGET thread_safe_tests POST_BUILD COMMAND mv thread_safe_tests thread_safe_EXECUTABLES/thread_safe_tests)
