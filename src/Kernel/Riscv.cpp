#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/print.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/kernel_syscall.h"

#include "../../lib/console.h"

void Riscv::returnFromSystemCall()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap()
{
    switch (auto scause = Riscv::readScause())
    {
        // Here the software interrupt is used as a timer interrupt!
        case SCAUSE_SOFTWARE_INTERRUPT:
            handleSoftwareInterrupt();
            break;
        case SCAUSE_EXTERNAL_INTERRUPT:
            handleExternalInterrupt();
            break;
        case SCAUSE_ECALL_USER_MODE:
        case SCAUSE_ECALL_SUPERVISOR_MODE:
            handleEcall();
            break;
        default:
            handleUnknownTrapCause(scause);
            break;
    }

    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);
}

inline void Riscv::handleSoftwareInterrupt()
{
    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::running->m_timeSlice)
    {
        // Save important supervisor registers on the stack!
        auto volatile sepc = readSepc();
        auto volatile sstatus = readSstatus();

        TCB::timeSliceCounter = 0;
        TCB::dispatch();

        // Restore important supervisor registers
        writeSstatus(sstatus);
        writeSepc(sepc);
    }
}

inline void Riscv::handleExternalInterrupt()
{
    console_handler();
}

inline void Riscv::handleEcall()
{
    // Ecall will have a sepc that points back to ecall, so we want to return to the
    // instruction after that ecall
    constexpr auto EcallInstructionSize = 4;

    // Save important supervisor registers on the stack!
    auto volatile sepc = readSepc() + EcallInstructionSize;
    auto volatile sstatus = readSstatus();

    handleSystemCalls();

    // Restore important supervisor registers
    writeSstatus(sstatus);
    writeSepc(sepc);
}

inline void Riscv::handleUnknownTrapCause(uint64 scause)
{
    printString("\nscause: ");
    printInteger(scause);

    auto volatile sepc = readSepc();
    printString("\nsepc: ");
    printInteger(sepc);

    auto volatile stval = readStval();
    printString("\nstval: ");
    printInteger(stval);
}

inline void Riscv::handleSystemCalls()
{
    uint64 volatile sysCallCode;
    __asm__ volatile ("mv %[outCode], a0" : [outCode] "=r" (sysCallCode));

    switch (sysCallCode)
    {
        case SYS_CALL_MEM_ALLOC:
            handleMemAlloc();
            break;
        case SYS_CALL_MEM_FREE:
            handleMemFree();
            break;
        case SYS_CALL_THREAD_CREATE:
            handleThreadCreate();
            break;
        case SYS_CALL_THREAD_EXIT:
            handleThreadExit();
            break;
        case SYS_CALL_THREAD_DISPATCH:
            handleThreadDispatch();
            break;
        case SYS_CALL_THREAD_JOIN:
            handleThreadJoin();
            break;
    }
}

inline void Riscv::handleMemAlloc()
{
    // Get arguments
    size_t volatile sizeArg;
    __asm__ volatile ("mv %[outSize], a1" : [outSize] "=r" (sizeArg));

    auto volatile returnValue = kernel_alloc(sizeArg);

    // Store result in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

inline void Riscv::handleMemFree()
{
    void* volatile ptrArg;

    // Get arguments
    __asm__ volatile ("mv %[outPtr], a1" : [outPtr] "=r" (ptrArg));

    auto volatile returnValue = kernel_free(ptrArg);

    // Store result in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

inline void Riscv::handleThreadCreate()
{
    TCB** volatile handle;
    TCB::Body volatile routine;
    void* volatile args;
    void* volatile stack;

    // Get arguments
    __asm__ volatile ("mv %[inHandle], a1" : [inHandle] "=r" (handle));
    __asm__ volatile ("mv %[outRoutine], a2" : [outRoutine] "=r" (routine));
    __asm__ volatile ("mv %[outArgs], a3" : [outArgs] "=r" (args));
    __asm__ volatile ("mv %[outStack], a6" : [outStack] "=r" (stack));

    *handle = TCB::createThread(routine, args, stack);

    auto returnValue = (*handle == nullptr ? -1 : 0);

    // Store results in A0 and A1
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

inline void Riscv::handleThreadExit()
{
    auto returnValue = TCB::deleteThread(TCB::running);

    // Store result in a0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

inline void Riscv::handleThreadDispatch()
{
    TCB::timeSliceCounter = 0;
    TCB::dispatch();
}

inline void Riscv::handleThreadJoin()
{
    TCB* volatile handle;

    // Get arguments
    __asm__ volatile ("mv %[outHandle], a1" : [outHandle] "=r" (handle));

    TCB::running->waitForThread(handle);
}