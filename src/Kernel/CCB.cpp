#include "../../h/Kernel/CCB.hpp"
#include "../../h/Kernel/Riscv.hpp"

CCB* CCB::running = nullptr;

void CCB::yield()
{
    // Push regsiters to CURRENT coroutine stack
    Riscv::pushRegisters();

    // Put old coroutine in scheduler, get new coroutine from scheduler,
    // and switch coroutine context (ra and sp)
    CCB::dispatch();

    // Pop regsiters from NEW coroutine stack
    Riscv::popRegisters();
}

void CCB::dispatch()
{
    auto old = running;

    if(!(old->m_Finished))
    {
        Scheduler::put(old);
    }
    running = Scheduler::get();

    CCB::contextSwitch(&old->m_Context, &running->m_Context);
}

CCB *CCB::createCoroutine(CCB::Body body)
{
    return new CCB(body);
}
