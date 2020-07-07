//
// Created by brothercomplex on 28/10/19.
//

#ifndef THREAD_EXECUTIONMANAGER_H
#define THREAD_EXECUTIONMANAGER_H

#include <unistd.h>
#include <vector>
#include <cassert>
#include <functional>

#include <stack/stack.h>
class Thread {
public:
    long int pid = -1; // process itself may be 0, init
    Stack stack = Stack();
    bool died = false;
    int returnCode = 0;
    bool suspend = false;
    bool killable = false;
    int status = 0;
    int signal = -1;
    bool core_dumped = false;
    bool reaped = false;
    bool reapable = false;
    bool terminated_by_user = false;
    bool killed_by_user = false;
    class {
    public:
        enum {
            UNDEFINED,
            RUNNING,
            STOPPED,
            DEAD,
            EXITED,
            KILLED_BY_SIGNAL
        };
    } statusList;
};

struct QTS {
    bool createSuspended = false;
    size_t stack_size = 0;
    // TODO: function pointer template specilization: https://godbolt.org/z/h3kE3D
    std::function<int(void*)> f = nullptr;
    void * arg = nullptr;
    Thread * t = nullptr;
    bool push = false;
};

class ExecutionManager {
    public:
        bool debug = false;
        bool close = false;

        Thread * thread = nullptr;
        bool isRunning = false;
        std::vector<struct QTS *> QTS_VECTOR;
        bool errorChecking(struct QTS * q);
        static void * sendRequest(int request, void * package);
        void handleRequest(ExecutionManager * EX);

        class REQUEST {
            public:
                int request = -1;
                void * package = nullptr;
                bool upload = false;
                bool uploadNotComplete = false;
                bool processing = false;
                bool processingNotComplete = false;
                void * reply = nullptr;
                Stack stack = Stack();
        } REQUEST;

        static const class {
            public:
                enum {
                    threadCreate,
                    threadJoin,
                    threadJoinUntilStopped,
                    threadWaitUntilStopped,
                    threadWaitUntilRunning,
                    threadWaitUntilRunningAndJoin,
                    threadWaitUntilRunningAndJoinUntilStopped,
                    threadPause,
                    threadResume,
                    threadResumeAndJoin,
                    threadTerminate,
                    threadKill
                };
        } REQUEST_ID;

        // threads
        Thread * threadNew(size_t stack_size, std::function<int(void*)> f, void * arg);
        Thread * threadNew(std::function<int(void*)> f, void * arg);
        Thread * threadNew(size_t stack_size, std::function<void()> f);
        Thread * threadNew(std::function<void()> f);
        Thread * threadNew(bool createSuspended, size_t stack_size, int (*f)(void*), void * arg);
        Thread * threadNew(bool createSuspended, size_t stack_size, std::function<int(void*)> f, void * arg);
        Thread * threadNewSuspended(size_t stack_size, std::function<int(void*)> f, void * arg);
        Thread * threadNewSuspended(std::function<int(void*)> f, void * arg);
        Thread * threadNewSuspended(size_t stack_size, std::function<void()> f);
        Thread * threadNewSuspended(std::function<void()> f);
        static void threadJoin(Thread * t);
        static void threadJoinUntilStopped(Thread * t);
        static void threadWaitUntilStopped(Thread * t);
        static void threadWaitUntilRunning(Thread * t);
        static void threadWaitUntilRunningAndJoin(Thread * t);
        static void threadWaitUntilRunningAndJoinUntilStopped(Thread * t);
        static bool threadIsRunning(Thread * t);
        static bool threadIsStopped(Thread * t);
        static void threadPause(Thread * t);
        static void threadResume(Thread * t);
        static void threadResumeAndJoin(Thread * t);
        static void threadTerminate(Thread * t);
        static void threadKill(Thread * t);
};



void executionManager_Terminate();

extern ExecutionManager * executionManager_Current;

void executionManager_SetExecutionManager(ExecutionManager * executionManager);
ExecutionManager * executionManager_GetExecutionManager();

void setExecutionManager(ExecutionManager * executionManager);
ExecutionManager * getExecutionManager();

Thread * threadNew(size_t stack_size, std::function<int(void*)> f, void * arg);
Thread * threadNew(std::function<int(void*)> f, void * arg);
Thread * threadNew(size_t stack_size, std::function<void()> f);
Thread * threadNew(std::function<void()> f);
Thread * threadNew(bool createSuspended, size_t stack_size, int (*f)(void*), void * arg);
Thread * threadNew(bool createSuspended, size_t stack_size, std::function<int(void*)> f, void * arg);
Thread * threadNewSuspended(size_t stack_size, std::function<int(void*)> f, void * arg);
Thread * threadNewSuspended(std::function<int(void*)> f, void * arg);
Thread * threadNewSuspended(size_t stack_size, std::function<void()> f);
Thread * threadNewSuspended(std::function<void()> f);
void threadJoin(Thread * t);
void threadJoinUntilStopped(Thread * t);
void threadWaitUntilStopped(Thread * t);
void threadWaitUntilRunning(Thread * t);
void threadWaitUntilRunningAndJoin(Thread * t);
void threadWaitUntilRunningAndJoinUntilStopped(Thread * t);
bool threadIsRunning(Thread * t);
bool threadIsStopped(Thread * t);
void threadPause(Thread * t);
void threadResume(Thread * t);
void threadResumeAndJoin(Thread * t);
void threadInfo(Thread * t);
void threadTerminate(Thread * t);
void threadKill(Thread * t);

#endif //THREAD_EXECUTIONMANAGER_H
