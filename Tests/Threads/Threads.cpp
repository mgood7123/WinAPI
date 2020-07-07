//
// Created by brothercomplex on 24/10/19.
//

#include <Windows/windows.h>

int main() {
    auto proc = "./libThreadsWindowsApplication.so";
    Kernel K;
    PROCESS_INFORMATION processInfo;
    puts((std::string("opening ") + proc).c_str());
    if (!CreateProcessA(K, proc, nullptr, nullptr, nullptr, false, CREATE_SUSPENDED, nullptr, nullptr, nullptr, &processInfo)) {
        printf("%s failed to created windows process\n", proc);
    } else {
        printf("%s successfully created windows process\n", proc);
    }
    return 0;
}