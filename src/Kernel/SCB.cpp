#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

SCB::SCB(unsigned startValue, bool binary)
    :
    m_Value((int)startValue),
    m_Binary(binary)
{
}

SCB::~SCB()
{
    auto current = m_BlockedQueue.removeFirst();
    while(current != nullptr)
    {
        Scheduler::put(current);
        current = m_BlockedQueue.removeFirst();
    }
}

void SCB::wait()
{
    //Riscv::lock();
    if(--m_Value < 0) block();
    //Riscv::unlock();
}

void SCB::signal()
{
    //Riscv::lock();
    if(m_Value++ < 0) unblock();
    if(m_Binary && m_Value > 1) m_Value = 1;
    //Riscv::unlock();
}

void SCB::block()
{
    m_BlockedQueue.addLast(TCB::running);
    TCB::running->m_PutInScheduler = false;
    thread_dispatch();
}

void SCB::unblock()
{
    auto threadToUnblock = m_BlockedQueue.removeFirst();
    Scheduler::put(threadToUnblock);
}
