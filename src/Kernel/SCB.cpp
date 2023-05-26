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
    auto current = blockedQueue.removeFirst(true);
    while(current != nullptr)
    {
        Scheduler::put(current, false);
        current = blockedQueue.removeFirst(true);
    }
}

void SCB::block()
{
    blockedQueue.addLast(TCB::running, true);
    Riscv::contextSwitch(false);
}

void SCB::unblock()
{
    auto threadToUnblock = blockedQueue.removeFirst(true);
    Scheduler::put(threadToUnblock, true);
    Riscv::contextSwitch();
}

void SCB::setValue(int value)
{
    m_Value = value;
}
