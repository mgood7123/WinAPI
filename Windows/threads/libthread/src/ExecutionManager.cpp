/*

// NOTE: users should use threadNew to create new threads, everything else in this will be internal

// TODO: implement tid lookup and return obtain
// NOTE: this will be tricky as the thread needs to be recycled when its return code is obtained
// if another thread starts with the same id due to pid recycling, and the previous thread return code has not been obtained
// then this is UB, it is unclear how this could be worked around given intentional abuse of pid recycling

// we cannot manually obtain the return code via the table index as the system releases all resources upon thread completion
*/

//
// Created by brothercomplex on 28/10/19.
//

#include <thread/internal/ExecutionManager.h>

ExecutionManager * executionManager_Current;

#include <println.h>

//#define println(format, ...) printf(format "\n", __VA_ARGS__)
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <signal.h>
#include <sched.h>
#include <println.h>
#include <thread/internal/ExecutionManager.h>

int ExecutionManager_instance(void * arg);

void executionManager_SetExecutionManager(class ExecutionManager * executionManager) {
    executionManager_Current = executionManager;
}

void setExecutionManager(class ExecutionManager * executionManager) {
    executionManager_SetExecutionManager(executionManager);
}

class ExecutionManager * executionManager_GetExecutionManager() {
    return executionManager_Current;
}

class ExecutionManager * getExecutionManager() {
    return executionManager_GetExecutionManager();
}

void executionManager_Terminate() {
    println("terminating execution manager");
    executionManager_Current->close = true;
    ExecutionManager::threadJoin(executionManager_Current->thread);
    println("terminated execution manager");
}

int suspended(void * arg) {
    struct QTS * q = static_cast<struct QTS *>(arg);
    assert(q != nullptr);
    assert(q->t != nullptr);
    q->t->died = false;
    q->t->killable = false;
    long int pid = getpid();
    if (executionManager_Current->debug) println("STOPPING THREAD %d", pid);
    q->t->status = q->t->statusList.STOPPED;
    kill(pid, SIGSTOP);
    q->t->status = q->t->statusList.RUNNING;
    if (executionManager_Current->debug) println("RESUMED THREAD %d", pid);
    assert(q->f != nullptr);
    assert(q->arg != nullptr);
    assert(q->t != nullptr);

    // main
    int R = q->f(q->arg);

    q->t->killable = true;
    if (executionManager_Current->debug) println("while (q->t->killable) THREAD %d", pid);
    while (q->t->killable) {
        sleep(1);
        println("waiting for killable to be false");
    }
    println("killable is false");
    if (executionManager_Current->debug) println("STOPPING THREAD %d, R = %d", pid, R);
    q->t->status = q->t->statusList.STOPPED;
    kill(pid, SIGSTOP);
    q->t->status = q->t->statusList.RUNNING;
    if (executionManager_Current->debug) println("RESUMED THREAD %d, R = %d, TERMINATING THREAD %d", pid, R, pid);
    return R;
}

Thread * threadNew_(struct QTS * q) {
    if (executionManager_Current->debug) println("threadNew_");
    if (executionManager_Current->debug) println("q->t = new Thread()");
    q->t = new Thread();
    if (executionManager_Current->debug) println("clone q->t = %p", q->t);
    q->t->suspend = q->createSuspended;
    q->t->stack.alloc(q->stack_size == 0 ? 4096 : q->stack_size);
    q->t->pid = clone(suspended, q->t->stack.top, CLONE_VM|SIGCHLD, q);
    if (executionManager_Current->debug) println("clone q->t->pid = %d", q->t->pid);
    if (q->t->pid == -1) {
        if (executionManager_Current->debug) perror("clone");
        q->t->stack.free();
    } else {
        if (q->push) {
            executionManager_Current->QTS_VECTOR.push_back(q);
            if (executionManager_Current->debug) println("returning 1 %p", executionManager_Current->QTS_VECTOR.back()->t);
            return executionManager_Current->QTS_VECTOR.back()->t;
        } else {
            if (executionManager_Current->debug) println("returning 2 %p", q->t);
            return q->t;
        }
    }
    if (executionManager_Current->debug) println("returning 3 nullptr");
    Thread * NULLPOINTER = nullptr;
    return NULLPOINTER;
}

