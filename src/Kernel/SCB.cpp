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
    auto current = blockedQueue.removeFirst();
    while(current != nullptr)
    {
        Scheduler::put(current);
        current = blockedQueue.removeFirst();
    }
}

void SCB::block()
{
    blockedQueue.addLast(TCB::running, true);

    Riscv::unlock();
    TCB::dispatch(false);
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst();
    Scheduler::put(threadToUnblock, true);

    Riscv::unlock();
    TCB::dispatch();
}
