//
// Created by konek on 7/19/2019.
//

#include <Windows/Kernel/WindowsAPIKernel.h>

// As each application finishes using the event, it closes its handle to the object.
// When there are no remaining open handles to the event object, the system destroys the event object.
// In contrast, an application can obtain a handle to an existing window object.
// When the window object is no longer needed, the application must destroy the object,
// which invalidates the window handle.

/* summary:
1. When there are no remaining open handles to the object, the system destroys the object.
2a. When the object is no longer needed, the application must the object, which invalidates the handle.
2b. a thread could create an event object and wait on the event handle.
While the thread is waiting, another thread could close the same event object handle.
The event object remains in memory, without any event object handles,
    until the event object is set to the signaled state and the wait operation is completed.
At this time, the system removes the object from memory.
 */
WINBASEAPI
BOOL
WINAPI
CloseHandle(
    _In_ Kernel &KERNEL,
    _In_ _Post_ptr_invalid_ HANDLE hObject
) {
    if (!KERNEL.validateHandle(hObject)) return 0;
    Handle *h = KERNEL.getHandle(hObject);
    if (h->object->handles == 1) KERNEL.deleteObject(h->object);
    else h->object->handles--;
    h->object = nullptr;
    h->invalidated = true;
    return 1;
}

WINBASEAPI
BOOL
WINAPI
DuplicateHandle(
    _In_ HANDLE hSourceProcessHandle,
    _In_ HANDLE hSourceHandle,
    _In_ HANDLE hTargetProcessHandle,
    _Outptr_ LPHANDLE lpTargetHandle,
    _In_ DWORD dwDesiredAccess,
    _In_ BOOL bInheritHandle,
    _In_ DWORD dwOptions
) {
    // TODO: need to get threads and processes working in order to implement this function
    return 0;
}

WINBASEAPI
BOOL
WINAPI
CompareObjectHandles(
    _In_ Kernel &KERNEL,
    _In_ HANDLE hFirstObjectHandle,
    _In_ HANDLE hSecondObjectHandle
) {
    // we cannot use validHandle() here because two HANDLE's may differ even when invalid
    if (hFirstObjectHandle != nullptr && hSecondObjectHandle != nullptr) {
        Handle *h1 = KERNEL.getHandle(hFirstObjectHandle);
        Handle *h2 = KERNEL.getHandle(hSecondObjectHandle);

        // it is unspecified what happens when two invalidated HANDLE's are compared

        if (h1->invalidated != h2->invalidated) return 0;
        if (h1->object != nullptr && h2->object != nullptr) {
            if (!Object::compare(*h1->object, *h2->object)) return 0;
        }
        return 1;
    } else if (hFirstObjectHandle == nullptr && hSecondObjectHandle == nullptr) return 1;
    else return 0;
}

WINBASEAPI
BOOL
WINAPI
GetHandleInformation(
    _In_ Kernel &KERNEL,
    _In_ HANDLE hObject,
    _Out_ LPDWORD lpdwFlags
) {
    if (KERNEL.validateHandle(hObject)) {
        // TODO
    }
    return 0;
}


WINBASEAPI
BOOL
WINAPI
SetHandleInformation(
    _In_ Kernel &KERNEL,
    _In_ HANDLE hObject,
    _In_ DWORD dwMask,
    _In_ DWORD dwFlags
) {
    if (KERNEL.validateHandle(hObject)) {
        // TODO
    }
    return 0;
}

Handle::Handle() {
    this->invalidated = true;
    this->object = nullptr;
}

Handle::~Handle() {
    this->invalidated = true;
    this->object = nullptr;
}

bool Kernel::validateHandle(HANDLE hObject) {
    if (hObject == nullptr) return 0;
    Handle *h = this->getHandle(hObject);
    if (h->object == nullptr) return 0;
    return !h->invalidated;
}

HANDLE Kernel::newHandle(ObjectType type) {
    return this->newHandle(type, nullptr);
}

HANDLE Kernel::newHandle(ObjectType type, PVOID resource) {
    Handle *hObject = new Handle();
    hObject->object = this->newObject(type, 0, resource);
    assert(hObject->object != nullptr);
    hObject->invalidated = false;
    hObject->object->handles++;
    assert(this->validateHandle(hObject));
    return hObject;
}

Handle *Kernel::getHandle(HANDLE handle) {
    return static_cast<Handle *>(handle);
}
