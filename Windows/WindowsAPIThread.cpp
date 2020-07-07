//
// Created by konek on 7/17/2019.
//

#include <Windows/WindowsAPIThread.h>
#include <unistd.h>
#include <string.h>

class threadpool WINDOWSAPITHREADPOOL {};

VOID * OBJ;
void threadpool::add(_In_ Kernel &KERNEL, DWORD dwCreationFlags, size_t stack_size_, LPTHREAD_START_ROUTINE __start_routine, LPVOID args, THREAD_INFO * threadinfo) {
    std::lock_guard<std::mutex> lock(this->lock);
    assert(*static_cast<size_t *>(args) == 0);
    OBJ = threadinfo;
    threadinfo->suspend_count = 0;

    threadinfo->stack_size = this->default_stack_size;
    if ((dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION) && (stack_size_ != 0)) {
        if (stack_size_ > this->default_stack_size) threadinfo->stack_size = roundUp<size_t>(stack_size_, 1_megabyte);
        else threadinfo->stack_size = stack_size_;
    }

    int s = 0;

    threadinfo->MAIN_STRUCT.__start_routine = __start_routine;
    struct XX {
        Kernel * kernel;
        LPVOID kernel_index;
    } * xx = new struct XX;
    xx->kernel = &KERNEL;
    xx->kernel_index = args; // INDEX TO KERNEL.table->table[INDEX]->resource

    threadinfo->MAIN_STRUCT.args = xx;
    struct X {
        Kernel * kernel;
        size_t threadpool_index;
        size_t threadpool_index_;
        size_t threadpool_index__;
    } * x = new struct X;
    x->kernel = &KERNEL;
    x->threadpool_index = this->threadKernel->table->findObject(this->threadKernel->newObject(ObjectTypeThread, 0, threadinfo));
    x->threadpool_index_ = this->threadKernel->table->findObject(this->threadKernel->newObject(ObjectTypeThread, 0, x));
    threadinfo->thread = threadNew(MASK_CONTAINS_VALUE(dwCreationFlags, CREATE_SUSPENDED), threadinfo->stack_size, PTHREAD_MAIN, x);
    if (threadinfo->thread->pid == -1) handle_error_en(s, "threadNew");
    printf("WINAPI: started thread with id %zu\n", threadinfo->thread->pid);
    if (MASK_CONTAINS_VALUE(dwCreationFlags, CREATE_SUSPENDED)) { threadinfo->suspend_count++; }
}

BOOL threadpool::contains(Thread * t) {
    std::lock_guard<std::mutex> lock(this->lock);
    for (int page = 0; page < this->threadKernel->table->Page.count(); page++)
        for (size_t index = this->threadKernel->table->page_size*page; index < this->threadKernel->table->page_size*(page*2); index++)
            if (this->threadKernel->table->table[index] != nullptr)
                if (static_cast<THREAD_INFO*>(this->threadKernel->table->table[index]->resource)->thread->pid == t->pid) return 1;
    return 0;
}

THREAD_INFO * threadpool::find(Thread * t) {
    std::lock_guard<std::mutex> lock(this->lock);
    for (int page = 0; page < this->threadKernel->table->Page.count(); page++)
        for (size_t index = this->threadKernel->table->page_size*page; index < this->threadKernel->table->page_size*(page*2); index++)
            if (this->threadKernel->table->table[index] != nullptr) {
                THREAD_INFO *thread = static_cast<THREAD_INFO *>(this->threadKernel->table->table[index]->resource);
                if (thread->thread->pid == t->pid) return thread;
            }
    return nullptr;
}

threadpool::threadpool() {
    this->threadKernel = new Kernel();
}

threadpool::~threadpool() {
    delete this->threadKernel;
}

HANDLE CreateThread(
    _In_ Kernel &KERNEL,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    SIZE_T dwStackSize,
    LPTHREAD_START_ROUTINE lpStartAddress,
    LPVOID lpParameter,
    DWORD dwCreationFlags,
    LPDWORD lpThreadId) {
    // check for unknown flag
    if (!((dwCreationFlags & CREATE_SUSPENDED) || (dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION) || (dwCreationFlags == 0))) return NULL;
    THREAD_INFO * thread = new THREAD_INFO;
    WINDOWSAPITHREADPOOL.add(
            KERNEL,
            dwCreationFlags,
            dwCreationFlags & STACK_SIZE_PARAM_IS_A_RESERVATION ? dwStackSize : 0,
            lpStartAddress,
            lpParameter,
            thread
    );
    HANDLE handle = KERNEL.newHandle(ObjectTypeThread, thread);
    DWORD id = static_cast<DWORD>(thread->thread->pid);
    if (lpThreadId != NULL) lpThreadId = &id;
    return handle;
}

