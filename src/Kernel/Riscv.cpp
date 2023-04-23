#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/print.hpp"
#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/kernel_syscall.h"

#include "../../lib/console.h"

void Riscv::popSppSpie()
{
    __asm__ volatile ("csrw sepc, ra");
    __asm__ volatile ("sret");
}

void Riscv::handleSupervisorTrap()
{
    switch (auto scause = Riscv::r_scause())
    {
        case SCAUSE_SOFTWARE_INTERRUPT:
            handleSoftwareInterrupt();
            break;
        case SCAUSE_EXTERNAL_INTERRUPT:
            handleExternalInterrupt();
            break;
        case SCAUSE_ECALL_FROM_USER_MODE:
        case SCAUSE_ECALL_FROM_SUPERVISOR_MODE:
            handleEcall();
            break;
        default:
            handleUnknownTrapCause(scause);
            break;
    }

    // Clear interrupt pending bit
    mc_sip(SIP_SSIP);
}

inline void Riscv::handleSoftwareInterrupt()
{
    TCB::timeSliceCounter++;
    if(TCB::timeSliceCounter >= TCB::running->m_timeSlice)
    {
        // Save important supervisor registers on the stack!
        auto sepc = r_sepc();
        auto sstatus = r_sstatus();

        TCB::timeSliceCounter = 0;
        TCB::dispatch();

        // Restore important supervisor registers
        w_sepc(sepc);
        w_sstatus(sstatus);
    }
}

inline void Riscv::handleExternalInterrupt()
{
    console_handler();
}

inline void Riscv::handleEcall()
{
    // Save important supervisor registers on the stack!
    // Ecall will have a sepc that points back to ecall, so we want to return to the
    // instruction after that ecall
    constexpr auto EcallInstructionSize = 4;
    auto sepc = r_sepc() + EcallInstructionSize;
    auto sstatus = r_sstatus();

    uint64 sysCallCode;
    __asm__ volatile ("mv %[outCode], a0" : [outCode] "=r" (sysCallCode));

    switch (sysCallCode)
    {
        case 0x01: // mem_alloc
        {
            // Get the size argument
            size_t sizeArg;
            __asm__ volatile ("mv %[outSize], a1" : [outSize] "=r" (sizeArg));

            void* returnValue = kernel_alloc(sizeArg);

            // Store result in A0
            __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

            break;
        }
        case 0x02: // mem_free
        {
            // Get the ptr argument
            void* ptrArg;
            __asm__ volatile ("mv %[outPtr], a1" : [outPtr] "=r" (ptrArg));

            int returnValue = kernel_free(ptrArg);

            // Store result in A0
            __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

            break;
        }
        case 0x13: // thread_dispatch / yield
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();

            break;
        }
    }

    // Restore important supervisor registers
    w_sepc(sepc);
    w_sstatus(sstatus);
}

inline void Riscv::handleUnknownTrapCause(uint64 scause)
{
    printString("\nscause: ");
    printInteger(scause);

    auto sepc = r_sepc();
    printString("\nsepc: ");
    printInteger(sepc);

    auto stval = r_stval();
    printString("\nstval: ");
    printInteger(stval);
}

inline void Riscv::handleSystemCalls()
{
    uint64 sysCallCode;
    __asm__ volatile ("mv %[outCode], a0" : [outCode] "=r" (sysCallCode));

    switch (sysCallCode)
    {
        case 0x01: // mem_alloc
        {
            // Get the size argument
            size_t sizeArg;
            __asm__ volatile ("mv %[outSize], a1" : [outSize] "=r" (sizeArg));

            void* returnValue = kernel_alloc(sizeArg);

            // Store result in A0
            __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

            break;
        }
        case 0x02: // mem_free
        {
            // Get the ptr argument
            void* ptrArg;
            __asm__ volatile ("mv %[outPtr], a1" : [outPtr] "=r" (ptrArg));

            int returnValue = kernel_free(ptrArg);

            // Store result in A0
            __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

            break;
        }
        case 0x13: // thread_dispatch / yield
        {
            TCB::timeSliceCounter = 0;
            TCB::dispatch();

            break;
        }
    }
}