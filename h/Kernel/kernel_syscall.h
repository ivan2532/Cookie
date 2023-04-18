/*
    Declarations of internal kernel functions
*/

#ifndef _kernel_syscall_h_
#define _kernel_syscall_h_

#include "../../lib/hw.h"

// Allocate a memory block of "size" bytes on the heap.
void* kernel_alloc(size_t size);

// Free memory allocated by kernel_alloc
int kernel_free(void* ptr);

#endif //_kernel_syscall_h_
