/*
    Declarations of internal kernel functions
*/

#ifndef kernel_syscall
#define kernel_syscall

#include "../../lib/hw.h"

// Allocate a memory block of "size" bytes on the heap.
void* kernel_alloc(size_t size);

// Free memory allocated by kernel_alloc
int kernel_free(void* ptr);

#endif //kernel_syscall
