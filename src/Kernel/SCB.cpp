#include "../../h/Kernel/SCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

void SCB::wait()
{
    bool previousSIE = Riscv::readSstatus() & Riscv::SSTATUS_SIE;
    if(previousSIE) Riscv::maskClearSstatus(Riscv::SSTATUS_SIE);

    if(--m_Value < 0) block();

    if(previousSIE) Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);
}

void SCB::signal()
{
    bool previousSIE = Riscv::readSstatus() & Riscv::SSTATUS_SIE;
    if(previousSIE) Riscv::maskClearSstatus(Riscv::SSTATUS_SIE);

    if(m_Value++ < 0) unblock();

    if(previousSIE) Riscv::maskSetSstatus(Riscv::SSTATUS_SIE);
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