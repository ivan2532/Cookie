#include "../../h/Kernel/kernel_syscall.h"
#include "../../lib/console.h"

// Refrence to the assembly function to set as the trap
void supervisorTrap();

// Moves SEPC after ECALL system call, so we don't get stuck in an
// infinite loop calling ECALL after we've processed an interrupt
// ---------------------------------------------------------------
// IMPORTANT: This function uses register A1 to increment SEPC
// so anything inside of A1 will be overwritten!
// ---------------------------------------------------------------
static void moveSEPCAfterECALL()
{
    // Read SEPC into A1
    __asm__ volatile ("csrr a1, sepc");

    // Increment A1 by 4 bytes (size of ECALL function)
    __asm__ volatile ("addi a1, a1, 4");

    //Write A1 to SEPC
    __asm__ volatile ("csrw sepc, a1");
}

// C function that we call from the assembly trap
void handleSupervisorTrap()
{
    // Read SCAUSE register value
    uint64 scauseResult;
    __asm__ volatile ("csrr %[outScause], scause" : [outScause] "=r" (scauseResult));

    // Check interrupt type

    // TODO: Why does it get called from supervisor regime?
    // ECALL from user or supervisor regime
    if(scauseResult == 0x08 || scauseResult == 0x09)
    {
        // Get the system call code
        uint64 sysCallCode;
        __asm__ volatile ("mv %[outCode], a0" : [outCode] "=r" (sysCallCode));

        // Handle different system calls
        switch (sysCallCode)
        {
            case 0x01: // __mem_alloc
            {
                // Get the size argument
                size_t sizeArg;
                __asm__ volatile ("mv %[outSize], a1" : [outSize] "=r" (sizeArg));

                void* returnValue = kernel_alloc(sizeArg);

                // Store result in A0
                __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

                break;
            }
            case 0x02: // __mem_free
            {
                // Get the ptr argument
                void* ptrArg;
                __asm__ volatile ("mv %[outPtr], a1" : [outPtr] "=r" (ptrArg));

                int returnValue = kernel_free(ptrArg);

                // Store result in A0
                __asm__ volatile ("mv a0, %[inReturnValue]" : : [inReturnValue] "r" (returnValue));

                break;
            }
        }

        // Clear first bit of SIP (Supervisor Interrupt Pending) register
        __asm__ volatile ("csrc sip, 0x02");

        // Move SEPC because ECALL was called
        moveSEPCAfterECALL();
    }

    // Timer interrupt
    else if(scauseResult == (0x01UL << 63 | 0x01))
    {
        // Clear first bit of SIP (Supervisor Interrupt Pending) register
        __asm__ volatile ("csrc sip, 0x02");
    }

    // Console interrupt routine
    console_handler();
}

// Sets the trap on the processor
void setTrap()
{
    // Set STVEC (Supervisor Trap Vector) to our supervisor trap function
    __asm__ volatile ("csrw stvec, %[vector]" : : [vector] "r" (&supervisorTrap));

    // Set SIE (Supervisor Interrupt Enable) bit to 1, so we get interrupt requests
    __asm__ volatile ("csrs sstatus, 0x02"); // 0x02 = 0b00000010, SIE is the 1st bit in sstatus

    // We're back in user mode from here
}