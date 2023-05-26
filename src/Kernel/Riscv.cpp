#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/print.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/kernel_allocator.h"
#include "../../h/C++_API/syscall_cpp.hpp"

#include "../../lib/console.h"
#include "../../h/Kernel/SCB.hpp"

volatile bool Riscv::kernelLock = false;
bool Riscv::dispatchOnUnlock = false;

void Riscv::lock()
{
    kernelLock = true;
}

void Riscv::unlock()
{
    kernelLock = false;
    if(dispatchOnUnlock) contextSwitch();
}

void Riscv::returnFromSystemCall()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::contextSwitch(bool putOldThreadInSchedule)
{
    // Save important supervisor registers on the stack!
    auto volatile sepc = readSepc();
    auto volatile sstatus = readSstatus();

    TCB::timeSliceCounter = 0;
    TCB::dispatch(putOldThreadInSchedule);

    // Restore important supervisor registers
    writeSstatus(sstatus);
    writeSepc(sepc);
}

void Riscv::handleTimerTrap()
{
    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);

    if(TCB::running == nullptr) return;

    for(auto it = TCB::allThreads.head; it != nullptr; it = it->next)
    {
        if(it->data->m_SleepCounter == 0) continue;

        if(--(it->data->m_SleepCounter) == 0 && !Scheduler::contains(it->data))
        {
            Scheduler::put(it->data, false, true);
        }
    }

    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::running->m_TimeSlice)
    {
        if(kernelLock)
        {
            dispatchOnUnlock = true;
            return;
        }

        contextSwitch();
    }
}

void Riscv::handleExternalTrap()
{
    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);

    console_handler();
}

void Riscv::handleEcallTrap()
{
    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);

    if(kernelLock) return;

    // Save A4 to A7, it will be overwritten
    __asm__ volatile ("mv a7, a4");

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

void Riscv::handleUnknownTrapCause(uint64 scause)
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

void Riscv::handleSystemCalls()
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
        case SYS_CALL_SEM_OPEN:
            handleSemaphoreOpen();
            break;
        case SYS_CALL_SEM_CLOSE:
            handleSemaphoreClose();
            break;
        case SYS_CALL_SEM_WAIT:
            handleSemaphoreWait();
            break;
        case SYS_CALL_SEM_SIGNAL:
            handleSemaphoreSignal();
            break;
        default:
            handleUnknownTrapCause(readScause());
    }
}

void Riscv::handleMemAlloc()
{
    // Get arguments
    size_t volatile sizeArg;
    __asm__ volatile ("mv %[outSize], a1" : [outSize] "=r" (sizeArg));

    auto volatile returnValue = kernel_alloc(sizeArg);

    // Store result in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleMemFree()
{
    void* volatile ptrArg;

    // Get arguments
    __asm__ volatile ("mv %[outPtr], a1" : [outPtr] "=r" (ptrArg));

    auto volatile returnValue = kernel_free(ptrArg);

    // Store result in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleThreadCreate()
{
    TCB::Body volatile routine;
    void* volatile args;
    void* volatile stack;

    // Save A1 (handle) to A7, A1 will be overwritten by createThread
    __asm__ volatile ("mv a7, a1");

    // Get other arguments
    __asm__ volatile ("mv %[outRoutine], a2" : [outRoutine] "=r" (routine));
    __asm__ volatile ("mv %[outArgs], a3" : [outArgs] "=r" (args));
    __asm__ volatile ("mv %[outStack], a6" : [outStack] "=r" (stack));

    auto newTCB = TCB::createThread(routine, args, stack);

    // Get handle
    TCB** volatile handle;
    __asm__ volatile ("mv %[inHandle], a7" : [inHandle] "=r" (handle));

    *handle = newTCB;
    auto returnValue = (*handle == nullptr ? -1 : 0);

    // Store results in A0 and A1
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleThreadExit()
{
    auto returnValue = TCB::deleteThread(TCB::running);

    // Store result in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleThreadDispatch()
{
    Riscv::contextSwitch();
}

void Riscv::handleThreadJoin()
{
    TCB* volatile handle;

    // Get arguments
    __asm__ volatile ("mv %[outHandle], a1" : [outHandle] "=r" (handle));

    TCB::running->waitForThread(handle);
}

void Riscv::handleSemaphoreOpen()
{
    // Save handle to A7, it will be overwritten by kernel_alloc
    __asm__ volatile ("mv a7, a1");

    auto newSCB = static_cast<SCB*>(kernel_alloc(sizeof(SCB)));

    SCB** volatile handle;
    unsigned volatile init;

    // Get arguments
    __asm__ volatile ("mv %[outHandle], a7" : [outHandle] "=r" (handle));
    __asm__ volatile ("mv %[outInit], a2" : [outInit] "=r" (init));

    *handle = new (newSCB) SCB(init);

    auto returnValue = (*handle == nullptr ? -1 : 0);

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleSemaphoreClose()
{
    // Move handle to A7, it can be overwritten by signal()
    __asm__ volatile ("mv a7, a1");

    SCB* volatile handle;

    // Get arguments
    __asm__ volatile ("mv %[outHandle], a7" : [outHandle] "=r" (handle));

    auto returnValue = kernel_free(handle);

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleSemaphoreWait()
{
    // Move id to A7, it can be overwritten by signal()
    __asm__ volatile ("mv a7, a1");

    SCB* volatile id;

    // Get arguments
    __asm__ volatile ("mv %[outId], a7" : [outId] "=r" (id));

    id->wait();
    auto returnValue = 0;

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleSemaphoreSignal()
{
    // Move id to A7, it can be overwritten by signal()
    __asm__ volatile ("mv a7, a1");

    SCB* volatile id;

    // Get arguments
    __asm__ volatile ("mv %[outId], a7" : [outId] "=r" (id));

    id->signal();
    auto returnValue = 0;

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}