Thread * QINIT() {
    if (executionManager_Current->isRunning) return executionManager_Current->thread;
    if (executionManager_Current->debug) println("initializing QINIT()");
    executionManager_Current->isRunning = true;
    // we cannot use threadnew_() as killable will never be triggered

    if (executionManager_Current->debug) println("q->t = new Thread()");
    struct QTS * q = new struct QTS;
    q->t = new Thread();
    if (executionManager_Current->debug) println("q->t = %p", q->t);
    q->t->stack.alloc(4096);
    q->t->pid = clone(ExecutionManager_instance, q->t->stack.top, CLONE_VM|SIGCHLD, executionManager_Current);
    if (q->t->pid == -1) {
        if (executionManager_Current->debug) perror("clone");
        q->t->stack.free();
    } else {
        kill(q->t->pid, SIGSTOP);
        executionManager_Current->QTS_VECTOR.push_back(q);
        executionManager_Current->thread = executionManager_Current->QTS_VECTOR.back()->t;
        kill(q->t->pid, SIGCONT);
        atexit(executionManager_Terminate);
        if (executionManager_Current->debug) println("initialized QINIT()");
        return executionManager_Current->thread;
    }
    if (executionManager_Current->debug) {
        println("failed to initialize QINIT()");
        println("returning 3 nullptr");
    }
    Thread * NULLPOINTER = nullptr;
    return NULLPOINTER;
}

void * ExecutionManager::sendRequest(int request, void * package) {
    executionManager_Current->REQUEST.request = request;
    executionManager_Current->REQUEST.package = package;
    if (executionManager_Current->debug) println("uploading");
    executionManager_Current->REQUEST.upload = true;
    executionManager_Current->REQUEST.uploadNotComplete = true;
    executionManager_Current->REQUEST.processingNotComplete = true;
    if (executionManager_Current->debug) println("while(executionManager_Current->REQUEST.uploadNotComplete) start");
    while(executionManager_Current->REQUEST.uploadNotComplete);
    if (executionManager_Current->debug) println("while(executionManager_Current->REQUEST.uploadNotComplete) done");
    if (executionManager_Current->debug) println("while(executionManager_Current->REQUEST.processingNotComplete) start");
    while(executionManager_Current->REQUEST.processingNotComplete);
    if (executionManager_Current->debug) println("while(executionManager_Current->REQUEST.processingNotComplete) done");
    if (executionManager_Current->debug) println("upload complete");
    if (executionManager_Current->debug) println("executionManager_Current->REQUEST.reply = %p", executionManager_Current->REQUEST.reply);
    return executionManager_Current->REQUEST.reply;
}

