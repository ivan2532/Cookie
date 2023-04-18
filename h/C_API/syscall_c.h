/*
    C API wrapper for system calls
*/

#ifndef _syscall_c_h_
#define _syscall_c_h_

#include "../../lib/hw.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

// Allocate a memory block of "size" bytes on the heap.
void* mem_alloc(size_t size);

// Free memory allocated by __mem_alloc
int mem_free(void* ptr);

#endif // _syscall_c_h_

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus