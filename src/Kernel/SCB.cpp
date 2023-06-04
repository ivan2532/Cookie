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
    while(!m_BlockedQueue.isEmpty())
    {
        auto current = m_BlockedQueue.removeFirst();
        Scheduler::put(current);
    }
}

void SCB::wait()
{
    if(--m_Value < 0) block();
}

void SCB::signal()
{
    if(m_Value++ < 0) unblock();
    if(m_Binary && m_Value > 1) m_Value = 1;
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