int staticHandleRequest(class ExecutionManager * arg) {
    class ExecutionManager * R = arg; //reinterpret_cast<class class ExecutionManager * >(arg);
    if (R->REQUEST.request != -1) {
        assert(R->REQUEST.package != nullptr);
        switch (R->REQUEST.request) {
            case ExecutionManager::REQUEST_ID.threadCreate:
                if (executionManager_Current->debug) println("request: threadCreate");
                if (executionManager_Current->debug) println("adding new thread");
                R->REQUEST.reply = threadNew_(reinterpret_cast<struct QTS *>(R->REQUEST.package));
                assert(R->REQUEST.reply != nullptr);
                R->REQUEST.package = nullptr;
                if (executionManager_Current->debug) println("R->REQUEST.reply = %p", R->REQUEST.reply);
                if (executionManager_Current->debug) println("added new thread");
                if (executionManager_Current->debug) println("waiting for thread stop");
                R->threadWaitUntilStopped(reinterpret_cast<Thread *>(R->REQUEST.reply));
                if (executionManager_Current->debug) println("thread has stopped");
                if (executionManager_Current->debug) println("request complete: threadCreate");
                break;
            case ExecutionManager::REQUEST_ID.threadJoin:
                if (executionManager_Current->debug) println("request: threadJoin");
                R->threadJoin(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadJoinUntilStopped:
                if (executionManager_Current->debug) println("request: threadJoinUntilStopped");
                R->threadJoinUntilStopped(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadJoinUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilStopped:
                if (executionManager_Current->debug) println("request: threadWaitUntilStopped");
                R->threadWaitUntilStopped(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadWaitUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunning:
                if (executionManager_Current->debug) println("request: threadWaitUntilRunning");
                R->threadWaitUntilRunning(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadWaitUntilRunning");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoin:
                if (executionManager_Current->debug) println("request: threadWaitUntilRunningAndJoin");
                R->threadWaitUntilRunningAndJoin(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadWaitUntilRunningAndJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoinUntilStopped:
                if (executionManager_Current->debug) println("request: threadWaitUntilRunningAndJoinUntilStopped");
                R->threadWaitUntilRunningAndJoinUntilStopped(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadWaitUntilRunningAndJoinUntilStopped");
                break;
            case ExecutionManager::REQUEST_ID.threadPause:
                if (executionManager_Current->debug) println("request: threadPause");
                R->threadPause(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadPause");
                break;
            case ExecutionManager::REQUEST_ID.threadResume:
                if (executionManager_Current->debug) println("request: threadResume");
                R->threadResume(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadResume");
                break;
            case ExecutionManager::REQUEST_ID.threadResumeAndJoin:
                if (executionManager_Current->debug) println("request: threadResumeAndJoin");
                R->threadResumeAndJoin(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadResumeAndJoin");
                break;
            case ExecutionManager::REQUEST_ID.threadTerminate:
                if (executionManager_Current->debug) println("request: threadTerminate");
                R->threadTerminate(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: threadTerminate");
                break;
            case ExecutionManager::REQUEST_ID.threadKill:
                if (executionManager_Current->debug) println("request: threadKill");
                R->threadKill(reinterpret_cast<Thread *>(R->REQUEST.package));
                if (executionManager_Current->debug) println("request complete: tthreadKill");
                break;
            default:
                if (executionManager_Current->debug) println("unknown request");
                abort();
                break;
        }
    }
    R->REQUEST.request = -1;
    R->REQUEST.stack.free();
    if (executionManager_Current->debug) println("R->REQUEST.processing = false);");
    R->REQUEST.processing = false;
    if (executionManager_Current->debug) println("R->REQUEST.processingNotComplete = false);");
    R->REQUEST.processingNotComplete = false;
    if (executionManager_Current->debug) println("return 0");
    return 0;
}

void ExecutionManager::handleRequest(class ExecutionManager * EX) {
    if (executionManager_Current->debug) println("REQUEST.processing = true);");
    REQUEST.processing = true;
    if (executionManager_Current->debug) println("REQUEST.upload = false);");
    REQUEST.upload = false;
    if (executionManager_Current->debug) println("REQUEST.uploadNotComplete = false);");
    REQUEST.uploadNotComplete = false;
    staticHandleRequest(EX);
    if (executionManager_Current->debug) println("return");
}

bool ExecutionManager::errorChecking(struct QTS * q) {
    bool r = false;
    if (q->t == nullptr) r = true; // skip non existant threads
    if (q->t->died) r = true; // skip died threads
    return r;
}

int ExecutionManager_instance(void * arg) {
    // without class, we get error: ‘ExecutionManager’ does not name a type
    class ExecutionManager * ExecutionManager = reinterpret_cast<class ExecutionManager * >(arg);

    if (ExecutionManager->debug) println("starting Execution Manager");

    pid_t self = ExecutionManager->thread->pid;
    assert(self == ExecutionManager->thread->pid);
    assert(ExecutionManager->QTS_VECTOR.size() != 0);
    assert(ExecutionManager->QTS_VECTOR[0] != nullptr);
    assert(self == ExecutionManager->QTS_VECTOR[0]->t->pid);
    if (ExecutionManager->debug) println("EXECUTION MANAGER!");
    bool e = false;
    while(true) {
        if (e) break;
        int i = 0;
        while(i < ExecutionManager->QTS_VECTOR.size()) {
            if (ExecutionManager->close) {
                if (i == 0) {
                    if (ExecutionManager->QTS_VECTOR.size() == 1) {
                        e = true;
                        break;
                    }
                }
                struct QTS * q = ExecutionManager->QTS_VECTOR[i];
                println("terminating thread %d", q->t->pid);
                threadTerminate(q->t);
                continue;
            };
            if (ExecutionManager->REQUEST.upload && !ExecutionManager->REQUEST.processing) {
                if (ExecutionManager->debug) println("if (ExecutionManager->REQUEST.upload && !ExecutionManager->REQUEST.processing)");
                if (executionManager_Current->debug) println("handling request");
                ExecutionManager->handleRequest(ExecutionManager);
                if (executionManager_Current->debug) println("handled request");
            }
            if (ExecutionManager->debug) println("ALIVE");

            // skip self
            if (i == 0) { i++; continue; }

            // obtain a thread
            struct QTS * q = ExecutionManager->QTS_VECTOR[i];
//            threadInfo(q->t);
            // error checking
//            if (ExecutionManager->debug) println("error checking");
            if (ExecutionManager->errorChecking(q)) { println("error occured, skipping"); i++; continue; }

            assert(q->t->pid != self);

            // check if thread is running
//            if (ExecutionManager->debug) println("threadIsRunning(q->t)");
            if (ExecutionManager->threadIsRunning(q->t)) {
//                if (ExecutionManager->debug) println("threadIsRunning(q->t) true");
//                if (ExecutionManager->debug) println("THREAD %d running", q->t->pid);
                // cleanup thread if needed
                if (q->t->killable) {
                    if (ExecutionManager->debug) println("THREAD %d is killable", q->t->pid);
                    q->t->killable = false;
                    if (ExecutionManager->debug) println("THREAD %d threadWaitUntilStopped", q->t->pid);
                    ExecutionManager->threadWaitUntilStopped(q->t);
                    if (ExecutionManager->debug) println("THREAD %d stopped", q->t->pid);
                    ExecutionManager->threadResumeAndJoin(q->t);
                    if (ExecutionManager->debug) println("THREAD %d has DIED: return code: %d", q->t->pid, q->t->returnCode);
                    q->t->died = true;
                } else {
//                    if (ExecutionManager->debug) println("THREAD %d is not killable", q->t->pid);
                }
//                if (ExecutionManager->debug) println("q->t->reapable");
                if (q->t->reapable) {
                    if (ExecutionManager->debug) println("q->t->reapable true");
                    if (ExecutionManager->debug) println("reaping pid %d", q->t->pid);
                    if (ExecutionManager->debug) println("erasing ExecutionManager->QTS_VECTOR[%d]", i);
                    if (ExecutionManager->debug) println("ExecutionManager->QTS_VECTOR.size() is %d",
                           ExecutionManager->QTS_VECTOR.size());
                    ExecutionManager->QTS_VECTOR.erase(ExecutionManager->QTS_VECTOR.begin() + i);
                    if (ExecutionManager->debug) println("ExecutionManager->QTS_VECTOR.size() is %d",
                           ExecutionManager->QTS_VECTOR.size());
                    q->t->reaped = true;
                } else {
//                    if (ExecutionManager->debug) println("q->t->reapable false");
                }
            } else {
//                if (ExecutionManager->debug) println("threadIsRunning(q->t) false");
            }
            i++;
        }
    }
    if (ExecutionManager->debug) println("exiting Execution Manager");
    ExecutionManager->QTS_VECTOR.clear();
    executionManager_Current = nullptr;
    return 0;
}

Thread * ExecutionManager::threadNew(bool createSuspended, size_t stack_size, std::function<int(void*)> f, void * arg) {
    struct QTS * qts = new struct QTS;
    qts->createSuspended = createSuspended;
    qts->stack_size = stack_size;
    qts->f = f;
    qts->arg = arg;
    qts->push = true;
    if (executionManager_Current->debug) println("Thread * x = static_cast<Thread*>(sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts)); starting");
    Thread * x = reinterpret_cast<Thread *>(sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts));
    if (executionManager_Current->debug) println("Thread * x = static_cast<Thread*>(sendRequest(ExecutionManager::REQUEST_ID.threadCreate, qts)); complete");
    if (executionManager_Current->debug) println("x = %p", x);
    assert(x != nullptr);
    if (executionManager_Current->debug) println("createSuspended: %s", boolToString(createSuspended));
    if (!createSuspended) {
        if (executionManager_Current->debug) println("resuming from threadNew");
        sendRequest(ExecutionManager::REQUEST_ID.threadResume, x);
        if (executionManager_Current->debug) println("resumed from threadNew");
    }
    return x;
}

Thread * ExecutionManager::threadNew(bool createSuspended, size_t stack_size, int(*f) (void*), void * arg) {
    return threadNew(createSuspended, stack_size, f, arg);
}

void ExecutionManager::threadJoin(Thread * t) {
    if (executionManager_Current->debug) println("joining");
    if (t->pid != -1) {
        siginfo_t info;
        for (;;) {
            if (waitid(P_PID, t->pid, &info, WEXITED | WSTOPPED) == -1) {
                if (executionManager_Current->debug) perror("waitid");
                return;
            }
            bool con = false;
            switch (info.si_code) {
                case CLD_EXITED:
                    t->status = t->statusList.EXITED;
                    t->returnCode = info.si_status;
                    t->reapable = true;
                    break;
                case CLD_KILLED:
                    t->status = t->statusList.KILLED_BY_SIGNAL;
                    t->signal = info.si_status;
                    t->reapable = true;
                    break;
                case CLD_DUMPED:
                    t->status = t->statusList.KILLED_BY_SIGNAL;
                    t->signal = info.si_status;
                    t->core_dumped = true;
                    t->reapable = true;
                    break;
                case CLD_STOPPED:
                    con = true;
                    break;
                default:
                    t->status = t->statusList.UNDEFINED;
                    break;
            }
            if (!con) break;
        }
    }
}

void ExecutionManager::threadJoinUntilStopped(Thread * t) {
    if (t->pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WEXITED|WSTOPPED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_EXITED:
                t->status = t->statusList.EXITED;
                t->returnCode = info.si_status;
                t->reapable = true;
                break;
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->reapable = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->reapable = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilStopped(Thread * t) {
    if (t->pid != -1) {
        siginfo_t info;
        if (executionManager_Current->debug) println("waiting for pid %d", t->pid);
        if (waitid(P_PID, t->pid, &info, WSTOPPED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        if (executionManager_Current->debug) println("waited for pid %d", t->pid);
        switch (info.si_code) {
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->reapable = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->reapable = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunning(Thread * t) {
    if (t->pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t->status = t->statusList.RUNNING;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunningAndJoin(Thread * t) {
    if (t->pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t->status = t->statusList.RUNNING;
                for (;;) {
                    if (waitid(P_PID, t->pid, &info, WEXITED | WSTOPPED) == -1) {
                        if (executionManager_Current->debug) perror("waitid");
                        return;
                    }
                    bool con = false;
                    switch (info.si_code) {
                        case CLD_EXITED:
                            t->status = t->statusList.EXITED;
                            t->returnCode = info.si_status;
                            t->reapable = true;
                            break;
                        case CLD_KILLED:
                            t->status = t->statusList.KILLED_BY_SIGNAL;
                            t->signal = info.si_status;
                            t->reapable = true;
                            break;
                        case CLD_DUMPED:
                            t->status = t->statusList.KILLED_BY_SIGNAL;
                            t->signal = info.si_status;
                            t->core_dumped = true;
                            t->reapable = true;
                            break;
                        case CLD_STOPPED:
                            con = true;
                            break;
                        default:
                            t->status = t->statusList.UNDEFINED;
                            break;
                    }
                    if (!con) break;
                }
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadWaitUntilRunningAndJoinUntilStopped(Thread * t) {
    if (t->pid != -1) {
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t->status = t->statusList.RUNNING;
                if (waitid(P_PID, t->pid, &info, WEXITED|WSTOPPED) == -1) {
                    if (executionManager_Current->debug) perror("waitid");
                    return;
                }
                switch (info.si_code) {
                    case CLD_EXITED:
                        t->status = t->statusList.EXITED;
                        t->returnCode = info.si_status;
                        t->reapable = true;
                        break;
                    case CLD_KILLED:
                        t->status = t->statusList.KILLED_BY_SIGNAL;
                        t->signal = info.si_status;
                        t->reapable = true;
                        break;
                    case CLD_DUMPED:
                        t->status = t->statusList.KILLED_BY_SIGNAL;
                        t->signal = info.si_status;
                        t->core_dumped = true;
                        t->reapable = true;
                        break;
                    case CLD_STOPPED:
                        t->status = t->statusList.STOPPED;
                        break;
                    default:
                        t->status = t->statusList.UNDEFINED;
                        break;
                }
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

bool ExecutionManager::threadIsStopped(Thread * t) {
    return t->status == t->statusList.STOPPED;
}

bool ExecutionManager::threadIsRunning(Thread * t) {
    return t->status == t->statusList.RUNNING;
}

void ExecutionManager::threadPause(Thread * t) {
    if (t->pid != -1) {
        if (kill(t->pid, SIGSTOP) == -1) if (executionManager_Current->debug) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WSTOPPED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->reapable = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->reapable = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadResume(Thread * t) {
    if (t->pid != -1) {
        if (kill(t->pid, SIGCONT) == -1) if (executionManager_Current->debug) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t->status = t->statusList.RUNNING;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadResumeAndJoin(Thread * t) {
    if (t->pid != -1) {
        if (kill(t->pid, SIGCONT) == -1) if (executionManager_Current->debug) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WCONTINUED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_CONTINUED:
                t->status = t->statusList.RUNNING;
                for (;;) {
                    if (waitid(P_PID, t->pid, &info, WEXITED | WSTOPPED) == -1) {
                        if (executionManager_Current->debug) perror("waitid");
                        return;
                    }
                    bool con = false;
                    switch (info.si_code) {
                        case CLD_EXITED:
                            t->status = t->statusList.EXITED;
                            t->returnCode = info.si_status;
                            t->reapable = true;
                            break;
                        case CLD_KILLED:
                            t->status = t->statusList.KILLED_BY_SIGNAL;
                            t->signal = info.si_status;
                            t->reapable = true;
                            break;
                        case CLD_DUMPED:
                            t->status = t->statusList.KILLED_BY_SIGNAL;
                            t->signal = info.si_status;
                            t->core_dumped = true;
                            t->reapable = true;
                            break;
                        case CLD_STOPPED:
                            con = true;
                            break;
                        default:
                            t->status = t->statusList.UNDEFINED;
                            break;
                    }
                    if (!con) break;
                }
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadTerminate(Thread * t) {
    if (t->pid != -1) {
        if (kill(t->pid, SIGTERM) == -1) if (executionManager_Current->debug) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WSTOPPED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->terminated_by_user = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->terminated_by_user = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

void ExecutionManager::threadKill(Thread * t) {
    if (t->pid != -1) {
        if (kill(t->pid, SIGKILL) == -1) if (executionManager_Current->debug) perror("kill");
        siginfo_t info;
        if (waitid(P_PID, t->pid, &info, WSTOPPED) == -1) {
            if (executionManager_Current->debug) perror("waitid");
            return;
        }
        switch (info.si_code) {
            case CLD_KILLED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->killed_by_user = true;
                break;
            case CLD_DUMPED:
                t->status = t->statusList.KILLED_BY_SIGNAL;
                t->signal = info.si_status;
                t->core_dumped = true;
                t->killed_by_user = true;
                break;
            case CLD_STOPPED:
                t->status = t->statusList.STOPPED;
                break;
            default:
                t->status = t->statusList.UNDEFINED;
                break;
        }
    }
}

int helperVoid(void * v) {
    reinterpret_cast<void (*)()>(v)();
    return 0;
}

Thread * threadNew(bool createSuspended, size_t stack_size, std::function<int(void*)> f, void * arg) {
    if (executionManager_Current != nullptr) QINIT();
    else {
        println("error: an execution manager must be set, use setExecutionManager(YourExecutionManager); to set one");
        abort();
    }
    return executionManager_Current->threadNew(createSuspended, stack_size, f, arg);
}

Thread * threadNew(bool createSuspended, size_t stack_size, int(*f) (void*), void * arg) {
    return threadNew(createSuspended, stack_size, f, arg);
}

Thread * threadNew(std::function<int(void*)> f, void * arg) {
    return threadNew(false, 0, f, arg);
}

Thread * threadNew(size_t stack_size, std::function<int(void*)> f, void * arg) {
    return threadNew(false, stack_size, f, arg);
}

Thread * threadNew(size_t stack_size, void (*f)()) {
    return threadNew(stack_size, helperVoid, reinterpret_cast<void*>(f));
}

Thread * threadNew(void (*f)()) {
    return threadNew(0, helperVoid, reinterpret_cast<void*>(f));
}

Thread * threadNewSuspended(std::function<int(void*)> f, void * arg) {
    return threadNew(false, 0, f, arg);
}

Thread * threadNewSuspended(size_t stack_size, std::function<int(void*)> f, void * arg) {
    return threadNew(false, stack_size, f, arg);
}

Thread * threadNewSuspended(size_t stack_size, void (*f)()) {
    return threadNew(stack_size, helperVoid, reinterpret_cast<void*>(f));
}

Thread * threadNewSuspended(void (*f)()) {
    return threadNew(0, helperVoid, reinterpret_cast<void*>(f));
}

void threadJoin(Thread * t) {
    while(!t->died);
//    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadJoin, t);
}

void threadJoinUntilStopped(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadJoinUntilStopped, reinterpret_cast<void *>(t));
}

void threadWaitUntilStopped(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilStopped, reinterpret_cast<void *>(t));
}

void threadWaitUntilRunning(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunning, reinterpret_cast<void *>(t));
}

void threadWaitUntilRunningAndJoin(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoin, reinterpret_cast<void *>(t));
}

void threadWaitUntilRunningAndJoinUntilStopped(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadWaitUntilRunningAndJoinUntilStopped, reinterpret_cast<void *>(t));
}

bool threadIsStopped(Thread * t) {
    return ExecutionManager::threadIsStopped(t);
}

bool threadIsRunning(Thread * t) {
    return ExecutionManager::threadIsRunning(t);
}

void threadPause(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadPause, reinterpret_cast<void *>(t));
}

void threadResume(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadResume, reinterpret_cast<void *>(t));
}

void threadResumeAndJoin(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadResumeAndJoin, reinterpret_cast<void *>(t));
}

void threadTerminate(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadTerminate, reinterpret_cast<void *>(t));
}

void threadKill(Thread * t) {
    ExecutionManager::sendRequest(ExecutionManager::REQUEST_ID.threadKill, reinterpret_cast<void *>(t));
}

void threadInfo(Thread * t) {
    println("THREAD INFO:");
    bool threadExists = t != nullptr;
    println("    thread exists = %s", boolToString(threadExists));
    if (threadExists) {
        println("    STACK INFO:");
        bool stackExists = t->stack.stack != nullptr;
        println("        stack exists = %s", boolToString(stackExists));
        if (stackExists) {
            println("        address = %p", t->stack.stack);
            println("        top = %p", t->stack.top);
            println("        size = %ld", t->stack.size);
            println("        direction = %s", t->stack.getStackDirectionAsString());
        }
        println("    pid = %d", t->pid);
        println("    killable = %s", boolToString(t->killable));
        println("    initially suspended = %s", boolToString(t->suspend));
        println("    dead = %s", boolToString(t->died));
        std::string stat = nullptr;
        switch (t->status) {
            case t->statusList.UNDEFINED:
                stat = "UNDEFINED";
                break;
            case t->statusList.RUNNING:
                stat = "running";
                break;
            case t->statusList.STOPPED:
                stat = "stopped";
                break;
            case t->statusList.DEAD:
                stat = "dead";
                break;
            case t->statusList.EXITED:
                stat = "exited";
                break;
            case t->statusList.KILLED_BY_SIGNAL:
                stat = "killed by signal";
                break;
            default:
                stat = "unknown";
                break;
        }
        println("    status = %s", stat);
        println("    signal = %d", t->signal);
        println("    returnCode = %d", t->returnCode);
        println("    core dumped = %s", boolToString(t->core_dumped));
        println("    reapable = %s", boolToString(t->reapable));
        println("    reaped = %s", boolToString(t->reaped));
        println("    terminated by user = %s", boolToString(t->terminated_by_user));
        println("    killed by user = %s", boolToString(t->killed_by_user));
    }
}
