/*
    C API wrapper to system calls

    In these functions we store the system call code and arguments in the
    corresponding registers and then switch to supervisor mode from which
    we call the internal kernel functions.

    IMPORTANT:
    Registers A4 and A5 (maybe some others as well) used for sending function
    parameters get overriden for some unknown reason.
*/

#include "../../h/C_API/syscall_c.hpp"

// Allocate a memory block of "size" bytes on the heap.
void* mem_alloc(size_t size)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, %[inSize]" : : [inSize] "r" (size));

    // Store the system call code in register A0
    __asm__ volatile ("li a0, 0x01");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    void* volatile returnValue = 0;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

// Free memory allocated by __mem_alloc
int mem_free(void* ptr)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, %[inPtr]" : : [inPtr] "r" (ptr));

    // Store the system call code in register A0
    __asm__ volatile ("li a0, 0x02");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)
{
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);

    // Store arguments starting from A1
    __asm__ volatile ("mv a1, %[inHandle]" : : [inHandle] "r" (*handle));
    __asm__ volatile ("mv a2, %[inRoutine]" : : [inRoutine] "r" (start_routine));
    __asm__ volatile ("mv a3, %[inArg]" : : [inArg] "r" (arg));
    __asm__ volatile ("mv a6, %[inStack]" : : [inStack] "r" (stack));

    // Store the system call code in register A0
    // This will invalidate the local stack variable, because mem_alloc returns value in A0
    __asm__ volatile ("li a0, 0x11");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    thread_t volatile returnHandle;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));
    __asm__ volatile ("mv %[outHandle], a1" : [outHandle] "=r" (returnHandle));

    *handle = returnHandle;
    return returnValue;
}

void thread_dispatch()
{
    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x13");

    // Generate interrupt
    __asm__ volatile ("ecall");
}