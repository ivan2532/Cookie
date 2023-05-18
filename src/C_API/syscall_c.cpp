/*
    C API wrapper to system calls

    In these functions we store the system call code and arguments in the
    corresponding registers and then switch to supervisor mode from which
    we call the internal kernel functions.
*/

#include "../../h/C_API/syscall_c.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/kernel_allocator.h"

// Allocate a memory block of "size" bytes on the heap.
void* mem_alloc(size_t size)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, a0");

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
    __asm__ volatile ("mv a1, a0");

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
    // Store arguments starting from A1
    __asm__ volatile ("mv a3, a2");
    __asm__ volatile ("mv a2, a1");

    // Save handle to S1, it will be overwritten by kernel_alloc
    __asm__ volatile ("mv s1, a0");

    // TODO: Take into account kernel stack!
    // Allocate stack for thread, A1 will be overwritten here
    void* stack = kernel_alloc(DEFAULT_STACK_SIZE);
    if(stack == 0) return -1;

    // Restore handle from S1
    __asm__ volatile ("mv a1, s1");

    // Store stack argument to A6 (A4 will get overwritten by some local variable)
    __asm__ volatile ("mv a6, %[inStack]" : : [inStack] "r" (stack));

    // Store the system call code in register A0
    // This will invalidate the local stack variable, because mem_alloc returns value in A0
    __asm__ volatile ("li a0, 0x11");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    // Thread create should also start the new thread
    thread_dispatch();

    return returnValue;
}

int thread_exit()
{
    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x12");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

void thread_dispatch()
{
    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x13");

    // Generate interrupt
    __asm__ volatile ("ecall");
}

void thread_join(thread_t handle)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, a0");

    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x14");

    // Generate interrupt
    __asm__ volatile ("ecall");
}

int sem_open(sem_t* handle, unsigned init)
{
    // Move arguments so they start from A1 instead of A0
    __asm__ volatile ("mv a2, a1");
    __asm__ volatile ("mv a1, a0");

    // Store the system call code in register A0
    __asm__ volatile ("li a0, 0x21");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

int sem_close(sem_t handle)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, a0");

    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x22");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

int sem_wait(sem_t id)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, a0");

    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x23");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}

int sem_signal(sem_t id)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a1, a0");

    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x24");

    // Generate interrupt
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    int volatile returnValue;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnValue));

    return returnValue;
}