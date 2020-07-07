//
// Created by konek on 7/16/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPITHREAD_H
#define MEDIA_PLAYER_PRO_WINDOWSAPITHREAD_H

#include <deque>
#include <vector>
#include <mutex>
#include <signal.h>
#include <pthread.h>
#include <thread/thread.h>
#include "WindowsAPIProcess.h"
#include "Kernel/WindowsAPIKernel.h"
#include "Locker.h"

void handle_error_en(int en, const char * msg);

void handle_error(const char* msg);

typedef struct {
    LPTHREAD_START_ROUTINE __start_routine;
    LPVOID args;
} PTHREAD_MAIN_STRUCT;

typedef struct {
    Stack stack;
    size_t stack_size;
    Thread * thread;
    DWORD suspend_count;
    PTHREAD_MAIN_STRUCT MAIN_STRUCT;
} THREAD_INFO;

class threadpool {
private:
    std::mutex lock;
    size_t default_stack_size = 1_megabyte;
public:
    Kernel * threadKernel;
    threadpool();
    ~threadpool();

    void add(_In_ Kernel &KERNEL, DWORD dwCreationFlags, size_t stack_size_, LPTHREAD_START_ROUTINE __start_routine, LPVOID args, THREAD_INFO * threadinfo);

    BOOL contains (Thread * t);

    THREAD_INFO * find(Thread * t);

};

extern class threadpool WINDOWSAPITHREADPOOL;

extern
WINBASEAPI
_Ret_maybenull_
HANDLE
WINAPI
CreateThread(
        _In_ Kernel &KERNEL,
        _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
        _In_ SIZE_T dwStackSize,
        _In_ LPTHREAD_START_ROUTINE lpStartAddress,
        _In_opt_ __drv_aliasesMem LPVOID lpParameter,
        _In_ DWORD dwCreationFlags,
        _Out_opt_ LPDWORD lpThreadId
);

extern
WINBASEAPI
DWORD
WINAPI
SuspendThread(
        _In_ Kernel &KERNEL,
        _In_ HANDLE hThread
);


extern
WINBASEAPI
DWORD
WINAPI
ResumeThread(
        _In_ Kernel &KERNEL,
        _In_ HANDLE hThread
);

#endif //MEDIA_PLAYER_PRO_WINDOWSAPITHREAD_H
