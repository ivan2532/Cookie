/*
    C API wrapper for system calls
*/

#ifndef _syscall_c_hpp_
#define _syscall_c_hpp_

#include "../../lib/hw.h"

    #ifdef __cplusplus
    extern "C"
    {
    #endif // __cplusplus

        // Allocate a memory block of "size" bytes on the heap.
        void* mem_alloc(size_t size);

        // Free memory allocated by __mem_alloc
        int mem_free(void* ptr);

        class TCB;
        typedef TCB* thread_t;

        // Start a thread with start_routine(arg), returns a handle to the created thread on thread_t* handle,
        // and returns the resulting code
        int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);

        // Let other threads know that they can take over the processor,
        // does not necesseraly stop the current thread
        void thread_dispatch();

    #ifdef __cplusplus
    } // extern "C"
    #endif // __cplusplus

#endif // _syscall_c_hpp_