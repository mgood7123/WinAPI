cmake_minimum_required(VERSION 3.4.1)

add_subdirectory(Kernel)
add_subdirectory(threads)

add_library(WinAPI SHARED
        WindowsAPIMessage.cpp
        WindowsAPIHander.cpp
        WindowsAPIProcess.cpp
        WindowsAPIThread.cpp
        Locker.cpp
        )
if(ANDROID)
    target_link_libraries(WinAPI WinKernel log EGL GLESv3 thread)
else()
    target_link_libraries(WinAPI WinKernel pthread dl thread)
endif()
