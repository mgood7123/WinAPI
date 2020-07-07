//
// Created by konek on 7/22/2019.
//

#include <Windows/Locker.h>

Lock::Lock() {
    this->LOCK = false;
}

void Lock::lock() {
    if (this->LOCK) while (this->LOCK) {}
    else this->LOCK = true;
}
void Lock::unlock() {
    this->LOCK = false;
}
