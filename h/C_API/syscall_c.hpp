/*
    C API wrapper for system calls
*/

#ifndef _syscall_c_hpp_
#define _syscall_c_hpp_

#include "../../lib/hw.h"

#define STACK_CONTEXT_EXTENSION 4096

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

        // Start a thread with start_routine(arg), returns a handle to the created thread in thread_t* handle,
        // returns negative value if it fails
        int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg);

        // Terminate current thread, returns negative value if it fails
        int thread_exit();

        // Let other threads know that they can take over the processor,
        // does not necesseraly stop the current thread
        void thread_dispatch();

        // Block current thread until thread_t handle finishes
        void thread_join(thread_t handle);

        class SCB;
        typedef SCB* sem_t;

        // Creates a semaphore with starting value init, returns a handle to the created semaphore in sem_t* handle,
        // returns negative value if it fails
        int sem_open(sem_t* handle, unsigned init);

        // Destroys the semaphore given by sem_t handle
        // Returns 0 if successful, negative value if it fails
        int sem_close(sem_t handle);

        // Wait for the semaphore given by sem_t id
        // Returns 0 if successful, negative value if it fails
        int sem_wait(sem_t id);

        // Siganl the semaphore given by sem_t id
        // Returns 0 if successful, negative value if it fails
        int sem_signal(sem_t id);

        typedef unsigned long time_t;

        int time_sleep (time_t time);

        char getc ();

        void putc (char output);

    #ifdef __cplusplus
    } // extern "C"
    #endif // __cplusplus

#endif // _syscall_c_hpp_