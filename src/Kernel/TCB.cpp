#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

void TCB::yield()
{
    // Store the system call code in register a0
    __asm__ volatile ("li a0, 0x13");
    // Generate an environment call exception
    __asm__ volatile ("ecall");
}

void TCB::bodyWrapper()
{
    // Here we are still in the handleSupervisorTrap()!
    // That means we are still in the supervisor regime!
    // TODO: Go back to user mode when needed.

    // The thread has is created and should start now, we are still in the supervisor regime
    // Continue program execution from here and return from the trap
    Riscv::popSppSpie();

    running->m_Body();
    running->m_Finished = true;

    // Once the thread has finished execution, give the processor to someone else
    TCB::yield();
}

void TCB::dispatch()
{
    auto old = running;

    if(!old->m_Finished) Scheduler::put(old);
    running = Scheduler::get();

    TCB::contextSwitch(&old->m_Context, &running->m_Context);
}

TCB *TCB::createThread(TCB::Body body)
{
    return new TCB(body, DEFAULT_TIME_SLICE);
}