int PTHREAD_MAIN(PVOID param) {
    struct X {
        Kernel * kernel;
        size_t threadpool_index;
        size_t threadpool_index_;
    } * x = static_cast<X *>(param);
    Object * THREADPOOL_OBJECTA = WINDOWSAPITHREADPOOL.threadKernel->table->table[x->threadpool_index];
    Object * THREADPOOL_OBJECTB = WINDOWSAPITHREADPOOL.threadKernel->table->table[x->threadpool_index];
    THREAD_INFO * TF = static_cast<THREAD_INFO*>(THREADPOOL_OBJECTA->resource);
    // args is an index into the kernel's internal table, it's resource points to PROCESS_MAIN_STRUCT
    // pass the KERNEL INDEX instead of the object itself to make it easier to delete
    DWORD R = static_cast<THREAD_INFO *>(THREADPOOL_OBJECTA->resource)->MAIN_STRUCT.__start_routine(TF->MAIN_STRUCT.args);
    // TF->MAIN_STRUCT.args is allocated, delete it
    delete TF->MAIN_STRUCT.args;
    TF->MAIN_STRUCT.args = nullptr;
    // THREADPOOL_OBJECT->resource is allocated, delete it
    delete THREADPOOL_OBJECTA->resource;
    THREADPOOL_OBJECTA->resource = nullptr;
    // if we know the index we can remove the Object directly
    WINDOWSAPITHREADPOOL.threadKernel->table->DELETE(x->threadpool_index);
    // delete the resource itself
    size_t i;
    memcpy(&i, &x->threadpool_index_, sizeof(size_t));
    delete THREADPOOL_OBJECTB->resource;
    THREADPOOL_OBJECTB->resource = nullptr;
    WINDOWSAPITHREADPOOL.threadKernel->table->DELETE(i);
    return static_cast<int>(R);
}

/*
 * if thread A, B, and C, are running and B is suspended, when it switches to either A or C would B get disabled in that it cannot be switched to, leaving only 2 threads running instead of 3, until thread B is then resumed
 * <Mysoft> if B is running and suspend increases the suspend count for >0 it imediatelly suspend that thread and runs the next thread
 */

DWORD SuspendThread(_In_ Kernel &KERNEL, HANDLE hThread) {
    // If the function succeeds, the return value is the thread's previous suspend count;
    // otherwise, it is (DWORD) -1. To get extended error information, use the GetLastError function.

    // it is undefined whether this suspends if the suspend count zero or greater
    // so we follow how ResumeThread behaves
    THREAD_INFO * thread = static_cast<THREAD_INFO*>(KERNEL.getHandle(hThread)->object->resource);
    DWORD psc = thread->suspend_count;
    if (thread->suspend_count == 0) {
        threadPause(thread->thread);
        printf("WINAPI: suspending thread %zu\n", thread->thread->pid);
    } else if (thread->suspend_count != MAXIMUM_SUSPEND_COUNT)
        thread->suspend_count++;
    else return (DWORD) -1;
    return psc;
}

DWORD ResumeThread(_In_ Kernel &KERNEL, HANDLE hThread) {
    THREAD_INFO * thread = static_cast<THREAD_INFO*>(KERNEL.getHandle(hThread)->object->resource);
    printf("WINAPI: resuming thread %zu\n", thread->thread->pid);
    DWORD psc = thread->suspend_count;
    if (thread->suspend_count != 0)
        thread->suspend_count--;
    else return (DWORD) -1;
    if (thread->suspend_count == 0)
        threadResume(thread->thread);
    return psc;
}

void handle_error_en(int en, const char *msg) {
    errno = en;
    perror(msg);
    exit(EXIT_FAILURE);
}

void handle_error(const char *msg) {
    perror(msg);
    exit(EXIT_FAILURE);
}
