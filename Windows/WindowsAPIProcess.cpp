//
// Created by konek on 7/17/2019.
//

#include <dlfcn.h>
#include <Windows/WindowsAPIProcess.h>
#include <functionPointer.h>
#include <Windows/WindowsAPIThread.h>

WINBASEAPI
BOOL
WINAPI
CreateProcessA(
    _In_ Kernel & KERNEL,
    _In_opt_ LPCSTR lpApplicationName,
    _Inout_opt_ LPSTR lpCommandLine,
    _In_opt_ LPSECURITY_ATTRIBUTES lpProcessAttributes,
    _In_opt_ LPSECURITY_ATTRIBUTES lpThreadAttributes,
    _In_ BOOL bInheritHandles,
    _In_ DWORD dwCreationFlags,
    _In_opt_ LPVOID lpEnvironment,
    _In_opt_ LPCSTR lpCurrentDirectory,
    _In_ LPSTARTUPINFOA lpStartupInfo,
    _Out_ LPPROCESS_INFORMATION lpProcessInformation
) {
    char * dlerrormsg = dlerror();
    if (dlerrormsg != nullptr) {
        printf("WinAPI: internal error: existing error: %s\n", dlerrormsg);
        return 0;
    }
    PVOID APP = dlopen(lpApplicationName, RTLD_NOW);
    dlerrormsg = dlerror();
    if (dlerrormsg != nullptr) {
        printf("WinAPI: internal error: %s could not be opened: %s\n", lpApplicationName, dlerrormsg);
        return 0;
    }
    if (APP == nullptr) return 0;
    PVOID WM = dlsym(APP, "WinMain");
    dlerrormsg = dlerror();
    if (dlerrormsg != nullptr) {
        printf("WinAPI: internal error: could not find WinMain in %s: %s\n", lpApplicationName, dlerrormsg);
        return 0;
    }
    if (WM == nullptr) {
        printf("WinAPI: internal error: address of WinMain is a null pointer\n");
        return 0;
    }
    size_t * INDEX = new size_t;
    *INDEX = KERNEL.table->findObject(KERNEL.newObject(ObjectTypeProcess, 0, new PROCESS_MAIN_STRUCT));
    functionPointerAssign4(DWORD, static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->MAIN, WM, HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine,int nCmdShow);
    PVOID WP = dlsym(APP, "WndProc");
    dlerrormsg = dlerror();
    static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->WNDPROC = nullptr;
    if(WM == nullptr)
        printf("WinAPI: internal warning: address of WndProc is a null pointer\n");
    else if (dlerrormsg != nullptr)
        printf("WinAPI: internal warning: address of WndProc is not a null pointer however an error was produced: %s\n", dlerrormsg);
    else functionPointerAssign4(LRESULT, static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->WNDPROC, WP, HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);
    static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation = new PROCESS_INFORMATION;
    static_cast<PROCESS_INFORMATION *>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation)->hProcess = KERNEL.table->table[*INDEX]->resource;
    static_cast<PROCESS_INFORMATION *>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation)->dwProcessId = KERNEL.PID_LAST += 4;
    static_cast<PROCESS_INFORMATION *>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation)->hThread = nullptr;
    static_cast<PROCESS_INFORMATION *>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation)->dwThreadId = 0;
    static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->hThread = CreateThread(KERNEL, nullptr, 0, PROCESS_MAIN, INDEX, dwCreationFlags|CREATE_SUSPENDED,
                                                      &static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->dwThreadId);
    Thread * t = static_cast<THREAD_INFO*>(static_cast<Handle*>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->hThread)->object->resource)->thread;
    static_cast<PROCESS_INFORMATION *>(static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation)->dwThreadId = t->pid;
    if (lpProcessInformation != nullptr) {
        lpProcessInformation->hProcess = static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->hProcess;
        lpProcessInformation->dwProcessId = static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->dwProcessId;
        lpProcessInformation->hThread = static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->hThread;
        lpProcessInformation->dwThreadId = static_cast<PROCESS_MAIN_STRUCT*>(KERNEL.table->table[*INDEX]->resource)->lpProcessInformation->dwThreadId;
        int R = 0;
        puts("PROCESS: RESUMING THREAD IN PROCESS");
        threadResumeAndJoin(t);
        printf("WINAPI: R = %d\n", t->returnCode);
    }
    return 1;
}

// should we move this into KERNEL?
DWORD PROCESS_MAIN(
    LPVOID lpParameter
) {
    // lpParameter is a index into the KERNEL's internal table
    struct X {
        Kernel * kernel;
        LPVOID kernel_index;
    } * x = static_cast<X *>(lpParameter);
    Kernel * KERNEL = x->kernel;
    size_t KERNEL_INDEX = *static_cast<size_t *>(x->kernel_index);
    assert(KERNEL_INDEX == 0);
    PROCESS_MAIN_STRUCT * app = static_cast<PROCESS_MAIN_STRUCT*>(KERNEL->table->table[KERNEL_INDEX]->resource);
    assert(app->lpProcessInformation->dwProcessId == KERNEL->PID_LAST);

    DWORD ret = app->MAIN(app->lpProcessInformation->hProcess, nullptr, nullptr, 0);

    // KERNEL.table->table[KERNEL_INDEX]->resource and KERNEL.table->table[KERNEL_INDEX]->resource->lpProcessInformation are both allocated, delete them
    delete static_cast<PROCESS_MAIN_STRUCT *>(KERNEL->table->table[KERNEL_INDEX]->resource)->lpProcessInformation;
    static_cast<PROCESS_MAIN_STRUCT *>(KERNEL->table->table[KERNEL_INDEX]->resource)->lpProcessInformation = nullptr;
    delete static_cast<PROCESS_MAIN_STRUCT *>(KERNEL->table->table[KERNEL_INDEX]->resource);
    KERNEL->table->table[KERNEL_INDEX]->resource = nullptr;
    // if we know the index we can remove the Object directly
    KERNEL->table->DELETE(KERNEL_INDEX);
    return ret;
}
