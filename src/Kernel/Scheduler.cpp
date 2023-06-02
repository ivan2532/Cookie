#include "../../h/Kernel/Scheduler.hpp"

KernelDeque<TCB*> Scheduler::threadQueue;

TCB *Scheduler::get()
{
    return threadQueue.removeFirst();
}

void Scheduler::put(TCB* handle, bool putAtFrontOfQueue)
{
    if(putAtFrontOfQueue) threadQueue.addFirst(handle);
    else threadQueue.addLast(handle);
}

bool Scheduler::contains(TCB *handle)
{
    return threadQueue.contains(handle);
}

bool Scheduler::isEmpty() {
    return threadQueue.isEmpty();
}
