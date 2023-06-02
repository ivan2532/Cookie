#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

void SCB::wait()
{
    Riscv::lock();
    if(--m_Value < 0) block();
    Riscv::unlock();
}

void SCB::signal()
{
    Riscv::lock();
    if(m_Value++ < 0) unblock();
    Riscv::unlock();
}

SCB::~SCB()
{
    auto current = blockedQueue.removeFirst();
    while(current != nullptr)
    {
        Scheduler::put(current, false);
        current = blockedQueue.removeFirst();
    }
}

void SCB::block()
{
    blockedQueue.addLast(TCB::running);
    TCB::running->m_PutInScheduler = false;
    thread_dispatch();
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst();
    Scheduler::put(threadToUnblock, true);
    thread_dispatch();
}