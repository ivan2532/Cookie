#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

void SCB::wait()
{
    Riscv::lock();

    if(--value < 0) block();

    Riscv::unlock();
}

void SCB::signal()
{
    Riscv::lock();

    if(value++ < 0) unblock();

    Riscv::unlock();
}

SCB::~SCB()
{
    auto current = blockedQueue.removeFirst(true);
    while(current != nullptr)
    {
        Scheduler::put(current, false, true);
        current = blockedQueue.removeFirst(true);
    }
}

void SCB::block()
{
    blockedQueue.addLast(TCB::running, true);
    Riscv::asyncContextSwitch(false);
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst(true);
    Scheduler::put(threadToUnblock, true, true);
    Riscv::asyncContextSwitch();
}
