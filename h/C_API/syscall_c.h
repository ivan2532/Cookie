/*
    C API wrapper for system calls
*/

#ifndef syscall_c
#define syscall_c

#include "../../lib/hw.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Allocate a memory block of "size" bytes on the heap.
void* mem_alloc(size_t size);

// Free memory allocated by __mem_alloc
int mem_free(void* ptr);

#endif // syscall_c

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus