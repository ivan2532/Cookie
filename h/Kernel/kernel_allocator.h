/*
    Declarations of internal kernel functions
*/

#ifndef _kernel_allocator_h_
#define _kernel_allocator_h_

#include "../../lib/hw.h"

#ifdef __cplusplus
extern "C" {
#endif

// Allocate a memory block of "size" bytes on the heap.
void* kernel_alloc(size_t size);

// Free memory allocated by kernel_alloc
int kernel_free(void* ptr);

#ifdef __cplusplus
}
#endif

#endif //_kernel_allocator_h_
