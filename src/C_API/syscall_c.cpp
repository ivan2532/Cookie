/*
    C API wrapper to system calls

    In these functions we store the system call code and arguments in the
    corresponding registers and then switch to supervisor mode from which
    we call the internal kernel functions.
*/

#include "../../h/C_API/syscall_c.hpp"
#include "../../h/Kernel/MemoryAllocator.hpp"
#include "../../h/Kernel/Riscv.hpp"

uint64 systemCall(uint64 systemCallCode, ...)
{
    // Arguments will already be stored in a0, ..., an
    __asm__ volatile ("ecall");

    // Get the return value after ECALL
    uint64 volatile returnData;
    __asm__ volatile ("mv %[outReturn], a0" : [outReturn] "=r" (returnData));

    return returnData;
}

void* mem_alloc(size_t size) { return (void*)systemCall(0x01, size); }

int mem_free(void* ptr) { return (int)systemCall(0x02, ptr); }

int thread_create(thread_t* handle, void(*start_routine)(void*), void* arg)
{
    // Store arguments starting from A1
    __asm__ volatile ("mv a3, a2");
    __asm__ volatile ("mv a2, a1");

    // Save handle to A7, it will be overwritten by alloc
    __asm__ volatile ("mv a7, a0");

    // Allocate stack for thread, A1 will be overwritten here
    // Stack context extension has enough space for deepest nesting of kernel code
    void* stack = MemoryAllocator::alloc(DEFAULT_STACK_SIZE + STACK_CONTEXT_EXTENSION);
    if(stack == 0) return -1;

    // Restore handle from A7
    __asm__ volatile ("mv a1, a7");

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

int thread_exit() { return (int)systemCall(0x12); }

void thread_dispatch() { systemCall(0x13); }

void thread_join(thread_t handle) { systemCall(0x14, handle); }

int sem_open(sem_t* handle, unsigned init) { return (int)systemCall(0x21, handle, init); }

int sem_close(sem_t handle) { return (int)systemCall(0x22, handle); }

int sem_wait(sem_t id) { return (int)systemCall(0x23, id); }

int sem_signal(sem_t id) { return (int)systemCall(0x24, id); }

int time_sleep(time_t time) { return (int)systemCall(0x31, time); }

char getc() { return (char) systemCall(0x41); }

void putc(char output) { systemCall(0x42, output); }