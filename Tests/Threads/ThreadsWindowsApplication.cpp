#include <string>
#include <Windows/windows.h>
#include <iostream>
bool sw = false;
CALLBACKNATIVE int WinMain() {
    std::cout << "sw before = " << sw << std::endl << "sw = !sw" << std::endl;
    sw = !sw;
    std::cout << "sw after = " << sw << std::endl;
    std::cout << "process!" << std::endl;
    return 67;
}
