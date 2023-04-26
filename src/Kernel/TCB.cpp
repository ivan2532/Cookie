#include "../../h/Kernel/TCB.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/C_API/syscall_c.hpp"

TCB* TCB::running = nullptr;
uint64 TCB::timeSliceCounter = 0;

void TCB::bodyWrapper()
{
    // The thread is created and should start now, we are still in the supervisor regime
    // Continue program execution from here and return from the trap
    Riscv::popSppSpie();

    // Here we are still in the handleSupervisorTrap()!
    // That means we are still in the supervisor regime!
    // TODO: Go back to user mode if needed (check SPP)!

    running->m_Body(running->m_Args);
    running->m_Finished = true;

    // Once the thread has finished execution, give the processor to someone else
    thread_dispatch();
}

void TCB::dispatch()
{
    auto old = running;

    if(!old->m_Finished)
    {
        Scheduler::put(old);
    }

    running = Scheduler::get();

    TCB::contextSwitch(&old->m_Context, &running->m_Context);
}

TCB *TCB::createThread(TCB::Body body, void* args, void* stack)
{
    auto result = new TCB(body, args, DEFAULT_TIME_SLICE, (uint64*)stack);
    if(body == nullptr) running = result;
    return result;
}
