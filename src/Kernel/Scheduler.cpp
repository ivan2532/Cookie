#include "../../h/Kernel/Scheduler.hpp"

List<TCB> Scheduler::threadQueue;

TCB *Scheduler::get()
{
    return threadQueue.removeFirst();
}

void Scheduler::put(TCB *handle, bool putAtFrontOfQueue)
{
    if(putAtFrontOfQueue) threadQueue.addFirst(handle);
    else threadQueue.addLast(handle);
}