#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"
#include "../../h/Kernel/print.hpp"

void SCB::wait()
{
    lock();

    if(--value < 0) block();

    unlock();
}

void SCB::signal()
{
    lock();

    if(value++ < 0) unblock();

    unlock();
}

SCB::~SCB()
{
    lock();

    auto current = blockedQueue.removeFirst();
    while(current != nullptr)
    {
        Scheduler::put(current);
        current = blockedQueue.removeFirst();
    }

    unlock();
}

void SCB::lock()
{

}

void SCB::unlock()
{

}

void SCB::block()
{
    blockedQueue.addLast(TCB::running);

    unlock();
    TCB::dispatch(false);
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst();
    Scheduler::put(threadToUnblock, true);

    unlock();
    TCB::dispatch();
}
