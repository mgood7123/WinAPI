#ifndef STACK_STACK
#define STACK_STACK

#include <stack/internal/direction.h>

#include <stdlib.h>

// atomic could be implemented with https://github.com/kw90/ReadWriteLock
// https://github.com/torvalds/linux/blob/master/include/linux/atomic.h

class Stack {
    public:
        char * stack = nullptr;
        char * top = nullptr ; // points to top of stack
        size_t size = 0;
        int direction = 0;
        void alloc(size_t size_);
        void free();
        class internal {
            public:
                int getStackDirection(int *addr);
        };
        int getStackDirection();
        const char * getStackDirectionAsString();

//        // Copy constructor, this gets called in the following way: Stack x; Stack y; y = x; // y(x);
//        Stack(const Stack & tmp) {
//                // copy atomic stack pointer
//                stack.store(tmp.stack.load());
//                // copy atomic top pointer
//                top.store(tmp.top.load());
//                // copy atomic size
//                size.store(tmp.size.load());
//                // copy atomic direction
//                direction.store(tmp.direction.load());
//        }
};

#endif