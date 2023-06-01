#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

void SCB::wait()
{
    if(--m_Value < 0) block();
}

void SCB::signal()
{
    if(m_Value++ < 0) unblock();
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
    Riscv::contextSwitch(false);
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst();
    Scheduler::put(threadToUnblock, true);
    Riscv::contextSwitch();
}