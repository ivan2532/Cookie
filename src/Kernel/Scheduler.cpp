#include "../../h/Kernel/Scheduler.hpp"

List<TCB> Scheduler::threadQueue;

TCB *Scheduler::get()
{
    return threadQueue.removeFirst();
}

void Scheduler::put(TCB *ccb)
{
    threadQueue.addLast(ccb);
}