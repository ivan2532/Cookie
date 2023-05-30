#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/TCB.hpp"

#include "../../lib/console.h"
#include "../../h/Kernel/SCB.hpp"

List<char> Riscv::inputBuffer;
List<char> Riscv::outputBuffer;
char Riscv::outputCh = '0';

SCB* Riscv::inputSemaphore;
SCB* Riscv::outputSemaphore;

void Riscv::returnFromSystemCall()
{
    if(TCB::running != TCB::inputThread && TCB::running != TCB::outputThread)
    {
        maskClearSstatus(SSTATUS_SPP);
    }

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
            Scheduler::put(it->data, false);
        }
    }

    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::running->m_TimeSlice)
    {
        contextSwitch();
    }
}

void Riscv::handleExternalTrap()
{
    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);
    console_handler();
    return;

    auto interruptId = plic_claim();

    // Check if the console generated an interrupt
    if(interruptId == CONSOLE_IRQ)
    {
        auto pStatus = (char*)CONSOLE_STATUS;
        auto pInData = (char*)CONSOLE_RX_DATA;

        // Read from the controller
        if(((*pStatus) & CONSOLE_RX_STATUS_BIT) != 0)
        {
            // Registered pressed char
            inputBuffer.addLast(new char(*pInData), true);
            *pStatus = ( (*pStatus) & (~CONSOLE_RX_STATUS_BIT) );
        }
    }

    plic_complete(interruptId);
}

void Riscv::handleEcallTrap()
{
    // Clear interrupt pending bit
    maskClearSip(SIP_SSIP);

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
    printInt(scause);

    auto volatile sepc = readSepc();
    printString("\nsepc: ");
    printInt(sepc);

    auto volatile stval = readStval();
    printString("\nstval: ");
    printInt(stval);

    while(true);
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
        case SYS_CALL_TIME_SLEEP:
            handleTimeSleep();
            break;
        case SYS_CALL_GETC:
            handleGetChar();
            break;
        case SYS_CALL_PUTC:
            handlePutChar();
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

void Riscv::handleTimeSleep()
{
    time_t volatile time;

    // Get arguments
    __asm__ volatile ("mv %[outTime], a1" : [outTime] "=r" (time));

    auto returnValue = TCB::sleep(time);

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handleGetChar()
{
    auto returnValue = __getc();

    // Store results in A0
    __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));
}

void Riscv::handlePutChar()
{
    char volatile outputChar;

    // Get arguments
    __asm__ volatile ("mv %[outChar], a1" : [outChar] "=r" (outputChar));

    outputBuffer.addLast(new char(outputChar), true);

    outputSemaphore->signal();
}